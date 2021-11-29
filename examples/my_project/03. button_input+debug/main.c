#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"

#include"nrf_gpio.h"

#define led 13
#define button 11

int main(void)
{
  printf("開始執行程式\r\n");
  nrf_gpio_cfg_output(led);
  nrf_gpio_cfg_input(button, NRF_GPIO_PIN_PULLUP);

  nrf_gpio_pin_set(led);//設定邏輯訊號1(OFF)
  printf("設定初始狀態，關\r\n");
  
  while(1)
  {
    if(nrf_gpio_pin_read(button) == 0)//如果有按下開關
    {
      nrf_gpio_pin_clear(led);//設定邏輯訊號0(ON)
      printf("按下按鈕\r\n");

      while(nrf_gpio_pin_read(button) == 0);//如果發現按鈕一直按著就不動，卡在那邊
      printf("按著不動\r\n");

      nrf_gpio_pin_set(led);//設定邏輯訊號1(OFF)
      printf("放開按鈕\r\n");

    }

  }
}


