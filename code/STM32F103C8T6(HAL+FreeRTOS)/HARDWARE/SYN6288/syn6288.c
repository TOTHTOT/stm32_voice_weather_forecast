/*
 * @Description: syn688芯片的驱动, 传入中文字符需要使用 ANSI 编码格式, keil 编译器使用 GB2312 编码格式
 * @Author: TOTHTOT
 * @Date: 2024-05-01 12:09:08
 * @LastEditTime: 2024-05-01 19:45:42
 * @LastEditors: TOTHTOT
 * @FilePath: \stm32_voice_weather_forecast\code\STM32F103C8T6(HAL+FreeRTOS)\HARDWARE\SYN6288\syn6288.c
 */

#include "syn6288.h"
#include "usart.h"

/* 用户代码开始 */
// syn6288的串口发送
uint8_t syn6288_uart_transmit(uint8_t *data, uint8_t len)
{
    return HAL_UART_Transmit(&huart1, data, len, 1000);
}
/* 用户代码结束 */

/* 函数声明 */
uint8_t syn6288_send_frame_info(struct syn6288_device *p_dev_st, uint8_t music, uint8_t *hzdata);

/* 全局变量 */
syn6288_device_t g_syn6288_device_st = {
    .ops_func.uart_transmit = syn6288_uart_transmit,
    .ops_func.send_frame_info = syn6288_send_frame_info,
};



/* 芯片设置命令  最后一个字节是前面所有数的异或，可以用计算器计算*/
uint8_t SYN_StopCom[] = {0xFD, 0X00, 0X02, 0X02, 0XFD};        //停止合成
uint8_t SYN_SuspendCom[] = {0XFD, 0X00, 0X02, 0X03, 0XFC}; //暂停合成
uint8_t SYN_RecoverCom[] = {0XFD, 0X00, 0X02, 0X04, 0XFB};  //恢复合成
/* 返回0x4E表名芯片任在合成播音中，返回0x4F表名芯片处于空闲状态*/
uint8_t SYN_ChackCom[] = {0XFD, 0X00, 0X02, 0X21, 0XDE};    //状态查询
uint8_t SYN_PowerDownCom[] = {0XFD, 0X00, 0X02, 0X88, 0X77};  //进入POWER DOWN 状态命令
/* 修改波特率，有3中模式 命令参数的低3位 0x00  0x01 0x02 分别对应9600 ,19200 38400*/
uint8_t SYN_SetBond_9600[] = {0xFD,0x00,0x03,0x31,0x00,0xCF}; // 修改波特率 9600
uint8_t SYN_SetBond_19200[] = {0xFD,0x00,0x03,0x31,0x01,0xCE}; // 修改波特率 19200
uint8_t SYN_SetBond_38400[] = {0xFD,0x00,0x03,0x31,0x02,0xCD}; // 修改波特率  38400

/**
 * @name: syn6288_send_frame_info
 * @msg: 发送合成的文字信息
 * @param {syn6288_device} *p_dev_st
 * @param {uint8_t} music 选择背景音乐。0:无背景音乐，1~15：选择背景音乐
 * @param {uint8_t} *hzdata 要发送的文本，一个字符串
 * @return {*}
 * @author: TOTHTOT
 * @Date: 2024-05-01 18:11:46
 */
uint8_t syn6288_send_frame_info(struct syn6288_device *p_dev_st, uint8_t music, uint8_t *hzdata)
{
    uint8_t Frame_Info[200];
    uint8_t HZ_Length; // 文本的长度
    uint8_t ecc = 0;   //定义校验字节
    uint16_t i = 0;
    HZ_Length = strlen((char *)hzdata); //需要发送文本的长度

    /* 帧固定配置信息 */
    Frame_Info[0] = 0xFD;          //构造帧头FD
    Frame_Info[1] = 0x00;          //构造数据区长度的高字节 重要 重要 重要 由于第二个字节是0x00 舍友无法使用u2_printf直接发送数据只能单个字符发送
    Frame_Info[2] = HZ_Length + 3; //构造数据区长度的低字节
    Frame_Info[3] = 0x01;          //构造命令字：合成播放命令
    /* 字节高5位表示背景音乐 ，低3位表示编码格式 0 是GB2312编码*/
    Frame_Info[4] = 0x01 | music << 4; //构造命令参数：背景音乐设定

    /*  校验码计算 */
    for (i = 0; i < 5; i++) //依次发送构造好的5个帧头字节
    {
        ecc = ecc ^ (Frame_Info[i]); //对发送的字节进行异或校验
    }

    for (i = 0; i < HZ_Length; i++) //依次发送待合成的文本数据
    {
        ecc = ecc ^ (hzdata[i]); //对发送的字节进行异或校验
    }
    /* 发送帧数据 */
    memcpy(&Frame_Info[5], hzdata, HZ_Length);
    Frame_Info[5 + HZ_Length] = ecc;

    if (p_dev_st->ops_func.uart_transmit(Frame_Info, 5 + HZ_Length + 1) != 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

