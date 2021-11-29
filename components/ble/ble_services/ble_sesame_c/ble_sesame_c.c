#include "sdk_common.h"

#if NRF_MODULE_ENABLED(BLE_LBS_C)

#include "ble_sesame_c.h"
#include "ble_db_discovery.h"
#include "ble_types.h"
#include "ble_gattc.h"

#define NRF_LOG_MODULE_NAME ble_sesame_c

#include "nrf_log.h"

NRF_LOG_MODULE_REGISTER();

static void gatt_error_handler(uint32_t nrf_error,
                               void *p_ctx,
                               uint16_t conn_handle) {
    ble_lbs_c_t *p_ble_lbs_c = (ble_lbs_c_t *) p_ctx;
    NRF_LOG_INFO("A GATT Client error has occurred on conn_handle: 0X%X", conn_handle);
    if (p_ble_lbs_c->error_handler != NULL) {
        p_ble_lbs_c->error_handler(nrf_error);
    }
}

//跟blinky的on_write有點像
static void on_hvx(ble_lbs_c_t *p_ble_lbs_c, ble_evt_t const *p_ble_evt) {
    // Check if the event is on the link for this instance.
    if (p_ble_lbs_c->conn_handle != p_ble_evt->evt.gattc_evt.conn_handle) {
        return;
    }
    // Check if this is a Button notification.
    NRF_LOG_INFO("[收][%d]",p_ble_evt->evt.gattc_evt.params.hvx.data[0])
    ble_lbs_c_evt_t ble_lbs_c_evt;
    ble_lbs_c_evt.evt_type = BLE_LBS_C_EVT_BUTTON_NOTIFICATION;
    ble_lbs_c_evt.conn_handle = p_ble_lbs_c->conn_handle;
    ble_lbs_c_evt.params.button.button_state = p_ble_evt->evt.gattc_evt.params.hvx.data[0];
    p_ble_lbs_c->evt_handler(p_ble_lbs_c, &ble_lbs_c_evt);  //不知道這邊的函數是從哪放進去的
}


static void on_disconnected(ble_lbs_c_t *p_ble_lbs_c, ble_evt_t const *p_ble_evt) {
    if (p_ble_lbs_c->conn_handle == p_ble_evt->evt.gap_evt.conn_handle) {
        p_ble_lbs_c->conn_handle = BLE_CONN_HANDLE_INVALID;
        p_ble_lbs_c->peer_lbs_db.button_cccd_handle = BLE_GATT_HANDLE_INVALID;
        p_ble_lbs_c->peer_lbs_db.button_handle = BLE_GATT_HANDLE_INVALID;
        p_ble_lbs_c->peer_lbs_db.led_handle = BLE_GATT_HANDLE_INVALID;
    }
}


