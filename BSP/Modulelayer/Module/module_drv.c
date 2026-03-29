

#include "module_drv.h"

void module_Init(void)
{
    asrpro_Init();

    BT_Init();

    ls_Init();

    mic_Init();
    
    rgb_Init();

    tft_Init();

}

