/*
 * @Description: 基于stm32的智能语音天气预报系统
 * @Author: TOTHTOT
 * @Date: 2024-05-01 17:41:28
 * @LastEditTime: 2024-05-10 21:34:06
 * @LastEditors: TOTHTOT
 * @FilePath: \stm32_voice_weather_forecast\code\STM32F103C8T6(HAL+FreeRTOS)\Core\Src\stm32_voice_weather_forecast.c
 */
#include "stm32_voice_weather_forecast.h"
#include "delay.h"
#include "test_u8g2.h"
#include "cJSON.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

/* 全局变量 */
stm32_voice_weather_forecast_t g_stm32_voice_weather_forecast_st = {
    // 使用的设备初始化
    .devices_info.p_syn6288_dev_st = &g_syn6288_device_st,
    .devices_info.p_esp_at_dev_st = &g_esp_at_st,
    .devices_info.p_ld3320_dev_st = &g_ld3320_device_st,
    // 默认显示时间
    .time_info.year = 2024,
    .time_info.month = 5,
    .time_info.day = 1,
    .time_info.hour = 17,
    .time_info.minute = 41,
    .time_info.second = 28,
};

static void parse_datetime(const char *datetime_str, time_info_t *time_info) {
    sscanf(datetime_str, "%hu-%hhu-%hhu %hhu:%hhu:%hhu",
           &time_info->year, &time_info->month, &time_info->day,
           &time_info->hour, &time_info->minute, &time_info->second);
    time_info->millisecond = 0; // 毫秒默认为 0，你也可以根据需要进行解析
}

/**
 * @name: stm32_voice_weather_forecast_analysis_json_time
 * @msg: 解析时间信息
 * @param {char} *json_data 串口数据
 * @param {void} *private_data 时间结构体
 * @return { == 0 成功; != 0 失败}
 * @author: TOTHTOT
 * @Date: 2024-05-10 14:21:42
 */
uint8_t stm32_voice_weather_forecast_analysis_json_time(char *json_data, void *private_data)
{
    time_info_t *p_time_info_st = (time_info_t *)private_data;

    if (json_data == NULL || private_data == NULL)
    {
        ERROR_PRINT("json_data or private_data is NULL!\n");
        return 3;
    }
     // 解析 JSON 字符串
    cJSON *root = cJSON_Parse((char *)json_data);
    if (root == NULL)
    {
        INFO_PRINT("Error parsing JSON: %s\n", cJSON_GetErrorPtr());
        return 1;
    }

    // 访问 JSON 数据
    // 从根节点中获取 result 对象
    cJSON *result = cJSON_GetObjectItem(root, "result");
    if (result == NULL) {
        ERROR_PRINT("result object not found!\n");
        cJSON_Delete(root);
        return 2;
    }

    // 输出解析结果
    cJSON *datetime_1 = cJSON_GetObjectItem(result, "datetime_1");
    parse_datetime(datetime_1->valuestring, p_time_info_st);
    // 释放 cJSON 对象
    cJSON_Delete(root);
	return 0;
}
/**
 * @name: stm32_voice_weather_forecast_analysis_json_weather
 * @msg: 解析保存在 json 中的天气数据, 填写在 p_weather_info_st 中
 * @param {char} *json_data json 数据
 * @param {void} *private_data 填写的结构体
 * @param {uint8_t} weather_info_len p_weather_info_st 的数量, 目前是3个
 * @return {*}
 * @author: TOTHTOT
 * @Date: 2024-05-04 21:39:50
 */
