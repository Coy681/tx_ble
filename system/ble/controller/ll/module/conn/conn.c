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

/************************************************ conn pdu prepare ***************************************************/

/******************** conn pdu category *******************/
#define CONN_LAST_PDU    0
#define CONN_NEW_PDU     1

/******************** conn pdu type ***********************/
#define CONN_CTRL_PDU    0
#define CONN_DATA_PDU    1
#define CONN_EMPTY_PDU   2

_RAM_CODE static void conn_prepare_pdu(ll_sm_t* ll,ll_internal_connection_ctrl_t* connParam,_u8 type)
{
	int md = 0;
	if(type == CONN_LAST_PDU)
	{
		if(connParam->lastPduType!=CONN_EMPTY_PDU)
		{
			ll_acl_packet_make(ll->phy.txAddress,connParam->nesn,connParam->sn,0);
			return;
		}
	}
	else
	{
		if(connParam->ctrl.out.nodeCnt!=0&&(!connParam->ctrl.pending))
		{
			ll->phy.txAddress = (_u8*)connParam->ctrl.out.popNodeInOrder(&connParam->ctrl.out);
			if((connParam->ctrl.out.nodeCnt!=0) || (connParam->data.out.rbCnt(&connParam->data.out)!=0))
			{
				md = 1;
			}
			ll_acl_packet_make(ll->phy.txAddress,connParam->nesn,connParam->sn,md);
			connParam->lastPduType = CONN_CTRL_PDU;
		}
		else if(!connParam->data.out.isEmpty(&connParam->data.out))
		{
			conn_prepare_data_pdu(ll,connParam);;//new control pdu
			ll->phy.txAddress = (_u8*)connParam->data.out.getReadPtr(&connParam->data.out);
			if((connParam->ctrl.out.nodeCnt!=0&&(!connParam->ctrl.pending))\
			|| (connParam->data.out.rbCnt(&connParam->data.out)>1))
			{
				md = 1;
			}
			ll_acl_packet_make(ll->phy.txAddress,connParam->nesn,connParam->sn,md);
			connParam->lastPduType = CONN_DATA_PDU;
		}
	}
	ll->phy.txAddress = ll_get_shared_phy_tx_address();
	ll_acl_packet_data_prepare(ll->phy.txAddress,0,LL_LLID_CONTINUE_OR_EMPTY_PDU);
	ll_acl_packet_make(ll->phy.txAddress,connParam->nesn,connParam->sn,md);
	connParam->lastPduType = CONN_EMPTY_PDU;
}

/************************************************ conn pdu prepare ***************************************************/
_RAM_CODE static void conn_prepare_phy(ll_sm_t* ll,ll_internal_connection_ctrl_t* connParam,_u32 timestamp,phy_dir_e phyDir)
{
	/**
	 *  phy parameters not assign here:
	 *  - access code  :assigned in conn stage
	 *  - crc init     :assigned in conn stage
	 *  - rx timeout   :assigned in conn stage,maybe changed by window widen.
	 *  - phy mode     :assigned in conn stage,and maybe update in connection.
	 *  - rx max octets:assigned in conn stage,and maybe update in connection.
	 *  - tx/rx address:maybe change each event,assigned in api 'conn_prepare_new_pdu'
	 *
	 *  phy parameters assignd here:
	 *  - chnIdx       :change each event,shall be assigned here
	 *  - dir          :maybe change each event,so need be assigned every time
	 *  - timestamp    :change every time
	 */
	/* calculate current event channel index*/
	connParam->csa.counter = connParam->eventCounter;
	ll->phy.chnIdx = ll_csa_cal_channel_index(&connParam->csa);
	/* phy tx or rx */
	ll->phy.dir = phyDir;
	/* phy timestamp */
	ll->phy.timestamp = timestamp;
	ll->phy.mode = (phyDir == PHY_DIR_TX?connParam->txPhyMode:connParam->rxPhyMode);
}

#if defined (BLE_SUPPORT_PER)

_RAM_CODE static int peri_conn_prepare_next_event_schedule(ll_sm_t* ll,ll_internal_connection_ctrl_t* connParam,_u32 anchor)
{
	_u32 windowWiden   = ll_ca_cal_window_winden(LL_CA_TYPE_SLEEP,connParam->peer.sca,0,((ll_conn_peri_t*)connParam->info)->lastSyncTime,anchor+connParam->duration);
	ll->sch.timestamp  = connParam->anchor - windowWiden;
	ll->sch.duration   = connParam->duration + 2*windowWiden;
	connParam->timeout = windowWiden+200;
}

