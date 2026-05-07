


#include "buzzer_drv.h"

typedef struct 
{
	uint8_t work_flag ;
	uint8_t work_count;
	
}Buzzer_t;

static Buzzer_t buzzer = {.work_flag = 0,.work_count = 0};


void buzzer_Init(void)
{
    HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_RESET); // 初始安静
}


void buzzer_work()
{
	buzzer.work_flag = 1;
	
	buzzer.work_count = 0; 
}

void buzzer_stop_work()
{
	buzzer.work_flag = 0;
}




void buzzer_monitor()
{
	if (buzzer.work_flag)
		HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_SET); // 工作
    else
		HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_RESET); // 安静
}

uint8_t buzzer_isworking()
{
	return buzzer.work_flag;
}

// 在SysTick_Handler(void)调用
void buzzer_tick_handler(void)
{
	if (buzzer.work_flag)
	{
		buzzer.work_count++;
		
		if (buzzer.work_count >= BUZZER_WORK_DURATION)
		{
			buzzer.work_count = 0;
			
			buzzer_stop_work();
		}
	}
}
