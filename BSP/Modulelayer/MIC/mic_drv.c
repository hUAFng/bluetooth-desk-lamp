
// ADC使用DMA连续扫描，将扫描结果存储到dma缓冲区中。 


// note : MAX9814内有直流偏置，静音时，ADC值为2048（恒定），有噪声会在2048附近波动
// 响度 ： |ADC - 2048|（绝对值） ， 越大响度越大

#include "mic_drv.h"

static mic_t mic ;
static arm_rfft_fast_instance_f32 fft_inst; //FFT 实例


void mic_ClearBuf(void)
{
    memset(mic.adc_dma_buf, 0, sizeof(mic.adc_dma_buf));
    memset(mic.adc_dma_buf_float, 0, sizeof(mic.adc_dma_buf_float));
    memset(mic.fft_Output, 0, sizeof(mic.fft_Output));
    mic.adc_avg = 0;
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
    arm_rfft_fast_init_f32(&fft_inst, MIC_ADC_DMA_BUF_LEN); // 初始化
}






/**
 * @brief 获取ADC平均值 - 使用于校准
 */
void mic_GetAdcAvg(void)
{
    uint32_t sum = 0;
    for (uint8_t i = 0; i < MIC_ADC_DMA_BUF_LEN; i++)
    {
        sum += mic.adc_dma_buf[i];
    }
    mic.adc_avg = sum / MIC_ADC_DMA_BUF_LEN ;
}



/**
 * @brief 校准ADC 获取噪声的ADC值，用作后续的映射的起始量，不然环境噪声也会导致有映射量，不符合预期
 * @note: 调用开始函数自动校准
 */
void mic_Calibrate(void)
{
    uint32_t total = 0;
    uint8_t time = 3; // 获取噪声的次数

    for (uint8_t i = 0; i < time; i++) //求噪声均值 ，求time次
    {
        HAL_Delay(50);
        mic_GetAdcAvg();
        total += mic.adc_avg;
    }

    uint16_t avg_value = total / time;
    uint16_t noise_floor = abs(avg_value - MIC_ADC_OFFSET);

    if(abs(noise_floor) > MIC_MAX_NOSIE_FLOOR_ADC_VALUE) 
        mic.noise_floor = MIC_MAX_NOSIE_FLOOR_ADC_VALUE;
    else
        mic.noise_floor = noise_floor;

    mic.calibrated_flag = 1;
}

static inline uint8_t mic_isCalibrate(void)
{
    return mic.calibrated_flag;
}

uint8_t mic_isWorking(void)
{
    return mic.mic_work_flag;
}

/**
 * @brief 麦克风工作函数，获取音频数据并进行处理
 * @param remap_value_MAX 需要映射的值(亮度)的最大值(<=255) ， 比如0-4095映射到0-255，则remap_value_MAX = 255
 * @param remap_value 映射后的数据
 * @param duration 间隔的时间，单位：ms 由于RGB切换需要时间来平滑过渡不生硬，所以需要设置一个间隔时间，可以尝试20ms
 */
void mic_Work(uint8_t brightness_max,uint8_t* loudness,uint8_t* fq)
{
    if (loudness == NULL || fq == NULL) return;
    if (brightness_max >= 255) return; // RGB通道的最大亮度值为255

    if (!mic.mic_work_flag) // 首次调用，启动麦克风 只进行一次
    {
        mic_PowerOn(); //自动校准

        // 未校准，或者未工作
        if (mic.calibrated_flag == 0 || !mic.mic_work_flag) return;
    }

    mic_DataProcess(brightness_max);

    *loudness = mic.loudness;
    *fq = mic.freq;
}



/*-----------------------------------------------------------------------------------------
 *                                      FFT 方案
 ------------------------------------------------------------------------------------------*/


 // 定时器采用TIM3溢出触发ADC转换，相对于直接触发ADC转换，可以减少ADC转换的次数，降低转换频率，节省CPU。
void mic_PowerOn(void)
{
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
 * @brief 获取FFT结果中的最大频率
 */
void mic_fft_GetMaxfreq()
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
        
        mic.freq = (float)max_freq_index * SAMPLE_RATE / MIC_ADC_DMA_BUF_LEN;

    }

}

void mic_fft_Getloudness()
{
    float energy = 0;

    for (int i = 0;i < MIC_ADC_DMA_BUF_LEN;i++)
    {
        energy += mic.adc_dma_buf_float[i] * mic.adc_dma_buf_float[i]; // 平方
    }

    mic.loudness = sqrt(energy / MIC_ADC_DMA_BUF_LEN);
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

 void mic_Run()
 {
    if ()

    // 1. 转换为float数据
    mic_dma_buf_to_float();

    // 2. FFT 处理
    arm_rfft_fast_f32(&fft_inst, mic.adc_dma_buf_float, mic.fft_Output, 0);

    // 3. 获取最大频率
    mic_fft_GetMaxfreq();

    // 4. 获取响度
    mic_fft_Getloudness();

    HAL_Delay(20);
}