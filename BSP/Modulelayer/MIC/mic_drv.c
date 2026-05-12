
// 需要先测量环境噪声值，调整最大值MIC_MAX_NOSIE_FLOOR_VALUE 目的是防止未校准之前就播放音乐，干扰校准结果
// 测量音乐的最大值

/*
┌──────────────┐    ┌──────────────────┐    ┌──────────────────┐
│ DMA 缓冲区    │ →  │ mic_dma_buf_to_  │ →  │ mic_Calibrate    │
│ (uint32_t[256])│    │ float()        │    │ (首次调用校准)     │
└──────────────┘    └──────────────────┘    └──────────────────┘
                              ↓
                    ┌──────────────────┐
                    │ adc_dma_buf_float│ (float[256], 去直流 - 2048)
                    └──────────────────┘
                              ↓
                    ┌──────────────────┐    ┌──────────────────┐
                    │ mic_goertzel()   │ →  │ mic_GetMaxfreq() │
                    │ (16个频点检测)     │    │ (取最大能量频点)   │
                    └──────────────────┘    └──────────────────┘
                              ↓
                    ┌──────────────────┐
                    │ mic_freq_filter  │ (动态低通平滑频率)
                    └──────────────────┘
                              ↓
                    ┌──────────────────┐    ┌──────────────────┐
                    │ mic_Getloudness  │ →  │ loudness_mapto_  │
                    │                  |    │brightness()      │ 
                    └──────────────────┘    └──────────────────┘
*/

#include "mic_drv.h"

static mic_t mic;

// 枚举频率点，减少计算量
static const goertzel_coeff_t goertzel_table[GOERTZEL_FREQ_NUM] =
{
    // target_freq,  k,  coeff
    {   50,    1,   1.9993976f  },  // sub-bass  
    {   80,    2,   1.9975909f  },  // bass     
    {  120,    3,   1.9945766f  },  // bass     
    {  180,    5,   1.9849591f  },  // bass      
    {  260,    7,   1.9705553f  },  // low-mid   
    {  380,   10,   1.9396926f  },  // mid-low   
    {  530,   14,   1.8830881f  },  // mid      
    {  720,   18,   1.8079789f  },  // mid       
    {  950,   24,   1.6629392f  },  // mid-high  
    { 1250,   32,   1.4142136f  },  // upper-mid 
    { 1650,   42,   1.0282055f  },  // presence  
    { 2100,   54,   0.4859606f  },  // high-mid 
    { 2700,   69,  -0.2410734f  },  // high     
    { 3400,   87,  -1.0745992f  },  // high-pres 
    { 4200,  108,  -1.7526134f  },  // brilliance 
    { 4800,  123,  -1.9849591f  },  // air     
};



static void mic_ClearBuf(void)
{
    mic.work_flag = 0;
    mic.dma_data_ready_flag = 0;

    mic.freq = 0.0f;
    mic.loudness = 0.0f;

    memset(mic.adc_dma_buf, 0, sizeof(mic.adc_dma_buf));
    memset(mic.adc_dma_buf_float, 0, sizeof(mic.adc_dma_buf_float));
}

void mic_Init(void)
{
    mic_PowerOn();
    HAL_Delay(200); // DMA完成数据转换

    mic_Calibrate();  // 初始化阶段完成噪声校准

    mic_PowerOff();
}

 // 定时器采用TIM3溢出触发ADC转换，相对于直接触发ADC转换，可以减少ADC转换的次数，降低转换频率，节省CPU。
void mic_PowerOn(void)
{
    if (mic.work_flag) return;

    mic_ClearBuf();

    if (HAL_TIM_Base_Start(&htim3) != HAL_OK) Error_Handler(); // 开启定时器3，用于触发ADC转换（TRGO）
    if (HAL_ADC_Start_DMA(&MIC_ADC_CHANNEL,mic.adc_dma_buf,MIC_ADC_DMA_BUF_LEN) != HAL_OK) 
        Error_Handler();

    mic.work_flag = 1;  
}

void mic_PowerOff(void)
{
    if (!mic.work_flag) return;

    mic.work_flag = 0;
    mic.dma_data_ready_flag = 0;

    if (HAL_TIM_Base_Stop(&htim3) != HAL_OK) Error_Handler(); // 关闭定时器3
    if (HAL_ADC_Stop_DMA(&MIC_ADC_CHANNEL) != HAL_OK) Error_Handler();
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if (hadc->Instance == MIC_ADC_CHANNEL.Instance)
    {
        mic.dma_data_ready_flag = 1; 
    }
}

void mic_GetFreq(float *freq)
{
    if (freq == NULL) return;

    *freq = mic.freq;
}

 /**
  * @brief 将DMA缓冲区的uint16_t数据转换为float数据，含去直流偏置（-2048）处理
  */
static void mic_dma_buf_to_float(void)
{
    for (uint16_t i = 0; i < MIC_ADC_DMA_BUF_LEN; i++)
    {
        mic.adc_dma_buf_float[i] = (float)(mic.adc_dma_buf[i] - 2048);
    }
}

