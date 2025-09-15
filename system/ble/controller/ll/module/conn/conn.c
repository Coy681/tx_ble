#include"conn.h"

typedef int(*conn_event_sm_cb)(ll_sm_t* ll,ll_internal_connection_ctrl_t* connParam);

typedef enum
{
	CONN_SM_STATE_IDLE,
	CONN_SM_SYNCING,
	CONN_SM_STATE_SENDING,
	CONN_SM_STATE_RECEIVING,
}conn_sm_state_e;;

typedef struct _PACKED
{
	conn_event_sm_cb cb;
	conn_sm_state_e  currentState;
	conn_sm_state_e  transSuccessState;
	conn_sm_state_e  transFailState;
	ll_event_e       event;
}conn_event_sm_t;

_RAM_CODE


static conn_event_sm_t conn_event_state_machine[] =
{
	{},

};



_RAM_CODE
static void conn_sch_callback(_u8 type)
{

}

_RAM_CODE
static void conn_phy_irq_callback(_u8 type)
{

}


int ble_ll_enter_connection_state(ble_ll_event_e event)
{
    ll_sm_t* ll = ll_get_current_state_machine();
    if(BLE_LL_STATE_ADVERTISING == ll->state)
    {
    	ll->conn->role = CONN_ROLE_PERIPHERAL;
    }
    else if(BLE_LL_STATE_INITIATING == ll->state)
    {
    	ll->conn->role = CONN_ROLE_CENTRAL;
    }
    else 
    {
        return 0;
    }
    ll->conn->maxTxOctets = BLE_CONN_TX_MAX_OCTETS;
    ll->conn->maxTxTime   = ll_get_air_packet_time(ll->phy.mode,BLE_CONN_TX_MAX_OCTETS,1);
    ll->conn->maxRxOctets = BLE_CONN_RX_MAX_OCTETS;
    ll->conn->maxRxTime   = ll_get_air_packet_time(ll->phy.mode,BLE_CONN_RX_MAX_OCTETS,1);
    ll->phy.hw_irq_cb     = conn_phy_irq_callback;
    ll->sch.cb            = conn_sch_callback;
	LOG_TRACE(LL_LOG_TRACE,"enter connection state",0,0)
    return 1;
}
