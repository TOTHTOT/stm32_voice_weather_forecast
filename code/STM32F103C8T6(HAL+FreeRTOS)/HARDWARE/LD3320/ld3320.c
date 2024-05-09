/*
 * @Description: ld3320 驱动
 * @Author: TOTHTOT
 * @Date: 2024-05-09 14:32:23
 * @LastEditTime: 2024-05-09 15:31:24
 * @LastEditors: TOTHTOT
 * @FilePath: \stm32_voice_weather_forecast\code\STM32F103C8T6(HAL+FreeRTOS)\HARDWARE\LD3320\ld3320.c
 */
/* 头文件 */
#include "ld3320.h"

/* 用户实现功能 开始 */
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "usart.h"
#include "delay.h"

int32_t ld3320_uartrecv_intp(ld3320_device_t *p_dev_st)
{
    extern osTimerId ld3320_uartrecv_timHandle;

    // 保存数据
    p_dev_st->uart_info_st.rxbuf[p_dev_st->uart_info_st.rxindex++] = p_dev_st->uart_info_st.rxtmp;
    // 开启rtos软件定时器
    osTimerStart(ld3320_uartrecv_timHandle, 10);
    // 开启一次接收
    HAL_UART_Receive_IT(&huart3, &p_dev_st->uart_info_st.rxtmp, 1);
    return 0;
}

/* 用户实现功能 结束 */

/* 全局变量 */
ld3320_device_t g_ld3320_device_st = {
    .ops_func.recv_data = ld3320_uartrecv_intp,
    .ops_func.delay_ms = delay_xms,
};

/* 函数实现 */
uint8_t ld3320_init(ld3320_device_t *p_dev_st)
{
	return 0;
}
