#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "ble.h"
#include "ble_err.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_conn_params.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "boards.h"
#include "app_timer.h"
#include "app_button.h"
#include "ble_moto.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"
#include "nrf_pwr_mgmt.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"


#define ADVERTISING_LED                 BSP_BOARD_LED_0                         /**< Is on when device is advertising. */
#define CONNECTED_LED                   BSP_BOARD_LED_1                         /**< Is on when device has connected. */
#define LEDBUTTON_LED                   BSP_BOARD_LED_2                         /**< LED to be toggled with the help of the LED Button Service. */
#define LEDBUTTON_BUTTON                BSP_BUTTON_0                            /**< Button that will trigger the notification event with the LED Button Service */

#define DEVICE_NAME                     "Nordic_Blinky"                         /**< Name of device. Will be included in the advertising data. */

#define APP_BLE_OBSERVER_PRIO           3                                       /**< Application's BLE observer priority. You shouldn't need to modify this value. */
#define APP_BLE_CONN_CFG_TAG            1                                       /**< A tag identifying the SoftDevice BLE configuration. */

#define APP_ADV_INTERVAL                64                                      /**< The advertising interval (in units of 0.625 ms; this value corresponds to 40 ms). */
#define APP_ADV_DURATION                BLE_GAP_ADV_TIMEOUT_GENERAL_UNLIMITED   /**< The advertising time-out (in units of seconds). When set to 0, we will never time out. */


#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(100, UNIT_1_25_MS)        /**< Minimum acceptable connection interval (0.5 seconds). */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(200, UNIT_1_25_MS)        /**< Maximum acceptable connection interval (1 second). */
#define SLAVE_LATENCY                   0                                       /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)         /**< Connection supervisory time-out (4 seconds). */

#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(20000)                  /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (15 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(5000)                   /**< Time between each call to sd_ble_gap_conn_param_update after the first call (5 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                       /**< Number of attempts before giving up the connection parameter negotiation. */

#define BUTTON_DETECTION_DELAY          APP_TIMER_TICKS(50)                     /**< Delay from a GPIOTE event until a button is reported as pushed (in number of timer ticks). */

#define DEAD_BEEF                       0xDEADBEEF                              /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

//for多機連線
#define LINK_TOTAL                      NRF_SDH_BLE_PERIPHERAL_LINK_COUNT + \
                                        NRF_SDH_BLE_CENTRAL_LINK_COUNT

BLE_LBS_DEF(m_lbs);                                                             /**< LED Button Service instance. */
NRF_BLE_GATT_DEF(m_gatt);                                                       /**< GATT module instance. */
NRF_BLE_QWR_DEF(m_qwr);                                                         /**< Context for the Queued Write module.*/

static uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID;                        /**< Handle of the current connection. */

static uint8_t m_adv_handle = BLE_GAP_ADV_SET_HANDLE_NOT_SET;                   /**< Advertising handle used to identify an advertising set. */
static uint8_t m_enc_advdata[BLE_GAP_ADV_SET_DATA_SIZE_MAX];                    /**< Buffer for storing an encoded advertising set. */
static uint8_t m_enc_scan_response_data[BLE_GAP_ADV_SET_DATA_SIZE_MAX];         /**< Buffer for storing an encoded scan data. */

static ble_gap_adv_data_t m_adv_data =
{
    .adv_data =
    {
        .p_data = m_enc_advdata,
        .len    = BLE_GAP_ADV_SET_DATA_SIZE_MAX
    },
    .scan_rsp_data =
    {
        .p_data = m_enc_scan_response_data,
        .len    = BLE_GAP_ADV_SET_DATA_SIZE_MAX

    }
};

void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    NRF_LOG_INFO("%s\n", __func__)
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}


static void gap_params_init(void)
{
    NRF_LOG_INFO("%s\n", __func__)
    ret_code_t              err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}

