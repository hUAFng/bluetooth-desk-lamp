


#include "rgb_drv.h"

/* --------------------------------  颜色的RGB通道值 -------------------------------- */

static const RGB_ColorTypeDef_t rgb_color[] =  // GRB顺序
{
    {  0, 255,  0 }, // RED     
    {255,   0,  0 }, // GREEN   
    {  0,   0, 255 }, // BLUE    
    {255, 255, 255 }, // WHITE   
    {255, 223, 186}, // WARM_WHITE 暖白 
    {255, 255,  0 }, // YELLOW  
    {  0, 255, 255 }, // PURPLE  紫色 
    {255,   0, 255 }, // CYAN    青色 
    {128, 255,  0 }, // ORANGE  
    { 60, 255, 180 }, // PINK   
};

static RGB_TypeDef_t rgb = {{0},0,0};
/*-------------------------------------function---------------------------------------*/

void rgb_ClearBuffer(void)
{
    for (uint8_t i = 0; i < RGB_LED_NUM; i++)
    {
        rgb.rgb_led_buf[i] = 0;
    }
}


/**
* @brief 灯带初始化，默认关闭状态，需按下按键启动灯带
*/
void rgb_Init(void)
{
    HAL_GPIO_WritePin(RGB_GPIO_Port, RGB_Pin, GPIO_PIN_RESET);

    rgb_ClearBuffer();
    rgb_PowerOff();
}

void rgb_WriteBit(uint8_t bit) //只看最低位 （灯带的时序逻辑）
{

    if(bit & 0x01) // 1
    {
        HAL_GPIO_WritePin(RGB_GPIO_Port,RGB_Pin,GPIO_PIN_SET);
        delay_us(0.8);
        HAL_GPIO_WritePin(RGB_GPIO_Port,RGB_Pin,GPIO_PIN_RESET);
        delay_us(0.45);
    }
    else  // 0
    {
        HAL_GPIO_WritePin(RGB_GPIO_Port,RGB_Pin,GPIO_PIN_SET);
        delay_us(0.4);
        HAL_GPIO_WritePin(RGB_GPIO_Port,RGB_Pin,GPIO_PIN_RESET);
        delay_us(0.85);
    }
}

void rgb_WriteByte(uint8_t byte)
{
    __disable_irq();

    for (uint8_t i = 0;i < 8;i++) rgb_WriteBit((byte >> (7-i)) & 0x01);

    __enable_irq();
}

/**
* @brief 关闭灯带
*/
void rgb_PowerOff(void)
{
    __disable_irq(); // 关闭全局中断，保护时序完整
    
    for (uint8_t i = 0; i < RGB_LED_NUM; i++)
    {
        rgb_WriteByte(0);
        rgb_WriteByte(0);
        rgb_WriteByte(0);
    }
    __enable_irq(); // 恢复全局中断
}

/**
 * @brief 设置单一LED状态（包括颜色和亮度）
 * @param led_index LED索引
 * @param color RGB颜色
 * @param brightness 亮度
 * @note: 修改亮度使用位运算，>>8相当于/256，相比于直接使用浮点可提高计算效率
 */
void rgb_SetSingleLed(uint8_t led_index , RGB_Color_e color,uint8_t brightness)
{
    if (led_index >= RGB_LED_NUM) return;
    if (brightness < RGB_MIN_BRIGHTNESS) brightness = RGB_MIN_BRIGHTNESS;
    else if (brightness > RGB_MAX_BRIGHTNESS) brightness = RGB_MAX_BRIGHTNESS;

    uint8_t color_index = color;

    /* 调节亮度 */
    RGB_ColorTypeDef_t temp_led_color_buf;
    temp_led_color_buf.green = (uint8_t)((rgb_color[color_index].green * brightness) >> 8);
    temp_led_color_buf.red = (uint8_t)((rgb_color[color_index].red * brightness) >> 8);
    temp_led_color_buf.blue = (uint8_t)((rgb_color[color_index].blue * brightness) >> 8);

    rgb.rgb_led_buf[led_index] = temp_led_color_buf.green << 16 | temp_led_color_buf.red << 8 | temp_led_color_buf.blue;
}


