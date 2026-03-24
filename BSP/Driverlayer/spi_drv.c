

#include "spi_drv.h"

#define SPI_TIMEOUT 100

/**
 * @brief SPI发送数据
 * @param hspi SPI句柄
 * @param pData 发送数据缓冲区
 * @param len 数据长度
 * @return HAL_StatusTypeDef
 */
HAL_StatusTypeDef SPI_WriteBytes(SPI_HandleTypeDef* hspi, uint8_t* pData, uint16_t len)
{
    if (hspi == NULL || pData == NULL || len == 0) return HAL_ERROR;

    return HAL_SPI_Transmit(hspi, pData, len, SPI_TIMEOUT);
}

/**
 * @brief SPI接收数据
 * @param hspi SPI句柄
 * @param pData 接收数据缓冲区
 * @param len 数据长度
 * @return HAL_StatusTypeDef
 */
HAL_StatusTypeDef SPI_ReadBytes(SPI_HandleTypeDef* hspi, uint8_t* pData, uint16_t len)
{
    if (hspi == NULL || pData == NULL || len == 0) return HAL_ERROR;

    return HAL_SPI_Receive(hspi, pData, len, SPI_TIMEOUT);
}

/**
 * @brief SPI同时发送和接收数据
 * @param hspi SPI句柄
 * @param pTxData 发送数据缓冲区
 * @param pRxData 接收数据缓冲区
 * @param len 数据长度
 * @return HAL_StatusTypeDef
 */
HAL_StatusTypeDef SPI_WriteAndRead(SPI_HandleTypeDef* hspi, uint8_t* pTxData, uint8_t* pRxData, uint16_t len)
{
    if (hspi == NULL || pTxData == NULL || pRxData == NULL || len == 0) return HAL_ERROR;

    return HAL_SPI_TransmitReceive(hspi, pTxData, pRxData, len, SPI_TIMEOUT);
}


/**
 * @brief SPI写入单字节数据
 * @param hspi SPI句柄
 * @param byte 要写入的字节
 * @return HAL_StatusTypeDef
 */
HAL_StatusTypeDef SPI_WriteByte(SPI_HandleTypeDef* hspi, uint8_t byte)
{
    return HAL_SPI_Transmit(hspi, &byte, 1, SPI_TIMEOUT);
}

/**
 * @brief SPI读取单字节数据
 * @param hspi SPI句柄
 * @param byte 接收数据的指针
 * @return HAL_StatusTypeDef
 */
HAL_StatusTypeDef SPI_ReadByte(SPI_HandleTypeDef* hspi, uint8_t* byte)
{
    if (byte == NULL) return HAL_ERROR;

    return HAL_SPI_Receive(hspi, byte, 1, SPI_TIMEOUT);
}




