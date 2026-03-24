

#include "tft_drv.h"


void spi_write(uint8_t data)
{
    SPI_WriteByte(TFT_spi_HANDLE,data);
}
void tft_WriteCmd(uint8_t cmd)
{
    SPI_CMD_ON;
    SPI_ENABLE;

    spi_write(&cmd,1);



}
