
#include"adv.h"
/*****************************************ADV State Machine***********************************************/
typedef enum
{
    ADV_SM_SCH_EVENT_BASE              = 0x10,
    ADV_SM_SCH_EVENT_START             = ADV_SM_SCH_EVENT_BASE+SCH_TASK_START,
    ADV_SM_SCH_EVENT_STOP              = ADV_SM_SCH_EVENT_BASE+SCH_TASK_STOP,             
    ADV_SM_SCH_EVENT_CANCELED          = ADV_SM_SCH_EVENT_BASE+SCH_TASK_CANCELED,
    ADV_SM_SCH_EVENT_PASSED            = ADV_SM_SCH_EVENT_BASE+SCH_TASK_PASSED,

    ADV_SM_PHY_EVENT_BASE              = 0x20,
    ADV_SM_PHY_EVENT_SEND_FINISHED     = ADV_SM_PHY_EVENT_BASE+PHY_IRQ_TX_FINISHED,
    ADV_SM_PHY_EVENT_RECEIVE_FINISHED  = ADV_SM_PHY_EVENT_BASE+PHY_IRQ_RX_FINISHED,
    ADV_SM_PHY_EVENT_RECEIVE_TIMEOUT   = ADV_SM_PHY_EVENT_BASE+PHY_IRQ_RX_TIMEOUT,
}adv_sm_event_e;

typedef enum
{
    ADV_SM_STATE_IDLE,
    ADV_SM_STATE_SENDING_ADV,
    ADV_SM_STATE_SENDING_RSP,
    ADV_SM_STATE_RECEIVING,
    #if(LL_SUPPORT_LE_EXTENDED_ADVERTISING==1)
    ADV_SM_STATE_SENDING_AUX_ADV,
    ADV_SM_STATE_SENDING_AUX_SCAN_RSP,
    ADV_SM_STATE_SENDING_AUX_CONNECT_RSP,
    ADV_SM_STATE_SENDING_AUX_CHAIN_ADV,
    ADV_SM_STATE_RECEIVING_AUX,
    #endif
    #if(LL_SUPPORT_LE_PERIODIC_ADVERTISING==1)
    #endif

    #if(LL_SUPPORT_PERIODIC_ADVERTISING_WITH_RESPONSES_ADVERTISER==1)
    #endif
}adv_sm_state_e;

typedef enum
{
    SM_SCH_EVENT = BIT(1),
    SM_PHY_EVENT = BIT(2),
}sm_event_type_e;

typedef enum
{
    ADV_CONTEXT_DEFAULT     = BIT(0),
    ADV_CONTEXT_SCANNABLE   = BIT(1),
    ADV_CONTEXT_CONNECTABLE = BIT(2),
    ADV_CONTEXT_AUXILIARY   = BIT(3),
}adv_context_e;

typedef int(*adv_event_sm_cb)(ll_ctrl_t* ll,ll_internal_adv_param_t* advParam,_u32 conetxt);
typedef struct _PACKED
{
    adv_event_sm_cb cb;
    adv_context_e   context;
    adv_sm_state_e  currentState;
    adv_sm_state_e  transSuccessState;
    adv_sm_state_e  transFailState;
    adv_sm_event_e  event;
}adv_event_sm_t; 

/*****************************************ADV Procedure ***********************************************/
typedef struct
{
    adv_event_class_e eventClass;
    adv_event_sm_t*   sm;
    _u32              listLen;
    _u32              context;
}adv_procedure_list_t;

/*****************************************ADV Sequence ***********************************************/
typedef struct
{
    adv_event_type_e       eventType;//adv mode
    adv_procedure_list_t   *procedureList;
    _u32                   listLen;
}adv_sequence_t;

#define ADV_PROCEDURE_LIST_LENGTH(adv_procedure_list)      (sizeof(adv_procedure_list)/sizeof(adv_procedure_list[0]))
#define ADV_SEQUENCE_LIST_LENGTH(adv_sequence_list)        (sizeof(adv_sequence_list)/sizeof(adv_sequence_list[0]))
#define ADV_SM_LIST_LENGTH(adv_sm_list)                    (sizeof(adv_sm_list)/sizeof(adv_sm_list[0]))

_RAM_CODE 
static  ll_internal_adv_param_t* adv_get_current_entity(ll_ctrl_t* ll,_u8* class)
{
	#if(LL_SUPPORT_LE_EXTENDED_ADVERTISING!=1)
	return &ll->adv->param[0];
	#else
	return &ll->adv->param[0];
	#endif
}



