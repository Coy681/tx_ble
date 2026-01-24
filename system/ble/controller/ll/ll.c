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
/************************************ll implementation***************************************/
ll_t* ll;


void ll_device_info_init(ll_device_info_t* info)
{
	if(POINTER_VALID(ll))
	{
		ll->localPPM = info->ppm;
	}
}

_u32 ll_get_local_ppm(void)
{
	if(POINTER_VALID(ll))
	{
		return ll->localPPM;
	}
	return 0;
}

void ll_init_state_machine(_u8 number)
{
	ll = (ll_t*)tx_malloc(sizeof(ll_t));
	ASSERT(POINTER_VALID(ll));
	ll->smNum = number;
	if(number==0)
	{
		ll->smNum = 1;
	}
	for(_u8 i=0;i<6;i++)
	{
		ll->addr[i] = 0x12+i;//todo,temporary value
	}
	ll->sm = (ll_sm_t*)tx_malloc(number*sizeof(ll_sm_t));
	ASSERT(POINTER_VALID(ll->sm));
	for(_u8 i=0;i<ll->smNum;i++)
	{
		ll->sm[i].state =  (_u8)BLE_LL_STATE_STANDBY;
	}
	phy_init();
}

_u8* ll_get_sm_entity_by_id(_u8 id)
{
	for(_u8 i=0;i<ll->smNum;i++)
	{
		if(id == ll->sm[i].id)
		{
			return (_u8*)&ll->sm[i];
		}
	}
	return NULL;
}

_u8* ll_get_sm_entity_by_state(ble_ll_state_e state,_u16 handle,_u8 allocate)
{
	for(_u8 i=0;i<ll->smNum;i++)
	{
		if(state == ll->sm[i].state)
		{
			switch(state)
			{
				case BLE_LL_STATE_CONNECTION://maybe multiple entity
				{
					if(POINTER_VALID(ll->sm[i].entity)&&handle==((ll_internal_connection_ctrl_t*)ll->sm[i].entity)->handle)
					{
						return (_u8*)&ll->sm[i];
					}
				}
					break;
				case BLE_LL_STATE_SYNCHRONIZATION://maybe multiple entity
				{
					if(POINTER_VALID(ll->sm[i].entity)&&handle==((ll_internal_synchronous_ctrl_t*)ll->sm[i].entity)->handle)
					{
						return (_u8*)&ll->sm[i];
					}
				}
					break;
				case BLE_LL_STATE_BROADCASTING://maybe multiple entity
				{
					if(POINTER_VALID(ll->sm[i].entity)&&handle==((ll_internal_broadcast_ctrl_t*)ll->sm[i].entity)->handle)
					{
						return (_u8*)&ll->sm[i];
					}
				}
					break;
				case BLE_LL_STATE_ADVERTISING://only one entity
				case BLE_LL_STATE_SCANNING://only one entity
				case BLE_LL_STATE_INITIATING://only one entity
					return (_u8*)&ll->sm[i];
				default:
					break;
			}
		}
	}
	if(allocate == 0)
	{
		return NULL;
	}
	for(_u8 i=0;i<ll->smNum;i++)
	{
		if(BLE_LL_STATE_STANDBY == ll->sm[i].state)
		{
			ll->sm[i].state = state;
			return (_u8*)&ll->sm[i];
		}
	}
	return NULL;
}

