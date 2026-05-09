

// App 端发送的字符串必须以\n结尾。

#include "BT_drv.h"

static BT_t bt;

static char bt_cmd[CMD_LEN][BT_CMD_BUF_MAX_LEN] = 
{
    "poweron",
    "poweroff",
    "manualmode",
    "automode",
    "musicmode",
    "changecolor",
    "lighton",
    "lightoff"
};



static HAL_StatusTypeDef BT_WriteBytes(const uint8_t* data, uint16_t len)
{
    if (data == NULL || len == 0) return HAL_ERROR;

    return HAL_UART_Transmit(&BT_UART_HANDLE, (uint8_t*)data, len,BT_UART_TIMEOUT);
}

static void BT_ClearBuf(void)
{
    bt.uart_rx_data_len = 0;
    memset(bt.uart_rx_buf,0,sizeof(bt.uart_rx_buf));
    bt.uart_rx_flag = 0;
    bt.cmd = NoneCmd;
}

/**
*@brief 发送AT指令并等待“OK”应答
*@param at_cmd 完整的AT指令 需要含\r\n
*/
static HAL_StatusTypeDef BT_SendAT_CheckOK(const uint8_t* at_cmd,uint16_t wait_ms)
{
	BT_ClearBuf();
	
	if (BT_WriteBytes(at_cmd, strlen((const char*)at_cmd)) != HAL_OK)
	{
		return HAL_ERROR;
	}
	
	// 等待应答
	uint32_t during;
	while (!bt.uart_rx_flag && during < wait_ms)
	{
		HAL_Delay(10);
		during += 10;
	}
	
	if (!bt.uart_rx_flag) return HAL_TIMEOUT;
		
	
	for (uint8_t i = 0;i < bt.uart_rx_data_len - 1;i++)
	{
		if (bt.uart_rx_buf[i] == 'O' && bt.uart_rx_buf[i+1] == 'K')
		{
			return HAL_OK;
		}
	}
	
	return HAL_ERROR;
}


/**
 * @brief 蓝牙模块初始化
 */
HAL_StatusTypeDef BT_Init(void)
{
	uint16_t wait_ms = 200;
	
    HAL_UART_AbortReceive_IT(&BT_UART_HANDLE);
    BT_ClearBuf();

    HAL_GPIO_WritePin(BT_EN_GPIO_Port,BT_EN_Pin,GPIO_PIN_RESET);
    HAL_Delay(100);

    if(HAL_UARTEx_ReceiveToIdle_IT(&BT_UART_HANDLE,bt.uart_rx_buf,BT_UART_RX_BUF_LEN) != HAL_OK) // 开启空闲中断接收
        return HAL_ERROR;
       
    HAL_GPIO_WritePin(BT_EN_GPIO_Port,BT_EN_Pin,GPIO_PIN_SET); //进入AT模式
    HAL_Delay(100);

    if(BT_SendAT_CheckOK("AT\r\n",wait_ms) != HAL_OK) return HAL_ERROR;
    
    if(BT_SendAT_CheckOK("AT+NAME=BT_DESK_LAMP\r\n",wait_ms) != HAL_OK) return HAL_ERROR;
    
    if(BT_SendAT_CheckOK("AT+PSWD=123456\r\n",wait_ms) != HAL_OK) return HAL_ERROR;
    
    if(BT_SendAT_CheckOK("AT+ROLE=0\r\n",wait_ms) != HAL_OK) return HAL_ERROR;
      
    if(BT_SendAT_CheckOK("AT+UART=9600,0,0\r\n",wait_ms) != HAL_OK) return HAL_ERROR;

    if(BT_SendAT_CheckOK("AT+PSAVE\r\n",wait_ms) != HAL_OK) return HAL_ERROR;

	HAL_UART_AbortReceive_IT(&BT_UART_HANDLE);
	BT_ClearBuf();
	
    HAL_UART_Transmit(&BT_UART_HANDLE,(uint8_t*)"AT+RESET\r\n",10,BT_UART_TIMEOUT);  
    HAL_Delay(1000);  //等待重启
	

    if(HAL_UARTEx_ReceiveToIdle_IT(&BT_UART_HANDLE,bt.uart_rx_buf,BT_UART_RX_BUF_LEN) != HAL_OK) // 开启空闲中断接收
        return HAL_ERROR;

    HAL_GPIO_WritePin(BT_EN_GPIO_Port,BT_EN_Pin,GPIO_PIN_RESET); // 进入传透模式
    HAL_Delay(100);
	
	BT_ClearBuf();

    return HAL_OK;
}

