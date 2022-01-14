#include <stdbool.h>
#include <stdint.h>

#include "nrf.h"
#include "nordic_common.h"
#include "boards.h"
#include "bsp.h"
#include "app_timer.h"
#include "nrf_error.h"
#include "nrf_drv_clock.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

void clock_init(void)
{
    ret_code_t err_code;
    err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_clock_lfclk_request(NULL); //不要產生中斷

}

void bsp_evt_handler(bsp_event_t evt)
{
    switch(evt)
    {
        case BSP_EVENT_KEY_0:
        bsp_board_led_on(1);
        NRF_LOG_INFO("LED_ON");
        break;

        case BSP_EVENT_KEY_1:
        bsp_board_led_off(1);
        NRF_LOG_INFO("LED_OFF");
        break;

        case BSP_EVENT_KEY_2:
        break;

        case BSP_EVENT_KEY_3:
        break;
    }
}

void bsp_config(void)
{
    ret_code_t err_code;
    err_code = bsp_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS, bsp_evt_handler);
    APP_ERROR_CHECK(err_code);
}

int main(void)
{
    ret_code_t err_code;

    clock_init();
    
    err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();

    bsp_config();

    NRF_LOG_INFO("APP_GO!!");

    NRF_LOG_FLUSH();

    
    while (true)
    {
        // Do nothing.
    }

}