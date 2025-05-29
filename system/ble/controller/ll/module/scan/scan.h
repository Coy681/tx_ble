#include"common/txCommon.h"
#include"../../ll_internal.h"


typedef struct _PACKED{
	_u8 scanA[6];
	_u8 advA[6];
}scan_type_scan_req_t;


int ble_ll_enter_scanning_state(ble_ll_event_e event);