

#ifndef __LIGHT_SENSOR_DRV_H__
#define __LIGHT_SENSOR_DRV_H__

#include "main.h"
#include "i2c.h"
#include "i2c_drv.h"

extern I2C_HandleTypeDef hi2c1;

/* -------------------------------------define ------------------------------------- */

#define LS_I2C_ADDR (0x23 << 1)   //光传感器I2C地址
#define LS_I2C_Handle hi2c1

#define LS_POWON 0x01   //上电
#define LS_POWOFF 0x00  //断电-进入低功耗模式
#define LS_RESET 0x02   //重置
#define LS_HRES_MODE1 0x10  //连续高分辨率模式1 - 1lx分辨率,测量时间120ms,默认常用模式 量程大
#define LS_HRES_MODE2 0x11  //连续高分辨率模式2 - 0.5lx分辨率,测量时间120ms,适合弱光环境 
#define LS_LRES_MODE 0x13   //连续低分辨率模式 - 4lx分辨率,测量时间16ms,适合快速采样
#define LS_SINGLE_MEAS_MODE 0x20  //单次测量模式，测量一次后自动断电,适合低功耗场景

#define LS_LUX_MAX 400 // 设置常见的最大光照强度为300lx，根据实际应用调整范围


typedef enum
{
    LS_MODE_HRES1 = 0,
    LS_MODE_HRES2,
    LS_MODE_LRES,
    LS_MODE_SINGLE_MEAS
}LS_MODE;
        


/* -------------------------------------function-------------------------------------- */
void ls_Init(void);
HAL_StatusTypeDef ls_SetMode(LS_MODE mode);
HAL_StatusTypeDef ls_MeasureLight(LS_MODE* mode,float* lux);
HAL_StatusTypeDef ls_ChangeModeByLus(float* lux,LS_MODE* cnt_mode);
void ls_Reset(void);
void ls_PowerOff(void);
void ls_PowerOn(void);



#endif