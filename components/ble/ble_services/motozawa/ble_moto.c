#include "sdk_common.h"
#if NRF_MODULE_ENABLED(BLE_LBS)
#include "ble_moto.h"
#include "ble_srv_common.h"


#include "nrf_log.h"


/**@brief Function for handling the Write event.
 *
 * @param[in] p_lbs      LED Button Service structure.  全域變數
 * @param[in] p_ble_evt  Event received from the BLE stack.  監聽
 */
static void on_write(ble_lbs_t * p_lbs, ble_evt_t const * p_ble_evt)
{
    NRF_LOG_INFO("%s\n", __func__)
//    p_lbs：全域變數
//    p_ble_evt：監聽
//    p_evt_write：監聽抄出來的手機寫入內容

    ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

//    NRF_LOG_INFO("[%x][mob][say]", p_ble_evt->evt.gap_evt.conn_handle)
//    NRF_LOG_INFO("p_evt_write->op:%d", p_evt_write->op)
    NRF_LOG_INFO("button_char_handles.cccd_handle:%d", p_lbs->button_char_handles.cccd_handle)
    NRF_LOG_INFO("button_char_handles.value_handle:%d", p_lbs->button_char_handles.value_handle)
    NRF_LOG_INFO("led_char_handles.value_handle:%d", p_lbs->led_char_handles.value_handle)

    NRF_LOG_INFO("p_evt_write->handle:%d", p_evt_write->handle)

    NRF_LOG_HEXDUMP_INFO(p_evt_write->data, p_evt_write->len);

    p_lbs->led_write_handler(p_ble_evt->evt.gap_evt.conn_handle, p_lbs, p_evt_write->data[0]);

//    conn_handle：多裝置連線時透過這個辨識是哪個裝置
//    p_lbs：紀錄很多東西的全域變數
//    led_state：透過這個控制LED亮暗
//    led_write_handler(conn_handle, p_lbs, led_state)

}

//p_ble_evt：監聽（內涵監測到的各個事件＆哪個裝置連線）
//p_context：全域變數（bls服務參數＆開關燈函數）
void ble_lbs_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context)
{
    ble_lbs_t * p_lbs = (ble_lbs_t *)p_context;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GATTS_EVT_WRITE:
            NRF_LOG_INFO("%s -> BLE_GATTS_EVT_WRITE \n", __func__)

            on_write(p_lbs, p_ble_evt);
            break;

        default:
            // No implementation needed.
            break;
    }
}

  //看來要開一個服務裡面要設定他的uuid
 //還要設定各個小服務的特徵，並用characteristic_add新增，
//新增同時會為服務開窗口(用編號區分，handle，softdevice自動分配)，之後手機會往該分配編號窗口寫入資料
uint32_t ble_lbs_init(ble_lbs_t * p_lbs, const ble_lbs_init_t * p_lbs_init)
{
    NRF_LOG_INFO("%s\n", __func__)

    uint32_t              err_code;
    ble_uuid_t            ble_uuid;
    ble_add_char_params_t add_char_params;

    // Initialize service structure.
    p_lbs->led_write_handler = p_lbs_init->led_write_handler;

    // Add service.
    // 加入服務
    ble_uuid128_t base_uuid = {LBS_UUID_BASE};
    err_code = sd_ble_uuid_vs_add(&base_uuid, &p_lbs->uuid_type);
    VERIFY_SUCCESS(err_code);

//    ble_uuid.type = p_lbs->uuid_type;
    ble_uuid.type = BLE_UUID_TYPE_BLE;
//    ble_uuid.type = BLE_UUID_TYPE_VENDOR_BEGIN;
//    ble_uuid.uuid = LBS_UUID_SERVICE; //1527
    ble_uuid.uuid = 0x180A; //1527

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_lbs->service_handle);
    VERIFY_SUCCESS(err_code);

    // Add Button characteristic.
    // 填入按鈕服務所需資訊
    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid              = LBS_UUID_BUTTON_CHAR;
    add_char_params.uuid_type         = p_lbs->uuid_type;
    add_char_params.init_len          = sizeof(uint8_t);
    add_char_params.max_len           = sizeof(uint8_t);
    add_char_params.char_props.read   = 1;
    add_char_params.char_props.notify = 1;

    add_char_params.read_access       = SEC_OPEN;
    add_char_params.cccd_write_access = SEC_OPEN;

    //新增按鈕服務
    err_code = characteristic_add(p_lbs->service_handle,
                                  &add_char_params,
                                  &p_lbs->button_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Add LED characteristic.
    // 填入LED服務所需資訊
    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid             = LBS_UUID_LED_CHAR;
    add_char_params.uuid_type        = p_lbs->uuid_type;
    add_char_params.init_len         = sizeof(uint8_t);
    add_char_params.max_len          = sizeof(uint8_t);
    add_char_params.char_props.read  = 1;
    add_char_params.char_props.write = 1;
    add_char_params.char_props.notify = 1;


    add_char_params.read_access  = SEC_OPEN;
    add_char_params.write_access = SEC_OPEN;
    add_char_params.cccd_write_access = SEC_OPEN;

    //開啟LED服務
    return characteristic_add(p_lbs->service_handle, &add_char_params, &p_lbs->led_char_handles);
}


//struct ble_lbs_s
//{
//    uint16_t                    service_handle;      /**< Handle of LED Button Service (as provided by the BLE stack). */
//    ble_gatts_char_handles_t    led_char_handles;    /**< Handles related to the LED Characteristic. */
//    ble_gatts_char_handles_t    button_char_handles; /**< Handles related to the Button Characteristic. */
//    uint8_t                     uuid_type;           /**< UUID type for the LED Button Service. */
//    ble_lbs_led_write_handler_t led_write_handler;   /**< Event handler to be called when the LED Characteristic is written. */
//};
uint32_t ble_lbs_on_button_change(uint16_t conn_handle, ble_lbs_t * p_lbs, uint8_t button_state)
{
    //conn_handle：決定封包要送給哪個裝置
    //p_lbs：全域變數，上面的註釋有說裡面放什麼
    NRF_LOG_INFO("%s\n", __func__)

    ble_gatts_hvx_params_t params; //開發板按下按鍵後要送出的資料
    uint16_t len = sizeof(button_state);

    memset(&params, 0, sizeof(params));
    params.type   = BLE_GATT_HVX_NOTIFICATION;
    params.handle = p_lbs->button_char_handles.value_handle; //按鈕服務handle編號
    params.p_data = &button_state; //按鍵狀態
    params.p_len  = &len; //要送出訊息的長度

    return sd_ble_gatts_hvx(conn_handle, &params);
}
#endif // NRF_MODULE_ENABLED(BLE_LBS)
