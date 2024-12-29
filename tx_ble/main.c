

#include"driver.h"

#include"platform/platform.h"

#include"system/system.h"

#include"common/txCommon.h"

/**
 * @brief       This function service to handle all the platform pre-defined interrupt or exception.
 * @return      none
 */
void trap_entry(void) __attribute__((interrupt("machine"), aligned(4)));

void trap_entry(void)
{

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

	while(1)
	{
        delay_ms(500);
        hal_gpio_led_toggle(LED_0);
	}
    return 0;
}
