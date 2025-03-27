/**
 * BLE Link Layer States
 * standby state
 * advertising state                - advertiser
 * scanning state                   - scanner         
 * initiating state                 - initiator
 * connection state                 - central/peripheral
 * synchronization state            - synchronized receiver(isochronous)
 * isochronous broadcasting state   - isochronous broadcaster
 */

/**
 * link layer state machine maybe have multiple instances,and only one state to be active at a time.
 */
typedef enum
{
    BLE_LL_STATE_STANDBY,
    BLE_LL_STATE_ADVERTISING,
    BLE_LL_STATE_SCANNING,
    BLE_LL_STATE_INITIATING,
    BLE_LL_STATE_CONNECTION,
    BLE_LL_STATE_SYNCHRONIZATION,
    BLE_LL_STATE_BROADCASTING,
}ble_ll_state_e;

typedef enum
{
    BLE_LL_EVENT_START_ADVERTISING,
    BLE_LL_EVENT_STOP_ADVERTISING,

    BLE_LL_EVENT_START_SCANNING,
    BLE_LL_EVENT_STOP_SCANNING,

    BLE_LL_EVENT_START_INITIATING,
    BLE_LL_EVENT_STOP_INITIATING,

    BLE_LL_EVENT_START_SYNCHRONIZATION,
    BLE_LL_EVENT_STOP_SYNCHRONIZATION,

    BLE_LL_EVENT_START_BROADCASTING,
    BLE_LL_EVENT_STOP_BROADCASTING,

    BLE_LL_EVENT_START_CONNECTION,
    BLE_LL_EVENT_STOP_CONNECTION,
}ble_ll_event_e;

typedef ble_ll_state_e(*ble_ll_event_handler)(ble_ll_event_e);

typedef struct 
{
    ble_ll_state_e       state;
    ble_ll_event_handler handler;
}ble_ll_state_machine_t;



