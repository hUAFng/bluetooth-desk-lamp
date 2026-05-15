

#include "light_sensor_drv_adc.h"

static ls_adc_t ls_adc;


void ls_adc_Init()
{
	ls_adc_PowerOff();
}

void ls_adc_work()
{
	uint32_t total_value = 0;
	
	for (uint8_t i = 0;i < LS_ADC_BUF_LEN;i++)
	{
		HAL_ADC_PollForConversion(&LS_ADC_HANDLE,10);
		
		ls_adc.buf[i] = HAL_ADC_GetValue(&LS_ADC_HANDLE);
		
		total_value += ls_adc.buf[i];
	}
	
    ls_adc.adc_average = total_value / LS_ADC_BUF_LEN;
}

void ls_adc_PowerOn()
{
    HAL_ADC_Start(&LS_ADC_HANDLE);
}

void ls_adc_PowerOff()
{
	HAL_ADC_Stop(&LS_ADC_HANDLE);
}


uint16_t ls_adc_get_value()
{
	return ls_adc.adc_average;
}
