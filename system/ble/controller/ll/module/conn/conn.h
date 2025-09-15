#include"common/txCommon.h"
#include"../../ll_internal.h"

typedef enum
{
	CONN_ROLE_PERIPHERAL = 0,
	CONN_ROLE_CENTRAL    = 1,
}conn_role_e;


int ble_ll_enter_connection_state(ble_ll_event_e event);
