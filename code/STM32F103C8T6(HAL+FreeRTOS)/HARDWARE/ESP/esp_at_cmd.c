/*
 * @Description: esp atָ����ع���, ʹ���������շ�ʽ��������, ʹ�� cjson ��Ҫ�㹻�� heap ��С
 * @Author: TOTHTOT
 * @Date: 2024-05-04 11:53:13
 * @LastEditTime: 2024-05-11 20:28:10
 * @LastEditors: TOTHTOT
 * @FilePath: \stm32_voice_weather_forecast\code\STM32F103C8T6(HAL+FreeRTOS)\HARDWARE\ESP\esp_at_cmd.c
 */

#include "esp_at_cmd.h"

/* �û�ʵ�ֺ�����ʼ */
#include "usart.h"
#include "usart1.h"
#include "stm32_voice_weather_forecast.h"
#include "cJSON.h"
#include "delay.h"
/**
 * @name: esp_at_uart_send_data
 * @msg: esp �Ĵ��ڷ��ͺ���
 * @param {uint8_t} *data ��������
 * @param {uint16_t} len ���ͳ���
 * @return { < 0 ʧ��, ���ش�����; >= 0 ���ͳ��� }
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
 * @msg: ����ָ�������ַ���, ��������, ��ʱʱ��Ϊ100ms, ��ʱδ������Ҳ��ɹ�
 * @param {uint8_t} *recvbuf ���ջ�����
 * @param {uint16_t} recvlen ���ճ���
 * @param {uint16_t} timeout_ms ��ʱʱ��, ��λ: ms
 * @return {< 0 ʧ��, ���ش�����; >= 0 ����HAL_OK}
 * @author: TOTHTOT
 * @Date: 2024-04-16 15:53:44
 */