static void advertising_init(void)
{
    NRF_LOG_INFO("%s\n", __func__)
    ret_code_t    err_code;
    ble_advdata_t advdata;  //藍牙廣播送的資料（這個結構有定好格式）
    ble_advdata_t srdata;   //有裝置掃描後廣播送的資料（格式跟上面一樣）

    //0xFD81公司服務，之後可以用這個當過濾器，因為藍牙掃描後會看到很多裝置，可以透過這個過濾哪一個是公司的服務
    //BLE_UUID_TYPE_BLE:只看前16bit(只看前16bit的是保留的UUID)
    ble_uuid_t adv_uuids[] = {{0xFD81, BLE_UUID_TYPE_BLE}}; //GATT的UUID

    // Build and set advertising data. (建立以及設定廣播資料)
    memset(&advdata, 0, sizeof(advdata));

    advdata.name_type          = BLE_ADVDATA_FULL_NAME; //廣播全名
    advdata.include_appearance = true;
    advdata.flags              = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;   //設定廣播模式（透過GAP設定）

    //p_manuf_specific_data:製造商特定數據
    //公司自訂0x055a公司編號
    //如果.company_identifier不填會自動填入0x0000:Ericsson
    uint8_t product_register[3] = {0x05, 0x00, 0x01}; //這是可以自訂要廣播的東西，想放啥就放啥
    ble_advdata_manuf_data_t m_sp_manuf_advdata = {.company_identifier = 0x055a, .data   ={.size   = 3, .p_data = product_register}};
    advdata.p_manuf_specific_data = &m_sp_manuf_advdata;

    memset(&srdata, 0, sizeof(srdata));
    srdata.uuids_complete.uuid_cnt = sizeof(adv_uuids) / sizeof(adv_uuids[0]);
    srdata.uuids_complete.p_uuids  = adv_uuids;
//    srdata.p_manuf_specific_data = &m_sp_manuf_advdata;

    //廣播的資料
    err_code = ble_advdata_encode(&advdata, m_adv_data.adv_data.p_data, &m_adv_data.adv_data.len);
    APP_ERROR_CHECK(err_code);

    //掃描後葛波的資料，不同的地方在於用scan_rsp_data
    err_code = ble_advdata_encode(&srdata, m_adv_data.scan_rsp_data.p_data, &m_adv_data.scan_rsp_data.len);
    APP_ERROR_CHECK(err_code);


    // Set advertising parameters.（設定廣播參數）

    //filter_policy
        //BLE_GAP_ADV_FP_ANY：允許來自任何設備的掃描請求和連接請求。
        //BLE_GAP_ADV_FP_FILTER_SCANREQ：使用白名單過濾掃描請求。
        //BLE_GAP_ADV_FP_FILTER_CONNREQ：使用白名單過濾連接請求。
        //BLE_GAP_ADV_FP_FILTER_BOTH：使用白名單過濾掃描和連接請求。
    ble_gap_adv_params_t adv_params;
    memset(&adv_params, 0, sizeof(adv_params));

    adv_params.primary_phy     = BLE_GAP_PHY_AUTO; //設定PHY，1m,2m,coded, auto
    adv_params.duration        = APP_ADV_DURATION; //設定廣播時間，這裡設無限
    adv_params.properties.type = BLE_GAP_ADV_TYPE_CONNECTABLE_SCANNABLE_UNDIRECTED;  //設定廣播屬性，可否連，可否掃描，有沒有方向
    adv_params.p_peer_addr     = NULL; //已經知道的目標地址
    adv_params.filter_policy   = BLE_GAP_ADV_FP_ANY; //要怎麼過濾廣播
    adv_params.interval        = APP_ADV_INTERVAL;  //廣播間隔

    err_code = sd_ble_gap_adv_set_configure(&m_adv_handle, &m_adv_data, &adv_params);
    APP_ERROR_CHECK(err_code);
}




static void nrf_qwr_error_handler(uint32_t nrf_error)
{
    NRF_LOG_INFO("%s\n", __func__)

    APP_ERROR_HANDLER(nrf_error);
}

//單機連線
static void led_write_handler(uint16_t conn_handle, ble_lbs_t * p_lbs, uint8_t led_state)
{
    NRF_LOG_INFO("%s\n", __func__)

    if (led_state)
    {
        bsp_board_led_on(LEDBUTTON_LED);
        NRF_LOG_INFO("Received LED ON!");
    }
    else
    {
        bsp_board_led_off(LEDBUTTON_LED);
        NRF_LOG_INFO("Received LED OFF!");
    }
}


