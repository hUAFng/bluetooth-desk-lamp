

#ifndef __MIC_DRV_H__
#define __MIC_DRV_H__ 

#include "main.h"
#include "adc_drv.h"
#include "string.h"
#include "stdio.h"
#include "tim.h"
#include "math.h"
#include "tft_drv.h"


extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim3;

/* -------------------------------------------------define---------------------------------------------------*/
#define MIC_ADC_CHANNEL hadc1

#define MIC_ADC_DMA_BUF_LEN 256  // DMA的缓冲区长度,使用FFT方案需要是间为2的幂数，数字越大分辨率越高
#define MIC_MAX_NOSIE_FLOOR_VALUE 3000  // 环境噪声的最大值
#define MIC_LOUDNESS_MAX 10000 // 响度的最大值

#define LOW_FREQ_THRESHOLD 200.0f // 低频区域阈值,往下走低频，往上走中频
#define MID_FREQ_THRESHOLD 2000.0f // 高频区域阈值，往下走中频，往上走高频
#define SAMPLE_RATE 10000 // 采样率 10KHz = 72M / (100 * 100) 定时器溢出频率

/*note : 频率适用的滤波系数 系数越大 响应越慢越平滑 */
#define DOWN_DIRETION_FILTER 0.75f // 降低方向滤波系数
#define NARROW_FILTER 0.9f // 短距离滤波系数 不分降低升高 频率亮度适用
#define MID_SPEED_FILTER 0.7f // 中距离升高滤波系数
#define HIGH_SPEED_FITLER 0.55f // 高距离升高滤波系数

#define GOERTZEL_FREQ_NUM 8
/* -------------------------------------------------valiables-------------------------------------------------*/

typedef struct 
{
    uint32_t adc_dma_buf[MIC_ADC_DMA_BUF_LEN]; // dma缓冲区，用于求均值滤波，储存ADC值（0-4095）对应0-3.3V
    float adc_dma_buf_float[MIC_ADC_DMA_BUF_LEN]; // dma缓冲区float类型，储存ADC值
    float fft_Output[GOERTZEL_FREQ_NUM]; // FFT 输出

    uint8_t mic_work_flag; 

    float noise_floor ;  // 环境噪声导致的响度偏移值
    uint8_t calibrated_flag ; // 0-未校准，1-已校准

    float loudness ; // 响度
    float freq; // 频率
}mic_t; 



/* -------------------------------------------------functions-------------------------------------------------*/
void mic_Init(void);
void mic_PowerOff(void);
void mic_PowerOn(void); // 已在mic_Work中调用，可选手动调用
void mic_Calibrate(void); //调用开始函数自动校准
void mic_loudness_mapto_brightness(uint8_t brightness_max,uint8_t brightness_min,uint8_t* brightness,uint8_t low_bright_area);
void mic_GetFreq(float *freq);
void mic_Run();
void mic_meature_noise_loudness_max();

#endif




