

#ifndef __CMD_H__
#define __CMD_H__

#include "main.h"

typedef enum
{
    CMD_POWER_ON = 0,  // "开灯"
    CMD_POWER_OFF, // "关灯"
    CMD_ManualMode, // "手动模式"
    CMD_AutoMode, // "自动模式"
    CMD_MusicMode, // "音乐模式"
    CMD_CHANGE_COLOR, // "改变颜色"
    CMD_RGB_LIGHT_ON, // "亮一点"
    CMD_RGB_LIGHT_OFF, // "暗一点"

    NoneCmd // 无命令

}Cmd_e;  // 全局命令枚举类型

#define CMD_LEN 8

#endif