

#include "app_algorithm.h"
#include "app_manager.h"

static led_b_work_t led_b_work;

/**
 * @brief 映射光照强度到灯带亮度 低通滤波
 */
void remap_lux_to_brightness(void)
{
    static float filtered_brightness = 60.0f;
    float lux = system.system_data.ls_data.lux;
    float brightness_filter = system.system_data.rgb_data.filter_lux;

	if (lux > LS_LUX_MAX) lux = LS_LUX_MAX;

    float target_brightness = RGB_MAX_BRIGHTNESS * sqrtf((LS_LUX_MAX - lux) / LS_LUX_MAX);

	if (target_brightness < RGB_MIN_BRIGHTNESS) target_brightness = RGB_MIN_BRIGHTNESS;
	
    filtered_brightness = brightness_filter * filtered_brightness + 
                          (1.0f - brightness_filter) * target_brightness;

    system.system_data.rgb_data.brightness = (uint8_t)filtered_brightness;
}


// 系统控制：蓝牙、语音控制处理算法，由system_Control()调用
void Sys_Control_By_BTorASR(Cmd_e cmd)
{
    if(system.mode == Sys_Mode_PowerOff)
    {
        if(cmd == CMD_POWER_ON) system_PowerOn(); // 只有在关机模式下才处理开机命令
        else return; // 其他命令在关机模式下不处理
    }
    else 
    {
        switch(cmd)
        {
            case CMD_POWER_OFF:
                system_PowerOff(); // 只有在非关机模式下才处理关机命令
                break;
            case CMD_ManualMode:
                sys_mode_Manual_Init(); //从其他工作模式切换到手动模式，需要初始化
                system.mode = Sys_Mode_Manual;
                break;
            case CMD_AutoMode:
                sys_mode_Auto_Init();
                system.mode = Sys_Mode_Auto;
                break;
            case CMD_MusicMode:
                sys_mode_Music_Init();
                system.mode = Sys_Mode_Music;
                break;
            case CMD_CHANGE_COLOR:
				if (system.mode != Sys_Mode_Music)
				{
					rgb_SetColor_Circle(&system.system_data.rgb_data.color); // 改变颜色(循环切换)
					rgb_update();
				}
					
                break;
            case CMD_RGB_LIGHT_ON:
				if(system.mode == Sys_Mode_Manual) 
				{
					rgb_SetBrightnessUp(&system.system_data.rgb_data.brightness); // 处理灯带亮一点命令
					rgb_update();
				}
                break;
            case CMD_RGB_LIGHT_OFF:
				if(system.mode == Sys_Mode_Manual)  // 只有手动模式才能调亮度
				{
					rgb_SetBrightnessDown(&system.system_data.rgb_data.brightness); // 处理灯带暗一点命令
					rgb_update();
				}
                break;
            default:
                break;
        }

    }
}

void led_blue_work_In_listen(Cmd_e cmd)
{
	if (cmd >= NoneCmd) return;
	
	led_b_work.work_flag = 1;
	
	led_b_work.start_tick = HAL_GetTick();

    led_b_SetworkingFlag();

}


// void SysTick_Handler(void)调用(调用周期T=1ms)
void led_b_work_handle()
{
	static uint16_t counter;
	
	if (!led_b_work.work_flag) return;
	
	if (counter == 0)
	{
		led_b_work.level_value == 1 ? led_work(LED_B_ON) : led_work(LED_B_OFF);
	}
	
	counter++;
	
	if (counter >= LED_B_LEVEL_KEEP_TIME) 
	{
		led_b_work.level_value = (led_b_work.level_value + 1) % 2;
		
		counter = 0;
		
		if (HAL_GetTick() - led_b_work.start_tick >= ASR_WAKEUP_WAIT_TIME * 1000) // 唤醒之后指定时间内没有得到有效信息
		{
			led_b_work.work_flag = 0; 

            led_b_ClearworkingFlag();
			
			led_work(LED_B_OFF);
		}
	}
}



