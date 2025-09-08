#include"ll.h"
#include"ll_internal.h"
#include"ll_config.h"
#include"system/task/message/message.h"
#include"system/task/task.h"
#include"system/task/event/event.h" 
#include"module/adv/adv.h"
#include"module/scan/scan.h"
#include"module/conn/conn.h"
#include"module/init/init.h"
#include"module/sync/sync.h"
#include"module/brd/brd.h"
#include"module/standby/standby.h"
/************************************ll implementation***************************************/
ll_t* ll;
static _u8 llSmConut;
static _u8 llCurrentSm;

typedef struct _PACKED
{
	ble_ll_state_e       currentState;
	ble_ll_state_e       nextState;
	ble_ll_event_e       event;
	ble_ll_event_cb      cb;
}ble_ll_state_table_t;


void ll_init_state_machine(_u8 number)
{
	llSmConut = number;
	if(number==0)
	{
		llSmConut = 1;
	}

	ll = (ll_t*)tx_malloc(sizeof(ll_t));
	for(_u8 i=0;i<6;i++)
	{
		ll->addr[i] = 0x12+i;//todo,temporary value
	}
	ll->sm = (ll_sm_t*)tx_malloc(number*sizeof(ll_sm_t));
	for(_u8 i=0;i<llSmConut;i++)
	{
		ll->sm[i].id  = i;
		ll->sm[i].state =  (_u8)BLE_LL_STATE_STANDBY;
	}
	llCurrentSm = ll->sm[0].id;
	phy_init();

}

_RAM_CODE
ll_sm_t* ll_get_idle_state_machine(void)
{
	for(_u8 i=0;i<llSmConut;i++)
	{
		if(ll->sm[i].state == (_u8)BLE_LL_STATE_STANDBY)
		{
			return &ll->sm[i];
		}
	}
	return NULL;
}

_RAM_CODE
ll_sm_t* ll_get_state_machine_by_id(_u8 id)
{
	for(_u8 i=0;i<llSmConut;i++)
	{
		if(ll->sm[i].id == id)
		{
			return &ll->sm[i];
		}
	}
	return NULL;
}

_RAM_CODE
ll_sm_t* ll_get_current_state_machine(void)
{
	return &ll->sm[llCurrentSm];
}

_RAM_CODE
_u8*     ll_get_device_address(void)
{
	return ll->addr;
}

_RAM_CODE
_u8*     ll_get_shared_phy_tx_address(void)
{
	return ll->txAddr;
}

_RAM_CODE
_u8*     ll_get_shared_phy_rx_address(void)
{
	return ll->rxAddr;
}


static const ble_ll_state_table_t ble_ll_state_table[]=
{
    //standby state transition
	{BLE_LL_STATE_STANDBY,        BLE_LL_STATE_ADVERTISING,      BLE_LL_EVENT_START_ADVERTISING,    ble_ll_enter_advertising_state},
	{BLE_LL_STATE_STANDBY,        BLE_LL_STATE_BROADCASTING,     BLE_LL_EVENT_START_BROADCASTING,   ble_ll_enter_broadcasting_state},
	{BLE_LL_STATE_STANDBY,        BLE_LL_STATE_SCANNING,         BLE_LL_EVENT_START_SCANNING,       ble_ll_enter_scanning_state},
	{BLE_LL_STATE_STANDBY,        BLE_LL_STATE_SYNCHRONIZATION,  BLE_LL_EVENT_START_SYNCHRONIZATION,ble_ll_enter_sychronization_state},
	{BLE_LL_STATE_STANDBY,	      BLE_LL_STATE_INITIATING     ,  BLE_LL_EVENT_START_INITIATING,     ble_ll_enter_initiating_state},

	//advertising state transition
	{BLE_LL_STATE_ADVERTISING,    BLE_LL_STATE_STANDBY,          BLE_LL_EVENT_STOP_ADVERTISING,     ble_ll_enter_standby_state},
	{BLE_LL_STATE_ADVERTISING,    BLE_LL_STATE_CONNECTION,       BLE_LL_EVENT_START_CONNECTION,     ble_ll_enter_connection_state},

	//broadcasting state transition
	{BLE_LL_STATE_BROADCASTING,   BLE_LL_STATE_STANDBY,          BLE_LL_EVENT_STOP_BROADCASTING,    ble_ll_enter_standby_state},

	//scanning state transition
	{BLE_LL_STATE_SCANNING,       BLE_LL_STATE_STANDBY,          BLE_LL_EVENT_STOP_SCANNING,        ble_ll_enter_standby_state},

	//synchronization state transition
	{BLE_LL_STATE_SYNCHRONIZATION,BLE_LL_STATE_STANDBY,          BLE_LL_EVENT_STOP_SYNCHRONIZATION, ble_ll_enter_standby_state},

	//initiating state transition
	{BLE_LL_STATE_INITIATING,     BLE_LL_STATE_STANDBY,          BLE_LL_EVENT_STOP_INITIATING,      ble_ll_enter_standby_state},
	{BLE_LL_STATE_INITIATING,     BLE_LL_STATE_CONNECTION,       BLE_LL_EVENT_START_CONNECTION,     ble_ll_enter_connection_state},

	//synchronization state transition
	{BLE_LL_STATE_CONNECTION,     BLE_LL_STATE_STANDBY,          BLE_LL_EVENT_STOP_CONNECTION,      ble_ll_enter_standby_state},
};

ble_ll_state_status_e ble_ll_process_event(ll_sm_t* sm,ble_ll_event_e event)
{
	if(sm==NULL)
	{
		LOG_TRACE(LL_LOG_TRACE,"error:sm null",0,0)
		return BLE_LL_STATE_INVALID_PARAMETER;
	}
	if(event>=BLE_LL_EVENT_MAX)
	{
		LOG_TRACE(LL_LOG_TRACE,"error:event invalid",(_u8*)&event,4)
		return BLE_LL_STATE_INVALID_PARAMETER;
	}

	for(_u8 i=0;i<ARRAY_SIZE(ble_ll_state_table);i++)
	{
		if(ble_ll_state_table[i].currentState == sm->state&&\
		   ble_ll_state_table[i].event        == event)
		{
			if(ble_ll_state_table[i].cb(event))
			{
				LOG_TRACE(LL_LOG_TRACE,"state transition success",(_u8*)&ble_ll_state_table[i].nextState,4)
				sm->state = ble_ll_state_table[i].nextState;
				return BLE_LL_STATE_SUCCESS;
			}
		}
	}
	LOG_TRACE(LL_LOG_TRACE,"state transition fail",0,0)
	LOG_TRACE(LL_LOG_TRACE,"current state",(_u8*)&sm->state,4)
	LOG_TRACE(LL_LOG_TRACE,"event",(_u8*)&event,4)
	return BLE_LL_STATE_TRANSITION_NOT_ALLOWED;
}

/*********************************ll feature implementation**********************************/
static _u64 ll_host_support_feature;

void ll_get_feature(_u8* feature,_u8 len)
{
	_u8 suppFea[]=
	{
		LL_FEATURE_SUPPORT_BYTE_0,
		LL_FEATURE_SUPPORT_BYTE_1,
		LL_FEATURE_SUPPORT_BYTE_2,
		LL_FEATURE_SUPPORT_BYTE_3,
		LL_FEATURE_SUPPORT_BYTE_4,
		LL_FEATURE_SUPPORT_BYTE_5,
		LL_FEATURE_SUPPORT_BYTE_6,
		LL_FEATURE_SUPPORT_BYTE_7,
		LL_FEATURE_SUPPORT_BYTE_8,
	};
	for(int i=0;i<len;i++)
	{
		feature[i] = suppFea[i];
	}
}

