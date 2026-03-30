

#ifndef __RGB_DRV_H__
#define __RGB_DRV_H__

#include "main.h"
#include "dwt.h"

/* ---------------------------------------define---------------------------------------*/
#define RGB_LED_NUM 30           // 灯带灯珠数量
#define RGB_COLOR_NUM 10         // 灯带颜色数量

#define RGB_MAX_BRIGHTNESS 160  //定义最大亮度为140 （过大刺眼）
#define RGB_MIN_BRIGHTNESS 20   //定义最小亮度为20
#define RGB_KEY_BRIGHTNESS_STEP 25  // 按键控制亮度步长 

/* ------------------------------------variables------------------------------------ */
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
    WarmWhite,
    Yellow,
    Purple,
    Cyan,
    Orange,
    Pink ,
    Num_of_Colors
}RGB_Color_e;

typedef struct 
{
    uint32_t rgb_led_buf[RGB_LED_NUM] ; // 储存每个LED的RGB通道值（颜色+亮度）

    RGB_Color_e cnt_color; // 当前显示的颜色
    uint8_t cnt_brightness; // 当前显示的亮度
}RGB_TypeDef_t;


/* ---------------------------------------function---------------------------------------*/
void rgb_Init(void);
void rgb_Display(RGB_Color_e color,uint8_t brightness);
void rgb_PowerOff(void);
void rgb_PowerOn(RGB_Color_e color,uint8_t brightness);
void rgb_SetColor(RGB_Color_e color);
void rgb_SetBrightness(uint8_t brightness);
void rgb_Update(void);
void rgb_SetColor_Circle(RGB_Color_e* color);
void rgb_SetBrightness_Circle(uint8_t* brightness);
void rgb_SetBrightnessOn(uint8_t* brightness);
void rgb_SetBrightnessOff(uint8_t* brightness);

#endif
