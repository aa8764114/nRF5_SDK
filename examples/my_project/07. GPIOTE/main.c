#include <stdbool.h>
#include "nrf.h"
#include "nrf_drv_gpiote.h"
#include "app_error.h"
#include "boards.h"

#define LED 13
#define Btn 11

void input_pin_handle(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    nrf_gpio_pin_toggle(LED);
}

void gpio_init()
{
    ret_code_t err_code;
    err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);

    nrf_gpio_cfg_output(LED);
    nrf_gpio_pin_set(LED);

    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_TOGGLE(false);
    in_config.pull = NRF_GPIO_PIN_PULLUP;

    err_code = nrf_drv_gpiote_in_init(Btn, &in_config, input_pin_handle);
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_event_enable(Btn, true);
}

int main(void)
{
    gpio_init();

    while (true)
    {
        // Do nothing.
    }
}

