

#ifndef __APP_COMMON_H__
#define __APP_COMMON_H__

#include "module_layer.h"
#include "app_ui.h"

/* --------------------------------------variables-------------------------------------- */

/*------------------------------------------------------------*/
/*----------------------模块数据结构体--------------------------*/
// 灯带数据
typedef struct
{
    uint8_t brightness;
    RGB_Color_e color;

    float filter_lux; // （自动模式下）滤波系数，0-1，需要根据实际情况调整,越小响应越慢也越平滑

}rgb_data_t;

// 光照传感器数据
typedef struct
{
    LS_MODE mode;
    float lux;
}LightSensor_data_t;

// 蓝牙数据
typedef struct
{
    Cmd_e cmd;
}bt_data_t;

// 语音数据
typedef struct
{
    Cmd_e cmd;
}asr_data_t;


// 用于判断命令来源的枚举 在system_Control中使用
typedef enum 
{
    BT_Cmd = 0,
    Asr_Cmd = 1,
}Cmd_Type_e;

/*------------------------------------------------------------*/
/*------------------------------系统--------------------------*/
// 系统模式枚举
typedef enum
{
    Sys_Mode_Manual = 0, // 手动模式（按键模式）
    Sys_Mode_Auto = 1, // 自动模式(传感器模式)
    Sys_Mode_Music = 2, // 音乐律动模式

    Sys_Mode_PowerOff = 3 // 灯带灭，蓝牙、语音可唤醒，TFT熄灭，传感器不工作，麦克风不工作
}system_mode_e;

// 系统数据
typedef struct 
{
    rgb_data_t rgb_data;
    LightSensor_data_t ls_data;
    bt_data_t bt_data;
    asr_data_t asr_data;
}system_data_t;


// 系统最终结构体
typedef struct
{
    system_mode_e mode;
    system_data_t system_data;
}System_t;

extern System_t system;


void system_valiable_Init(void);

#endif