uint8_t stm32_voice_weather_forecast_analysis_json_weather(const char *json_data, void *private_data, uint8_t weather_info_len)
{
    weather_info_t *p_weather_info_st = (weather_info_t *)private_data;
    // 解析 JSON 字符串
    cJSON *root = cJSON_Parse((char *)json_data);
    if (root == NULL)
    {
        printf("Error parsing JSON: %s\n", cJSON_GetErrorPtr());
        return 1;
    }
    // INFO_PRINT("json buf:%s\r\n", json_data);

    // 访问 JSON 数据
    cJSON *results = cJSON_GetObjectItem(root, "results");
    cJSON *location = cJSON_GetObjectItem(results->child, "location");
    cJSON *daily = cJSON_GetObjectItem(results->child, "daily");

    // 输出解析结果
    // printf("Location: %s\r\n", cJSON_GetObjectItem(location, "name")->valuestring);

    // 遍历每一天的天气信息
    cJSON *daily_item = NULL;
    uint8_t index = 0;
    cJSON_ArrayForEach(daily_item, daily)
    {
        printf("Date: %s, high: %s, low: %s, humidity: %s, wind_speed = %s, text_night = %s\r\n",
               cJSON_GetObjectItem(daily_item, "date")->valuestring,
               cJSON_GetObjectItem(daily_item, "high")->valuestring,
               cJSON_GetObjectItem(daily_item, "low")->valuestring,
               cJSON_GetObjectItem(daily_item, "humidity")->valuestring,
               cJSON_GetObjectItem(daily_item, "wind_speed")->valuestring,
               cJSON_GetObjectItem(daily_item, "text_night")->valuestring);
        if (index < weather_info_len)
        {
            p_weather_info_st[index].temperature_high = atoi(cJSON_GetObjectItem(daily_item, "high")->valuestring);
            p_weather_info_st[index].temperature_low = atoi(cJSON_GetObjectItem(daily_item, "low")->valuestring);
            p_weather_info_st[index].humidity = atoi(cJSON_GetObjectItem(daily_item, "humidity")->valuestring);
            p_weather_info_st[index].wind_speed = atof(cJSON_GetObjectItem(daily_item, "wind_speed")->valuestring);
            strcpy((char *)p_weather_info_st[index].weather_time, cJSON_GetObjectItem(daily_item, "date")->valuestring);
            strcpy((char *)p_weather_info_st[index].weather, cJSON_GetObjectItem(daily_item, "text_night")->valuestring);
            index++;
        }
    }

    // 释放 cJSON 对象
    cJSON_Delete(root);
	return 0;
}

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

    // 复位 esp
    HAL_GPIO_WritePin(ESP_RST_IO_GPIO_Port, ESP_RST_IO_Pin, GPIO_PIN_RESET);
    delay_xms(10);
    HAL_GPIO_WritePin(ESP_RST_IO_GPIO_Port, ESP_RST_IO_Pin, GPIO_PIN_SET);
    // 等待 esp 稳定
    delay_xms(2000);
    
    // 上电获取天气数据
#if (ESP_AT_CMD_ENABLE == 1)
    if ((ret = esp_at_cmd_init(p_dev_st->devices_info.p_esp_at_dev_st)) != 0)
    {
        goto ERROR_PRINT;
    }
    // 获取网络时间
#if (ESP_AT_CMD_ENABLE == 1)
    // 获取天气数据, 并解析
    if ((ret = p_dev_st->devices_info.p_esp_at_dev_st->ops_func.get_time(p_dev_st->devices_info.p_esp_at_dev_st)) != 0)
    {
        ERROR_PRINT("get_time() fail[%d]\r\n", ret);
        return 4;
    }
    else
    {
        // 退出命令输入模式
        if (stm32_voice_weather_forecast_analysis_json_time((char *)p_dev_st->devices_info.p_esp_at_dev_st->uart_info_st.rxbuf, &p_dev_st->time_info) == 0)
        {
            // 解析成功才开启软件定时器
            extern osTimerId time_1sHandle;
            osTimerStart(time_1sHandle, 1000);
        }
        p_dev_st->devices_info.p_esp_at_dev_st->ops_func.exit_cmd_mode(p_dev_st->devices_info.p_esp_at_dev_st);
        p_dev_st->devices_info.p_esp_at_dev_st->ops_func.closecip(p_dev_st->devices_info.p_esp_at_dev_st);
    }
#endif

    // 获取天气数据, 并解析
    if ((ret = p_dev_st->devices_info.p_esp_at_dev_st->ops_func.get_weather(p_dev_st->devices_info.p_esp_at_dev_st, "fujianfuzhou")) != 0)
    {
        ERROR_PRINT("get_weather() fail[%d]\r\n", ret);
        return 3;
    }
    else
    {
        stm32_voice_weather_forecast_analysis_json_weather((char *)p_dev_st->devices_info.p_esp_at_dev_st->uart_info_st.rxbuf, p_dev_st->weather_info_st, 3);
        p_dev_st->devices_info.p_esp_at_dev_st->ops_func.exit_cmd_mode(p_dev_st->devices_info.p_esp_at_dev_st);
        p_dev_st->devices_info.p_esp_at_dev_st->ops_func.closecip(p_dev_st->devices_info.p_esp_at_dev_st);
    }
