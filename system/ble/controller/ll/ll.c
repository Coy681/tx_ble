#include"ll.h"
#include"ll_internal.h"
#include"ll_config.h"
#include"system/task/message/message.h"
#include"system/task/task.h"
#include"system/task/event/event.h" 
#include"module/adv/adv_internal.h"
#include"module/scan/scan_internal.h"
#include"module/conn/conn_internal.h"
#include"module/init/init_internal.h"
#include"module/sync/sync_internal.h"
#include"module/brd/brd_internal.h"
#include"module/standby/standby_internal.h"
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
			llCurrentSm = i;
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
			llCurrentSm = i;
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

void ble_ll_setup_new_entity(void)
{
	ll_get_idle_state_machine();
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
static _u32 ll_host_supported_feature;

//bit0 -LL_FEATURE_BIT_CONNECTED_ISOCHRONOUS_STREAM_HOST_SUPPORT     FEATURE BIT(32)
//bit1 -LL_FEATURE_BIT_CONNECTION_SUBRATING_HOST_SUPPORT			 FEATURE BIT(38)
//bit2 -LL_FEATURE_BIT_ADVERTISING_CODING_SELECTION_HOST_SUPPORT     FEATURE BIT(41)
//bit3 -LL_FEATURE_BIT_CHANNEL_SOUNDING_HOST_SUPPORT                 FEATURE BIT(47)

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
	if(ll_host_supported_feature&BIT(0))
	{
		suppFea[4] |= BIT(0);//LL_FEATURE_BIT_CONNECTED_ISOCHRONOUS_STREAM_HOST_SUPPORT     FEATURE BIT(32)
	}
	if(ll_host_supported_feature&BIT(1))
	{
		suppFea[4] |= BIT(6);//LL_FEATURE_BIT_CONNECTION_SUBRATING_HOST_SUPPORT			 FEATURE BIT(38)
	}
	if(ll_host_supported_feature&BIT(2))
	{
		suppFea[5] |= BIT(2);//LL_FEATURE_BIT_ADVERTISING_CODING_SELECTION_HOST_SUPPORT     FEATURE BIT(41)
	}
	if(ll_host_supported_feature&BIT(3))
	{
		suppFea[5] |= BIT(7);//LL_FEATURE_BIT_CHANNEL_SOUNDING_HOST_SUPPORT                 FEATURE BIT(47)
	}
	for(int i=0;i<len;i++)
	{
		feature[i] = suppFea[i];
	}
}

controller_error_code_e ll_set_host_feature(_u16 bitNum,_u8 bitValue)
{
	switch(BIT64(bitNum))
	{
		case LL_FEATURE_BIT_CONNECTED_ISOCHRONOUS_STREAM_HOST_SUPPORT:
		{
			if(LL_SUPPORT_CONNECTED_ISOCHRONOUS_STREAM_CENTRAL||LL_SUPPORT_CONNECTED_ISOCHRONOUS_STREAM_PERIPHERAL)
			{
				ll_host_supported_feature|= BIT(0);
			}
			else
			{
				return UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE;
			}
		}
			break;
		case LL_FEATURE_BIT_CONNECTION_SUBRATING_HOST_SUPPORT:
		{
			if(LL_SUPPORT_CONNECTION_SUBRATING)
			{
				ll_host_supported_feature|= BIT(1);
			}
			else
			{
				return UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE;
			}
		}
			break;
		case LL_FEATURE_BIT_ADVERTISING_CODING_SELECTION_HOST_SUPPORT:
		{
			if(LL_SUPPORT_ADVERTISING_CODING_SELECTION)
			{
				ll_host_supported_feature|= BIT(2);
			}
			else
			{
				return UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE;
			}
		}
			break;
		case LL_FEATURE_BIT_CHANNEL_SOUNDING_HOST_SUPPORT:
		{
			if(LL_SUPPORT_CHANNEL_SOUNDING)
			{
				ll_host_supported_feature|= BIT(3);
			}
			else
			{
				return UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE;
			}
		}
			break;
		default:
		{
			return UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE;
		}
	}
	return SUCCESS;
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
	ll_host_supported_feature = 0;
	//
	sch_stop();
	for(int i=0;i<llSmConut;i++)
	{
	    //ll reset process
		ll_sm_t* llSm = &ll->sm[i];
		llSm->state = BLE_LL_STATE_STANDBY;
		if(POINTER_VALID(llSm->adv))
		{
			if(POINTER_VALID(llSm->adv->reset))
			{
				llSm->adv->reset();
			}
			tx_free((_u8*)llSm->adv);
			llSm->adv = NULL;
		}
		if(POINTER_VALID(llSm->scan))
		{
			if(POINTER_VALID(llSm->scan->reset))
			{
				llSm->scan->reset();
			}
			tx_free((_u8*)llSm->scan);
			llSm->scan = NULL;
		}
		if(POINTER_VALID(llSm->initiating))
		{
			if(POINTER_VALID(llSm->initiating->reset))
			{
				llSm->initiating->reset();
			}
			tx_free((_u8*)llSm->initiating);
			llSm->initiating = NULL;
		}
		if(POINTER_VALID(llSm->conn))
		{
			if(POINTER_VALID(llSm->conn->reset))
			{
				llSm->conn->reset();
			}
			tx_free((_u8*)llSm->conn);
			llSm->conn = NULL;
		}
		if(POINTER_VALID(llSm->synchronous))
		{
			if(POINTER_VALID(llSm->synchronous->reset))
			{
				llSm->synchronous->reset();
			}
			tx_free((_u8*)llSm->synchronous);
			llSm->synchronous = NULL;
		}
		if(POINTER_VALID(llSm->broadcast))
		{
			if(POINTER_VALID(llSm->broadcast->reset))
			{
				llSm->broadcast->reset();
			}
			tx_free((_u8*)llSm->broadcast);
			llSm->broadcast = NULL;
		}
	}

    return SUCCESS;
}

controller_error_code_e ll_set_event_mask(_u64 eventMask)
{
	ll->eventMask = eventMask;
	return SUCCESS;
}

controller_error_code_e ll_set_event_mask2(_u64 eventMask)
{
	ll->eventMask2 = eventMask;
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

#if(LL_SUPPORT_CONNECTION_SUBRATING)
controller_error_code_e ll_set_default_subrate(_u16 subrateMin,_u16 subrateMax,_u16 maxLatency,_u16 continuation,_u16 timeout)
{
	if(subrateMin>500||subrateMin==0\
	 ||subrateMax>500||subrateMax==0\
	 ||subrateMax<subrateMin\
	 ||maxLatency>0x1f3||continuation>0x1f3\
	 ||timeout<0x0a||timeout>0xc80\
     ||((subrateMax*(maxLatency+1))>500)\
	 ||continuation>subrateMax)
	{
		return IVALID_HCI_COMMAND_PARAMETERS;
	}
	else
	{
		ll->srMin     = subrateMin;
		ll->srMax     = subrateMax;
		ll->srLatency = maxLatency;
		ll->srConNum  = continuation;
		ll->srTimeout = timeout;
		return SUCCESS;
	}
}
#endif
