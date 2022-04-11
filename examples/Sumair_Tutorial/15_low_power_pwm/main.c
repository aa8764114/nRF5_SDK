#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "app_error.h"
#include "sdk_errors.h"
#include "app_timer.h"
#include "nrf_drv_clock.h"
#include "app_util_platform.h"
#include "low_power_pwm.h"
#include "nordic_common.h"

static low_power_pwm_t low_power_pwm_0;
static volatile uint8_t m_duty = 0;

static void pwm_handler(void* p_context){}

static void pwm_init(void)
{
    ret_code_t err_code;
    low_power_pwm_config_t low_power_pwm_config;
    APP_TIMER_DEF(lpp_timer_0);

    low_power_pwm_config.active_high = false;
    low_power_pwm_config.period = 100;
//    low_power_pwm_config.bit_mask = BSP_LED_0_MASK;
    low_power_pwm_config.bit_mask = PIN_MASK(28);
    low_power_pwm_config.p_timer_id = &lpp_timer_0;
    low_power_pwm_config.p_port = NRF_GPIO;

    err_code = low_power_pwm_init(&low_power_pwm_0, &low_power_pwm_config, pwm_handler);
    APP_ERROR_CHECK(err_code);

    err_code = low_power_pwm_duty_set(&low_power_pwm_0, 20);
    APP_ERROR_CHECK(err_code);

    err_code = low_power_pwm_start(&low_power_pwm_0, low_power_pwm_0.bit_mask);
    APP_ERROR_CHECK(err_code);
}

//沒有開藍牙的時候需要自已開low_frequency_clock
static void lfclk_init(void)
{
    ret_code_t err_code;
    err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_clock_lfclk_request(NULL);

}

int main(void)
{
    ret_code_t err_code;

    lfclk_init();

    err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    pwm_init();

    while (true)
    {
        for(int i = 1; i <=100; i++)
        {
            err_code = low_power_pwm_duty_set(&low_power_pwm_0, i);
            nrf_delay_ms(10);
        }
        for(int i = 100; i >=1; i--)
        {
            err_code = low_power_pwm_duty_set(&low_power_pwm_0, i);
            nrf_delay_ms(10);
        }
    }
}