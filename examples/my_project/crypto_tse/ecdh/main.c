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

static void test_ecdh() {
//
//    NRF_LOG_INFO("===ECDH===")
//    NRF_LOG_INFO("[目標] 第一次見面  Alice(鎖) <->Bob(手機)  之間溝通希望不被別人知道")
//    NRF_LOG_INFO("[ECDH] 被監聽的過程中，雙方可以擁有共同的密鑰")
//    NRF_LOG_INFO("[簡化] 如果雙方可以有一個共同的 密鑰，那彼此溝通就不會被人知道")
//    NRF_LOG_INFO("[簡化] 如果雙方可以有一個共同的 密碼日記，那彼此溝通就不會被人知道")
//    NRF_LOG_INFO("[複雜] 用共通密鑰加密 傳訊息不被別人知道")
//
//    NRF_LOG_INFO("[1]  ECDH_secp256r1")
//    NRF_LOG_INFO("[2.10] Alice 利用 ECDH演算法 產生 [Private-Key(32bytes)]")
//    NRF_LOG_INFO("[2.11] Alice 利用 ECDH_secp256r1(Private-Key) --> [Public-Key]")
//    NRF_LOG_INFO("[2.13] [ex]  利用 ECDH_secp256r1  (ㄅㄆㄇ)  一定算出 [ㄍㄎㄏ]")
//
//    NRF_LOG_INFO("[2.20] Bob  利用 ECDH演算法  產生 [Public Key(64bytes)][Private Key(32bytes)]")
//    NRF_LOG_INFO("[2.21] [ex]  利用 ECDH_secp256r1  (甲乙丙)  一定算出 [戊己庚]")
//
//    NRF_LOG_INFO("[3.1] Alice 給 Bob [Public Key Alice]")
//    NRF_LOG_INFO("[3.2] Bob   給 Alice [Public Key Bob]")
//    NRF_LOG_INFO("[結論]交換公鑰")
//
//    NRF_LOG_INFO("[4.2] Alice ECDH 算出共同密鑰(Private_Key_Alice[ㄅㄆㄇ]) , Public_Key_Bob[戊己庚])  =  {￥タ￥タ}")
//    NRF_LOG_INFO("[4.2] Bob   ECDH 算出共同密鑰(Private_Key_Bob　[甲乙丙]  ,Public_Key_Alice[ㄍㄎㄏ]) =  {￥タ￥タ}")
//    NRF_LOG_INFO("[4.2] 兩邊算出來一樣{￥タ￥タ}。共同秘密鑰就產生了，而且此密鑰不曾在空氣中傳播，沒有洩漏的可能")

//    // aes-ccm 之後教
//    NRF_LOG_INFO("[ex]  加密aes-ccm({￥タ￥タ},你好)--->  ASSS")
//    NRF_LOG_INFO("[ex]  解密aes-ccm({￥タ￥タ},ASSS)--->  你好")


    //Alice的Key們
    static nrf_crypto_ecc_private_key_t app_private_key;
    static nrf_crypto_ecc_public_key_t app_public_key;
    static nrf_crypto_ecc_secp256r1_raw_private_key_t app_private_key_raw;
    static nrf_crypto_ecc_secp256r1_raw_public_key_t app_public_key_raw;
    //Alice產生公私鑰
    nrf_crypto_ecc_key_pair_generate(NULL,
                                     &g_nrf_crypto_ecc_secp256r1_curve_info,
                                     &app_private_key,
                                     &app_public_key);

    //Bob的Key們
    static nrf_crypto_ecc_private_key_t ssm_private_key;
    static nrf_crypto_ecc_public_key_t ssm_public_key;
    static nrf_crypto_ecc_secp256r1_raw_private_key_t ssm_private_key_raw;
    static nrf_crypto_ecc_secp256r1_raw_public_key_t ssm_public_key_raw;
    //Bob產生公私鑰
    nrf_crypto_ecc_key_pair_generate(NULL,
                                     &g_nrf_crypto_ecc_secp256r1_curve_info,
                                     &ssm_private_key,
                                     &ssm_public_key);

    static nrf_crypto_ecdh_secp256r1_shared_secret_t m_shared_secret_app;   //Alice算出的共同密鑰
    static nrf_crypto_ecdh_secp256r1_shared_secret_t m_shared_secret_ssm;   //Bob算出的共同密鑰

    //各變數的大小
    size_t size_ssm = sizeof(m_shared_secret_ssm);
    size_t size_app = sizeof(m_shared_secret_app);
    size_t size_ssm_pb = sizeof(ssm_public_key_raw);
    size_t size_ssm_pr = sizeof(ssm_private_key_raw);
    size_t size_app_pb = sizeof(app_public_key_raw);
    size_t size_app_pr = sizeof(app_private_key_raw);

    //把各個內部處理的格式轉成陣列
    //Alice
    nrf_crypto_ecc_public_key_to_raw(
            &app_public_key,
            app_public_key_raw,
            &size_app_pb);

    nrf_crypto_ecc_private_key_to_raw(
            &app_private_key,
            app_private_key_raw,
            &size_app_pr);
    //Bob
    nrf_crypto_ecc_public_key_to_raw(
            &ssm_public_key,
            ssm_public_key_raw,
            &size_ssm_pb);

    nrf_crypto_ecc_private_key_to_raw(
            &ssm_private_key,
            ssm_private_key_raw,
            &size_ssm_pr);

    //Alice用自己的private key + Bob的public key 算出共同密鑰
    nrf_crypto_ecdh_compute(NULL,
                            &app_private_key,
                            &ssm_public_key,
                            m_shared_secret_app,
                            &size_app);

    //Bob用自己的private key + Alice的public key 算出共同密鑰
    nrf_crypto_ecdh_compute(NULL,
                            &ssm_private_key,
                            &app_public_key,
                            m_shared_secret_ssm,
                            &size_ssm);

    NRF_LOG_INFO("Alice 私鑰 32 byte:")
    NRF_LOG_HEXDUMP_INFO(app_private_key_raw, sizeof(app_private_key_raw));

    NRF_LOG_INFO("Alice 公鑰 64 byte:")
    NRF_LOG_HEXDUMP_INFO(app_public_key_raw, sizeof(app_public_key_raw));

    NRF_LOG_INFO("BOB 私鑰 32 byte:")
    NRF_LOG_HEXDUMP_INFO(ssm_private_key_raw, sizeof(ssm_private_key_raw));

    NRF_LOG_INFO("BOB 公鑰 64 byte:")
    NRF_LOG_HEXDUMP_INFO(ssm_public_key_raw, sizeof(ssm_public_key_raw));

    NRF_LOG_INFO("Alice 算出共同密鑰 32 bytes:")
    NRF_LOG_HEXDUMP_INFO(m_shared_secret_ssm, sizeof(m_shared_secret_ssm));

    NRF_LOG_INFO("BOB 算出共同密鑰 32 bytes:")
    NRF_LOG_HEXDUMP_INFO(m_shared_secret_app, sizeof(m_shared_secret_app));


    nrf_crypto_ecc_private_key_free(&ssm_private_key);
    nrf_crypto_ecc_private_key_free(&ssm_private_key);
    nrf_crypto_ecc_public_key_free(&app_public_key);
    nrf_crypto_ecc_public_key_free(&app_public_key);
    NRF_LOG_INFO("<====>")
}

int main(void) {

    NRF_LOG_INIT(NULL);
    NRF_LOG_DEFAULT_BACKENDS_INIT();
    nrf_mem_init();
    nrf_crypto_init();
    test_ecdh();

    for (;;) {
    }
}


/** @}
 */
