

#include "light_sensor_drv.h"


/* 使用不同的阈值 防止模式跳变*/
#define LOW_LUS_THRESHOLD 10.0f    // 低光照阈值,切换到0.5lx分辨率
#define HIGH_LUS_THRESHOLD 15.0f   // 高光照阈值,切换到1lx分辨率

HAL_StatusTypeDef ls_WriteByte(uint8_t byte)
{
    return  I2C_WriteRaw(&LS_I2C_Handle,LS_I2C_ADDR,&byte,1);
}
HAL_StatusTypeDef ls_Readdata(uint8_t* buf , uint8_t len)
{
    return I2C_ReadRaw(&LS_I2C_Handle,LS_I2C_ADDR,buf,len);

}
void ls_Init(void)
{
    ls_WriteByte(LS_POWON);      //上电
    HAL_Delay(10); 

    ls_WriteByte(LS_RESET);      //重置
    HAL_Delay(10);

    ls_PowerOff(); // 默认不进入工作模式，保持功耗最低
}

void ls_PowerOn(void)
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
    uint8_t temp = 0;
    if (mode == LS_MODE_HRES1) temp = LS_HRES_MODE1;
    else if (mode == LS_MODE_HRES2) temp = LS_HRES_MODE2;
    else if (mode == LS_MODE_LRES) temp = LS_LRES_MODE;
    else if (mode == LS_MODE_SINGLE_MEAS) temp = LS_SINGLE_MEAS_MODE;
    else return HAL_ERROR; // 无效模式
    
    if (ls_WriteByte(temp) != HAL_OK) return HAL_ERROR;

    return HAL_OK;
}

/**
 * @brief 测量光线强度
 * @param mode : 测量模式
 * @param lux :  存储测量结果的指针(光照强度单位为lx)
 */
HAL_StatusTypeDef ls_MeasureLight(LS_MODE* mode,float* lux)
{
    if(mode == NULL || lux == NULL || *mode < LS_MODE_HRES1 || *mode > LS_MODE_SINGLE_MEAS) return HAL_ERROR;

    uint8_t buf[2] = {0};
    float lux_raw = 0.0f;

    if (ls_Readdata(buf,2) != HAL_OK) return HAL_ERROR;

    lux_raw = (buf[0] << 8) | buf[1]; //合成16位原始数据

    // 计算公式
    if (*mode == LS_MODE_HRES2) *lux = lux_raw / 1.2f / 2.0f; // 0.5lx分辨率 
    else *lux = lux_raw / 1.2f; // 其他模式分辨率

    ls_ChangeModeByLus(lux,mode);

    HAL_Delay(130); // 测量间隔（周期）低精度可小点，这里统一处理

    return HAL_OK;
}

/**
 * @brief 根据环境的测量结果来切换测量模式，适应不同的光照强度范围与测量精度，增强灯珠的控制效果
 * @param lux : 光照强度指针
 * @param cnt_mode : 当前模式指针
 */
HAL_StatusTypeDef ls_ChangeModeByLus(float* lux,LS_MODE* cnt_mode)
{
    if (lux == NULL || cnt_mode == NULL) return HAL_ERROR;

    if (*cnt_mode == LS_MODE_HRES1) // 1lx分辨率
    {
        if (*lux <= LOW_LUS_THRESHOLD) 
        {
            *cnt_mode = LS_MODE_HRES2;
            return ls_SetMode(LS_MODE_HRES2); // 0.5lx分辨率,提高弱光环境的测量精度
        }
    }
    else if (*cnt_mode == LS_MODE_HRES2) // 0.5lx分辨率
    {
        if (*lux >= HIGH_LUS_THRESHOLD) 
        {
            *cnt_mode = LS_MODE_HRES1;
            return ls_SetMode(LS_MODE_HRES1); // 1lx分辨率
        }
    }
    else return HAL_ERROR;

    return HAL_OK;
}

void ls_PowerOff(void)
{
    // 关闭光线传感器电源
    ls_WriteByte(LS_POWOFF);
}

void ls_Reset(void)
{
    ls_Poweroff();

    HAL_Delay(10);

    ls_Init();
}
