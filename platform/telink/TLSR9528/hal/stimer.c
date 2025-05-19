/*
 * stimer.c
 *
 *  Created on: 2024年12月6日
 *      Author: Admin
 */
#include"driver.h"
#include"../../hal/stimer.h"

#include "../../../../common/txCommon.h"
static hal_stimer_task hal_stimer_irq_cb = NULL;
static unsigned int stimerTime = 0;
static unsigned int lastClockTick = 0;
static unsigned int lastClockRemain = 0;
static unsigned int stimeCapture = 0;
/**
 * @brief		System timer interrupt handler.
 * @param[in]	none
 * @return      none
 */
_RAM_CODE void stimer_irq_handler(void)
{
    if(stimer_get_irq_status(FLD_SYSTEM_IRQ))
    {
        stimer_clr_irq_status(FLD_SYSTEM_IRQ);
        if(hal_stimer_irq_cb)
        {
        	hal_stimer_irq_cb();
        }
    }
}
PLIC_ISR_REGISTER(stimer_irq_handler, IRQ_SYSTIMER)


void hal_stimer_register_task(hal_stimer_task cb)
{
	stimer_set_irq_capture(system_clock() + BIT(30)); //set to long time to prevent abnormal STIMER IRQ
	stimer_set_irq_mask(FLD_SYSTEM_IRQ);
    plic_interrupt_enable(IRQ_SYSTIMER);
    if(cb)
    {
    	hal_stimer_irq_cb = cb;
    }
}

_RAM_CODE void hal_stimer_clear_irq(void)
{
	stimer_clr_irq_status(FLD_SYSTEM_IRQ);
}

_RAM_CODE void hal_stimer_set_capture(int captureTime)
{
	stimeCapture = captureTime ;
    unsigned int captureTick = stimeCapture*CLOCK_TICK_US;
    reg_system_irq_level = captureTick;
}

_RAM_CODE unsigned int hal_stimer_get_capture(void)
{
	return stimeCapture;
}

_RAM_CODE unsigned int system_clock(void)
{
	if(lastClockTick == 0)
	{
		lastClockTick = stimer_get_tick()|1;
		stimerTime = lastClockTick/CLOCK_TICK_US;
		lastClockRemain = lastClockTick%CLOCK_TICK_US;
		return lastClockTick;
	}
	unsigned int clockTick = stimer_get_tick()|1;
	stimerTime += ((clockTick-lastClockTick+lastClockRemain)/CLOCK_TICK_US);
	lastClockRemain = (clockTick-lastClockTick)%CLOCK_TICK_US;
	lastClockTick = clockTick;
	return clockTick;
}

_RAM_CODE unsigned int system_time(void)
{
	system_clock();
	return stimerTime;
}