void ll_feature_reset(void)
{
	ll_host_support_feature = 0;
}
/******************************ll filter accept list operation*****************************/
controller_error_code_e ll_add_device_to_filter_accept_list(_u8 addrType,_u8* addr)
{
	ll_sm_t* llSm = ll_get_current_state_machine();
	if(POINTER_VALID(llSm->adv))
	{
		for(_u8 i=0;i<BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS;i++)
		{
			if((llSm->adv->param[i].enable)&&\
	          (llSm->adv->param[i].filterPolicy!=LL_FILTER_LIST_NOT_USE))
			{
				return COMMAND_DISALLOWED;
			}
		}
	}
	//todo,process initing and scanning
	for(_u8 i=0;i<BLE_FILTER_ACCEPT_LIST_SIZE;i++)
	{
		if((ll->filterAcceptList[i].occupy)&&\
		   (ll->filterAcceptList[i].addrType == addrType)&&\
		   (!txMemcmp(ll->filterAcceptList[i].addr,addr,6)))
		{
			return SUCCESS;
		}
	}
	for(_u8 i=0;i<BLE_FILTER_ACCEPT_LIST_SIZE;i++)
	{
		if(!(ll->filterAcceptList[i].occupy))
		{
			ll->filterAcceptList[i].addrType = addrType;
			txMemcpy(ll->filterAcceptList[i].addr,addr,6);
			return SUCCESS;
		}
	}
	return MEMORY_CAPACITY_EXCEEDED;
}
controller_error_code_e ll_remove_device_from_filter_accept_list(_u8 addrType,_u8* addr)
{
	ll_sm_t* llSm = ll_get_current_state_machine();
	if(POINTER_VALID(llSm->adv))
	{
		for(_u8 i=0;i<BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS;i++)
		{
			if((llSm->adv->param[i].enable)&&\
	          (llSm->adv->param[i].filterPolicy!=LL_FILTER_LIST_NOT_USE))
			{
				return COMMAND_DISALLOWED;
			}
		}
	}
	//todo,process initing and scanning
	for(_u8 i=0;i<BLE_FILTER_ACCEPT_LIST_SIZE;i++)
	{
		if((ll->filterAcceptList[i].occupy)&&\
		   (ll->filterAcceptList[i].addrType == addrType)&&\
		   (!txMemcmp(ll->filterAcceptList[i].addr,addr,6)))
		{
			ll->filterAcceptList[i].occupy = 0;
			return SUCCESS;
		}
	}
	return MEMORY_CAPACITY_EXCEEDED;
}
controller_error_code_e ll_clear_filter_accept_list(void)
{
	ll_sm_t* llSm = ll_get_current_state_machine();
	if(POINTER_VALID(llSm->adv))
	{
		for(_u8 i=0;i<BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS;i++)
		{
			if((llSm->adv->param[i].enable)&&\
	          (llSm->adv->param[i].filterPolicy!=LL_FILTER_LIST_NOT_USE))
			{
				return COMMAND_DISALLOWED;
			}
		}
	}
	//todo,process initing and scanning 
	for(_u8 i=0;i<BLE_FILTER_ACCEPT_LIST_SIZE;i++)
	{
		if(ll->filterAcceptList[i].occupy)
		{
			ll->filterAcceptList[i].occupy = 0;
		}	
	}
	return SUCCESS;
}

/******************************************ll reset*******************************************/
controller_error_code_e ll_reset(void)
{
	//ll feature reset
	ll_feature_reset();

    //ll reset process
    return SUCCESS;
}

controller_error_code_e ll_set_event_mask(_u64 eventMask)
{
	ll->eventMask = eventMask;
	return SUCCESS;
}

controller_error_code_e ll_set_le_event_mask(_u64 eventMask)
{
	ll->leEventMask = eventMask;
	return SUCCESS;
}
controller_error_code_e ll_set_random_address(_u8* addr)
{
	ll_sm_t* llSm = ll_get_current_state_machine();
	if(POINTER_VALID(llSm->adv))
	{
		if(llSm->adv->param[0].enable)
		{
			return COMMAND_DISALLOWED;
		}
	}
	//todo,process initing and scanning
	ll->addrType = 1;
	txMemcmp(ll->addr,addr,6);

}
/*************************************Bluetooth LE Advertising ******************************/


controller_error_code_e ll_set_advertising_parameters(_u16 interval,\
	                                                  ll_advertising_type_e type,\
													  ll_own_address_type_e ownAddressType,\
	                                                  ll_peer_address_type_e peerAddressType,\
													  _u8* peerAddress,_u8 channelMap,\
	                                                  ll_advertising_filter_policy_e policy)
{
	ll_sm_t* ll = ll_get_current_state_machine();
	_u8 index = 0;
	if(ll->adv == NULL)
	{
		ll->adv = (ll_internal_adv_ctrl_t*)tx_malloc(sizeof(ll_internal_adv_ctrl_t));
		ll->adv->param[index].la = (ll_adv_set_t*)tx_malloc(sizeof(ll_adv_set_t));
	}
	ll->adv->param[index].ownAddressType  = ownAddressType;
	ll->adv->param[index].peerAddressType = peerAddressType;
	ll->adv->param[index].filterPolicy = policy;
	txMemcpy(ll->adv->param[index].peerAddress,peerAddress,6);
	
	ll->adv->param[index].la->sch.interval = interval*625;
	ll->adv->param[index].la->channelCnt   = 0;
    for(int i=0;i<3;i++)
    {
    	if(channelMap&BIT(i))
    	{
    		ll->adv->param[index].la->chnTable[ll->adv->param[index].la->channelCnt++] = 37+i;
    	}
    }
	switch(type)
	{
		case LL_ADV_IND:
		{
			ll->adv->param[index].eventType = ADV_EVENT_CONNECTABLE_SCANNABLE_UNDIRECTED;
		}break;
		case LL_ADV_DIRECT_IND_HIGH_DUTY:
		case LL_ADV_DIRECT_IND_LOW_DUTY:
		{
			ll->adv->param[index].eventType = ADV_EVENT_CONNECTABLE_DIRECTED;
		}break;
		case LL_ADV_SCAN_IND:
		{
			ll->adv->param[index].eventType = ADV_EVENT_SCANNABLE_UNDIRECTED;
		}break;
		case LL_ADV_NONCONN_IND:
		{
			ll->adv->param[index].eventType = ADV_EVENT_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED;
		}break;
	}
	LOG_TRACE(1,"set adv param",0,0)
	return SUCCESS;
}

controller_error_code_e ll_set_advertising_data(_u8* data,_u8 length)
{
	ll_sm_t* ll = ll_get_current_state_machine();
	_u8 index = 0;
	if(ll->adv == NULL)
	{
		ll->adv = (ll_internal_adv_ctrl_t*)tx_malloc(sizeof(ll_internal_adv_ctrl_t));
		ll->adv->param[index].la = (ll_adv_set_t*)tx_malloc(sizeof(ll_adv_set_t));
	}
    if(ll->adv->param[index].data.addr)
	{
		tx_free(ll->adv->param[index].data.addr);
	}
    ll->adv->param[index].data.len  = length;
    ll->adv->param[index].data.addr = tx_malloc(length);
	txMemcpy(ll->adv->param[index].data.addr,data,length);
	LOG_TRACE(1,"set adv data",0,0)
	return SUCCESS;
}
	
controller_error_code_e ll_set_scan_response_data(_u8* data,_u8 length)
{
	ll_sm_t* ll = ll_get_current_state_machine();
	_u8 index = 0;
	if(ll->adv == NULL)
	{
		ll->adv = (ll_internal_adv_ctrl_t*)tx_malloc(sizeof(ll_internal_adv_ctrl_t));
		ll->adv->param[index].la = (ll_adv_set_t*)tx_malloc(sizeof(ll_adv_set_t));
	}
    if(ll->adv->param[index].scanRsp.addr)
	{
		tx_free(ll->adv->param[index].scanRsp.addr);
	}
    ll->adv->param[index].scanRsp.len  = length;
    ll->adv->param[index].scanRsp.addr = tx_malloc(length);
	txMemcpy(ll->adv->param[index].scanRsp.addr,data,length);
	LOG_TRACE(1,"set scan rsp data",0,0)
	return SUCCESS;
}

