

#include "app_common.h"

System_t system ;


// 系统变量初始化
void system_valiable_Init(void)
{
    system = (System_t)
    {
        .mode = Sys_Mode_PowerOff, // 默认初始模式为关机
		.prev_mode = Sys_Mode_PowerOff,

        .system_data = 
        {
            .rgb_data = 
            {
                .brightness = 20 , // 默认初始亮度
                .color = Red, // 默认颜色
            },
        },
        
    };

    return;
}