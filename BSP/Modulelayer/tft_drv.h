

#ifndef __TFT_DRV_H__
#define __TFT_DRV_H__

#include "main.h"
#include "Driverlayer/spi_drv.h"
#include "spi.h"

extern SPI_HandleTypeDef hspi1;


/* --------------------------------------define-------------------------------------------------------*/

/* TFT引脚 */
#define TFT_RST_PIN     TFT_RES_Pin
#define TFT_RST_PORT    TFT_RES_GPIO_Port
#define TFT_DC_PIN      TFT_DC_Pin
#define TFT_DC_PORT     TFT_DC_GPIO_Port
#define TFT_CS_PIN      TFT_CS_Pin
#define TFT_CS_PORT     TFT_CS_GPIO_Port
#define TFT_BL_PIN      TFT_BL_Pin
#define TFT_BL_PORT     TFT_BL_GPIO_Port

/* TFT尺寸 */
#define TFT_WIDTH       128
#define TFT_HEIGHT      128

/* TFT控制命令 */
#define CMD_SWRESET     0x01    // 软件复位
#define CMD_SLPIN       0x10    // 进入睡眠模式
#define CMD_SLPOUT      0x11    // 退出睡眠模式
#define CMD_PTLON       0x12    // 部分显示模式开启
#define CMD_NORON       0x13    // 正常显示模式开启
#define CMD_INVOFF      0x20    // 关闭显示反转
#define CMD_INVON       0x21    // 开启显示反转
#define CMD_DISPOFF     0x28    // 关闭显示
#define CMD_DISPON      0x29    // 开启显示
#define CMD_CASET       0x2A    // 列地址设置
#define CMD_RASET       0x2B    // 行地址设置
#define CMD_RAMWR       0x2C    // 写内存
#define CMD_MADCTL      0x36    // 内存数据访问控制
#define CMD_COLMOD      0x3A    // 像素格式设置

/* LCD颜色定义 */
#define COLOR_BLACK     0x0000
#define COLOR_WHITE     0xFFFF
#define COLOR_RED       0xF800
#define COLOR_GREEN     0x07E0
#define COLOR_BLUE      0x001F
#define COLOR_YELLOW    0xFFE0
#define COLOR_CYAN      0x07FF
#define COLOR_MAGENTA   0xF81F
#define COLOR_ORANGE    0xFD20
#define COLOR_GRAY      0x7BEF

/* 功能定义 */
#define TFT_SPI_HANDLE &hspi1
#define SPI_CMD_ON HAL_GPIO_WritePin(TFT_DC_GPIO_Port, TFT_DC_Pin, GPIO_PIN_RESET) // DC = 0, 命令模式
#define SPI_DATA_ON HAL_GPIO_WritePin(TFT_DC_GPIO_Port, TFT_DC_Pin, GPIO_PIN_SET)  // DC = 1, 数据模式
#define SPI_ENABLE HAL_GPIO_WritePin(TFT_CS_GPIO_Port, TFT_CS_Pin, GPIO_PIN_RESET) // CS=0, 使能SPI
#define SPI_DISABLE HAL_GPIO_WritePin(TFT_CS_GPIO_Port, TFT_CS_Pin, GPIO_PIN_SET)  // CS=1, 禁用SPI


/* --------------------------------------function-------------------------------------------------------*/
void tft_WriteCmd(uint8_t cmd);
void tft_WriteData(uint8_t* data,uint8_t len);




#endif