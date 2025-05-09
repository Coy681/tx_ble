

#include"driver.h"

#include"platform/platform.h"

#include"tx_common.h"

#include"common/txCommon.h"

#include"system/task/event/event.h"

#include"system/task/message/message.h"

#include"system/ble/controller/state.h"

#include"system/scheduler/tsched.h"

volatile _u32 AAA_Mcause = 0;
volatile _u32 AAA_Mtval = 0;
volatile _u32 AAA_Mpec = 0;
volatile _u32 AAA_Mstatus = 0;
volatile _u32 AAA_Mdcause = 0;
volatile _u32 AAA_Ra = 0;

/**
 * @brief       This function service to handle all the platform pre-defined interrupt or exception.
 * @return      none
 */
void trap_entry(void) __attribute__((interrupt("machine"), aligned(4)));

void trap_entry(void)
{
	AAA_Mtval   = read_csr(NDS_MTVAL);
	AAA_Mpec    = read_csr(NDS_MEPC);
	AAA_Mstatus = read_csr(NDS_MSTATUS);
	AAA_Mcause  = read_csr(NDS_MCAUSE);
    AAA_Mdcause = read_csr(NDS_MDCAUSE);
}


void task1_callback(_u8 type)
{
	if(type == SCH_TASK_START)
	{
//		LOG_TRACE(1,"task 1 start",0,0)
	}
	else if(type == SCH_TASK_STOP)
	{
//		LOG_TRACE(1,"task 1 stop",0,0)
	}
	else if(type == SCH_TASK_CANCELED)
	{
//		LOG_TRACE(1,"task 1 canceled",0,0)
	}
	else if(type == SCH_TASK_PASSED)
	{
//		LOG_TRACE(1,"task 1 passed",0,0)
	}
}

sch_node_t aTask1=
{
	.llId = 0x00,
	.type = SCH_FIXED_PERIODIC_TASK,
    .priority = SCH_TASK_PRIORITY_F,
	.update = 0,
	.timestamp = 0,
    .period = 20000,
	.duration = 200,
	.startLatency = 50,
	.stopLatency = 50,
	.cb = task1_callback,
};

void task2_callback(_u8 type)
{
	if(type == SCH_TASK_START)
	{
//		LOG_TRACE(1,"task 2 start",0,0)
	}
	else if(type == SCH_TASK_STOP)
	{
//		LOG_TRACE(1,"task 2 stop",0,0)
	}
	else if(type == SCH_TASK_CANCELED)
	{
//		LOG_TRACE(1,"task 2 canceled",0,0)
	}
	else if(type == SCH_TASK_PASSED)
	{
//		LOG_TRACE(1,"task 2 passed",0,0)
	}

}
sch_node_t aTask2=
{
	.llId = 0x01,
	.type = SCH_FIXED_PERIODIC_TASK,
    .priority = SCH_TASK_PRIORITY_F,
	.update = 0,
	.timestamp = 0,
    .period = 20000,
	.duration = 200,
	.startLatency = 50,
	.stopLatency = 50,
	.cb = task2_callback,
};

void task3_callback(_u8 type)
{
	if(type == SCH_TASK_START)
	{
		LOG_TRACE(1,"task 3 start",0,0)
	}
	else if(type == SCH_TASK_STOP)
	{
		LOG_TRACE(1,"task 3 stop",0,0)
	}
	else if(type == SCH_TASK_CANCELED)
	{
		LOG_TRACE(1,"task 3 canceled",0,0)
	}
	else if(type == SCH_TASK_PASSED)
	{
		LOG_TRACE(1,"task 3 passed",0,0)
	}

}
sch_node_t aTask3=
{
	.llId = 0x02,
	.type = SCH_FIXED_PERIODIC_TASK,
    .priority = SCH_TASK_PRIORITY_F,
	.update = 0,
	.timestamp = 0,
    .period = 20000,
	.duration = 200,
	.startLatency = 50,
	.stopLatency = 50,
	.cb = task3_callback,
};

