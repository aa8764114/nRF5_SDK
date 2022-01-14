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

#define LED_Pin1 13
#define Btn_Pin1 11

static nrf_ppi_channel_t ppi_channel;//取得PPI頻道編號

void interrupt_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    //啥也不幹
}

static void gpiote_pins_init(void)
{
    uint32_t error_code = NRF_SUCCESS;
    
    //初始化GPIOTE
    error_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(error_code);
    
    //創建輸入腳位的插斷設定
    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
    in_config.pull = NRF_GPIO_PIN_PULLUP;
    
    //初始化插斷腳位(輸入)
    error_code = nrf_drv_gpiote_in_init(Btn_Pin1, &in_config, interrupt_pin_handler);
    APP_ERROR_CHECK(error_code);

    //創建輸出設定檔，並初始化輸出腳位(輸出)    
    nrf_drv_gpiote_out_config_t out_config = GPIOTE_CONFIG_OUT_TASK_TOGGLE(true);
    error_code = nrf_drv_gpiote_out_init(LED_Pin1, &out_config);
    APP_ERROR_CHECK(error_code);

    nrf_drv_gpiote_out_task_enable(LED_Pin1);
    nrf_drv_gpiote_in_event_enable(Btn_Pin1,true);
}

static void ppi_init(void)
{
    uint32_t error_code = NRF_SUCCESS;
    uint32_t btn_event_addr; //創建按鈕事件的地址變數
    uint32_t led_task_addr;  //創建LED任務任務地址

    //初始化PPI
    error_code = nrf_drv_ppi_init();
    APP_ERROR_CHECK(error_code);

    //為ppi_channel拿記憶體
    error_code = nrf_drv_ppi_channel_alloc(&ppi_channel);
    APP_ERROR_CHECK(error_code);
    
    btn_event_addr = nrf_drv_gpiote_in_event_addr_get(Btn_Pin1);//取得按鈕事件的記憶體位址
    led_task_addr = nrf_drv_gpiote_out_task_addr_get(LED_Pin1);//取得ＬＥＤ任務的記憶體位址

    //透過取得的ppi_channel編號，btn_event_addr，led_task_addr讓系統給一個ppi_channel
    error_code = nrf_drv_ppi_channel_assign(ppi_channel, btn_event_addr, led_task_addr);
    APP_ERROR_CHECK(error_code);

    //開啟ppi_channel
    error_code = nrf_drv_ppi_channel_enable(ppi_channel);
    APP_ERROR_CHECK(error_code);

}

int main(void)
{
    gpiote_pins_init();
    ppi_init();

    while (true)
    {
        // Do Nothing - GPIO can be toggled without software intervention.
    }
}