/*****************************************ADV Event Process ***********************************************/
_RAM_CODE
static void adv_event_prepare_packet(ll_ctrl_t* ll,ll_internal_adv_param_t* advParam)
{
	_u8* packet = NULL;
	switch(advParam->eventType)
	{
		case ADV_EVENT_CONNECTABLE_SCANNABLE_UNDIRECTED:
		     packet = ll_get_adv_packet(ll->txSharedPacket,6+advParam->dataLen,LL_ADV_TYPE_ADV_IND,0,advParam->ownAddressType?1:0,0);
		     txMemcpy(((adv_type_ind_t*)packet)->advA,ll->ownAddr,6);
		     txMemcpy(((adv_type_ind_t*)packet)->advData,advParam->data,advParam->dataLen);
		     break;
        case ADV_EVENT_CONNECTABLE_DIRECTED:
			 packet = ll_get_adv_packet(ll->txSharedPacket,12,LL_ADV_TYPE_ADV_DIRECT_IND,0,advParam->ownAddressType?1:0,advParam->peerAddressType?1:0);
		     txMemcpy(((adv_type_direct_ind_t*)packet)->advA,ll->ownAddr,6);
		     txMemcpy(((adv_type_direct_ind_t*)packet)->targetA,advParam->peerAddress,6);
			 break;
		case ADV_EVENT_SCANNABLE_UNDIRECTED:
		     packet = ll_get_adv_packet(ll->txSharedPacket,6+advParam->dataLen,LL_ADV_TYPE_ADV_SCAN_IND,0,advParam->ownAddressType?1:0,0);
		     txMemcpy(((adv_type_scan_ind_t*)packet)->advA,ll->ownAddr,6);
		     txMemcpy(((adv_type_scan_ind_t*)packet)->advData,advParam->data,advParam->dataLen);
		     break;
		case ADV_EVENT_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED:
		     packet = ll_get_adv_packet(ll->txSharedPacket,6+advParam->dataLen,LL_ADV_TYPE_ADV_NONCONN_IND,0,advParam->ownAddressType?1:0,0);
		     txMemcpy(((adv_type_nonConn_ind_t*)packet)->advA,ll->ownAddr,6);
		     txMemcpy(((adv_type_nonConn_ind_t*)packet)->advData,advParam->data,advParam->dataLen);
		     break;
	}
}

_RAM_CODE
static void scan_rsp_prepare_packet(ll_ctrl_t* ll,ll_internal_adv_param_t* advParam)
{
	_u8* packet = NULL;
	packet = ll_get_adv_packet(ll->txSharedPacket,6+advParam->scanRspDataLen,LL_ADV_TYPE_SCAN_RSP,0,advParam->ownAddressType?1:0,0);
	txMemcpy(((adv_type_scan_rsp_t*)packet)->advA,ll->ownAddr,6);
	txMemcpy(((adv_type_scan_rsp_t*)packet)->scanRsp,advParam->scanRspData,advParam->scanRspDataLen);
}

/**
 *
 * @param timestamp - if present,timestamp is air packet time,so need minus hardware prepare time
 * 				    - if not present,packet send time is not specified,so we can use system time.
 */
_RAM_CODE
static void adv_event_prepare_phy(ll_ctrl_t* ll,ll_internal_adv_param_t* advParam,_u32 timestamp,phy_dir_e phydir,phy_mode_e mode)
{
    phy_obj_cast(&ll->phy);
    ll->phy.accessCode = BLE_ADV_ACCESS_CODE;
    ll->phy.crcInit    = BLE_ADV_CRC_INIT;
    ll->phy.mode       = mode;
    ll->phy.dir        = phydir;
    ll->phy.chnIdx     = advParam->currentChn;
    if(timestamp)
    {
        ll->phy.timestamp  = timestamp - ll->phy.hw_get_prepare_time();
    }
    else
    {
        ll->phy.timestamp  = system_time();
    }

    if(phydir == PHY_DIR_TX)
    {
        ll->phy.txAddress  = ll->txSharedPacket;
    }
    else if(phydir == PHY_DIR_RX)
    {
    	ll->phy.rxAddress = ll->rxSharedPacket;
        ll->phy.rxTimeout = BLE_ADV_DEFAULT_RX_TIMEOUT_US;
        ll->phy.rxMaxOctets = BLE_ADV_DEFAULT_MAX_LENGTH;
    }
}

_RAM_CODE
static void adv_event_process_next_task(ll_ctrl_t* ll,ll_internal_adv_param_t* advParam)
{
    if(advParam->availableChnCnt)
    {
    	advParam->availableChnCnt--;
    }
    if(advParam->availableChnCnt)
    {
        ll->sch.timestamp += (ll->sch.duration+ll->sch.startLatency+ll->sch.stopLatency);
    }
    else
    {
        ll->sch.timestamp += (ll->sch.period + 30*(random_byte()|0x0f));
        advParam->availableChnCnt = advParam->channelCnt;
    }
}

_RAM_CODE
static int adv_event_step_phy_send_advertising(ll_ctrl_t* ll,ll_internal_adv_param_t* advParam,_u32 property)
{
    if(advParam->availableChnCnt==0)
    {
        return 0;
    }
    advParam->instant++;
    advParam->currentChn = advParam->chnTable[(advParam->instant%advParam->channelCnt)];
    adv_event_prepare_packet(ll,advParam);
    adv_event_prepare_phy(ll,advParam,0,PHY_DIR_TX,advParam->phyMode);
    ll->phy.start();
    return 1;
}

_RAM_CODE
static int adv_event_step_phy_start_listen(ll_ctrl_t* ll,ll_internal_adv_param_t* advParam,_u32 property)
{
    if(property&(ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE))//connectable or scannable adv event both shall can start listen
    {
        adv_event_prepare_phy(ll,advParam,0,PHY_DIR_RX,advParam->phyMode);
        ll->phy.start();
        return 1;
    }
    return 0;
}

