/*
 * sche.c
 *
 *  Created on: 2024年12月4日
 *      Author: 12407
 */

#include"../platform/platform.h"
#include"../platform/debug/log.h"
static void system_timer_task(void);
static void system_rf_task(void);

void system_init()
{
	log_init(TRACE_MODE_UART);
	hal_stimer_register_task(system_timer_task);
}

static void system_timer_task(void)
{
	hal_stimer_set_capture(system_clock()+1000*SYSTEM_TIME_MS);
}

static void system_rf_task(void)
{

}
