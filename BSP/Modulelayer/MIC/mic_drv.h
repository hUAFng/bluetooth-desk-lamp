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

#define MIC_ADC_DMA_BUF_LEN 256
#define MIC_LOUDNESS_MAX 2000

#define LOW_FREQ_THRESHOLD 300.0f
#define MID_FREQ_THRESHOLD 2000.0f
#define SAMPLE_RATE 10000

#define DOWN_DIRETION_FILTER 0.92f
#define NARROW_FILTER 0.95f
#define MID_SPEED_FILTER 0.85f
#define HIGH_SPEED_FILTER 0.75f

#define GOERTZEL_FREQ_NUM 7

#define LOUDNESS_GATE_THRESHOLD  40.0f
#define MIC_GOERTZEL_MAX_FREQ 4800.0f
#define GOERTZEL_ENERGY_THRESHOLD 500.0f
#define LOUDNESS_TO_BRIGHTNESS_GAIN  6.0f
/* -------------------------------------------------valiables-------------------------------------------------*/

typedef struct 
{
    uint16_t adc_dma_buf[MIC_ADC_DMA_BUF_LEN];
    float adc_dma_buf_float[MIC_ADC_DMA_BUF_LEN];

    uint8_t dma_data_ready_flag;
    uint8_t work_flag;
    
    float noise_floor;

    float loudness;
    float freq;
}mic_t; 

typedef struct 
{
    float target_freq;
    uint16_t k;
    float coeff;
} goertzel_coeff_t;

/*
k = (uint16_t)(0.5f + ((MIC_ADC_DMA_BUF_LEN * target_freq) / SAMPLE_RATE));
coeff = 2.0f * cosf(2.0f * 3.1415926f * k / MIC_ADC_DMA_BUF_LEN);
*/



/* -------------------------------------------------functions-------------------------------------------------*/
void mic_Init(void);
void mic_PowerOff(void);
void mic_PowerOn(void);
void mic_Calibrate(void);
void mic_loudness_mapto_brightness(uint8_t brightness_max,uint8_t brightness_min,uint8_t* brightness,uint8_t low_bright_area);
void mic_GetFreq(float *freq);
void mic_GetLoudness(float *loudness);
uint8_t mic_Run();

void mic_DebugDumpToUART(void);

#endif
