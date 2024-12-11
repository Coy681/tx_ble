

#include"driver.h"

#include"../platform/platform.h"

#include"../system/system.h"

#include"../common/txBuffer.h"
#include"../common/txType.h"
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
	platform_init();
	system_init();

	hal_stimer_set_capture(system_clock()+1000*SYSTEM_TIME_MS);

	while(1)
	{
        delay_ms(500);
        hal_gpio_led_toggle(LED_0);
	}
    return 0;
}
