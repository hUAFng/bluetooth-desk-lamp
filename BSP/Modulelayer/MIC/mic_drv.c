
// ADC使用DMA连续扫描，将扫描结果存储到dma缓冲区中。 

#include "mic_drv.h"

static mic_t mic ;


void mic_ClearBuf(void)
{
    memset(mic.adc_dma_buf, 0, sizeof(mic.adc_dma_buf));
    mic.adc_avg = 0;
    mic.mic_work_flag = 0; 
    mic.noise_floor = 0;
    mic.is_calibrated = 0;
}

void mic_Init(void)
{
    mic_ClearBuf();
    mic_Stop();
}


/**
 * @brief 启动ADC采集，已在mic_Work中调用，可选手动调用
 */
void mic_Start(void)
{
    mic_ClearBuf();

    if (HAL_ADC_Start_DMA(&MIC_ADC_CHANNEL, (uint32_t *)mic.adc_dma_buf, MIC_ADC_DMA_BUF_LEN) != HAL_OK) 
        return;

    HAL_Delay(100); // 延时100ms，等待数据稳定

    mic_Calibrate();
}


/**
 * @brief 停止ADC采集
 */
void mic_Stop(void)
{
    HAL_ADC_Stop_DMA(&MIC_ADC_CHANNEL);
}


/**
 * @brief 获取ADC平均值
 * @note : 注意！此处获取的是ADC的测量值（0-4095），并未做任何映射
 */
void mic_GetAdcAvg(void)
{
    uint32_t sum = 0;

    for (uint8_t i = 0; i < MIC_ADC_DMA_BUF_LEN; i++)
    {
        sum += mic.adc_dma_buf[i];    
    }
    
    mic.adc_avg = sum / MIC_ADC_DMA_BUF_LEN;
}


/**
 * @brief 校准ADC 获取噪声的ADC值，用作后续的映射的起始量，不然环境噪声也会导致有映射量，不符合预期
 * @note: 调用开始函数自动校准
 */
void mic_Calibrate(void)
{
    uint32_t total = 0;

    for (uint8_t i = 0; i < 3; i++) //求噪声均值
    {
        HAL_Delay(50);
        mic_GetAdcAvg();
        total += mic.adc_avg;
    }

    uint16_t avg_value = total / 3;

    if (avg_value > MIC_MAX_NOSIE_FLOOR_ADC_VALUE) // 防止未校准之前就播放音乐，干扰校准结果
        mic.noise_floor = MIC_MAX_NOSIE_FLOOR_ADC_VALUE;
    else 
        mic.noise_floor = avg_value;

    mic.is_calibrated = 1;
}

static inline uint8_t mic_isCalibrate(void)
{
    return mic.is_calibrated;
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
void mic_Work(uint8_t remap_value_MAX , uint8_t* remap_value , uint8_t duration)
{
    if (remap_value == NULL) return;

    if (remap_value_MAX > MIC_MAX_REMAP_VALUE) remap_value_MAX = MIC_MAX_REMAP_VALUE;

    if (!mic.mic_work_flag) // 首次调用，启动麦克风 只进行一次
    {
        mic_Start(); //自动校准

        mic.mic_work_flag = 1;
    }

    mic_GetAdcAvg();

    // ">> 12"与 /4095 是一样的
    if (mic_isCalibrate() && mic.noise_floor <= mic.adc_avg) // 校准完成，且当前ADC值大于噪声值
    {
        uint16_t adjusted_value = mic.adc_avg - mic.noise_floor; //以噪声值作为基准，映射空间：noise_floor - 4095

        *remap_value = (remap_value_MAX * adjusted_value) >> 12;
    }
    else 
        *remap_value = (remap_value_MAX * mic.adc_avg) >> 12; // 未校准时的一般映射

    HAL_Delay(duration);
    
}