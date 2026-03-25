

#ifndef __RGB_DRV_H__
#define __RGB_DRV_H__

#include "main.h"
#include "dwt.h"


typedef struct 
{
    uint8_t green;
    uint8_t red;
    uint8_t blue;

} RGB_ColorTypeDef_t;

typedef enum
{
    Red = 0,
    Green,
    Blue,
    White,
    Black,
    Yellow,
    Purple,
    Cyan,
    Orange,
    Pink = 9
}RGB_Color_e;



/* ---------------------------------------define---------------------------------------*/
#define RGB_LED_NUM 30           // 灯带灯珠数量
#define RGB_MAX_BRIGHTNESS 255  //定义最大亮度为255
#define RGB_MIN_BRIGHTNESS 25  //定义最小亮度为25
#define RGB_COLOR_NUM 10         // 灯带颜色数量



/* ---------------------------------------function---------------------------------------*/
void rgb_Display(RGB_Color_e color,uint8_t brightness);


#endif

