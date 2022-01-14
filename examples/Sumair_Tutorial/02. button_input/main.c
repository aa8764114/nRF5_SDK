#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"

#include"nrf_gpio.h"

#define led 13
#define button 11

int main(void)
{
  nrf_gpio_cfg_output(led);
  nrf_gpio_cfg_input(button, NRF_GPIO_PIN_PULLUP);

  nrf_gpio_pin_set(led);//設定邏輯訊號1(OFF)
  
  while(1)
  {
    if(nrf_gpio_pin_read(button) == 0)//如果有按下開關
    {
      nrf_gpio_pin_clear(led);//設定邏輯訊號0(ON)
      while(nrf_gpio_pin_read(button) == 0);//如果發現按鈕一直按著就不動，卡在那邊

      nrf_gpio_pin_set(led);//設定邏輯訊號1(OFF)
    }

  }
}


