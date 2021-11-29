

#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "nrf_gpiote.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "nrf_drv_ppi.h"
#include "nrf_drv_timer.h"
#include "nrf_drv_gpiote.h"
#include "app_error.h"





// Create a hardware timer 0 handle
static nrf_drv_timer_t timer = NRF_DRV_TIMER_INSTANCE(0);


#define LED_Pin1 13 // Pin connected with LED



// Empty interrupt handler for dealing with interrupt events, you can use it in conjuction with PPI
// make sure it executes faster by not placing any functions that are slow to execute like delay etc
void timer0_interrupt_handler(nrf_timer_event_handler_t evt, void * p_context)
{

}




// A function to initialize and set everything for the timers with PPi and GPIOTE

static void start_timers_leds(void)
{
// Variables to hold the event address and task address
  // they are needed to connect with the PPI TEP(Task End Point) & PPI EEP (Event End Point)
    uint32_t compare_event_addr;
    uint32_t gpiote_task_addr;


    nrf_ppi_channel_t ppi_channel; // A struct to hol ppi channel values

 // Variable to hold error values
    ret_code_t err_code;


// Initialize gpiote module
    err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);

// Initialize the PPI module, make sure its only enabled once in your code
    err_code = nrf_drv_ppi_init();
    APP_ERROR_CHECK(err_code);

// Allocate the channel from the available PPI channels
    err_code = nrf_drv_ppi_channel_alloc(&ppi_channel);
    APP_ERROR_CHECK(err_code);



// enable and configure the task of LED pin with its function
// it can be:
// set high
// set low
// or toggle pin

// True means the Pin is at high state initially
// false means the Pin is at Low state initially
    nrf_drv_gpiote_out_config_t out_config = GPIOTE_CONFIG_OUT_TASK_TOGGLE(false);

    err_code = nrf_drv_gpiote_out_init(LED_Pin1, &out_config);
    APP_ERROR_CHECK(err_code);



// Create timers configurations and assign default configurations
    nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;

// initialze the timer with default configurations
    err_code = nrf_drv_timer_init(&timer, &timer_cfg, timer0_interrupt_handler);
    APP_ERROR_CHECK(err_code);

 // a function to calculate the number of ticks(1 tick =  1 counter increment) for desired milliseconds time  
    uint32_t time_ticks = nrfx_timer_ms_to_ticks(&timer, 500);

// configure the channel 0 of timer 0, and pass it the counter value at which the timer resets and generates an Interrupt Event
    nrf_drv_timer_extended_compare(&timer, NRF_TIMER_CC_CHANNEL0, time_ticks, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);


// Get the address of the respective event and tasks from the respective peripherals
    compare_event_addr = nrf_drv_timer_event_address_get(&timer, NRF_TIMER_EVENT_COMPARE0);
    gpiote_task_addr = nrf_drv_gpiote_out_task_addr_get(LED_Pin1);


// connect the EEP & TEP with Peripheral Events & Tasks using their addresses and assign them to an allocated channel
    err_code = nrf_drv_ppi_channel_assign(ppi_channel, compare_event_addr, gpiote_task_addr);
    APP_ERROR_CHECK(err_code);

// Enable the channel so that it can start receiving events and then route them to tasks
    err_code = nrf_drv_ppi_channel_enable(ppi_channel);
    APP_ERROR_CHECK(err_code);

// enable LED Pin task so that it can be executed when ever called by any event via PPI
    nrf_drv_gpiote_out_task_enable(LED_Pin1);
    






  }


/**
 * @brief Function for application main entry.
 */
int main(void)
{

  start_timers_leds(); // call the function created above to initialize and set everything

  nrf_drv_timer_enable(&timer); // start the timer
    

    while (true)
    {
        // Do Nothing - GPIO can be toggled without software intervention.
    }
}


/** @} */
