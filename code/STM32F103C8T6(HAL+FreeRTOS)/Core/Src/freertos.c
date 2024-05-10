/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
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

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "semphr.h"
#include "delay.h"
#include "led.h"
#include "key.h"
#include "usart.h"
#include "string.h"
#include <stdio.h>
#include "usart1.h"
#include "stm32_voice_weather_forecast.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId LED_TASKHandle;
osTimerId time_1sHandle;
osTimerId ld3320_uartrecv_timHandle;
osSemaphoreId Usart1_Receive_BinSemaphoreHandle;
osSemaphoreId oled_refreashHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void led_task(void const * argument);
void time_1s_cb(void const * argument);
void ld3320_uartrecv_tim_cb(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* GetTimerTaskMemory prototype (linked to static allocation support) */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/* USER CODE BEGIN GET_TIMER_TASK_MEMORY */
static StaticTask_t xTimerTaskTCBBuffer;
static StackType_t xTimerStack[configTIMER_TASK_STACK_DEPTH];

void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
{
  *ppxTimerTaskTCBBuffer = &xTimerTaskTCBBuffer;
  *ppxTimerTaskStackBuffer = &xTimerStack[0];
  *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
  /* place for user code */
}
/* USER CODE END GET_TIMER_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* definition and creation of Usart1_Receive_BinSemaphore */
  osSemaphoreDef(Usart1_Receive_BinSemaphore);
  Usart1_Receive_BinSemaphoreHandle = osSemaphoreCreate(osSemaphore(Usart1_Receive_BinSemaphore), 1);

  /* definition and creation of oled_refreash */
  osSemaphoreDef(oled_refreash);
  oled_refreashHandle = osSemaphoreCreate(osSemaphore(oled_refreash), 1);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
    xSemaphoreTake(Usart1_Receive_BinSemaphoreHandle, portMAX_DELAY);
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* definition and creation of time_1s */
  osTimerDef(time_1s, time_1s_cb);
  time_1sHandle = osTimerCreate(osTimer(time_1s), osTimerPeriodic, NULL);

  /* definition and creation of ld3320_uartrecv_tim */
  osTimerDef(ld3320_uartrecv_tim, ld3320_uartrecv_tim_cb);
  ld3320_uartrecv_timHandle = osTimerCreate(osTimer(ld3320_uartrecv_tim), osTimerPeriodic, NULL);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 256);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of LED_TASK */
  osThreadDef(LED_TASK, led_task, osPriorityLow, 0, 128);
  LED_TASKHandle = osThreadCreate(osThread(LED_TASK), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
    uint32_t loop_times = 0;
    if (stm32_voice_weather_forecast_init(&g_stm32_voice_weather_forecast_st) != 0)
    {
        g_stm32_voice_weather_forecast_st.system_is_ready = false;
        return;
    }
    g_stm32_voice_weather_forecast_st.system_is_ready = true;
    /* Infinite loop */
    for (;;)
    {
        if (g_stm32_voice_weather_forecast_st.system_is_ready != true)
            osDelay(10000);

        if (loop_times %10 == 0)
        {
            // 500ms 刷新屏幕天气信息
            if (loop_times % 50 == 0)
            {
                g_stm32_voice_weather_forecast_st.cur_show_weather_info_index++;
                if (g_stm32_voice_weather_forecast_st.cur_show_weather_info_index > WEATHER_INFO_INDEX_TOMORROW)
                    g_stm32_voice_weather_forecast_st.cur_show_weather_info_index = WEATHER_INFO_INDEX_YESTERDAY;
                // INFO_PRINT("cur_show_weather_info_index = %d\r\n", g_stm32_voice_weather_forecast_st.cur_show_weather_info_index);
            }
            u8g2_refresh_scr(&g_stm32_voice_weather_forecast_st);
        }
        loop_times++;
        osDelay(DEFAULT_THR_DELAY_TIME_MS);
    }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_led_task */
/**
* @brief Function implementing the LED_TASK thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_led_task */
void led_task(void const * argument)
{
  /* USER CODE BEGIN led_task */
  /* Infinite loop */
  for(;;)
  {
        // INFO_PRINT("led\r\n");
        LED0_TOGGLE;
        delay_ms(500);
  }
  /* USER CODE END led_task */
}

/* time_1s_cb function */
void time_1s_cb(void const * argument)
{
  /* USER CODE BEGIN time_1s_cb */
    system_time_increase(&g_stm32_voice_weather_forecast_st.time_info);
  /* USER CODE END time_1s_cb */
}

/* ld3320_uartrecv_tim_cb function */
void ld3320_uartrecv_tim_cb(void const * argument)
{
  /* USER CODE BEGIN ld3320_uartrecv_tim_cb */
    char city_name[50] = {0};
		uint8_t ret = 0;
		
    stm32_voice_weather_forecast_t *p_dev_st = &g_stm32_voice_weather_forecast_st;

    if (stm32_voice_weather_forecast_analysis_ld3320_data(p_dev_st->devices_info.p_ld3320_dev_st->uart_info_st.rxbuf, city_name) == 0)
    {
        // 获取天气数据
        memset(p_dev_st->devices_info.p_esp_at_dev_st->uart_info_st.rxbuf, 0, sizeof(p_dev_st->devices_info.p_esp_at_dev_st->uart_info_st.rxbuf));
        if ((ret = p_dev_st->devices_info.p_esp_at_dev_st->ops_func.get_weather(p_dev_st->devices_info.p_esp_at_dev_st, "fujianfuzhou")) != 0)
        {
            ERROR_PRINT("get_weather() fail[%d]\r\n", ret);
        }
        else
        {
            stm32_voice_weather_forecast_analysis_json_weather((char *)p_dev_st->devices_info.p_esp_at_dev_st->uart_info_st.rxbuf, p_dev_st->weather_info_st, 3);
        }
    }
    else
    {
        ERROR_PRINT("check rxbuf fail[%s]\r\n", p_dev_st->devices_info.p_ld3320_dev_st->uart_info_st.rxbuf);
        for(int i = 0; i < p_dev_st->devices_info.p_ld3320_dev_st->uart_info_st.rxindex; i++)
        {
            printf("%#x ", p_dev_st->devices_info.p_ld3320_dev_st->uart_info_st.rxbuf[i]);
        }
    }
    osTimerStop(ld3320_uartrecv_timHandle);
    // 处理完清空数据
    memset(&p_dev_st->devices_info.p_ld3320_dev_st->uart_info_st, 0,
           sizeof(p_dev_st->devices_info.p_ld3320_dev_st->uart_info_st));
    /* USER CODE END ld3320_uartrecv_tim_cb */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

