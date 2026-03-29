

#ifndef __HC05_DRV_H__
#define __HC05_DRV_H__

#include "main.h"
#include "uart_drv.h"
#include "string.h"


extern UART_HandleTypeDef huart1;

#define BT_UART_HANDLE huart1
#define BT_UART_RX_BUF_LEN 64

/* --------------------------------------valiables --------------------------------------*/
typedef enum
{
    BT_STATE_DISCONNECTED,
    BT_STATE_CONNECTED,
    BT_STATE_ERROR
}BT_State_t;

typedef struct 
{
    uint8_t uart_rx_buf[BT_UART_RX_BUF_LEN];
    uint8_t uart_rx_data_len;
    uint8_t uart_rx_flag;

    BT_State_t state;
}BT_t;



/*-----------------------------------------function--------------------------------------*/

uint8_t BT_isReceive(void);
void BT_GetReceiveData(uint8_t* data,uint16_t* len);
void BT_Init(void);
void BT_Reset(void);

#endif
