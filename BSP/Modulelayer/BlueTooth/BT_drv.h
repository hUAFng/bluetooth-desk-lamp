

#ifndef __BT_DRV_H__
#define __BT_DRV_H__

#include "main.h"
#include "uart_drv.h"
#include "string.h"
#include "asrpro_drv.h"
#include "cmd.h"

extern UART_HandleTypeDef huart2;

#define BT_UART_HANDLE huart2
#define BT_UART_RX_BUF_LEN 64
#define BT_CMD_BUF_MAX_LEN 16 // 每条命令最大长度
#define BT_UART_TIMEOUT 100

#define BT_RECONNECT_TIMEOUT_MS  5000
#define BT_HEARTBEAT_INTERVAL_MS 30000
#define BT_MAX_RETRY              3

typedef enum
{
    BT_STATE_UNINIT = 0,
    BT_STATE_AT_CONFIG,
    BT_STATE_READY,
    BT_STATE_CONNECTED,
    BT_STATE_ERROR
} BT_State_e;

typedef enum
{
    BT_ERR_NONE = 0,
    BT_ERR_AT_FAILED,
    BT_ERR_UART_FAILED,
    BT_ERR_TIMEOUT,
    BT_ERR_DISCONNECTED
} BT_Error_e;

typedef struct 
{
    uint8_t uart_rx_buf[BT_UART_RX_BUF_LEN];
    uint16_t uart_rx_data_len;
    uint8_t uart_rx_flag;
    Cmd_e cmd;

    BT_State_e state;
    BT_Error_e last_error;
    uint8_t connected;
	uint32_t last_rx_time_ms;
} BT_t;



HAL_StatusTypeDef BT_Init(void);
void BT_Reset(void);
void BT_GetCmd(Cmd_e* cmd);
uint8_t BT_IsConnected(void);
#endif
