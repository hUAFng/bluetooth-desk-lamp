

// BT使用串口DMA发送与接收

#include "BT_drv.h"

static BT_t bt;

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
    bt.state = BT_STATE_DISCONNECTED;
}

/**
 * @brief 蓝牙模块初始化
 */
HAL_StatusTypeDef BT_Init(void)
{
    HAL_StatusTypeDef status;
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

    HAL_GPIO_WritePin(BT_EN_GPIO_Port,BT_EN_Pin,GPIO_PIN_RESET); // 进入传透模式
    HAL_Delay(100);

}

void BT_Reset(void)
{
    HAL_GPIO_WritePin(BT_EN_GPIO_Port, BT_EN_Pin, GPIO_PIN_RESET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(BT_EN_GPIO_Port, BT_EN_Pin, GPIO_PIN_SET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(BT_EN_GPIO_Port, BT_EN_Pin, GPIO_PIN_RESET);
    HAL_Delay(100);

    BT_ClearBuf();
}
/**
 * @brief 重写HAL_UARTEx_RxEventCallback函数，用于接收蓝牙数据
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == BT_UART_HANDLE.Instance)
    {
        if (Size > BT_UART_RX_BUF_LEN) Size = BT_UART_RX_BUF_LEN;
            
        bt.uart_rx_data_len = Size;
        bt.uart_rx_flag = 1;

        if (bt.state == BT_STATE_DISCONNECTED && Size > 0) bt.state = BT_STATE_CONNECTED;

        HAL_UARTEx_ReceiveToIdle_DMA(&BT_UART_HANDLE,bt.uart_rx_buf,BT_UART_RX_BUF_LEN); // 重启DMA接收-空闲中断
    }
}


/**
 * @brief 检查是否有数据接收
 * @return uint8_t 1:有数据接收 0:无数据接收
 */
uint8_t BT_isReceive(void)
{
    uint8_t flag = bt.uart_rx_flag;
    bt.uart_rx_flag = 0;
    return flag;
}

/**
 * @brief 获取接收的数据
 * @param data 接收数据缓冲区
 * @param len 接收数据长度
 */
void BT_GetReceiveData(uint8_t* data,uint16_t* len)
{
    if (data == NULL || len == 0) return;
    memcpy(data,bt.uart_rx_buf,bt.uart_rx_data_len);
    *len = bt.uart_rx_data_len;
}




