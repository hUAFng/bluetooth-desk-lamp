
#ifndef UART_DRV_H
#define UART_DRV_H

#include "main.h"
#include "usart.h"


HAL_StatusTypeDef uart_WriteBytes(UART_HandleTypeDef* huart,uint8_t* pData,uint8_t len);
HAL_StatusTypeDef uart_ReadBytes(UART_HandleTypeDef* huart,uint8_t* pData,uint8_t len);


#endif 