controller_error_code_e ll_set_advertising_enable(_u8 enable)
{
	ll_sm_t* ll = ll_get_current_state_machine();
	_u8 index = 0;
	//assert ll->adv == NULL
    if(ll->adv->param[index].enable == enable)
	{
		return SUCCESS;
	}
	LOG_TRACE(1,"set scan enable",&enable,1)
	if(ll->adv->param[index].enable!=enable)
	{
		ll->adv->param[index].enable = enable;
		if(ll->adv->param[index].enable == LL_ADVERTISING_ENABLE)
		{
	        phy_obj_cast(&ll->phy);
	        phy_obj_init(&ll->phy);
	        ll_internal_adv_param_t* advParam = &ll->adv->param[0];
			//sch init
			advParam->la->availableChnCnt     = advParam->la->channelCnt;
			advParam->la->eventCnt        = 0;
			advParam->la->sch.anchorPoint     = system_time() + 500;
			if(advParam->eventType == ADV_EVENT_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED)
			{
				advParam->la->sch.duration = ll->phy.hw_get_prepare_time()+ll_get_air_packet_time(PHY_MODE_1M,BLE_ADV_PRI_PHY_MAX_TX_LEN,0)+PACKET_DEFAULT_TIFS_TIME;
			}
			else
			{
				advParam->la->sch.duration = ll->phy.hw_get_prepare_time()+3*ll_get_air_packet_time(PHY_MODE_1M,BLE_ADV_PRI_PHY_MAX_TX_LEN,0)+2*PACKET_DEFAULT_TIFS_TIME;
			}
			advParam->la->sch.startMargin     = 100;
			advParam->la->sch.stopMargin      = 75;
			//phy init
			advParam->la->phy.mode            = PHY_MODE_1M;
			advParam->la->phy.crcInit         = BLE_ADV_CRC_INIT;
			advParam->la->phy.accessCode      = BLE_ADV_ACCESS_CODE;
			advParam->la->phy.rxMaxOctets     = BLE_ADV_PRI_PHY_MAX_TX_LEN;
			advParam->la->phy.rxAddress       = ll_get_shared_phy_rx_address();
			advParam->la->phy.txAddress       = ll_get_shared_phy_tx_address();
			if(BLE_LL_STATE_SUCCESS!=ble_ll_process_event(ll,BLE_LL_EVENT_START_ADVERTISING))
			{
				return IVALID_HCI_COMMAND_PARAMETERS;
			}
		}
		else//LL_ADVERTISING_DISABLE
		{
			if(BLE_LL_STATE_SUCCESS!=ble_ll_process_event(ll,BLE_LL_EVENT_STOP_ADVERTISING))
			{
				return IVALID_HCI_COMMAND_PARAMETERS;
			}
		}
	}
	return SUCCESS;
}

#if(LL_SUPPORT_LE_EXTENDED_ADVERTISING)
controller_error_code_e ll_set_extended_advertising_parameters(ll_extended_adv_param_t* pParam)
{
	ll_sm_t* ll = ll_get_current_state_machine();
	if(POINTER_NOT_VALID(ll->adv))
	{
		ll->adv = (ll_internal_adv_ctrl_t*)tx_malloc(sizeof(ll_internal_adv_param_t)*BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS);
		ASSERT(ll->adv != NULL);
		for(int i=0;i<BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS;i++)
		{
			ll->adv->param[i].handle = LL_EXTENDED_ADV_INVALID_HANDLE;
		}
	}

	ll_internal_adv_param_t* pAdv = ll_extended_adv_get_entity(pParam->advHandle,1);
	if(POINTER_NOT_VALID(pAdv))
	{
		return MEMORY_CAPACITY_EXCEEDED;
	}
	pAdv->eventProperty     = pParam->advEventProperty;
	pAdv->filterPolicy      = pParam->filterPolicy;
	pAdv->ownAddressType    = pParam->ownAddrType;
	pAdv->peerAddressType   = pParam->peerAddrType;
	pAdv->la->sch.interval  = pParam->primaryAdvInterval * 625;
	pAdv->la->channelCnt    = 0;
    for(int i=0;i<3;i++)
    {
    	if(pParam->primaryAdvChnMap&BIT(i))
    	{
    		pAdv->la->chnTable[pAdv->la->channelCnt++] = 37+i;
    	}
    }
	if(pParam->advEventProperty&LL_ADV_EVENT_PROPERTY_LEGACY_PDU)
	{
		switch(pParam->advEventProperty)
		{
			case (LL_ADV_EVENT_PROPERTY_LEGACY_PDU|LL_ADV_EVENT_PROPERTY_CONNECTED|LL_ADV_EVENT_PROPERTY_SCANNABLE):
			{
				pAdv->eventType = ADV_EVENT_CONNECTABLE_SCANNABLE_UNDIRECTED;
			}
				break;
			case (LL_ADV_EVENT_PROPERTY_LEGACY_PDU|LL_ADV_EVENT_PROPERTY_CONNECTED|LL_ADV_EVENT_PROPERTY_DIRECTED):
			case (LL_ADV_EVENT_PROPERTY_LEGACY_PDU|LL_ADV_EVENT_PROPERTY_CONNECTED|LL_ADV_EVENT_PROPERTY_DIRECTED|LL_ADV_EVENT_PROPERTY_HIGH_DUTY_CONNECTED):
			{
				//todo,high duty cycle
				pAdv->eventType = ADV_EVENT_CONNECTABLE_DIRECTED;
			}
				break;
			case (LL_ADV_EVENT_PROPERTY_LEGACY_PDU|LL_ADV_EVENT_PROPERTY_SCANNABLE):
			{
				pAdv->eventType = ADV_EVENT_SCANNABLE_UNDIRECTED;
			}
				break;
			case (LL_ADV_EVENT_PROPERTY_LEGACY_PDU):
			{
				pAdv->eventType = ADV_EVENT_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED;
			}
		}
	}
	else
	{
		if(POINTER_NOT_VALID(pAdv->ea))
		{
			pAdv->ea = (ll_adv_ea_set_t*)tx_malloc(sizeof(ll_adv_ea_set_t));
		}

		switch(pParam->advEventProperty&(LL_ADV_EVENT_PROPERTY_CONNECTED|LL_ADV_EVENT_PROPERTY_SCANNABLE|LL_ADV_EVENT_PROPERTY_DIRECTED))
		{
			case(LL_ADV_EVENT_PROPERTY_CONNECTED|LL_ADV_EVENT_PROPERTY_DIRECTED):
			{
				pAdv->eventType = ADV_EVENT_EXTENDED_CONNECTABLE_DIRECTED;
				pAdv->schMap |= ADV_SCH_MAP_AUX;
			}
				break;
			case(LL_ADV_EVENT_PROPERTY_CONNECTED):
			{
				pAdv->eventType = ADV_EVENT_EXTENDED_CONNECTABLE_UNDIRECTED;
				pAdv->schMap |= ADV_SCH_MAP_AUX;
			}
				break;
			case(LL_ADV_EVENT_PROPERTY_SCANNABLE|LL_ADV_EVENT_PROPERTY_DIRECTED):
			{
				pAdv->eventType = ADV_EVENT_EXTENDED_SCANNABLE_DIRECTED;
				pAdv->schMap |= ADV_SCH_MAP_AUX;
			}
				break;	
			case(LL_ADV_EVENT_PROPERTY_SCANNABLE):
			{
				pAdv->eventType = ADV_EVENT_EXTENDED_SCANNABLE_UNDIRECTED;
				pAdv->schMap |= ADV_SCH_MAP_AUX;
			}
				break;	
			case(LL_ADV_EVENT_PROPERTY_DIRECTED):
			{
				pAdv->eventType = ADV_EVENT_EXTENDED_NON_CONNECTABLE_NON_SCANNABLE_DIRECTED_WITHOUT_AUXILIARY;
			}
				break;	
			case(0):
			{
				pAdv->eventType = ADV_EVENT_EXTENDED_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED_WITHOUT_AUXILIARY;
			}
				break;
		}
		pAdv->ea->sid                  = pParam->advSid;
		pAdv->ea->scanReqNotifyEnable  = pParam->scanReqNotifyEnable;
		pAdv->ea->secondaryMaxSkip     = pParam->secondaryAdvMaxSkip;
		pAdv->ea->power  = 0;
		if(pParam->txPower!=0x7F)
		{
			pAdv->ea->power = pParam->txPower;
		}
		if(pParam->secondaryAdvPhy == LL_ADV_PHY_CODED)
		{
			if(pParam->secondaryAdvphyOptions == LL_ADV_PHY_OPTIONS_PREFER_S2||\
			pParam->secondaryAdvphyOptions == LL_ADV_PHY_OPTIONS_REQUIRE_S2)
			{
				pAdv->ea->phyMode = PHY_MODE_CODED_S2;
			}
			else if(pParam->secondaryAdvphyOptions == LL_ADV_PHY_OPTIONS_PREFER_S8||\
					pParam->secondaryAdvphyOptions == LL_ADV_PHY_OPTIONS_REQUIRE_S8)
			{
				pAdv->ea->phyMode = (_u8)PHY_MODE_CODED_S8;
			}
			else
			{
				pAdv->ea->phyMode = (_u8)PHY_MODE_CODED_S8;
			}
		}
		else if(pParam->secondaryAdvPhy == LL_ADV_PHY_1M)
		{
			pAdv->ea->phyMode = PHY_MODE_1M;
		}
		else 
		{
			pAdv->ea->phyMode = PHY_MODE_2M;
		}
	}
	if(pParam->primaryAdvPhy == LL_ADV_PHY_CODED)
	{
		if(pParam->primaryAdvphyOptions == LL_ADV_PHY_OPTIONS_PREFER_S2||\
	       pParam->primaryAdvphyOptions == LL_ADV_PHY_OPTIONS_REQUIRE_S2)
		{
			pAdv->la->phy.mode = PHY_MODE_CODED_S2;
		}
		else if(pParam->primaryAdvphyOptions == LL_ADV_PHY_OPTIONS_PREFER_S8||\
	            pParam->primaryAdvphyOptions == LL_ADV_PHY_OPTIONS_REQUIRE_S8)
		{
			pAdv->la->phy.mode = (_u8)PHY_MODE_CODED_S8;
		}
		else 
		{
			pAdv->la->phy.mode = (_u8)PHY_MODE_CODED_S8;
		}
	}
	else
	{
		pAdv->la->phy.mode = LL_ADV_PHY_1M;
	}
	return SUCCESS;
}

