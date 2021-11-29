#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_sdh_soc.h"
#include "nrf_pwr_mgmt.h"
#include "app_timer.h"
#include "boards.h"
#include "bsp.h"
#include "bsp_btn_ble.h"
#include "ble.h"
#include "ble_hci.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "ble_db_discovery.h"
#include "ble_sesame_c.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_scan.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#define APP_BLE_CONN_CFG_TAG            1                       /**< A tag identifying the SoftDevice BLE configuration. */

NRF_BLE_SCAN_DEF(m_scan);                                       /**< Scanning module instance. */

//開一個全域變數，並打開一個同名的監聽，這個監聽可以收到跟其他監聽相同的資料
//不過這個監聽專門處理藍牙中心設備的相關事情
BLE_LBS_C_DEF(m_ble_lbs_c);                                     /**< Main structure used by the LBS client module. */

NRF_BLE_GATT_DEF(m_gatt);                                       /**< GATT module instance. */
//BLE_DB_DISCOVERY_DEF(m_db_disc);                                /**< DB discovery module instance. */
NRF_BLE_GQ_DEF(m_ble_gatt_queue,                                /**< BLE GATT Queue instance. */
               NRF_SDH_BLE_CENTRAL_LINK_COUNT,
               NRF_BLE_GQ_QUEUE_SIZE);

static char const m_target_periph_name[] = "Nordic_Blinky";     /**< Name of the device we try to connect to. This name is searched in the scan report data*/

void assert_nrf_callback(uint16_t line_num, const uint8_t *p_file_name) {
    app_error_handler(0xDEADBEEF, line_num, p_file_name);
}

static void lbs_error_handler(uint32_t nrf_error) {
    APP_ERROR_HANDLER(nrf_error);
}

static void scan_start(void) {
    APP_ERROR_CHECK(nrf_ble_scan_start(&m_scan));
}

//不知道ble_lbs_c_evt_t這個結構是幹嘛的
//只知道透過他可以拿到外圍設備送來的資料
static void lbs_c_evt_handler(ble_lbs_c_t *p_lbs_c, ble_lbs_c_evt_t *p_lbs_c_evt) {
    switch (p_lbs_c_evt->evt_type) {
//        case BLE_LBS_C_EVT_DISCOVERY_COMPLETE: {
//            ret_code_t err_code;
//            NRF_LOG_INFO("[%x][c][discovery]<-", p_lbs_c_evt->conn_handle);
//
//            err_code = ble_lbs_c_handles_assign(&m_ble_lbs_c,
//                                                p_lbs_c_evt->conn_handle,
//                                                &p_lbs_c_evt->params.peer_db);
//
//            APP_ERROR_CHECK(err_code);
//
//            err_code = ble_lbs_c_button_notif_enable(p_lbs_c);
//            APP_ERROR_CHECK(err_code);
//        }
//            break; // BLE_LBS_C_EVT_DISCOVERY_COMPLETE

        case BLE_LBS_C_EVT_BUTTON_NOTIFICATION: {
            NRF_LOG_INFO("[收][%x]", p_lbs_c_evt->params.button.button_state);
        }
            break; // BLE_LBS_C_EVT_BUTTON_NOTIFICATION

        default:
            // No implementation needed.
            break;
    }
}

