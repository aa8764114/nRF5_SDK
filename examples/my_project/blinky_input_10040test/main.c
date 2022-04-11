#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "boards.h"

#include"nrf_gpio.h"

#define led 17
#define button 13

int main(void)
{
    nrf_gpio_cfg_input(button, NRF_GPIO_PIN_PULLUP);
    bsp_board_init(BSP_INIT_LEDS);//一次初始化所有LED

    bsp_board_led_off(0);

    while(1)
    {
        nrf_delay_ms(10);
        if(nrf_gpio_pin_read(button) == 0)//如果有按下開關
        {
            bsp_board_led_invert(0);//本來如果是亮就變暗，暗就變亮
            while(nrf_gpio_pin_read(button) == 0);//如果發現按鈕一直按著就不動，卡在那邊
        }

    }
}


