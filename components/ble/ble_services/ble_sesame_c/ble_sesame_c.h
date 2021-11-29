
#ifndef BLE_LBS_C_H__
#define BLE_LBS_C_H__

#include <stdint.h>
#include "ble.h"
#include "ble_db_discovery.h"
#include "ble_srv_common.h"
#include "nrf_ble_gq.h"
#include "nrf_sdh_ble.h"


#define BLE_LBS_C_DEF(_name)                                                                        \
static ble_lbs_c_t _name;                                                                           \
NRF_SDH_BLE_OBSERVER(_name ## _obs,                                                                 \
                     BLE_LBS_C_BLE_OBSERVER_PRIO,                                                   \
                     ble_lbs_c_on_ble_evt, &_name)

#define BLE_LBS_C_ARRAY_DEF(_name, _cnt)                                                            \
static ble_lbs_c_t _name[_cnt];                                                                     \
NRF_SDH_BLE_OBSERVERS(_name ## _obs,                                                                \
                      BLE_LBS_C_BLE_OBSERVER_PRIO,                                                  \
                      ble_lbs_c_on_ble_evt, &_name, _cnt)


#define LBS_UUID_BASE        {0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, \
                              0xDE, 0xEF, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00}
#define LBS_UUID_SERVICE     0x1523
#define LBS_UUID_BUTTON_CHAR 0x1524
#define LBS_UUID_LED_CHAR    0x1525

/**@brief LBS Client event type. */
typedef enum {
    BLE_LBS_C_EVT_DISCOVERY_COMPLETE = 1,  /**< Event indicating that the LED Button Service was discovered at the peer. */
    BLE_LBS_C_EVT_BUTTON_NOTIFICATION      /**< Event indicating that a notification of the LED Button Button characteristic was received from the peer. */
} ble_lbs_c_evt_type_t;

/**@brief Structure containing the Button value received from the peer. */
typedef struct {
    uint8_t button_state;  /**< Button Value. */
} ble_button_t;

/**@brief Structure containing the handles related to the LED Button Service found on the peer. */
typedef struct {
    uint16_t button_cccd_handle;  /**< Handle of the CCCD of the Button characteristic. */
    uint16_t button_handle;       /**< Handle of the Button characteristic as provided by the SoftDevice. */
    uint16_t led_handle;          /**< Handle of the LED characteristic as provided by the SoftDevice. */
} lbs_db_t;

/**@brief LED Button Event structure. */
typedef struct {
    ble_lbs_c_evt_type_t evt_type;    /**< Type of the event. */
    uint16_t conn_handle; /**< Connection handle on which the event occured.*/
    union {
        ble_button_t button;          /**< Button value received. This is filled if the evt_type is @ref BLE_LBS_C_EVT_BUTTON_NOTIFICATION. */
        lbs_db_t peer_db;         /**< Handles related to the LED Button Service found on the peer device. This is filled if the evt_type is @ref BLE_LBS_C_EVT_DISCOVERY_COMPLETE.*/
    } params;
} ble_lbs_c_evt_t;

// Forward declaration of the ble_lbs_c_t type.
typedef struct ble_lbs_c_s ble_lbs_c_t;

typedef void (*ble_lbs_c_evt_handler_t)(ble_lbs_c_t *p_ble_lbs_c, ble_lbs_c_evt_t *p_evt);

/**@brief LED Button Client structure. */
struct ble_lbs_c_s {
    uint16_t conn_handle;   /**< Connection handle as provided by the SoftDevice. */
    lbs_db_t peer_lbs_db;   /**< Handles related to LBS on the peer. */
    ble_lbs_c_evt_handler_t evt_handler;   /**< Application event handler to be called when there is an event related to the LED Button service. */
    ble_srv_error_handler_t error_handler; /**< Function to be called in case of an error. */
    uint8_t uuid_type;     /**< UUID type. */
    nrf_ble_gq_t *p_gatt_queue;  /**< Pointer to the BLE GATT Queue instance. */
};

typedef struct {
    ble_lbs_c_evt_handler_t evt_handler;   /**< Event handler to be called by the LED Button Client module when there is an event related to the LED Button Service. */
    nrf_ble_gq_t *p_gatt_queue;  /**< Pointer to the BLE GATT Queue instance. */
    ble_srv_error_handler_t error_handler; /**< Function to be called in case of an error. */
} ble_lbs_c_init_t;


uint32_t ble_lbs_c_init(ble_lbs_c_t *p_ble_lbs_c, ble_lbs_c_init_t *p_ble_lbs_c_init);

void ble_lbs_c_on_ble_evt(ble_evt_t const *p_ble_evt, void *p_context);


uint32_t ble_lbs_c_button_notif_enable(ble_lbs_c_t *p_ble_lbs_c);


void ble_lbs_on_db_disc_evt(ble_lbs_c_t *p_ble_lbs_c, const ble_db_discovery_evt_t *p_evt);


uint32_t ble_lbs_c_handles_assign(ble_lbs_c_t *p_ble_lbs_c,
                                  uint16_t conn_handle,
                                  const lbs_db_t *p_peer_handles);


uint32_t ble_lbs_led_status_send(ble_lbs_c_t *p_ble_lbs_c, uint8_t status);


#endif // BLE_LBS_C_H__

/** @} */
