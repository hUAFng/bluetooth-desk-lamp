
// ADC使用DMA连续扫描，将扫描结果存储到dma缓冲区中。 


// note : MAX9814内有直流偏置，静音时，ADC值为2048（恒定），有噪声会在2048附近波动
// 响度 ： |ADC - 2048|（绝对值） ， 越大响度越大

#include "mic_drv.h"

static mic_t mic ;

static arm_rfft_fast_instance_f32 fft_inst; //FFT 实例


void mic_ClearBuf(void)
{
    memset(mic.adc_dma_buf, 0, sizeof(mic.adc_dma_buf));
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
 * @brief 启动ADC采集，已在mic_Work中调用，可选手动调用
 */
void mic_PowerOn(void)
{
    mic_ClearBuf();

    if (HAL_ADC_Start_DMA(&MIC_ADC_CHANNEL, (uint32_t *)mic.adc_dma_buf, MIC_ADC_DMA_BUF_LEN) != HAL_OK) 
        return;
    else
        mic.mic_work_flag = 1;


    HAL_Delay(100); // 延时100ms，等待数据稳定

    // mic_Calibrate();
}


/**
 * @brief 停止ADC采集
 */
void mic_PowerOff(void)
{
    HAL_ADC_Stop_DMA(&MIC_ADC_CHANNEL);
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
 * @brief 数据处理-保存响度与频率
 * @param brightness_max 最大亮度值，用于规范响度的最大值
 */
void mic_DataProcess(uint8_t brightness_max)
{
    uint32_t square_sum = 0; // 求响度-差值平方的总和
    
    uint8_t zero_count = 0; // 信号会反复过0点（2048），记录过0点的次数，用于求频率
    uint16_t value_prev = 0; // 上一个ADC值，用于确定是否过零点
    uint16_t value_current = 0; // 当前ADC值

    for (uint8_t i = 0; i < MIC_ADC_DMA_BUF_LEN; i++)
    {
        int16_t diff = mic.adc_dma_buf[i] - MIC_ADC_OFFSET; // 差值才是响度，所以要减去基准量 
        
        diff = abs(diff);

        // 噪声范围内的毛刺信号，不管 或 去除噪音之后的有效信号
        diff = (diff <= mic.noise_floor) ? 0 : diff - mic.noise_floor;
        
        square_sum += diff * diff; //计算平方和

        value_current = mic.adc_dma_buf[i];
        if (value_prev != 0 && (value_current > MIC_ADC_OFFSET && value_prev < MIC_ADC_OFFSET  \
        || value_current < MIC_ADC_OFFSET && value_prev > MIC_ADC_OFFSET) ) // 两个数据点在2048的不同侧，说明过零点
        {
            zero_count++;
        }
        
        value_prev = value_current;
    }

    // note : 响度采用平方求平均，因为平方的增长速度比较快，对声音比较敏感 1->1 2->4 3->9 
    // 其次，/ MIC_LOUDNESS_PARAM 想将响度投射到亮度的区间（平方是非线性的，不要去求平凡的最大值，那样和不平方没有区别）
    // 但是没有合适的模型，所以这里得不停调 MIC_LOUDNESS_PARAM
    // 观察RGB的表现以获取最佳的MIC_LOUDNESS_PARAM值
    mic.loudness = (uint8_t)((MIC_LOW_PASS_FILTER)*(square_sum / (MIC_ADC_DMA_BUF_LEN * MIC_LOUDNESS_PARAM)) \
    + (1-MIC_LOW_PASS_FILTER)*mic.loudness);   //低通滤波，给两个值加上权重，平滑响应


    if (mic.loudness > brightness_max) mic.loudness = brightness_max;  // 规范区间
    else if (mic.loudness < 3) mic.loudness = 0; // 减少小噪声让RGB乱跳

    if (zero_count > 2) //添加噪声导致的毛刺判断
    {
        // 增加低通滤波，给两个值加上权重，平滑响应
        mic.freq = (1-MIC_LOW_PASS_FILTER)*mic.freq + (uint8_t)(MIC_LOW_PASS_FILTER*(zero_count * MIC_FQ_MAX / MIC_ADC_DMA_BUF_LEN)); // 直接映射 到0-100的区间 假如zero_count == MIC_ADC_DMA_BUF_LEN，说明每个数据点都在跳，说明频率fq很高
    }
    else mic.freq = 0; // 噪声导致的少量过零点视为无信号
    
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

uint8_t mic_GetFreqLevel(void)
{
    if (mic.freq == 0) return 0; // 无信号
    if (mic.freq <= 10) return 1; // 低频
    if (mic.freq <= 25) return 2; // 中低频
    if (mic.freq <= 45) return 3; // 中频
    if (mic.freq <= 70) return 4; // 中高频
    return 5; // 高频
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

void 

 void mic_Run()
 {
    if ()
    // 1. 转换为float数据
    mic_dma_buf_to_float();

    // 2. FFT 处理
    arm_rfft_fast_f32(&fft_inst, mic.adc_dma_buf_float, mic.fft_Output, 0);

    // 3. 获取最大频率
    mic_fft_GetMaxfreq();

    HAL_Delay(20);



 }