/**
 * @brief 蓝牙模块复位：进入AT模式 → 发送AT+RESET → 等待重启 → 回到透传模式
 */
void BT_Reset(void)
{
    HAL_UART_AbortReceive_IT(&BT_UART_HANDLE);
    HAL_Delay(10);

    HAL_GPIO_WritePin(BT_EN_GPIO_Port, BT_EN_Pin, GPIO_PIN_SET); // 进入AT模式
    HAL_Delay(200);

    HAL_UART_Transmit(&BT_UART_HANDLE, (uint8_t*)"AT+RESET\r\n", 10, BT_UART_TIMEOUT); // 发送复位指令
    HAL_Delay(1000); // 等待模块重启完成

    HAL_GPIO_WritePin(BT_EN_GPIO_Port, BT_EN_Pin, GPIO_PIN_RESET); // 回到透传模式
    HAL_Delay(200);

    BT_ClearBuf();

    HAL_UARTEx_ReceiveToIdle_IT(&BT_UART_HANDLE, bt.uart_rx_buf, BT_UART_RX_BUF_LEN); // 重启中断
}
/**
 * @brief 重写HAL_UARTEx_RxEventCallback函数，用于接收蓝牙数据
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == BT_UART_HANDLE.Instance)
    {

        __disable_irq(); // 进入临界区，防止数据冲突

        if (Size > BT_UART_RX_BUF_LEN) Size = BT_UART_RX_BUF_LEN;
            
        bt.uart_rx_data_len = Size;
        bt.uart_rx_flag = 1;

        HAL_UARTEx_ReceiveToIdle_IT(&BT_UART_HANDLE,bt.uart_rx_buf,BT_UART_RX_BUF_LEN); // 重启中断
        
        __enable_irq(); // 退出临界区
    }
}


/**
 * @brief 检查是否有数据接收
 * @return uint8_t 1:有数据接收 0:无数据接收
 */
static uint8_t BT_isReceive(void)
{
    uint8_t flag = bt.uart_rx_flag;
    bt.uart_rx_flag = 0; // 清除接收标志
    return flag;
}

/**
*@brief 字符串匹配，必须以\n结尾
*@return 1:匹配成功 0：匹配失败
*/
static uint8_t BT_MatchCmd(const uint8_t* data,uint16_t data_len,const char* cmd)
{
	size_t cmd_len = strlen(cmd);
	
	if (data_len < cmd_len) return 0;
	
	if (strncmp((const char*)data,cmd,cmd_len) != 0) return 0;
	
	if (data_len == cmd_len) return 1;
	else if (data[cmd_len] == '\n') return 1;
	
	return 0;
}

/**
 * @brief 将接收到的数据进行处理，判断是否为有效命令
 * @return uint8_t 1:有效命令 0:无效命令
 */
uint8_t BT_DataProcess(void)
{
    uint8_t valid_flag = 0;

    for (uint8_t i = 0; i < CMD_LEN ;i++)  //字符串匹配
    {
        if(BT_MatchCmd(bt.uart_rx_buf,bt.uart_rx_data_len,bt_cmd[i]))
        {
            bt.cmd = (Cmd_e)i;
            valid_flag = 1;
            break;
        }
    }

    if (valid_flag == 0)
    {
		char help_msg[256] ; // 发回去的字符串
		uint8_t pos = 0;
		
        pos += snprintf(help_msg + pos,sizeof(help_msg) - pos,"There is no such command\r\n" );
		pos += snprintf(help_msg + pos,sizeof(help_msg) - pos,"Valid commands are:\r\n");
		
        for (uint8_t i = 0; i < CMD_LEN ;i++)
        {
            pos += snprintf(help_msg + pos,sizeof(help_msg) - pos,"%s\n",bt_cmd[i]);
        }
		
		BT_WriteBytes((const uint8_t*)help_msg,(uint16_t)pos);
    }
    else 
    {
        BT_WriteBytes((const uint8_t*)"Command is valid\r\n",18);
    }

    return valid_flag;

}

/**
 * @brief 获取接收的数据
 * @param data 接收数据缓冲区
 * @param len 接收数据长度
 */
void BT_GetCmd(Cmd_e* cmd)
{
    if (cmd == NULL) return;

    if (BT_isReceive())
    {
        *cmd = BT_DataProcess() ? bt.cmd : NoneCmd;
    }
    else
    {
        *cmd = NoneCmd; // 无数据
    }
}



