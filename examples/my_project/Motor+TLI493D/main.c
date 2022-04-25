#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "boards.h"

#include"nrf_gpio.h"

/*
 * LED亮：GPIO低電位
 * LED暗：GPIO高電位
 *
 * motor_switch：操控馬達開關GPIO
 * motor_reverse：操控馬達正反轉GPIO
 *
 * switch_btn：操控馬達開關按鍵
 * reverse_btn：操控馬達正反轉按鍵
 * */

#define motor_switch 31
#define motor_reverse 30

//led_test
//#define motor_switch 17
//#define motor_reverse 18

#define switch_btn 13
#define reverse_btn 14

int main(void)
{
    nrf_gpio_cfg_input(switch_btn, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(reverse_btn, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_output(motor_switch);
    nrf_gpio_cfg_output(motor_reverse);

    nrf_gpio_pin_clear(motor_switch);
    nrf_gpio_pin_clear(motor_reverse);

    while(1)
    {
        nrf_delay_ms(10);
        if(nrf_gpio_pin_read(switch_btn) == 0)//如果有按下開關
        {
            nrf_gpio_pin_toggle(motor_switch);//本來如果是轉就變不轉，不轉就變轉
            while(nrf_gpio_pin_read(switch_btn) == 0);//如果發現按鈕一直按著就不動，卡在那邊
        }

        if(nrf_gpio_pin_read(reverse_btn) == 0)//如果有按下開關
        {
            nrf_gpio_pin_toggle(motor_reverse);//本來如果是轉就變不轉，不轉就變轉
            while(nrf_gpio_pin_read(reverse_btn) == 0);//如果發現按鈕一直按著就不動，卡在那邊
        }
    }
}


