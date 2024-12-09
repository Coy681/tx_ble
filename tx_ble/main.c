

#include"driver.h"

#include"../platform/platform.h"

#include"../system/system.h"

//#include"../common/common.h"
/**
 * @brief       This function service to handle all the platform pre-defined interrupt or exception.
 * @return      none
 */
void trap_entry(void) __attribute__((interrupt("machine"), aligned(4)));

void trap_entry(void)
{

}

_u8 AAA_test0[5] = {0x01,0x02,0x03,0x04,0x05};
_u8 AAA_target[5] = {0x01,0x02,0x03,0x04,0x05};
_u32 AAA_target1[5] = {0x01223344,0x021122112,0x03,0x04,0x05};
_u8 AAA_result = 0;
_u8 AAA_result1 = 0;
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
