/*
 * @Description: 基于stm32的智能语音天气预报系统
 * @Author: TOTHTOT
 * @Date: 2024-05-01 17:41:28
 * @LastEditTime: 2024-05-01 19:47:19
 * @LastEditors: TOTHTOT
 * @FilePath: \stm32_voice_weather_forecast\code\STM32F103C8T6(HAL+FreeRTOS)\Core\Src\stm32_voice_weather_forecast.c
 */
#include "stm32_voice_weather_forecast.h"

/* 全局变量 */
stm32_voice_weather_forecast_t g_stm32_voice_weather_forecast_st = {
    .devices_info.p_syn6288_dev_st = &g_syn6288_device_st,
    .devices_info.p_oled_dev_st = &g_oled_device_st,
};

uint8_t stm32_voice_weather_forecast_init(stm32_voice_weather_forecast_t *p_dev_st)
{
    INFO_PRINT("stm32_voice_weather_forecast: %s, %s\n", DEVICE_BUILD_DATE, DEVICE_BUILD_TIME);

    OLED_Init(p_dev_st->devices_info.p_oled_dev_st);
	delay_xms(100);
    p_dev_st->devices_info.p_syn6288_dev_st->ops_func.send_frame_info(p_dev_st->devices_info.p_syn6288_dev_st, 0, (uint8_t *)"你好");
    return 0;
}

