
#include "mic_drv.h"

#include "usart.h"    // ← 新增：串口句柄
#include "stdio.h"    // ← 新增：sprintf


static mic_t mic;

static const goertzel_coeff_t goertzel_table[GOERTZEL_FREQ_NUM] =
{
    {   80,    3,   1.9945766f  },
    {  200,    6,   1.9783594f  },
    {  400,   11,   1.9275524f  },
    {  800,   21,   1.7397463f  },
    { 1500,   39,   1.1506341f  },
    { 3000,   77,  -0.6213531f  },
    { 4500,  116,  -1.9174882f  },
};



static void mic_ClearBuf(void)
{
    mic.work_flag = 0;
    mic.dma_data_ready_flag = 0;

    mic.freq = 0.0f;
    mic.loudness = 0.0f;

    memset(mic.adc_dma_buf, 0, sizeof(mic.adc_dma_buf));
    memset(mic.adc_dma_buf_float, 0, sizeof(mic.adc_dma_buf_float));
}

void mic_Init(void)
{
    mic_PowerOn();
    HAL_Delay(200);

    mic_Calibrate();

    mic_PowerOff();
}

void mic_PowerOn(void)
{
    if (mic.work_flag) return;

    mic_ClearBuf();

    if (HAL_TIM_Base_Start(&htim3) != HAL_OK) Error_Handler();
    if (HAL_ADC_Start_DMA(&MIC_ADC_CHANNEL,(uint32_t *)mic.adc_dma_buf,MIC_ADC_DMA_BUF_LEN) != HAL_OK) 
        Error_Handler();

    mic.work_flag = 1;  
}

void mic_PowerOff(void)
{
    if (!mic.work_flag) return;

    mic.work_flag = 0;
    mic.dma_data_ready_flag = 0;

    if (HAL_TIM_Base_Stop(&htim3) != HAL_OK) Error_Handler();
    if (HAL_ADC_Stop_DMA(&MIC_ADC_CHANNEL) != HAL_OK) Error_Handler();
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if (hadc->Instance == MIC_ADC_CHANNEL.Instance)
    {
        mic.dma_data_ready_flag = 1; 
    }
}

void mic_GetFreq(float *freq)
{
    if (freq == NULL) return;

    *freq = mic.freq;
}

static void mic_dma_buf_to_float(void)
{
    for (uint16_t i = 0; i < MIC_ADC_DMA_BUF_LEN; i++)
    {
        mic.adc_dma_buf_float[i] = (float)(mic.adc_dma_buf[i] - 2048);
    }
}

void mic_Calibrate(void)
{
    float min_noise = 9999.0f;

    for (uint8_t j = 0; j < 5;j++)
    {
        while (!mic.dma_data_ready_flag);
        mic.dma_data_ready_flag = 0;

        mic_dma_buf_to_float();

        float energy = 0.0f;

        for (uint16_t i = 0; i < MIC_ADC_DMA_BUF_LEN; i++)
        {
            float s = mic.adc_dma_buf_float[i];
            energy += s * s;
        }
        float nf = sqrtf(energy / (float)MIC_ADC_DMA_BUF_LEN);

        if (nf < min_noise) min_noise = nf;
    }

    mic.noise_floor = min_noise;
}

static float mic_goertzel(uint8_t index)
{
    float coeff = goertzel_table[index].coeff;
    float q0 = 0, q1 = 0, q2 = 0;
	
	for(uint16_t i = 0;i < MIC_ADC_DMA_BUF_LEN;i++)
	{
		q0 = coeff * q1 - q2 + mic.adc_dma_buf_float[i];
		q2 = q1;
		q1 = q0;
	}
	
	return q1*q1 + q2*q2 - coeff *q1*q2;
}

static float mic_GetMaxfreq(float *total_energy_out)
{
    float max_power = 0;
	float best_freq = 0;
    float total_energy = 0;

    for(uint8_t i = 0;i < GOERTZEL_FREQ_NUM;i++)
    {
		float power = mic_goertzel(i);

        if (power < 0.0f) power = 0.0f;
        
        total_energy += power;
        
        if (power > max_power)
        {
            max_power = power;
            best_freq = goertzel_table[i].target_freq;
        }
    } 
    
    if (total_energy_out) *total_energy_out = total_energy;
    return best_freq;
}

static void mic_Getloudness(void)
{
    float energy = 0.0f;

    for (uint16_t i = 0; i < MIC_ADC_DMA_BUF_LEN; i++)
    {
        float s = mic.adc_dma_buf_float[i];
        energy += s * s;
    }

    float rms = sqrtf(energy / (float)MIC_ADC_DMA_BUF_LEN);

    mic.loudness = (rms > mic.noise_floor) ? (rms - mic.noise_floor) : 0.0f;
}

void mic_loudness_mapto_brightness(uint8_t brightness_max,uint8_t brightness_min,\
                                    uint8_t* brightness,uint8_t low_bright_area)
{
    if(brightness == NULL) return;

    float raw = mic.loudness * LOUDNESS_TO_BRIGHTNESS_GAIN * brightness_max / MIC_LOUDNESS_MAX;

    if (raw > brightness_max)               raw = brightness_max;
    else if (raw < brightness_min)          raw = brightness_min;

    if (raw < low_bright_area)
    {
        float range = low_bright_area - brightness_min;
        float t = (raw - brightness_min) / range;
        t = 1.0f - (1.0f - t) * (1.0f - t);
        raw = brightness_min + t * range;
    }

    *brightness = (uint8_t)raw;
}

