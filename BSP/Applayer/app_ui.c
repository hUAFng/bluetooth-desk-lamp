

#include "app_ui.h"


void system_show_Init()
{
    tft_DisplayString(2,2,"Mode : Manual",COLOR_WHITE,COLOR_BLACK);
    tft_DrawLine(2,6,240,6,COLOR_WHITE);

}


// 显示亮度、颜色
void system_show_manual_auto(void)
{

    const char color_str[][10] = { "Red", "Green", "Blue", "White", "WarmWhite","Yellow","Purple", "Cyan", "Orange", "Pink" } ;

    uint16_t bright = system.system_data.rgb_data.brightness * 100 / RGB_MAX_BRIGHTNESS;

    uint8_t str1[30];
    sprintf(str1,"Brightness : %d%%",bright);
    tft_DisplayString(2,10,str1,COLOR_WHITE,COLOR_BLACK);

    uint8_t str2[20];
    sprintf(str2,"Color : %s",color_str[system.system_data.rgb_data.color]);
    tft_DisplayString(2,20,str2,COLOR_WHITE,COLOR_BLACK);

}

void system_show_music(void)
{
    tft_DisplayString(2,20,"Enjoy Music!",COLOR_WHITE,COLOR_BLACK);
}



void system_show(void)
{
    if (system.mode == Sys_Mode_PowerOff) return ; // 系统在工作模式才显示
    
    static system_mode_e mode_prev = Sys_Mode_Manual;

    if (mode_prev != system.mode)
    {
        tft_ClearScreen(COLOR_BLACK);

        mode_prev = system.mode;

        if (system.mode == Sys_Mode_Manual)   tft_DisplayString(2,2,"Mode : Manual",COLOR_WHITE,COLOR_BLACK);
        else if (system.mode == Sys_Mode_Auto)   tft_DisplayString(2,2,"Mode : Auto",COLOR_WHITE,COLOR_BLACK);
        else if (system.mode == Sys_Mode_Music)   tft_DisplayString(2,2,"Mode : Music",COLOR_WHITE,COLOR_BLACK);

        tft_DrawLine(2,6,240,6,COLOR_WHITE);
    }

    switch(system.mode)
    {
        case Sys_Mode_Manual:
            system_show_manual_auto();
            break;

        case Sys_Mode_Auto:
            system_show_manual_auto();
            break;
            
        case Sys_Mode_Music:
            system_show_music();
            break;

        default:
            break;

    }

    
}