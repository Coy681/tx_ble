

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

_u8 test[5] = {0x11,0x22,0x33,0x44,0x55};

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

	LOG_TRACE(1,"log test",test,5)
	test[0]++;

	LOG_TRACE(1,"log test1",test,5)
	test[0]++;

	LOG_TRACE(1,"log test2",test,5)


	tx_task_start();

	// while(1)
	// {
    //     delay_ms(500);
    //     LED_TOGGLE(LED_0);
	// 	DEBUG_GPIO_HIGH(GPIO_0);
	// 	DEBUG_GPIO_LOW(GPIO_0);
	// }
    return 0;
}