_RAM_CODE static int peri_conn_event_sch_start(ll_sm_t* ll,ll_internal_connection_ctrl_t* connParam)
{
    phy_obj_cast(&ll->phy);
	connParam->eventCounter++;
	ll->phy.rxAddress = ll_get_shared_phy_rx_address();
	conn_prepare_phy(ll,connParam,ll->sch.timestamp,PHY_DIR_RX);
	ll->phy.start();
	return 1;
}
_RAM_CODE static int peri_conn_event_sch_stop(ll_sm_t* ll,ll_internal_connection_ctrl_t* connParam)
{
	ll->phy.stop();
	connParam->anchor  += connParam->interval;
	peri_conn_prepare_next_event_schedule(ll,connParam,connParam->anchor);
	return 1;
}
_RAM_CODE static int peri_conn_event_sch_passed(ll_sm_t* ll,ll_internal_connection_ctrl_t* connParam)
{
	_u32 currentTime        = system_time();
    _u32 periodicInt        = (currentTime - connParam->anchor)/connParam->interval;
    _u32 periodicRemain     = (currentTime - connParam->anchor)%connParam->interval;
    connParam->eventCounter+=  periodicInt;
	connParam->anchor      += (periodicInt*connParam->interval);
    _u32 windowWiden        = ll_ca_cal_window_winden(LL_CA_TYPE_SLEEP,connParam->peer.sca,0,((ll_conn_peri_t*)connParam->info)->lastSyncTime,connParam->anchor+connParam->duration);
	if(periodicRemain<(windowWiden+ll->sch.startLatency))
	{
	    connParam->eventCounter++;
		connParam->anchor      += connParam->interval;
	}
	peri_conn_prepare_next_event_schedule(ll,connParam,connParam->anchor);
	return 1;
}
_RAM_CODE static int peri_conn_event_sch_canceled(ll_sm_t* ll,ll_internal_connection_ctrl_t* connParam)
{
    connParam->eventCounter++;
    connParam->anchor += connParam->interval;
	peri_conn_prepare_next_event_schedule(ll,connParam,connParam->anchor);
	return 1;
}
_RAM_CODE static int peri_conn_event_phy_send_finished(ll_sm_t* ll,ll_internal_connection_ctrl_t* connParam)
{
	//need extended event?
	if(connParam->peer.md)
	{
		_u32 targetTime = system_time()+500;//need optimize
		if(sch_task_extended(targetTime))
		{
			//prepare phy rx schedule by last sent packet
			ll_acl_packet_t* pdu = (ll_acl_packet_t*)(ll->phy.txAddress + ll_get_packet_header_offset_from_address(PHY_DIR_TX));
			connParam->connPhyTs = connParam->connPhyTs + ll_get_air_packet_time(connParam->txPhyMode,pdu->hdr.length,connParam->enc)+connParam->tifs_pc;;
			conn_prepare_phy(ll,connParam,connParam->connPhyTs,PHY_DIR_RX);
			//start receive
			ll->phy.start();
			return 1;
		}
		else
		{
			sch_stop_task_early();
			return 0;
		}
	}
}
_RAM_CODE static int peri_conn_event_phy_receive_finished(ll_sm_t* ll,ll_internal_connection_ctrl_t* connParam)
{
	if(ll->phy.hw_is_rx_packet_valid())
	{
		ll_acl_packet_t* pdu = (ll_acl_packet_t*)(ll->phy.rxAddress + ll_get_packet_header_offset_from_address(PHY_DIR_RX));
		connParam->peer.sn   = pdu->hdr.sn;
		connParam->peer.nesn = pdu->hdr.nesn;
		connParam->peer.md   = pdu->hdr.md;

		//if there is enough space to store packet,then allow central send next packet
		if((pdu->hdr.llId == LL_LLID_CONTROL_PDU&&(connParam->ctrl.in.freeCnt != 0))\
	     ||(pdu->hdr.llId != LL_LLID_CONTROL_PDU&&(!connParam->data.in.isFull(&connParam->data.in))))
		{
			connParam->nesn = (~connParam->nesn);
		}
		//prepare next packet
		if(connParam->peer.nesn != connParam->sn)
		{
			if(connParam->lastPduType == CONN_CTRL_PDU)
			{
				connParam->ctrl.out.freeNode(&connParam->ctrl.out,ll->phy.txAddress);
			}
			else if(connParam->lastPduType == CONN_DATA_PDU)
			{
				connParam->data.out.moveReadPtr(&connParam->data.out);
			}
			connParam->sn = (~connParam->sn);
			conn_prepare_pdu(ll,connParam,CONN_NEW_PDU);
		}
		else
		{
			conn_prepare_pdu(ll,connParam,CONN_LAST_PDU);
		}
		((ll_conn_peri_t*)connParam->info)->lastSyncTime = ll->phy.hw_get_rx_air_ts;
		connParam->connPhyTs = ll->phy.hw_get_rx_air_ts + ll_get_air_packet_time(connParam->rxPhyMode,pdu->hdr.length,connParam->enc)+connParam->tifs_cp;
		//prepare phy tx schedule by receiving packet
		conn_prepare_phy(ll,connParam,connParam->connPhyTs,PHY_DIR_TX);
		ll->phy.start();
		//packet push
		if(pdu->hdr.length!=0)
		{
			_u8* packetAddr = NULL;
			if(pdu->hdr.llId == LL_LLID_CONTROL_PDU)
			{
				packetAddr = connParam->ctrl.in.allocNode(&connParam->ctrl.in,0);
			}
			else
			{
				packetAddr = connParam->data.in.getWritePtr(&connParam->data.in);;
			}
			txMemcpy(packetAddr,(_u8*)&pdu->hdr,2+pdu->hdr.length+(connParam->enc?4:0));
		}
		return 1;
	}
	else//even if crc invalid,peripheral shall send packet
	{
		sch_stop_task_early();
		return 0;
	}
}
_RAM_CODE static int peri_conn_event_phy_receive_timeout(ll_sm_t* ll,ll_internal_connection_ctrl_t* connParam)
{
	ll->phy.stop();
	sch_stop_task_early();
	return 1;
}
_RAM_CODE static int peri_conn_event_default_process(ll_sm_t* ll,ll_internal_connection_ctrl_t* connParam)
{
	ll->phy.stop();
	return 1;
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

/*
 * return 0 means analyze failed,shall send unknow pdu
 * return 1 means analyze success
 */
static int connect_update_ind_process(_u8* data)
{
	ll_ctrlPdu_connection_update_ind_t *ind = (ll_ctrlPdu_connection_update_ind_t*)data;
	return 1;
}

conn_ctrl_pdu_process_t ctrlPduProcess[] = 
{
	{LL_CTRL_PDU_CONNECTION_UPDATE_IND,sizeof(ll_ctrlPdu_connection_update_ind_t),connect_update_ind_process}
};

static void conn_ctrl_pdu_process(ll_sm_t* ll,ll_internal_connection_ctrl_t* connParam)
{
	if(connParam->ctrl.in.nodeCnt)
	{
		ll_acl_packet_t* pdu = (ll_acl_packet_t*)connParam->ctrl.in.popNodeInOrder(&connParam->ctrl.in);
		//maybe need decrypt？
		int ret = 0;
		for(int i=0;i<(sizeof(ctrlPduProcess)/sizeof(ctrlPduProcess[0]));i++)
		{
			if(ctrlPduProcess[i].opcode == pdu->data[0]\
			 &&POINTER_VALID(ctrlPduProcess[i].process)\
			 &&ctrlPduProcess[i].length == (pdu->hdr.length-1))
			{
				ret = ctrlPduProcess[i].process(&pdu->data[1]);
			}
		}
		connParam->ctrl.in.freeNode(&connParam->ctrl.in,(_u8*)pdu);
		if(ret==0)
		{
			//unknow pdu
		}
	}
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

		tx_bl_init(&ll->conn->ctrl.in,BLE_PER_MAX_RX_OCTETS,BLE_MAX_CTRL_PACKET_NUM);
		tx_bl_init(&ll->conn->ctrl.out,BLE_PER_MAX_TX_OCTETS,BLE_MAX_CTRL_PACKET_NUM);
		tx_rb_init(&ll->conn->data.in,BLE_PER_MAX_RX_OCTETS,BLE_PER_PACKET_NUMBER);
		tx_rb_init(&ll->conn->data.out,BLE_PER_MAX_TX_OCTETS,BLE_PER_PACKET_NUMBER);

		ll->conn->info = tx_malloc(sizeof(ll_conn_peri_t));
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

		tx_bl_init(&ll->conn->ctrl.in,BLE_CEN_MAX_RX_OCTETS,BLE_MAX_CTRL_PACKET_NUM);
		tx_bl_init(&ll->conn->ctrl.out,BLE_CEN_MAX_TX_OCTETS,BLE_MAX_CTRL_PACKET_NUM);
		tx_rb_init(&ll->conn->data.in,BLE_CEN_MAX_RX_OCTETS,BLE_CEN_PACKET_NUMBER);
		tx_rb_init(&ll->conn->data.out,BLE_CEN_MAX_TX_OCTETS,BLE_CEN_PACKET_NUMBER);
		ll->conn->info = tx_malloc(sizeof(ll_conn_cen_t));
    	ll->conn->role = CONN_ROLE_CENTRAL;
		#endif

    }
    else 
    {
        return 0;
    }

    ll->phy.hw_irq_cb     = conn_phy_irq_callback;
    ll->sch.cb            = conn_sch_callback;
    ll->conn->tifs_cp     = 150;
    ll->conn->tifs_pc     = 150;
	LOG_TRACE(LL_LOG_TRACE,"enter connection state",0,0)
    return 1;
}
#else
int ble_ll_enter_connection_state(ble_ll_event_e event)
{
    return 0;
}
#endif/*BLE_SUPPORT_CONN*/
