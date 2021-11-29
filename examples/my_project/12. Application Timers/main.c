#include <stdbool.h>
#include <stdint.h>

#include "nrf.h"
#include "nordic_common.h"
#include "boards.h"
#include "nrf_drv_clock.h"

#define LEDPin1 13
#define LED_INTERVAL APP_TIMER_TICKS(100)

APP_TIMER_DEF(m_app_timer_id);

static void lfclk_config(void)
{
    ret_code_t err_code;
    err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_clock_lfclk_request(NULL);
}

static void timer_init(void)
{
    ret_code_t err_code;
    err_code = app_timer_init();

}

int main(void)
{
    while (true)
    {
        // Do nothing.
    }
}