int32_t esp_at_uart_recv_data(uint8_t *recvbuf, uint16_t recvlen, uint16_t timeout_ms)
{
    int32_t ret = 0;

    ret = HAL_UART_Receive(&huart2, recvbuf, recvlen, timeout_ms);
    if (ret != HAL_OK)
    {
        // ����Ļ��ٴ��ж��ǳ�ʱ���������ݻ�����������
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

/* �û�ʵ�ֺ������� */

/**
 * @name: esp_at_send_cmd_by_waitack
 * @msg: sim800l ����cmd, �ȴ�ack�����ó�ʱ
 * @param {sim800l_device_t} *sim800l_dev_p �豸�ṹ��
 * @param {char} *cmd ����
 * @param {uint16_t} cmdlen �����
 * @param {char} *ack Ӧ�� ack, ���ack ���� null, �͸��� acklen �ж��Ƿ���ճɹ�
 * @param {uint16_t} acklen ack ����, ack��acklen ��Ϊ0 ������Ƿ���ȷ, ֱ�ӷ��سɹ�
 * @param {uint32_t} timeout ��ʱʱ��
 * @return { == 0 �ɹ�; == 1, ���մ���; == 2, ack ��ƥ��; == 3, ���ճ���С�� acklen}
 * @author: TOTHTOT
 * @Date: 2024-04-16 16:19:16
 */
uint8_t esp_at_send_cmd_by_waitack(esp_at_t *p_dev_st, char *cmd, uint16_t cmdlen, char *ack, uint16_t acklen, uint32_t timeout)
{
    memset(&p_dev_st->uart_info_st, 0, sizeof(p_dev_st->uart_info_st));
    p_dev_st->ops_func.send_data((uint8_t *)cmd, cmdlen);
    if (p_dev_st->ops_func.recv_data(p_dev_st->uart_info_st.rxbuf,
                                     sizeof(p_dev_st->uart_info_st.rxbuf), timeout) != HAL_OK)
    {
        return 1;
    }
    // ��� ack Ϊ null, �͸��� acklen �ж��Ƿ���ճɹ�
    if (ack == NULL)
    {
        // acklen ����0�����ֱ�ӷ��سɹ�
        if (acklen == 0)
            return 0;
        if (strlen((char *)p_dev_st->uart_info_st.rxbuf) >= acklen)
        {
            return 0;
        }
        else
        {
            ERROR_PRINT("len = %d, recv = %s\r\n", strlen((char *)p_dev_st->uart_info_st.rxbuf), p_dev_st->uart_info_st.rxbuf);
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
 * @name: esp_at_cmd_get_time
 * @msg: ��ȡ����ʱ��, ִ�гɹ����ݱ�����rxbuf��
 * @param {esp_at_t} *p_dev_st
 * @return { == 0 �ɹ�;  > 0 ʧ��}
 * @author: TOTHTOT
 * @Date: 2024-05-10 13:49:51
 */
uint8_t esp_at_cmd_get_time(esp_at_t *p_dev_st)
{
    uint8_t ret = 0;

    // ���ж��Ƿ����ӳɹ�WiFi
    if (p_dev_st->ops_func.check_wifi_is_connected(p_dev_st) == true)
    {
        if (esp_at_send_cmd_by_waitack(p_dev_st, ESP_AT_CMD_GETTIME_CIPMUX_SINGLE, sizeof(ESP_AT_CMD_GETTIME_CIPMUX_SINGLE), ESP_AT_CMD_GETTIME_CIPMUX_SINGLE_ACK, strlen(ESP_AT_CMD_GETTIME_CIPMUX_SINGLE_ACK), 200) != 0)
        {
            ret = 1;
            goto ERROR_RETURN;
        }
        if (esp_at_send_cmd_by_waitack(p_dev_st, ESP_AT_CMD_GETTIME_CIPSTART_TCP, sizeof(ESP_AT_CMD_GETTIME_CIPSTART_TCP), ESP_AT_CMD_GETTIME_CIPSTART_TCP_ACk, strlen(ESP_AT_CMD_GETTIME_CIPSTART_TCP_ACk), 2000) != 0)
        {
            ret = 2;
            goto ERROR_RETURN;
        }
        if (esp_at_send_cmd_by_waitack(p_dev_st, ESP_AT_CMD_GETTIME_CIPMODE_TRANSPARENT, sizeof(ESP_AT_CMD_GETTIME_CIPMODE_TRANSPARENT), ESP_AT_CMD_GETTIME_CIPMODE_TRANSPARENT_ACK, strlen(ESP_AT_CMD_GETTIME_CIPMODE_TRANSPARENT_ACK), 200) != 0)
        {
            ret = 3;
            goto ERROR_RETURN;
        }
        if (esp_at_send_cmd_by_waitack(p_dev_st, ESP_AT_CMD_GETTIME_CIPSEND_PREPARE, sizeof(ESP_AT_CMD_GETTIME_CIPSEND_PREPARE), ESP_AT_CMD_GETTIME_CIPSEND_PREPARE_ACK, strlen(ESP_AT_CMD_GETTIME_CIPSEND_PREPARE_ACK), 2000) != 0)
        {
            ret = 4;
            goto ERROR_RETURN;
        }
        if (esp_at_send_cmd_by_waitack(p_dev_st, ESP_AT_CMD_GETTIME_HTTP_GET, sizeof(ESP_AT_CMD_GETTIME_HTTP_GET), NULL, ESP_AT_CMD_GETTIME_HTTP_GET_ACK, 2000) != 0)
        {
            ret = 5;
            goto ERROR_RETURN;
        }
    }
    return 0;

ERROR_RETURN:
    return  ret;
}

/**
 * @name: esp_at_get_weather
 * @msg: ��ȡ��������
 * @param {esp_at} *p_dev_st
 * @param {char} *city ��������
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
        // ���ӵ���֪����
        if (esp_at_send_cmd_by_waitack(p_dev_st, ESP_AT_CMD_CONNECT_WEATHER, sizeof(ESP_AT_CMD_CONNECT_WEATHER), ESP_AT_CMD_CONNECT_WEATHER_ACK, strlen(ESP_AT_CMD_CONNECT_WEATHER_ACK), 2000) != 0)
        {
            ret = 1;
            goto ERROR_RETURN;
        }

        // ʹ��͸��ģʽ
        if (esp_at_send_cmd_by_waitack(p_dev_st, ESP_AT_CMD_CIPMODE, sizeof(ESP_AT_CMD_CIPMODE), ESP_AT_CMD_CIPMODE_ACK, strlen(ESP_AT_CMD_CIPMODE_ACK), 200) != 0)
        {
            ret = 2;
            goto ERROR_RETURN;
        }
        // ��ʼ��������
        if (esp_at_send_cmd_by_waitack(p_dev_st, ESP_AT_CMD_CIPSEND, sizeof(ESP_AT_CMD_CIPSEND), ESP_AT_CMD_CIPSEND_ACK, strlen(ESP_AT_CMD_CIPSEND_ACK), 200) != 0)
        {
            ret = 3;
            goto ERROR_RETURN;
        }
        // ��ȡ��������
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

/**
 * @name: esp_is_connect_wifi
 * @msg: ����Ƿ����ӵ�WiFi,���ݻظ�����Ϣ���Ƿ���ָ��WiFi�����ж��Ƿ����ӳɹ�
 * @param {esp_at_t} *p_dev_st
 * @return {*}
 * @author: TOTHTOT
 * @Date: 2024-05-04 16:56:02
 */
bool esp_is_connect_wifi(esp_at_t *p_dev_st)
{
    /* �������������
    RX��AT+CWJAP?

    +CWJAP:"esp-2.4G","46:33:bb:ed:0e:37",1,-43

    OK */
    /*  ���û����WiFi, ����յ����
     RX��AT+CWJAP?

     No AP

     OK */
    if (esp_at_send_cmd_by_waitack(p_dev_st, ESP_AT_CMD_CWJAP_STATUS, sizeof(ESP_AT_CMD_CWJAP_STATUS),
                                   ESP_AT_CMD_CWJAP_STATUS_ACK, strlen(ESP_AT_CMD_CWJAP_STATUS_ACK), 100) == 0)
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
                                   ESP_AT_CMD_AT_ACK, strlen(ESP_AT_CMD_AT_ACK), 200) != 0)
    {
        // �豸����
        ERROR_PRINT("esp offline\r\n");
        return false;
    }
    return true;
}

/**
 * @name: esp_at_cmd_exit_cmd_mode
 * @msg: �˳��� AT+CIPSEND ���������ģʽ
 * @param {esp_at} *p_dev_st
 * @return { == 0  �ɹ�; == 1 ʧ��}
 * @author: TOTHTOT
 * @Date: 2024-05-10 21:06:02
 */
uint8_t esp_at_cmd_exit_cmd_mode(struct esp_at *p_dev_st)
{
    // �˳���������ģʽ
    if (esp_at_send_cmd_by_waitack(p_dev_st, ESP_AT_CMD_GETTIME_EXIT_SEND_MODE, strlen(ESP_AT_CMD_GETTIME_EXIT_SEND_MODE), NULL, 0, 100) != 0)
        return 1;
    return 0;
}

/**
 * @name: esp_at_cmd_closecip
 * @msg: �ر� cip , ���ܴ��������ֵ
 * @param {esp_at} *p_dev_st
 * @return {*}
 * @author: TOTHTOT
 * @Date: 2024-05-10 21:28:35
 */
uint8_t esp_at_cmd_closecip(struct esp_at *p_dev_st)
{
    // �˳���������ģʽ
    if (esp_at_send_cmd_by_waitack(p_dev_st, ESP_AT_CMD_CIPCLOSE, strlen(ESP_AT_CMD_CIPCLOSE), NULL, 0, 100) != 0)
        return 1;
    return 0;
}

/* ȫ�ֱ��� */
esp_at_t g_esp_at_st = {
    .ops_func.recv_data = esp_at_uart_recv_data,
    .ops_func.send_data = esp_at_uart_send_data,
    .ops_func.check_wifi_is_connected = esp_is_connect_wifi,
    .ops_func.check_device_is_avaliable = esp_is_avaliable,
    .ops_func.get_weather = esp_at_get_weather,
    .ops_func.delay_ms = delay_xms,
    .ops_func.get_time = esp_at_cmd_get_time,
    .ops_func.exit_cmd_mode = esp_at_cmd_exit_cmd_mode,
    .ops_func.closecip = esp_at_cmd_closecip,
    //.ops_func.analysis_json_weather = esp_at_analysis_json_weather,
};

/**
 * @name: esp_at_cmd_init
 * @msg: ��ʼ�� esp atָ���
 * @param {esp_at_t} *p_dev_st
 * @return { == 0 �ɹ�; != 0 ʧ��}
 * @author: TOTHTOT
 * @Date: 2024-05-04 16:39:20
 */
uint8_t esp_at_cmd_init(esp_at_t *p_dev_st)
{
//    uint8_t ret = 0;
    char wifi_connect_buf[90] = {0};
    // ��֤�豸�Ƿ����
    p_dev_st->dev_is_ok = p_dev_st->ops_func.check_device_is_avaliable(p_dev_st);
    if (p_dev_st->dev_is_ok == false)
        return 1;
    // �����ӳ�ȷ��esp�ȶ����ٻ�ȡ״̬
    p_dev_st->ops_func.delay_ms(7000);

    // ��֤�Ƿ�����WiFi
    p_dev_st->dev_is_connect_wifi = p_dev_st->ops_func.check_wifi_is_connected(p_dev_st);
    // INFO_PRINT("state = %d, buf = %s\r\n", p_dev_st->dev_is_connect_wifi, p_dev_st->uart_info_st.rxbuf);
    // û����WiFi�ų�������wifi
    if (p_dev_st->dev_is_connect_wifi == false)
    {
        sprintf(wifi_connect_buf, ESP_AT_CMD_CONNECT_WIFI, ESP_01S_WIFI_NAME, ESP_01S_WIFI_PASSWORD);
        // ����WiFi
        if (esp_at_send_cmd_by_waitack(p_dev_st, wifi_connect_buf, strlen(wifi_connect_buf), ESP_AT_CMD_CONNECT_WIFI_CONNECTED, strlen(ESP_AT_CMD_CONNECT_WIFI_CONNECTED), 3000) != 0)
                    return 2;
        p_dev_st->dev_is_connect_wifi = true;
        p_dev_st->ops_func.delay_ms(2000);
    }
    // ��������ģʽ
    esp_at_send_cmd_by_waitack(p_dev_st, ESP_AT_CMD_CONNECT_MODE, sizeof(ESP_AT_CMD_CONNECT_MODE), ESP_AT_CMD_CONNECT_MODE_ACK, strlen(ESP_AT_CMD_CONNECT_MODE_ACK), 2000);

    return 0;
}
