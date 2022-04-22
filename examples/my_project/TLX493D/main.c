#include <stdio.h>
#include "boards.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "nrf_drv_twi.h"
#include "nrf_delay.h"
#include "math.h"


#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

/* TWI instance ID. */
#if TWI0_ENABLED
#define TWI_INSTANCE_ID     0
#elif TWI1_ENABLED
#define TWI_INSTANCE_ID     1
#endif

/* TLI493D參數 */
#define TLI493D_ADDRESS 0x35
#define SCL_PIN 27
#define SDA_PIN 26
#define INT_PIN 2

#define PI  3.14159265358979f

//nRF52有兩組I2C(TWI)要開哪一個
static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);//開 TWI0

void TLI493D_twi_init (void)
{
    const nrf_drv_twi_config_t twi_config = {
            .scl                = SCL_PIN, //SCL接腳要用哪個GPIO
            .sda                = SDA_PIN, //SDA接腳要用哪個GPIO
            .frequency          = NRF_DRV_TWI_FREQ_400K, //I2C頻率
            .interrupt_priority = APP_IRQ_PRIORITY_HIGH, //插斷優先序
            .clear_bus_init     = false //是否清除之前PIN的设置
    };
    nrf_drv_twi_init(&m_twi, &twi_config, NULL, NULL);  //用填入的設定檔初始化TWI(I2C)
    nrf_delay_ms(50); //等50ms

    nrf_drv_twi_enable(&m_twi); //啟動TWI
    nrf_delay_ms(50); //等50ms
}

void set_mode()
{
    //禁用Ｚ偵測、溫度偵測
    //Infineon-TLI_493D-W2BW-UserManual-v01_10-EN.pdf 第9頁
//    uint8_t reg2[2] = {0x10, 0b11000000};
//    nrf_drv_twi_tx(&m_twi, TLI493D_ADDRESS, reg2, sizeof(reg2), false);

    //Low Power Mode update rate
    //Infineon-TLI_493D-W2BW-UserManual-v01_10-EN.pdf 第13頁
    uint8_t reg3[2] = {0x13, 0b111};
    nrf_drv_twi_tx(&m_twi, TLI493D_ADDRESS, reg3, sizeof(reg3), false);

    //要設定X2跟X4這兩個空間->設定磁場偵測範圍
    //因為X2的暫存器空間也在0x10，所以可以把它跟禁用Ｚ偵測、溫度偵測的指令合併
    //0b11000000:禁用Ｚ偵測、溫度偵測（Infineon-TLI_493D-W2BW-UserManual-v01_10-EN.pdf 第9頁）
    //0b00001000:X2->1（Infineon-TLI_493D-W2BW-UserManual-v01_10-EN.pdf 第10頁）
    uint8_t reg4[2] = {0x10, (0b11000000|0b00001000)};
    nrf_drv_twi_tx(&m_twi, TLI493D_ADDRESS, reg4, sizeof(reg4), false);

    uint8_t reg5[2] = {0x14, 0b1};  //X4->1
    nrf_drv_twi_tx(&m_twi, TLI493D_ADDRESS, reg5, sizeof(reg5), false);
}

void TLI493D_pin_setup()
{
    nrf_gpio_cfg_input(INT_PIN, NRF_GPIO_PIN_NOPULL);
}

void TLI493D_init()
{
    TLI493D_twi_init();    //開啟I2C
    set_mode();    //設定TLI493D模式
    TLI493D_pin_setup();     //設定軟體操控按鍵
}

void TLI493D_data_read()
{
    //送出設定值觸發開始測量
    uint8_t config = 0b00100000;
    nrf_drv_twi_tx(&m_twi, TLI493D_ADDRESS, &config, sizeof(config), false);

    //讀取測量數據
    uint8_t buf[7];
    nrf_drv_twi_rx(&m_twi, TLI493D_ADDRESS, buf, sizeof(buf));

    //解析讀取數據轉ＸＹＺＴ
    int16_t X = (int16_t)((buf[0] << 8) | (buf[4] & 0xF0)) >> 4;
    int16_t Y = (int16_t)((buf[1] << 8) | ((buf[4] & 0x0F) << 4)) >> 4;
//    int16_t Z = (int16_t)((buf[2] << 8) | ((buf[5] & 0x0F) << 4)) >> 4;
//    uint16_t T = (buf[3] << 4) | (buf[5] >> 4);
//    NRF_LOG_INFO("X:%d Y:%d Z:%d T:%d", X, Y, Z, T)

    //把解析數據轉角度
    int degree = 0;
    degree = ((atan2(Y, X)*360)/(2*PI))+180;
    NRF_LOG_INFO("degree:%d", degree)
}

int main(void)
{
    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_DEFAULT_BACKENDS_INIT();
    NRF_LOG_INFO("TLI493D started.");
    TLI493D_init();

    while (true)
    {
        nrf_delay_ms(50);
        TLI493D_data_read();
        NRF_LOG_FLUSH();
    }
}