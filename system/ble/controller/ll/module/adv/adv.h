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

typedef enum
{
    ADV_EVENT,
    #if(LL_SUPPORT_LE_EXTENDED_ADVERTISING==1)
    ADV_EXTENDED_EVENT,
    #endif
    #if(LL_SUPPORT_LE_PERIODIC_ADVERTISING==1)
    ADV_PERIODIC_EVENT,
    #endif
    #if(LL_SUPPORT_PERIODIC_ADVERTISING_WITH_RESPONSES_ADVERTISER==1)
    ADV_PERIODIC_WITH_RSP_EVENT,
    #endif
}adv_event_class_e;

typedef enum
{
    ADV_EVENT_CONNECTABLE_SCANNABLE_UNDIRECTED,
    ADV_EVENT_CONNECTABLE_DIRECTED,
    ADV_EVENT_SCANNABLE_UNDIRECTED,
    ADV_EVENT_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED,
    #if(LL_SUPPORT_LE_EXTENDED_ADVERTISING==1)
    ADV_EVENT_EXTENDED_CONNECTABLE_DIRECTED,
    ADV_EVENT_EXTENDED_CONNECTABLE_UNDIRECTED,
    ADV_EVENT_EXTENDED_SCANNABLE_DIRECTED,
    ADV_EVENT_EXTENDED_SCANNABLE_UNDIRECTED,
    ADV_EVENT_EXTENDED_NON_CONNECTABLE_NON_SCANNABLE_DIRECTED,
    ADV_EVENT_EXTENDED_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED,
    #endif
    #if(LL_SUPPORT_LE_PERIODIC_ADVERTISING==1)
    ADV_EVENT_EXTENDED_PERIODIC,
    #endif
    #if(LL_SUPPORT_PERIODIC_ADVERTISING_WITH_RESPONSES_ADVERTISER==1)
    ADV_EVENT_EXTENDED_PERIODIC_WITH_RESPONSE,
    #endif
}adv_event_type_e;

int ble_ll_enter_advertising_state(ble_ll_event_e event);
