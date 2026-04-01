

#ifndef __BT_DRV_H__
#define __BT_DRV_H__

#include "main.h"
#include "uart_drv.h"
#include "string.h"
#include "asrpro_drv.h"

extern UART_HandleTypeDef huart2;

#define BT_UART_HANDLE huart2
#define BT_UART_RX_BUF_LEN 64

#define BT_CMD_BUF_MAX_LEN 16 // 每条命令最大长度


/*----------------------------------------Command-----------------------------------------*/


/* --------------------------------------valiables --------------------------------------*/

typedef struct 
{
    uint8_t uart_rx_buf[BT_UART_RX_BUF_LEN];
    uint8_t uart_rx_data_len;
    uint8_t uart_rx_flag;
    Cmd_e cmd;
}BT_t;



/*-----------------------------------------function--------------------------------------*/

uint8_t BT_isReceive(void);
void BT_GetCmd(Cmd_e* cmd);
HAL_StatusTypeDef BT_Init(void);
void BT_Reset(void);

#endif
