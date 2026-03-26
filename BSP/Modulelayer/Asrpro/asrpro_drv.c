


#include "asrpro_drv.h"

static asr_rx_t asr_rx = {{0},0,0,0xFFFFFFFF};
void asrpro_Reset(void)
{
    HAL_GPIO_WritePin(ASR_Res_GPIO_Port, ASR_Res_Pin, GPIO_PIN_RESET);
    HAL_Delay(120); // 拉低复位

    HAL_GPIO_WritePin(ASR_Res_GPIO_Port, ASR_Res_Pin, GPIO_PIN_SET);
    HAL_Delay(500); // 复位后等待启动
}


void asrpro_Init(void)
{
    asrpro_Reset();

    HAL_UART_Receive_IT(&ASR_UART_Handle, asr_rx.asr_rx_buf, ASR_FRAME_LEN);
    
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
    if (huart->Instance == ASR_UART_Handle.Instance)
    {
        if (asr_rx.asr_rx_buf[0] == ASR_FRAME_HEAD1 && asr_rx.asr_rx_buf[1] == ASR_FRAME_HEAD2 && asr_rx.asr_rx_buf[4] == ASR_FRAME_TAIL)
        {
            uint8_t check_num = ASR_FRAME_HEAD1 + ASR_FRAME_HEAD2 + asr_rx.asr_rx_buf[2];

            if (asr_rx.asr_rx_buf[3] == (check_num & 0xFF))
            {
                asr_rx.asr_rx_flag = 1;
            }
        }

        HAL_UART_Receive_IT(&ASR_UART_Handle, asr_rx.asr_rx_buf, ASR_FRAME_LEN);
    }
}

/**
 * @brief 查看当前条命令是否在有效时间内接收（不允许短时间内持续接收）
 * @return 0: 无效命令 1: 有效命令
 */
uint8_t asrpro_CheckCmdValid(void)
{
    uint32_t cnt_time = HAL_GetTick(); //获取当前时刻

    if(cnt_time - asr_rx.prev_cmd_time < ASR_RX_CMD_DURATION)
    {
        asr_rx.asr_rx_flag = 0;

        return 0;
    }
    else
    {
        asr_rx.prev_cmd_time = cnt_time;
        asr_rx.asr_rx_flag = 0;
        asr_rx.asr_rc_cmd_id = asr_rx.asr_rx_buf[2]; 

        return 1;
    } 
}

/**
 * @brief 读取当前命令ID(自动判别是否有效，含去抖处理)
 * @param cmd_id 命令ID （0：无效命令ID 其他：有效命令ID）
 */
void asrpro_ReadCmdID(uint8_t* cmd_id)
{
    if (cmd_id == NULL) return;

    __disable_irq();
    uint8_t rx_flag = asr_rx.asr_rx_flag;
    __enable_irq();

    if (rx_flag)
    {
        if(asrpro_CheckCmdValid())
        {
            *cmd_id = asr_rx.asr_rc_cmd_id;
        }
        else 
        {
            *cmd_id = 0; // 无效命令ID
        }
    }
}


