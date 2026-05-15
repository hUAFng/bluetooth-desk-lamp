#ifndef __RGB_DRV_H__
#define __RGB_DRV_H__

#include "main.h"
#include "mic_drv.h"
#include "tim.h"

/* ---------------------------------------define---------------------------------------*/
#define RGB_LED_NUM 10
#define RGB_COLOR_NUM 10
#define RGB_MAX_BRIGHTNESS 180
#define RGB_MIN_BRIGHTNESS 20
#define RGB_BRIGHTNESS_STEP 40
#define RGB_AUTOMODE_BRIGHTNESS_FILTER 0.83f

#define RGB_MAX_BRIGHTNESS_MUSIC 255
#define RGB_MIN_BRIGHTNESS_MUSIC 10
#define RGB_MUSIC_BRIGHT_LOW_AREA 77

#define MUSIC_COLOR_SMOOTH_UP    0.50f
#define MUSIC_COLOR_SMOOTH_DOWN  0.88f

#define MUSIC_BRIGHT_ATTACK_FILTER   0.40f
#define MUSIC_BRIGHT_RELEASE_FILTER  0.80f

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
    uint32_t rgb_led_buf[RGB_LED_NUM];
	uint16_t pwm_pulse_dma_buf[RGB_LED_NUM * 24 + 50];
    uint8_t is_sending;
	
	RGB_Color_e cnt_color;
    uint8_t cnt_brightness;

    uint8_t music_target_r;
    uint8_t music_target_g;
    uint8_t music_target_b;
    uint8_t music_smooth_r;
    uint8_t music_smooth_g;
    uint8_t music_smooth_b;
}RGB_TypeDef_t;


/* ---------------------------------------function---------------------------------------*/
void rgb_Init(void);
void rgb_Display(RGB_Color_e color,uint8_t brightness);
void rgb_PowerOff(void);
void rgb_PowerOn(RGB_Color_e color,uint8_t brightness);
void rgb_SetColor(RGB_Color_e color);
void rgb_SetBrightness(uint8_t brightness);
void rgb_SetColor_Circle(RGB_Color_e* color);
void rgb_SetBrightness_Circle(uint8_t* brightness);
void rgb_SetBrightnessUp(uint8_t* brightness);
void rgb_SetBrightnessDown(uint8_t* brightness);
void rgb_update(void);

void rgb_RunInMusic(void);

#endif
