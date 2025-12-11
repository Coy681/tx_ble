#include"standby.h"
#include"standby_internal.h"
int ble_ll_enter_standby_state(ble_ll_event_e event)
{
    ll_sm_t* ll = ll_get_current_state_machine();
    if(BLE_LL_EVENT_STOP_ADVERTISING == event)
    {
    	sch_remove_task(ll->sch.id);
    	LOG_TRACE(LL_LOG_TRACE,"advertising state to standby state",0,0)
    }
    if(BLE_LL_EVENT_STOP_SCANNING == event)
    {
    	if(POINTER_VALID(ll->scan))
    	{
    		if(POINTER_VALID(ll->scan->reset))
    		{
    			ll->scan->reset();
    		}
    		tx_free((_u8*)ll->scan);
    		ll->scan =NULL;
    	}
    	LOG_TRACE(LL_LOG_TRACE,"scanning state to standby state",0,0)
    }
    if(BLE_LL_EVENT_STOP_INITIATING == event)
    {
    	if(POINTER_VALID(ll->initiating))
    	{
    		if(POINTER_VALID(ll->initiating->reset))
    		{
    			ll->initiating->reset();
    		}
    		tx_free((_u8*)ll->initiating);
    		ll->initiating =NULL;
    	}
    	LOG_TRACE(LL_LOG_TRACE,"initiating state to standby state",0,0)
    }
    if(BLE_LL_EVENT_STOP_CONNECTION == event)
    {
    	if(POINTER_VALID(ll->conn))
    	{
    		if(POINTER_VALID(ll->conn->reset))
    		{
    			ll->conn->reset();
    		}
    		tx_free((_u8*)ll->conn);
    		ll->conn =NULL;
    	}
    	LOG_TRACE(LL_LOG_TRACE,"connection state to standby state",0,0)
    }
    if(BLE_LL_EVENT_STOP_SYNCHRONIZATION == event)
    {
    	if(POINTER_VALID(ll->synchronous))
    	{
    		if(POINTER_VALID(ll->synchronous->reset))
    		{
    			ll->synchronous->reset();
    		}
    		tx_free((_u8*)ll->synchronous);
    		ll->synchronous =NULL;
    	}
    	LOG_TRACE(LL_LOG_TRACE,"synchronization state to standby state",0,0)
    }
    if(BLE_LL_EVENT_STOP_BROADCASTING == event)
    {
    	if(POINTER_VALID(ll->broadcast))
    	{
    		if(POINTER_VALID(ll->broadcast->reset))
    		{
    			ll->broadcast->reset();
    		}
    		tx_free((_u8*)ll->broadcast);
    		ll->broadcast =NULL;
    	}
    	LOG_TRACE(LL_LOG_TRACE,"broadcasting state to standby state",0,0)
    }
	LOG_TRACE(LL_LOG_TRACE,"enter standby state success",0,0)
    return 1;
}
