#include"common/txCommon.h"
#include"../../ll_internal.h"
#include"../../ll.h"
#define BLE_ADV_ACCESS_CODE   0xd6be898e
#define BLE_ADV_CRC_INIT      0x555555

#define BLE_ADV_MAX_LENGTH    37
#define BLE_ADV_INTERVAL_UNIT 1250

#define BLE_ADV_CHANNEL_IDX_BIT0  37
#define BLE_ADV_CHANNEL_IDX_BIT1  38
#define BLE_ADV_CHANNEL_IDX_BIT2  39

typedef struct _PACKED{
	_u8 advA[6];
	_u8 advData[2];//0-31 byte
}adv_type_ind_t;

typedef struct _PACKED{
	_u8 advA[6];
	_u8 targetA[6];
}adv_type_direct_ind_t;

typedef struct _PACKED{
	_u8 advA[6];
	_u8 advData[2];//0-31 byte
}adv_type_nonConn_ind_t;

typedef struct _PACKED{
	_u8 advA[6];
	_u8 advData[2];//0-31 byte
}adv_type_scan_ind_t;

typedef struct _PACKED{
	_u8 advA[6];
	_u8 scanRsp[2];//0-31 byte
}adv_type_scan_rsp_t;



int ble_ll_enter_advertising_state(ble_ll_event_e event);
