#include"common/txCommon.h"
#include"../../ll_internal.h"

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

int ble_ll_enter_connection_state(ble_ll_event_e event);


/***********************Bluetooth LE Subrate **************************/
#if(LL_SUPPORT_CONNECTION_SUBRATING)
typedef struct _PACKED
{
    _u16 subrateMin;//0x001-0x1f4
    _u16 subrateMax;//0x001-0x1f4
    _u16 maxLatency;//0x000-0x1f3
    _u16 continuationNumber;//0x000-0x1f3
    _u16 supervisionTimeout;//0x00a-0xc80
}ll_conn_subrate_param_t;

controller_error_code_e ll_set_default_subrate(_u16 connHandle,ll_conn_subrate_param_t* param);

#endif