volatile int AAA_DATA_ADDRESS;
volatile int AAA_RSP_ADDRESS;
volatile int AAA_DATA_LEN;
volatile int AAA_RSP_LEN;
controller_error_code_e ll_set_extended_advertising_data(_u8 advHandle,\
														ll_advertising_data_operation_e operation,\
														ll_advertising_data_fragment_perference_e fragPref,\
														_u8 dataLen,\
														_u8* data)
{
	ll_internal_adv_param_t* pAdv = ll_extended_adv_get_entity(advHandle,0);
	if(POINTER_NOT_VALID(pAdv))
	{
		return UNKNOWN_ADVERTISING_IDENTIFIER;
	}
	if(pAdv->eventProperty & LL_ADV_EVENT_PROPERTY_LEGACY_PDU)
	{
		if(dataLen > 31||\
		   operation!=LL_ADV_DATA_OPERATION_COMPLETE||\
		   pAdv->eventType == ADV_EVENT_CONNECTABLE_DIRECTED)//direct pdu don't have advertising data
		{
			return IVALID_HCI_COMMAND_PARAMETERS;
		}
	}

	if(operation == LL_ADV_DATA_OPERATION_UNCHANGED)
	{
		if(pAdv->enable == 0||\
		   pAdv->data.len == 0||\
		   dataLen != 0)
		{
			return IVALID_HCI_COMMAND_PARAMETERS;
		}
	}

	if(operation!=LL_ADV_DATA_OPERATION_COMPLETE&&\
	   operation!=LL_ADV_DATA_OPERATION_UNCHANGED&&\
	   dataLen == 0)
	{
		return IVALID_HCI_COMMAND_PARAMETERS;
	}

	if(pAdv->enable)
	{
		if(operation!=LL_ADV_DATA_OPERATION_COMPLETE&&\
		   operation!=LL_ADV_DATA_OPERATION_UNCHANGED)
		{
			return COMMAND_DISALLOWED;
		}
	}

	if(pAdv->eventType == ADV_EVENT_EXTENDED_SCANNABLE_DIRECTED||\
	   pAdv->eventType == ADV_EVENT_EXTENDED_SCANNABLE_UNDIRECTED)
	{
		return IVALID_HCI_COMMAND_PARAMETERS;//mode don't have advertising data
	}

	if(dataLen > BLE_ADV_MAXIMUM_ADVERTISING_DATA_LENGTH)
	{
		return MEMORY_CAPACITY_EXCEEDED;
	}
	if(!(pAdv->eventProperty&LL_ADV_EVENT_PROPERTY_LEGACY_PDU))
	{
		if(ll_get_air_packet_time(pAdv->ea->phyMode,dataLen,0)>(pAdv->la->sch.interval*3/4))
		{
			return PACKET_TOO_LONG;
		}
	}
	static _u32 dataFillOffset = 0;
	if((operation == LL_ADV_DATA_OPERATION_INTERMEDIATE_FRAGMENT) || (operation == LL_ADV_DATA_OPERATION_LAST_FRAGMENT))
	{
		if((dataFillOffset+dataLen)>BLE_ADV_MAXIMUM_ADVERTISING_DATA_LENGTH)
		{
			dataFillOffset = 0;
			return MEMORY_CAPACITY_EXCEEDED;
		}
		if(ll_get_air_packet_time(pAdv->ea->phyMode,dataFillOffset+dataLen,0)>(pAdv->la->sch.interval*3/4))
		{
			dataFillOffset = 0;
			return PACKET_TOO_LONG;
		}
	}

	if(POINTER_NOT_VALID(pAdv->data.addr))
	{
		if(operation == LL_ADV_DATA_OPERATION_COMPLETE)
		{
			pAdv->data.addr = tx_malloc(dataLen);
		}
		else
		{
			pAdv->data.addr = tx_malloc(BLE_ADV_MAXIMUM_ADVERTISING_DATA_LENGTH);
		}
		AAA_DATA_ADDRESS = pAdv->data.addr;
	}
	
	if(pAdv->eventType == ADV_EVENT_EXTENDED_NON_CONNECTABLE_NON_SCANNABLE_DIRECTED_WITHOUT_AUXILIARY)
	{
		pAdv->eventType = ADV_EVENT_EXTENDED_NON_CONNECTABLE_NON_SCANNABLE_DIRECTED_WITH_AUXILIARY;
		pAdv->schMap |= ADV_SCH_MAP_AUX;
	}
	else if(pAdv->eventType == ADV_EVENT_EXTENDED_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED_WITHOUT_AUXILIARY)
	{
		pAdv->eventType = ADV_EVENT_EXTENDED_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED_WITH_AUXILIARY;
		pAdv->schMap |= ADV_SCH_MAP_AUX;
	}

	switch(operation)
	{
		case LL_ADV_DATA_OPERATION_INTERMEDIATE_FRAGMENT:
		{
			txMemcpy4(pAdv->data.addr+dataFillOffset,data,dataLen);
			dataFillOffset+=dataLen;
		}
			break;
		case LL_ADV_DATA_OPERATION_FIRST_FRAGMENT:
		{
			txMemcpy4(pAdv->data.addr,data,dataLen);
			dataFillOffset = dataLen;
		}
			break;	
		case LL_ADV_DATA_OPERATION_LAST_FRAGMENT:
		{
			txMemcpy4(pAdv->data.addr+dataFillOffset,data,dataLen);
			pAdv->ea->did++;
			pAdv->data.len = dataFillOffset + dataLen;
		}
			break;
		case LL_ADV_DATA_OPERATION_COMPLETE:
		{
			txMemcpy(pAdv->data.addr,data,dataLen);
			pAdv->ea->did++;
			pAdv->data.len = dataLen;
		}
			break;
		case LL_ADV_DATA_OPERATION_UNCHANGED:
		{
			pAdv->ea->did++;
		}
			break;
	}
	if((!(pAdv->eventProperty&LL_ADV_EVENT_PROPERTY_LEGACY_PDU))&&(operation == LL_ADV_DATA_OPERATION_LAST_FRAGMENT||operation == LL_ADV_DATA_OPERATION_COMPLETE))
	{
		if(POINTER_VALID(pAdv->ea->chain.entry))
		{
			tx_free((_u8*)pAdv->ea->chain.entry);
		}
		pAdv->ea->aux.data.addr= pAdv->data.addr;
		//process data fragment
		if(pAdv->data.len<=(BLE_ADV_SEC_PHY_MAX_TX_LEN - BLE_ADV_EXTENDED_HEADER_MAX_LEN))
		{
			pAdv->ea->chain.cnt= 0;//only aux packet exist
			pAdv->ea->aux.data.len = pAdv->data.len;

		}
		else
		{
			_u8 remainLen         = ((pAdv->data.len-(BLE_ADV_SEC_PHY_MAX_TX_LEN - BLE_ADV_EXTENDED_HEADER_MAX_LEN)))%BLE_ADV_SEC_PHY_MAX_TX_LEN;
			_u8 chainCnt          = ((pAdv->data.len-(BLE_ADV_SEC_PHY_MAX_TX_LEN - BLE_ADV_EXTENDED_HEADER_MAX_LEN)))/BLE_ADV_SEC_PHY_MAX_TX_LEN + (remainLen==0?0:1);
			pAdv->schMap         |= ADV_SCH_MAP_CHAIN;
			pAdv->ea->chain.entry = (ll_adv_entry_t*)tx_malloc(chainCnt*sizeof(ll_adv_entry_t));
			pAdv->ea->chain.cnt   = chainCnt;
			pAdv->ea->aux.data.len = BLE_ADV_SEC_PHY_MAX_TX_LEN - BLE_ADV_EXTENDED_HEADER_MAX_LEN;
			_u16 offset           = pAdv->ea->aux.data.len;
			for(_u8 i=0;i<chainCnt-1;i++)
			{
				pAdv->ea->chain.entry[i].data.len = BLE_ADV_SEC_PHY_MAX_TX_LEN;
				pAdv->ea->chain.entry[i].data.addr= (pAdv->data.addr+offset);
				offset+=BLE_ADV_SEC_PHY_MAX_TX_LEN;
			}
			if(chainCnt!=0)
			{
				pAdv->ea->chain.entry[chainCnt-1].data.len = remainLen;
				pAdv->ea->chain.entry[chainCnt-1].data.addr= (pAdv->data.addr+offset);
			}
		}	
	}
	AAA_DATA_LEN = pAdv->data.len;
	pAdv->ea->advDatafragPerf = fragPref;
	return SUCCESS;
}


