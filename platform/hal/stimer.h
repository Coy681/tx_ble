/*
 * stimer.h
 *
 *  Created on: 2024年12月6日
 *      Author: Admin
 */

#ifndef HAL_STIMER_H_
#define HAL_STIMER_H_
#include"driver.h"

#define system_clock    stimer_get_tick
#define system_delay_us delay_us
#define system_delay_ms delay_ms

enum
{
	SYSTEM_TIME_US = 24,
	SYSTEM_TIME_MS = 24000,
	SYSTEM_TIME_S  = 24000000,
};

typedef void(*hal_stimer_task)(void);

void hal_stimer_register_task(hal_stimer_task cb);

void hal_stimer_clear_irq(void);

void hal_stimer_set_capture(int captureTick);

int  hal_stimer_get_capture(void);

#endif /* HAL_STIMER_H_ */