//m_lbs記載如果要運行lbs服務需要設定的參數(UUID等)以及函數
//單機連線
static void services_init(void)
{
    NRF_LOG_INFO("%s\n", __func__)

    ret_code_t         err_code;
    ble_lbs_init_t     init     = {0};
    nrf_ble_qwr_init_t qwr_init = {0};

    // Initialize Queued Write Module.(初始化GATT佇列寫入模組) 佇列：先進先出的資料結構
    qwr_init.error_handler = nrf_qwr_error_handler;

    err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init); //初始化藍牙GATT佇列
    APP_ERROR_CHECK(err_code);

    // Initialize LBS.(初始化LED＆按鍵)
    init.led_write_handler = led_write_handler;

    err_code = ble_lbs_init(&m_lbs, &init);
    APP_ERROR_CHECK(err_code);
}


static void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
    NRF_LOG_INFO("%s\n", __func__)

    ret_code_t err_code;

    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}


static void conn_params_error_handler(uint32_t nrf_error)
{
    NRF_LOG_INFO("%s\n", __func__)

    APP_ERROR_HANDLER(nrf_error);
}


static void conn_params_init(void)
{
    NRF_LOG_INFO("%s\n", __func__)

    ret_code_t             err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = on_conn_params_evt;
    cp_init.error_handler                  = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}


static void advertising_start(void)
{
    NRF_LOG_INFO("%s\n", __func__)

    ret_code_t           err_code;

    err_code = sd_ble_gap_adv_start(m_adv_handle, APP_BLE_CONN_CFG_TAG);
    APP_ERROR_CHECK(err_code);

    bsp_board_led_on(ADVERTISING_LED);
}


//藍芽監聽事件回應動作
static void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context)
{
    NRF_LOG_INFO("%s\n", __func__)

    ret_code_t err_code;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            NRF_LOG_INFO("\t%s -> BLE_GAP_EVT_CONNECTED \n", __func__)

            NRF_LOG_INFO("Connected");
            bsp_board_led_on(CONNECTED_LED);
            bsp_board_led_off(ADVERTISING_LED);
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle; //連線後拿到handle編號
            err_code = nrf_ble_qwr_conn_handle_assign(&m_qwr, m_conn_handle);
            APP_ERROR_CHECK(err_code);
            err_code = app_button_enable();
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            NRF_LOG_INFO("\t%s -> BLE_GAP_EVT_DISCONNECTED \n", __func__)

            NRF_LOG_INFO("Disconnected");
            bsp_board_led_off(CONNECTED_LED);
            m_conn_handle = BLE_CONN_HANDLE_INVALID;
            err_code = app_button_disable();
            APP_ERROR_CHECK(err_code);
            advertising_start();
            break;

        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            NRF_LOG_INFO("\t%s -> BLE_GAP_EVT_SEC_PARAMS_REQUEST \n", __func__)

            // Pairing not supported
            err_code = sd_ble_gap_sec_params_reply(m_conn_handle,
                                                   BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP,
                                                   NULL,
                                                   NULL);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
        {
            NRF_LOG_INFO("\t%s -> BLE_GAP_EVT_PHY_UPDATE_REQUEST \n", __func__)

            NRF_LOG_DEBUG("PHY update request.");
            ble_gap_phys_t const phys =
            {
                .rx_phys = BLE_GAP_PHY_AUTO,
                .tx_phys = BLE_GAP_PHY_AUTO,
            };
            err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
            APP_ERROR_CHECK(err_code);
        } break;

        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
            NRF_LOG_INFO("\t%s -> BLE_GATTS_EVT_SYS_ATTR_MISSING \n", __func__)

            // No system attributes have been stored.
            err_code = sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0, 0);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTC_EVT_TIMEOUT:
            NRF_LOG_INFO("\t%s -> BLE_GATTC_EVT_TIMEOUT \n", __func__)

            // Disconnect on GATT Client timeout event.
            NRF_LOG_DEBUG("GATT Client Timeout.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_TIMEOUT:
            NRF_LOG_INFO("\t%s -> BLE_GATTS_EVT_TIMEOUT \n", __func__)

            // Disconnect on GATT Server timeout event.
            NRF_LOG_DEBUG("GATT Server Timeout.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;



        default:
            // No implementation needed.
            break;
    }
}


