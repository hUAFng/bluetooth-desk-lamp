

#ifndef __KEY_H__
#define __KEY_H__

#include "main.h"

/* 按键按下标志位 - 0: 未按下, 1: 已按下 */
typedef struct 
{
    uint8_t key1_flag;
    uint8_t key2_flag;
    uint8_t key3_flag;
}key_t;

typedef enum 
{
    KEY_NONE_ON = 0,
    KEY1_ON = 1,
    KEY2_ON = 2,
    KEY3_ON = 3,
}key_e;


key_e key_read(void);

#endif


