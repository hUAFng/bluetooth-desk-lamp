

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
    KEY1 = 0,
    KEY2 = 1,
    KEY3 = 2,
}key_e;


uint8_t key_Read(key_e KEY);

#endif


