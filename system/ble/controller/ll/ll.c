#include"ll.h"
#include"ll_internal.h"
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
}
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
	ll->adv->interval = interval;
	ll->adv->channelMap = channelMap;
	ll->adv->advType = type;
	ll->adv->ownAddressType = ownAddressType;
	ll->adv->peerAddressType = peerAddressType;
	txMemcpy(ll->adv->peerAddress,peerAddress,6);
	ll->adv->filterPolicy = policy;
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
	ll->adv->advData = tx_malloc(length);
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
	if(ll->adv->enable!=enable)
	{
		ll->adv->enable = enable;
		if(ll->adv->enable == LL_ADVERTISING_ENABLE)
		{
			if(BLE_LL_STATE_SUCCESS!=ble_ll_process_event(ll,BLE_LL_EVENT_START_ADVERTISING))
			{
				return IVALID_HCI_COMMAND_PARAMETERS;
			}
			// //schedule start
			// sch_message_t* message = (sch_message_t*)tx_message_allocate(8);
			// message->eventType = SCHE_MESSAGE_TASK_ADD;
			// message->message[0] = ((_u32)&ll->sch);
			// message->message[1] = ((_u32)&ll->sch)>>8;
			// message->message[2] = ((_u32)&ll->sch)>>16;
			// message->message[3] = ((_u32)&ll->sch)>>24;
			// tx_message_send(TX_TASK_ID_SCH,(_u8*)message);
		}
		else//LL_ADVERTISING_DISABLE
		{
			if(BLE_LL_STATE_SUCCESS!=ble_ll_process_event(ll,BLE_LL_EVENT_STOP_ADVERTISING))
			{
				return IVALID_HCI_COMMAND_PARAMETERS;
			}
			// //schedule stop
			// sch_message_t* message = (sch_message_t*)tx_message_allocate(4);
			// message->eventType = SCHE_MESSAGE_TASK_ADD;
			// message->message[0] = ll->id;
			// tx_message_send(TX_TASK_ID_SCH,(_u8*)message);
		}
	}
	return SUCCESS;
}



