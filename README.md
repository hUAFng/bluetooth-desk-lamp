# bluetooth-desk-lamp

## 目录结构

`c   
[bluetooth-desk-lamp]/
├── Core/                          # 系统配置文件目录
│   ├── main.c                     # 主函数
│   
├── BSP/                           # 代码存放文件夹
│   ├── Applayer/                  # 上层业务应用逻辑
│   ├── Modulelayer/               # 中层外设模块逻辑
|          ├── TFT/                # TFT的相关驱动与封装
|               ├── tft_drv.c
|               ├── tft_fonts      # 字模
|          ├── Light_Sensor/       # 光照传感器的驱动与封装
|               ├── light_sensor_drv.c 
|          ├── RGB/                # 灯带
|               ├── DWT.c          # 灯带时序的微秒级延时函数来源
|               ├── rgb_drv.c      # 灯带的驱动代码
|   ├── Driverlayer/               # 底层驱动
|          ├── i2c_drv.c           # I2C驱动与封装
|          ├── key.c               # 按键驱动
|          ├── led.c               # led 驱动
|          ├── spi_drv.c           # SPI驱动
|          ├── uart_drv.c          # 串口驱动
|          ├── buzzer_drv.c        # 蜂鸣器驱动
|          ├── drv_layer.c         # 底层驱动代码的统一
├── data/                          # 数据目录
│   ├── raw/                       # 原始数据（未处理的源数据，只读）
│   └── processed/                 # 处理后的数据（可用于后续分析/输出）
├── bluetooth_desk_lamp.ioc        # cubemx的配置文件
└── README.md                      # 说明文档

`
