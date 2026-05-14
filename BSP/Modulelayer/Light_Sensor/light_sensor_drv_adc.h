

#ifndef __LIGHT_SENSOR_DRV_ADC_H__
#define __LIGHT_SENSOR_DRV_ADC_H__

#include "main.h"


extern ADC_HandleTypeDef hadc2;

#define LS_ADC_HANDLE hadc2

#define LS_ADC_MAX      4000   //最暗
#define LS_ADC_MIN  	400    //最亮
#define LS_ADC_BUF_LEN 32

typedef struct 
{
    uint16_t buf[LS_ADC_BUF_LEN];
    uint16_t adc_average;

}ls_adc_t;



void ls_adc_Init(void);
void ls_adc_PowerOn(void);
void ls_adc_PowerOff(void);
void ls_adc_work();
uint16_t ls_adc_get_value();

#endif