controller_error_code_e ll_set_extended_scan_response_data(_u8 advHandle,\
															ll_advertising_data_operation_e operation,\
															ll_advertising_data_fragment_perference_e fragPref,\
															_u8 dataLen,\
															_u8* data)
{
	ll_internal_adv_param_t* pAdv = ll_extended_adv_get_entity(advHandle,0);
	if(POINTER_NOT_VALID(pAdv))
	{
		return UNKNOWN_ADVERTISING_IDENTIFIER;
	}

	if(pAdv->eventProperty & LL_ADV_EVENT_PROPERTY_LEGACY_PDU)
	{
		if(dataLen > 31||\
		   operation!=LL_ADV_DATA_OPERATION_COMPLETE||\
		   ((pAdv->eventType != ADV_EVENT_CONNECTABLE_SCANNABLE_UNDIRECTED)&&(pAdv->eventType != ADV_EVENT_SCANNABLE_UNDIRECTED)))
		{
			return IVALID_HCI_COMMAND_PARAMETERS;
		}
	}
	else 
	{
		if(pAdv->eventType != ADV_EVENT_EXTENDED_SCANNABLE_DIRECTED||\
		   pAdv->eventType != ADV_EVENT_EXTENDED_SCANNABLE_UNDIRECTED||\
		   pAdv->eventType != ADV_EVENT_CONNECTABLE_SCANNABLE_UNDIRECTED||\
		   pAdv->eventType != ADV_EVENT_SCANNABLE_UNDIRECTED)
		{
			return IVALID_HCI_COMMAND_PARAMETERS;
		}
	}

	if(operation!=LL_ADV_DATA_OPERATION_COMPLETE)
	{
		if(dataLen == 0)
		{
			return IVALID_HCI_COMMAND_PARAMETERS;
		}
		if(pAdv->enable)
		{
			return COMMAND_DISALLOWED;
		}
	}

	if(pAdv->enable)
	{
		if(dataLen == 0)
		{
			return COMMAND_DISALLOWED;
		}
	}

	if(dataLen > BLE_ADV_MAXIMUM_ADVERTISING_DATA_LENGTH)
	{
		return MEMORY_CAPACITY_EXCEEDED;
	}
	if(!(pAdv->eventProperty&LL_ADV_EVENT_PROPERTY_LEGACY_PDU))
	{
		if(ll_get_air_packet_time(pAdv->ea->phyMode,dataLen,0)>(pAdv->la->sch.interval*3/4))
		{
			return PACKET_TOO_LONG;
		}
	}
	static _u32 dataFillOffset = 0;
	if((operation == LL_ADV_DATA_OPERATION_INTERMEDIATE_FRAGMENT) || (operation == LL_ADV_DATA_OPERATION_LAST_FRAGMENT))
	{
		if((dataFillOffset+dataLen)>BLE_ADV_MAXIMUM_ADVERTISING_DATA_LENGTH)
		{
			dataFillOffset = 0;
			return MEMORY_CAPACITY_EXCEEDED;
		}
		if(ll_get_air_packet_time(pAdv->ea->phyMode,dataFillOffset+dataLen,0)>(pAdv->la->sch.interval*3/4))
		{
			return PACKET_TOO_LONG;
		}
	}
	if(POINTER_NOT_VALID(pAdv->scanRsp.addr))
	{
		if(operation == LL_ADV_DATA_OPERATION_COMPLETE)
		{
			pAdv->scanRsp.addr = tx_malloc(dataLen);
		}
		else
		{
			pAdv->scanRsp.addr = tx_malloc(BLE_ADV_MAXIMUM_ADVERTISING_DATA_LENGTH);
		}
		AAA_RSP_ADDRESS = pAdv->scanRsp.addr;
	}
	switch(operation)
	{
		case LL_ADV_DATA_OPERATION_INTERMEDIATE_FRAGMENT:
		{
			txMemcpy4(pAdv->scanRsp.addr+dataFillOffset,data,dataLen);
			dataFillOffset+=dataLen;
		}
			break;
		case LL_ADV_DATA_OPERATION_FIRST_FRAGMENT:
		{
			txMemcpy4(pAdv->scanRsp.addr,data,dataLen);
			dataFillOffset=dataLen;
		}
			break;	
		case LL_ADV_DATA_OPERATION_LAST_FRAGMENT:
		{
			txMemcpy4(pAdv->scanRsp.addr+dataFillOffset,data,dataLen);
			pAdv->scanRsp.len = dataFillOffset+dataLen;
			dataFillOffset = 0;
			
		}
			break;
		case LL_ADV_DATA_OPERATION_COMPLETE:
		{
			txMemcpy(pAdv->scanRsp.addr,data,dataLen);
			pAdv->scanRsp.len = dataLen;
		}
			break;
	}
	if(!(pAdv->eventProperty&LL_ADV_EVENT_PROPERTY_LEGACY_PDU)&&(operation == LL_ADV_DATA_OPERATION_LAST_FRAGMENT||operation == LL_ADV_DATA_OPERATION_COMPLETE))
	{
		if(POINTER_VALID(pAdv->ea->chain.entry))
		{
			tx_free((_u8*)pAdv->ea->chain.entry);
		}
		pAdv->ea->aux.data.addr= pAdv->scanRsp.addr;
		//process data fragment
		if(pAdv->scanRsp.len<=(BLE_ADV_SEC_PHY_MAX_TX_LEN - BLE_ADV_EXTENDED_HEADER_MAX_LEN))
		{
			pAdv->ea->chain.cnt= 0;//only aux packet exist
			pAdv->ea->aux.data.len = pAdv->scanRsp.len;

		}
		else
		{
			_u8 remainLen         = ((pAdv->scanRsp.len-(BLE_ADV_SEC_PHY_MAX_TX_LEN - BLE_ADV_EXTENDED_HEADER_MAX_LEN)))%BLE_ADV_SEC_PHY_MAX_TX_LEN;
			_u8 chainCnt          = ((pAdv->scanRsp.len-(BLE_ADV_SEC_PHY_MAX_TX_LEN - BLE_ADV_EXTENDED_HEADER_MAX_LEN)))/BLE_ADV_SEC_PHY_MAX_TX_LEN + (remainLen==0?0:1);
			pAdv->schMap         |= ADV_SCH_MAP_CHAIN;
			pAdv->ea->chain.entry = (ll_adv_entry_t*)tx_malloc(chainCnt*sizeof(ll_adv_entry_t));
			pAdv->ea->chain.cnt   = chainCnt;
			pAdv->ea->aux.data.len = BLE_ADV_SEC_PHY_MAX_TX_LEN - BLE_ADV_EXTENDED_HEADER_MAX_LEN;
			_u16 offset           = pAdv->ea->aux.data.len;
			for(_u8 i=0;i<chainCnt-1;i++)
			{
				pAdv->ea->chain.entry[i].data.len = BLE_ADV_SEC_PHY_MAX_TX_LEN;
				pAdv->ea->chain.entry[i].data.addr= (pAdv->scanRsp.addr+offset);
				offset+=BLE_ADV_SEC_PHY_MAX_TX_LEN;
			}
			if(chainCnt!=0)
			{
				pAdv->ea->chain.entry[chainCnt-1].data.len = remainLen;
				pAdv->ea->chain.entry[chainCnt-1].data.addr= (pAdv->scanRsp.addr+offset);
			}
		}
	}
	pAdv->ea->scanRspDatafragPerf = fragPref;
	AAA_RSP_LEN = pAdv->scanRsp.len;
	return SUCCESS;
}
/**
 * Advertising disabled condition
 * 1.Host send "ll_set_extended_advertising_enable" with 'enable' field set to 0
 * 2.A connection is created using the advertising set.
 * 3.The duration field expires
 * 4.The number of extended advertising events for the set exceed the MAX_EXTENDED_ADVERTISING_EVENT. 
 */