/**
 * @brief 校准ADC 获取噪声的响度值
 * @note: 调用开始函数自动校准
 */
void mic_Calibrate(void)
{
    mic_dma_buf_to_float();

    float energy = 0.0f;

    for (uint16_t i = 0; i < MIC_ADC_DMA_BUF_LEN; i++)
    {
        float s = mic.adc_dma_buf_float[i];
        energy += s * s;
    }
    mic.noise_floor = sqrtf(energy / (float)MIC_ADC_DMA_BUF_LEN);
}

// 单频检测,goertzel算法，返回声波中一个频率点的能量值，能量值越大，频率点越接近目标频率
static float mic_goertzel(uint8_t index)
{
    float coeff = goertzel_table[index].coeff;
    float q0 = 0, q1 = 0, q2 = 0;
	
	for(uint16_t i = 0;i < MIC_ADC_DMA_BUF_LEN;i++)
	{
		q0 = coeff * q1 - q2 + mic.adc_dma_buf_float[i];
		q2 = q1;
		q1 = q0;
	}
	
	return q1*q1 + q2*q2 - coeff *q1*q2;
}

/*
 * @brief 获取输出结果中的最大频率,返回最大频率
 */
static float mic_GetMaxfreq()
{
    float max_power = 0;
	float best_freq = 0;

    for(uint8_t i = 0;i < GOERTZEL_FREQ_NUM;i++)
    {
		float power = mic_goertzel(i);

        if (power < 0.0f) power = 0.0f;
        if (power > max_power)
        {
            max_power = power;
            best_freq = goertzel_table[i].target_freq;
        }
    }
    return best_freq;  // 最匹配的频率点
}

/*
 * @brief 获取频域信号响度（RMS），去除环境噪声
 */
static void mic_Getloudness(void)
{
    float energy = 0.0f;

    for (uint16_t i = 0; i < MIC_ADC_DMA_BUF_LEN; i++)
    {
        float s = mic.adc_dma_buf_float[i];
        energy += s * s;
    }

    float rms = sqrtf(energy / (float)MIC_ADC_DMA_BUF_LEN);

    mic.loudness = (rms > mic.noise_floor) ? (rms - mic.noise_floor) : 0.0f;  // 去除环境噪声的响度，防止为负
}


/**
 * @brief 映射响度到亮度 
 * @param brightness_max 亮度的最大值 设置为255，便于后面计算
 * @param brightness_min 亮度的最小值 建议设置为25 , 亮度不要过低
 * @param low_bright_area 过低亮度下的映射区域，建议设置为77
 * @note: 在过低亮度下，非线性增加，即提升暗部的亮度，其余就直接线性映射
 */
void mic_loudness_mapto_brightness(uint8_t brightness_max,uint8_t brightness_min,\
                                    uint8_t* brightness,uint8_t low_bright_area)
{
    if(brightness == NULL) return;

    float raw = mic.loudness * brightness_max / MIC_LOUDNESS_MAX ;

    // 上下钳位
    if (raw > brightness_max)               raw = brightness_max;
    else if (raw < brightness_min)          raw = brightness_min;

    // 暗区内非线性映射，提升暗部表现
    if (raw < low_bright_area)
    {
        float range = low_bright_area - brightness_min;
        float t = (raw - brightness_min) / range;    // 归一化
        t = 1.0f - (1.0f - t) * (1.0f - t); // 非线性映射
        raw = brightness_min + t * range;
    }

    *brightness = (uint8_t)raw;
}


// 动态低通滤波
static void mic_freq_filter(float* cnt_freq)
{
    if (cnt_freq == NULL) return;

    float delta_freq = *cnt_freq - mic.freq;

    if (fabsf(delta_freq) < 10.0f) // 10hz内变换，缓慢一点
    {
        mic.freq = NARROW_FILTER * mic.freq + (1 - NARROW_FILTER) * *cnt_freq;
    }
    else if (delta_freq < 0.0f) // 下降
    {
        mic.freq = DOWN_DIRETION_FILTER * mic.freq + (1 - DOWN_DIRETION_FILTER) * *cnt_freq;
    }
    else if (delta_freq <= 100.0f) // 中速升
    {
        mic.freq = MID_SPEED_FILTER * mic.freq + (1 - MID_SPEED_FILTER) * *cnt_freq;
    }
    else// 快速升
    {
        mic.freq = HIGH_SPEED_FITLER * mic.freq + (1 - HIGH_SPEED_FITLER) * *cnt_freq;
    }
}

void mic_Run()
{
    if (!mic.dma_data_ready_flag) return; // 等待DMA数据完成

    mic.dma_data_ready_flag = 0; // 清除标志位

    // 转换为float数据
    mic_dma_buf_to_float();

    float cnt_freq = mic_GetMaxfreq(); // 获取最大频率
    mic_freq_filter(&cnt_freq);

    mic_Getloudness(); // 获取响度    
    
}
