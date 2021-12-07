/**

 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "nrf.h"
#include "nrf_drv_saadc.h"
#include "nrf_drv_ppi.h"
#include "nrf_drv_timer.h"
#include "boards.h"
#include "app_error.h"
#include "nrf_delay.h"
#include "app_util_platform.h"
#include "nrf_pwr_mgmt.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"


/* Create an empty handler and pass this handler in the saadc initialization function
  > Normally this handler deals with the adc events but we are using blocking mode
  > In blocking mode the functions are called and the processor waits for the adc to finish taking samples from the respective channels
  > Event handler will not be called in this method
*/

void saadc_callback_handler(nrf_drv_saadc_evt_t const *p_event) {
    // Empty handler function
}




// Create a function which configures the adc input pins and channels as well as the mode of operation of adc

void saadc_init(void) {
    // A variable to hold the error code
    ret_code_t err_code;

    // Create a config struct and assign it default values along with the Pin number for ADC Input
    // Configure the input as Differential Mode(Two inputs Reading)
    // Make sure you allocate the right pins.
    // Pass the pins which you want to use in differential mode
    // we have used VDD internal voltage as first parameter for input you can use any pin as well
// second paramter is the second pin to be used

    nrf_saadc_channel_config_t channel_config = NRFX_SAADC_DEFAULT_CHANNEL_CONFIG_DIFFERENTIAL(NRF_SAADC_INPUT_AIN1,
                                                                                               NRF_SAADC_INPUT_AIN0);

    // Initialize the saadc
    // first parameter is for configuring the adc resolution and other features, we will see in future tutorial
    //on how to work with it. right now just pass a simple null value
    err_code = nrf_drv_saadc_init(NULL, saadc_callback_handler);
    APP_ERROR_CHECK(err_code);

// Initialize the Channel which will be connected to that specific pin.
    err_code = nrfx_saadc_channel_init(0, &channel_config);
    APP_ERROR_CHECK(err_code);


}


// A function which will initialize the Log module for us
void log_init(void) {
    // check if any error occurred during its initialization
    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));

    // Initialize the log backends module
    NRF_LOG_DEFAULT_BACKENDS_INIT();

}

/**
 * @brief Function for main application entry.
 */
int main(void) {

    // call the log initialization function
    log_init();

// call the saadc initialization function created above
    saadc_init();

// a struct to hold 16-bit value, create a variable of this type because our input resolution may vary from 8 bit to 14 bits depending on our configurations
// this variable holds the adc sample value
    nrf_saadc_value_t adc_val;


// Print a simple msg that everything started without any error
    NRF_LOG_INFO("Application Started!!!");



// Inifinite loop
    while (1) {
        // a blocking function which will be called and the processor waits until the value is read
        // the sample value read is in 2's complement and is automatically converted once retrieved
        // first parameter is for the adc input channel
        // second parameter is to pass the address of the variable in which we store our adc sample value
        nrfx_saadc_sample_convert(0, &adc_val);

        // print this value using nrf log : here %d represents the integer value
        NRF_LOG_INFO("Sample value Read: %d", adc_val);

        // use nrf log and float marker to show the floating point values on the log
        // In diferential modde we calculate the voltage by this: input_sample / 2 * 3.6 / 2^n (where n = 8 or 10 or 12 or 14 depending on our configuration for resolution in bits)
        //
        NRF_LOG_INFO("Volts: " NRF_LOG_FLOAT_MARKER "\r\n", NRF_LOG_FLOAT(adc_val * 3.6 / 512));

        // give 500ms delay
        nrf_delay_ms(500);

    }
}


/** @} */
