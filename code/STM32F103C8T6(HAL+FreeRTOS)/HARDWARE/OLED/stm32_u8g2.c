#include "stm32_u8g2.h"
#include "delay.h"
#include "i2c.h"

uint8_t u8x8_byte_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    /* u8g2/u8x8 will never send more than 32 bytes between START_TRANSFER and END_TRANSFER */
    static uint8_t buffer[128];
    static uint8_t buf_idx;
    uint8_t *data;

    switch (msg)
    {
        case U8X8_MSG_BYTE_INIT:
        {
            /* add your custom code to init i2c subsystem */
            MX_I2C1_Init(); //I2C初始化
        }
            break;

        case U8X8_MSG_BYTE_START_TRANSFER:
        {
            buf_idx = 0;
        }
            break;

        case U8X8_MSG_BYTE_SEND:
        {
            data = (uint8_t *)arg_ptr;

            while (arg_int > 0)
            {
                buffer[buf_idx++] = *data;
                data++;
                arg_int--;
            }
        }
            break;

        case U8X8_MSG_BYTE_END_TRANSFER:
        {
            if (HAL_I2C_Master_Transmit(&hi2c1, OLED_ADDRESS, buffer, buf_idx, 1000) != HAL_OK)
                return 0;
        }
            break;

        case U8X8_MSG_BYTE_SET_DC:
            break;

        default:
            return 0;
    }

    return 1;
}



uint8_t u8x8_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    switch (msg)
    {
        case U8X8_MSG_DELAY_100NANO: // delay arg_int * 100 nano seconds
            __NOP();
            break;
        case U8X8_MSG_DELAY_10MICRO: // delay arg_int * 10 micro seconds
            for (uint16_t n = 0; n < 320; n++)
            {
                __NOP();
            }
            break;
        case U8X8_MSG_DELAY_MILLI: // delay arg_int * 1 milli second
            HAL_Delay(1);
            break;
        case U8X8_MSG_DELAY_I2C: // arg_int is the I2C speed in 100KHz, e.g. 4 = 400 KHz
            delay_us(5);
            break;                    // arg_int=1: delay by 5us, arg_int = 4: delay by 1.25us
        case U8X8_MSG_GPIO_I2C_CLOCK: // arg_int=0: Output low at I2C clock pin
            break;                    // arg_int=1: Input dir with pullup high for I2C clock pin
        case U8X8_MSG_GPIO_I2C_DATA:  // arg_int=0: Output low at I2C data pin
            break;                    // arg_int=1: Input dir with pullup high for I2C data pin
        case U8X8_MSG_GPIO_MENU_SELECT:
            u8x8_SetGPIOResult(u8x8, /* get menu select pin state */ 0);
            break;
        case U8X8_MSG_GPIO_MENU_NEXT:
            u8x8_SetGPIOResult(u8x8, /* get menu next pin state */ 0);
            break;
        case U8X8_MSG_GPIO_MENU_PREV:
            u8x8_SetGPIOResult(u8x8, /* get menu prev pin state */ 0);
            break;
        case U8X8_MSG_GPIO_MENU_HOME:
            u8x8_SetGPIOResult(u8x8, /* get menu home pin state */ 0);
            break;
        default:
            u8x8_SetGPIOResult(u8x8, 1); // default return value
            break;
    }
    return 1;
}

//U8g2的初始化，需要调用下面这个u8g2_Setup_ssd1306_128x64_noname_f函数，该函数的4个参数含义：
//u8g2：传入的U8g2结构体
//U8G2_R0：默认使用U8G2_R0即可（用于配置屏幕是否要旋转）
//u8x8_byte_sw_i2c：使用软件IIC驱动，该函数由U8g2源码提供
//u8x8_gpio_and_delay：就是上面我们写的配置函数

/**
 * @name: u8g2_init
 * @msg: 初始化u8g2相关功能以及屏幕
 * @param {u8g2_t} *u8g2
 * @return {*}
 * @author: TOTHTOT
 * @Date: 2024-05-04 10:55:51
 */
void u8g2_init(u8g2_t *u8g2)
{
    u8g2_Setup_ssd1306_i2c_128x64_noname_f(u8g2, U8G2_R0, u8x8_byte_hw_i2c, u8x8_gpio_and_delay); // 初始化u8g2 结构体
    u8g2_InitDisplay(u8g2);                                                                       // 
    u8g2_SetPowerSave(u8g2, 0);                                                                   // 
    u8g2_ClearBuffer(u8g2);
}

/**
 * @name: u8g2_draw
 * @msg: 测试用的写屏函数
 * @param {u8g2_t} *u8g2
 * @return {*}
 * @author: TOTHTOT
 * @Date: 2024-05-04 10:55:29
 */
