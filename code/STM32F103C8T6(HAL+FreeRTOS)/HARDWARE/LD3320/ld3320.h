#ifndef __LD3320_H__
#define __LD3320_H__

/* 头文件 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* 类型定义 */
typedef struct ld3320_device
{
    struct
    {
        uint8_t rxbuf[80];
        uint8_t rxtmp;
        uint8_t rxindex;
    }uart_info_st;
    struct
    {
        // 用户实现函数
        int32_t (*recv_data)(struct ld3320_device *p_dev_st);
        void (*delay_ms)(uint32_t ms);
    }ops_func;
}ld3320_device_t;

/* 全局变量 */
extern ld3320_device_t g_ld3320_device_st;

/* 全局函数 */
extern uint8_t ld3320_init(ld3320_device_t *p_dev_st);


#endif /* __LD3320_H__ */
