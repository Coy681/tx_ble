/*
 * stimer.h
 *
 *  Created on: 2024年12月6日
 *      Author: Admin
 */

#ifndef HAL_STIMER_H_
#define HAL_STIMER_H_
#include"driver.h"

#define system_delay_us delay_us
#define system_delay_ms delay_ms

enum
{
	CLOCK_TICK_US = 24,
	CLOCK_TICK_MS = 24000,
	CLOCK_TICK_S  = 24000000,
};

typedef void(*hal_stimer_task)(void);

void hal_stimer_register_task(hal_stimer_task cb);

void hal_stimer_clear_irq(void);

void hal_stimer_set_capture(int captureTick);

unsigned int hal_stimer_get_capture(void);

unsigned int system_clock(void);

unsigned int system_time(void);

unsigned int system_switch_tick_to_time(unsigned int tick);

#endif /* HAL_STIMER_H_ */