_RAM_CODE
static int adv_event_step_received_packet_analyze(ll_ctrl_t* ll)
{
	 ll_adv_packet_t* packet = (ll_adv_packet_t*)(ll->phy.rxAddress + ll_get_packet_header_offset_from_address(PHY_DIR_RX));

	 if(packet->hdr.pduType == LL_ADV_TYPE_SCAN_REQ && packet->hdr.length == sizeof(scan_type_scan_req_t))
	 {
		 //scan req process
		 scan_type_scan_req_t* scanReq = (scan_type_scan_req_t*)(packet->data);
		 if(txMemcmp(ll->ownAddr,scanReq->advA,6) == 0)
		 {
			 return 1;
		 }
	 }
	 else if(packet->hdr.pduType == LL_ADV_TYPE_CONNECT_IND && packet->hdr.length == sizeof(init_type_connectInd_t))
	 {
		 //connect ind process
		 init_type_connectInd_t* connInd = (init_type_connectInd_t*)(packet->data);
		 //ll state machine transform
	 }
	return 0;
}

_RAM_CODE
static int adv_event_step_phy_send_scan_rsp(ll_ctrl_t* ll,ll_internal_adv_param_t* advParam,_u32 property)
{
	if((adv_event_step_received_packet_analyze(ll)!=0)&&(property&ADV_CONTEXT_SCANNABLE))//packet analyze shall be execute first
	{
        scan_rsp_prepare_packet(ll,advParam);
        _u32 timestamp = ll->phy.hw_get_rx_air_ts() + ll_get_air_packet_time(advParam->phyMode,sizeof(scan_type_scan_req_t),0)+PACKET_DEFAULT_TIFS_TIME;
        adv_event_prepare_phy(ll,advParam,timestamp,PHY_DIR_TX,advParam->phyMode);
        ll->phy.start();
        return 1;
	}
	return 0;
}

_RAM_CODE
static int adv_event_step_sch_stop(ll_ctrl_t* ll,ll_internal_adv_param_t* advParam,_u32 property)
{
	ll->phy.stop();
	advParam->state = ADV_SM_STATE_IDLE;
    adv_event_process_next_task(ll,advParam);
    return 1;
}

_RAM_CODE
static int adv_event_step_sch_passed(ll_ctrl_t* ll,ll_internal_adv_param_t* advParam,_u32 property)
{
	_u32 systemTime = system_time();
	_u32 periodicDiff = (systemTime - ll->sch.timestamp)/ll->sch.period;
	ll->sch.timestamp+=((periodicDiff+1)*ll->sch.period);
	advParam->availableChnCnt = advParam->channelCnt;
    return 1;
    //todo,maybe we can jump to next adv channel
}

_RAM_CODE
static int adv_event_step_sch_canceled(ll_ctrl_t* ll,ll_internal_adv_param_t* advParam,_u32 property)
{
	_u32 systemTime = system_time();
	_u32 periodicDiff = (systemTime - ll->sch.timestamp)/ll->sch.period;
	ll->sch.timestamp+=((periodicDiff+1)*ll->sch.period);
	advParam->availableChnCnt = advParam->channelCnt;
    return 1;
    //todo,maybe we can jump to next adv channel
}

_RAM_CODE
static int adv_event_step_phy_send_rsp_finished(ll_ctrl_t* ll,ll_internal_adv_param_t* advParam,_u32 property)
{
    return 1;
}


static adv_event_sm_t adv_event_state_machine[]=
{
    {adv_event_step_phy_send_advertising, ADV_CONTEXT_DEFAULT|ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE,ADV_SM_STATE_IDLE,       ADV_SM_STATE_SENDING_ADV, ADV_SM_STATE_IDLE, ADV_SM_SCH_EVENT_START},
    {adv_event_step_sch_stop,             ADV_CONTEXT_DEFAULT|ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE,ADV_SM_STATE_IDLE,       ADV_SM_STATE_IDLE,        ADV_SM_STATE_IDLE, ADV_SM_SCH_EVENT_STOP},
    {adv_event_step_sch_canceled,         ADV_CONTEXT_DEFAULT|ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE,ADV_SM_STATE_IDLE,       ADV_SM_STATE_IDLE,        ADV_SM_STATE_IDLE, ADV_SM_SCH_EVENT_CANCELED},
    {adv_event_step_sch_passed,           ADV_CONTEXT_DEFAULT|ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE,ADV_SM_STATE_IDLE,       ADV_SM_STATE_IDLE,        ADV_SM_STATE_IDLE, ADV_SM_SCH_EVENT_PASSED},

    {adv_event_step_phy_start_listen,     ADV_CONTEXT_DEFAULT|ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE,ADV_SM_STATE_SENDING_ADV,ADV_SM_STATE_RECEIVING,   ADV_SM_STATE_IDLE, ADV_SM_PHY_EVENT_SEND_FINISHED},
    {adv_event_step_sch_stop,             ADV_CONTEXT_DEFAULT|ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE,ADV_SM_STATE_SENDING_ADV,ADV_SM_STATE_IDLE,        ADV_SM_STATE_IDLE, ADV_SM_SCH_EVENT_STOP},

    {adv_event_step_phy_send_scan_rsp,    ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE,                    ADV_SM_STATE_RECEIVING,  ADV_SM_STATE_SENDING_RSP, ADV_SM_STATE_IDLE, ADV_SM_PHY_EVENT_RECEIVE_FINISHED},
    {adv_event_step_sch_stop,             ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE,                    ADV_SM_STATE_RECEIVING,  ADV_SM_STATE_IDLE,        ADV_SM_STATE_IDLE, ADV_SM_SCH_EVENT_STOP},

    {adv_event_step_phy_send_rsp_finished,ADV_CONTEXT_SCANNABLE,                                            ADV_SM_STATE_SENDING_RSP,ADV_SM_STATE_IDLE,        ADV_SM_STATE_IDLE, ADV_SM_PHY_EVENT_SEND_FINISHED},
    {adv_event_step_sch_stop,             ADV_CONTEXT_SCANNABLE,                                            ADV_SM_STATE_SENDING_RSP,ADV_SM_STATE_IDLE,        ADV_SM_STATE_IDLE, ADV_SM_SCH_EVENT_STOP},
};
static adv_procedure_list_t adv_con_scan_undirected_procedure[] =
{
    {ADV_EVENT,adv_event_state_machine,ADV_SM_LIST_LENGTH(adv_event_state_machine),ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE},
};

