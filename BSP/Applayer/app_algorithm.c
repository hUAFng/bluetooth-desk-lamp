

#include "app_algorithm.h"


/**
 * @brief 映射光照强度到灯带亮度 低通滤波
 */
void remap_lux_to_brightness(void)
{
    uint8_t brightness = system.system_data.rgb_data.brightness;
    uint16_t lux = system.system_data.ls_data.lux;
    float brightness_filter = system.system_data.rgb_data.filter_lux;

    brightness = (uint8_t)(brightness_filter*(lux * RGB_MAX_BRIGHTNESS / LS_LUX_MAX) + (1-brightness_filter)*brightness);

    system.system_data.rgb_data.brightness = brightness;
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
                rgb_SetColor_Circle(&system.system_data.rgb_data.color); // 改变颜色(循环切换)
                break;
            case CMD_RGB_LIGHT_ON:
                rgb_SetBrightnessOn(&system.system_data.rgb_data.brightness); // 处理灯带亮一点命令
                break;
            case CMD_RGB_LIGHT_OFF:
                rgb_SetBrightnessOff(&system.system_data.rgb_data.brightness); // 处理灯带暗一点命令
                break;
            default:
                break;
        }

    }
}