#include"common/txCommon.h"
#include"../../ll_internal.h"

#define BLE_ADV_ACCESS_CODE   0xd6be898e
#define BLE_ADV_CRC_INIT      0x555555

#define BLE_ADV_MAX_LENGTH    37

int ble_ll_enter_advertising_state(ble_ll_event_e event);