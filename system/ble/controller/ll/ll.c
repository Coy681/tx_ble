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
	ll_internal_adv_shareSection_t* advSection = &ll->adv->section;
	advSection->interval = interval;
	advSection->ownAddressType = ownAddressType;
	advSection->peerAddressType = peerAddressType;
	advSection->channelCnt = 0;
    for(int i=0;i<3;i++)
    {
    	if(channelMap&BIT(i))
    	{
    		advSection->chnTable[advSection->channelCnt] = 37+i;
    		advSection->channelCnt++;
    	}
    }
	txMemcpy(advSection->peerAddress,peerAddress,6);
	advSection->filterPolicy = policy;
	switch(type)
	{
		case LL_ADV_IND:
		{
			advSection->eventType = ADV_EVENT_CONNECTABLE_SCANNABLE_UNDIRECTED;
		}break;
		case LL_ADV_DIRECT_IND_HIGH_DUTY:
		case LL_ADV_DIRECT_IND_LOW_DUTY:
		{
			advSection->eventType = ADV_EVENT_CONNECTABLE_DIRECTED;
		}break;
		case LL_ADV_SCAN_IND:
		{
			advSection->eventType = ADV_EVENT_SCANNABLE_UNDIRECTED;
		}break;
		case LL_ADV_NONCONN_IND:
		{
			advSection->eventType = ADV_EVENT_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED;
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
	ll_internal_adv_shareSection_t* advSection = &ll->adv->section;
    if(advSection->data)
	{
		tx_free(advSection->data);
	}
	advSection->dataLen = length;
	advSection->data    = tx_malloc(length);
	txMemcpy(advSection->data,data,length);
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
	ll_internal_adv_shareSection_t* advSection = &ll->adv->section;
    if(advSection->scanRspData)
	{
		tx_free(advSection->scanRspData);
	}
	advSection->scanRspDataLen = length;
	advSection->scanRspData    = tx_malloc(length);
	txMemcpy(advSection->scanRspData,data,length);
	LOG_TRACE(1,"set scan rsp data",0,0)
	return SUCCESS;
}

controller_error_code_e ll_set_advertising_enable(_u8 enable)
{
	ll_ctrl_t* ll = ll_get_current_state_machine();
	ll_internal_adv_shareSection_t* advSection = &ll->adv->section;
	//assert ll->adv == NULL
    if(advSection->enable == enable)
	{
		return SUCCESS;
	}
	LOG_TRACE(1,"set scan enable",&enable,1)
	if(advSection->enable!=enable)
	{
		advSection->enable = enable;
		if(advSection->enable == LL_ADVERTISING_ENABLE)
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
		ll->adv = (ll_internal_adv_ctrl_t*)tx_malloc(sizeof(ll_internal_adv_ctrl_t*BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS));
		ASSERT(ll->adv == NULL);
		for(int i=0;i<BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS;i++)
		{
			ll->adv->advSet[i].advHandle = 0xFF;
		}
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
	return SUCCESS;
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
		   pAdv->advEventType == ADV_EVENT_CONNECTABLE_DIRECTED)//direct pdu don't have advertising data
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

	if(pAdv->enable)
	{
		if(operation!=LL_ADV_DATA_OPERATION_COMPLETE&&\
		   operation!=LL_ADV_DATA_OPERATION_UNCHANGED)
		{
			return COMMAND_DISALLOWED;
		}
	}

	if(pAdv->advEventType == ADV_EVENT_EXTENDED_SCANNABLE_DIRECTED||\
	   pAdv->advEventType == ADV_EVENT_EXTENDED_SCANNABLE_UNDIRECTED)
	{
		return IVALID_HCI_COMMAND_PARAMETERS;//mode don't have advertising data
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

	if(POINTER_NOT_VALID(pAdv->advData))
	{
		pAdv->advData = tx_malloc(BLE_ADV_MAXIMUM_ADVERTISING_DATA_LENGTH);
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
			pAdv->advDataFillOffset = dataLen;
		}
			break;	
		case LL_ADV_DATA_OPERATION_LAST_FRAGMENT:
		{
			txMemcpy4(pAdv->advData+pAdv->advDataFillOffset,data,dataLen);
			pAdv->advDID++;
			pAdv->advDataLen = pAdv->advDataFillOffset + dataLen;
		}
			break;
		case LL_ADV_DATA_OPERATION_COMPLETE:
		{
			txMemcpy(pAdv->advData,data,dataLen);
			pAdv->advDID++;
			pAdv->advDataLen = dataLen;
		}
			break;
		case LL_ADV_DATA_OPERATION_UNCHANGED:
		{
			pAdv->advDID++;
		}
			break;
	}
	pAdv->advDatafragPerf = fragPref;
	return SUCCESS;

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
	if(POINTER_NOT_VALID(pAdv->advData))
	{
		pAdv->advData = tx_malloc(BLE_ADV_MAXIMUM_ADVERTISING_DATA_LENGTH);
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
			pAdv->scanRspDataFillOffset=dataLen;
		}
			break;	
		case LL_ADV_DATA_OPERATION_LAST_FRAGMENT:
		{
			txMemcpy4(pAdv->scanRspData+pAdv->scanRspDataFillOffset,data,dataLen);
			pAdv->scanRspDataLen = pAdv->scanRspDataFillOffset+dataLen;
		}
			break;
		case LL_ADV_DATA_OPERATION_COMPLETE:
		{
			txMemcpy(pAdv->scanRspData,data,dataLen);
			pAdv->scanRspDataLen = dataLen;
		}
			break;
	}
	pAdv->scanRspDatafragPerf = fragPref;
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
	ll_ctrl_t* ll = ll_get_current_state_machine();
	if((enable == 0) && (numSets == 0))
	{
		for(int i=0;i<BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS;i++)
		{
			if(ll->adv->advSet[i].advHandle <= 0xEF)
			{
				if(ll->adv->advSet[i].advDataFillOffset !=0 || ll->adv->advSet[i].scanRspDataFillOffset !=0)
				{
					return COMMAND_DISALLOWED;
				}
				if((ll->adv->advSet[i].advEventProperty & LL_ADV_EVENT_PROPERTY_SCANNABLE)&&ll->adv->advSet[i].scanRspDataLen == 0)
				{	
					return COMMAND_DISALLOWED;
				}
				if((ll->adv->advSet[i].advEventProperty & LL_ADV_EVENT_PROPERTY_CONNECTED)&&ll->adv->advSet[i].advDataLen == 0)
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
			ll_internal_extended_adv_t* pAdv = ll_extended_adv_get_entity(pEnableSubFiled[i].advHandle);
			if(POINTER_NOT_VALID(pAdv))
			{
				return IVALID_HCI_COMMAND_PARAMETERS; 
			}
			if(pAdv->advDataFillOffset !=0 || pAdv->scanRspDataFillOffset !=0)
			{
				return COMMAND_DISALLOWED;
			}
			if((pAdv->advEventProperty & LL_ADV_EVENT_PROPERTY_SCANNABLE)&&pAdv->scanRspDataLen == 0)
			{	
				return COMMAND_DISALLOWED;
			}
			if((pAdv->advEventProperty & LL_ADV_EVENT_PROPERTY_CONNECTED)&&pAdv->advDataLen == 0)
			{
				return IVALID_HCI_COMMAND_PARAMETERS;//?,need check if valid
			}
		}
	}

	if((enable == 0) && (numSets == 0))
	{
		for(int i=0;i<BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS;i++)
		{
			if(ll->adv->advSet[i].advHandle <= 0xEF)
			{
				ll->adv->advSet[i].enable   = enable;
				if(pEnableSubFiled[i].duration!=0)
				{
					ll->adv->advSet[i].expireTime =  system_time() + ll->adv->advSet[i].primaryAdvInterval + pEnableSubFiled[i].duration*10000;
				}
				else 
				{
					ll->adv->advSet[i].expireTime = 0;
				}

				if(pEnableSubFiled[i].maxEvents!=0)
				{
					ll->adv->advSet[i].maxEvents    = pEnableSubFiled[i].maxEvents;
					ll->adv->advSet[i].eventCounter = 0;
				}
				else
				{
					ll->adv->advSet[i].maxEvents = 0;
				}
			}
		}
	}
	else
	{
		for(_u8 i=0;i<numSets;i++)
		{	
			ll_internal_extended_adv_t* pAdv = ll_extended_adv_get_entity(pEnableSubFiled[i].advHandle);
			pAdv->enable = enable;
			if(pEnableSubFiled[i].duration!=0)
			{
				pAdv->expireTime =  system_time() + pAdv->primaryAdvInterval + pEnableSubFiled[i].duration*10000;
			}
			else 
			{
				pAdv->expireTime = 0;
			}

			if(pEnableSubFiled[i].maxEvents!=0)
			{
				pAdv->maxEvents    = pEnableSubFiled[i].maxEvents;
				pAdv->eventCounter = 0;
			}
			else
			{
				pAdv->maxEvents = 0;
			}
		}
	}
	ble_ll_process_event(ll,BLE_LL_EVENT_START_ADVERTISING);
	return SUCCESS;
}
  
controller_error_code_e ll_set_adv_set_random_address(_u8 advHandle,_u8* address)
{
	ll_internal_extended_adv_t* pAdv = ll_extended_adv_get_entity(advHandle);

	if(POINTER_NOT_VALID(pAdv))
	{
		return IVALID_HCI_COMMAND_PARAMETERS;
	}
	if(pAdv->enable &&\
	  (pAdv->advEventProperty&LL_ADV_EVENT_PROPERTY_CONNECTED))
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
	ll_internal_extended_adv_t* pAdv = ll_extended_adv_get_entity(advHandle);
	if(POINTER_NOT_VALID(pAdv))
	{
		return IVALID_HCI_COMMAND_PARAMETERS;
	}
	// if() periodic adv is enable,not allowed ro remove the adv
	// {

	// }
	if(POINTER_VALID(pAdv->advData))
	{
		tx_free(pAdv->advData);
	}
	if(POINTER_VALID(pAdv->scanRspData))
	{
		tx_free(pAdv->scanRspData);
	}
	txMemsetByte(pAdv,0,sizeof(ll_internal_extended_adv_t));
	pAdv->advHandle = 0xFF;

	if(ll_extended_adv_get_current_set_number()==0)
	{
		ll_ctrl_t* ll = ll_get_current_state_machine();
		ble_ll_process_event(ll,BLE_LL_EVENT_STOP_ADVERTISING);
	}
	return SUCCESS;
}

controller_error_code_e ll_clear_advertising_sets(void)
{
	ll_ctrl_t* ll = ll_get_current_state_machine();

	for(int i=0;i<BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS;i++)
	{
		if(ll->adv->advSet[i].advHandle!=0xFF)
		{
			if(POINTER_VALID(ll->adv->advSet.advData))
			{
				tx_free(ll->adv->advSet.advData);
			}
			if(POINTER_VALID(ll->adv->advSet.scanRspData))
			{
				tx_free(ll->adv->advSet.scanRspData);
			}
		}
	}
	ble_ll_process_event(ll,BLE_LL_EVENT_STOP_ADVERTISING);
	return SUCCESS;
}
#endif
