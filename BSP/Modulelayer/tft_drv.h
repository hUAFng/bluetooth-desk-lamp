

#ifndef __TFT_DRV_H__
#define __TFT_DRV_H__

#include "main.h"
#include "Driverlayer/spi_drv.h"
#include "spi.h"

SPI_HandleTypeDef* hspi1;


/* --------------------------------------define-------------------------------------------------------*/
#define TFT_spi_HANDLE hspi1
#define SPI_CMD_ON HAL_GPIO_WritePin(TFT_DC_GPIO_Port, TFT_DC_PIN, GPIO_PIN_RESET) // DC = 0, 命令模式
#define SPI_DATA_ON HAL_GPIO_WritePin(TFT_DC_GPIO_Port, TFT_DC_PIN, GPIO_PIN_SET) // DC = 1, 数据模式
#define SPI_ENABLE HAL_GPIO_WritePin(TFT_CS_GPIO_Port, TFT_CS_PIN, GPIO_PIN_RESET) // CS=0, 使能SPI
#define SPI_DISABLE HAL_GPIO_WritePin(TFT_CS_GPIO_Port, TFT_CS_PIN, GPIO_PIN_SET) // CS=1, 禁用SPI



#endif