//void ble_lbs_on_db_disc_evt(ble_lbs_c_t *p_ble_lbs_c, ble_db_discovery_evt_t const *p_evt) {
//    // Check if the LED Button Service was discovered.
//    if (p_evt->evt_type == BLE_DB_DISCOVERY_COMPLETE &&
//        p_evt->params.discovered_db.srv_uuid.uuid == LBS_UUID_SERVICE &&
//        p_evt->params.discovered_db.srv_uuid.type == p_ble_lbs_c->uuid_type) {
//        ble_lbs_c_evt_t evt;
//        NRF_LOG_INFO("[BLE_DB_DISCOVERY_COMPLETE]")
//
//        evt.evt_type = BLE_LBS_C_EVT_DISCOVERY_COMPLETE;
//        evt.conn_handle = p_evt->conn_handle;
//        for (uint32_t i = 0; i < p_evt->params.discovered_db.char_count; i++) {
//            const ble_gatt_db_char_t *p_char = &(p_evt->params.discovered_db.charateristics[i]);
//            switch (p_char->characteristic.uuid.uuid) {
//                case LBS_UUID_LED_CHAR:
//                    evt.params.peer_db.led_handle = p_char->characteristic.handle_value;
//                    NRF_LOG_INFO("evt.params.peer_db.led_handle :%d", evt.params.peer_db.led_handle)
//                    break;
//                case LBS_UUID_BUTTON_CHAR:
//                    evt.params.peer_db.button_handle = p_char->characteristic.handle_value;
//                    evt.params.peer_db.button_cccd_handle = p_char->cccd_handle;
//                    NRF_LOG_INFO("evt.params.peer_db.button_handle :%d", evt.params.peer_db.button_handle)
//                    NRF_LOG_INFO("evt.params.peer_db.button_cccd_handle :%d", evt.params.peer_db.button_cccd_handle)
//                    break;
//
//                default:
//                    break;
//            }
//        }
//
////        NRF_LOG_INFO("LED Button Service discovered at peer.");
//        //If the instance was assigned prior to db_discovery, assign the db_handles
//        if (p_ble_lbs_c->conn_handle != BLE_CONN_HANDLE_INVALID) {
//            if ((p_ble_lbs_c->peer_lbs_db.led_handle == BLE_GATT_HANDLE_INVALID) &&
//                (p_ble_lbs_c->peer_lbs_db.button_handle == BLE_GATT_HANDLE_INVALID) &&
//                (p_ble_lbs_c->peer_lbs_db.button_cccd_handle == BLE_GATT_HANDLE_INVALID)) {
//                p_ble_lbs_c->peer_lbs_db = evt.params.peer_db;//這裡存下所有特徵值句柄
//            }
//        }
//
//        p_ble_lbs_c->evt_handler(p_ble_lbs_c, &evt);
//
//    }
//}


uint32_t ble_lbs_c_init(ble_lbs_c_t *p_ble_lbs_c, ble_lbs_c_init_t *p_ble_lbs_c_init) {
    uint32_t err_code;

    ble_uuid128_t lbs_base_uuid = {LBS_UUID_BASE};  //LBS的UUID

//    p_ble_lbs_c->peer_lbs_db.button_cccd_handle = 14;
//    p_ble_lbs_c->peer_lbs_db.button_handle = 800;
//    p_ble_lbs_c->peer_lbs_db.led_handle = 600;

    //這個不知道是指自己的handle還是外圍設備的handle編號
    //我猜是自己設備的handle編號，然後我發現button_cccd_handle，似乎要跟外圍設備一樣，其他兩個可以亂設
    //經過觀察發現外圍設備的cccd handle是sd自動分配的，但固定都分到14
    p_ble_lbs_c->peer_lbs_db.button_cccd_handle = 14;
    p_ble_lbs_c->peer_lbs_db.button_handle = 13;
    p_ble_lbs_c->peer_lbs_db.led_handle = 16;

    p_ble_lbs_c->conn_handle = BLE_CONN_HANDLE_INVALID;
    p_ble_lbs_c->evt_handler = p_ble_lbs_c_init->evt_handler;
    p_ble_lbs_c->p_gatt_queue = p_ble_lbs_c_init->p_gatt_queue;
    p_ble_lbs_c->error_handler = p_ble_lbs_c_init->error_handler;


    //新增gatt服務
    //看來東西設定好後用這個打開，之後就會動了
    //看來要開gatt服務需要設定文的參數，還有這個服務的UUID
    err_code = sd_ble_uuid_vs_add(&lbs_base_uuid, &p_ble_lbs_c->uuid_type);
    if (err_code != NRF_SUCCESS) {
        return err_code;
    }

    VERIFY_SUCCESS(err_code);
    return err_code;
//    ble_uuid_t lbs_uuid;
//    lbs_uuid.type = p_ble_lbs_c->uuid_type;
//    lbs_uuid.uuid = LBS_UUID_SERVICE;
//    ble_db_discovery_evt_register(&lbs_uuid);
}

void ble_lbs_c_on_ble_evt(ble_evt_t const *p_ble_evt, void *p_context) {
    if ((p_context == NULL) || (p_ble_evt == NULL)) {
        return;
    }

    ble_lbs_c_t *p_ble_lbs_c = (ble_lbs_c_t *) p_context;

    switch (p_ble_evt->header.evt_id) {
        case BLE_GATTC_EVT_HVX: //外圍設備按按鈕
            on_hvx(p_ble_lbs_c, p_ble_evt);
            break;

        case BLE_GAP_EVT_DISCONNECTED:  //斷線
            on_disconnected(p_ble_lbs_c, p_ble_evt);    //把之前設定的handle編號都清空
            break;

        default:
            break;
    }
}

