/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED0_Pin GPIO_PIN_13
#define LED0_GPIO_Port GPIOC
#define ESP01S_RX_Pin GPIO_PIN_2
#define ESP01S_RX_GPIO_Port GPIOA
#define ESP01S_TX_Pin GPIO_PIN_3
#define ESP01S_TX_GPIO_Port GPIOA
#define LD3320_RX_Pin GPIO_PIN_10
#define LD3320_RX_GPIO_Port GPIOB
#define LD3320_TX_Pin GPIO_PIN_11
#define LD3320_TX_GPIO_Port GPIOB
#define SYN6288_BUSY_IO_Pin GPIO_PIN_8
#define SYN6288_BUSY_IO_GPIO_Port GPIOA
#define SYN6288_RX_Pin GPIO_PIN_9
#define SYN6288_RX_GPIO_Port GPIOA
#define SYN6288_TX_Pin GPIO_PIN_10
#define SYN6288_TX_GPIO_Port GPIOA
#define ESP_RST_IO_Pin GPIO_PIN_5
#define ESP_RST_IO_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
