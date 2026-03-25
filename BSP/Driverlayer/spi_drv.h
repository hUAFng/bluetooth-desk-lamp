


#ifndef __SPI_DRV_H__
#define __SPI_DRV_H__

#include "main.h"
#include "spi.h"

/* 发送接收函数 */
HAL_StatusTypeDef SPI_WriteBytes(SPI_HandleTypeDef* hspi, uint8_t* pData, uint16_t size);
HAL_StatusTypeDef SPI_ReadBytes(SPI_HandleTypeDef* hspi, uint8_t* pData, uint16_t size);
HAL_StatusTypeDef SPI_WriteAndRead(SPI_HandleTypeDef* hspi, uint8_t* pTxData, uint8_t* pRxData, uint16_t len);

/* 单字节读写 */
HAL_StatusTypeDef SPI_WriteByte(SPI_HandleTypeDef* hspi, uint8_t byte);
HAL_StatusTypeDef SPI_ReadByte(SPI_HandleTypeDef* hspi, uint8_t* byte);


#endif