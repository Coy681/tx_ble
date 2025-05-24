

#include"driver.h"

#include"platform/platform.h"

#include"tx_common.h"

#include"common/txCommon.h"

#include"system/task/event/event.h"

#include"system/task/message/message.h"

#include"system/scheduler/sch.h"

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

void app_rx_cmd(_u8* data,_u32 len)
{
	LOG_TRACE(1,"rx data",data,len)
	switch(data[0])
	{

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
	hal_stimer_set_capture(system_time()+1000);
	log_register_rx_callback(app_rx_cmd);
	tx_task_start();
    return 0;
}