//監聽到一些事件要做什麼
static void ble_evt_handler(ble_evt_t const *p_ble_evt, void *p_context) {
    ret_code_t err_code;
    ble_gap_evt_t const *p_gap_evt = &p_ble_evt->evt.gap_evt;
    switch (p_ble_evt->header.evt_id) {
        case BLE_GAP_EVT_CONNECTED: {
            NRF_LOG_INFO("[%x][a][connect]", p_gap_evt->conn_handle);   //p_gap_evt->conn_handle：連線設備的handle編號
            NRF_LOG_INFO("[%x][b][discovery]->", p_gap_evt->conn_handle);

            //m_ble_lbs_c變數在最一開始就透過define就建立了，並且同時也開好了同名的監聽
            ble_lbs_c_handles_assign(&m_ble_lbs_c, p_gap_evt->conn_handle, NULL);
            err_code = ble_lbs_c_button_notif_enable(&m_ble_lbs_c);

            APP_ERROR_CHECK(err_code);
        }
            break;
        case BLE_GAP_EVT_DISCONNECTED: {
            NRF_LOG_INFO("Disconnected.");
            scan_start();
        }
            break;

        case BLE_GAP_EVT_TIMEOUT: {
            if (p_gap_evt->params.timeout.src == BLE_GAP_TIMEOUT_SRC_CONN) {
                NRF_LOG_DEBUG("Connection request timed out.");
            }
        }
            break;

        case BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST: {
            err_code = sd_ble_gap_conn_param_update(p_gap_evt->conn_handle,
                                                    &p_gap_evt->params.conn_param_update_request.conn_params);
            APP_ERROR_CHECK(err_code);
        }
            break;

        case BLE_GAP_EVT_PHY_UPDATE_REQUEST: {
            NRF_LOG_DEBUG("PHY update request.");
            ble_gap_phys_t const phys =
                    {
                            .rx_phys = BLE_GAP_PHY_AUTO,
                            .tx_phys = BLE_GAP_PHY_AUTO,
                    };
            err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
            APP_ERROR_CHECK(err_code);
        }
            break;

        case BLE_GATTC_EVT_TIMEOUT: {
            NRF_LOG_DEBUG("GATT Client Timeout.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
        }
            break;

        case BLE_GATTS_EVT_TIMEOUT: {
            NRF_LOG_DEBUG("GATT Server Timeout.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
        }
            break;

        default:
            // No implementation needed.
            break;
    }
}

//先在lbs_c_init_obj設定好參數，再透過ble_lbs_c_init把設定好的資料放進m_ble_lbs_c
//m_ble_lbs_c變數在最一開始就透過define就建立了，並且同時也開好了同名的監聽
static void lbs_c_init(void) {
    ret_code_t err_code;
    ble_lbs_c_init_t lbs_c_init_obj;
    lbs_c_init_obj.evt_handler = lbs_c_evt_handler;
    lbs_c_init_obj.p_gatt_queue = &m_ble_gatt_queue;
    lbs_c_init_obj.error_handler = lbs_error_handler;
    err_code = ble_lbs_c_init(&m_ble_lbs_c, &lbs_c_init_obj);
    APP_ERROR_CHECK(err_code);
}

static void ble_stack_init(void) {
    ret_code_t err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);
    NRF_SDH_BLE_OBSERVER(m_ble_observer, 3, ble_evt_handler, NULL); //開啟藍牙監聽
}

static void button_event_handler(uint8_t pin_no, uint8_t button_action) {
    NRF_LOG_INFO("[發][%d]", button_action);
    NRF_LOG_INFO("m_ble_lbs_c.conn_handle:%x", m_ble_lbs_c.conn_handle);    //不知道是從啥時拿到這個handle編號
    ret_code_t err_code = ble_lbs_led_status_send(&m_ble_lbs_c, button_action);
    APP_ERROR_CHECK(err_code);
}

static void scan_evt_handler(scan_evt_t const *p_scan_evt) {
    ret_code_t err_code;
    switch (p_scan_evt->scan_evt_id) {
        case NRF_BLE_SCAN_EVT_CONNECTING_ERROR:
            err_code = p_scan_evt->params.connecting_err.err_code;
            APP_ERROR_CHECK(err_code);
            break;
        default:
            break;
    }
}

static void buttons_init(void) {
    static app_button_cfg_t buttons[] = {{BSP_BUTTON_0, false, BUTTON_PULL, button_event_handler}};
    ret_code_t err_code = app_button_init(buttons, ARRAY_SIZE(buttons), APP_TIMER_TICKS(50));
    APP_ERROR_CHECK(err_code);
}

//static void db_disc_handler(ble_db_discovery_evt_t *p_evt) {
//    ble_lbs_on_db_disc_evt(&m_ble_lbs_c, p_evt);
//}

//static void db_discovery_init(void) {
//    ble_db_discovery_init_t db_init = {0};
//    db_init.evt_handler = db_disc_handler;
//    db_init.p_gatt_queue = &m_ble_gatt_queue;
//    ret_code_t err_code = ble_db_discovery_init(&db_init);
//    APP_ERROR_CHECK(err_code);
//}

static void scan_init(void) {
    ret_code_t err_code;
    nrf_ble_scan_init_t init_scan = {0};
    init_scan.connect_if_match = true;
    init_scan.conn_cfg_tag = APP_BLE_CONN_CFG_TAG;
    err_code = nrf_ble_scan_init(&m_scan, &init_scan, scan_evt_handler);
    APP_ERROR_CHECK(err_code);
    err_code = nrf_ble_scan_filters_enable(&m_scan, NRF_BLE_SCAN_NAME_FILTER, false);
    APP_ERROR_CHECK(err_code);
    err_code = nrf_ble_scan_filter_set(&m_scan, SCAN_NAME_FILTER, m_target_periph_name);
    APP_ERROR_CHECK(err_code);
}

int main(void) {

    NRF_LOG_INIT(NULL);
    NRF_LOG_DEFAULT_BACKENDS_INIT();// log_init();
    APP_ERROR_CHECK(app_timer_init());//timer_init()
    buttons_init();
    app_button_enable();
    APP_ERROR_CHECK(nrf_pwr_mgmt_init());
    ble_stack_init();
    scan_init();
    APP_ERROR_CHECK(nrf_ble_gatt_init(&m_gatt, NULL)); //gatt_init();
//    db_discovery_init();
    lbs_c_init();
    scan_start();
    NRF_LOG_INFO("[APP] =====> ")

//    for(;;) = while(1)
    for (;;) {
        NRF_LOG_FLUSH();// idle_state_handle();
        nrf_pwr_mgmt_run();
    }
}