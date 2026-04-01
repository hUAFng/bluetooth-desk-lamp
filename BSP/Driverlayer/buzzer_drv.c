


#include "buzzer_drv.h"

#define BUZZER_WORK_DURATION 1000 // 工作持续时间

void buzzer_Init(void)
{
    HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_RESET); // 初始安静
}

void buzzer_work()
{
    HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_SET); // 工作
    HAL_Delay(BUZZER_WORK_DURATION);
    HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_RESET); // 安静
}


