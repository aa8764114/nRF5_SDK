#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "nrf_gpio.h"

#define led 13

int main(void)
{
  nrf_gpio_cfg_output(13);

  while(1)
  {
    nrf_gpio_pin_set(led);//送出邏輯訊號1(OFF)
    nrf_delay_ms(500);
    nrf_gpio_pin_clear(led);//送出邏輯訊號0(ON)
    nrf_delay_ms(500);
  }
}

