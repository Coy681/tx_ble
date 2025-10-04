#include"conn.h"

#if defined(BLE_SUPPORT_CONN)


typedef int(*conn_event_sm_cb)(ll_sm_t* ll,ll_internal_connection_ctrl_t* connParam);

typedef enum
{
	CONN_STATE_IDLE,
	CONN_STATE_SYNCING,
	CONN_STATE_CONNECTED,
}conn_state_e;
typedef enum
{
	CONN_STATE_IDLE,
	CONN_STATE_SYNCING,
	CONN_STATE_CONNECTED,
}conn_state_e;

typedef enum
{
	CONN_SM_STATE_IDLE,
	CONN_SM_SYNCING,
	CONN_SM_STATE_SENDING,
	CONN_SM_STATE_RECEIVING,
}conn_sm_state_e;

typedef struct _PACKED
{
	conn_event_sm_cb cb;
	conn_sm_state_e  currentState;
	conn_sm_state_e  transSuccessState;
	conn_sm_state_e  transFailState;
	ll_event_e       event;
}conn_event_sm_t;

_RAM_CODE static int peri_conn_event_sch_start(ll_sm_t* ll,ll_internal_connection_ctrl_t* connParam)
{

}
_RAM_CODE static int peri_conn_event_sch_stop(ll_sm_t* ll,ll_internal_connection_ctrl_t* connParam)
{

}
_RAM_CODE static int peri_conn_event_sch_passed(ll_sm_t* ll,ll_internal_connection_ctrl_t* connParam)
{

}
_RAM_CODE static int peri_conn_event_sch_canceled(ll_sm_t* ll,ll_internal_connection_ctrl_t* connParam)
{

}
_RAM_CODE static int peri_conn_event_phy_send_finished(ll_sm_t* ll,ll_internal_connection_ctrl_t* connParam)
{

}
_RAM_CODE static int peri_conn_event_phy_receive_finished(ll_sm_t* ll,ll_internal_connection_ctrl_t* connParam)
{

}
_RAM_CODE static int peri_conn_event_phy_receive_timeout(ll_sm_t* ll,ll_internal_connection_ctrl_t* connParam)
{

}
_RAM_CODE static int peri_conn_event_default_process(ll_sm_t* ll,ll_internal_connection_ctrl_t* connParam)
{

}
static conn_event_sm_t peri_conn_event_state_machine[] =
{
	{peri_conn_event_sch_start,           CONN_SM_STATE_IDLE,     CONN_SM_STATE_RECEIVING, CONN_SM_STATE_IDLE,LL_SCH_EVENT_START},
	{peri_conn_event_sch_stop,            CONN_SM_STATE_IDLE,     CONN_SM_STATE_IDLE,      CONN_SM_STATE_IDLE,LL_SCH_EVENT_STOP},
    {peri_conn_event_sch_passed,          CONN_SM_STATE_IDLE,     CONN_SM_STATE_IDLE,      CONN_SM_STATE_IDLE,LL_SCH_EVENT_CANCELED},
    {peri_conn_event_sch_canceled,        CONN_SM_STATE_IDLE,     CONN_SM_STATE_IDLE,      CONN_SM_STATE_IDLE,LL_SCH_EVENT_PASSED},

    {peri_conn_event_phy_receive_finished,CONN_SM_STATE_RECEIVING,CONN_SM_STATE_SENDING,   CONN_SM_STATE_IDLE,LL_PHY_EVENT_RECEIVE_FINISHED},
    {peri_conn_event_phy_receive_timeout, CONN_SM_STATE_RECEIVING,CONN_SM_STATE_IDLE,      CONN_SM_STATE_IDLE,LL_PHY_EVENT_RECEIVE_TIMEOUT},
    {peri_conn_event_default_process,     CONN_SM_STATE_RECEIVING,CONN_SM_STATE_IDLE,      CONN_SM_STATE_IDLE,LL_SCH_EVENT_STOP},

    {peri_conn_event_phy_send_finished,   CONN_SM_STATE_SENDING,  CONN_SM_STATE_RECEIVING, CONN_SM_STATE_IDLE,LL_PHY_EVENT_SEND_FINISHED},
    {peri_conn_event_default_process,     CONN_SM_STATE_SENDING,  CONN_SM_STATE_IDLE,      CONN_SM_STATE_IDLE,LL_SCH_EVENT_STOP},
};

_RAM_CODE static int cen_conn_event_sch_start(ll_sm_t* ll,ll_internal_connection_ctrl_t* connParam)
{

}
_RAM_CODE static int cen_conn_event_sch_stop(ll_sm_t* ll,ll_internal_connection_ctrl_t* connParam)
{

}
_RAM_CODE static int cen_conn_event_sch_passed(ll_sm_t* ll,ll_internal_connection_ctrl_t* connParam)
{

}
_RAM_CODE static int cen_conn_event_sch_canceled(ll_sm_t* ll,ll_internal_connection_ctrl_t* connParam)
{

}
_RAM_CODE static int cen_conn_event_phy_send_finished(ll_sm_t* ll,ll_internal_connection_ctrl_t* connParam)
{

}
_RAM_CODE static int cen_conn_event_phy_receive_finished(ll_sm_t* ll,ll_internal_connection_ctrl_t* connParam)
{

}
_RAM_CODE static int cen_conn_event_phy_receive_timeout(ll_sm_t* ll,ll_internal_connection_ctrl_t* connParam)
{

}
static conn_event_sm_t cen_conn_event_state_machine[] =
{
	{cen_conn_event_sch_start,   CONN_SM_STATE_IDLE,CONN_SM_STATE_SENDING,  CONN_SM_STATE_IDLE,LL_SCH_EVENT_START},
	{cen_conn_event_sch_stop,    CONN_SM_STATE_IDLE,CONN_SM_STATE_IDLE,     CONN_SM_STATE_IDLE,LL_SCH_EVENT_STOP},
    {cen_conn_event_sch_passed,  CONN_SM_STATE_IDLE,CONN_SM_STATE_IDLE,     CONN_SM_STATE_IDLE,LL_SCH_EVENT_CANCELED},
    {cen_conn_event_sch_canceled,CONN_SM_STATE_IDLE,CONN_SM_STATE_IDLE,     CONN_SM_STATE_IDLE,LL_SCH_EVENT_PASSED},
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
#else
int ble_ll_enter_connection_state(ble_ll_event_e event)
{
    return 0;
}
#endif/*BLE_SUPPORT_CONN*/