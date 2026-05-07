


#include "buzzer_drv.h"

static uint8_t buzzer_work_flag ;

void buzzer_Init(void)
{
    HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_RESET); // 初始安静
}


void buzzer_work()
{
	buzzer_work_flag = 1;
}

void buzzer_stop_work()
{
	buzzer_work_flag = 0;
}


// 在SysTick_Handler(void)停止

void buzzer_monitor()
{
	if (buzzer_work_flag)
		HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_SET); // 工作
    else
		HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_RESET); // 安静
}

uint8_t buzzer_isworking()
{
	return buzzer_work_flag;
}
