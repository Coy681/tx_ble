
#include"state.h"

#include"tx_common.h"

#include"common/txCommon.h"

ble_ll_state_machine_t bleLLStateMachine;

typedef struct _PACKED
{
	ble_ll_state_e       currentState;
	ble_ll_state_e       nextState;
	ble_ll_event_e       event;
	ble_ll_event_cb      cb;
}ble_ll_state_table_t;


int ble_ll_state_standby(ble_ll_event_e event)
{
	LOG_TRACE(1,"enter standby state",0,0)
    return 1;
}

int ble_ll_state_advertising(ble_ll_event_e event)
{
	LOG_TRACE(1,"enter advertising state",0,0)
    return 1;
}

int ble_ll_state_scanning(ble_ll_event_e event)
{
	LOG_TRACE(1,"enter scanning state",0,0)
    return 1;
}

int ble_ll_state_initiating(ble_ll_event_e event)
{
	LOG_TRACE(1,"enter initiating state",0,0)
    return 1;
}

int ble_ll_state_connection(ble_ll_event_e event)
{
	LOG_TRACE(1,"enter connection state",0,0)
    return 1;
}

int ble_ll_state_sychronization(ble_ll_event_e event)
{
	LOG_TRACE(1,"enter synchronization state",0,0)
    return 1;
}

int ble_ll_state_broadcasting(ble_ll_event_e event)
{
	LOG_TRACE(1,"enter broadcasting state",0,0)
    return 1;
}


static const ble_ll_state_table_t ble_ll_state_table[]=
{
    //standby state transition
	{BLE_LL_STATE_STANDBY,        BLE_LL_STATE_ADVERTISING,      BLE_LL_EVENT_START_ADVERTISING,    ble_ll_state_advertising},
	{BLE_LL_STATE_STANDBY,        BLE_LL_STATE_BROADCASTING,     BLE_LL_EVENT_START_BROADCASTING,   ble_ll_state_broadcasting},
	{BLE_LL_STATE_STANDBY,        BLE_LL_STATE_SCANNING,         BLE_LL_EVENT_START_SCANNING,       ble_ll_state_scanning},
	{BLE_LL_STATE_STANDBY,        BLE_LL_STATE_SYNCHRONIZATION,  BLE_LL_EVENT_START_SYNCHRONIZATION,ble_ll_state_sychronization},
	{BLE_LL_STATE_STANDBY,	      BLE_LL_STATE_INITIATING     ,  BLE_LL_EVENT_START_INITIATING,     ble_ll_state_initiating},

	//advertising state transition
	{BLE_LL_STATE_ADVERTISING,    BLE_LL_STATE_STANDBY,          BLE_LL_EVENT_STOP_ADVERTISING,     ble_ll_state_standby},
	{BLE_LL_STATE_ADVERTISING,    BLE_LL_STATE_CONNECTION,       BLE_LL_EVENT_START_CONNECTION,     ble_ll_state_connection},

	//broadcasting state transition
	{BLE_LL_STATE_BROADCASTING,   BLE_LL_STATE_STANDBY,          BLE_LL_EVENT_STOP_BROADCASTING,    ble_ll_state_standby},

	//scanning state transition
	{BLE_LL_STATE_SCANNING,       BLE_LL_STATE_STANDBY,          BLE_LL_EVENT_STOP_SCANNING,        ble_ll_state_standby},

	//synchronization state transition
	{BLE_LL_STATE_SYNCHRONIZATION,BLE_LL_STATE_STANDBY,          BLE_LL_EVENT_STOP_SYNCHRONIZATION, ble_ll_state_standby},

	//initiating state transition
	{BLE_LL_STATE_INITIATING,     BLE_LL_STATE_STANDBY,          BLE_LL_EVENT_STOP_INITIATING,      ble_ll_state_standby},
	{BLE_LL_STATE_INITIATING,     BLE_LL_STATE_CONNECTION,       BLE_LL_EVENT_START_CONNECTION,     ble_ll_state_connection},

	//synchronization state transition
	{BLE_LL_STATE_CONNECTION,     BLE_LL_STATE_STANDBY,          BLE_LL_EVENT_STOP_CONNECTION,      ble_ll_state_standby},
};

void ble_ll_process_event(ble_ll_state_machine_t* sm,ble_ll_event_e event)
{
	if(sm==NULL)
	{
		LOG_TRACE(1,"error:sm null",0,0)
		return;
	}
	if(event>=BLE_LL_EVENT_MAX)
	{
		LOG_TRACE(1,"error:event invalid",(_u8*)&event,4)
		return;
	}

	for(_u8 i=0;i<ARRAY_SIZE(ble_ll_state_table);i++)
	{
		if(ble_ll_state_table[i].currentState == sm->state&&\
		   ble_ll_state_table[i].event        == event)
		{
			if(ble_ll_state_table[i].cb(event))
			{
				LOG_TRACE(1,"state transition success",(_u8*)&ble_ll_state_table[i].nextState,4)
				sm->state = ble_ll_state_table[i].nextState;
				return;
			}
		}
	}
	LOG_TRACE(1,"state transition fail",0,0)
	LOG_TRACE(1,"current state",(_u8*)&sm->state,4)
	LOG_TRACE(1,"event",(_u8*)&event,4)
}

void ble_ll_init_state_machine(void)
{
	bleLLStateMachine.state = BLE_LL_STATE_STANDBY;
}
TASK_INIT(ble_ll_init_state_machine);

