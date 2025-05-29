#include"common/txCommon.h"
#include"../../ll_internal.h"

typedef struct _PACKED{
    _u32 AA;

    _u32 crcInit:24;
    _u32 winSize:8;

    _u16 winOffset;
    _u16 interval;

    _u16 latency;
    _u16 timeout;

    _u8 chM[5];
    _u8 hop:5;
    _u8 sca:3;
}init_connectInd_llData_t;

typedef struct _PACKED{
	_u8 initA[6];
    _u8 advA[6];
	_u8 llData[22];
}init_type_connectInd_t;

int ble_ll_enter_initiating_state(ble_ll_event_e event);