

#ifndef __APP_MODES_H__
#define __APP_MODES_H__

#include "app_common.h"

// 系统模式工作
void sys_mode_Manual(void);
void sys_mode_Auto(void);
void sys_mode_Music(void);
void sys_mode_PowerOff(void);


//进入模式时的初始化函数
void sys_mode_Manual_Init();
void sys_mode_Auto_Init();
void sys_mode_Music_Init();


#endif