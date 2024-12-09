/*
 * stimer.c
 *
 *  Created on: 2024年12月6日
 *      Author: Admin
 */
#include"driver.h"
#include"../../hal/stimer.h"
#include"../../../common/common.h"
static hal_stimer_task hal_stimer_irq_cb = NULL;

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

void hal_stimer_clear_irq(void)
{
	stimer_clr_irq_status(FLD_SYSTEM_IRQ);
}

void hal_stimer_set_capture(int captureTick)
{
	stimer_set_irq_capture(captureTick);
}

int hal_stimer_get_capture(void)
{
	return stimer_get_tick();
}


