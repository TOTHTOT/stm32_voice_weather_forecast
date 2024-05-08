/*
 * @Description: esp at指令相关功能, 使用阻塞接收方式接收数据, 使用 cjson 需要足够的 heap 大小
 * @Author: TOTHTOT
 * @Date: 2024-05-04 11:53:13
 * @LastEditTime: 2024-05-08 21:38:54
 * @LastEditors: TOTHTOT
 * @FilePath: \stm32_voice_weather_forecast\code\STM32F103C8T6(HAL+FreeRTOS)\HARDWARE\ESP\esp_at_cmd.c
 */

#include "esp_at_cmd.h"

/* 用户实现函数开始 */
#include "usart.h"
#include "usart1.h"
#include "stm32_voice_weather_forecast.h"
#include "cJSON.h"
#include "delay.h"
/**
 * @name: esp_at_uart_send_data
 * @msg: esp 的串口发送函数
 * @param {uint8_t} *data 发送数据
 * @param {uint16_t} len 发送长度
 * @return { < 0 失败, 返回错误码; >= 0 发送长度 }
 * @author: TOTHTOT
 * @Date: 2024-04-16 14:07:14
 */
int32_t esp_at_uart_send_data(uint8_t *data, uint16_t len)
{
    int32_t ret = 0;
    ret = HAL_UART_Transmit(&huart2, data, len, 1000);

    if (ret != HAL_OK)
        return -ret;
    else
        return len;
}

/**
 * @name: esp_at_uart_recv_data
 * @msg: 接收指定长度字符串, 阻塞接收, 超时时间为100ms, 超时未接收满也算成功
 * @param {uint8_t} *recvbuf 接收缓冲区
 * @param {uint16_t} recvlen 接收长度
 * @param {uint16_t} timeout_ms 超时时间, 单位: ms
 * @return {< 0 失败, 返回错误码; >= 0 返回HAL_OK}
 * @author: TOTHTOT
 * @Date: 2024-04-16 15:53:44
 */
int32_t esp_at_uart_recv_data(uint8_t *recvbuf, uint16_t recvlen, uint16_t timeout_ms)
{
    int32_t ret = 0;

    ret = HAL_UART_Receive(&huart2, recvbuf, recvlen, timeout_ms);
    if (ret != HAL_OK)
    {
        // 错误的话再次判断是超时但是有数据还是其他错误
        if (ret == HAL_TIMEOUT)
        {
            int32_t len = strlen((char *)recvbuf);
            if (len > 0)
            {
                return HAL_OK;
            }
            else
                return -ret;
        }

        return -ret;
    }
    return HAL_OK;
}

/* 用户实现函数结束 */

/**
 * @name: esp_at_send_cmd_by_waitack
 * @msg: sim800l 发送cmd, 等待ack并设置超时
 * @param {sim800l_device_t} *sim800l_dev_p 设备结构体
 * @param {char} *cmd 命令
 * @param {uint16_t} cmdlen 命令长度
 * @param {char} *ack 应答 ack, 如果ack 传入 null, 就根据 acklen 判断是否接收成功
 * @param {uint16_t} acklen ack 长度
 * @param {uint32_t} timeout 超时时间
 * @return { == 0 成功; == 1, 接收错误; == 2, ack 不匹配; == 3, 接收长度小于 acklen}
 * @author: TOTHTOT
 * @Date: 2024-04-16 16:19:16
 */
