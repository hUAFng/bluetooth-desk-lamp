

// BT使用串口DMA发送与接收

#include "BT_drv.h"

static BT_t bt;

static char bt_cmd[BT_OR_ASR_CMD_LEN][BT_CMD_BUF_MAX_LEN] = 
{
    "poweron",
    "poweroff",
    "manualmode",
    "automode",
    "musicmode",
    "changecolor",
    "lighton",
    "lightoff"
};



HAL_StatusTypeDef BT_WriteBytes(uint8_t* data,uint16_t len)
{
    if (data == NULL || len == 0) return HAL_ERROR;

    return HAL_UART_Transmit_DMA(&BT_UART_HANDLE,data,len);
}

void BT_ClearBuf(void)
{
    bt.uart_rx_data_len = 0;
    memset(bt.uart_rx_buf,0,sizeof(bt.uart_rx_buf));
    bt.uart_rx_flag = 0;
    bt.cmd = NoneCmd;
}

/**
 * @brief 蓝牙模块初始化
 */
HAL_StatusTypeDef BT_Init(void)
{
    HAL_StatusTypeDef status;

    HAL_UART_DMAStop(&BT_UART_HANDLE);

    BT_ClearBuf();

    HAL_GPIO_WritePin(BT_EN_GPIO_Port,BT_EN_Pin,GPIO_PIN_RESET);
    HAL_Delay(100);

    if(HAL_UARTEx_ReceiveToIdle_DMA(&BT_UART_HANDLE,bt.uart_rx_buf,BT_UART_RX_BUF_LEN) != HAL_OK) // 开启DMA接收-空闲中断
        return HAL_ERROR;
       
    HAL_GPIO_WritePin(BT_EN_GPIO_Port,BT_EN_Pin,GPIO_PIN_SET); //进入AT模式
    HAL_Delay(100);

    if(BT_WriteBytes("AT\r\n",4) != HAL_OK)
        return HAL_ERROR;
    HAL_Delay(100);
    
    if(BT_WriteBytes("AT+NAME=BT_DESK_LAMP\r\n",22) != HAL_OK)
        return HAL_ERROR;
    HAL_Delay(100);
    
    if(BT_WriteBytes("AT+PSWD=123456\r\n",16) != HAL_OK)
        return HAL_ERROR;
    HAL_Delay(100);
    
    if(BT_WriteBytes("AT+ROLE=0\r\n",11) != HAL_OK)
        return HAL_ERROR;
    HAL_Delay(100);

    if(BT_WriteBytes("AT+UART=9600,0,0\r\n",16) != HAL_OK)
        return HAL_ERROR;
    HAL_Delay(100);

    if(BT_WriteBytes("AT+PSAVE\r\n",10) != HAL_OK)
        return HAL_ERROR;
    HAL_Delay(100);

    if(BT_WriteBytes("AT+RESET\r\n",10) != HAL_OK)
        return HAL_ERROR;
    HAL_Delay(100);

    // 复位完毕重新启动DMA接收
    HAL_UART_DMAStop(&BT_UART_HANDLE);  
    HAL_Delay(100);

    if(HAL_UARTEx_ReceiveToIdle_DMA(&BT_UART_HANDLE,bt.uart_rx_buf,BT_UART_RX_BUF_LEN) != HAL_OK)
        return HAL_ERROR;

    HAL_GPIO_WritePin(BT_EN_GPIO_Port,BT_EN_Pin,GPIO_PIN_RESET); // 进入传透模式
    HAL_Delay(100);

    return HAL_OK;
}

void BT_Reset(void)
{
    HAL_UART_DMAStop(&BT_UART_HANDLE); // 停止DMA接收
    HAL_Delay(10);

    HAL_GPIO_WritePin(BT_EN_GPIO_Port, BT_EN_Pin, GPIO_PIN_RESET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(BT_EN_GPIO_Port, BT_EN_Pin, GPIO_PIN_SET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(BT_EN_GPIO_Port, BT_EN_Pin, GPIO_PIN_RESET);
    HAL_Delay(100);

    BT_ClearBuf();

    HAL_UARTEx_ReceiveToIdle_DMA(&BT_UART_HANDLE, bt.uart_rx_buf, BT_UART_RX_BUF_LEN); // 重启DMA接收-空闲中断
}
/**
 * @brief 重写HAL_UARTEx_RxEventCallback函数，用于接收蓝牙数据
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == BT_UART_HANDLE.Instance)
    {

        __disable_irq(); // 进入临界区，防止数据冲突

        if (Size > BT_UART_RX_BUF_LEN) Size = BT_UART_RX_BUF_LEN;
            
        bt.uart_rx_data_len = Size;
        bt.uart_rx_flag = 1;

        HAL_UARTEx_ReceiveToIdle_DMA(&BT_UART_HANDLE,bt.uart_rx_buf,BT_UART_RX_BUF_LEN); // 重启DMA接收-空闲中断
        
        __enable_irq(); // 退出临界区
    }
}


/**
 * @brief 检查是否有数据接收
 * @return uint8_t 1:有数据接收 0:无数据接收
 */
uint8_t BT_isReceive(void)
{
    uint8_t flag = bt.uart_rx_flag;
    bt.uart_rx_flag = 0; // 清除接收标志
    return flag;
}


/**
 * @brief 将接收到的数据进行处理，判断是否为有效命令
 * @return uint8_t 1:有效命令 0:无效命令
 */
uint8_t BT_DataProcess(void)
{
    uint8_t valid_flag = 0;

    for (uint8_t i = 0; i < BT_OR_ASR_CMD_LEN ;i++)  //字符串匹配
    {
        if(strncmp(bt.uart_rx_buf,bt_cmd[i],strlen(bt_cmd[i])) == 0)
        {
            bt.cmd = (Cmd_e)i;
            valid_flag = 1;

            break;
        }
    }

    if (valid_flag == 0)
    {
        BT_WriteBytes("There is no such command\r\n",26);
        
        BT_WriteBytes("Valid commands are:\r\n",21);

        for (uint8_t i = 0; i < BT_OR_ASR_CMD_LEN ;i++)
        {
            BT_WriteBytes(bt_cmd[i],strlen(bt_cmd[i]));
            BT_WriteBytes("\r\n",2);
        }
    }
    else 
    {
        BT_WriteBytes("Command is valid\r\n",18);
    }

    return valid_flag;

}

/**
 * @brief 获取接收的数据
 * @param data 接收数据缓冲区
 * @param len 接收数据长度
 */
void BT_GetCmd(Cmd_e* cmd)
{
    if (cmd == NULL) return;

    if (BT_isReceive())
    {
        if(BT_DataProcess())
        {
            *cmd = bt.cmd; // 有效命令
        }
        else
        {
            *cmd = NoneCmd; // 无效命令
        }
    }
    else
    {
        *cmd = NoneCmd; // 无数据
    }
}




