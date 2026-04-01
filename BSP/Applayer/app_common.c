

#include "app_common.h"

System_t system ;


// 系统变量初始化
void system_valiable_Init(void)
{
    system = (System_t)
    {
        .mode = Sys_Mode_PowerOff, // 默认初始模式为关机

        .system_data = 
        {
            .rgb_data = 
            {
                .brightness = 80, // 默认初始亮度
                .color = WarmWhite, // 默认初始颜色为暖白
                .filter_lux = 0.7f,    // 默认的滤波系数
            },

            .ls_data = 
            {
                .mode = LS_MODE_HRES1,
                .lux = 50.0f
            },
        },
        
    };

    return;
}