

#include "module_layer.h"

void module_Init(void)
{
    drv_layer_Init();

    asrpro_Init();

    if (BT_Init() != HAL_OK)
    {
        BT_Reset();
    }  

    if (ls_Init())
    {
        ls_Reset();
    }

    // mic_Init();
    
    rgb_Init();

    tft_Init();

}

