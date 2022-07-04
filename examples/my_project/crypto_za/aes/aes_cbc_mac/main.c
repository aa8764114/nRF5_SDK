#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <ctype.h>


#include "nrf.h"
#include "nrf_drv_clock.h"
#include "nrf_delay.h"

#include "nrf_drv_power.h"

#include "app_error.h"
#include "app_util.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "boards.h"

#include "nrf_crypto.h"
#include "nrf_crypto_error.h"
#include "mem_manager.h"

static void crypt_cbc_mac(void) {
    uint8_t mac[16];
    nrf_crypto_aes_context_t mac_ctx;
    nrf_crypto_aes_info_t const *p_cmac_info;
    p_cmac_info = &g_nrf_crypto_aes_cmac_128_info;
    nrf_crypto_aes_init(&mac_ctx, p_cmac_info, NRF_CRYPTO_MAC_CALCULATE);
    static uint8_t m_key[16] = {0x00, 0x11, 0x22, 0x33,
                                0x44, 0x55, 0x66, 0x77,
                                0x88, 0x99, 0x00, 0x11,
                                0x22, 0x33, 0x44, 0x55};
    nrf_crypto_aes_key_set(&mac_ctx, m_key);
    static uint8_t m_data[4] = {0x00, 0x11, 0x22, 0x33};
    size_t len_out = sizeof(mac);
    nrf_crypto_aes_finalize(&mac_ctx,
                            (uint8_t *) m_data,
                            sizeof(m_data),
                            (uint8_t *) mac,
                            &len_out);

    nrf_crypto_aes_uninit(&mac_ctx);
    NRF_LOG_HEXDUMP_INFO(mac, len_out)
//    ans  2c58d707 6d6e4ae2 2120b503 4bbe0a16
}

int main(void) {

    NRF_LOG_INIT(NULL);
    NRF_LOG_DEFAULT_BACKENDS_INIT();
//    nrf_drv_clock_init();
//    nrf_drv_clock_lfclk_request(NULL);

    nrf_crypto_init();
    nrf_mem_init();
    crypt_cbc_mac();
    while (true) {
        NRF_LOG_FLUSH();
        UNUSED_RETURN_VALUE(NRF_LOG_PROCESS());
    }
}


