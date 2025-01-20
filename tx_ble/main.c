

#include"driver.h"

#include"platform/platform.h"

#include"tx_common.h"

#include"common/txCommon.h"

#include"system/task/event/event.h"

#include"system/task/message/message.h"

/**
 * @brief       This function service to handle all the platform pre-defined interrupt or exception.
 * @return      none
 */
void trap_entry(void) __attribute__((interrupt("machine"), aligned(4)));

void trap_entry(void)
{

}
void app_rx_cmd(_u8* data,_u32 len)
{
	LOG_TRACE(1,"rx data",data,len)
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
//    _u8* pData = tx_message_allocate(8);
//	pData[0] = 7;
//	pData[1] = 5;
//	LOG_TRACE(1,"message address",(_u8*)&pData,4)
//	tx_message_send(TX_TASK_ID_LOG_RX,pData);

	log_register_rx_callback(app_rx_cmd);
	tx_task_start();
    return 0;
}