static adv_procedure_list_t adv_con_directed_procedure[]=
{
    {ADV_EVENT,adv_event_state_machine,ADV_SM_LIST_LENGTH(adv_event_state_machine),ADV_CONTEXT_CONNECTABLE},
};

static adv_procedure_list_t adv_scan_undirected_procedure[]=
{
    {ADV_EVENT,adv_event_state_machine,ADV_SM_LIST_LENGTH(adv_event_state_machine),ADV_CONTEXT_SCANNABLE},
};

static adv_procedure_list_t adv_non_con_non_scan_undirected_procedure[]=
{
    {ADV_EVENT,adv_event_state_machine,ADV_SM_LIST_LENGTH(adv_event_state_machine),ADV_CONTEXT_DEFAULT},
};

/*****************************************ADV Extended Event Process ***********************************************/
#if(LL_SUPPORT_LE_EXTENDED_ADVERTISING==1)
ll_internal_adv_param_t* ll_extended_adv_get_entity(_u8 handle,_u8 allocate)
{
    ll_ctrl_t* ll     = ll_get_current_state_machine();
	for(int i=0;i<BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS;i++)
	{
		if(ll->adv->param[i].handle == handle)
		{
			return &ll->adv->param[i];
		}
	}
	if(allocate==0)
	{
		return NULL;
	}
	for(int i=0;i<BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS;i++)
	{
        if(ll->adv->param[i].handle == LL_EXTENDED_ADV_INVALID_HANDLE)
        {
            return &ll->adv->param[i];
        }
    }
    return NULL;
}

int ll_extended_adv_get_current_active_set_number(void)
{
    ll_ctrl_t* ll = ll_get_current_state_machine();
    _u8 count = 0;
	for(int i=0;i<BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS;i++)
	{
        if(ll->adv->param[i].handle!=LL_EXTENDED_ADV_INVALID_HANDLE && ll->adv->param[i].enable == 1)
        {
            count++;
        }
    }
    return count;
}

int ll_extended_adv_get_current_set_number(void)
{
    ll_ctrl_t* ll = ll_get_current_state_machine();
    _u8 count = 0;
	for(int i=0;i<BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS;i++)
	{
        if(ll->adv->param[i].handle!=0xFF)
        {
            count++;
        }
    }
    return count;
}


