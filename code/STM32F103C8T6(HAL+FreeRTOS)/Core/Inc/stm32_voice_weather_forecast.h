/*
 * @Description: 
 * @Author: TOTHTOT
 * @Date: 2024-05-01 17:41:37
 * @LastEditTime: 2024-05-01 19:58:59
 * @LastEditors: TOTHTOT
 * @FilePath: \stm32_voice_weather_forecast\code\STM32F103C8T6(HAL+FreeRTOS)\Core\Inc\stm32_voice_weather_forecast.h
 */
#ifndef __STM32_VOICE_WEATHER_FORECAST_H__
#define __STM32_VOICE_WEATHER_FORECAST_H__

/* 头文件 */
#include "main.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "usart1.h"
#include "syn6288.h"
#include "oled.h"

/* 宏定义 */
#define DEVICE_BUILD_DATE __DATE__
#define DEVICE_BUILD_TIME __TIME__
#define ESP_01S_WIFI_NAME "esp-2.4G"    // esp 连接wifi名称
#define ESP_01S_WIFI_PASSWORD "12345678"    // esp 连接wifi密码
#define ESP_01S_SERVER_IP "192.168.1.100"
#define ESP_01S_SERVER_PORT 8080
#define ESP_01S_REQUEST_URL "/api/weather/now.json?location=beijing&key=1234567890"


/* 类型定义 */
typedef struct time_info
{
    uint16_t year;  // 年
    uint8_t month;  // 月
    uint8_t day;    // 日
    uint8_t hour;   // 时
    uint8_t minute; // 分
    uint8_t second; // 秒
} time_info_t;

typedef struct stm32_voice_weather_forecast
{
    struct devices
    {
        syn6288_device_t *p_syn6288_dev_st;
        struct oled_device *p_oled_dev_st;
    } devices_info;

    struct weather_info
    {
        uint8_t temperature;   // 温度
        uint8_t humidity;      // 湿度
        uint8_t weather;       // 天气
        uint8_t wind_speed;    // 风速
        time_info_t time_info; // 时间
    } weather_info_st[3];

    bool system_is_ready; // == true 表示系统已经初始化完成, 其他任务才能开始执行
} stm32_voice_weather_forecast_t;

/* 全局变量 */
extern stm32_voice_weather_forecast_t g_stm32_voice_weather_forecast_st;

/* 全局函数 */
extern uint8_t stm32_voice_weather_forecast_init(stm32_voice_weather_forecast_t *p_dev_st);

#endif /* __STM32_VOICE_WEATHER_FORECAST_H__ */
