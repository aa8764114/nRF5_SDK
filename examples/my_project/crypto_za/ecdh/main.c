#include <stdbool.h>
#include <stdint.h>
#include "sdk_common.h"
#include "app_error.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_crypto.h"
#include "nrf_crypto_ecc.h"
#include "nrf_crypto_ecdh.h"
#include "nrf_crypto_error.h"
#include "mem_manager.h"

static void test_ecdh()
{
    //Alice的Key們
    static nrf_crypto_ecc_private_key_t alice_private_key;
    static nrf_crypto_ecc_public_key_t alice_public_key;
    static nrf_crypto_ecc_secp256r1_raw_private_key_t alice_private_key_raw;
    static nrf_crypto_ecc_secp256r1_raw_public_key_t alice_public_key_raw;
    static nrf_crypto_ecdh_secp256r1_shared_secret_t m_shared_secret_app;   //Alice算出的共同密鑰

    //各變數的大小
//    size_t size_app = sizeof(m_shared_secret_app);
    size_t size_alice_pb = sizeof(alice_public_key_raw);
    size_t size_alice_pr = sizeof(alice_private_key_raw);

    //Alice產生公私鑰(用已知的私要產生公鑰)
    uint8_t raw_pr_key[32] = {0x8A, 0xE7, 0x2E, 0x48, 0x76, 0x95, 0xC3, 0x4B, 0xF1, 0x56, 0x51, 0xB5, 0xBE, 0x4A, 0xE4,
                              0xFA, 0x92, 0x7F, 0xAA, 0xFB, 0xCA, 0x2A, 0x8C, 0x94, 0xBA, 0x67, 0x93, 0x8B, 0x28, 0xE4,
                              0xFC, 0xF1};
    nrf_crypto_ecc_private_key_from_raw(&g_nrf_crypto_ecc_secp256r1_curve_info,
                                        &alice_private_key,
                                        raw_pr_key, 32);

    nrf_crypto_ecc_public_key_calculate(NULL, &alice_private_key, &alice_public_key);


    //把各個內部處理的格式轉成陣列
    //Alice
    nrf_crypto_ecc_public_key_to_raw(
            &alice_public_key,
            alice_public_key_raw,
            &size_alice_pb);

    nrf_crypto_ecc_private_key_to_raw(
            &alice_private_key,
            alice_private_key_raw,
            &size_alice_pr);


    //Bob的Key們
    static nrf_crypto_ecc_public_key_t bob_public_key;

    //Bob的公鑰
    uint8_t raw_pu_key[64] = {0x2b, 0x2d, 0x94, 0x9b, 0xdc, 0xbd, 0xfe, 0xaf, 0xfb, 0x89, 0x4e, 0xac, 0xea, 0x33, 0x72,
                              0x39, 0x97, 0xf7, 0x00, 0xfc, 0xe1, 0x3a, 0xc1, 0xe6, 0xd4, 0xc3, 0x53, 0xb0, 0x90, 0x9f,
                              0xcb, 0x73, 0x36, 0x9e, 0xc1, 0xd3, 0x9a, 0x40, 0xb7, 0xdb, 0x85, 0x82, 0x1e, 0xdb, 0xf8,
                              0x47, 0x85, 0x25, 0x8b, 0x28, 0xcd, 0xec, 0xa2, 0x4c, 0x0f, 0x1e, 0xc7, 0x28, 0x3b, 0x88,
                              0xa9, 0x3a, 0x62, 0xc8};
    nrf_crypto_ecc_public_key_from_raw(&g_nrf_crypto_ecc_secp256r1_curve_info,
                                       &bob_public_key,
                                       raw_pu_key, 64);


    //Alice用自己的private key + Bob的public key 算出共同密鑰
    size_t size_app = sizeof(m_shared_secret_app);
    nrf_crypto_ecdh_compute(NULL,
                            &alice_private_key,
                            &bob_public_key,
                            m_shared_secret_app,
                            &size_app);

    NRF_LOG_INFO("Alice 私鑰 32 byte:")
    NRF_LOG_HEXDUMP_INFO(alice_private_key_raw, sizeof(alice_private_key_raw));

    NRF_LOG_INFO("Alice 公鑰 64 byte:")
    NRF_LOG_HEXDUMP_INFO(alice_public_key_raw, sizeof(alice_public_key_raw));

    NRF_LOG_INFO("Alice 算出共同密鑰 32 bytes:")
    NRF_LOG_HEXDUMP_INFO(m_shared_secret_app, sizeof(m_shared_secret_app));

    nrf_crypto_ecc_public_key_free(&alice_public_key);
    nrf_crypto_ecc_public_key_free(&alice_public_key);
//    nrf_crypto_ecc_private_key_free(&bob_private_key);
//    nrf_crypto_ecc_private_key_free(&bob_private_key);
    NRF_LOG_INFO("<====>")
}

int main(void)
{
    NRF_LOG_INIT(NULL);
    NRF_LOG_DEFAULT_BACKENDS_INIT();
    nrf_mem_init();
    nrf_crypto_init();
    test_ecdh();

    for (;;)
    {
    }
}