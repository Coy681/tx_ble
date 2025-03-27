
#include"state.h"

#include"tx_common.h"

#include"common/txCommon.h"

ble_ll_state_machine_t bleLlStateMachine;


void ble_ll_init_state_machine(ble_ll_state_machine_t* sm )
{
    sm->state = BLE_LL_STATE_STANDBY;
    sm->handler = 0;
}

ble_ll_state_e ble_ll_handle_standby(ble_ll_event_e event)
{
	switch(event)
	{
		case BLE_LL_EVENT_START_ADVERTISING:
		{
			return BLE_LL_STATE_ADVERTISING;
		}
		case BLE_LL_EVENT_START_SCANNING:
		{
			return BLE_LL_STATE_SCANNING;
		}
		case BLE_LL_EVENT_START_SYNCHRONIZATION:
		{
			return BLE_LL_STATE_SYNCHRONIZATION;
		}
		case BLE_LL_EVENT_START_BROADCASTING:
		{
			return BLE_LL_EVENT_START_BROADCASTING;
		}
		case BLE_LL_EVENT_START_INITIATING:
		{
			return BLE_LL_STATE_INITIATING;
		}
		default:
			return BLE_LL_STATE_STANDBY;
	}
}

ble_ll_state_e ble_ll_handle_advertising(ble_ll_event_e event)
{
	switch(event)
	{
		case BLE_LL_EVENT_START_CONNECTION:
		{
			return BLE_LL_STATE_CONNECTION;
		}
		case BLE_LL_EVENT_STOP_ADVERTISING:
		{
			return BLE_LL_STATE_STANDBY;
		}
		default:
			return BLE_LL_STATE_ADVERTISING;
	}
}

ble_ll_state_e ble_ll_handle_scanning(ble_ll_event_e event)
{
	switch(event)
	{
		case BLE_LL_EVENT_START_CONNECTION:
		{
			return BLE_LL_STATE_CONNECTION;
		}
		case BLE_LL_EVENT_STOP_ADVERTISING:
		{
			return BLE_LL_STATE_STANDBY;
		}
		default:
			return BLE_LL_STATE_SCANNING;
	}
}

ble_ll_state_e ble_ll_handle_initiaing(ble_ll_event_e event)
{

}

ble_ll_state_e ble_ll_handle_sychronization(ble_ll_event_e event)
{

}

ble_ll_state_e ble_ll_handle_isochronous_broadcasting(ble_ll_event_e event)
{

}


