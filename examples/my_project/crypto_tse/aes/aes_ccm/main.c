/**
 * Copyright (c) 2018 - 2021, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
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

static void crypt_ccm(void) {
    static uint8_t m_key[16] = {0x3f, 0x10, 0x98, 0x5b,
                                0x0d, 0x66, 0xb6, 0x9b,
                                0x0a, 0xad, 0x2e, 0x24,
                                0xca, 0xf9, 0x3e, 0x03};

    static uint8_t m_plain_text[3] = {0x02, 0x04, 0x01};
    static uint8_t m_encrypted_text[3];
    static uint8_t mac[4];
    static uint8_t nonce[13] = {0x00, 0x00, 0x00, 0x00, 0x80, 0xf4, 0xa6, 0x6f, 0x0f, 0x1f, 0xb1, 0xb6, 0xc0};
    static uint8_t adata[] = {0x00};

    static nrf_crypto_aead_context_t ccm_ctx;

    nrf_crypto_aead_init(&ccm_ctx,
                         &g_nrf_crypto_aes_ccm_128_info,
                         m_key);

    nrf_crypto_aead_crypt(&ccm_ctx,
                          NRF_CRYPTO_ENCRYPT,
                          nonce,
                          sizeof(nonce),
                          adata,
                          sizeof(adata),
                          m_plain_text,
                          sizeof(m_plain_text),
                          m_encrypted_text,
                          mac,
                          sizeof(mac));

    NRF_LOG_INFO("m_key")
    NRF_LOG_HEXDUMP_INFO(m_key, 16)//3F 10 98 5B 0D 66 B6 9B 0A AD 2E 24 CA F9 3E 03

    NRF_LOG_INFO("nonce")
    NRF_LOG_HEXDUMP_INFO(nonce, sizeof(nonce))//00 00 00 00 80 F4 A6 6F 0F 1F B1 B6 C0

    NRF_LOG_INFO("m_plain_text")
    NRF_LOG_HEXDUMP_INFO(m_plain_text, sizeof(m_plain_text))//02 04 01

    NRF_LOG_INFO("m_encrypted_text")
    NRF_LOG_HEXDUMP_INFO(m_encrypted_text, sizeof(m_encrypted_text))//58 EA C7

    NRF_LOG_INFO("mac")
    NRF_LOG_HEXDUMP_INFO(mac, sizeof(mac))//69 93 10 08
//    nonce:0000000080f4a66f0f1fb1b6c0
//    sessionKey:3f10985b0d66b69b0aad2e24caf93e03
//    plaintext:020401
//    加密結果:58eac769931008
}

int main(void) {
    ret_code_t ret;

    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    NRF_LOG_RAW_INFO("AES CCM example started.\r\n\r\n");
    NRF_LOG_FLUSH();

//    ret = nrf_drv_clock_init();
//    APP_ERROR_CHECK(ret);
//    nrf_drv_clock_lfclk_request(NULL);

    ret = nrf_crypto_init();
    APP_ERROR_CHECK(ret);

    ret = nrf_mem_init();
    APP_ERROR_CHECK(ret);

    crypt_ccm();
    while (true) {
        NRF_LOG_FLUSH();
        UNUSED_RETURN_VALUE(NRF_LOG_PROCESS());
    }
}



