#include "rgb_drv.h"

/* --------------------------------  颜色的RGB通道值 -------------------------------- */

static const RGB_ColorTypeDef_t rgb_color[] =
{
    {  0, 255,  0 },
    {255,   0,  0 },
    {  0,   0, 255 },
    {255, 255, 255 },
    {255, 223, 186},
    {255, 255,  0 },
    {  0, 255, 255 },
    {255,   0, 255 },
    {128, 255,  0 },
    { 60, 255, 180 },
};

static RGB_TypeDef_t rgb = {
    .is_sending = 0,
    .music_target_r = 0,
    .music_target_g = 0,
    .music_target_b = 0,
    .music_smooth_r = 0,
    .music_smooth_g = 0,
    .music_smooth_b = 0,
};

#define BIT_1 60
#define BIT_0 30
/*-------------------------------------function---------------------------------------*/

void rgb_ClearBuffer(void)
{
    for (uint8_t i = 0; i < RGB_LED_NUM; i++)
    {
        rgb.rgb_led_buf[i] = 0;
    }
	
	rgb.is_sending = 0;
}

void rgb_Init(void)
{
    HAL_GPIO_WritePin(RGB_GPIO_Port, RGB_Pin, GPIO_PIN_RESET);

    rgb_ClearBuffer();
    rgb_PowerOff();
}

void rgb_SendBit(void)
{
	if (rgb.is_sending) return;
	
	uint16_t index = 0;
	
	for (uint8_t i = 0;i < RGB_LED_NUM;i++)
	{
		for(int8_t j = 23;j >= 0;j--)
		{
			if(rgb.rgb_led_buf[i] & (1 << j))
			{
				rgb.pwm_pulse_dma_buf[index] = BIT_1;
			}
			else 
			{
				rgb.pwm_pulse_dma_buf[index] = BIT_0;
			}
			
			index++;
		}
	}
	
	for(int i = 0;i < 50;i++) rgb.pwm_pulse_dma_buf[index++] = 0;
		
	rgb.is_sending = 1;
    HAL_TIM_PWM_Start_DMA(&htim1,TIM_CHANNEL_1,(uint32_t *)rgb.pwm_pulse_dma_buf,index);
}


void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == htim1.Instance)
	{
		rgb.is_sending = 0;
		HAL_TIM_PWM_Stop_DMA(&htim1,TIM_CHANNEL_1);
	}
}

void rgb_PowerOff(void)
{
	memset(rgb.rgb_led_buf,0,sizeof(rgb.rgb_led_buf));
	rgb_SendBit();
}

void rgb_SetSingleLed(uint8_t led_index , RGB_Color_e color,uint8_t brightness)
{
    if (led_index >= RGB_LED_NUM) return;
    if (brightness < RGB_MIN_BRIGHTNESS) brightness = RGB_MIN_BRIGHTNESS;
    else if (brightness > RGB_MAX_BRIGHTNESS) brightness = RGB_MAX_BRIGHTNESS;

    uint8_t color_index = color;

    RGB_ColorTypeDef_t temp_led_color_buf;
    temp_led_color_buf.green = (uint8_t)((rgb_color[color_index].green * brightness) >> 8);
    temp_led_color_buf.red = (uint8_t)((rgb_color[color_index].red * brightness) >> 8);
    temp_led_color_buf.blue = (uint8_t)((rgb_color[color_index].blue * brightness) >> 8);

    rgb.rgb_led_buf[led_index] = temp_led_color_buf.green << 16 | temp_led_color_buf.red << 8 | temp_led_color_buf.blue;
}


void rgb_SetAllLed(RGB_Color_e color,uint8_t brightness)
{
    rgb.cnt_color = color;
    rgb.cnt_brightness = brightness;

    for (uint8_t i = 0;i < RGB_LED_NUM;i++) rgb_SetSingleLed(i,color,brightness);
}

void rgb_update(void)
{
	rgb_SendBit();
}

void rgb_Display(RGB_Color_e color,uint8_t brightness)
{
    if (color >= RGB_COLOR_NUM) return;
    rgb_SetAllLed(color,brightness);
}

