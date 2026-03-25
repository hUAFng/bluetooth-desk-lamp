

#include "key.h"

key_t key;

/*
*   @brief: 按键中断回调函数
*   @param: GPIO_Pin: 按键引脚
*   @return: None
*/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == Key1_Pin)
    {
        key.key1_flag = 1;
    }
    else if (GPIO_Pin == Key2_Pin)
    {
        key.key2_flag = 1;
    }
    else if (GPIO_Pin == Key3_Pin)
    {
        key.key3_flag = 1;
    }
}


/*
*   @brief: 读取按键状态 + 清除标志位
*   @param: None
*   @return: 按键状态
*/
key_e key_read(void)
{
    if (key.key1_flag == 1)
    {
        key.key1_flag = 0;
        return KEY1_ON;
    }
    else if (key.key2_flag == 1)
    {
        key.key2_flag = 0;
        return KEY2_ON;
    }
    else if (key.key3_flag == 1)
    {
        key.key3_flag = 0;
        return KEY3_ON;
    }
    else
    {
        return KEY_NONE_ON;
    }
}

