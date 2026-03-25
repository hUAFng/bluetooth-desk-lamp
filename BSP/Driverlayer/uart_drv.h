
#ifndef UART_DRV_H
#define UART_DRV_H

#include "main.h"
#include "usart.h"
#include "stdio.h"

HAL_StatusTypeDef uart_WriteBytes(UART_HandleTypeDef* huart,uint8_t* pData,uint8_t len);
HAL_StatusTypeDef uart_ReadBytes(UART_HandleTypeDef* huart,uint8_t* pData,uint8_t len);
void uart_Init(void);

#endif 