static int adv_extended_event_step_phy_send_aux_advertising(ll_ctrl_t* ll,ll_internal_adv_param_t* advParam,_u32 property)
{
	return 1;
}
static int adv_extended_event_step_phy_send_chain_advertising(ll_ctrl_t* ll,ll_internal_adv_param_t* advParam,_u32 property)
{
	return 1;
}
static int adv_extended_event_step_phy_start_listen_aux(ll_ctrl_t* ll,ll_internal_adv_param_t* advParam,_u32 property)
{
	return 1;
}
static int adv_extended_event_step_phy_send_aux_scan_rsp(ll_ctrl_t* ll,ll_internal_adv_param_t* advParam,_u32 property)
{
	return 1;
}
static int adv_extended_event_step_phy_send_aux_connect_rsp(ll_ctrl_t* ll,ll_internal_adv_param_t* advParam,_u32 property)
{
	return 1;
}
static int adv_extended_event_step_sch_stop(ll_ctrl_t* ll,ll_internal_adv_param_t* advParam,_u32 property)
{

}
static int adv_extended_event_step_sch_canceled(ll_ctrl_t* ll,ll_internal_adv_param_t* advParam,_u32 property)
{

}
static int adv_extended_event_step_sch_passed(ll_ctrl_t* ll,ll_internal_adv_param_t* advParam,_u32 property)
{

}
static adv_event_sm_t adv_extended_event_state_machine[]= 
{
    {adv_extended_event_step_phy_send_aux_advertising,  ADV_CONTEXT_AUXILIARY|ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE,ADV_SM_STATE_IDLE,                  ADV_SM_STATE_SENDING_AUX_ADV,        ADV_SM_STATE_IDLE, ADV_SM_SCH_EVENT_START},
    {adv_extended_event_step_sch_stop,                  ADV_CONTEXT_AUXILIARY|ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE,ADV_SM_STATE_IDLE,                  ADV_SM_STATE_IDLE,                   ADV_SM_STATE_IDLE, ADV_SM_SCH_EVENT_STOP},
    {adv_extended_event_step_sch_canceled,              ADV_CONTEXT_AUXILIARY|ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE,ADV_SM_STATE_IDLE,                  ADV_SM_STATE_IDLE,                   ADV_SM_STATE_IDLE, ADV_SM_SCH_EVENT_CANCELED},
    {adv_extended_event_step_sch_passed,                ADV_CONTEXT_AUXILIARY|ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE,ADV_SM_STATE_IDLE,                  ADV_SM_STATE_IDLE,                   ADV_SM_STATE_IDLE, ADV_SM_SCH_EVENT_PASSED},

    {adv_extended_event_step_phy_start_listen_aux,      ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE,                      ADV_SM_STATE_SENDING_AUX_ADV,       ADV_SM_STATE_RECEIVING_AUX,          ADV_SM_STATE_IDLE, ADV_SM_PHY_EVENT_SEND_FINISHED},
    {adv_extended_event_step_sch_stop,                  ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE,                      ADV_SM_STATE_SENDING_AUX_ADV,       ADV_SM_STATE_IDLE,                   ADV_SM_STATE_IDLE, ADV_SM_SCH_EVENT_STOP},

    {adv_extended_event_step_phy_send_aux_scan_rsp,     ADV_CONTEXT_CONNECTABLE,                                            ADV_SM_STATE_RECEIVING_AUX,         ADV_SM_STATE_SENDING_AUX_SCAN_RSP,   ADV_SM_STATE_IDLE, ADV_SM_PHY_EVENT_RECEIVE_FINISHED},
    {adv_extended_event_step_phy_send_aux_connect_rsp,  ADV_CONTEXT_SCANNABLE,                                              ADV_SM_STATE_RECEIVING_AUX,         ADV_SM_STATE_SENDING_AUX_CONNECT_RSP,ADV_SM_STATE_IDLE, ADV_SM_PHY_EVENT_RECEIVE_FINISHED},
    {adv_extended_event_step_sch_stop,                  ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE,                      ADV_SM_STATE_RECEIVING_AUX,         ADV_SM_STATE_IDLE,                   ADV_SM_STATE_IDLE, ADV_SM_SCH_EVENT_STOP},

    {adv_extended_event_step_phy_send_chain_advertising,ADV_CONTEXT_AUXILIARY,                                              ADV_SM_STATE_SENDING_AUX_ADV,       ADV_SM_STATE_SENDING_AUX_CHAIN_ADV,  ADV_SM_STATE_IDLE, ADV_SM_PHY_EVENT_SEND_FINISHED},
    {adv_extended_event_step_sch_stop,                  ADV_CONTEXT_AUXILIARY,                                              ADV_SM_STATE_SENDING_AUX_ADV,       ADV_SM_STATE_IDLE,                   ADV_SM_STATE_IDLE, ADV_SM_SCH_EVENT_STOP},
    {adv_extended_event_step_sch_canceled,              ADV_CONTEXT_AUXILIARY,                                              ADV_SM_STATE_SENDING_AUX_ADV,       ADV_SM_STATE_IDLE,                   ADV_SM_STATE_IDLE, ADV_SM_SCH_EVENT_CANCELED},
    {adv_extended_event_step_sch_passed,                ADV_CONTEXT_AUXILIARY,                                              ADV_SM_STATE_SENDING_AUX_ADV,       ADV_SM_STATE_IDLE,                   ADV_SM_STATE_IDLE, ADV_SM_SCH_EVENT_PASSED},

    {adv_extended_event_step_phy_send_chain_advertising,ADV_CONTEXT_AUXILIARY,                                              ADV_SM_STATE_SENDING_AUX_CHAIN_ADV, ADV_SM_STATE_SENDING_AUX_CHAIN_ADV,  ADV_SM_STATE_IDLE, ADV_SM_PHY_EVENT_SEND_FINISHED},
    {adv_extended_event_step_sch_stop,                  ADV_CONTEXT_AUXILIARY,                                              ADV_SM_STATE_SENDING_AUX_CHAIN_ADV, ADV_SM_STATE_IDLE,                   ADV_SM_STATE_IDLE, ADV_SM_SCH_EVENT_STOP},
    {adv_extended_event_step_sch_canceled,              ADV_CONTEXT_AUXILIARY,                                              ADV_SM_STATE_SENDING_AUX_CHAIN_ADV, ADV_SM_STATE_IDLE,                   ADV_SM_STATE_IDLE, ADV_SM_SCH_EVENT_CANCELED},
    {adv_extended_event_step_sch_passed,                ADV_CONTEXT_AUXILIARY,                                              ADV_SM_STATE_SENDING_AUX_CHAIN_ADV, ADV_SM_STATE_IDLE,                   ADV_SM_STATE_IDLE, ADV_SM_SCH_EVENT_PASSED},
};
static adv_procedure_list_t adv_extended_con_undirected_procedure[]=
{
    {ADV_EVENT,         adv_event_state_machine,         ADV_SM_LIST_LENGTH(adv_event_state_machine),         ADV_CONTEXT_DEFAULT},
    {ADV_EXTENDED_EVENT,adv_extended_event_state_machine,ADV_SM_LIST_LENGTH(adv_extended_event_state_machine),ADV_CONTEXT_CONNECTABLE},
};

