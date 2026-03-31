
//                                      系统初始化（不工作（含手动模式初始化））
//                                            |
//                                        系统上电（工作）
//                                            |
//                                        手动模式



//                                        改变模式
//                                            | 
//                                        模式初始化
//                                           |
//                                        模式工作











#include "app_manager.h"



/**
 * @brief 系统电源控制
 * @details 按键1开启或者关闭系统
 */
void system_power_Control_key(void)
{
    if (key_Read(KEY1)) //按键按下
    {
        if (system.mode == System_Mode_PowerOff) system_PowerOn();

        else system_PowerOff();
    }
}

// 系统蓝牙、语音控制
void system_Control(void)
{ 
    system_power_Control_key(); // 按键1控制系统开关机

    BT_GetCmd(&system.system_data.bt_data.cmd); // 获取蓝牙命令
    if (system.system_data.bt_data.cmd != NoneCmd) //蓝牙有命令 - 优先处理蓝牙命令，不处理语音命令
    {
        Sys_Control_By_BTorASR(system.system_data.bt_data.cmd);
    }
    else //蓝牙没有命令
    {
        asrpro_GetCmd(&system.system_data.asr_data.cmd); // 获取语音命令

        if (system.system_data.asr_data.cmd != NoneCmd)
        {
            Sys_Control_By_BTorASR(system.system_data.asr_data.cmd);
        }
    }
}

// 系统初始化
void system_Init()
{
    system_valiable_Init(); // 系统变量初始化
    module_Init();          // 模块初始化
}    

/**
 * @brief 代指从系统模式为System_Mode_PowerOff切换到上电 或者 初始化后上电（初始化不工作）-> 手动模式
 */
void system_PowerOn(void)
{
    system_valiable_Init();

    system.mode = Sys_Mode_Manual;

    rgb_PowerOn(system.system_data.rgb_data.color,system.system_data.rgb_data.brightness); 
    tft_PowerOn(); 
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
    // 按键1、蓝牙、语音工作，可唤醒
}

// 系统运行
void system_Run(void)
{
    system_Control(); // 首先处理蓝牙、语音、按键1命令，控制系统整体模式和状态

    switch(system.mode)
    {
        case Sys_Mode_Manual:// 手动模式处理

            sys_mode_Manual();

            break;
        
        
        case Sys_Mode_Auto:// 自动模式处理

            sys_mode_Auto();
            
            break;

        case Sys_Mode_Music:// 音乐律动模式处理
            
            sys_mode_Music();

            break;

        default:
            break;
    }
}

