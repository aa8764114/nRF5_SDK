#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "nrf_drv_timer.h"
#include "bsp.h"
#include "app_error.h"

//開啟timer0
const nrfx_timer_t TIMER_LED = NRFX_TIMER_INSTANCE(0);

#define LED1 13
#define LED4 16

void timer_handler(nrf_timer_event_t event_type, void* p_context)
{
    switch(event_type)
    {
        case NRF_TIMER_EVENT_COMPARE0:

        nrf_gpio_pin_toggle(LED1);
        nrf_gpio_pin_toggle(LED4);


        break;

        default:
        //不做事
        break;

    }
}

void timer_init(void)
{
    uint32_t err_code = NRF_SUCCESS;
    uint32_t time_ms = 500;
    uint32_t time_ticks;
    
    //把建出來的timer設成預設值
    nrfx_timer_config_t timer_cfg = NRFX_TIMER_DEFAULT_CONFIG;
    err_code = nrfx_timer_init(&TIMER_LED, &timer_cfg, timer_handler);
    APP_ERROR_CHECK(err_code); //確定有沒有發生錯誤
    
    //將毫秒轉換成時脈
    time_ticks = nrfx_timer_ms_to_ticks(&TIMER_LED, time_ms);
    
    //設定一個頻道，傳遞ticks數字，以及開啟插斷
    nrfx_timer_extended_compare(&TIMER_LED, NRF_TIMER_CC_CHANNEL0, time_ticks, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);



}

int main(void)
{
    //初始化PIN
    nrf_gpio_cfg_output(LED1);
    nrf_gpio_cfg_output(LED4);
    
    //關閉這兩個接腳的LED
    nrf_gpio_pin_set(LED1);
    nrf_gpio_pin_set(LED4);

    timer_init();

    nrfx_timer_enable(&TIMER_LED);


    
    while (1)
    {
        __WFI();
    }
}