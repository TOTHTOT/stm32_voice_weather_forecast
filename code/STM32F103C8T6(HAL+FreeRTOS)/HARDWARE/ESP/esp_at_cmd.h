#ifndef __ESP_AT_CMD_H__
#define __ESP_AT_CMD_H__
/* 头文件 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* 宏定义 */
#define ESP_AT_CMD_ENABLE 1
#define ESP_01S_WIFI_NAME "esp-2.4G"       // esp 连接wifi名称
#define ESP_01S_WIFI_PASSWORD "12345678.." // esp 连接wifi密码

// 检测设备是否在线
#define ESP_AT_CMD_AT "AT\r\n"
#define ESP_AT_CMD_AT_ACK "OK"

// 设置连接模式
#define ESP_AT_CMD_CONNECT_MODE "AT+CWMODE_DEF=3\r\n"
#define ESP_AT_CMD_CONNECT_MODE_ACK "OK"

// 连接wifi, 需要传入 WiFi密码以及名称
#define ESP_AT_CMD_CONNECT_WIFI "AT+CWJAP_DEF=\"%s\",\"%s\"\r\n"
#define ESP_AT_CMD_CONNECT_WIFI_DISCONNECT "WIFI DISCONNECT"
#define ESP_AT_CMD_CONNECT_WIFI_CONNECTED "WIFI CONNECTED"
#define ESP_AT_CMD_CONNECT_WIFI_CONNECT_BUSY "busy p..."

// 连接知心天气
#define ESP_AT_CMD_CONNECT_WEATHER "AT+CIPSTART=\"TCP\",\"api.seniverse.com\",80\r\n"
#define ESP_AT_CMD_CONNECT_WEATHER_ACK "OK"
// 开启透传模式
#define ESP_AT_CMD_CIPMODE "AT+CIPMODE=1\r\n"
#define ESP_AT_CMD_CIPMODE_ACK "OK"

// 开始发送数据
#define ESP_AT_CMD_CIPSEND "AT+CIPSEND\r\n"
#define ESP_AT_CMD_CIPSEND_ACK "OK"

// 获取数据, 需要传入城市例如 fujianfuzhou, 根据长度判断是否成功
#define ESP_AT_CMD_GET_WEATHER "GET https://api.seniverse.com/v3/weather/daily.json?key=S_PcI0OeUiEbEZBpp&location=%s&language=en&unit=c&start=-1&days=3\r\n"
#define ESP_AT_CMD_GET_WEATHER_ACK 500

// 查询是否连接wifi
#define ESP_AT_CMD_CWJAP_STATUS "AT+CWJAP_DEF?\r\n"
#define ESP_AT_CMD_CWJAP_STATUS_ACK ESP_01S_WIFI_NAME

// 关闭 CIPSTART 连接
#define ESP_AT_CMD_CIPCLOSE "AT+CIPCLOSE\r\n"
#define ESP_AT_CMD_CIPCLOSE_ACK "OK"

/* 获取网络时间 */
// TCP/IP连接模式为单连接模式 
#define ESP_AT_CMD_GETTIME_CIPMUX_SINGLE "AT+CIPMUX=0\r\n"
#define ESP_AT_CMD_GETTIME_CIPMUX_SINGLE_ACK "OK"
// 建立TCP连接到目标服务器
#define ESP_AT_CMD_GETTIME_CIPSTART_TCP "AT+CIPSTART=\"TCP\",\"api.k780.com\",80\r\n"
#define ESP_AT_CMD_GETTIME_CIPSTART_TCP_ACk "OK"

// 设置ESP8266为透传模式
#define ESP_AT_CMD_GETTIME_CIPMODE_TRANSPARENT "AT+CIPMODE=1\r\n"
#define ESP_AT_CMD_GETTIME_CIPMODE_TRANSPARENT_ACK "OK"

// 准备发送数据到已经建立的TCP连接
#define ESP_AT_CMD_GETTIME_CIPSEND_PREPARE "AT+CIPSEND\r\n"
#define ESP_AT_CMD_GETTIME_CIPSEND_PREPARE_ACK ">"

// 退出发送模式
#define ESP_AT_CMD_GETTIME_EXIT_SEND_MODE "+++"
#define ESP_AT_CMD_GETTIME_EXIT_SEND_MODE_ACK 0 // 没有回复

// 发送HTTP GET请求到服务器，获取时间相关的数据
#define ESP_AT_CMD_GETTIME_HTTP_GET "GET http://api.k780.com:88/?app=life.time&appkey=10003&sign=b59bc3ef6191eb9f747dd4e83c99f2a4&format=json&HTTP/1.1\r\n"
#define ESP_AT_CMD_GETTIME_HTTP_GET_ACK 190

/* 类型定义 */
typedef struct esp_at
{
    bool dev_is_ok;
    bool dev_is_connect_wifi;

    struct uart_info
    {
        uint8_t rxbuf[1024];
        uint8_t rxindex;
        uint8_t rxtemp;
    } uart_info_st;
    struct dev_ops_func
    {
        /* 用户实现函数 */
        int32_t (*send_data)(uint8_t *data, uint16_t len);
        int32_t (*recv_data)(uint8_t *recvbuf, uint16_t recvlen, uint16_t timeout_ms);
        void (*delay_ms)(uint32_t ms);
        /* 外部函数 */
        bool (*connect_wifi)(struct esp_at *p_dev_st, char *ssid, char *password);
        bool (*check_device_is_avaliable)(struct esp_at *p_dev_st);
        bool (*check_wifi_is_connected)(struct esp_at *p_dev_st);
        uint8_t (*get_weather)(struct esp_at *p_dev_st, const char *city);
        uint8_t (*analysis_json_weather)(const char *json_data, void *p_weather_info_st, uint8_t weather_info_len);
        uint8_t (*get_time)(struct esp_at *p_dev_st);
        uint8_t (*exit_cmd_mode)(struct esp_at *p_dev_st);
        uint8_t (*closecip)(struct esp_at *p_dev_st);

    } ops_func;
} esp_at_t;
/* 全局函数 */
extern esp_at_t g_esp_at_st;

/* 全局变量 */
extern uint8_t esp_at_cmd_init(esp_at_t *p_dev_st);

#endif /* __ESP_AT_CMD_H__ */