uint8_t esp_at_send_cmd_by_waitack(esp_at_t *p_dev_st, char *cmd, uint16_t cmdlen, char *ack, uint16_t acklen, uint32_t timeout)
{
    memset(p_dev_st->uart_info_st.rxbuf, 0, sizeof(p_dev_st->uart_info_st.rxbuf));
    p_dev_st->ops_func.send_data((uint8_t *)cmd, cmdlen);
    if (p_dev_st->ops_func.recv_data(p_dev_st->uart_info_st.rxbuf,
                                     sizeof(p_dev_st->uart_info_st.rxbuf), timeout) != HAL_OK)
    {
        return 1;
    }
    // 如果 ack 为 null, 就根据 acklen 判断是否接收成功
    if (ack == NULL)
    {
        if (strlen((char *)p_dev_st->uart_info_st.rxbuf) >= acklen)
        {
            return 0;
        }
        else
        {
            return 3;
        }
    }
    else
    {
        if (strstr((char *)p_dev_st->uart_info_st.rxbuf, ack) == NULL)
        {
            ERROR_PRINT("ack = [%s], recv = [%s], len = %d\r\n", ack, p_dev_st->uart_info_st.rxbuf, strlen((char *)p_dev_st->uart_info_st.rxbuf));
            return 2;
        }

        return 0;
    }
}

/**
 * @name: esp_is_connect_wifi
 * @msg: 检测是否连接到WiFi,根据回复的信息中是否含有指定WiFi名称判断是否连接成功
 * @param {esp_at_t} *p_dev_st
 * @return {*}
 * @author: TOTHTOT
 * @Date: 2024-05-04 16:56:02
 */
bool esp_is_connect_wifi(esp_at_t *p_dev_st)
{
    /* 正常接收是这个
    RX：AT+CWJAP?

    +CWJAP:"esp-2.4G","46:33:bb:ed:0e:37",1,-43

    OK */
    /*  如果没连接WiFi, 则接收到这个
     RX：AT+CWJAP?

     No AP

     OK */
    if (esp_at_send_cmd_by_waitack(p_dev_st, ESP_AT_CMD_CWJAP_STATUS, sizeof(ESP_AT_CMD_CWJAP_STATUS),
                                   ESP_AT_CMD_CWJAP_STATUS_ACK, strlen(ESP_AT_CMD_CWJAP_STATUS_ACK), 1000) == 0)
        return true;
    else
    {
       /*  p_dev_st->ops_func.delay_ms(1000);
        if (esp_at_send_cmd_by_waitack(p_dev_st, ESP_AT_CMD_CWJAP_STATUS, sizeof(ESP_AT_CMD_CWJAP_STATUS),
                                        ESP_AT_CMD_CWJAP_STATUS_ACK, strlen(ESP_AT_CMD_CWJAP_STATUS_ACK), 1000) == 0)
            return true;
        else */
            return false;
    }
}

bool esp_is_avaliable(esp_at_t *p_dev_st)
{
    if (esp_at_send_cmd_by_waitack(p_dev_st, ESP_AT_CMD_AT, sizeof(ESP_AT_CMD_AT),
                                   ESP_AT_CMD_AT_ACK, strlen(ESP_AT_CMD_AT_ACK), 2000) != 0)
    {
        // 设备掉线
        ERROR_PRINT("esp offline\r\n");
        return false;
    }
    return true;
}

/**
 * @name: esp_at_get_weather
 * @msg: 获取天气数据
 * @param {esp_at} *p_dev_st
 * @param {char} *city 城市名称
 * @return {*}
 * @author: TOTHTOT
 * @Date: 2024-05-04 21:54:35
 */
