
// 需要先测量环境噪声值，调整最大值MIC_MAX_NOSIE_FLOOR_VALUE 目的是防止未校准之前就播放音乐，干扰校准结果
// 测量音乐的最大值


#include "mic_drv.h"

static mic_t mic ;

// 枚举频点
static const float goertzel_freq_list[GOERTZEL_FREQ_NUM] =
{
    80,150,300,600,1000,2000,3500,4500
};


void mic_ClearBuf(void)
{
    memset(mic.adc_dma_buf, 0, sizeof(mic.adc_dma_buf));
    memset(mic.adc_dma_buf_float, 0, sizeof(mic.adc_dma_buf_float));
    memset(mic.fft_Output, 0, sizeof(mic.fft_Output));

    mic.mic_work_flag = 0; 
    mic.noise_floor = 0;
    mic.calibrated_flag = 0;
    mic.loudness = 0;
    mic.freq = 0;
}

void mic_Init(void)
{
    mic_ClearBuf();
    mic_PowerOff();
}


/**
 * @brief 校准ADC 获取噪声的响度值
 * @note: 调用开始函数自动校准
 */
void mic_Calibrate(void)
{
    float energy = 0;

    for (uint8_t i = 0; i < 3 ;i++)
    {
        for (int i = 0;i < MIC_ADC_DMA_BUF_LEN;i++)
        {
            energy += mic.adc_dma_buf_float[i] * mic.adc_dma_buf_float[i]; // 平方
        }
    }

    energy = energy / 3 / MIC_ADC_DMA_BUF_LEN;

    mic.noise_floor = energy;
    mic.calibrated_flag = 1;
}

uint8_t mic_isCalibrate(void)
{
    return mic.calibrated_flag;
}
uint8_t mic_isWorking(void)
{
    return mic.mic_work_flag;
}


 // 定时器采用TIM3溢出触发ADC转换，相对于直接触发ADC转换，可以减少ADC转换的次数，降低转换频率，节省CPU。
void mic_PowerOn(void)
{
    mic_ClearBuf();

    HAL_TIM_Base_Start(&htim3); // 开启定时器3，用于触发ADC转换（TRGO）

    HAL_ADC_Start_DMA(&MIC_ADC_CHANNEL,mic.adc_dma_buf,MIC_ADC_DMA_BUF_LEN);

    mic.mic_work_flag = 1;

    HAL_Delay(100);
}

void mic_PowerOff(void)
{
    HAL_TIM_Base_Stop(&htim3); // 关闭定时器3

    HAL_ADC_Stop_DMA(&MIC_ADC_CHANNEL);
}


void mic_GetFreq(float *freq)
{
    if (freq == NULL) return;

    *freq = mic.freq;
}

 /**
  * @brief 将DMA缓冲区的uint16_t数据转换为float数据，用于FFT处理,含去直流偏置（-2048）处理
  */
void mic_dma_buf_to_float(void)
{
    for (uint16_t i = 0; i < MIC_ADC_DMA_BUF_LEN; i++)
    {
        mic.adc_dma_buf_float[i] = (float)(mic.adc_dma_buf[i] - 2048);
    }
}

// 单频检测
float mic_goertzel(float target_freq)
{
	float coeff;  // 递推系数
	float q0 = 0,q1 = 0,q2 = 0;  // 滤波器状态变量
	uint16_t k; // 频点编号
	
	k = (uint16_t)(0.5f + ((MIC_ADC_DMA_BUF_LEN * target_freq) / SAMPLE_RATE));
	coeff = 2.0f * cosf(2.0f * 3.1415926f * k / MIC_ADC_DMA_BUF_LEN);
	
	for(uint8_t i = 0;i < MIC_ADC_DMA_BUF_LEN;i++)
	{
		q0 = coeff * q1 - q2 + mic.adc_dma_buf_float[i];
		q2 = q1;
		q1 = q0;
	}
	
	return q1*q1 + q2*q2 - coeff *q1*q2;
	
}
/*
 * @brief 获取最大频率,返回最大频率
 */
float mic_GetMaxfreq()
{
    float max_power = 0;
	float best_freq = 0;
	
	for(uint8_t i = 0;i < GOERTZEL_FREQ_NUM;i++)
	{
		float power = mic_goertzel(goertzel_freq_list[i]);
		mic.fft_Output[i] = power;
		
		if (power > max_power)
		{
            max_power = power;
            best_freq = goertzel_freq_list[i];
        }
    }

    return best_freq;
}

/*
 * @brief 获取FFT结果中的响度
 */
void mic_Getloudness()
{
    float energy = 0;

    for (int i = 0;i < MIC_ADC_DMA_BUF_LEN;i++)
    {
        energy += mic.adc_dma_buf_float[i] * mic.adc_dma_buf_float[i]; // 平方
    }

    mic.loudness = sqrt(energy / MIC_ADC_DMA_BUF_LEN) - mic.noise_floor;  // 去除环境噪声的响度偏移值
}


/**
 * @brief 映射响度到亮度 
 * @param brightness_max 亮度的最大值 设置为255，便于后面计算
 * @param brightness_min 亮度的最小值 建议设置为25 , 亮度不要过低
 * @param low_bright_area 过低亮度下的映射区域，建议设置为77
 */
void mic_loudness_mapto_brightness(uint8_t brightness_max,uint8_t brightness_min,uint8_t* brightness,uint8_t low_bright_area)
{
    if(brightness == NULL) return;

    *brightness = (uint8_t)((uint32_t)mic.loudness * brightness_max / MIC_LOUDNESS_MAX);

    if (*brightness > brightness_max) *brightness = brightness_max;
    else if (*brightness < brightness_min) *brightness = brightness_min;

    // 在过低亮度下，线性增加，即提升暗部的亮度，其余就直接映射
    else if (*brightness < low_bright_area) *brightness = ((*brightness) * 3 > low_bright_area) ? low_bright_area : (*brightness) * 3;
}


// 动态低通滤波
void mic_freq_filter(float* cnt_freq)
{
    if (cnt_freq == NULL) return;

    float delta_freq = *cnt_freq - mic.freq;

    if (fabs(delta_freq) < 10.0f) // 10hz内变换，缓慢一点
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
    else //快速升
    {
        mic.freq = HIGH_SPEED_FITLER * mic.freq + (1 - HIGH_SPEED_FITLER) * *cnt_freq;
    }

}

 void mic_Run()
 {
    // 1. 转换为float数据
    mic_dma_buf_to_float();

    if (!mic_isCalibrate()) // 校准环境噪声
    {
        mic_Calibrate();
    }
    else
    {
        // 3. 获取最大频率
        float cnt_freq = mic_GetMaxfreq();
        mic_freq_filter(&cnt_freq);

        // 4. 获取响度
        mic_Getloudness();

        HAL_Delay(5);
    }
}
