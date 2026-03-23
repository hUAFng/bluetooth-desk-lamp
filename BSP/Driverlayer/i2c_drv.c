

#include "i2c_drv.h"


/**
 * @brief 写入I2C寄存器数据
 * @param hi2c I2C_HandleTypeDef指针 \ 句柄
 * @param devAddr 设备地址指针
 * @param regAddr 设备寄存器地址指针
 * @param data 要写入的数据
 * @return HAL_StatusEnum_t HAL_OK:成功 HAL_ERROR:失败
 */
HAL_StatusTypeDef I2C_WriteByte(I2C_HandleTypeDef* hi2c , uint8_t devAddr , uint8_t* regAddr,uint8_t data)
{
    if (hi2c == NULL || regAddr == NULL ) return HAL_ERROR;
    
    uint8_t buf[2] = {*regAddr , data};
    
    if (HAL_I2C_Master_Transmit(hi2c,devAddr << 1,buf,2,100) != HAL_OK) return HAL_ERROR;

    return HAL_OK;
}

/**
 * @brief 读取I2C寄存器数据
 * @param hi2c I2C_HandleTypeDef指针 \ 句柄
 * @param devAddr 设备地址指针
 * @param regAddr 设备寄存器地址指针
 * @param data 要读取的数据指针
 * @return HAL_StatusEnum_t HAL_OK:成功 HAL_ERROR:失败
 */
HAL_StatusTypeDef I2C_ReadByte(I2C_HandleTypeDef* hi2c , uint8_t devAddr , uint8_t* regAddr,uint8_t* data)
{
    if (hi2c == NULL || regAddr == NULL || data == NULL) return HAL_ERROR;

    if (HAL_I2C_Master_Transmit(hi2c,devAddr << 1,regAddr , 1, 100) != HAL_OK) return HAL_ERROR;
    
    if (HAL_I2C_Master_Receive(hi2c,devAddr << 1,data,1,100) != HAL_OK)

    return HAL_OK;
}
    