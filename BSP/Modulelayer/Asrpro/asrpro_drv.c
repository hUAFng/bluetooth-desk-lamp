


#include "asrpro_drv.h"

static asr_rx_t asr_rx;

void asrpro_ClearBuffer(void)
{
    memset(&asr_rx,0,sizeof(asr_rx));
}

void asrpro_Init(void)
{
    HAL_UART_Receive_IT(&ASR_UART_Handle, asr_rx.asr_rx_buf, ASR_FRAME_LEN);
}



void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
    if (huart->Instance == ASR_UART_Handle.Instance)
    {
		asr_rx.asr_rx_flag = 1;
		asr_rx.prev_cmd_time = HAL_GetTick();
		
        HAL_UART_Receive_IT(&ASR_UART_Handle, asr_rx.asr_rx_buf, ASR_FRAME_LEN); 
    }
}

/**
 * @brief 查看当前条命令是否在有效时间内接收（不允许短时间内持续接收）
 * @return 0: 无效命令 1: 有效命令
 */
uint8_t asrpro_CheckCmdTimeValid(void)
{
    uint32_t cnt_time = HAL_GetTick(); //获取当前时刻

    // 当前命令在2s内重复接收并且不是第一次接收，无效命令
    if(asr_rx.prev_cmd_time != 0 && cnt_time - asr_rx.prev_cmd_time <= ASR_RX_CMD_DURATION) 
    {
        asr_rx.asr_rx_flag = 0;
        return 0;
    }
    else // 命令有效 在2s外接收 或者 是第一次接收
    {
        asr_rx.prev_cmd_time = cnt_time;
        asr_rx.asr_rx_flag = 0;
        asr_rx.asr_rx_cmd = asr_rx.asr_rx_buf[2];  
        return 1;
    }
}

/**
*@brief 命令本身处理
*/
Cmd_e asrpro_CmdProcess(uint16_t cmd)
{
	if (cmd >= CMD_ASR_LEN) asr_rx.asr_rx_cmd = NoneCmd;
	
	else
		switch(cmd)
		{
			case 0x0000: asr_rx.asr_rx_cmd = CMD_ASR_WAKEUP; break;
			
			case 0x0001: asr_rx.asr_rx_cmd = CMD_POWER_ON;   break;
			
			case 0x0002: asr_rx.asr_rx_cmd = CMD_POWER_OFF;  break;
			
			case 0x0003: asr_rx.asr_rx_cmd = CMD_ManualMode; break;
			
			case 0x0004: asr_rx.asr_rx_cmd = CMD_AutoMode;   break;
			
			case 0x0005: asr_rx.asr_rx_cmd = CMD_MusicMode;  break;
			
			case 0x0006: asr_rx.asr_rx_cmd = CMD_CHANGE_COLOR; break;
			
			case 0x0007: asr_rx.asr_rx_cmd = CMD_RGB_LIGHT_ON; break;
			
			case 0x0008: asr_rx.asr_rx_cmd = CMD_RGB_LIGHT_OFF; break;
			
			default: break;
		}
	
	return asr_rx.asr_rx_cmd;
}


/**
* @brief 读取当前命令(自动判别是否有效，含短时间重复处理)
 * @param cmd 命令 （0：无效命令 其他：有效命令）
 */
void asrpro_GetCmd(Cmd_e* cmd)
{
    if (cmd == NULL) return;

    if (asr_rx.asr_rx_flag)
    {
        if(asrpro_CheckCmdTimeValid())
        {
            // buzzer_work();
			
			uint16_t rx_cmd = ( asr_rx.asr_rx_buf[0] << 8 | asr_rx.asr_rx_buf[1] );
			
			*cmd = asrpro_CmdProcess(rx_cmd); // 有效命令或者命令不存在

        }
        else 
        {
            *cmd = NoneCmd; // 时间无效命令
        }
    }
    else
    {
        *cmd = NoneCmd; // 无命令
    }
}

