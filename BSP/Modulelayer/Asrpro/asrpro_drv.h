


#ifndef __ASRPRO_DRV_H__
#define __ASRPRO_DRV_H__ 

#include "main.h"
#include "uart_drv.h"

/*--------------------------------------------define--------------------------------------------*/
#define ASR_UART_Handle huart1
#define ASR_RX_BUF_SIZE 10
#define ASR_RX_CMD_DURATION 2000  // 2s允许接收处理一次命令

// 语音识别帧长度与格式 格式：帧头1+帧头2+数据+校验+帧尾
#define ASR_FRAME_LEN 5
#define ASR_FRAME_HEAD1 0xAA // 帧头1
#define ASR_FRAME_HEAD2 0x55 // 帧头2
#define ASR_FRAME_TAIL 0x0D // 帧尾

/*--------------------------------------------variables--------------------------------------------*/

typedef struct 
{
    uint8_t asr_rx_buf[ASR_RX_BUF_SIZE]; // 串口中断接收缓冲区
    uint8_t asr_rc_cmd_id; // 接收命令ID
    uint8_t asr_rx_flag; // 接收标志 0：未接收 1：已接收
    uint32_t prev_cmd_time; // 上一次接收命令时间 （ms） 用于处理消抖 初始为最大值


}asr_rx_t;

extern UART_HandleTypeDef huart1;

/* --------------------------------------------functions--------------------------------------------*/
void asrpro_ReadCmdID(uint8_t* cmd_id);
void asrpro_Reset(void);
void asrpro_Init(void);

#endif