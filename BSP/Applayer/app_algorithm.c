

#include "app_algorithm.h"


/**
 * @brief 映射光照强度到灯带亮度 低通滤波
 */
void remap_lux_to_brightness(void)
{
    uint8_t brightness = system.system_data.rgb_data.brightness;
    uint16_t lux = system.system_data.ls_data.lux;
    float brightness_filter = system.system_data.rgb_data.filter_lux;

    brightness = (uint8_t)(brightness_filter*(lux * RGB_MAX_BRIGHTNESS / LS_LUX_MAX) + (1-brightness_filter)*brightness);

    system.system_data.rgb_data.brightness = brightness;
}