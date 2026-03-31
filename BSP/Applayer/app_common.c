

#include "app_common.h"

System_t system ;


// 系统变量初始化
void system_valiable_Init(void)
{
    system = (System_t)
    {
        .mode = System_Mode_PowerOff, // 默认初始模式为关机

        .system_data = 
        {
            .rgb_data = 
            {
                .brightness = 80, // 默认初始亮度
                .color = WarmWhite, // 默认初始颜色为暖白
                .filter_lux = 0.3f,    // 默认初始滤波系数
            },

            .ls_data = 
            {
                .mode = LS_MODE_HRES1,
                .lux = 10 // 默认初始光照强度，实际应用中可根据环境调整
            },
        },
        
    };

    return;
}