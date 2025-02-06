/*
 * system.c
 *
 *  Created on: 2024年12月4日
 *      Author: 12407
 */

#include"driver.h"
#include"../../platform.h"



void platform_init(void)
{
	sys_init(LDO_1P4_LDO_2P0, VBAT_MAX_VALUE_GREATER_THAN_3V6, GPIO_VOLTAGE_3V3, INTERNAL_CAP_XTAL24M);
    gpio_set_up_down_res(GPIO_SWS, GPIO_PIN_PULLUP_1M);
    wd_32k_stop();
    wd_stop();
    core_interrupt_enable();
    #if(TX_DEBUG_GPIO_ENABLE) 
    hal_gpio_debug_init();
    #endif
    #if(TX_DEBUG_LED_ENABLE) 
    hal_gpio_led_init();
    #endif
    #if(TX_HARDWARE_RADIO_ENABLE) 
    hal_rf_hardware_init();
    #endif
}
HAREWARE_INIT(platform_init);