static adv_procedure_list_t adv_extended_con_directed_procedure[]=
{
    {ADV_EVENT,         adv_event_state_machine,         ADV_SM_LIST_LENGTH(adv_event_state_machine),         ADV_CONTEXT_DEFAULT},
    {ADV_EXTENDED_EVENT,adv_extended_event_state_machine,ADV_SM_LIST_LENGTH(adv_extended_event_state_machine),ADV_CONTEXT_CONNECTABLE},
};

static adv_procedure_list_t adv_extended_scan_undirected_procedure[]=
{
    {ADV_EVENT,         adv_event_state_machine,         ADV_SM_LIST_LENGTH(adv_event_state_machine),         ADV_CONTEXT_DEFAULT},
    {ADV_EXTENDED_EVENT,adv_extended_event_state_machine,ADV_SM_LIST_LENGTH(adv_extended_event_state_machine),ADV_CONTEXT_SCANNABLE},
};

static adv_procedure_list_t adv_extended_scan_directed_procedure[]=
{
    {ADV_EVENT,         adv_event_state_machine,         ADV_SM_LIST_LENGTH(adv_event_state_machine),         ADV_CONTEXT_DEFAULT},
    {ADV_EXTENDED_EVENT,adv_extended_event_state_machine,ADV_SM_LIST_LENGTH(adv_extended_event_state_machine),ADV_CONTEXT_SCANNABLE},
};

static adv_procedure_list_t adv_extended_non_con_non_scan_undirected_procedure_with_auxiliary[]=
{
    {ADV_EVENT,         adv_event_state_machine,         ADV_SM_LIST_LENGTH(adv_event_state_machine),         ADV_CONTEXT_DEFAULT},
    {ADV_EXTENDED_EVENT,adv_extended_event_state_machine,ADV_SM_LIST_LENGTH(adv_extended_event_state_machine),ADV_CONTEXT_AUXILIARY},
};

static adv_procedure_list_t adv_extended_non_con_non_scan_directed_procedure_with_auxiliary[]=
{
    {ADV_EVENT,         adv_event_state_machine,         ADV_SM_LIST_LENGTH(adv_event_state_machine),         ADV_CONTEXT_DEFAULT},
    {ADV_EXTENDED_EVENT,adv_extended_event_state_machine,ADV_SM_LIST_LENGTH(adv_extended_event_state_machine),ADV_CONTEXT_AUXILIARY},
};

static adv_procedure_list_t adv_extended_non_con_non_scan_undirected_procedure_without_auxiliary[]=
{
    {ADV_EVENT,         adv_event_state_machine,         ADV_SM_LIST_LENGTH(adv_event_state_machine),         ADV_CONTEXT_DEFAULT},
};

static adv_procedure_list_t adv_extended_non_con_non_scan_directed_procedure_without_auxiliary[]=
{
    {ADV_EVENT,         adv_event_state_machine,         ADV_SM_LIST_LENGTH(adv_event_state_machine),         ADV_CONTEXT_DEFAULT},
};
#endif
/*****************************************ADV Periodic Event Process ***********************************************/
#if(LL_SUPPORT_LE_PERIODIC_ADVERTISING==1)
typedef enum
{
    ADV_PERIODIC_EVENT_NONE,
}adv_periodic_event_property_e;
static adv_event_sm_t adv_periodic_event_state_machine[]= 
{

};
static adv_procedure_list_t adv_extended_periodic_procedure[]=
{
    {ADV_EVENT,         adv_event_state_machine,         ADV_SM_LIST_LENGTH(adv_event_state_machine),         ADV_EVENT_NONE},
    {ADV_EXTENDED_EVENT,adv_extended_event_state_machine,ADV_SM_LIST_LENGTH(adv_extended_event_state_machine),ADV_EXTENDED_EVENT_NONE},
    {ADV_PERIODIC_EVENT,adv_periodic_event_state_machine,ADV_SM_LIST_LENGTH(adv_periodic_event_state_machine),ADV_PERIODIC_EVENT_NONE},
};
#endif
/*****************************************ADV Periodic With Rsp Process ***********************************************/
#if(LL_SUPPORT_PERIODIC_ADVERTISING_WITH_RESPONSES_ADVERTISER==1)
typedef enum
{
    ADV_PERIODIC_WITH_RSP_EVENT_NONE,
}adv_periodic_with_rsp_event_property_e;
static adv_event_sm_t adv_periodic_with_rsp_event_state_machine[]= 
{

};
static adv_procedure_list_t adv_extended_periodic_with_rsp_procedure[]=
{
    {ADV_EVENT,                  adv_event_state_machine,                  ADV_SM_LIST_LENGTH(adv_event_state_machine),                  ADV_EVENT_NONE},
    {ADV_EXTENDED_EVENT,         adv_extended_event_state_machine,         ADV_SM_LIST_LENGTH(adv_extended_event_state_machine),         ADV_EXTENDED_EVENT_NONE},
    {ADV_PERIODIC_WITH_RSP_EVENT,adv_periodic_with_rsp_event_state_machine,ADV_SM_LIST_LENGTH(adv_periodic_with_rsp_event_state_machine),ADV_PERIODIC_WITH_RSP_EVENT_NONE},
};
#endif

