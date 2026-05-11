

#include "app_ui.h"

static uint8_t last_brightness = 255;  // 非法初始值，首次必定刷新
static uint8_t last_color = 255;

// 动态刷新区域范围（标题栏以下） 
#define DYNAMIC_Y_START 30
#define DYNAMIC_Y_END   127


/**
 * @brief 绘制界面静态框架（外框、标题栏）
 * @param title 要显示的标题字符串
 */
static void draw_frame(const char *title)
{
    // 上线
    tft_DrawLine(2, 1, 125, 1, COLOR_WHITE);
    // 标题
    tft_DisplayString(4, 12, title, COLOR_WHITE, COLOR_BLACK);
    // 下线
    tft_DrawLine(2, 27, 125, 27, COLOR_WHITE);
}


/**
 * @brief 清除动态区域（用黑色填充）
 */
static void clear_dynamic_area(void)
{
    tft_FillRect(0, DYNAMIC_Y_START, 128, DYNAMIC_Y_END - DYNAMIC_Y_START + 1, COLOR_BLACK);
}



/**
 * @brief 将系统颜色枚举转换为实际RGB值
 */
static uint16_t get_rgb565(RGB_Color_e c)
{
    switch (c) 
	{
        case Red:       return COLOR_RED;
        case Green:     return COLOR_GREEN;
        case Blue:      return COLOR_BLUE;
        case White:     return COLOR_WHITE;
        case WarmWhite: return COLOR_YELLOW;   // 暖白用黄色近似
        case Yellow:    return COLOR_YELLOW;
        case Purple:    return COLOR_MAGENTA;
        case Cyan:      return COLOR_CYAN;
        case Orange:    return COLOR_ORANGE;
        case Pink:      return COLOR_MAGENTA;  // 品红近似粉色
        default:        return COLOR_WHITE;
    }
}


/**
 * @brief 绘制亮度进度条（x:2, y:52, 宽:100, 高:8）
 */
static void draw_progress_bar(uint8_t percent)
{
    // 背景条
    tft_FillRect(2, 52, 100, 8, COLOR_GRAY);
	
    // 前景条（百分比映射到宽度）
    uint16_t fill_w = (uint16_t)(percent * 100 / 100); // 直接使用 percent，范围 0~100
    fill_w = fill_w > 100 ? 100 : fill_w;
	
    if (fill_w > 0) 
	{
        tft_FillRect(2, 52, fill_w, 8, COLOR_WHITE);
    }
}

/**
 * @brief 显示蓝牙连接状态
 * @param y 起始Y坐标
 */
static void system_show_bluetooth_status(uint16_t y)
{
    uint8_t is_connected = BT_IsConnected();
    uint16_t square_color = is_connected ? COLOR_GREEN : COLOR_RED;
    const char* status_str = is_connected ? "Y" : "N";

    
    uint8_t square_x = 8; // 色块起始x
    
    uint8_t text_x = square_x + 16 + 4;  // 字体起始x

    tft_FillRect(square_x, y, 16, 16, square_color);
    tft_DrawLine(square_x, y, square_x + 15, y, COLOR_WHITE);
    tft_DrawLine(square_x, y + 15, square_x + 15, y + 15, COLOR_WHITE);
    tft_DrawLine(square_x, y, square_x, y + 15, COLOR_WHITE);
    tft_DrawLine(square_x + 15, y, square_x + 15, y + 15, COLOR_WHITE);

    tft_DisplayString(text_x, y, "BT : ", COLOR_WHITE, COLOR_BLACK);
    tft_DisplayString(text_x + 5 * 8, y, status_str, COLOR_WHITE, COLOR_BLACK);
}


void system_show_Init(void)
{
   
    tft_ClearScreen(COLOR_BLACK);
    draw_frame(" MODE: MANUAL");
	
    last_brightness = 255;
    last_color = 255;
}

void system_show_manual_auto(void)
{
    uint8_t bright = system.system_data.rgb_data.brightness * 100 / RGB_MAX_BRIGHTNESS;
    uint8_t color_idx = (uint8_t)system.system_data.rgb_data.color;

    if (bright == last_brightness && color_idx == last_color) 
	{
		system_show_bluetooth_status(105);

		return;  //参数不变时仅判断蓝牙状态
	}
	
    last_brightness = bright;
    last_color = color_idx;

    // 清空动态区
    clear_dynamic_area();

    // 亮度 + 进度条
    char buf1[16];
    sprintf(buf1, "BRI: %d%%", bright);
    tft_DisplayString(4, 35, buf1, COLOR_WHITE, COLOR_BLACK);
    draw_progress_bar(bright);

    // 颜色文本 + 颜色方块
    const char *color_name[] = {
        "Red", "Green", "Blue", "White", "Warm",
        "Yellow", "Purple", "Cyan", "Orange", "Pink"
    };
    char buf2[20];
    sprintf(buf2, "COL: %s", color_name[color_idx]);
    tft_DisplayString(4, 70, buf2, COLOR_WHITE, COLOR_BLACK);

    uint16_t rgb = get_rgb565(color_idx);
    tft_FillRect(98, 68, 25, 25, rgb);
	
    // 方块边框（白色）
    tft_DrawLine(98, 68, 122, 68, COLOR_WHITE);
    tft_DrawLine(98, 93, 122, 93, COLOR_WHITE);
    tft_DrawLine(98, 68, 98, 93, COLOR_WHITE);
    tft_DrawLine(122, 68, 122, 93, COLOR_WHITE);
}

void system_show_music(void)
{
	if (system.prev_mode == system.mode) return;
	
    clear_dynamic_area();

    tft_DisplayString(28, 32, "ENJOY MUSIC", COLOR_WHITE, COLOR_BLACK);
	
	system_show_bluetooth_status(50);

    // 5条不同高度的竖线
    uint16_t bar_x[] = {20, 42, 64, 86, 108};
    uint16_t bar_h[] = {15, 30, 22, 35, 18};
	
    for (int i = 0; i < 5; i++) 
	{
        uint16_t x = bar_x[i];
        uint16_t top = 115 - bar_h[i];   
        tft_DrawLine(x, top, x, 114, COLOR_CYAN);
        tft_DrawLine(x+1, top, x+1, 114, COLOR_CYAN);
    }
}


void system_show(void)
{
    if (system.mode == Sys_Mode_PowerOff) return;

    static system_mode_e mode_prev = Sys_Mode_Manual;

    // 模式切换时，重绘整个界面框架
    if (mode_prev != system.mode) 
	{
        tft_ClearScreen(COLOR_BLACK);
        mode_prev = system.mode;

        const char *title;
        switch (system.mode) 
		{
            case Sys_Mode_Manual: title = " MODE: MANUAL"; break;
            case Sys_Mode_Auto:   title = " MODE: AUTO";   break;
            case Sys_Mode_Music:  title = " MODE: MUSIC";  break;
            default:              title = " MODE: ----";   break;
        }
        draw_frame(title);
		
        last_brightness = 255;
        last_color = 255;
    }

    switch (system.mode) {
        case Sys_Mode_Manual:
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