controller_error_code_e ll_set_extended_advertising_enable(_u8 enable,\
														   _u8 numSets,\
														   ll_extended_adv_enable_subField_e* pEnableSubFiled)
{
	//if the ll_set_extended_advertising_enable set again when the adv set is enabled,the timer for duration 
	//and the number of events counter are reset,any change to random address shall take effect


	//disable advertising set will not disable periodic advertising 

	//disable a disabled advertising set have no effect

	//if adv set is marked by more than one advHandle[i] parameter,should return error code 0x12
	if((enable != 0) && (numSets == 0))
	{
		return IVALID_HCI_COMMAND_PARAMETERS;
	}
	if(numSets>BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS)
	{
		return IVALID_HCI_COMMAND_PARAMETERS; 
	}
	ll_sm_t* ll = ll_get_current_state_machine();

	if((enable == 0) && (numSets == 0))
	{
		for(int i=0;i<BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS;i++)
		{
			if(ll->adv->param[i].handle <= 0xEF)
			{
				if((ll->adv->param[i].eventProperty & LL_ADV_EVENT_PROPERTY_SCANNABLE)&&ll->adv->param[i].scanRsp.len == 0)
				{	
					return COMMAND_DISALLOWED;
				}
				if((ll->adv->param[i].eventProperty & LL_ADV_EVENT_PROPERTY_CONNECTED)&&ll->adv->param[i].data.len == 0)
				{
					return IVALID_HCI_COMMAND_PARAMETERS;//?,need check if valid
				}
			}
		}
	}
	else 
	{
		for(_u8 i=0;i<numSets;i++)
		{	
			ll_internal_adv_param_t* pAdv = ll_extended_adv_get_entity(pEnableSubFiled[i].advHandle,0);
			if(POINTER_NOT_VALID(pAdv))
			{
				return IVALID_HCI_COMMAND_PARAMETERS; 
			}
			if((pAdv->eventProperty & LL_ADV_EVENT_PROPERTY_SCANNABLE)&&pAdv->scanRsp.len == 0)
			{	
				return COMMAND_DISALLOWED;
			}
			if((pAdv->eventProperty & LL_ADV_EVENT_PROPERTY_CONNECTED)&&pAdv->data.len == 0)
			{
				return IVALID_HCI_COMMAND_PARAMETERS;//?,need check if valid
			}
		}
	}
	if((enable == 0) && (numSets == 0))
	{
		for(int i=0;i<BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS;i++)
		{
			if(ll->adv->param[i].handle <= 0xEF)
			{
				ll->adv->param[i].enable   = enable;
				if(POINTER_VALID(ll->adv->param[i].ea))
				{
					if(pEnableSubFiled[i].duration!=0)
					{
						ll->adv->param[i].ea->expireTime =  system_time() + ll->adv->param[i].la->sch.interval + pEnableSubFiled[i].duration*10000;
					}
					else 
					{
						ll->adv->param[i].ea->expireTime = 0;
					}

					if(pEnableSubFiled[i].maxEvents!=0)
					{
						ll->adv->param[i].ea->maxEvents    = pEnableSubFiled[i].maxEvents;
						ll->adv->param[i].ea->eventCnt = 0;
					}
					else
					{
						ll->adv->param[i].ea->maxEvents = 0;
					}
				}
			}
		}
	}
	else
	{
		for(_u8 i=0;i<numSets;i++)
		{	
			ll_internal_adv_param_t* pAdv = ll_extended_adv_get_entity(pEnableSubFiled[i].advHandle,0);
			pAdv->enable = enable;
			if(pEnableSubFiled[i].duration!=0)
			{
				pAdv->ea->expireTime =  system_time() + pAdv->la->sch.interval + pEnableSubFiled[i].duration*10000;
			}
			else
			{
				pAdv->ea->expireTime = 0;
			}

			if(pEnableSubFiled[i].maxEvents!=0)
			{
				pAdv->ea->maxEvents    = pEnableSubFiled[i].maxEvents;
				pAdv->ea->eventCnt = 0;
			}
			else
			{
				pAdv->ea->maxEvents = 0;
			}
			if(enable == 1)
			{
				pAdv->schMap|=ADV_SCH_MAP_PRI;
				//la phy init
				pAdv->la->phy.crcInit         = BLE_ADV_CRC_INIT;
				pAdv->la->phy.accessCode      = BLE_ADV_ACCESS_CODE;
				pAdv->la->phy.rxMaxOctets     = BLE_ADV_SEC_PHY_MAX_TX_LEN;
                pAdv->la->phy.rxAddress       = ll_get_shared_phy_rx_address();
                pAdv->la->phy.txAddress       = ll_get_shared_phy_tx_address();
				//la sch init
				pAdv->la->availableChnCnt = 0;
				pAdv->la->availableChnCnt = pAdv->la->channelCnt;
				pAdv->la->eventCnt    = 0;
				pAdv->la->sch.startMargin = 100;
				pAdv->la->sch.stopMargin  = 100;
		        phy_obj_cast(&ll->phy);
		        phy_obj_init(&ll->phy);
                if(pAdv->eventProperty&LL_ADV_EVENT_PROPERTY_LEGACY_PDU)
                {
                    if(pAdv->eventType == ADV_EVENT_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED)
                    {
                    	pAdv->la->sch.duration = ll->phy.hw_get_prepare_time()+ll_get_air_packet_time(ll->phy.mode,BLE_ADV_PRI_PHY_MAX_TX_LEN,0)+PACKET_DEFAULT_TIFS_TIME;
                    }
                    else
                    {
                    	pAdv->la->sch.duration = ll->phy.hw_get_prepare_time()+3*ll_get_air_packet_time(ll->phy.mode,BLE_ADV_PRI_PHY_MAX_TX_LEN,0)+2*PACKET_DEFAULT_TIFS_TIME;
                    }
                }
                else
                {
					if(POINTER_NOT_VALID(pAdv->ea->chain.entry))
					{
						pAdv->ea->aux.data.len = 0;
						pAdv->ea->chain.cnt = 0;
					}
                	pAdv->la->sch.duration = ll->phy.hw_get_prepare_time()+ll_get_air_packet_time(ll->phy.mode,BLE_ADV_PRI_PHY_MAX_TX_LEN,0)+PACKET_DEFAULT_TIFS_TIME;
                    //ea sch and phy init
                	if(pAdv->eventProperty&LL_ADV_EVENT_PROPERTY_CONNECTED)
                	{
						pAdv->ea->aux.sch.duration = ll->phy.hw_get_prepare_time()+ll_get_air_packet_time(ll->phy.mode,2+BLE_ADV_EXTENDED_HEADER_MAX_LEN+pAdv->ea->aux.data.len,0)+PACKET_DEFAULT_TIFS_TIME\
												        + ll_get_air_packet_time(ll->phy.mode,3+sizeof(scan_type_aux_scan_req_t),0)+PACKET_DEFAULT_TIFS_TIME\
												        + ll_get_air_packet_time(ll->phy.mode,3+sizeof(init_type_auxConnectRsp_t),0);
					}
					else if(pAdv->eventProperty&LL_ADV_EVENT_PROPERTY_SCANNABLE)
                	{
						pAdv->ea->aux.sch.duration = ll->phy.hw_get_prepare_time()+ll_get_air_packet_time(ll->phy.mode,2+BLE_ADV_EXTENDED_HEADER_MAX_LEN,0)+PACKET_DEFAULT_TIFS_TIME\
											            + ll_get_air_packet_time(ll->phy.mode,3+sizeof(scan_type_aux_scan_req_t),0)+PACKET_DEFAULT_TIFS_TIME\
												        + ll_get_air_packet_time(ll->phy.mode,2+BLE_ADV_EXTENDED_HEADER_MAX_LEN+pAdv->ea->aux.data.len,0);
					}
                	pAdv->ea->aux.phy.crcInit         = BLE_ADV_CRC_INIT;
                	pAdv->ea->aux.phy.accessCode      = BLE_ADV_ACCESS_CODE;
                	pAdv->ea->aux.phy.rxMaxOctets     = BLE_ADV_SEC_PHY_MAX_TX_LEN;
                	pAdv->ea->aux.phy.rxAddress       = ll_get_shared_phy_rx_address();
                	pAdv->ea->aux.phy.txAddress       = ll_get_shared_phy_tx_address();
					pAdv->ea->aux.phy.mode            = pAdv->ea->phyMode;
					pAdv->ea->eventCnt = 0;
                    if(pAdv->schMap&ADV_SCH_MAP_CHAIN)
                    {
                        for(int i=0;i<pAdv->ea->chain.cnt;i++)
                        {
                        	pAdv->ea->chain.entry[i].sch.startMargin = 100;
                            if(i==pAdv->ea->chain.cnt)
                            {
                            	pAdv->ea->chain.entry[i].sch.stopMargin  = 200;
                            }
                            else
                            {
                            	pAdv->ea->chain.entry[i].sch.stopMargin  = 100;
                            }
                            pAdv->ea->chain.entry[i].phy.crcInit    = BLE_ADV_CRC_INIT;
                            pAdv->ea->chain.entry[i].phy.accessCode = BLE_ADV_ACCESS_CODE;
                            pAdv->ea->chain.entry[i].phy.mode       = pAdv->ea->phyMode;
                            pAdv->ea->chain.entry[i].phy.txAddress  = ll_get_shared_phy_tx_address();
							pAdv->ea->chain.entry[i].sch.duration   = ll->phy.hw_get_prepare_time()+ll_get_air_packet_time(ll->phy.mode,2+BLE_ADV_EXTENDED_HEADER_MAX_LEN+pAdv->ea->chain.entry[i].data.len,0)+PACKET_DEFAULT_TIFS_TIME;
                        }
                    	pAdv->ea->aux.sch.startMargin     = 100;
                    	pAdv->ea->aux.sch.stopMargin      = 100;//shall location the next event
                    }
                    else
                    {
                    	pAdv->ea->aux.sch.startMargin     = 100;
                    	pAdv->ea->aux.sch.stopMargin      = 200;//shall location the next event
                    }
                }

			}
		}
	}
	ble_ll_process_event(ll,BLE_LL_EVENT_START_ADVERTISING);
	return SUCCESS;
}
  
