

#ifndef __MIC_DRV_H__
#define __MIC_DRV_H__ 


#include "adc_drv.h"
#include "string.h"

extern ADC_HandleTypeDef hadc1;


/* -------------------------------------------------define---------------------------------------------------*/
#define MIC_ADC_CHANNEL hadc1

#define MIC_ADC_DMA_BUF_LEN 15  // DMA的缓冲区长度

#define MIC_MAX_REMAP_VALUE 255
/* -------------------------------------------------valiables-------------------------------------------------*/



typedef struct 
{
    uint16_t adc_dma_buf[MIC_ADC_DMA_BUF_LEN]; // dma缓冲区，用于求均值滤波，储存ADC值（0-4095）对应0-3.3V

    uint16_t adc_avg; // 平滑后的ADC平均值

    uint8_t mic_work_flag; // 0-不工作，1-工作 用于调用一次mic_Start();
}mic_t; 



/* -------------------------------------------------functions-------------------------------------------------*/
void mic_Work(uint8_t remap_value_MAX , uint8_t* remap_value , uint8_t duration);
void mic_Init(void);
void mic_Stop(void);
void mic_Start(void); // 已在mic_Work中调用，可选手动调用


#endif

