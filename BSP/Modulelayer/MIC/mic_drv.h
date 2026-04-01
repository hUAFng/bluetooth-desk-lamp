

#ifndef __MIC_DRV_H__
#define __MIC_DRV_H__ 


#include "adc_drv.h"
#include "string.h"
#include "stdio.h"
#include "tim.h"
#include "arm_math.h"

extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim3;

/* -------------------------------------------------define---------------------------------------------------*/
#define MIC_ADC_CHANNEL hadc1

#define MIC_ADC_DMA_BUF_LEN 256  // DMA的缓冲区长度,使用FFT方案需要是间为2的幂数，数字越大分辨率越高
#define MIC_ADC_OFFSET 2048 // ADC的基准量 ， 静音时，ADC值为2048（恒定）
#define MIC_FQ_MAX 100 // 最大频率值
#define MIC_MAX_REMAP_VALUE 255
#define MIC_MAX_NOSIE_FLOOR_ADC_VALUE 200 // 正常环境的最大的噪声值ADC值偏移量 目的是防止未校准之前就播放音乐，干扰校准结果

#define SAMPLE_RATE 10000 // 采样率 10KHz = 72M / (100 * 100) 定时器溢出频率

// 需要调节的参数：
#define MIC_LOW_PASS_FILTER 0.35 // 低通滤波系数，用于平滑ADC值，减少冲击
#define MIC_LOUDNESS_PARAM 3000 // 未来需要调参修改，其实就是用来求响度的平均，需要看灯光的响应效果以及RGB的最大亮度来调节
#define MIC_LOUDNESS_MAX 300 // 需要在安静的环境下测试，查看这个数据的最大值
/* -------------------------------------------------valiables-------------------------------------------------*/

typedef struct 
{
    uint16_t adc_dma_buf[MIC_ADC_DMA_BUF_LEN]; // dma缓冲区，用于求均值滤波，储存ADC值（0-4095）对应0-3.3V
    float adc_dma_buf_float[MIC_ADC_DMA_BUF_LEN]; // dma缓冲区float类型，储存ADC值
    float fft_Output[MIC_ADC_DMA_BUF_LEN * 2]; // FFT 输出，储存FFT结果,每个数是占两个字节-实部和虚部

    uint16_t adc_avg; // 平滑后的ADC平均值

    uint8_t mic_work_flag; // 0-不工作，1-工作 用于调用一次mic_Start();

    uint8_t noise_floor ;  // 环境噪声导致的偏移量 , 注意不是ADC值 ， 而是ADC值与基准量的差值
    uint8_t calibrated_flag ; // 0-未校准，1-已校准

    float loudness ; // 响度
    float freq; // 频率
}mic_t; 



/* -------------------------------------------------functions-------------------------------------------------*/
void mic_Init(void);
void mic_PowerOff(void);
void mic_PowerOn(void); // 已在mic_Work中调用，可选手动调用
void mic_Calibrate(void); //调用开始函数自动校准
void mic_Work(uint8_t brightness_max,uint8_t* loudness,uint8_t* fq);
uint8_t mic_GetFreqLevel(void);
void mic_loudness_mapto_brightness(uint8_t brightness_max,uint8_t brightness_min,uint8_t* brightness,uint8_t low_bright_area);



#endif



