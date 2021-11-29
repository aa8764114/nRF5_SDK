#include <stdint.h>
#include <string.h>
#include "nrf.h"
#include "nordic_common.h"
#ifdef SOFTDEVICE_PRESENT
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#else
#include "nrf_drv_clock.h"
#endif
#include "fds.h"
#include "app_timer.h"
#include "app_error.h"
#include "nrf_cli.h"
#include "fds_example.h"

#define NRF_LOG_MODULE_NAME app
#include "nrf_log.h"
#include "nrf_log_ctrl.h"


/* A tag identifying the SoftDevice BLE configuration. */
#define APP_BLE_CONN_CFG_TAG    1

/* Array to map FDS events to strings. */
static char const * fds_evt_str[] =
{
    "FDS_EVT_INIT",
    "FDS_EVT_WRITE",
    "FDS_EVT_UPDATE",
    "FDS_EVT_DEL_RECORD",
    "FDS_EVT_DEL_FILE",
    "FDS_EVT_GC",
};

/* Dummy configuration data. */
static configuration_t m_dummy_cfg =
{
    .config1_on  = false,
    .config2_on  = true,
    .boot_count  = 0x0,
    .device_name = "dummy",
};

/* A record containing dummy configuration data. */
static fds_record_t const m_dummy_record =
{
    .file_id           = CONFIG_FILE,
    .key               = CONFIG_REC_KEY,
    .data.p_data       = &m_dummy_cfg,
    /* The length of a record is always expressed in 4-byte units (words). */
    .data.length_words = (sizeof(m_dummy_cfg) + 3) / sizeof(uint32_t),
};

/* Keep track of the progress of a delete_all operation. */
static struct
{
    bool delete_next;   //!< Delete next record.
    bool pending;       //!< Waiting for an fds FDS_EVT_DEL_RECORD event, to delete the next record.
} m_delete_all;

/* Flag to check fds initialization. */
static bool volatile m_fds_initialized;


const char *fds_err_str(ret_code_t ret)
{
    /* Array to map FDS return values to strings. */
    static char const * err_str[] =
    {
        "FDS_ERR_OPERATION_TIMEOUT",
        "FDS_ERR_NOT_INITIALIZED",
        "FDS_ERR_UNALIGNED_ADDR",
        "FDS_ERR_INVALID_ARG",
        "FDS_ERR_NULL_ARG",
        "FDS_ERR_NO_OPEN_RECORDS",
        "FDS_ERR_NO_SPACE_IN_FLASH",
        "FDS_ERR_NO_SPACE_IN_QUEUES",
        "FDS_ERR_RECORD_TOO_LARGE",
        "FDS_ERR_NOT_FOUND",
        "FDS_ERR_NO_PAGES",
        "FDS_ERR_USER_LIMIT_REACHED",
        "FDS_ERR_CRC_CHECK_FAILED",
        "FDS_ERR_BUSY",
        "FDS_ERR_INTERNAL",
    };

    return err_str[ret - NRF_ERROR_FDS_ERR_BASE];
}


static void fds_evt_handler(fds_evt_t const * p_evt)
{
    if (p_evt->result == NRF_SUCCESS)
    {
        NRF_LOG_GREEN("Event: %s received (NRF_SUCCESS)",
                      fds_evt_str[p_evt->id]);
    }
    else
    {
        NRF_LOG_GREEN("Event: %s received (%s)",
                      fds_evt_str[p_evt->id],
                      fds_err_str(p_evt->result));
    }

    switch (p_evt->id)
    {
        case FDS_EVT_INIT:
            if (p_evt->result == NRF_SUCCESS)
            {
                m_fds_initialized = true;
            }
            break;

        case FDS_EVT_WRITE:
        {
            if (p_evt->result == NRF_SUCCESS)
            {
                NRF_LOG_INFO("Record ID:\t0x%04x",  p_evt->write.record_id);
                NRF_LOG_INFO("File ID:\t0x%04x",    p_evt->write.file_id);
                NRF_LOG_INFO("Record key:\t0x%04x", p_evt->write.record_key);
            }
        } break;

        case FDS_EVT_DEL_RECORD:
        {
            if (p_evt->result == NRF_SUCCESS)
            {
                NRF_LOG_INFO("Record ID:\t0x%04x",  p_evt->del.record_id);
                NRF_LOG_INFO("File ID:\t0x%04x",    p_evt->del.file_id);
                NRF_LOG_INFO("Record key:\t0x%04x", p_evt->del.record_key);
            }
            m_delete_all.pending = false;
        } break;

        default:
            break;
    }
}


/**@brief   Begin deleting all records, one by one. */
void delete_all_begin(void)
{
    m_delete_all.delete_next = true;
}


/**@brief   Process a delete all command.
 *
 * Delete records, one by one, until no records are left.
 */
void delete_all_process(void)
{
    if (   m_delete_all.delete_next
        & !m_delete_all.pending)
    {
        NRF_LOG_INFO("Deleting next record.");

        m_delete_all.delete_next = record_delete_next();
        if (!m_delete_all.delete_next)
        {
            NRF_LOG_CYAN("No records left to delete.");
        }
    }
}


