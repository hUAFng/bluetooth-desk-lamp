
#ifndef __APP_ALGORITHM_H__
#define __APP_ALGORITHM_H__

#include "main.h"
#include "module_layer.h"
#include "app_common.h"
#include "math.h"
#include "app_modes.h"


#define LED_B_LEVEL_KEEP_TIME 100 // 高低电平保持时间100ms ,即周期为200ms

typedef struct
{
	uint8_t work_flag; // 是否工作标志位
	
	uint8_t level_value;  // 高电平或低电平，由转换
	
	uint32_t start_tick;  // 开始工作的时间戳
}led_b_work_t;


void remap_lux_to_brightness(void);
void Sys_Control_By_BTorASR(Cmd_e cmd);
void led_blue_work_In_listen(Cmd_e cmd);
void led_b_work_handle();

#endif