static void ble_stack_init(void)
{
    NRF_LOG_INFO("%s\n", __func__)

    ret_code_t err_code;

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // Configure the BLE stack using the default settings.(使用預設設定配置藍牙協定疊)
    // Fetch the start address of the application RAM.(取得app在ram的起始位址)
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);   //先隨便建一個指標變數傳入，之後會填上內部設定好的app記憶體門牌號碼
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.(開啟藍牙協定疊，我發現他只要知道app記憶體位置就知道要怎麼啟動藍牙協定疊了)
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    // Register a handler for BLE events.(為低功耗藍牙事件註冊一個監聽者)
    //傳入參數：名字，監聽優先序，事件監聽函數，預留一個參數的空間想放啥就放啥
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);
}


static void button_event_handler(uint8_t pin_no, uint8_t button_action)
{
    NRF_LOG_INFO("%s\n", __func__)

    ret_code_t err_code;

    switch (pin_no)
    {
        case LEDBUTTON_BUTTON:
            NRF_LOG_INFO("Send button state change.");
            err_code = ble_lbs_on_button_change(m_conn_handle, &m_lbs, button_action);
            if (err_code != NRF_SUCCESS &&
                err_code != BLE_ERROR_INVALID_CONN_HANDLE &&
                err_code != NRF_ERROR_INVALID_STATE &&
                err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING)
            {
                APP_ERROR_CHECK(err_code);
            }
            break;

        default:
            APP_ERROR_HANDLER(pin_no);
            break;
    }
}


static void buttons_init(void)
{
    NRF_LOG_INFO("%s\n", __func__)

    //The array must be static because a pointer to it will be saved in the button handler module.
    //如果有多個按鈕在這邊設定
    //    app_button_cfg_t
    //            pin_no:按鈕編號
    //            active_state：初始狀態要是0或1
    //            pull_cfg
    //            button_handler：按下後要執行的函數
    static app_button_cfg_t buttons[] =
    {
        {LEDBUTTON_BUTTON, false, BUTTON_PULL, button_event_handler}
    };

//    NRF_LOG_INFO("ARRAY_SIZE(buttons):%d", ARRAY_SIZE(buttons)); //算有幾個按鈕
    ret_code_t err_code = app_button_init(buttons, ARRAY_SIZE(buttons),
                               BUTTON_DETECTION_DELAY);
    APP_ERROR_CHECK(err_code);
}

static void idle_state_handle(void)
{
    if (NRF_LOG_PROCESS() == false)
    {
        nrf_pwr_mgmt_run();
    }
}


int main(void)
{
    NRF_LOG_INFO("%s\n", __func__)

    // Initialize.
    NRF_LOG_INIT(NULL);     // log_init();
    NRF_LOG_DEFAULT_BACKENDS_INIT();    //log_init();
    bsp_board_init(BSP_INIT_LEDS);  //leds_init();
    app_timer_init();   //    timers_init();
    buttons_init();
    nrf_pwr_mgmt_init();    //power_management_init();
    ble_stack_init();   //開啟sd中的藍牙協定疊，如果需要修改app記憶體位址提示怎麼改，最後為低功耗藍牙事件註冊一個監聽者
    gap_params_init();  //設定GAP參數，GAP用來控制裝置連線和廣播，使你的裝置被其他裝置可見，並決定了你的裝置是否可以或者怎樣與互動裝置進行通訊。做握手的感覺
    nrf_ble_gatt_init(&m_gatt, NULL); //gatt_init(); 初始化GATT，GATT控制主機跟外設用GAP建立連線後的雙向通訊
    services_init(); //初始化GATT佇列，LED＆按鍵
    advertising_init(); //廣播設定
    conn_params_init();

    // Start execution.
    NRF_LOG_INFO("Blinky example started.");
    advertising_start();

    // Enter main loop.
    for (;;)
    {
        idle_state_handle();
    }
}
