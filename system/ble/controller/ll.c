#include"ll.h"
#include"ll_internal.h"
#include"system/task/message/message.h"
#include"system/task/task.h"
#include"system/task/event/event.h" 
/************************************ll implementation***************************************/
ll_ctrl_t* llSm;
static _u8 llSmConut;
static _u8 llCurrentSm;
void ll_init(_u8 number)
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
			//schedule start
			sch_message_t* message = (sch_message_t*)tx_message_allocate(8);
			message->eventType = SCHE_MESSAGE_TASK_ADD;
			message->message[0] = ((_u32)&ll->sch);
			message->message[1] = ((_u32)&ll->sch)>>8;
			message->message[2] = ((_u32)&ll->sch)>>16;
			message->message[3] = ((_u32)&ll->sch)>>24;
			tx_message_send(TX_TASK_ID_SCH,(_u8*)message);
		}
		else//LL_ADVERTISING_DISABLE
		{
			//schedule stop
			sch_message_t* message = (sch_message_t*)tx_message_allocate(4);
			message->eventType = SCHE_MESSAGE_TASK_ADD;
			message->message[0] = ll->id;
			tx_message_send(TX_TASK_ID_SCH,(_u8*)message);
		}
	}
	return SUCCESS;
}