


#ifndef __ASRPRO_DRV_H__
#define __ASRPRO_DRV_H__ 

#include "main.h"
#include "uart_drv.h"
#include "string.h"
#include "buzzer_drv.h"
#include "cmd.h"
/*--------------------------------------------define--------------------------------------------*/
#define ASR_UART_Handle huart1
#define ASR_RX_CMD_DURATION 2000  // 2s之后允许接收处理一次命令

#define ASR_FRAME_LEN 2  //指令两字节

#define ASR_WAKEUP_WAIT_TIME 15 // asrpro在唤醒后无命令到休眠的时间(s) 需要在软件中设置
/*--------------------------------------------variables--------------------------------------------*/


typedef struct 
{
    uint8_t asr_rx_buf[ASR_FRAME_LEN * 2]; // 串口中断接收缓冲区（防止溢出）
    Cmd_e asr_rx_cmd; // 接收命令
    uint8_t asr_rx_flag; // 接收标志 0：未接收 1：已接收
    uint32_t prev_cmd_time; // 上一次接收命令时间 （ms） 用于处理消抖 初始为最大值
}asr_rx_t;

extern UART_HandleTypeDef huart1;

/* --------------------------------------------functions--------------------------------------------*/
void asrpro_GetCmd(Cmd_e* cmd);
void asrpro_Init(void);

#endif

