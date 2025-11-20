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
	ll_ctrlPdu_opcode_e      opcode;
	conn_ctrl_pdu_process_f  process;
}conn_ctrl_pdu_process_t;

int ble_ll_enter_connection_state(ble_ll_event_e event);
