#include <stdbool.h>
#include <stdint.h>

#include "nrf.h"
#include "bsp.h"
#include "app_timer.h"
#include "app_error.h"
#include "nrf_drv_wdt.h"
#include "nrf_drv_clock.h"
#include "nrf_delay.h"
#include "app_util_platform.h"
#include "nrf_log.h"

nrfx_wdt_channel_id channel_ID;

void wdt_event_handler(void)
{

}

void wdt_init(void)
{
    uint32_t err_code = NRF_SUCCESS;
    nrfx_wdt_config_t w_config = NRFX_WDT_DEAFULT_CONFIG;

    err_code = nrfx_wdt_init(&w_config, wdt_event_handler);
    APP_ERROR_CHECK(err_code);

    err_code = nrfx_wdt_channel_alloc(&channel_ID);
    APP_ERROR_CHECK(err_code);

    nrfx_wdt_enable();
}

int main(void)
{
    bsp_board_init(BSP_INIT_LEDS);
    nrf_gpio_cfg_input(BUTTON_1, NRF_GPIO_PIN_PULLUP);

    ret_code_t err_code = NRF_SUCCESS;
    err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);
    nrf_drv_clock_lfclk_request(NULL);

    for(int i = 0; i < 10; i++)
    {
        bsp_board_led_invert(0);
        nrf_delay_ms(100);
    }
    bsp_board_leds_off();
    wdt_init();
    NRF_LOG_INFO("還沒餵狗！");

    while (1)
    {
        if(nrf_gpio_pin_read(BUTTON_1)==0)
        {
            bsp_board_leds_on();
        }
        while(nrf_gpio_pin_read(BUTTON_1)==0);
        bsp_board_leds_off();

        nrfx_wdt_channel_feed(channel_ID);
        NRF_LOG_INFO("餵狗！");
    }
}