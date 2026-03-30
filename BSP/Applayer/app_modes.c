

// note : 此文件为系统模式工作相关的函数，包括手动模式和自动模式、音乐模式，由app_manager.c调用

#include "app_modes.h"



void sys_mode_Manual_Init() 
{
    
}

void sys_mode_Auto_Init()
{
}

void sys_mode_Music_Init()
{

}



/**
 * @brief 手动模式 仅使用按键3和按键2调节颜色和亮度
 */
void sys_mode_Manual(void)
{
    key_e key = key_read();

    if (key == KEY2_ON) // 按键2 调节亮度，循环调节
    {
        rgb_SetBrightness_Circle(&system.system_data.rgb_data.brightness);
    }
    else if (key == KEY3_ON) // 按键3 调节颜色，循环调节
    {
        rgb_SetColor_Circle(&system.system_data.rgb_data.color);
    }

    rgb_Update();
}


/**
 * @brief 自动模式 按键3调节颜色  亮度根据光照强度自动调节
 */
void sys_mode_Auto(void)
{
    
    if (key_read() == KEY3_ON) rgb_SetColor_Circle(&system.system_data.rgb_data.color); // 按键3 调节颜色，循环调节

    if(ls_MeasureLight(&system.system_data.ls_data.mode,&system.system_data.ls_data.lux) == HAL_OK) // 测量光照强度 根据环境光强自动转换模式
    {
        remap_lux_to_brightness();

        rgb_Display(system.system_data.rgb_data.color,system.system_data.rgb_data.brightness);
    }
}


void sys_mode_PowerOff(void)
{
    system_PowerOff();
}




