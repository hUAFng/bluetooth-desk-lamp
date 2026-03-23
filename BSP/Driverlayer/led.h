

#ifndef __LED_H__
#define __LED_H__

#include "main.h"


typedef enum 
{
    LED_G_ON = 0,
    LED_G_OFF , 
    LED_B_ON,
    LED_B_OFF
}led_e;

void led_work(led_e led_num);

#endif
