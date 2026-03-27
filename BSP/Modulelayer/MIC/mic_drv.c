
// ADC使用DMA连续扫描，将扫描结果存储到dma缓冲区中。 

#include "mic_drv.h"

static mic_t mic ;


void mic_ClearBuf(void)
{
    memset(mic.adc_dma_buf, 0, sizeof(mic.adc_dma_buf));
    mic.adc_avg = 0;
    mic.mic_work_flag = 0; 
}

void mic_Init(void)
{
    HAL_ADC_Stop_DMA(&MIC_ADC_CHANNEL);  // 上电默认关断ADC与麦克风功能

    mic_ClearBuf();
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
}


/**
 * @brief 停止ADC采集
 */
void mic_Stop(void)
{
    HAL_ADC_Stop_DMA(&MIC_ADC_CHANNEL);
    mic.mic_work_flag = 0;
    memset(mic.adc_dma_buf, 0, sizeof(mic.adc_dma_buf));
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
 * @brief 麦克风工作函数，获取音频数据并进行处理
 * @param remap_value_MAX 需要映射的值(亮度)的最大值(<=255) ， 比如0-4095映射到0-255，则remap_value_MAX = 255
 * @param remap_value 映射后的数据
 * @param duration 间隔的时间，单位：ms 由于RGB切换需要时间来平滑过渡不生硬，所以需要设置一个间隔时间，可以尝试20ms
 */
void mic_Work(uint8_t remap_value_MAX , uint8_t* remap_value , uint8_t duration)
{
    if (remap_value == NULL) return;

    if (remap_value_MAX > MIC_MAX_REMAP_VALUE) remap_value_MAX = MIC_MAX_REMAP_VALUE;

    if (!mic.mic_work_flag)
    {
        mic_Start();
        mic.mic_work_flag = 1;
    }



    mic_GetAdcAvg();

    *remap_value = (remap_value_MAX * mic.adc_avg) >> 12; 

    HAL_Delay(duration);
}