void rgb_SetBrightness(uint8_t brightness)
{
    rgb_SetAllLed(rgb.cnt_color,brightness);
}

void rgb_SetBrightness_Circle(uint8_t* brightness)
{
    if (brightness == NULL) return;

    if (*brightness >= RGB_MAX_BRIGHTNESS) *brightness = RGB_MIN_BRIGHTNESS;
    else *brightness += RGB_BRIGHTNESS_STEP;

    rgb.cnt_brightness = *brightness;

    rgb_SetBrightness(rgb.cnt_brightness);
}

void rgb_SetBrightnessUp(uint8_t* brightness)
{
    if (brightness == NULL) return;

    *brightness += RGB_BRIGHTNESS_STEP; 

    if (*brightness > RGB_MAX_BRIGHTNESS) *brightness = RGB_MAX_BRIGHTNESS;

    rgb.cnt_brightness = *brightness;
    
    rgb_SetBrightness(rgb.cnt_brightness);
}

void rgb_SetBrightnessDown(uint8_t* brightness)
{
    if (brightness == NULL) return;

    if (*brightness <= RGB_MIN_BRIGHTNESS + RGB_BRIGHTNESS_STEP)
        rgb.cnt_brightness = RGB_MIN_BRIGHTNESS;
    else
        rgb.cnt_brightness = *brightness - RGB_BRIGHTNESS_STEP;
    
    *brightness = rgb.cnt_brightness;
    
    rgb_SetBrightness(rgb.cnt_brightness);
}

void rgb_SetColor(RGB_Color_e color)
{
    rgb_SetAllLed(color,rgb.cnt_brightness);
}

void rgb_SetColor_Circle(RGB_Color_e* color)
{
    if (color == NULL) return;
    (*color)++;
    
    if (*color >= RGB_COLOR_NUM) *color = 0;

    rgb.cnt_color = *color;
    rgb_SetColor(rgb.cnt_color);
}

void rgb_PowerOn(RGB_Color_e color,uint8_t brightness)
{
    rgb_ClearBuffer();
    
    rgb_Display(color,brightness);
	
	rgb_update();
}


/*------------------------------------------------------------------------------
                                    音乐模式
------------------------------------------------------------------------------*/

const uint8_t LOW_MIN_COLOR[3] = {  64,   0,   0 };
const uint8_t LOW_MAX_COLOR[3] = { 255, 165,   0 };

const uint8_t MID_MIN_COLOR[3] = {  0,  128,   0 };
const uint8_t MID_MAX_COLOR[3] = {255, 255,   0 };

const uint8_t HIGH_MIN_COLOR[3] = {  0,   0, 128 };
const uint8_t HIGH_MAX_COLOR[3] = {128,   0, 128 };


void rgb_brightness_filter(uint8_t *brightness)
{
    if (brightness == NULL) return;

    int16_t delta_bright = (int16_t)*brightness - (int16_t)rgb.cnt_brightness;

    float filter;
    if (delta_bright > 0)
    {
        filter = MUSIC_BRIGHT_ATTACK_FILTER;
    }
    else
    {
        filter = MUSIC_BRIGHT_RELEASE_FILTER;
    }

    rgb.cnt_brightness = (uint8_t)(filter * rgb.cnt_brightness + (1.0f - filter) * (*brightness));
}

