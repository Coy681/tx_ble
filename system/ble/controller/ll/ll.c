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
ll_ctrl_t* llSm;
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
	llSm = (ll_ctrl_t*)tx_malloc(number*sizeof(ll_ctrl_t));
	for(_u8 i=0;i<llSmConut;i++)
	{
		llSm[i].id  = i;
		llSm[i].state =  (_u8)BLE_LL_STATE_STANDBY;
	}
	llCurrentSm = llSm[0].id;
	phy_init();

}

_RAM_CODE
ll_ctrl_t* ll_get_idle_state_machine(void)
{
	for(_u8 i=0;i<llSmConut;i++)
	{
		if(llSm[i].state == (_u8)BLE_LL_STATE_STANDBY)
		{
			return &llSm[i];
		}
	}
	return NULL;
}

_RAM_CODE
ll_ctrl_t* ll_get_state_machine_by_id(_u8 id)
{
	for(_u8 i=0;i<llSmConut;i++)
	{
		if(llSm[i].id == id)
		{
			return &llSm[i];
		}
	}
	return NULL;
}

_RAM_CODE
ll_ctrl_t* ll_get_current_state_machine(void)
{
	return &llSm[llCurrentSm];
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

ble_ll_state_status_e ble_ll_process_event(ll_ctrl_t* sm,ble_ll_event_e event)
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

_u64 ll_get_feature(void)
{
	return (LL_FEATURE_SUPPORT|ll_host_support_feature);
}

void ll_feature_reset(void)
{
	ll_host_support_feature = 0;
}
/******************************************ll reset*******************************************/
controller_error_code_e ll_reset(void)
{
	//ll feature reset
	ll_feature_reset();

    //ll reset process
    return SUCCESS;
}


/*************************************Bluetooth LE Advertising ******************************/

#if(LL_SUPPORT_LE_EXTENDED_ADVERTISING!=1)
controller_error_code_e ll_set_advertising_parameters(_u16 interval,\
	                                                  ll_advertising_type_e type,\
													  ll_own_address_type_e ownAddressType,\
	                                                  ll_peer_address_type_e peerAddressType,\
													  _u8* peerAddress,_u8 channelMap,\
	                                                  ll_advertising_filter_policy_e policy)
{
	ll_ctrl_t* ll = ll_get_current_state_machine();
	if(ll->adv == NULL)
	{
		ll->adv = (ll_internal_adv_ctrl_t*)tx_malloc(sizeof(ll_internal_adv_ctrl_t));
	}
	ll->adv->advInterval = interval;
	ll->adv->channelMap = channelMap;
	ll->adv->advType = type;
	ll->adv->ownAddressType = ownAddressType;
	ll->adv->peerAddressType = peerAddressType;
	ll->adv->channelCnt = 0;
    for(int i=0;i<3;i++)
    {
    	if(ll->adv->channelMap&BIT(i))
    	{
    		ll->adv->chnTable[ll->adv->channelCnt] = 37+i;
    		ll->adv->channelCnt++;
    	}
    }
	txMemcpy(ll->adv->peerAddress,peerAddress,6);
	ll->adv->filterPolicy = policy;
	switch(type)
	{
		case LL_ADV_IND:
		{
			ll->adv->advEventType = ADV_EVENT_CONNECTABLE_SCANNABLE_UNDIRECTED;
		}break;
		case LL_ADV_DIRECT_IND_HIGH_DUTY:
		case LL_ADV_DIRECT_IND_LOW_DUTY:
		{
			ll->adv->advEventType = ADV_EVENT_CONNECTABLE_DIRECTED;
		}break;
		case LL_ADV_SCAN_IND:
		{
			ll->adv->advEventType = ADV_EVENT_SCANNABLE_UNDIRECTED;
		}break;
		case LL_ADV_NONCONN_IND:
		{
			ll->adv->advEventType = ADV_EVENT_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED;
		}break;
	}
	LOG_TRACE(1,"set adv param",0,0)
	return SUCCESS;
}

controller_error_code_e ll_set_advertising_data(_u8* data,_u8 length)
{
	ll_ctrl_t* ll = ll_get_current_state_machine();
	if(ll->adv == NULL)
	{
		ll->adv = (ll_internal_adv_ctrl_t*)tx_malloc(sizeof(ll_internal_adv_ctrl_t));
	}
    if(ll->adv->advData)
	{
		tx_free(ll->adv->advData);
	}
	ll->adv->advDataLen = length;
	ll->adv->advData    = tx_malloc(length);
	txMemcpy(ll->adv->advData,data,length);
	LOG_TRACE(1,"set adv data",0,0)
	return SUCCESS;
}
	
controller_error_code_e ll_set_scan_response_data(_u8* data,_u8 length)
{
	ll_ctrl_t* ll = ll_get_current_state_machine();
	if(ll->adv == NULL)
	{
		ll->adv = (ll_internal_adv_ctrl_t*)tx_malloc(sizeof(ll_internal_adv_ctrl_t));
	}
    if(ll->adv->scanRspData)
	{
		tx_free(ll->adv->scanRspData);
	}
	ll->adv->scanRspDataLen = length;
	ll->adv->scanRspData    = tx_malloc(length);
	txMemcpy(ll->adv->scanRspData,data,length);
	LOG_TRACE(1,"set scan rsp data",0,0)
	return SUCCESS;
}

controller_error_code_e ll_set_advertising_enable(_u8 enable)
{
	ll_ctrl_t* ll = ll_get_current_state_machine();
	//assert ll->adv == NULL
    if(ll->adv->enable == enable)
	{
		return SUCCESS;
	}
	LOG_TRACE(1,"set scan enable",&enable,1)
	if(ll->adv->enable!=enable)
	{
		ll->adv->enable = enable;
		if(ll->adv->enable == LL_ADVERTISING_ENABLE)
		{
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

#else
controller_error_code_e ll_set_extended_advertising_parameters(ll_extended_adv_param_t* pParam)
{
	ll_ctrl_t* ll = ll_get_current_state_machine();
	if(ll->adv == NULL)
	{
		ll->adv = (ll_internal_adv_ctrl_t*)tx_malloc(sizeof(ll_internal_adv_ctrl_t));
	}
	ll_internal_extended_adv_t* pAdv = ll_extended_adv_get_entity(pParam->advHandle);
	if(pAdv == NULL)
	{
		return MEMORY_CAPACITY_EXCEEDED;
	}
	if(pParam->advEventProperty&LL_ADV_EVENT_PROPERTY_LEGACY_PDU)
	{
		switch(pParam->advEventProperty)
		{
			case (LL_ADV_EVENT_PROPERTY_LEGACY_PDU|LL_ADV_EVENT_PROPERTY_CONNECTED|LL_ADV_EVENT_PROPERTY_SCANNABLE):
			{
				pAdv->advEventType = ADV_EVENT_CONNECTABLE_SCANNABLE_UNDIRECTED;
			}
				break;
			case (LL_ADV_EVENT_PROPERTY_LEGACY_PDU|LL_ADV_EVENT_PROPERTY_CONNECTED|LL_ADV_EVENT_PROPERTY_DIRECTED):
			case (LL_ADV_EVENT_PROPERTY_LEGACY_PDU|LL_ADV_EVENT_PROPERTY_CONNECTED|LL_ADV_EVENT_PROPERTY_DIRECTED|LL_ADV_EVENT_PROPERTY_HIGH_DUTY_CONNECTED):
			{
				pAdv->advEventType = ADV_EVENT_CONNECTABLE_DIRECTED;
			}
				break;
			case (LL_ADV_EVENT_PROPERTY_LEGACY_PDU|LL_ADV_EVENT_PROPERTY_SCANNABLE):
			{
				pAdv->advEventType = ADV_EVENT_SCANNABLE_UNDIRECTED;
			}
				break;
			case (LL_ADV_EVENT_PROPERTY_LEGACY_PDU):
			{
				pAdv->advEventType = ADV_EVENT_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED;
			}
		}
	}
	else
	{
		switch(pParam->advEventProperty&(LL_ADV_EVENT_PROPERTY_CONNECTED|LL_ADV_EVENT_PROPERTY_SCANNABLE|LL_ADV_EVENT_PROPERTY_DIRECTED))
		{
			case(LL_ADV_EVENT_PROPERTY_CONNECTED|LL_ADV_EVENT_PROPERTY_DIRECTED):
			{
				pAdv->advEventType = ADV_EVENT_EXTENDED_CONNECTABLE_DIRECTED;
			}
				break;
			case(LL_ADV_EVENT_PROPERTY_CONNECTED):
			{
				pAdv->advEventType = ADV_EVENT_EXTENDED_CONNECTABLE_UNDIRECTED;
			}
				break;
			case(LL_ADV_EVENT_PROPERTY_SCANNABLE|LL_ADV_EVENT_PROPERTY_DIRECTED):
			{
				pAdv->advEventType = ADV_EVENT_EXTENDED_SCANNABLE_DIRECTED;
			}
				break;	
			case(LL_ADV_EVENT_PROPERTY_SCANNABLE):
			{
				pAdv->advEventType = ADV_EVENT_EXTENDED_SCANNABLE_UNDIRECTED;
			}
				break;	
			case(LL_ADV_EVENT_PROPERTY_DIRECTED):
			{
				pAdv->advEventType = ADV_EVENT_EXTENDED_NON_CONNECTABLE_NON_SCANNABLE_DIRECTED;
			}
				break;	
			case(0):
			{
				pAdv->advEventType = ADV_EVENT_EXTENDED_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED;
			}
				break;
		}
	}
	pAdv->filterPolicy = pParam->filterPolicy;
	if(pParam->txPower!=0x7F)
	{
		pAdv->txPower = pParam->txPower;
	}
	pAdv->sid                 = pParam->advSid;
	pAdv->ownAddressType      = pParam->ownAddrType;
	pAdv->peerAddressType     = pParam->peerAddrType;
	pAdv->scanReqNotifyEnable = pParam->scanReqNotifyEnable;
	pAdv->primaryAdvInterval  = pParam->primaryAdvInterval * 625;
	pAdv->advEventProperty    = pParam->advEventProperty;
	pAdv->primaryChnCnt       = 0;
    for(int i=0;i<3;i++)
    {
    	if(pParam->primaryAdvChnMap&BIT(i))
    	{
    		pAdv->primaryChnTable[pAdv->primaryChnCnt] = 37+i;
    		pAdv->primaryChnCnt++;
    	}
    }
	pAdv->secondaryAdvMaxSkip = pParam->secondaryAdvMaxSkip;
	if(pParam->primaryAdvPhy == LL_ADV_PHY_CODED)
	{
		if(pParam->primaryAdvphyOptions == LL_ADV_PHY_OPTIONS_PREFER_S2||\
	       pParam->primaryAdvphyOptions == LL_ADV_PHY_OPTIONS_REQUIRE_S2)
		{
			pAdv->primaryAdvPhyMode = PHY_MODE_CODED_S2;
		}
		else if(pParam->primaryAdvphyOptions == LL_ADV_PHY_OPTIONS_PREFER_S8||\
	            pParam->primaryAdvphyOptions == LL_ADV_PHY_OPTIONS_REQUIRE_S8)
		{
			pAdv->primaryAdvPhyMode = HAL_RF_MODE_CODED_S8;
		}
		else 
		{
			pAdv->primaryAdvPhyMode = HAL_RF_MODE_CODED_S8;
		}
	}
	else
	{
		pAdv->primaryAdvPhyMode = LL_ADV_PHY_1M;
	}


	if(pParam->secondaryAdvPhy == LL_ADV_PHY_CODED)
	{
		if(pParam->secondaryAdvphyOptions == LL_ADV_PHY_OPTIONS_PREFER_S2||\
	       pParam->secondaryAdvphyOptions == LL_ADV_PHY_OPTIONS_REQUIRE_S2)
		{
			pAdv->secondaryAdvPhyMode = PHY_MODE_CODED_S2;
		}
		else if(pParam->secondaryAdvphyOptions == LL_ADV_PHY_OPTIONS_PREFER_S8||\
	            pParam->secondaryAdvphyOptions == LL_ADV_PHY_OPTIONS_REQUIRE_S8)
		{
			pAdv->secondaryAdvPhyMode = PHY_MODE_CODED_S8;
		}
		else
		{
			pAdv->secondaryAdvPhyMode = PHY_MODE_CODED_S8;
		}
	}
	else if(pParam->secondaryAdvPhy == LL_ADV_PHY_1M)
	{
		pAdv->secondaryAdvPhyMode = PHY_MODE_1M;
	}
	else 
	{
		pAdv->secondaryAdvPhyMode = PHY_MODE_2M;
	}
}

controller_error_code_e ll_set_extended_advertising_data(_u8 advHandle,\
														ll_advertising_data_operation_e operation,\
														ll_advertising_data_fragment_perference_e fragPref,\
														_u8 dataLen,\
														_u8* data)
{
	ll_internal_extended_adv_t* pAdv = ll_extended_adv_get_entity(advHandle);
	if(POINTER_NOT_VALID(pAdv))
	{
		return UNKNOWN_ADVERTISING_IDENTIFIER;
	}
	if(pAdv->advEventProperty&LL_ADV_EVENT_PROPERTY_LEGACY_PDU)
	{
		if(dataLen > 31||\
		   operation!=LL_ADV_DATA_OPERATION_COMPLETE||\
		   pAdv->advEventType == ADV_EVENT_CONNECTABLE_DIRECTED)
		{
			return IVALID_HCI_COMMAND_PARAMETERS;
		}
	}
	if(operation == LL_ADV_DATA_OPERATION_UNCHANGED)
	{
		if(pAdv->enable == 0||\
		   pAdv->advDataLen == 0||\
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
	if(pAdv->advEventType == ADV_EVENT_EXTENDED_SCANNABLE_DIRECTED||\
	   pAdv->advEventType == ADV_EVENT_EXTENDED_SCANNABLE_UNDIRECTED)
	{
		return IVALID_HCI_COMMAND_PARAMETERS;//these mode don't have advertising data
	}
	if(pAdv->enable)
	{
		if(operation!=LL_ADV_DATA_OPERATION_COMPLETE&&\
		   operation!=LL_ADV_DATA_OPERATION_UNCHANGED)
		{
			return COMMAND_DISALLOWED;
		}
	}
	if(dataLen > BLE_ADV_MAXIMUM_ADVERTISING_DATA_LENGTH)
	{
		return MEMORY_CAPACITY_EXCEEDED;
	}
	if(ll_get_air_packet_time(pAdv->secondaryAdvPhyMode,dataLen,0)>(pAdv->primaryAdvInterval*3/4))
	{
		return PACKET_TOO_LONG;
	}
	if((operation == LL_ADV_DATA_OPERATION_INTERMEDIATE_FRAGMENT) || (operation == LL_ADV_DATA_OPERATION_LAST_FRAGMENT))
	{
		if((pAdv->advDataFillOffset+dataLen)>BLE_ADV_MAXIMUM_ADVERTISING_DATA_LENGTH)
		{
			pAdv->advDataFillOffset = 0;
			return MEMORY_CAPACITY_EXCEEDED;
		}
		if(ll_get_air_packet_time(pAdv->secondaryAdvPhyMode,pAdv->advDataFillOffset+dataLen,0)>(pAdv->primaryAdvInterval*3/4))
		{
			pAdv->advDataFillOffset = 0;
			return PACKET_TOO_LONG;
		}
	}

	switch(operation)
	{
		case LL_ADV_DATA_OPERATION_INTERMEDIATE_FRAGMENT:
		{
			txMemcpy4(pAdv->advData+pAdv->advDataFillOffset,data,dataLen);
			pAdv->advDataFillOffset+=dataLen;
		}
			break;
		case LL_ADV_DATA_OPERATION_FIRST_FRAGMENT:
		{

			txMemcpy4(pAdv->advData,data,dataLen);
			pAdv->advDataFillOffset +=dataLen;
		}
			break;	
		case LL_ADV_DATA_OPERATION_LAST_FRAGMENT:
		{
			txMemcpy4(pAdv->advData+pAdv->advDataFillOffset,data,dataLen);
			pAdv->advDID++;
		}
			break;
		case LL_ADV_DATA_OPERATION_COMPLETE:
		{
			txMemcpy(pAdv->advData,data,dataLen);
			pAdv->advDID++;
		}
			break;
		case LL_ADV_DATA_OPERATION_UNCHANGED:
		{
			pAdv->advDID++;
		}
			break;
	}
	pAdv->advDatafragPerf = fragPref;

}

controller_error_code_e ll_set_extended_scan_response_data(_u8 advHandle,\
															ll_advertising_data_operation_e operation,\
															ll_advertising_data_fragment_perference_e fragPref,\
															_u8 dataLen,\
															_u8* data)
{
	ll_internal_extended_adv_t* pAdv = ll_extended_adv_get_entity(advHandle);
	if(POINTER_NOT_VALID(pAdv))
	{
		return UNKNOWN_ADVERTISING_IDENTIFIER;
	}
	if(pAdv->advEventProperty&LL_ADV_EVENT_PROPERTY_LEGACY_PDU)
	{
		if(dataLen > 31||\
		   operation!=LL_ADV_DATA_OPERATION_COMPLETE||\
		   (pAdv->advEventType != ADV_EVENT_CONNECTABLE_SCANNABLE_UNDIRECTED)&&(pAdv->advEventType != ADV_EVENT_SCANNABLE_UNDIRECTED))
		{
			return IVALID_HCI_COMMAND_PARAMETERS;
		}
	}
	else 
	{
		if(pAdv->advEventType != ADV_EVENT_EXTENDED_SCANNABLE_DIRECTED||\
		   pAdv->advEventType != ADV_EVENT_EXTENDED_SCANNABLE_UNDIRECTED)
		{
			return ;
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
	if(ll_get_air_packet_time(pAdv->secondaryAdvPhyMode,dataLen,0)>(pAdv->primaryAdvInterval*3/4))
	{
		return PACKET_TOO_LONG;
	}
	if((operation == LL_ADV_DATA_OPERATION_INTERMEDIATE_FRAGMENT) || (operation == LL_ADV_DATA_OPERATION_LAST_FRAGMENT))
	{
		if((pAdv->scanRspDataFillOffset+dataLen)>BLE_ADV_MAXIMUM_ADVERTISING_DATA_LENGTH)
		{
			pAdv->scanRspDataFillOffset = 0;
			return MEMORY_CAPACITY_EXCEEDED;
		}
		if(ll_get_air_packet_time(pAdv->secondaryAdvPhyMode,pAdv->scanRspDataFillOffset+dataLen,0)>(pAdv->primaryAdvInterval*3/4))
		{
			return PACKET_TOO_LONG;
		}
	}
	switch(operation)
	{
		case LL_ADV_DATA_OPERATION_INTERMEDIATE_FRAGMENT:
		{
			txMemcpy4(pAdv->scanRspData+pAdv->scanRspDataFillOffset,data,dataLen);
			pAdv->scanRspDataFillOffset+=dataLen;
		}
			break;
		case LL_ADV_DATA_OPERATION_FIRST_FRAGMENT:
		{

			txMemcpy4(pAdv->scanRspData,data,dataLen);
			pAdv->scanRspDataFillOffset +=dataLen;
		}
			break;	
		case LL_ADV_DATA_OPERATION_LAST_FRAGMENT:
		{
			txMemcpy4(pAdv->scanRspData+pAdv->scanRspDataFillOffset,data,dataLen);
		}
			break;
		case LL_ADV_DATA_OPERATION_COMPLETE:
		{
			txMemcpy(pAdv->scanRspData,data,dataLen);
		}
			break;
	}
	pAdv->scanRspDatafragPerf = fragPref;
}

controller_error_code_e ll_set_extended_advertising_enable()
{

}
  
controller_error_code_e ll_set_adv_set_random_address(_u8 advHandle,_u8* address)
{

}

controller_error_code_e ll_read_maximum_advertising_data_length()
{

}

controller_error_code_e ll_read_number_of_supported_advertising_sets()
{

}

controller_error_code_e ll_remove_advertising_sets()
{

}

controller_error_code_e ll_clear_advertising_sets()
{

}
#endif
