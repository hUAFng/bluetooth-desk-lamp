

#ifndef __I2C_DRV_H__
#define __I2C_DRV_H__

#include "main.h"
#include "i2c.h"
#include "string.h"

/* 读写函数(带寄存器地址) */
HAL_StatusTypeDef I2C_WriteByte(I2C_HandleTypeDef* hi2c , uint8_t devAddr , uint8_t regAddr,uint8_t data);
HAL_StatusTypeDef I2C_ReadByte(I2C_HandleTypeDef* hi2c , uint8_t devAddr , uint8_t regAddr,uint8_t* data);
HAL_StatusTypeDef I2C_WriteBytes(I2C_HandleTypeDef* hi2c, uint8_t devAddr , uint8_t regAddr , uint8_t* data , uint8_t len);
HAL_StatusTypeDef I2C_ReadBytes(I2C_HandleTypeDef* hi2c,uint8_t devAddr,uint8_t regAddr,uint8_t* data,uint8_t len);

/* 直接读写函数(不带寄存器地址) */
HAL_StatusTypeDef I2C_WriteRaw(I2C_HandleTypeDef* hi2c, uint8_t devAddr, uint8_t* data, uint8_t len);
HAL_StatusTypeDef I2C_ReadRaw(I2C_HandleTypeDef* hi2c, uint8_t devAddr, uint8_t* data, uint8_t len);

/* 位操作函数 */
HAL_StatusTypeDef I2C_WriteBit(I2C_HandleTypeDef* hi2c, uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data);
HAL_StatusTypeDef I2C_ReadBit(I2C_HandleTypeDef* hi2c, uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t* data);




#endif