static void mic_freq_filter(float* cnt_freq)
{
    if (cnt_freq == NULL) return;

    float delta_freq = *cnt_freq - mic.freq;

    if (fabsf(delta_freq) < 10.0f)
    {
        mic.freq = NARROW_FILTER * mic.freq + (1 - NARROW_FILTER) * *cnt_freq;
    }
    else if (delta_freq < 0.0f)
    {
        mic.freq = DOWN_DIRETION_FILTER * mic.freq + (1 - DOWN_DIRETION_FILTER) * *cnt_freq;
    }
    else if (delta_freq <= 100.0f)
    {
        mic.freq = MID_SPEED_FILTER * mic.freq + (1 - MID_SPEED_FILTER) * *cnt_freq;
    }
    else
    {
        mic.freq = HIGH_SPEED_FILTER * mic.freq + (1 - HIGH_SPEED_FILTER) * *cnt_freq;
    }
}

uint8_t mic_Run()
{
    if (!mic.dma_data_ready_flag) return 0;

    mic.dma_data_ready_flag = 0;

    mic_dma_buf_to_float();
    
    mic_Getloudness();

    float cnt_freq = 0;

    if (mic.loudness > LOUDNESS_GATE_THRESHOLD) 
    {
        float total_energy = 0;
        cnt_freq = mic_GetMaxfreq(&total_energy);

        if (total_energy > GOERTZEL_ENERGY_THRESHOLD)
        {
            mic_freq_filter(&cnt_freq);
        }
    }

    return 1;
}

void mic_GetLoudness(float *loudness)
{
    if (loudness == NULL) return;
    *loudness = mic.loudness;
}


void mic_DebugDumpToUART(void)
{
    char buf[128];
    uint16_t len;
    uint16_t samples[16];  // 阻塞采集16个样本

    HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n===== MIC ADC DIAGNOSE =====\r\n", 31, 200);

    /* ---- 方法A: 停止DMA，做16次阻塞ADC采集（不依赖DMA是否循环） ---- */
    HAL_ADC_Stop_DMA(&hadc1);          // 暂时停止DMA
    HAL_ADC_Start(&hadc1);             // 启动阻塞模式ADC

    uint32_t sum = 0;
    uint16_t min_v = 4095, max_v = 0;

    for (uint8_t i = 0; i < 16; i++)
    {
        HAL_ADC_PollForConversion(&hadc1, 10);   // 等待转换完成
        samples[i] = (uint16_t)HAL_ADC_GetValue(&hadc1);  // 读取值

        if (samples[i] < min_v) min_v = samples[i];
        if (samples[i] > max_v) max_v = samples[i];
        sum += samples[i];

        len = sprintf(buf, "  blk[%2d] = %4u\r\n", i, samples[i]);
        HAL_UART_Transmit(&huart1, (uint8_t*)buf, len, 200);
    }
    HAL_ADC_Stop(&hadc1);    // 停止阻塞模式

    uint16_t avg = (uint16_t)(sum / 16);
    uint16_t range = max_v - min_v;

    len = sprintf(buf, "\r\n  min=%u  max=%u  range=%u  avg=%u\r\n",
                  min_v, max_v, range, avg);
    HAL_UART_Transmit(&huart1, (uint8_t*)buf, len, 200);

    /* ---- 方法B: 恢复DMA采集，打印noise_floor和loudness ---- */
    mic.dma_data_ready_flag = 0;
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)mic.adc_dma_buf, MIC_ADC_DMA_BUF_LEN);

    // 等一帧DMA数据
    uint32_t tick = HAL_GetTick();
    while (!mic.dma_data_ready_flag)
    {
        if (HAL_GetTick() - tick > 100) break;  // 超时100ms
    }

    if (mic.dma_data_ready_flag)
    {
        mic_dma_buf_to_float();   // 转float
        mic_Getloudness();        // 计算响度

        int nf_int = (int)(mic.noise_floor * 10);
        int ld_int = (int)(mic.loudness * 10);
        len = sprintf(buf, "\r\n  noise_floor=%d.%d  loudness=%d.%d  freq=%d\r\n",
                      nf_int / 10, nf_int % 10,
                      ld_int / 10, ld_int % 10,
                      (int)mic.freq);
        HAL_UART_Transmit(&huart1, (uint8_t*)buf, len, 200);
    }
    else
    {
        HAL_UART_Transmit(&huart1, (uint8_t*)"  DMA timeout!\r\n", 16, 200);
    }

    /* ---- 结论 ---- */
    if (range < 5)
    {
        HAL_UART_Transmit(&huart1,
            (uint8_t*)"\r\n>>> WRONG: ADC值几乎冻结! 检查以下硬件:\r\n"
            "  1. 麦克风VCC/GND供电\r\n"
            "  2. 麦克风偏置电阻\r\n"
            "  3. ADC输入引脚连接\r\n", 118, 200);
    }
    else if (range < 40)
    {
        HAL_UART_Transmit(&huart1,
            (uint8_t*)"\r\n>>> WEAK: 有信号但很微弱，贴近麦克风大声说话看range是否变大\r\n", 63, 200);
    }
    else
    {
        HAL_UART_Transmit(&huart1,
            (uint8_t*)"\r\n>>> OK: ADC正常采集到信号!\r\n", 32, 200);
        HAL_UART_Transmit(&huart1,
            (uint8_t*)"  如果灯带仍然不亮，问题在 loudness/freq 门控参数上\r\n", 54, 200);
    }

    HAL_UART_Transmit(&huart1, (uint8_t*)"===== END =====\r\n\r\n", 19, 200);
}