void rgb_MapFreqToRGBValue()
{
    float freq = 0.0f;
    mic_GetFreq(&freq);

    if (freq >= 1.0f)
    {
        const uint8_t* min_color_ptr;
        const uint8_t* max_color_ptr;
        uint8_t freq_pos;

        if(freq <= LOW_FREQ_THRESHOLD)
        {
            min_color_ptr = LOW_MIN_COLOR;
            max_color_ptr = LOW_MAX_COLOR;
            freq_pos = (uint8_t)(freq / LOW_FREQ_THRESHOLD * 255.0f);
        }
        else if(freq <= MID_FREQ_THRESHOLD)
        {
            min_color_ptr = MID_MIN_COLOR;
            max_color_ptr = MID_MAX_COLOR;
            freq_pos = (uint8_t)((freq - LOW_FREQ_THRESHOLD) / (MID_FREQ_THRESHOLD - LOW_FREQ_THRESHOLD) * 255.0f);
        }
        else
        {
            min_color_ptr = HIGH_MIN_COLOR;
            max_color_ptr = HIGH_MAX_COLOR;
            freq_pos = (uint8_t)((freq - MID_FREQ_THRESHOLD) / (MIC_GOERTZEL_MAX_FREQ - MID_FREQ_THRESHOLD) * 255.0f);
            if (freq_pos > 255) freq_pos = 255;
        }

        uint8_t delta_r = max_color_ptr[0] - min_color_ptr[0];
        uint8_t delta_g = max_color_ptr[1] - min_color_ptr[1];
        uint8_t delta_b = max_color_ptr[2] - min_color_ptr[2];

        uint8_t base_r = min_color_ptr[0] + ((delta_r * freq_pos) >> 8);
        uint8_t base_g = min_color_ptr[1] + ((delta_g * freq_pos) >> 8);
        uint8_t base_b = min_color_ptr[2] + ((delta_b * freq_pos) >> 8);

        float loudness = 0.0f;
        mic_GetLoudness(&loudness);

        float loudness_ratio = loudness * LOUDNESS_TO_BRIGHTNESS_GAIN / MIC_LOUDNESS_MAX;
        if (loudness_ratio > 1.0f) loudness_ratio = 1.0f;

        float quiet_r = (float)base_r * 0.30f;
        float quiet_g = (float)base_g * 0.30f;
        float quiet_b = (float)base_b * 0.30f;

        rgb.music_target_r = (uint8_t)(quiet_r + (base_r - quiet_r) * loudness_ratio);
        rgb.music_target_g = (uint8_t)(quiet_g + (base_g - quiet_g) * loudness_ratio);
        rgb.music_target_b = (uint8_t)(quiet_b + (base_b - quiet_b) * loudness_ratio);

        int16_t dr = (int16_t)rgb.music_target_r - (int16_t)rgb.music_smooth_r;
        int16_t dg = (int16_t)rgb.music_target_g - (int16_t)rgb.music_smooth_g;
        int16_t db = (int16_t)rgb.music_target_b - (int16_t)rgb.music_smooth_b;

        float filter_r = (dr >= 0) ? MUSIC_COLOR_SMOOTH_UP : MUSIC_COLOR_SMOOTH_DOWN;
        float filter_g = (dg >= 0) ? MUSIC_COLOR_SMOOTH_UP : MUSIC_COLOR_SMOOTH_DOWN;
        float filter_b = (db >= 0) ? MUSIC_COLOR_SMOOTH_UP : MUSIC_COLOR_SMOOTH_DOWN;

        rgb.music_smooth_r = (uint8_t)(filter_r * rgb.music_smooth_r + (1.0f - filter_r) * rgb.music_target_r);
        rgb.music_smooth_g = (uint8_t)(filter_g * rgb.music_smooth_g + (1.0f - filter_g) * rgb.music_target_g);
        rgb.music_smooth_b = (uint8_t)(filter_b * rgb.music_smooth_b + (1.0f - filter_b) * rgb.music_target_b);
    }

    uint16_t bright = rgb.cnt_brightness;
    uint8_t r = (uint8_t)(((uint16_t)rgb.music_smooth_r * bright) >> 8);
    uint8_t g = (uint8_t)(((uint16_t)rgb.music_smooth_g * bright) >> 8);
    uint8_t b = (uint8_t)(((uint16_t)rgb.music_smooth_b * bright) >> 8);

    for (uint8_t i = 0; i < RGB_LED_NUM; i++)
    {
        rgb.rgb_led_buf[i] = (g << 16) | (r << 8) | b;
    }
}

void rgb_RunInMusic(void)
{
    uint8_t cnt_bright = 0;

    mic_loudness_mapto_brightness(RGB_MAX_BRIGHTNESS_MUSIC,RGB_MIN_BRIGHTNESS_MUSIC,&cnt_bright,RGB_MUSIC_BRIGHT_LOW_AREA);
    
    rgb_brightness_filter(&cnt_bright);

    rgb_MapFreqToRGBValue();
}
