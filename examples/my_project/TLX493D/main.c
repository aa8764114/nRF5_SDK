#include <stdio.h>
#include "boards.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "nrf_drv_twi.h"
#include "nrf_delay.h"



#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

/* TWI instance ID. */
#if TWI0_ENABLED
#define TWI_INSTANCE_ID     0
#elif TWI1_ENABLED
#define TWI_INSTANCE_ID     1
#endif


#define TLI493D_ADDRESS 0x35
#define SCL_PIN 27
#define SDA_PIN 26
#define INT_PIN 2

/* TWI instance. */
//nRF52有兩組I2C(TWI)要開哪一個
static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);//開 TWI0


ret_code_t TLI493D_twi_init (void)
{
    ret_code_t err_code;

    const nrf_drv_twi_config_t twi_config = {
            .scl                = SCL_PIN, //SCL接腳要用哪個GPIO
            .sda                = SDA_PIN, //SDA接腳要用哪個GPIO
            .frequency          = NRF_DRV_TWI_FREQ_400K, //I2C頻率
            .interrupt_priority = APP_IRQ_PRIORITY_HIGH, //插斷優先序
            .clear_bus_init     = false //是否清除之前PIN的设置
    };

    err_code = nrf_drv_twi_init(&m_twi, &twi_config, NULL, NULL);
    if (err_code != NRF_SUCCESS)
    {
        NRF_LOG_INFO("Failed to initialize TWI, err_code = %d", err_code);
        return err_code;
    }
    //等0.1s
    nrf_delay_ms(100);

    nrf_drv_twi_enable(&m_twi); //開啟TWI
    return  NRF_SUCCESS;
}

ret_code_t set_mode()
{
    ret_code_t err_code;
    uint8_t reg[3] = {0x10, 0b00000000, 0b00011001};
    err_code = nrf_drv_twi_tx(&m_twi, TLI493D_ADDRESS, reg, sizeof(reg), false);
    APP_ERROR_CHECK(err_code);
    return err_code;
}

void TLI493D_pin_setup()
{
    nrf_gpio_cfg_input(INT_PIN, NRF_GPIO_PIN_NOPULL);
}

ret_code_t TLI493D_init()
{
    ret_code_t err_code;

    //開啟I2C
    err_code = TLI493D_twi_init();
    if (err_code != NRF_SUCCESS)
    {
        NRF_LOG_INFO("Failed to create I2C, err_code = %d", err_code);
        return err_code;
    }

    //等0.1s
    nrf_delay_ms(100);

    //reset sensor
//    uint8_t reg = 0x00;
//    err_code = nrf_drv_twi_tx(&m_twi, 0x00, &reg, sizeof(reg), false);
//    APP_ERROR_CHECK(err_code);

    err_code = set_mode();
    APP_ERROR_CHECK(err_code);

    //設定軟體操控按鍵
    TLI493D_pin_setup();

    //等0.1s
    nrf_delay_ms(100);

    //觸發第一個測量
    uint8_t reg2 = 0b00100000;
    err_code = nrf_drv_twi_tx(&m_twi, TLI493D_ADDRESS, &reg2, sizeof(reg2), false);
    APP_ERROR_CHECK(err_code);
    nrf_delay_ms(100);

    return NRF_SUCCESS;
}

ret_code_t TLI493D_data_read()
{
    ret_code_t err_code;
    // In case of I2C, read the additional status byte.
    uint8_t buf[7];

    NRF_LOG_INFO("Reading (%d bytes): ", sizeof(buf));
    err_code = nrf_drv_twi_rx(&m_twi, TLI493D_ADDRESS, buf, sizeof(buf));
    if (err_code != NRF_SUCCESS)
    {
        NRF_LOG_INFO("Failed while calling TWI rx, err_code = %d", err_code);
        return err_code;
    }
//    else
//    {
//        for(int i = 0; i < sizeof(buf) ; i++)
//        {
//            NRF_LOG_INFO("sample_data:0x%x\n", buf[i])
//        }
//    }

    // Built 12 bit data
    int16_t X = (int16_t)((buf[0] << 8) | (buf[4] & 0xF0)) >> 4;
    int16_t Y = (int16_t)((buf[1] << 8) | ((buf[4] & 0x0F) << 4)) >> 4;
    int16_t Z = (int16_t)((buf[2] << 8) | ((buf[5] & 0x0F) << 4)) >> 4;
    uint16_t T = (buf[3] << 4) | (buf[5] >> 4);

    NRF_LOG_INFO("X:%d Y:%d Z:%d T:%d", X, Y, Z, T);

    return NRF_SUCCESS;
}


/**
 * @brief Function for main application entry.
 */
int main(void)
{
    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    NRF_LOG_INFO("TWI scanner started.");
    NRF_LOG_FLUSH();

    APP_ERROR_CHECK(TLI493D_init());
    APP_ERROR_CHECK(TLI493D_data_read());


    NRF_LOG_FLUSH();


    while (true)
    {
//        APP_ERROR_CHECK(TLI493D_data_read());
        /* Empty loop. */
    }
}

/** @} */
