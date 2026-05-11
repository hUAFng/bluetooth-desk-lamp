

// note : 此文件为系统模式工作相关的函数，包括手动模式和自动模式、音乐模式，由app_manager.c调用

#include "app_modes.h"


void sys_mode_Manual_Init() 
{
    system_valiable_Init(); // 系统模式已经在app_algorithm.c中赋值为手动模式

    rgb_PowerOn(system.system_data.rgb_data.color,system.system_data.rgb_data.brightness); 
    ls_PowerOff(); // 手动模式不使用光线传感器
    mic_PowerOff(); // 手动模式不使用麦克风
}

void sys_mode_Auto_Init()
{
	system.system_data.ls_data.mode = LS_MODE_HRES1;
    system.system_data.ls_data.lux = 50.0f;
	
    rgb_PowerOn(system.system_data.rgb_data.color,system.system_data.rgb_data.brightness); 

	/*
    if (ls_PowerOn() != HAL_OK)
    {
        if (ls_Reset() != HAL_OK) Error_Handler();
    }
	
	*/
	

    mic_PowerOff(); // 自动模式不使用麦克风
}

void sys_mode_Music_Init()
{
    system_valiable_Init(); // 系统模式已经在app_algorithm.c中赋值为音乐模式

    rgb_PowerOn(system.system_data.rgb_data.color,system.system_data.rgb_data.brightness); 
	
    ls_PowerOff();
    mic_PowerOn();
}



/**
 * @brief 手动模式 仅使用按键3和按键2调节颜色和亮度
 */
void sys_mode_Manual(void)
{
    if (key_Read(KEY2)) // 按键2 调节亮度，循环调节
    {
        rgb_SetBrightness_Circle(&system.system_data.rgb_data.brightness);

         rgb_update();
    }
    else if (key_Read(KEY3)) // 按键3 调节颜色，循环调节
    {
        rgb_SetColor_Circle(&system.system_data.rgb_data.color);
        rgb_update();
    }
}


/**
 * @brief 自动模式 按键3调节颜色  亮度根据光照强度自动调节
 */
void sys_mode_Auto(void)
{
    static uint8_t ls_error_count = 0;
	static uint8_t ls_sample_tick = 0;

    if (key_Read(KEY3)) 
	{
		rgb_SetColor_Circle(&system.system_data.rgb_data.color); // 按键3 调节颜色，循环调节
		
		rgb_update(); // 更新RGB显示
	}
	
	ls_sample_tick++;

	if (ls_sample_tick >= 20)  //自动模式10ms调用一次，这里调整采样时间为200ms
	{
		ls_sample_tick = 0;
		
		 HAL_StatusTypeDef ret = ls_MeasureLight(&system.system_data.ls_data.mode,&system.system_data.ls_data.lux);  // 测量光照强度 根据环境光强自动转换模式
		
		
		if (ret == HAL_OK)
		{	
			remap_lux_to_brightness();

			rgb_Display(system.system_data.rgb_data.color,system.system_data.rgb_data.brightness);
			
			rgb_update();
			
			ls_error_count = 0;
		}
		else 
		{	
			ls_error_count++;

			if (ls_error_count >= 5)
			{
				if (ls_Reset() != HAL_OK)
				{
					ls_PowerOff();
				}
				ls_error_count = 0;
			}
		}
	}
    
}


void sys_mode_Music(void)
{
	static uint32_t last_tick;
	
	if (HAL_GetTick() - last_tick >= 50) 
	{
        last_tick = HAL_GetTick();
		
		mic_Run();

		rgb_RunInMusic();

		rgb_update();
        
    }
	
    
}