void rgb_SetAllLed(RGB_Color_e color,uint8_t brightness)
{
    rgb.cnt_color = color;
    rgb.cnt_brightness = brightness;

    for (uint8_t i = 0;i < RGB_LED_NUM;i++) rgb_SetSingleLed(i,color,brightness);
}


/**
 * @brief 刷新显示 - 依赖当前rgb_led_buf中的数据
 */
void rgb_Update(void)
{
    
    for (uint8_t i = 0; i < RGB_LED_NUM; i++)
    {
        rgb_WriteByte((rgb.rgb_led_buf[i] >> 16) & 0xFF);
        rgb_WriteByte((rgb.rgb_led_buf[i] >> 8) & 0xFF);
        rgb_WriteByte(rgb.rgb_led_buf[i] & 0xFF);
    }
}

/** 
* @brief 灯带的设置与显示
*/
void rgb_Display(RGB_Color_e color,uint8_t brightness)
{
    if (color >= RGB_COLOR_NUM) return;

    rgb_SetAllLed(color,brightness);     // 设置参数  

    rgb_Update();                         // 刷新显示

    HAL_GPIO_WritePin(RGB_GPIO_Port,RGB_Pin,GPIO_PIN_RESET); // 低电平复位持续1ms（刷新显示）
    
    HAL_Delay(1);
}

/**
 * @brief 仅设置所有LED的亮度
 * @param brightness 亮度值（0-255）
 */
void rgb_SetBrightness(uint8_t brightness)
{
    rgb_SetAllLed(rgb.cnt_color,brightness);
}

/** 
 * @brief 循环调节亮度
 */
void rgb_SetBrightness_Circle(uint8_t* brightness)
{
    if (brightness == NULL) return;
    
    *brightness += RGB_KEY_BRIGHTNESS_STEP;

    // 循环调整亮度
    if (*brightness > RGB_MAX_BRIGHTNESS) *brightness = RGB_MAX_BRIGHTNESS;
    else if (*brightness == RGB_MAX_BRIGHTNESS) *brightness = RGB_MIN_BRIGHTNESS;

    rgb.cnt_brightness = *brightness;

    rgb_SetBrightness(rgb.cnt_brightness);
}


// 调亮亮度
void rgb_SetBrightnessOn(uint8_t* brightness)
{
    if (brightness == NULL) return;

    *brightness += RGB_KEY_BRIGHTNESS_STEP; 

    if (*brightness > RGB_MAX_BRIGHTNESS) *brightness = RGB_MAX_BRIGHTNESS;

    rgb.cnt_brightness = *brightness;
    
    rgb_SetBrightness(rgb.cnt_brightness);
}

// 调暗亮度
void rgb_SetBrightnessOff(uint8_t* brightness)
{
    if (brightness == NULL) return;

    *brightness -= RGB_KEY_BRIGHTNESS_STEP;
    
    if (*brightness < RGB_MIN_BRIGHTNESS) *brightness = RGB_MIN_BRIGHTNESS;

    rgb.cnt_brightness = *brightness;

    rgb_SetBrightness(rgb.cnt_brightness);
}

/**
 * @brief 仅设置所有LED的颜色
 * @param color 颜色枚举值
 */
void rgb_SetColor(RGB_Color_e color)
{
    rgb_SetAllLed(color,rgb.cnt_brightness);
}


/**
 * @brief 循环调节颜色
 */
void rgb_SetColor_Circle(RGB_Color_e* color)
{
    if (color == NULL) return;
    (*color)++;
    
    // 循环调整颜色
    if (*color >= RGB_COLOR_NUM) *color = 0;

    rgb.cnt_color = *color;
    rgb_SetColor(rgb.cnt_color);
}

void rgb_PowerOn(RGB_Color_e color,uint8_t brightness)
{
    rgb_ClearBuffer();
    rgb_Display(color,brightness);
}


