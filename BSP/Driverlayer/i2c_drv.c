

#include "i2c_drv.h"
#include <string.h>

#define I2C_TIMEOUT 100 // I2C超时时间 100ms

/**
 * @brief 写入I2C寄存器字节
 * @param hi2c I2C_HandleTypeDef指针 \ 句柄
 * @param devAddr 设备地址指针
 * @param regAddr 设备寄存器地址指针
 * @param data 要写入的数据
 * @return HAL_StatusEnum_t HAL_OK:成功 HAL_ERROR:失败
 */
HAL_StatusTypeDef I2C_WriteByte(I2C_HandleTypeDef* hi2c , uint8_t devAddr , uint8_t regAddr,uint8_t data)
{
    if (hi2c == NULL) return HAL_ERROR;
    
    uint8_t buf[2] = {regAddr , data};
    
    if (HAL_I2C_Master_Transmit(hi2c,devAddr,buf,2,I2C_TIMEOUT) != HAL_OK) return HAL_ERROR;

    return HAL_OK;
}

/**
 * @brief 读取I2C寄存器字节
 * @param hi2c I2C_HandleTypeDef指针 \ 句柄
 * @param devAddr 设备地址指针
 * @param regAddr 设备寄存器地址指针
 * @param data 要读取的数据指针
 * @return HAL_StatusTypeDef HAL_OK:成功 HAL_ERROR:失败
 */
HAL_StatusTypeDef I2C_ReadByte(I2C_HandleTypeDef* hi2c , uint8_t devAddr , uint8_t regAddr,uint8_t* data)
{
    if (hi2c == NULL || data == NULL) return HAL_ERROR;

    if (HAL_I2C_Master_Transmit(hi2c,devAddr,&regAddr, 1, I2C_TIMEOUT) != HAL_OK) return HAL_ERROR;
    
    if (HAL_I2C_Master_Receive(hi2c,devAddr,data,1,I2C_TIMEOUT) != HAL_OK) return HAL_ERROR;

    return HAL_OK;
}
    

/**
 * @brief I2C写字符串
 * @param hi2c I2C_HandleTypeDef指针 \ 句柄
 * @param devAddr 设备地址指针
 * @param regAddr 设备寄存器地址指针
 * @param data 要写入的数据指针
 * @param len 数据长度
 * @return HAL_StatusTypeDef HAL_OK:成功 HAL_ERROR:失败
 */
HAL_StatusTypeDef I2C_WriteBytes(I2C_HandleTypeDef* hi2c, uint8_t devAddr , uint8_t regAddr , uint8_t* data , uint8_t len)
{
    if (hi2c == NULL || data == NULL || len == 0) return HAL_ERROR;

    uint8_t buf[256];
    buf[0] = regAddr;

    memcpy(buf+1,data,len);

    if (HAL_I2C_Master_Transmit(hi2c,devAddr,buf,len+1,I2C_TIMEOUT) != HAL_OK) return HAL_ERROR;

    return HAL_OK;
}

/**
 * @brief I2C读字符串
 * @param hi2c I2C_HandleTypeDef指针 \ 句柄
 * @param devAddr 设备地址指针
 * @param regAddr 设备寄存器地址指针
 * @param data 要读取的数据指针
 * @param len 数据长度
 * @return HAL_StatusEnum_t HAL_OK:成功 HAL_ERROR:失败
 */
HAL_StatusTypeDef I2C_ReadBytes(I2C_HandleTypeDef* hi2c,uint8_t devAddr,uint8_t regAddr,uint8_t* data,uint8_t len)
{
    if (hi2c == NULL || data == NULL || len == 0) return HAL_ERROR;

    if (HAL_I2C_Master_Transmit(hi2c,devAddr,&regAddr,1,I2C_TIMEOUT) != HAL_OK) return HAL_ERROR;

    if (HAL_I2C_Master_Receive(hi2c,devAddr,data,len,I2C_TIMEOUT) != HAL_OK) return HAL_ERROR;

    return HAL_OK;
}

/**
 * @brief 直接写入I2C设备数据(不带寄存器地址)
 * @return HAL_StatusTypeDef HAL_OK:成功 HAL_ERROR:失败
 */
HAL_StatusTypeDef I2C_WriteRaw(I2C_HandleTypeDef* hi2c, uint8_t devAddr, uint8_t* data, uint8_t len)
{
    if (hi2c == NULL || data == NULL || len == 0) return HAL_ERROR;

    if (HAL_I2C_Master_Transmit(hi2c, devAddr, data, len, I2C_TIMEOUT) != HAL_OK) return HAL_ERROR;

    return HAL_OK;
}

/**
 * @brief 直接读取I2C设备数据(不带寄存器地址)
 * @return HAL_StatusTypeDef HAL_OK:成功 HAL_ERROR:失败
 */
HAL_StatusTypeDef I2C_ReadRaw(I2C_HandleTypeDef* hi2c, uint8_t devAddr, uint8_t* data, uint8_t len)
{
    if (hi2c == NULL || data == NULL || len == 0) return HAL_ERROR;

    if (HAL_I2C_Master_Receive(hi2c, devAddr, data, len, I2C_TIMEOUT) != HAL_OK) return HAL_ERROR;

    return HAL_OK;
}

/**
 * @brief 写入寄存器的指定位
 * @param bitNum 位号(0-7)
 * @param data 位值(0或1)
 * @return HAL_StatusTypeDef HAL_OK:成功 HAL_ERROR:失败
 */
HAL_StatusTypeDef I2C_WriteBit(I2C_HandleTypeDef* hi2c, uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data)
{
    uint8_t temp;

    if (I2C_ReadByte(hi2c, devAddr, regAddr, &temp) != HAL_OK) return HAL_ERROR; // 读取整个寄存器字节

    temp = (data != 0) ? (temp | (1 << bitNum)) : (temp & ~(1 << bitNum));  // 修改指定位

    return I2C_WriteByte(hi2c, devAddr, regAddr, temp);
}

/**
 * @brief 读取寄存器的指定位
 * @param bitNum 位号(0-7)
 * @param data 接收位值的指针
 * @return HAL_StatusTypeDef HAL_OK:成功 HAL_ERROR:失败
 */
HAL_StatusTypeDef I2C_ReadBit(I2C_HandleTypeDef* hi2c, uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t* data)
{
    uint8_t temp;

    if (I2C_ReadByte(hi2c, devAddr, regAddr, &temp) != HAL_OK) return HAL_ERROR;

    *data = (temp >> bitNum) & 0x01;

    return HAL_OK;
}



