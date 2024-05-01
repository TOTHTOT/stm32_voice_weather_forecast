/*
 * @Description: oled 的驱动代码, 使用硬件i2c
 * @Author: TOTHTOT
 * @Date: 2023-03-14 19:06:07
 * @LastEditTime: 2024-03-23 09:28:42
 * @LastEditors: TOTHTOT
 * @FilePath: \stm32_voice_weather_forecast\code\STM32F103C8T6(HAL+FreeRTOS)\HARDWARE\OLED\oled.h
 */

#ifndef __OLED_H_
#define __OLED_H_

/* FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* HARDWARE */
#include "delay.h"
#include "stm32_voice_weather_forecast.h"

#define OLED_ADDER 0x78
#define SYSTEM_SUPPORT_OS 1

/* oled 显示结构体, 对屏幕的所有操作都封装在其中, 不需要去操作底层函数, 在创建变量时要对其进行初始化 */
struct oled_device
{
/* 使用的I2C模式, DMA或者硬件I2C */
#define I2C_OLED_HARDWARE
// #define I2C_OLED_HARDWARE_DMA 1

#define USE_OLED_GRAM 0 // 使用oled显存
#if USE_OLED_GRAM
    uint8_t oled_gram[8][128]; // 定义模拟显存
#endif                         /* USE_OLED_GRAM */

    /* 以后使用这个库只需要改动这个结构图体和 data_pos 结构体 */
    struct oled_show_data
    {
        // stm32_waste_recovery_t *waste_recovery_p;
        // struct dht11_device *dht11_p;      // dht11的数据结构体, 更新数据时使用
        // struct bh1750_device *bh1750_p;    // bh1750设备
        // struct hcsr501_device *hcsr501_p;  // hcsr501 设备
        // struct pt4115_device *pt4115_p;    // pt4115 设备
        // struct gp2y1014uof_device *gp2y_p; // gp2y1014uof 设备
        // struct esp8266_device *eps8266_p;  // esp8266 设备
        void *private_data;
        uint8_t char_size;                 // 字体大小
    } oled_data_st;

#ifdef SYSTEM_SUPPORT_OS
    osSemaphoreId *en_refresh_screen_p; // 更新屏幕, 使用FreeRTOS时使用
    /* 使用操作系统时 oled线程会根据这个状态机进行不同类型的操作 */
    enum os_refresh_screen_type
    {
        SCREEN_NONE,                   // 默认状态
        SCREEN_CLREAR,                 // 清屏
        SCREEN_REFRESH_MAIN_PAGE,      // 刷新主页面
        SCREEN_REFRESH_MAIN_PAGE_DATA, // 刷新数据
        SCREEN_TOTAL_TYPE              // 总共状态
    } current_screen_page_em,
        set_screen_page_em;
#endif /* SYSTEM_SUPPORT_OS */

    /* 数据显示的位置, 在主页面里赋值 */
    struct data_pos
    {
        uint8_t x;
        uint8_t y;
    } dht11_pos_st[2], passwd_pos_st, is_empyt_pos_st;

    /* 外部可以调用的函数 */
    void (*oled_show_char)(uint8_t x, uint8_t y, uint8_t chr, uint8_t size, uint8_t mode); // 显示一个字符
    void (*oled_show_string)(uint8_t, uint8_t, const uint8_t *, uint8_t);                  // 显示一个字符串
    void (*oled_show_num)(uint8_t, uint8_t, unsigned int, uint8_t, uint8_t);               // 显示一个数字

#ifdef SYSTEM_SUPPORT_OS

    osStatus (*rtos_start_refresh_screen)(struct oled_device *dev, enum os_refresh_screen_type refresh_type_em); // 开始刷新屏幕
    void (*rtos_refresh_screen)(struct oled_device *dev, enum os_refresh_screen_type refresh_type_em);           // 使用操作系统时的刷新屏幕函数
#else

    void (*oled_clear_screen)(void);                        // 清屏
    void (*updata_main_page)(struct oled_device *dev);      // 更新主页面
    void (*updata_main_page_data)(struct oled_device *dev); // 更新主页面数据

#endif /* SYSTEM_SUPPORT_OS */
};
extern struct oled_device g_oled_device_st;

void OLED_Init(struct oled_device *dev);

#endif /* __OLED_H_ */
