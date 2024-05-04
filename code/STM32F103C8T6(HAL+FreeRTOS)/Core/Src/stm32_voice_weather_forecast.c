/*
 * @Description: 基于stm32的智能语音天气预报系统
 * @Author: TOTHTOT
 * @Date: 2024-05-01 17:41:28
 * @LastEditTime: 2024-05-04 17:36:35
 * @LastEditors: TOTHTOT
 * @FilePath: \stm32_voice_weather_forecast\code\STM32F103C8T6(HAL+FreeRTOS)\Core\Src\stm32_voice_weather_forecast.c
 */
#include "stm32_voice_weather_forecast.h"
#include "delay.h"
#include "test_u8g2.h"

/* 全局变量 */
stm32_voice_weather_forecast_t g_stm32_voice_weather_forecast_st = {
    .devices_info.p_syn6288_dev_st = &g_syn6288_device_st,
    .devices_info.p_esp_at_dev_st = &g_esp_at_st,
    .time_info.year = 2024,
    .time_info.month = 5,
    .time_info.day = 1,
    .time_info.hour = 17,
    .time_info.minute = 41,
    .time_info.second = 28,
    //.devices_info.p_oled_dev_st = &g_oled_device_st,
};

/**
 * @name: stm32_voice_weather_forecast_init
 * @msg: 初始化设备
 * @param {stm32_voice_weather_forecast_t} *p_dev_st
 * @return {*}
 * @author: TOTHTOT
 * @Date: 2024-05-04 17:17:16
 */
uint8_t stm32_voice_weather_forecast_init(stm32_voice_weather_forecast_t *p_dev_st)
{
    uint8_t ret = 0;
    char error_buf[50] = {0};

    INFO_PRINT("stm32_voice_weather_forecast: %s, %s\n", DEVICE_BUILD_DATE, DEVICE_BUILD_TIME);

    u8g2_init(&p_dev_st->devices_info.u8g2);
    u8g2_ClearBuffer(&p_dev_st->devices_info.u8g2);
    u8g2_SetFont(&p_dev_st->devices_info.u8g2, u8g2_font_t0_11_te);
    u8g2_DrawStr(&p_dev_st->devices_info.u8g2, 5, 20, "start init, soft ver:");
    u8g2_DrawStr(&p_dev_st->devices_info.u8g2, 5, 40, DEVICE_BUILD_DATE);
    u8g2_DrawStr(&p_dev_st->devices_info.u8g2, 5, 60, DEVICE_BUILD_TIME);
    u8g2_SendBuffer(&p_dev_st->devices_info.u8g2); // 发送缓冲区内容到 OLED 显示屏

    if ((ret = esp_at_cmd_init(p_dev_st->devices_info.p_esp_at_dev_st)) != 0)
    {
        goto ERROR_PRINT;
    }

    u8g2_ClearBuffer(&p_dev_st->devices_info.u8g2);
    u8g2_SetFont(&p_dev_st->devices_info.u8g2, u8g2_font_t0_11_te);
    u8g2_DrawStr(&p_dev_st->devices_info.u8g2, 5, 20, "init success");
    u8g2_SendBuffer(&p_dev_st->devices_info.u8g2); // 发送缓冲区内容到 OLED 显示屏
    INFO_PRINT("weather:%s\r\n", p_dev_st->devices_info.p_esp_at_dev_st->uart_info_st.rxbuf);
    delay_xms(500);

    u8g2_refresh_scr(p_dev_st);

#if 0
    u8g2_FirstPage(&p_dev_st->devices_info.u8g2);
    do
    {
        u8g2_draw(&p_dev_st->devices_info.u8g2);

        u8g2DrawTest(&p_dev_st->devices_info.u8g2);
    } while (u8g2_NextPage(&p_dev_st->devices_info.u8g2));
#endif
    delay_xms(100);
    return 0;

ERROR_PRINT:
    sprintf(error_buf, "init fail ret = %d", ret);
    u8g2_ClearBuffer(&p_dev_st->devices_info.u8g2);
    u8g2_SetFont(&p_dev_st->devices_info.u8g2, u8g2_font_t0_11_te);
    u8g2_DrawStr(&p_dev_st->devices_info.u8g2, 5, 20, error_buf);
    u8g2_SendBuffer(&p_dev_st->devices_info.u8g2); // 发送缓冲区内容到 OLED 显示屏
    // p_dev_st->devices_info.p_syn6288_dev_st->ops_func.send_frame_info(p_dev_st->devices_info.p_syn6288_dev_st, 0, (uint8_t *)error_buf);
ERROR_RETURN:
    return ret;
}
