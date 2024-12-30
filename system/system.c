/*
 * sche.c
 *
 *  Created on: 2024年12月4日
 *      Author: 12407
 */

#include"platform/platform.h"
#include"platform/debug/log.h"
#include"common/txCommon.h"

static void system_timer_task(void);
static void system_rf_task(void);


void system_init()
{
	tx_malloc_init();
//	malloc_test();
	hal_stimer_register_task(system_timer_task);
}
ARCH_INIT(system_init);

static void system_timer_task(void)
{
	hal_gpio_led_toggle(LED_1);
	hal_stimer_set_capture(system_clock()+10000*SYSTEM_TIME_MS);
}

static void system_rf_task(void)
{

}
