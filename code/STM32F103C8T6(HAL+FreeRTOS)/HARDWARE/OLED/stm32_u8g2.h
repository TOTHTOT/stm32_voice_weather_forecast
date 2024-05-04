/*
 * @Description:
 * @Author: TOTHTOT
 * @Date: 2024-05-01 17:53:19
 * @LastEditTime: 2024-05-01 20:45:34
 * @LastEditors: TOTHTOT
 * @FilePath: \stm32_voice_weather_forecast\code\STM32F103C8T6(HAL+FreeRTOS)\HARDWARE\OLED\stm32_u8g2.h
 */
#ifndef __STM32_U8G2_H
#define __STM32_U8G2_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "./csrc/u8g2.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */
#define u8 unsigned char  // ?unsigned char ????
#define MAX_LEN 128       //
#define OLED_ADDRESS 0x78 // oled
#define OLED_CMD 0x00     //
#define OLED_DATA 0x40    //

/* USER CODE BEGIN Prototypes */
uint8_t u8x8_byte_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
uint8_t u8x8_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
void u8g2_init(u8g2_t *u8g2);
void u8g2_draw(u8g2_t *u8g2);
void testDrawPixelToFillScreen(u8g2_t *u8g2);
/* 用户代码开始 */
extern uint8_t u8g2_refresh_scr(void *private_data);
/* 用户代码结束 */
#endif