/*------------------------------------------------------------------------------
                                    音乐模式
------------------------------------------------------------------------------*/


/*note : 音乐模式的颜色列举，分三个档次，低频中频高频，对应这三个颜色范围 */

// 低频区
const uint8_t LOW_MIN_COLOR[3] = {  64,   0,   0 }; // 暗红
const uint8_t LOW_MAX_COLOR[3] = { 255, 165,   0 }; // 亮橙

// 中频区
const uint8_t MID_MIN_COLOR[3] = {  0,  128,   0 }; // 暗绿
const uint8_t MID_MAX_COLOR[3] = {255, 255,   0 }; // 亮黄

// 高频区
const uint8_t HIGH_MIN_COLOR[3] = {  0,   0, 128 }; // 暗蓝
const uint8_t HIGH_MAX_COLOR[3] = {128,   0, 128 }; // 亮紫


void rgb_brightness_filter(uint8_t *brightness)
{
    if (brightness == NULL) return;

    int16_t delta_bright = *brightness - rgb.cnt_brightness;

    float Up_filter = 0.6f;
    float Down_filter = 0.85f;

    if (delta_bright > 0)
    {
        rgb.cnt_brightness = (uint8_t)(Up_filter * rgb.cnt_brightness + (1 - Up_filter) * *brightness);
    }
    else 
    {
        rgb.cnt_brightness = (uint8_t)(Down_filter * rgb.cnt_brightness + (1 - Down_filter) * *brightness);
    }


}
void rgb_MapFreqToRGBValue()
{
    float freq = 0.0f;
    mic_GetFreq(&freq);

    if (freq == 0.0f) return;

    const uint8_t* min_color_ptr;
    const uint8_t* max_color_ptr; // 用于定位当前频率下RGB对应的颜色区间
    
    if(freq <= LOW_FREQ_THRESHOLD) // 判断当前频率所属的颜色区间
    {
        min_color_ptr = LOW_MIN_COLOR;
        max_color_ptr = LOW_MAX_COLOR;
    }
    else if(freq <= MID_FREQ_THRESHOLD)
    {
        min_color_ptr = MID_MIN_COLOR;
        max_color_ptr = MID_MAX_COLOR;
    }
    else 
    {
        min_color_ptr = HIGH_MIN_COLOR;
        max_color_ptr = HIGH_MAX_COLOR;
    }

    uint8_t delta_r = max_color_ptr[0] - min_color_ptr[0];   // 计算通道的长度
    uint8_t delta_g = max_color_ptr[1] - min_color_ptr[1];
    uint8_t delta_b = max_color_ptr[2] - min_color_ptr[2];

    // 通过当前的亮度来决定RGB最终的value 
    // note: >>8 = /256 ,位操作，避免浮点数计算，因为FFT比较耗时
    uint8_t r = min_color_ptr[0] + ((delta_r * rgb.cnt_brightness) >> 8);  
    uint8_t g = min_color_ptr[1] + ((delta_g * rgb.cnt_brightness) >> 8);
    uint8_t b = min_color_ptr[2] + ((delta_b * rgb.cnt_brightness) >> 8);

    // 写入LED缓冲区
    for (uint8_t i = 0; i < RGB_LED_NUM; i++)
    {
        rgb.rgb_led_buf[i] = (g << 16) | (r << 8) | b;
    }
}

// 音乐模式下仅调用这个函数
void rgb_RunInMusic(void)
{
    uint8_t cnt_bright = 0;

    // 获取当前应该显示的亮度
    mic_loudness_mapto_brightness(RGB_MAX_BRIGHTNESS_MUSIC,RGB_MIN_BRIGHTNESS,&cnt_bright,RGB_MUSIC_BRIGHT_LOW_AREA);
    rgb_brightness_filter(&cnt_bright); // 放入滤波

    rgb_MapFreqToRGBValue(); // 按照当前的频率以及亮度写入对应的RGB值

    rgb_Update(); // 刷新显示
}