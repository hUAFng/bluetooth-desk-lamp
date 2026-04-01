


#ifndef __ASRPRO_DRV_H__
#define __ASRPRO_DRV_H__ 

#include "main.h"
#include "uart_drv.h"
#include "string.h"

/*--------------------------------------------define--------------------------------------------*/
#define ASR_UART_Handle huart1
#define ASR_RX_BUF_SIZE 10
#define ASR_RX_CMD_DURATION 2000  // 2s之后允许接收处理一次命令

// 语音识别帧长度与格式 格式：帧头1+帧头2+数据+校验+帧尾
#define ASR_FRAME_LEN 5
#define ASR_FRAME_HEAD1 0xAA // 帧头1
#define ASR_FRAME_HEAD2 0x55 // 帧头2
#define ASR_FRAME_TAIL 0x0D // 帧尾

#define BT_OR_ASR_CMD_LEN 8 // 蓝牙或者语音命令个数

/*--------------------------------------------variables--------------------------------------------*/

typedef enum
{
    CMD_POWER_ON = 0,  // "开灯"
    CMD_POWER_OFF, // "关灯"
    CMD_ManualMode, // "手动模式"
    CMD_AutoMode, // "自动模式"
    CMD_MusicMode, // "音乐模式"
    CMD_CHANGE_COLOR, // "改变颜色"
    CMD_RGB_LIGHT_ON, // "亮一点"
    CMD_RGB_LIGHT_OFF, // "暗一点"

    NoneCmd // 无命令

}Cmd_e;  // 全局命令枚举类型

typedef struct 
{
    uint8_t asr_rx_buf[ASR_RX_BUF_SIZE]; // 串口中断接收缓冲区
    Cmd_e asr_rx_cmd; // 接收命令
    uint8_t asr_rx_ready_flag; // 接收就绪标志 0：未就绪 1：就绪接收
    uint8_t asr_rx_flag; // 接收标志 0：未接收 1：已接收
    uint32_t prev_cmd_time; // 上一次接收命令时间 （ms） 用于处理消抖 初始为最大值

    uint8_t asr_rx_err_num; // 错误接收次数 , 用于处理异常情况

}asr_rx_t;

extern UART_HandleTypeDef huart1;

/* --------------------------------------------functions--------------------------------------------*/
void asrpro_GetCmd(Cmd_e* cmd);
void asrpro_Reset(void);
void asrpro_Init(void);
uint8_t asrpro_IsReady(void);



#endif

