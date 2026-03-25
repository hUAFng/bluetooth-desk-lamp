

#include "light_sensor_drv.h"


HAL_StatusTypeDef ls_WriteByte(uint8_t byte)
{
    return I2C_WriteRaw(&LS_I2C_Handle,LS_I2C_ADDR,&byte,1);
}

void ls_Init(void)
{
    ls_WriteByte(LS_POWON);      //上电
    HAL_Delay(10); 

    ls_WriteByte(LS_RESET);      //重置
    HAL_Delay(10);

    ls_WriteByte(LS_HRES_MODE1); //连续高分辨率模式 - 1lx分辨率
    HAL_Delay(120);              //等待测量完成
}

/**
 * @brief 切换测量模式
 * @param mode : 使用LS_MODE枚举
 */
HAL_StatusTypeDef ls_SetMode(LS_MODE mode)
{
    if (mode < LS_MODE_HRES1 || mode > LS_MODE_SINGLE_MEAS) return HAL_ERROR; // 无效
    
    uint8_t temp = 0;
    if (mode == LS_MODE_HRES1) temp = LS_HRES_MODE1;
    else if (mode == LS_MODE_HRES2) temp = LS_HRES_MODE2;
    else if (mode == LS_MODE_LRES) temp = LS_LRES_MODE;
    else if (mode == LS_MODE_SINGLE_MEAS) temp = LS_SINGLE_MEAS_MODE;
    
    if (ls_WriteByte(temp) != HAL_OK) return HAL_ERROR;

    HAL_Delay(120);

    return HAL_OK;
}


void ls_MesureLight(LS_MODE mode)
{

}
