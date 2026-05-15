

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
* @brief TFT写入字节
* @param byte 数据
*/
void tft_WriteByte(uint8_t byte)
{
    SPI_DATA_ON;
    SPI_ENABLE;
    SPI_WriteByte(TFT_SPI_HANDLE,byte);
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
* @brief 选中TFT某一部分区域
* @param x1 起始X坐标
* @param y1 起始Y坐标
* @param x2 结束X坐标
* @param y2 结束Y坐标
*/
void tft_SetAddrWindow(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{ 
    if (x1 >= TFT_WIDTH || y1 >= TFT_HEIGHT || x2 >= TFT_WIDTH || y2 >= TFT_HEIGHT) return;
    if (x1 > x2 || y1 > y2) return; // 起始坐标不能大于结束坐标
	
	/*
	x1 += 2;x2 += 2;
	y1 += 1;y2 += 1;
	*/
	uint8_t buf[4];

    SPI_ENABLE; 
	
	// 发送列地址设置命令 
	SPI_CMD_ON; 
	buf[0] = CMD_CASET; 
	HAL_SPI_Transmit(TFT_SPI_HANDLE, buf, 1, SPI_TIMEOUT); 
	SPI_DATA_ON; 
	buf[0] = x1 >> 8; 
	buf[1] = x1 & 0xFF; 
	buf[2] = x2 >> 8; 
	buf[3] = x2 & 0xFF; 
	HAL_SPI_Transmit(TFT_SPI_HANDLE,buf,4,SPI_TIMEOUT); 
	
	// 发送行地址设置命令 
	SPI_CMD_ON; 
	buf[0] = CMD_RASET; 
	HAL_SPI_Transmit(TFT_SPI_HANDLE, buf, 1, SPI_TIMEOUT); 
	SPI_DATA_ON; 
	buf[0] = y1 >> 8; 
	buf[1] = y1 & 0xFF; 
	buf[2] = y2 >> 8;
	buf[3] = y2 & 0xFF; 
	HAL_SPI_Transmit(TFT_SPI_HANDLE,buf,4,SPI_TIMEOUT); 
	
	SPI_CMD_ON; 
	buf[0] = CMD_RAMWR; 
	HAL_SPI_Transmit(TFT_SPI_HANDLE, buf, 1, SPI_TIMEOUT); 
	
	SPI_DATA_ON; // 切换到数据模式，准备接收像素数据 此处不停止
}

void tft_ClearScreen(uint16_t color)
{
	uint8_t buf[2];
	buf[0] = color >> 8;
	buf[1] = color & 0xFF;
	
    tft_SetAddrWindow(0, 0, TFT_WIDTH - 1, TFT_HEIGHT - 1);

    for (uint16_t i = 0;i < TFT_HEIGHT * TFT_WIDTH;i++)
    {
        HAL_SPI_Transmit(TFT_SPI_HANDLE,buf,2,SPI_TIMEOUT);
    }
	SPI_DISABLE;
}


/**
 * @brief  TFT初始化
 */
void tft_Init(void)
{
    // 硬件复位
    HAL_GPIO_WritePin(TFT_RST_PORT, TFT_RST_PIN, GPIO_PIN_RESET);
    HAL_Delay(20);
    HAL_GPIO_WritePin(TFT_RST_PORT, TFT_RST_PIN, GPIO_PIN_SET);
    HAL_Delay(120);
    
    // 退出睡眠模式
    tft_WriteCmd(CMD_SLPOUT);
    HAL_Delay(120);
    
    // 设置颜色模式为16位RGB565
    tft_WriteCmd(CMD_COLMOD);
    tft_WriteByte(0x05); // 16位像素格式
    
    // 设置内存数据访问控制
    tft_WriteCmd(CMD_MADCTL);
    tft_WriteByte(0xC8); //  BGR顺序，垂直方向镜像
    
    // 设置显示方向和颜色顺序
    // tft_WriteCmd(CMD_INVON);  
    
    // 退出部分显示模式
    tft_WriteCmd(CMD_NORON);
    
    // 开启显示
    tft_WriteCmd(CMD_DISPON);
    
    // 清屏
    tft_ClearScreen(COLOR_BLACK);
    
    // 开启背光
    HAL_GPIO_WritePin(TFT_BL_PORT, TFT_BL_PIN, GPIO_PIN_SET);
}

void tft_PowerOff(void)
{
    HAL_GPIO_WritePin(TFT_BL_PORT, TFT_BL_PIN, GPIO_PIN_RESET);
}

void tft_PowerOn(void)
{
    HAL_GPIO_WritePin(TFT_BL_PORT, TFT_BL_PIN, GPIO_PIN_SET);
}

/**
 * @brief 绘制单个像素点
 * @param x X坐标
 * @param y Y坐标
 * @param color 颜色值
 */
void tft_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    if (x >= TFT_WIDTH || y >= TFT_HEIGHT) return ;

    tft_SetAddrWindow(x,y,x,y);

	uint8_t buf[2] = {color >> 8, color & 0xFF}; 
	
	HAL_SPI_Transmit(TFT_SPI_HANDLE, buf, 2, SPI_TIMEOUT);

	SPI_DISABLE; 
}

/**
 * @brief 显示单个字符
 * @param x X坐标
 * @param y Y坐标
 * @param ch 字符  
 * @param fg_color 字符色
 * @param bg_color 背景色
 */
void tft_DisplayChar(uint8_t x,uint8_t y,char ch,uint16_t fg_color,uint16_t bg_color)
{
    if(x >= TFT_WIDTH || y >= TFT_HEIGHT) return;
    if(ch < 0x20 || ch > 0x7E) return; // 仅支持部分字符

    const uint8_t* char_map = ASCII_Font8x16[ch - 0x20];  // 指向字体点阵数据
	uint8_t fg_buf[2] = {fg_color >> 8, fg_color & 0xFF};
    uint8_t bg_buf[2] = {bg_color >> 8, bg_color & 0xFF};

    for (uint8_t row = 0;row < FONT_8X16_HEIGHT;row++)
    {
        uint8_t row_data = char_map[row];

        tft_SetAddrWindow(x ,y + row,x + FONT_8X16_WIDTH - 1,y + row);
		
		for (uint8_t col = 0;col < FONT_8X16_WIDTH;col++)
		{
			if (row_data & (0x80 >> col)) HAL_SPI_Transmit(TFT_SPI_HANDLE,fg_buf,2,SPI_TIMEOUT);
			else  HAL_SPI_Transmit(TFT_SPI_HANDLE,bg_buf,2,SPI_TIMEOUT);
				
		}
		
		SPI_DISABLE;
    }

}

void tft_DisplayString(uint8_t x,uint8_t y,const char* str,uint16_t fg_color,uint16_t bg_color)
{
    uint8_t x_cnt = x; // 当前字符X坐标
    uint8_t y_cnt = y; // 当前字符Y坐标

    while(*str != '\0') // 当当前字符不为结束符时继续
    {
        if(*str < 0x20 || *str > 0x7E) return;

        if(x_cnt + FONT_8X16_WIDTH > TFT_WIDTH)  // 如果下一个字符超出屏幕宽度 则换行
        {
            x_cnt = x;
            y_cnt += FONT_8X16_HEIGHT; // 换行
            
            if (y_cnt > TFT_HEIGHT) return; // 超出屏幕范围，停止显示
        }
        
        if (*str == '\n')
        {
            x_cnt = x;
            y_cnt += FONT_8X16_HEIGHT; // 换行
            
            if (y_cnt > TFT_HEIGHT) return; // 超出屏幕范围，停止显示
        }
        else if (*str == '\r') x_cnt = x;  // 行首
        else
        {
            tft_DisplayChar(x_cnt, y_cnt, *str, fg_color, bg_color);
            
            x_cnt += FONT_8X16_WIDTH;
        }

        str++;
    }
}

/**
 * @brief 展示整数（含负数）
 * @param num 待显示的数字
 */
void tft_DisplayNum(uint16_t x,uint16_t y,int32_t num,uint16_t fg_color,uint16_t bg_color)
{
    char str[12]; 

    sprintf(str, "%d", num); // 将数字转换为字符串（自动处理负数 + 末尾结束符）

    tft_DisplayString(x, y, str, fg_color, bg_color);
}

/**
 * @brief 绘制线段
 */
void tft_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    int32_t dx = abs((int32_t)(x2 - x1));
    int32_t dy = -abs((int32_t)(y2 - y1)); 
    int16_t sx = (x1 < x2) ? 1 : -1;// 步进方向
    int16_t sy = (y1 < y2) ? 1 : -1;// 步进方向
    int32_t err = dx + dy; // 误差项
    int32_t e2;// 误差项
    while(1)
    {
        tft_DrawPixel(x1, y1, color);
        
        if(x1 == x2 && y1 == y2) break;
        
        e2 = 2 * err;   // 误差两倍-用于比较

        if(e2 > -dy)
        {
            err -= dy;
            x1 += sx;
        }
        if(e2 < dx)
        {
            err += dx;
            y1 += sy;
        }
    }
}

/**
 * @brief 填充矩形区域
 * @param x,y 起始坐标
 * @param w,h 矩形宽和高（像素）
 * @param color RGB565颜色
 */
void tft_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    if (x >= TFT_WIDTH || y >= TFT_HEIGHT) return;
    if (x + w > TFT_WIDTH)  w = TFT_WIDTH - x;
    if (y + h > TFT_HEIGHT) h = TFT_HEIGHT - y;
    if (w == 0 || h == 0) return;

    tft_SetAddrWindow(x, y, x + w - 1, y + h - 1);
	
    uint8_t buf[2] = { color >> 8, color & 0xFF };
    for (uint32_t i = 0; i < (uint32_t)w * h; i++) 
	{
        HAL_SPI_Transmit(TFT_SPI_HANDLE, buf, 2, SPI_TIMEOUT);
    }
    SPI_DISABLE; 
}