void u8g2_draw(u8g2_t *u8g2)
{
    u8g2_ClearBuffer(u8g2);

    u8g2_SetFontMode(u8g2, 1); /*字体模式选择*/
    u8g2_SetFontDirection(u8g2, 0); /*字体方向选择*/
    u8g2_SetFont(u8g2, u8g2_font_inb24_mf); /*字库选择*/
    u8g2_DrawStr(u8g2, 0, 20, "U");

    u8g2_SetFontDirection(u8g2, 1);
    u8g2_SetFont(u8g2, u8g2_font_inb30_mn);
    u8g2_DrawStr(u8g2, 21,8,"8");

    u8g2_SetFontDirection(u8g2, 0);
    u8g2_SetFont(u8g2, u8g2_font_inb24_mf);
    u8g2_DrawStr(u8g2, 51,30,"g");
    u8g2_DrawStr(u8g2, 67,30,"\xb2");

    u8g2_DrawHLine(u8g2, 2, 35, 47);
    u8g2_DrawHLine(u8g2, 3, 36, 47);
    u8g2_DrawVLine(u8g2, 45, 32, 12);
    u8g2_DrawVLine(u8g2, 46, 33, 12);

    u8g2_SetFont(u8g2, u8g2_font_4x6_tr);
    u8g2_DrawStr(u8g2, 1,54,"github.com/olikraus/u8g2");

    u8g2_SendBuffer(u8g2);
    HAL_Delay(1000);
}

/* 用户代码开始 */
#include "stm32_voice_weather_forecast.h"

/**
 * @name: u8g2_refresh_scr
 * @msg: 刷新屏幕
 * @param {void} *private_data
 * @return {*}
 * @author: TOTHTOT
 * @Date: 2024-05-08 20:47:53
 */
uint8_t u8g2_refresh_scr(void *private_data)
{
    char frame_buffer[80] = {0};
    stm32_voice_weather_forecast_t *p_dev_st = private_data;
    u8g2_ClearBuffer(&p_dev_st->devices_info.u8g2);

    u8g2_SetFont(&p_dev_st->devices_info.u8g2, u8g2_font_t0_11_te);
    // 显示时间日期
    sprintf(frame_buffer, "%04d-%02d-%02d %02d:%02d:%02d", p_dev_st->time_info.year, p_dev_st->time_info.month, p_dev_st->time_info.day, p_dev_st->time_info.hour, p_dev_st->time_info.minute, p_dev_st->time_info.second);
    u8g2_DrawStr(&p_dev_st->devices_info.u8g2, 1, 10, frame_buffer); // 在坐标 (0, 20) 处显示字符串
    memset(frame_buffer, 0, sizeof(frame_buffer));

    u8g2_SetFont(&p_dev_st->devices_info.u8g2, u8g2_font_5x8_mr);

    memset(frame_buffer, 0, sizeof(frame_buffer));
    sprintf(frame_buffer, "weather:%s", p_dev_st->weather_info_st[p_dev_st->cur_show_weather_info_index].weather);
    u8g2_DrawStr(&p_dev_st->devices_info.u8g2, 5, 20, frame_buffer);

    memset(frame_buffer, 0, sizeof(frame_buffer));
    sprintf(frame_buffer, "temp_h: %03d", p_dev_st->weather_info_st[p_dev_st->cur_show_weather_info_index].temperature_high);
    u8g2_DrawStr(&p_dev_st->devices_info.u8g2, 5, 30, frame_buffer);
    memset(frame_buffer, 0, sizeof(frame_buffer));
    sprintf(frame_buffer, "temp_l: %03d", p_dev_st->weather_info_st[p_dev_st->cur_show_weather_info_index].temperature_low);
    u8g2_DrawStr(&p_dev_st->devices_info.u8g2, 65, 30, frame_buffer);

    memset(frame_buffer, 0, sizeof(frame_buffer));
    sprintf(frame_buffer, "humidity: %03d", p_dev_st->weather_info_st[p_dev_st->cur_show_weather_info_index].humidity);
    u8g2_DrawStr(&p_dev_st->devices_info.u8g2, 5, 40, frame_buffer);

    memset(frame_buffer, 0, sizeof(frame_buffer));
    sprintf(frame_buffer, "wind speed = %2.1f", p_dev_st->weather_info_st[p_dev_st->cur_show_weather_info_index].wind_speed);
    u8g2_DrawStr(&p_dev_st->devices_info.u8g2, 5, 50, frame_buffer);


    u8g2_SendBuffer(&p_dev_st->devices_info.u8g2); // 发送缓冲区内容到 OLED 显示屏
    return 0;
}
/* 用户代码结束 */