uint8_t esp_at_get_weather(struct esp_at *p_dev_st, const char *city)
{
    uint8_t ret = 0;
    if (p_dev_st->dev_is_connect_wifi == true)
    {
        char city_buf[150] = {0};
        // 连接到心知天气
        if (esp_at_send_cmd_by_waitack(p_dev_st, ESP_AT_CMD_CONNECT_WEATHER, sizeof(ESP_AT_CMD_CONNECT_WEATHER), ESP_AT_CMD_CONNECT_WEATHER_ACK, strlen(ESP_AT_CMD_CONNECT_WEATHER_ACK), 2000) != 0)
        {
            ret = 1;
            goto ERROR_RETURN;
        }

        // 使用透传模式
        if (esp_at_send_cmd_by_waitack(p_dev_st, ESP_AT_CMD_CIPMODE, sizeof(ESP_AT_CMD_CIPMODE), ESP_AT_CMD_CIPMODE_ACK, strlen(ESP_AT_CMD_CIPMODE_ACK), 2000) != 0)
        {
            ret = 2;
            goto ERROR_RETURN;
        }
        // 开始发送数据
        if (esp_at_send_cmd_by_waitack(p_dev_st, ESP_AT_CMD_CIPSEND, sizeof(ESP_AT_CMD_CIPSEND), ESP_AT_CMD_CIPSEND_ACK, strlen(ESP_AT_CMD_CIPSEND_ACK), 2000) != 0)
        {
            ret = 3;
            goto ERROR_RETURN;
        }
        // 获取天气数据
        sprintf(city_buf, ESP_AT_CMD_GET_WEATHER, city);
        if (esp_at_send_cmd_by_waitack(p_dev_st, city_buf, strlen(city_buf), NULL, ESP_AT_CMD_GET_WEATHER_ACK, 2000) != 0)
        {
            ret = 4;
            goto ERROR_RETURN;
        }
        return ret;
    }
    else
        return 5;
ERROR_RETURN:
    return ret;
}

/* 全局变量 */
esp_at_t g_esp_at_st = {
    .ops_func.recv_data = esp_at_uart_recv_data,
    .ops_func.send_data = esp_at_uart_send_data,
    .ops_func.check_wifi_is_connected = esp_is_connect_wifi,
    .ops_func.check_device_is_avaliable = esp_is_avaliable,
    .ops_func.get_weather = esp_at_get_weather,
    .ops_func.delay_ms = delay_xms,
    //.ops_func.analysis_json_weather = esp_at_analysis_json_weather,
};

/**
 * @name: esp_at_cmd_init
 * @msg: 初始化 esp at指令功能
 * @param {esp_at_t} *p_dev_st
 * @return { == 0 成功; != 0 失败}
 * @author: TOTHTOT
 * @Date: 2024-05-04 16:39:20
 */
uint8_t esp_at_cmd_init(esp_at_t *p_dev_st)
{
//    uint8_t ret = 0;
    char wifi_connect_buf[90] = {0};
    // 验证设备是否存在
    p_dev_st->dev_is_ok = p_dev_st->ops_func.check_device_is_avaliable(p_dev_st);
    if (p_dev_st->dev_is_ok == false)
        return 1;
    // 启动延迟确保esp稳定了再获取状态
    p_dev_st->ops_func.delay_ms(3000);

    // 验证是否连接WiFi
    p_dev_st->dev_is_connect_wifi = p_dev_st->ops_func.check_wifi_is_connected(p_dev_st);
    INFO_PRINT("state = %d, buf = %s\r\n", p_dev_st->dev_is_connect_wifi, p_dev_st->uart_info_st.rxbuf);
    // 没连接WiFi才尝试连接wifi
    if (p_dev_st->dev_is_connect_wifi == false)
    {
        sprintf(wifi_connect_buf, ESP_AT_CMD_CONNECT_WIFI, ESP_01S_WIFI_NAME, ESP_01S_WIFI_PASSWORD);
        // 连接WiFi
        if (esp_at_send_cmd_by_waitack(p_dev_st, wifi_connect_buf, strlen(wifi_connect_buf), ESP_AT_CMD_CONNECT_WIFI_CONNECTED, strlen(ESP_AT_CMD_CONNECT_WIFI_CONNECTED), 3000) != 0)
                    return 2;
        p_dev_st->dev_is_connect_wifi = true;
        p_dev_st->ops_func.delay_ms(2000);
    }
    // 设置连接模式
    esp_at_send_cmd_by_waitack(p_dev_st, ESP_AT_CMD_CONNECT_MODE, sizeof(ESP_AT_CMD_CONNECT_MODE), ESP_AT_CMD_CONNECT_MODE_ACK, strlen(ESP_AT_CMD_CONNECT_MODE_ACK), 2000);

    return 0;
}
