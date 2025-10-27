#include"conn.h"
#include"../../channel/channel.h"
#if defined(BLE_SUPPORT_CONN)

#define BLE_MAX_CTRL_PACKET_NUM       4

typedef int(*conn_event_sm_cb)(ll_sm_t* ll,ll_internal_connection_ctrl_t* connParam);

typedef enum
{
	CONN_STATE_IDLE,
	CONN_STATE_SYNCING,
	CONN_STATE_CONNECTED,
}conn_state_e;

  typedef enum
{
	CONN_SM_STATE_IDLE,
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


#if defined (BLE_SUPPORT_PER)
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
#endif/*BLE_SUPPORT_PER*/

#if defined (BLE_SUPPORT_CEN)
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
#endif/*BLE_SUPPORT_CEN*/


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
		#if defined(BLE_SUPPORT_PER)
    	/*default tx and rx max octet is 27,and max time is 328us in 1M
    	 *before LL DATA Length Update procedure,ll can only use 27 octets and 328us
    	 */
		ll->conn->data.maxTxOctets    = 27;
		ll->conn->data.maxTxTime      = 328;
		ll->conn->data.maxRxOctets    = 27;
		ll->conn->data.maxRxTime      = 328;

		ll->conn->data.ownMaxTxOctets = BLE_PER_MAX_TX_OCTETS;
		ll->conn->data.ownMaxTxTime   = ll_get_air_packet_time(ll->phy.mode,BLE_PER_MAX_TX_OCTETS,1);
		ll->conn->data.ownMaxRxOctets = BLE_PER_MAX_RX_OCTETS;
		ll->conn->data.ownMaxRxTime   = ll_get_air_packet_time(ll->phy.mode,BLE_PER_MAX_RX_OCTETS,1);

		tx_bl_init(&ll->conn->data.ctrlIn,BLE_PER_MAX_RX_OCTETS,BLE_MAX_CTRL_PACKET_NUM);
		tx_bl_init(&ll->conn->data.ctrlOut,BLE_PER_MAX_TX_OCTETS,BLE_MAX_CTRL_PACKET_NUM);
		tx_rb_init(&ll->conn->data.dataIn,BLE_PER_MAX_RX_OCTETS,BLE_PER_PACKET_NUMBER);
		tx_rb_init(&ll->conn->data.dataOut,BLE_PER_MAX_TX_OCTETS,BLE_PER_PACKET_NUMBER);

		ll->conn->role = CONN_ROLE_PERIPHERAL;
		#endif
    }
    else if(BLE_LL_STATE_INITIATING == ll->state)
    {
		#if defined(BLE_SUPPORT_CEN)
    	/*default tx and rx max octet is 27,and max time is 328us in 1M
    	 *before LL DATA Length Update procedure,ll can only use 27 octets and 328us
    	 */
		ll->conn->data.maxTxOctets    = 27;
		ll->conn->data.maxTxTime      = 328;
		ll->conn->data.maxRxOctets    = 27;
		ll->conn->data.maxRxTime      = 328;

		ll->conn->data.ownMaxTxOctets = BLE_CEN_MAX_TX_OCTETS;
		ll->conn->data.ownMaxTxTime   = ll_get_air_packet_time(ll->phy.mode,BLE_CEN_MAX_TX_OCTETS,1);
		ll->conn->data.ownMaxRxOctets = BLE_CEN_MAX_RX_OCTETS;
		ll->conn->data.ownMaxRxTime   = ll_get_air_packet_time(ll->phy.mode,BLE_CEN_MAX_RX_OCTETS,1);

		tx_bl_init(&ll->conn->data.ctrlIn,BLE_CEN_MAX_RX_OCTETS,BLE_MAX_CTRL_PACKET_NUM);
		tx_bl_init(&ll->conn->data.ctrlOut,BLE_CEN_MAX_TX_OCTETS,BLE_MAX_CTRL_PACKET_NUM);
		tx_rb_init(&ll->conn->data.dataIn,BLE_CEN_MAX_RX_OCTETS,BLE_CEN_PACKET_NUMBER);
		tx_rb_init(&ll->conn->data.dataOut,BLE_CEN_MAX_TX_OCTETS,BLE_CEN_PACKET_NUMBER);

    	ll->conn->role = CONN_ROLE_CENTRAL;
		#endif

    }
    else 
    {
        return 0;
    }

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
