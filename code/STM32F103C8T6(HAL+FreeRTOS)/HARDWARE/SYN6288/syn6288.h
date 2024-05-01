/*
 * @Author: TOTHTOT
 * @Date: 2022-02-28 14:14:33
 * @LastEditTime: 2024-05-01 18:24:35
 * @LastEditors: TOTHTOT
 * @Description: 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 * @FilePath: \stm32_voice_weather_forecast\code\STM32F103C8T6(HAL+FreeRTOS)\HARDWARE\SYN6288\syn6288.h
 */
#ifndef __SYN6288_H__
#define __SYN6288_H__
#include <stdbool.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* 声音提示音列表  25段声音提示音
 sounda  出错声
 soundb  刷卡成功
 soundc  刷卡成功
 soundd  刷卡成功
 sounde  刷卡成功
 soundf   激光声
 soundg  门铃声
 soundh  门铃声
 soundi   电话铃声
 soundj   电话铃声
 soundk  广播提示音
 soundl   提示音
 soundm  提示音
 soundn   警报
 soundo   警报
 soundp   警报
 soundq  紧急警报
 soundr   紧急警报
 sounds  布谷声
 soundt   提示音
 soundu  提示音
 soundv  提示音
 soundw 提示音
 soundx  提示音
 soundy  提示音
*/



/* 和弦提示音列表  8首
msga   1s
msgb   1s
msgc   1s
msgd   1s
msge   2s
msgf    3s
msgg   4s
msgh   5s
*/

/* 和弦铃声   15首
ringa    60s
ringb    70s
ringc    27s
ringd    65a
ringe    60s
ringf     57s
ringg    60s
ringh    53s
ringi     35s
ringj     25s
ringk    18s
ringl     38s
ringm   60s
ringn    23s
ringo    5s
*/

/* 类型定义 */
typedef struct syn6288_device
{
    struct syn6288_ops_func
    {
        /* 用户实现函数 */
        uint8_t (*uart_transmit)(uint8_t *data, uint8_t len); // 发送命令
        uint8_t (*read_busy_pin)(void); // 读取芯片忙状态
        /* 外部函数 */
        bool (*is_busy)(struct syn6288_device); // 判断芯片是否处于空闲状态
        uint8_t (*send_frame_info)(struct syn6288_device *p_dev_st, uint8_t music, uint8_t *hzdata);// 发送文字转语音
    }ops_func;
    
}syn6288_device_t;

/* 全局变量 */
extern syn6288_device_t g_syn6288_device_st;

#if 0 // 没用到
/* 芯片设置命令  最后一个字节是前面所有数的异或，可以用计算器计算*/
extern uint8_t SYN_StopCom[];    //停止合成
extern uint8_t SYN_SuspendCom[]; //暂停合成
extern uint8_t SYN_RecoverCom[]; //恢复合成
/* 返回0x4E表名芯片任在合成播音中，返回0x4F表名芯片处于空闲状态*/
extern uint8_t SYN_ChackCom[];     //状态查询
extern uint8_t SYN_PowerDownCom[]; //进入POWER DOWN 状态命令
/* 修改波特率，有3中模式 命令参数的低3位 0x00  0x01 0x02 分别对应9600 ,19200 38400*/
extern uint8_t SYN_SetBond_9600[];  // 修改波特率 9600
extern uint8_t SYN_SetBond_19200[]; // 修改波特率 19200
extern uint8_t SYN_SetBond_38400[]; // 修改波特率  38400
#endif

#endif /* __SYN6288_H__ */

