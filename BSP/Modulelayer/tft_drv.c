

#include "tft_drv.h"


/*
* @brief TFT写入命令
* @param cmd 命令
*/
void tft_WriteCmd(uint8_t cmd)
{
    SPI_CMD_ON;
    SPI_ENABLE;
    SPI_WriteByte(TFT_SPI_HANDLE,cmd);
    SPI_DISABLE;
}

/*
* @brief TFT写入数据
* @param data 数据
* @param len 数据长度
*/
void tft_WriteData(uint8_t* data,uint8_t len)
{
    if (data == NULL || len == 0) return;

    SPI_DATA_ON;
    SPI_ENABLE;
    SPI_WriteBytes(TFT_SPI_HANDLE,data,len);
    SPI_DISABLE;
}
/**
 * @brief TFT写入2字节的数据
 * @param data 2字节数据
 */
void tft_WriteData_16Bit(uint16_t data)
{
    uint8_t buf[2];
    buf[0] = data >> 8;
    buf[1] = data & 0xFF;

    SPI_DATA_ON;
    SPI_ENABLE;
    SPI_WriteBytes(TFT_SPI_HANDLE, buf, 2);
    SPI_DISABLE;
}

/*
* @brief 设置TFT某一部分显示窗口
* @param x1 起始X坐标
* @param y1 起始Y坐标
* @param x2 结束X坐标
* @param y2 结束Y坐标
*/
void tft_SetAddrWindow(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{ 
    if (x1 >= TFT_WIDTH || y1 >= TFT_HEIGHT || x2 >= TFT_WIDTH || y2 >= TFT_HEIGHT) return;

    tft_WriteCmd(CMD_CASET);
    tft_WriteData_16Bit(x1);
    tft_WriteData_16Bit(x2);
    
    tft_WriteCmd(CMD_RASET);
    tft_WriteData_16Bit(y1);
    tft_WriteData_16Bit(y2);

    tft_WriteCmd(CMD_RAMWR);  //准备写入内存数据
}










