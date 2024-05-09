/*
 * @Description: 基于stm32的智能语音天气预报系统
 * @Author: TOTHTOT
 * @Date: 2024-05-01 17:41:37
 * @LastEditTime: 2024-05-08 20:57:22
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
#include "stm32_u8g2.h"
#include "esp_at_cmd.h"
#include "ld3320.h"

/* 宏定义 */
#define DEVICE_BUILD_DATE __DATE__
#define DEVICE_BUILD_TIME __TIME__
#define DEFAULT_THR_DELAY_TIME_MS 10 // StartDefaultTask() 线程延时时间, 单位:ms

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

typedef struct weather_info
{
    uint8_t temperature_high; // 温度
    uint8_t temperature_low;  // 温度
    uint8_t humidity;         // 湿度
    uint8_t weather[20];      // 天气
    uint8_t weather_time[20]; // 天气对应时间
    float wind_speed;         // 风速
} weather_info_t;

typedef struct stm32_voice_weather_forecast
{
    struct devices
    {
        syn6288_device_t *p_syn6288_dev_st;
        u8g2_t u8g2;
        esp_at_t *p_esp_at_dev_st;
        ld3320_device_t *p_ld3320_dev_st;
    } devices_info;

    weather_info_t weather_info_st[3]; // 天气信息, 0: 昨天, 1: 今天, 2: 明天;
    time_info_t time_info;             // 时间
    enum
    {
        WEATHER_INFO_INDEX_YESTERDAY,
        WEATHER_INFO_INDEX_TODAY,
        WEATHER_INFO_INDEX_TOMORROW
    } cur_show_weather_info_index; // 当前显示的天气, 0: 昨天, 1: 今天, 2: 明天;

    bool system_is_ready; // == true 表示系统已经初始化完成, 其他任务才能开始执行
} stm32_voice_weather_forecast_t;

/* 全局变量 */
extern stm32_voice_weather_forecast_t g_stm32_voice_weather_forecast_st;

/* 全局函数 */
extern uint8_t stm32_voice_weather_forecast_init(stm32_voice_weather_forecast_t *p_dev_st);
extern uint8_t stm32_voice_weather_forecast_analysis_ld3320_data(const uint8_t *data, char *result);
extern uint8_t stm32_voice_weather_forecast_analysis_json_weather(const char *json_data, void *private_data, uint8_t weather_info_len);

#endif /* __STM32_VOICE_WEATHER_FORECAST_H__ */
