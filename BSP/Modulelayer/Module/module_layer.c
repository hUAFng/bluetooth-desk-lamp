

#include "module_layer.h"

void module_Init(void)
{
    drv_layer_Init();

    asrpro_Init();

    BT_Init();

    ls_Init();

    mic_Init();
    
    rgb_Init();

    tft_Init();

}

