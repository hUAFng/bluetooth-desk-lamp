

#ifndef __MIC_DRV_H__
#define __MIC_DRV_H__ 


#include "adc_drv.h"
#include "string.h"

extern ADC_HandleTypeDef hadc1;


/* -------------------------------------------------define---------------------------------------------------*/
#define MIC_ADC_CHANNEL hadc1

#define MIC_ADC_DMA_BUF_LEN 15  // DMA的缓冲区长度

#define MIC_MAX_REMAP_VALUE 255

#define MIC_MAX_NOSIE_FLOOR_ADC_VALUE 200 // 正常环境的最大的噪声值ADC值 目的是防止未校准之前就播放音乐，干扰校准结果

/* -------------------------------------------------valiables-------------------------------------------------*/



typedef struct 
{
    uint16_t adc_dma_buf[MIC_ADC_DMA_BUF_LEN]; // dma缓冲区，用于求均值滤波，储存ADC值（0-4095）对应0-3.3V

    uint16_t adc_avg; // 平滑后的ADC平均值

    uint8_t mic_work_flag; // 0-不工作，1-工作 用于调用一次mic_Start();

    uint8_t noise_floor ;  // 环境噪声导致的偏移量 ， 并将这个量作为映射的起始量 （noise_floor - 4095）
    uint8_t is_calibrated ; // 0-未校准，1-已校准
}mic_t; 



/* -------------------------------------------------functions-------------------------------------------------*/
void mic_Work(uint8_t remap_value_MAX , uint8_t* remap_value , uint8_t duration);
void mic_Init(void);
void mic_Stop(void);
void mic_Start(void); // 已在mic_Work中调用，可选手动调用
void mic_Calibrate(void); //调用开始函数自动校准


#endif