static adv_sequence_t advSequence[] = 
{
    {ADV_EVENT_CONNECTABLE_SCANNABLE_UNDIRECTED,                                    adv_con_scan_undirected_procedure,                                   ADV_PROCEDURE_LIST_LENGTH(adv_con_scan_undirected_procedure)},
    {ADV_EVENT_CONNECTABLE_DIRECTED,                                                adv_con_directed_procedure,                                          ADV_PROCEDURE_LIST_LENGTH(adv_con_directed_procedure)},
    {ADV_EVENT_SCANNABLE_UNDIRECTED,                                                adv_scan_undirected_procedure,                                       ADV_PROCEDURE_LIST_LENGTH(adv_scan_undirected_procedure)},
    {ADV_EVENT_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED,                            adv_non_con_non_scan_undirected_procedure,                           ADV_PROCEDURE_LIST_LENGTH(adv_non_con_non_scan_undirected_procedure)},
    #if(LL_SUPPORT_LE_EXTENDED_ADVERTISING==1)
    {ADV_EVENT_EXTENDED_CONNECTABLE_DIRECTED,                                       adv_extended_con_undirected_procedure,                               ADV_PROCEDURE_LIST_LENGTH(adv_extended_con_undirected_procedure)},
    {ADV_EVENT_EXTENDED_CONNECTABLE_UNDIRECTED,                                     adv_extended_con_directed_procedure,                                 ADV_PROCEDURE_LIST_LENGTH(adv_extended_con_directed_procedure)},
    {ADV_EVENT_EXTENDED_SCANNABLE_DIRECTED,                                         adv_extended_scan_undirected_procedure,                              ADV_PROCEDURE_LIST_LENGTH(adv_extended_scan_undirected_procedure)},
    {ADV_EVENT_EXTENDED_SCANNABLE_UNDIRECTED,                                       adv_extended_scan_directed_procedure,                                ADV_PROCEDURE_LIST_LENGTH(adv_extended_scan_directed_procedure)},
    {ADV_EVENT_EXTENDED_NON_CONNECTABLE_NON_SCANNABLE_DIRECTED_WITH_AUXILIARY,      adv_extended_non_con_non_scan_undirected_procedure_with_auxiliary,   ADV_PROCEDURE_LIST_LENGTH(adv_extended_non_con_non_scan_undirected_procedure_with_auxiliary)},
    {ADV_EVENT_EXTENDED_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED_WITH_AUXILIARY,    adv_extended_non_con_non_scan_directed_procedure_with_auxiliary,     ADV_PROCEDURE_LIST_LENGTH(adv_extended_non_con_non_scan_directed_procedure_with_auxiliary)},
    {ADV_EVENT_EXTENDED_NON_CONNECTABLE_NON_SCANNABLE_DIRECTED_WITHOUT_AUXILIARY,   adv_extended_non_con_non_scan_undirected_procedure_without_auxiliary,ADV_PROCEDURE_LIST_LENGTH(adv_extended_non_con_non_scan_undirected_procedure_without_auxiliary)},
    {ADV_EVENT_EXTENDED_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED_WITHOUT_AUXILIARY, adv_extended_non_con_non_scan_directed_procedure_without_auxiliary,  ADV_PROCEDURE_LIST_LENGTH(adv_extended_non_con_non_scan_directed_procedure_without_auxiliary)},
    #endif
    #if(LL_SUPPORT_LE_PERIODIC_ADVERTISING==1)
    {ADV_EVENT_EXTENDED_PERIODIC,                                 adv_extended_periodic_procedure,                   ADV_PROCEDURE_LIST_LENGTH(adv_extended_periodic_procedure)},
    #endif
    #if(LL_SUPPORT_PERIODIC_ADVERTISING_WITH_RESPONSES_ADVERTISER==1)
    {ADV_EVENT_EXTENDED_PERIODIC_WITH_RESPONSE,                   adv_extended_periodic_with_rsp_procedure,          ADV_PROCEDURE_LIST_LENGTH(adv_extended_periodic_with_rsp_procedure)},
    #endif
};

_RAM_CODE
static void adv_sequence_process(sm_event_type_e type,_u8 event)
{
    _u8 smEventType = 0; 
    if(type == SM_SCH_EVENT)
    {
        smEventType = ADV_SM_SCH_EVENT_BASE+event;
    }
    else
    {
    	smEventType = ADV_SM_PHY_EVENT_BASE+event;
    }
    _u8 eventClass = 0;
    ll_ctrl_t* ll     = ll_get_current_state_machine();
    ll_internal_adv_param_t* advParam = adv_get_current_entity(ll,&eventClass);

    _u8  eventType    = advParam->eventType;

    if(advSequence[eventType].listLen>eventClass)
    {
        for(_u8 i=0;i<advSequence[eventType].procedureList[eventClass].listLen;i++)
        {
            if(advParam->state  == advSequence[eventType].procedureList[eventClass].sm[i].currentState\
                && smEventType  == advSequence[eventType].procedureList[eventClass].sm[i].event\
				&& (advSequence[eventType].procedureList[eventClass].context&advSequence[eventType].procedureList[eventClass].sm[i].context))
            {

                if(advParam->processingEvent == smEventType)
                {
                    return;//reload same event,return
                }
                if(advSequence[eventType].procedureList[eventClass].sm[i].cb != NULL)
                {
                	advParam->processingEvent = smEventType;//use member value to avoid many entity conflict
                    int ret = advSequence[eventType].procedureList[eventClass].sm[i].cb(ll,advParam,advSequence[eventType].procedureList[eventClass].context);
                    if(ret)
                    {
                    	advParam->state = advSequence[eventType].procedureList[eventClass].sm[i].transSuccessState;
                    }
                    else
                    {
                    	advParam->state = advSequence[eventType].procedureList[eventClass].sm[i].transFailState;
                    }
                    advParam->processingEvent = 0;
                }
                break;               
                 
            }
        }
    }
}

