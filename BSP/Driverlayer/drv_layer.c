

#include "drv_layer.h"

void drv_layer_Init(void)
{
    uart_Init();
    buzzer_Init();
    led_Init();
}


