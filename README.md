# bluetooth-desk-lamp

## 目录结构

```c   
[bluetooth-desk-lamp]/
├── Core/                          # 系统配置文件目录
│   ├── main.c                     # 主函数
│   
├── BSP/                           # 代码存放文件夹
│   ├── Applayer/                  # 上层业务应用逻辑
|          ├── app_algotithm.c     # 应用层的部分算法文件
|          ├── app_common.c        # 应用层的系统变量定义与初始化
|          ├── app_manager.c       # 应用层的管理函数，包括整个系统的初始化与运行、控制
|          ├── app_modes.c         # 系统不同模式下的运行函数的封装 
|          ├── app_ui.c            # 系统的GUI交互函数
|
│   ├── Modulelayer/               # 中层外设模块逻辑
|          ├── TFT/                # TFT的相关驱动与封装
|               ├── tft_drv.c
|               ├── tft_fonts      # 字模
|          ├── Light_Sensor/       # 光照传感器的驱动与封装
|               ├── light_sensor_drv.c 
|          ├── RGB/                # 灯带
|               ├── DWT.c          # 灯带时序的微秒级延时函数来源
|               ├── rgb_drv.c      # 灯带的驱动代码
|          ├── ASRPRO/             # 语音识别模块
|               ├── asrpro_drv.c   # 语音识别模块的驱动代码
|          ├── MIC/                # 麦克风音乐律动模块
|               ├── mic_drv.c      # 麦克风驱动
|          ├── BlueTooth/          # 蓝牙模块
|               ├── bt_drv.c       # 蓝牙模块驱动
|
|   ├── Driverlayer/               # 底层驱动
|          ├── i2c_drv.c           # I2C驱动与封装
|          ├── key.c               # 按键驱动
|          ├── led.c               # led 驱动
|          ├── spi_drv.c           # SPI驱动
|          ├── uart_drv.c          # 串口驱动
|          ├── buzzer_drv.c        # 蜂鸣器驱动
|          ├── drv_layer.c         # 底层驱动代码的统一
|
├── bluetooth_desk_lamp.ioc        # cubemx的配置文件
└── README.md                      # 说明文档

```

    
