
// 需要先测量环境噪声值，调整最大值MIC_MAX_NOSIE_FLOOR_VALUE 目的是防止未校准之前就播放音乐，干扰校准结果
// 测量音乐的最大值


#include "mic_drv.h"

static mic_t mic ;
static arm_rfft_fast_instance_f32 fft_inst; //FFT 实例


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
    arm_rfft_fast_init_f32(&fft_inst, MIC_ADC_DMA_BUF_LEN); // 初始化
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

    HAL_ADC_DMA_Start(&MIC_ADC_CHANNEL);

    mic.mic_work_flag = 1;

    HAL_Delay(100);
}

void mic_PowerOff(void)
{
    HAL_TIM_Base_Stop(&htim3); // 关闭定时器3

    HAL_ADC_DMA_Stop(&MIC_ADC_CHANNEL);
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


/*
 * @brief 获取FFT结果中的最大频率,返回最大频率
 */
float mic_fft_GetMaxfreq()
{
    float max_freq = 0;
    float max_freq_index = 0;

    for (uint16_t i = 0; i < MIC_ADC_DMA_BUF_LEN / 2;i++) // 虚实交替为一个元素
    {
        float temp  = mic.fft_Output[i*2] * mic.fft_Output[i*2] + mic.fft_Output[i*2+1] * mic.fft_Output[i*2+1];

        if (temp > max_freq)
        {
            max_freq = temp;
            max_freq_index = i;
        }
        
    }
    
    return (float)max_freq_index * SAMPLE_RATE / MIC_ADC_DMA_BUF_LEN;
}

/*
 * @brief 获取FFT结果中的响度,返回响度
 */
float mic_fft_Getloudness()
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

    *brightness = mic.loudness * brightness_max / MIC_LOUDNESS_MAX ;

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
    else if (delta_freq > 100.0f) // 快速升
    {
        mic.freq = HIGH_SPEED_FITLER * mic.freq + (1 - HIGH_SPEED_FITLER) * *cnt_freq;
    }
    else return;
}

 void mic_Run()
 {
    // 1. 转换为float数据
    mic_dma_buf_to_float();

    // 2. FFT 处理
    arm_rfft_fast_f32(&fft_inst, mic.adc_dma_buf_float, mic.fft_Output, 0);

    if (!mic_isCalibrate()) // 校准环境噪声
    {
        mic_Calibrate();
    }
    else
    {
        // 3. 获取最大频率
        float cnt_freq = mic_fft_GetMaxfreq();
        mic_freq_filter(&cnt_freq);

        // 4. 获取响度
        mic_fft_Getloudness();

        HAL_Delay(10);
    }
}


// 显示在TFT，用于调试
// note: 1.安静环境下看噪声最大值，2.音乐环境下看最大音量值
void mic_meature_noise_loudness_max()
{
    // 1. 转换为float数据
    mic_dma_buf_to_float();

    // 2. FFT 处理
    arm_rfft_fast_f32(&fft_inst, mic.adc_dma_buf_float, mic.fft_Output, 0);

    // 3. 获取响度
    mic_fft_Getloudness();

    // 4. 显示
    tft_DisplayNum(0,0,(uint32_t)mic.loudness,COLOR_WHITE,COLOR_BLACK);

    HAL_Delay(1000);
}