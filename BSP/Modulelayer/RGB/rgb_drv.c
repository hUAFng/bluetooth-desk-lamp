


#include "rgb_drv.h"

/* --------------------------------  颜色的RGB通道值 -------------------------------- */

static const RGB_ColorTypeDef_t rgb_color[] =  // GRB顺序
{
    {  0, 255,  0 }, // RED     
    {255,   0,  0 }, // GREEN   
    {  0,   0, 255 }, // BLUE    
    {255, 255, 255 }, // WHITE   
    {  0,   0,  0 }, // BLACK   
    {255, 255,  0 }, // YELLOW  
    {  0, 255, 255 }, // PURPLE  紫色 
    {255,   0, 255 }, // CYAN    青色 
    {128, 255,  0 }, // ORANGE  
    { 60, 255, 180 }, // PINK   
};


static uint32_t rgb_led_buf[RGB_LED_NUM]; // 储存每个LED的RGB通道值（颜色+亮度）

/*-------------------------------------function---------------------------------------*/
void rgb_WriteBit(uint8_t bit) //只看最低位 （灯带的时序逻辑）
{
    __disable_irq();  // 关闭中断 保护时序

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

    __enable_irq();   // 恢复中断
}

void rgb_WriteByte(uint8_t byte)
{
    for (uint8_t i = 0;i < 8;i++) rgb_WriteBit(byte >> (7-i));
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

    rgb_led_buf[led_index] = temp_led_color_buf.green << 16 | temp_led_color_buf.red << 8 | temp_led_color_buf.blue;
}


void rgb_SetAllLed(RGB_Color_e color,uint8_t brightness)
{
    for (uint8_t i = 0;i < RGB_LED_NUM;i++) rgb_SetSingleLed(i,color,brightness);
}


/** 
* @brief 灯带的设置与显示
*/
void rgb_Display(RGB_Color_e color,uint8_t brightness)
{
    if (color >= RGB_COLOR_NUM) return;

    rgb_SetAllLed(color,brightness);     // 设置参数   

    for (uint8_t i = 0;i < RGB_LED_NUM;i++) // 发送数据
    {
        rgb_WriteByte((rgb_led_buf[i] >> 16) & 0xFF);
        rgb_WriteByte((rgb_led_buf[i] >> 8) & 0xFF);
        rgb_WriteByte(rgb_led_buf[i] & 0xFF);
    }

    HAL_GPIO_WritePin(RGB_GPIO_Port,RGB_Pin,GPIO_PIN_RESET); // 低电平复位持续1ms（刷新显示）
    HAL_Delay(1);
}