#endif

    u8g2_ClearBuffer(&p_dev_st->devices_info.u8g2);
    u8g2_SetFont(&p_dev_st->devices_info.u8g2, u8g2_font_t0_11_te);
    u8g2_DrawStr(&p_dev_st->devices_info.u8g2, 5, 20, "init success");
    u8g2_SendBuffer(&p_dev_st->devices_info.u8g2); // 发送缓冲区内容到 OLED 显示屏
    // INFO_PRINT("weather:%s\r\n", p_dev_st->devices_info.p_esp_at_dev_st->uart_info_st.rxbuf);

    // 开启ld3320中断接收
    HAL_UART_Receive_IT(&huart3, &p_dev_st->devices_info.p_ld3320_dev_st->uart_info_st.rxtmp, 1);
    // 默认显示今天天气
    p_dev_st->cur_show_weather_info_index = WEATHER_INFO_INDEX_TODAY;
    u8g2_refresh_scr(p_dev_st);

    return 0;

ERROR_PRINT:
    sprintf(error_buf, "init fail ret = %d", ret);
    u8g2_ClearBuffer(&p_dev_st->devices_info.u8g2);
    u8g2_SetFont(&p_dev_st->devices_info.u8g2, u8g2_font_t0_11_te);
    u8g2_DrawStr(&p_dev_st->devices_info.u8g2, 5, 20, error_buf);
    u8g2_SendBuffer(&p_dev_st->devices_info.u8g2); // 发送缓冲区内容到 OLED 显示屏
    // p_dev_st->devices_info.p_syn6288_dev_st->ops_func.send_frame_info(p_dev_st->devices_info.p_syn6288_dev_st, 0, (uint8_t *)error_buf);
// ERROR_RETURN:
    return ret;
}

/**
 * @name: stm32_voice_weather_forecast_analysis_ld3320_data
 * @msg: 解析 ld3320 发送的数据
 * @param {uint8_t} *data ld3320 发送的数据
 * @param {char} *result 解析后的数据
 * @return {== 0 成功; == 1 帧头错误; == 2 帧尾错误}
 * @author: TOTHTOT
 * @Date: 2024-05-09 14:26:32
 */
uint8_t stm32_voice_weather_forecast_analysis_ld3320_data(const uint8_t *data, char *result)
{
    #define LD3320_FRAME_HEAD 0xa0
    #define LD3320_FRAME_TAIL 0xaa
    uint8_t datalen = 0;

    // 校验帧头
    if (data[0] != LD3320_FRAME_HEAD)
    {
        return 1;
    }
    datalen = data[1];
    // 校验帧尾
    if (data[datalen + 2] != LD3320_FRAME_TAIL)
    {
        return 2;
    }

    // 复制数据
    memcpy(result, &data[2], datalen);
    return 0;
}

/**
 * @name: system_time_increase
 * @msg: 计算系统时间, 返回 unix 时间
 * @param {time_info_t} *p_time_info_st
 * @return {*}
 * @author: TOTHTOT
 * @Date: 2024-05-10 09:03:20
 */
uint32_t system_time_increase(time_info_t *p_time_info_st)
{
    uint8_t month_day_tab[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; // 每月的天数
    p_time_info_st->millisecond++;
    if (p_time_info_st->millisecond > 0) // 1ms进入中断一次，1000次就是1ms
    {
        p_time_info_st->millisecond = 0L;
        p_time_info_st->second++; // 秒进位
        if (p_time_info_st->second >= 60)
        {
            // printf("时间:%s\r\n", now_time);
            p_time_info_st->second = 0;
            p_time_info_st->minute++; // 分钟进位
            if (p_time_info_st->minute >= 60)
            {
                p_time_info_st->minute = 0;
                p_time_info_st->hour++; // 小时进位
                if (p_time_info_st->hour >= 24)
                {
                    p_time_info_st->hour = 0;
                    p_time_info_st->day++; // 天进位
                    if (p_time_info_st->day > (((p_time_info_st->year % 4 == 0) && (p_time_info_st->month == 2)) ? month_day_tab[p_time_info_st->month] + 1 : month_day_tab[p_time_info_st->month]))
                    {
                        p_time_info_st->day = 1;
                        p_time_info_st->month++; // 月进位
                        if (p_time_info_st->month > 12)
                        {
                            p_time_info_st->month = 1;
                            p_time_info_st->year++; // 年进位
                            if (p_time_info_st->year > 99)
                            {
                                p_time_info_st->year = 0;
                            }
                        }
                    }
                }
            }
        }
    }
    // 计算UNIX时间戳
    uint32_t unix_time = ((p_time_info_st->year - 70) * 31536000UL) +
                         (((p_time_info_st->year - 69) / 4) * 86400UL) +
                         ((p_time_info_st->month <= 2 && (p_time_info_st->year % 4 == 0)) ? 86400UL : 0) +
                         ((p_time_info_st->day - 1) * 86400UL) +
                         (p_time_info_st->hour * 3600UL) +
                         (p_time_info_st->minute * 60UL) +
                         p_time_info_st->second;
    return unix_time;
}

