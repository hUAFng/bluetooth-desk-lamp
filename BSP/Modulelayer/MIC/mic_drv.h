

#ifndef __MIC_DRV_H__
#define __MIC_DRV_H__ 

#include "main.h"
#include "adc_drv.h"
#include "string.h"
#include "math.h"
#include "tim.h"


extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim3;

/* -------------------------------------------------define---------------------------------------------------*/
#define MIC_ADC_CHANNEL hadc1

#define MIC_ADC_DMA_BUF_LEN 256  // DMA的缓冲区长度,使用FFT方案需要是间为2的幂数，数字越大分辨率越高
#define MIC_LOUDNESS_MAX 2000 // 响度的最大值（RMS值，12位ADC最大偏差±2047，RMS≈1448，预留余量）

#define LOW_FREQ_THRESHOLD 300.0f // 低频区域阈值,往下走低频，往上走中频
#define MID_FREQ_THRESHOLD 2000.0f // 高频区域阈值，往下走中频，往上走高频
#define SAMPLE_RATE 10000 // 采样率 10KHz = 72M / (100 * 100) 定时器溢出频率

/*note : 频率适用的滤波系数 系数越大 响应越慢越平滑 */
#define DOWN_DIRETION_FILTER 0.75f // 降低方向滤波系数
#define NARROW_FILTER 0.85f // 短距离滤波系数 不分降低升高 频率亮度适用
#define MID_SPEED_FILTER 0.7f // 中距离升高滤波系数
#define HIGH_SPEED_FILTER 0.55f // 高距离升高滤波系数

#define GOERTZEL_FREQ_NUM 16
/* -------------------------------------------------valiables-------------------------------------------------*/

typedef struct 
{
    uint16_t adc_dma_buf[MIC_ADC_DMA_BUF_LEN]; // ADC原始值(0-4095)
    float adc_dma_buf_float[MIC_ADC_DMA_BUF_LEN]; // 将原始数据转float并去直流偏置后的值

    uint8_t dma_data_ready_flag; // DMA数据传输完成标志位
    uint8_t work_flag;
    
    float noise_floor ;  // 环境噪声导致的响度偏移值

    float loudness ; // 响度
    float freq; // 频率
}mic_t; 

typedef struct 
{
    float target_freq;  // 目标频率
    uint16_t k;         // 频点编号 
    float coeff;        // 递推系数 
} goertzel_coeff_t;

/*
k = (uint16_t)(0.5f + ((MIC_ADC_DMA_BUF_LEN * target_freq) / SAMPLE_RATE));
coeff = 2.0f * cosf(2.0f * 3.1415926f * k / MIC_ADC_DMA_BUF_LEN);
*/



/* -------------------------------------------------functions-------------------------------------------------*/
void mic_Init(void);
void mic_PowerOff(void);
void mic_PowerOn(void); // 已在mic_Work中调用，可选手动调用
void mic_Calibrate(void); //调用开始函数自动校准
void mic_loudness_mapto_brightness(uint8_t brightness_max,uint8_t brightness_min,uint8_t* brightness,uint8_t low_bright_area);
void mic_GetFreq(float *freq);
void mic_Run();

#endif




