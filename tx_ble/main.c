

#include"driver.h"

#include"platform/platform.h"

#include"system/system.h"

#include"common/txCommon.h"

#include"platform/debug/debug_gpio.h"

#include"platform/debug/log.h"

#include"system/task/event/event.h"


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
	LOG_TRACE(1,"rx data",data,len);
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

	log_register_rx_callback(app_rx_cmd);
	tx_task_start();
    return 0;
}