_RAM_CODE void ll_free_sm_entity(void)
{

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
//	ll_sm_t* llSm = ll_get_current_state_machine();
//	if(POINTER_VALID(llSm->adv))
//	{
//		for(_u8 i=0;i<BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS;i++)
//		{
//			if((llSm->adv->param[i].enable)&&\
//	          (llSm->adv->param[i].filterPolicy!=LL_FILTER_LIST_NOT_USE))
//			{
//				return COMMAND_DISALLOWED;
//			}
//		}
//	}
//	//todo,process initing and scanning
//	for(_u8 i=0;i<BLE_FILTER_ACCEPT_LIST_SIZE;i++)
//	{
//		if((ll->filterAcceptList[i].occupy)&&\
//		   (ll->filterAcceptList[i].addrType == addrType)&&\
//		   (!txMemcmp(ll->filterAcceptList[i].addr,addr,6)))
//		{
//			return SUCCESS;
//		}
//	}
//	for(_u8 i=0;i<BLE_FILTER_ACCEPT_LIST_SIZE;i++)
//	{
//		if(!(ll->filterAcceptList[i].occupy))
//		{
//			ll->filterAcceptList[i].addrType = addrType;
//			txMemcpy(ll->filterAcceptList[i].addr,addr,6);
//			return SUCCESS;
//		}
//	}
	return MEMORY_CAPACITY_EXCEEDED;
}
controller_error_code_e ll_remove_device_from_filter_accept_list(_u8 addrType,_u8* addr)
{
//	ll_sm_t* llSm = ll_get_current_state_machine();
//	if(POINTER_VALID(llSm->adv))
//	{
//		for(_u8 i=0;i<BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS;i++)
//		{
//			if((llSm->adv->param[i].enable)&&\
//	          (llSm->adv->param[i].filterPolicy!=LL_FILTER_LIST_NOT_USE))
//			{
//				return COMMAND_DISALLOWED;
//			}
//		}
//	}
//	//todo,process initing and scanning
//	for(_u8 i=0;i<BLE_FILTER_ACCEPT_LIST_SIZE;i++)
//	{
//		if((ll->filterAcceptList[i].occupy)&&\
//		   (ll->filterAcceptList[i].addrType == addrType)&&\
//		   (!txMemcmp(ll->filterAcceptList[i].addr,addr,6)))
//		{
//			ll->filterAcceptList[i].occupy = 0;
//			return SUCCESS;
//		}
//	}
	return MEMORY_CAPACITY_EXCEEDED;
}
controller_error_code_e ll_clear_filter_accept_list(void)
{
//	ll_sm_t* llSm = ll_get_current_state_machine();
//	if(POINTER_VALID(llSm->adv))
//	{
//		for(_u8 i=0;i<BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS;i++)
//		{
//			if((llSm->adv->param[i].enable)&&\
//	          (llSm->adv->param[i].filterPolicy!=LL_FILTER_LIST_NOT_USE))
//			{
//				return COMMAND_DISALLOWED;
//			}
//		}
//	}
//	//todo,process initing and scanning
//	for(_u8 i=0;i<BLE_FILTER_ACCEPT_LIST_SIZE;i++)
//	{
//		if(ll->filterAcceptList[i].occupy)
//		{
//			ll->filterAcceptList[i].occupy = 0;
//		}
//	}
	return SUCCESS;
}

/******************************************ll reset*******************************************/
controller_error_code_e ll_reset(void)
{
	//ll feature reset
	ll_host_supported_feature = 0;
	//
	sch_stop();
//	for(int i=0;i<llSmConut;i++)
//	{
//	    //ll reset process
//		ll_sm_t* llSm = &ll->sm[i];
//		llSm->state = BLE_LL_STATE_STANDBY;
//		if(POINTER_VALID(llSm->adv))
//		{
//			if(POINTER_VALID(llSm->adv->reset))
//			{
//				llSm->adv->reset();
//			}
//			tx_free((_u8*)llSm->adv);
//			llSm->adv = NULL;
//		}
//		if(POINTER_VALID(llSm->scan))
//		{
//			if(POINTER_VALID(llSm->scan->reset))
//			{
//				llSm->scan->reset();
//			}
//			tx_free((_u8*)llSm->scan);
//			llSm->scan = NULL;
//		}
//		if(POINTER_VALID(llSm->initiating))
//		{
//			if(POINTER_VALID(llSm->initiating->reset))
//			{
//				llSm->initiating->reset();
//			}
//			tx_free((_u8*)llSm->initiating);
//			llSm->initiating = NULL;
//		}
//		if(POINTER_VALID(llSm->conn))
//		{
//			if(POINTER_VALID(llSm->conn->reset))
//			{
//				llSm->conn->reset();
//			}
//			tx_free((_u8*)llSm->conn);
//			llSm->conn = NULL;
//		}
//		if(POINTER_VALID(llSm->synchronous))
//		{
//			if(POINTER_VALID(llSm->synchronous->reset))
//			{
//				llSm->synchronous->reset();
//			}
//			tx_free((_u8*)llSm->synchronous);
//			llSm->synchronous = NULL;
//		}
//		if(POINTER_VALID(llSm->broadcast))
//		{
//			if(POINTER_VALID(llSm->broadcast->reset))
//			{
//				llSm->broadcast->reset();
//			}
//			tx_free((_u8*)llSm->broadcast);
//			llSm->broadcast = NULL;
//		}
//	}

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
//	ll_sm_t* llSm = ll_get_current_state_machine();
//	if(POINTER_VALID(llSm->adv))
//	{
//		if(llSm->adv->param[0].enable)
//		{
//			return COMMAND_DISALLOWED;
//		}
//	}
	//todo,process initing and scanning
	ll->addrType = 1;
	txMemcmp(ll->addr,addr,6);

}

_u8* ll_get_default_channel_table(void)
{
	return ll->chnTable;
}

controller_error_code_e ll_set_default_channel_table(_u8* chnTable)
{
    if(POINTER_NOT_VALID(chnTable)||(((_u32)chnTable&0xffffffff == 0) && (chnTable[5]&0x1f == 0)))
    {
    	return IVALID_HCI_COMMAND_PARAMETERS;
    }
    chnTable[5]&=0x1f;
    txMemcpy(ll->chnTable,chnTable,5);
    return SUCCESS;
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