controller_error_code_e ll_set_adv_set_random_address(_u8 advHandle,_u8* address)
{
	ll_internal_adv_param_t* pAdv = ll_extended_adv_get_entity(advHandle,0);

	if(POINTER_NOT_VALID(pAdv))
	{
		return UNKNOWN_ADVERTISING_IDENTIFIER;
	}
	if(pAdv->enable &&\
	  (pAdv->eventProperty&LL_ADV_EVENT_PROPERTY_CONNECTED))
	{
		return COMMAND_DISALLOWED;
	}
	txMemcpy(pAdv->randomAddress,address,6);
	return SUCCESS;
}

controller_error_code_e ll_read_maximum_advertising_data_length(_u16* length)
{
	*length = BLE_ADV_MAXIMUM_ADVERTISING_DATA_LENGTH;
	return SUCCESS;
}

controller_error_code_e ll_read_number_of_supported_advertising_sets(_u8* number)
{
	*number = BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS;
	return SUCCESS;
}

controller_error_code_e ll_remove_advertising_sets(_u8 advHandle)
{
	ll_internal_adv_param_t* pAdv = ll_extended_adv_get_entity(advHandle,0);
	if(POINTER_NOT_VALID(pAdv))
	{
		return IVALID_HCI_COMMAND_PARAMETERS;
	}
	if(pAdv->enable)
	{
		return COMMAND_DISALLOWED;
	}
	if(POINTER_VALID(pAdv->data.addr))
	{
		tx_free(pAdv->data.addr);
	}
	if(POINTER_VALID(pAdv->scanRsp.addr))
	{
		tx_free(pAdv->scanRsp.addr);
	}
	if(POINTER_VALID(pAdv->ea->chain.entry))
	{
		tx_free((_u8*)pAdv->ea->chain.entry);
	}
	if(POINTER_VALID(pAdv->la))
	{
		tx_free((_u8*)pAdv->la);
	}
	if(POINTER_VALID(pAdv->ea))
	{
		tx_free((_u8*)pAdv->ea);
	}
	txMemsetByte((_u8*)pAdv,0,sizeof(ll_internal_adv_param_t));
	pAdv->handle = LL_EXTENDED_ADV_INVALID_HANDLE;

	if(ll_extended_adv_get_current_set_number()==0)
	{
		ll_sm_t* ll = ll_get_current_state_machine();
		ble_ll_process_event(ll,BLE_LL_EVENT_STOP_ADVERTISING);
	}
	return SUCCESS;
}