_RAM_CODE
static void adv_phy_irq_callback(_u8 type)
{   
    if(type == PHY_IRQ_TX_FINISHED)
    {
        DEBUG_GPIO_HIGH(GPIO_8);
        DEBUG_GPIO_LOW(GPIO_8);
    }
    else if(type == PHY_IRQ_RX_FINISHED)
    {
        DEBUG_GPIO_HIGH(GPIO_9);
        DEBUG_GPIO_LOW(GPIO_9);
    }
    else if(type == PHY_IRQ_RX_TIMEOUT)
    {
        DEBUG_GPIO_HIGH(GPIO_10);
        DEBUG_GPIO_LOW(GPIO_10);
    }
    adv_sequence_process(SM_PHY_EVENT,type);
}

_RAM_CODE
static void adv_sch_callback(_u8 type)
{
    if(type == SCH_TASK_START)
    {
        DEBUG_GPIO_HIGH(GPIO_11);
        DEBUG_GPIO_LOW(GPIO_11);
    }
    else if(type == SCH_TASK_STOP)
    {
        DEBUG_GPIO_HIGH(GPIO_12);
        DEBUG_GPIO_LOW(GPIO_12);
    }
    else if(type == SCH_TASK_CANCELED)
    {
        DEBUG_GPIO_HIGH(GPIO_13);
        DEBUG_GPIO_LOW(GPIO_13);
    }
    else if(type == SCH_TASK_PASSED)
    {
        DEBUG_GPIO_HIGH(GPIO_14);
        DEBUG_GPIO_LOW(GPIO_14);
    }
    adv_sequence_process(SM_SCH_EVENT,type);
}

int ble_ll_enter_advertising_state(ble_ll_event_e event)
{
    if(event == BLE_LL_EVENT_START_ADVERTISING)
    {
        ll_ctrl_t* ll = ll_get_current_state_machine();
        if(ll->adv == NULL)
        {   
            return 0;
        }
		#if(LL_SUPPORT_LE_EXTENDED_ADVERTISING!=1)
        _u8 index = 0;
		#else
        _u8 index = 0;
		#endif
        //param init
        ll->adv->param[index].instant         = 0;
        ll->adv->param[index].availableChnCnt = ll->adv->param[index].channelCnt;
        ll->adv->param[index].phyMode         = PHY_MODE_1M;
        ll->adv->param[index].state           = ADV_SM_STATE_IDLE;
        ll->ownAddr[0] = ll->ownAddr[1] = ll->ownAddr[2] = ll->ownAddr[3]= ll->ownAddr[4] =ll->ownAddr[5]= 0x12;
        //phy init
        ll->phy.hw_irq_cb  = adv_phy_irq_callback;
        phy_obj_cast(&ll->phy);
        phy_obj_init(&ll->phy);
        //sch init
        ll->sch.llId = ll->id;
        ll->sch.type = SCH_PERIODIC_TASK;
        ll->sch.priority = LL_ADV_PRIORITY;
        ll->sch.timestamp = system_time() + 500;//maybe need planner
        ll->sch.period    = ll->adv->param[index].interval;
        ll->sch.startLatency = 100;
        ll->sch.stopLatency  = 75;
        ll->sch.cb = adv_sch_callback;
        if(ll->adv->param[index].eventType == ADV_EVENT_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED)
        {
            ll->sch.duration = ll->phy.hw_get_prepare_time()+ll_get_air_packet_time(ll->phy.mode,BLE_ADV_DEFAULT_MAX_LENGTH,0)+PACKET_DEFAULT_TIFS_TIME;
        }
        else
        {
            ll->sch.duration = ll->phy.hw_get_prepare_time()+3*ll_get_air_packet_time(ll->phy.mode,BLE_ADV_DEFAULT_MAX_LENGTH,0)+2*PACKET_DEFAULT_TIFS_TIME;
        }

        sch_message_t* message = (sch_message_t*)tx_message_allocate(8);
        message->eventType = SCHE_MESSAGE_TASK_ADD;
        message->message[0] = ((_u32)&ll->sch);
        message->message[1] = ((_u32)&ll->sch)>>8;
        message->message[2] = ((_u32)&ll->sch)>>16;
        message->message[3] = ((_u32)&ll->sch)>>24;
        tx_message_send(TX_TASK_ID_SCH,(_u8*)message);
        LOG_TRACE(LL_LOG_TRACE,"enter advertising state",0,0);
        return 1;
    }
    return 0;
}
