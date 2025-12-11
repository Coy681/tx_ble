#include <system/ble/controller/ll/ll_enum.h>
#include"common/txCommon.h"
#include"../../ll_feature.h"

typedef enum
{
	CONN_ROLE_PERIPHERAL = 0,
	CONN_ROLE_CENTRAL    = 1,
}conn_role_e;

typedef int (*conn_ctrl_pdu_process_f)(_u8* data);
typedef struct
{
	_u16      opcode;
	_u16      length;
	conn_ctrl_pdu_process_f  process;
}conn_ctrl_pdu_process_t;

/***********************Bluetooth LE Subrate **************************/
#if defined(BLE_SUPPORT_CONN)
#if(LL_SUPPORT_CONNECTION_SUBRATING)
controller_error_code_e ll_subrate_request(_u16 connHandle,\
		                                   _u16 subrateMin,\
										   _u16 subrateMax,
										   _u16 maxLatency,
										   _u16 continuation,
										   _u16 timeout);
#endif

#endif

