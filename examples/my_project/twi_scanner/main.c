#include <stdio.h>
#include "boards.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "nrf_drv_twi.h"



#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

/* TWI instance ID. */
#if TWI0_ENABLED
#define TWI_INSTANCE_ID     0
#elif TWI1_ENABLED
#define TWI_INSTANCE_ID     1
#endif

// I2C slave地址掃描範圍
 #define TWI_ADDRESSES      127

/* TWI instance. */
//nRF52有兩組I2C(TWI)要開哪一個
static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);//開 TWI0


/**
 * @brief TWI initialization.
 */
void twi_init (void)
{
    ret_code_t err_code;

    const nrf_drv_twi_config_t twi_config = {
       .scl                = ARDUINO_SCL_PIN, //SCL接腳要用哪個GPIO
       .sda                = ARDUINO_SDA_PIN, //SDA接腳要用哪個GPIO
       .frequency          = NRF_DRV_TWI_FREQ_400K, //I2C頻率
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH, //插斷優先序
       .clear_bus_init     = false //是否清除之前PIN的设置
    };

    /*
     * m_twi：開好的TWI用這個變數操作
     * twi_config：上面設定的TWI參數
     * */
    err_code = nrf_drv_twi_init(&m_twi, &twi_config, NULL, NULL);
    APP_ERROR_CHECK(err_code);

    nrf_drv_twi_enable(&m_twi); //開啟TWI
}

/**
 * @brief Function for main application entry.
 */
int main(void)
{
    ret_code_t err_code;
    uint8_t address;
    uint8_t sample_data[7];
    bool detected_device = false;

    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    NRF_LOG_INFO("TWI scanner started.");
    NRF_LOG_FLUSH();
    twi_init();

    for (address = 1; address <= TWI_ADDRESSES; address++)
    {
        //瘋狂一個個位址讀東西，如果成功代表位址有用
        err_code = nrf_drv_twi_rx(&m_twi, address, sample_data, sizeof(sample_data));
        if (err_code == NRF_SUCCESS)
        {
            detected_device = true;
            NRF_LOG_INFO("TWI device detected at address 0x%x.", address);
            NRF_LOG_INFO("sample_data:0x%x\n", sample_data[3])
        }
        NRF_LOG_FLUSH();
    }


    if (!detected_device)
    {
        NRF_LOG_INFO("No device was found.");
        NRF_LOG_FLUSH();
    }

    while (true)
    {
        /* Empty loop. */
    }
}

/** @} */