static uint32_t cccd_configure(ble_lbs_c_t *p_ble_lbs_c, bool enable) {
    NRF_LOG_INFO("Configuring CCCD. CCCD Handle = %d, Connection Handle = %d",
                 p_ble_lbs_c->peer_lbs_db.button_cccd_handle,
                 p_ble_lbs_c->conn_handle);

    nrf_ble_gq_req_t cccd_req;
    uint16_t cccd_val = enable ? BLE_GATT_HVX_NOTIFICATION : 0;
    uint8_t cccd[BLE_CCCD_VALUE_LEN];

    cccd[0] = LSB_16(cccd_val);
    cccd[1] = MSB_16(cccd_val);

    cccd_req.type = NRF_BLE_GQ_REQ_GATTC_WRITE;
    cccd_req.error_handler.cb = gatt_error_handler;
    cccd_req.error_handler.p_ctx = p_ble_lbs_c;
    cccd_req.params.gattc_write.handle = p_ble_lbs_c->peer_lbs_db.button_cccd_handle;
    cccd_req.params.gattc_write.len = BLE_CCCD_VALUE_LEN;
    cccd_req.params.gattc_write.offset = 0;
    cccd_req.params.gattc_write.p_value = cccd;
    cccd_req.params.gattc_write.write_op = BLE_GATT_OP_WRITE_REQ;

    return nrf_ble_gq_item_add(p_ble_lbs_c->p_gatt_queue, &cccd_req, p_ble_lbs_c->conn_handle);
}


uint32_t ble_lbs_c_button_notif_enable(ble_lbs_c_t *p_ble_lbs_c) {

    if (p_ble_lbs_c->conn_handle == BLE_CONN_HANDLE_INVALID) {
        return NRF_ERROR_INVALID_STATE;
    }

    return cccd_configure(p_ble_lbs_c, true);
}

//發送訊號到外圍設備
uint32_t ble_lbs_led_status_send(ble_lbs_c_t *p_ble_lbs_c, uint8_t status) {
    VERIFY_PARAM_NOT_NULL(p_ble_lbs_c);

    if (p_ble_lbs_c->conn_handle == BLE_CONN_HANDLE_INVALID) {
        return NRF_ERROR_INVALID_STATE;
    }


    nrf_ble_gq_req_t write_req;

    memset(&write_req, 0, sizeof(nrf_ble_gq_req_t));

    write_req.type = NRF_BLE_GQ_REQ_GATTC_WRITE;
    write_req.error_handler.cb = gatt_error_handler;
    write_req.error_handler.p_ctx = p_ble_lbs_c;
    write_req.params.gattc_write.handle = p_ble_lbs_c->peer_lbs_db.led_handle;
    write_req.params.gattc_write.len = sizeof(status);
    write_req.params.gattc_write.p_value = &status;
    write_req.params.gattc_write.offset = 0;
    write_req.params.gattc_write.write_op = BLE_GATT_OP_WRITE_CMD;

    return nrf_ble_gq_item_add(p_ble_lbs_c->p_gatt_queue, &write_req, p_ble_lbs_c->conn_handle);
}

uint32_t ble_lbs_c_handles_assign(ble_lbs_c_t *p_ble_lbs_c,
                                  uint16_t conn_handle,
                                  const lbs_db_t *p_peer_handles) {

    p_ble_lbs_c->conn_handle = conn_handle;
    if (p_peer_handles != NULL) {
        p_ble_lbs_c->peer_lbs_db = *p_peer_handles;
    }
    return nrf_ble_gq_conn_handle_register(p_ble_lbs_c->p_gatt_queue, conn_handle);
}

#endif // NRF_MODULE_ENABLED(BLE_LBS_C)