void app_rx_cmd(_u8* data,_u32 len)
{
	LOG_TRACE(1,"rx data",data,len)
	switch(data[0])
	{
		case 0x01:
		{
			_u8* message = tx_message_allocate(8);

			message[0] = SCHE_MESSAGE_TASK_ADD;
			message[1] = ((_u32)&aTask1);
			message[2] = ((_u32)&aTask1)>>8;
			message[3] = ((_u32)&aTask1)>>16;
			message[4] = ((_u32)&aTask1)>>24;
			tx_message_send(TX_TASK_ID_SCH,message);
		}
		break;
		case 0x02:
		{
			_u8* message = tx_message_allocate(8);
			message[0] = SCHE_MESSAGE_TASK_ADD;
			message[1] = ((_u32)&aTask2);
			message[2] = ((_u32)&aTask2)>>8;
			message[3] = ((_u32)&aTask2)>>16;
			message[4] = ((_u32)&aTask2)>>24;
			tx_message_send(TX_TASK_ID_SCH,message);
		}
		break;
		case 0x03:
		{
			_u8* message = tx_message_allocate(8);
			message[0] = SCHE_MESSAGE_TASK_ADD;
			message[1] = ((_u32)&aTask3);
			message[2] = ((_u32)&aTask3)>>8;
			message[3] = ((_u32)&aTask3)>>16;
			message[4] = ((_u32)&aTask3)>>24;
			tx_message_send(TX_TASK_ID_SCH,message);
		}
		break;
		case 0x04:
		{
			_u8* message = tx_message_allocate(4);
			message[0] = SCHE_MESSAGE_TASK_REMOVE;
			message[1] = 0x00;
			tx_message_send(TX_TASK_ID_SCH,message);
		}
		break;
		case 0x05:
		{
			_u8* message = tx_message_allocate(4);
			message[0] = SCHE_MESSAGE_TASK_REMOVE;
			message[1] = 0x01;
			tx_message_send(TX_TASK_ID_SCH,message);
		}
		break;
		case 0x06:
		{
			_u8* message = tx_message_allocate(4);
			message[0] = SCHE_MESSAGE_TASK_REMOVE;
			message[1] = 0x02;
			tx_message_send(TX_TASK_ID_SCH,message);
		}
		break;
	}
//	switch(data[0])
//	{
//		case 1:
//			ble_ll_process_event(&bleLLStateMachine,BLE_LL_EVENT_START_ADVERTISING);
//			break;
//		case 2:
//			ble_ll_process_event(&bleLLStateMachine,BLE_LL_EVENT_STOP_ADVERTISING);
//			break;
//		case 3:
//			ble_ll_process_event(&bleLLStateMachine,BLE_LL_EVENT_START_INITIATING);
//			break;
//		case 4:
//			ble_ll_process_event(&bleLLStateMachine,BLE_LL_EVENT_STOP_INITIATING);
//			break;
//		case 5:
//			ble_ll_process_event(&bleLLStateMachine,BLE_LL_EVENT_START_CONNECTION);
//			break;
//		case 6:
//			ble_ll_process_event(&bleLLStateMachine,BLE_LL_EVENT_STOP_CONNECTION);
//			break;
//		default:
//			break;
//	}
}


int main(void)
{
	//init call function
	initcall_f pInit;
	_u32 *pTemp;
	extern _u32 _INIT_START;
    extern _u32 _INIT_END;
    for(pTemp = &_INIT_START;pTemp<&_INIT_END; pTemp++)
    {
    	pInit = (initcall_f)(*pTemp);
    	pInit();
    }
	LOG_TRACE(1,"system start",0,0)
	DEBUG_GPIO_HIGH(GPIO_1);
	DEBUG_GPIO_LOW(GPIO_1);

//	hal_stimer_set_capture(system_clock()+1000*SYSTEM_TIME_MS);
	log_register_rx_callback(app_rx_cmd);
	tx_task_start();
    return 0;
}
