#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "boards.h"

#include"nrf_gpio.h"

#define led 10
#define button 28

int main(void)
{
    nrf_gpio_cfg_input(button, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_output(led);


    bsp_board_led_off(0);

    while(1)
    {
        nrf_delay_ms(10);
        if(nrf_gpio_pin_read(button) == 0)//如果有按下開關
        {
            nrf_gpio_pin_toggle(led);//本來如果是亮就變暗，暗就變亮
            while(nrf_gpio_pin_read(button) == 0);//如果發現按鈕一直按著就不動，卡在那邊
        }
    }
}


