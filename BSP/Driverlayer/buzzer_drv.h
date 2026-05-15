

#ifndef __BUZZER_DRV_H__
#define __BUZZER_DRV_H__


#include "main.h"

#define BUZZER_WORK_DURATION 50 // 工作持续时间（ms）


void buzzer_work();
void buzzer_stop_work();
void buzzer_Init();
void buzzer_monitor();
uint8_t buzzer_isworking();
void buzzer_tick_handler(void);

#endif