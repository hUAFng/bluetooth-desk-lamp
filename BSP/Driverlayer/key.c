

#include "key.h"

key_t key;

/*
*   @brief: 按键中断回调函数
*   @param: GPIO_Pin: 按键引脚
*   @return: None
*/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    __disable_irp();
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
    __enable_irp(); 
}


/*
*   @brief: 读取某个按键的状态 + 清除标志位
*   @param: KEY: 按键枚举值
*   @return: 按键状态 ： 1 - 按键被按下过，0 - 按键未被按下过
*/
uint8_t key_Read(key_e KEY)
{
    switch(KEY)
    {
        case KEY1:
            if(key.key1_flag == 1)
            {
                key.key1_flag = 0;
                return 1;
            }
            break;
        case KEY2:
            if(key.key2_flag == 1)
            {
                key.key2_flag = 0;
                buzzer_work();
                return 1;
            }
            break;
        case KEY3:
            if(key.key3_flag == 1)
            {
                key.key3_flag = 0;
                buzzer_work();
                return 1;
            }
            break; 
        default : break;
    }

    return 0;
}
