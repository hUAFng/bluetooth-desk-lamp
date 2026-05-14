

#include "light_sensor_drv_adc.h"


void ls_adc_Init()
{
	
}

void ls_adc_work()
{
    
}

void ls_adc_PowerOn()
{
    HAL_ADC_Start(&LS_ADC_HANDLE);
}

void ls_adc_PowerOff()
{
	HAL_ADC_Stop(&LS_ADC_HANDLE);
}

