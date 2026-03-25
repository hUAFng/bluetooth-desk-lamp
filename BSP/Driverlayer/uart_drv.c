

#include "uart_drv.h"

#define UART_TIMEOUT 100

/*
 * @brief UART发送数据
 * @param huart UART句柄
 * @param pData 数据指针
 * @param len 数据长度
 * @return HAL_StatusTypeDef HAL_OK 成功，HAL_ERROR 失败
 */
HAL_StatusTypeDef uart_WriteBytes(UART_HandleTypeDef* huart,uint8_t* pData,uint8_t len)
{
    if (huart == NULL || pData == NULL || len == 0) return HAL_ERROR;

    return HAL_UART_Transmit(huart, pData, len, UART_TIMEOUT);
}

/*
 * @brief 读取指定长度的数据 
 */
HAL_StatusTypeDef uart_ReadBytes(UART_HandleTypeDef* huart,uint8_t* pData,uint8_t len)
{
    if (huart == NULL || pData == NULL || len == 0) return HAL_ERROR;

    return HAL_UART_Receive(huart, pData, len, UART_TIMEOUT);
}


// 重定向printf函数到UART
int fputc(int ch,FILE* f) 
{
    if (f == NULL) return -1;

    HAL_UART_Transmit(&huart1,(uint8_t*)&ch,1,UART_TIMEOUT);
    return ch;
}

void uart_Init(void)
{
    // 开启中断
}
