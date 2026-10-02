/*
 * stimer.h
 *
 *  Created on: 2024年12月6日
 *      Author: Admin
 */

#ifndef HAL_STIMER_H_
#define HAL_STIMER_H_
#include"common/txCommon.h"

typedef void(*hal_stimer_task)(void);

void system_delay_us(_u32 us);

void system_delay_ms(_u32 ms);

void hal_stimer_register_task(hal_stimer_task cb);

void hal_stimer_clear_irq(void);

void hal_stimer_set_capture(int captureTick);

unsigned int hal_stimer_get_capture(void);

unsigned int system_clock(void);

unsigned int system_time(void);

unsigned int system_switch_tick_to_time(unsigned int tick);

#endif /* HAL_STIMER_H_ */
