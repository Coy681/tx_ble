/*
 * gpio.h
 *
 *  Created on: 2026年10月2日
 *      Author: Admin
 *
 *  Debug gpio toggle: 16 logical debug pins for timing/interrupt tracing.
 *  Led is NOT here, led is a business service in system/led.
 */

#ifndef DEBUG_GPIO_H_
#define DEBUG_GPIO_H_
#include "common/txCommon.h"
#include "config.h"

#define DEBUG_GPIO_NUM      16

/* logical debug pin index (0 ~ DEBUG_GPIO_NUM-1), maps to debugPinMap[]
 * entries in gpio.c by index. GPIO_n naming kept for call-site compatibility,
 * these are board-level trace point numbers, NOT chip pin encoding. */
typedef enum
{
    DBG_GPIO_0 = 0,
	DBG_GPIO_1,
    DBG_GPIO_2,
	DBG_GPIO_3,
    DBG_GPIO_4,
	DBG_GPIO_5,
    DBG_GPIO_6,
	DBG_GPIO_7,
    DBG_GPIO_8,
	DBG_GPIO_9,
    DBG_GPIO_10,
	DBG_GPIO_11,
    DBG_GPIO_12,
	DBG_GPIO_13,
    DBG_GPIO_14,
	DBG_GPIO_15,
}debug_gpio_id_e;

#if(TX_DEBUG_GPIO_ENABLE)

/**
 * @brief     These functions toggle debug pins for timing trace.
 * @param[in] n - debug pin number, form '0' ~ to 'DEBUG_GPIO_NUM-1'
 * @return    none
 */
void debug_gpio_high(_u8 n);

void debug_gpio_low(_u8 n);

void debug_gpio_toggle(_u8 n);

#define DEBUG_GPIO_HIGH(n)      debug_gpio_high(n);
#define DEBUG_GPIO_LOW(n)       debug_gpio_low(n);
#define DEBUG_GPIO_TOGGLE(n)    debug_gpio_toggle(n);
#else
#define DEBUG_GPIO_HIGH(n)
#define DEBUG_GPIO_LOW(n)
#define DEBUG_GPIO_TOGGLE(n)
#endif//TX_DEBUG_GPIO_ENABLE

#endif /* DEBUG_GPIO_H_ */
