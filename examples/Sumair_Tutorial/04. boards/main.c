#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "boards.h"

//之前都透過ＧＰＩＯ找ＬＥＤ還有開關在哪一個接腳
//但其實boards.h已經透過define將每個接腳的編號對應到一個名字了
//所以就不需要背接腳數字

int main(void)
{
  bsp_board_init(BSP_INIT_LEDS);//一次初始化所有LED

  while(1)
  {
    /*
    bsp_board_led_on(0);//讓第0個LED亮（不用管是第幾個接腳，只要知道是在板子上的第幾個LED）
    nrf_delay_ms(500);
    bsp_board_led_off(0);
    nrf_delay_ms(500);
    */

    
    nrf_delay_ms(500);
    bsp_board_led_invert(1);//本來如果是亮就變暗，暗就變亮
    nrf_delay_ms(500);
    

    //看來不能用這個函數全開關
    /*
    bsp_board_led_on();//全開
    nrf_delay_ms(500);
    bsp_board_led_off();//全關
    nrf_delay_ms(500);
    */
  }
}

