/*
 * platform.h
 *
 *  Created on: 2024年12月4日
 *      Author: 12407
 */

#ifndef PLATFORM_H_
#define PLATFORM_H_

#include"platform/hal/gpio.h"
#include"platform/hal/rf.h"
#include"platform/hal/stimer.h"
#include"platform/hal/uart.h"
#include"platform/hal/misc.h"

void platform_init(void);

int  irq_disable(void);

void irq_restore(unsigned int _irq_state);

void irq_enable(void);

// Disable interrupts and save the current interrupt state
#define IRQ_DISABLE           (unsigned int _irq_state = irq_disable())

// Restore the saved interrupt state
#define IRQ_RESTORE           (irq_restore(_irq_state))


#endif /* PLATFORM_H_ */