#ifdef SOFTDEVICE_PRESENT
/**@brief   Function for initializing the SoftDevice and enabling the BLE stack. */
static void ble_stack_init(void)
{
    ret_code_t rc;
    uint32_t   ram_start;

    /* Enable the SoftDevice. */
    rc = nrf_sdh_enable_request();
    APP_ERROR_CHECK(rc);

    rc = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(rc);

    rc = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(rc);
}
#else
static void clock_init(void)
{
    /* Initialize the clock. */
    ret_code_t rc = nrf_drv_clock_init();
    APP_ERROR_CHECK(rc);

    nrf_drv_clock_lfclk_request(NULL);

    /* Wait for the clock to be ready. */
    while (!nrf_clock_lf_is_running()) {;}
}
#endif


/**@brief   Initialize the timer. */
static void timer_init(void)
{
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
}


/**@brief   Initialize logging. */
static void log_init(void)
{
    ret_code_t rc = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(rc);
}


/**@brief   Sleep until an event is received. */
static void power_manage(void)
{
#ifdef SOFTDEVICE_PRESENT
    (void) sd_app_evt_wait();
#else
    __WFE();
#endif
}


/**@brief   Wait for fds to initialize. */
static void wait_for_fds_ready(void)
{
    while (!m_fds_initialized)
    {
        power_manage();
        NRF_LOG_INFO("wait_for_fds_ready");
    }
}


int main(void)
{
    ret_code_t rc;

#ifdef SOFTDEVICE_PRESENT
    ble_stack_init();
#else
    clock_init();
#endif

    timer_init();
    log_init();

    NRF_LOG_INFO("FDS example started.")

    /* Register first to receive an event when initialization is complete. */
    (void) fds_register(fds_evt_handler); //註冊FDS

    NRF_LOG_INFO("Initializing fds...");

    rc = fds_init();    //初始化FDS
    APP_ERROR_CHECK(rc);

    /* Wait for fds to initialize. */
    wait_for_fds_ready();   //等待FDS初始化完成

    NRF_LOG_INFO("Available commands:");
    NRF_LOG_INFO("- print all\t\tprint records");
    NRF_LOG_INFO("- print config\tprint configuration");
    NRF_LOG_INFO("- update\t\tupdate configuration");
    NRF_LOG_INFO("- stat\t\tshow statistics");
    NRF_LOG_INFO("- write\t\twrite a new record");
    NRF_LOG_INFO("- delete\t\tdelete a record");
    NRF_LOG_INFO("- delete_all\tdelete all records");
    NRF_LOG_INFO("- gc\t\trun garbage collection");

    NRF_LOG_INFO("Reading flash usage statistics...");


    fds_record_desc_t desc = {0};   //透過他拿到要拿到的內部儲存空間在哪
    fds_find_token_t  tok  = {0};

    rc = fds_record_find(CONFIG_FILE, CONFIG_REC_KEY, &desc, &tok); //找到要讀取的內部儲存空間位址

    if (rc == NRF_SUCCESS)  //如果有找到
    {
        /* A config file is in flash. Let's update it. */
        fds_flash_record_t config = {0};    //從內部儲存空間讀到的資料會放這裡

        /* Open the record and read its contents. */
        rc = fds_record_open(&desc, &config);   //打開儲存空間中的內容
        APP_ERROR_CHECK(rc);

        /* Copy the configuration from flash into m_dummy_cfg. */
        //m_dummy_cfg:存要記錄資訊的結構
        memcpy(&m_dummy_cfg, config.p_data, sizeof(configuration_t));   //把弄到的儲存空間內容複製到RAM中

        NRF_LOG_INFO("Config file found, updating boot count to %d.", m_dummy_cfg.boot_count);  //印出當前的重開次數

        /* Update boot count. */
        m_dummy_cfg.boot_count++;   //重開次數+1

        /* Close the record when done reading. */
        rc = fds_record_close(&desc);   //關檔
        APP_ERROR_CHECK(rc);

        /* Write the updated record to flash. */
        //m_dummy_record裡面的欄位用指標接到m_dummy_cfg
        rc = fds_record_update(&desc, &m_dummy_record); //更新內部儲存空間中的內容
        if ((rc != NRF_SUCCESS) && (rc == FDS_ERR_NO_SPACE_IN_FLASH))
        {
            NRF_LOG_INFO("No space in flash, delete some records to update the config file.");
        }
        else
        {
            APP_ERROR_CHECK(rc);
        }
    }
    else     //如果沒搜尋到
    {
        /* System config not found; write a new one. */
        NRF_LOG_INFO("Writing config file...");

        rc = fds_record_write(&desc, &m_dummy_record);  //直接把m_dummy_record的內容寫入內部儲存空間
        if ((rc != NRF_SUCCESS) && (rc == FDS_ERR_NO_SPACE_IN_FLASH))
        {
            NRF_LOG_INFO("No space in flash, delete some records to update the config file.");
        }
        else
        {
            APP_ERROR_CHECK(rc);
        }
    }


    /* Enter main loop. */
    for (;;)
    {
        if (!NRF_LOG_PROCESS())
        {
            power_manage();
        }
        delete_all_process();
    }
}


/**
 * @}
 */
