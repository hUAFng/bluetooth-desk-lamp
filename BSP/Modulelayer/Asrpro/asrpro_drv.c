


#include "asrpro_drv.h"

static asr_rx_t asr_rx = {{0},0,0,0xFFFFFFFF,0};

void asrpro_ClearBuffer(void)
{
    memset(asr_rx.asr_rx_buf, 0, ASR_RX_BUF_SIZE);
    asr_rx.asr_rx_cmd = NoneCmd;
    asr_rx.asr_rx_flag = 0;
    asr_rx.asr_rx_ready_flag = 0;
    asr_rx.prev_cmd_time = 0xFFFFFFFF;
    asr_rx.asr_rx_err_num = 0;
}

void asrpro_Reset(void)
{
    HAL_GPIO_WritePin(ASR_Res_GPIO_Port, ASR_Res_Pin, GPIO_PIN_RESET);
    HAL_Delay(120); // 拉低复位

    HAL_GPIO_WritePin(ASR_Res_GPIO_Port, ASR_Res_Pin, GPIO_PIN_SET);
    HAL_Delay(500); // 复位后等待启动

    asrpro_ClearBuffer();
}


void asrpro_Init(void)
{
    asrpro_Reset();

    HAL_UART_Receive_IT(&ASR_UART_Handle, asr_rx.asr_rx_buf, ASR_FRAME_LEN);
    
}
/**
 * @brief 错误处理函数 超过三次错误自动复位
 */
void asrpro_ErroeHandler(void)
{
    asr_rx.asr_rx_err_num++;

    if (asr_rx.asr_rx_err_num >= 3) 
    {
        asrpro_Reset();

        asr_rx.asr_rx_err_num = 0;
    }

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
    if (huart->Instance == ASR_UART_Handle.Instance)
    {
        __disable_irq();

        if (asr_rx.asr_rx_buf[0] == ASR_FRAME_HEAD1 && asr_rx.asr_rx_buf[1] == ASR_FRAME_HEAD2 && asr_rx.asr_rx_buf[4] == ASR_FRAME_TAIL)
        {
            uint8_t check_num = ASR_FRAME_HEAD1 + ASR_FRAME_HEAD2 + asr_rx.asr_rx_buf[2];

            if (asr_rx.asr_rx_buf[3] == (check_num & 0xFF))
            {
                asr_rx.asr_rx_flag = 1;
            }
            else asrpro_ErroeHandler();
        }
        else asrpro_ErroeHandler();

        __enable_irq();

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

    // 当前命令在2s内重复接收并且不是第一次接收，无效命令
    if(asr_rx.prev_cmd_time != 0xFFFFFFFF && cnt_time - asr_rx.prev_cmd_time < ASR_RX_CMD_DURATION) 
    {
        asr_rx.asr_rx_flag = 0;
        return 0;
    }
    else // 命令有效 在2s外接收 或者 是第一次接收
    {
        asr_rx.prev_cmd_time = cnt_time;
        asr_rx.asr_rx_flag = 0;
        asr_rx.asr_rx_cmd = asr_rx.asr_rx_buf[2]; 
        asr_rx.asr_rx_ready_flag = 1; 
        return 1;
    }
}


/**
 * @brief 读取当前命令(自动判别是否有效，含去抖处理)
 * @param cmd 命令 （0：无效命令 其他：有效命令）
 */
void asrpro_GetCmd(Cmd_e* cmd)
{
    if (cmd == NULL) return;

    uint8_t rx_flag = asr_rx.asr_rx_flag;

    if (rx_flag)
    {
        if(asrpro_CheckCmdValid())
        {
            *cmd = asr_rx.asr_rx_cmd;
            asr_rx.asr_rx_ready_flag = 0; // 读取命令后清除就绪标志，等待下一次接收
        }
        else 
        {
            *cmd = NoneCmd; // 无效命令
        }
    }
    else
    {
        *cmd = NoneCmd; // 无命令
    }
}



/**
 * @brief 读取当前命令是否准备就绪（自动判别是否有效）
 * @return 0: 命令未准备就绪 1: 命令已准备就绪
 */
uint8_t asrpro_IsReady(void)
{
    return asr_rx.asr_rx_flag;
}