controller_error_code_e ll_clear_advertising_sets(void)
{
	ll_sm_t* ll = ll_get_current_state_machine();

	for(int i=0;i<BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS;i++)
	{
		if(ll->adv->param[i].handle!=LL_EXTENDED_ADV_INVALID_HANDLE)
		{
			ll_internal_adv_param_t* pAdv = &ll->adv->param[i];
			if(ll->adv->param[i].enable)
			{
				return COMMAND_DISALLOWED;
			}
			if(POINTER_VALID(pAdv->data.addr))
			{
				tx_free(pAdv->data.addr);
			}
			if(POINTER_VALID(pAdv->scanRsp.addr))
			{
				tx_free(pAdv->scanRsp.addr);
			}
			if(POINTER_VALID(pAdv->ea->chain.entry))
			{
				tx_free((_u8*)pAdv->ea->chain.entry);
			}
			if(POINTER_VALID(pAdv->la))
			{
				tx_free((_u8*)pAdv->la);
			}
			if(POINTER_VALID(pAdv->ea))
			{
				tx_free((_u8*)pAdv->ea);
			}
		}
	}
	ble_ll_process_event(ll,BLE_LL_EVENT_STOP_ADVERTISING);
	return SUCCESS;
}

#endif/*(LL_SUPPORT_LE_EXTENDED_ADVERTISING)*/

#if(LL_SUPPORT_LE_PERIODIC_ADVERTISING)

controller_error_code_e ll_set_periodic_advertising_paramters(_u8 advHandle,_u8 interval,_u16 property)
{
	ll_internal_adv_param_t* pAdv = ll_extended_adv_get_entity(advHandle,0);
	if(POINTER_NOT_VALID(pAdv))
	{
		return UNKNOWN_ADVERTISING_IDENTIFIER;
	}
	if(POINTER_NOT_VALID(pAdv->pa))
	{
		pAdv->pa = (ll_adv_pa_set_t*)tx_malloc(sizeof(ll_adv_pa_set_t));
	}
	if(property&LL_ADV_EVENT_PROPERTY_INCLUDE_TX_POWER)
	{
		pAdv->pa->includeTxPower = 1;
	}
	pAdv->pa->sch.interval = 1250*interval;
	pAdv->schMap |= ADV_SCH_MAP_PA;
	return SUCCESS;
}

controller_error_code_e ll_set_periodic_advertising_data(_u8 advHandle,ll_advertising_data_operation_e operation,_u8 dataLen,_u8* data)
{
	ll_internal_adv_param_t* pAdv = ll_extended_adv_get_entity(advHandle,0);
	if(POINTER_NOT_VALID(pAdv))
	{
		return UNKNOWN_ADVERTISING_IDENTIFIER;
	}
	if((!pAdv->schMap&ADV_SCH_MAP_PA))
	{
		return COMMAND_DISALLOWED;
	}
	#if (LL_SUPPORT_PERIODIC_ADVERTISING_WITH_RESPONSES_ADVERTISER)
	if(pAdv->schMap&ADV_SCH_MAP_PAWR)
	{
		return COMMAND_DISALLOWED;
	}
	#endif
	static _u32 dataFillOffset = 0;
	if(dataLen > BLE_ADV_MAXIMUM_ADVERTISING_DATA_LENGTH || (dataLen+dataFillOffset>BLE_ADV_MAXIMUM_ADVERTISING_DATA_LENGTH))
	{
		return MEMORY_CAPACITY_EXCEEDED;
	}
	if(ll_get_air_packet_time(pAdv->ea->phyMode,dataFillOffset+dataLen,0)>(pAdv->pa->sch.interval*3/4))
	{
		return PACKET_TOO_LONG;
	}
	if(operation == LL_ADV_DATA_OPERATION_UNCHANGED)
	{
		if(pAdv->pa->enable == 0\
		||POINTER_NOT_VALID(pAdv->pa->sync.data.addr)\
	    ||pAdv->pa->sync.data.len==0\
	    ||dataLen!=0)
		{
			return IVALID_HCI_COMMAND_PARAMETERS;
		}
	}
	if(dataLen==0\
	&&(operation!=LL_ADV_DATA_OPERATION_COMPLETE)\
	&&(operation!=LL_ADV_DATA_OPERATION_UNCHANGED))
	{
		return IVALID_HCI_COMMAND_PARAMETERS;
	}
	if(pAdv->pa->enable\
	&&(operation!=LL_ADV_DATA_OPERATION_COMPLETE)\
	&&(operation!=LL_ADV_DATA_OPERATION_UNCHANGED))
	{
		return COMMAND_DISALLOWED;
	}
	if(POINTER_NOT_VALID(pAdv->pa->sync.data.addr))
	{
		if(operation == LL_ADV_DATA_OPERATION_COMPLETE)
		{
			pAdv->pa->sync.data.addr = tx_malloc(dataLen);
		}
		else
		{
			pAdv->pa->sync.data.addr = tx_malloc(BLE_ADV_MAXIMUM_ADVERTISING_DATA_LENGTH);
		}
	}
	switch(operation)
	{
		case LL_ADV_DATA_OPERATION_INTERMEDIATE_FRAGMENT:
		{
			txMemcpy4(pAdv->pa->sync.data.addr+dataFillOffset,data,dataLen);
			dataFillOffset+=dataLen;
		}
			break;
		case LL_ADV_DATA_OPERATION_FIRST_FRAGMENT:
		{
			txMemcpy4(pAdv->pa->sync.data.addr,data,dataLen);
			dataFillOffset=dataLen;
		}
			break;	
		case LL_ADV_DATA_OPERATION_LAST_FRAGMENT:
		{
			txMemcpy4(pAdv->pa->data.addr+dataFillOffset,data,dataLen);
			pAdv->pa->data.len = dataFillOffset+dataLen;
			pAdv->ea->did++;
			dataFillOffset = 0;
		}
			break;
		case LL_ADV_DATA_OPERATION_COMPLETE:
		{
			txMemcpy(pAdv->pa->data.addr,data,dataLen);
			pAdv->ea->did++;
			pAdv->pa->data.len = dataLen;
		}
			break;
		case LL_ADV_DATA_OPERATION_UNCHANGED:
		{
			pAdv->ea->did++;
		}
			break;
	}
}

controller_error_code_e ll_set_periodic_advertising_enable(_u8 enable,_u8 advHandle)
{
	ll_internal_adv_param_t* pAdv = ll_extended_adv_get_entity(advHandle,0);
	if(POINTER_NOT_VALID(pAdv))
	{
		return UNKNOWN_ADVERTISING_IDENTIFIER;
	}
	pAdv->pa->enable = (enable&BIT(0)==0?0:1);
	pAdv->pa->includeAdi = (enable&BIT(1)==0?0:1);
}

#endif
