

#include "app_manager.h"


/**
 * @brief 代指从系统模式为System_Mode_PowerOff切换到上电
 */
void system_PowerOn(void)
{
    system.mode = Sys_Mode_Manual;

    rgb_PowerOn(); 
    tft_PowerOn();
    ls_PowerOn();
    
}


/**
 * @brief 代指从系统模式为工作模式切换到关机，可由蓝牙或者语音、按键1唤醒
 */
void system_PowerOff(void)
{
    system.mode = System_Mode_PowerOff;

    rgb_PowerOff(); // 灯带关闭
    tft_PowerOff(); // TFT关闭
    ls_PowerOff();  // 光线传感器关闭
    mic_PowerOff(); // 麦克风关闭

    // 蓝牙、语音工作，可唤醒
}

void system_power_Control(void)
{
    key_e key = key_Read();

    if (key == KEY1_ON)
    {
        if (system.mode == System_Mode_PowerOff) system_PowerOn();
        else system_PowerOff();
    }
}

   