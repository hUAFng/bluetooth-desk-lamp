/*
* 由于RGB模块需要微秒级别的延时精度，这里采用DWT定时器来解决
* DWT定时器是Cortex_M内部自带的定时器，频率为芯片主频（72MHz)
*
*/


#include "dwt.h"

void dwt_Init(void)
{
    // 开启DWT时钟
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;  // 使能周期计数器
    DWT->CYCCNT = 0;                      // 计数器清零

}
/**
 * 微秒级延时函数
 */
void delay_us(uint16_t us)
{
    uint32_t start = DWT->CYCCNT;

    uint32_t cycle = us * (SYSTEM_CLOCK / 1000000);  // 计算延时周期

    while ((DWT->CYCCNT - start) < cycle);
}