/*
 * gpio.h
 *
 *  Created on: 2024年12月6日
 *      Author: Admin
 *
 *  Generic gpio abstraction: logical pin group driven by a caller-supplied
 *  pin map. No debug/led policy here, callers (debug/gpio, system/led)
 *  own their pin maps.
 */

#ifndef HAL_GPIO_H_
#define HAL_GPIO_H_
#include "../../common/txCommon.h"

/**
 * @brief     Abstract gpio pin identity, chip-independent sequential values.
 *            Chip encoding is mapped in the hal implementation (pinLut[]).
 *            Covers all general-purpose pins PA0~PF7; chip-specific pins
 *            (e.g. MSPI-only group) are not abstracted.
 */
typedef enum
{
    HAL_GPIO_PA0 = 0,
    HAL_GPIO_PA1, HAL_GPIO_PA2, HAL_GPIO_PA3, HAL_GPIO_PA4, HAL_GPIO_PA5, HAL_GPIO_PA6, HAL_GPIO_PA7,
    HAL_GPIO_PB0, HAL_GPIO_PB1, HAL_GPIO_PB2, HAL_GPIO_PB3, HAL_GPIO_PB4, HAL_GPIO_PB5, HAL_GPIO_PB6, HAL_GPIO_PB7,
    HAL_GPIO_PC0, HAL_GPIO_PC1, HAL_GPIO_PC2, HAL_GPIO_PC3, HAL_GPIO_PC4, HAL_GPIO_PC5, HAL_GPIO_PC6, HAL_GPIO_PC7,
    HAL_GPIO_PD0, HAL_GPIO_PD1, HAL_GPIO_PD2, HAL_GPIO_PD3, HAL_GPIO_PD4, HAL_GPIO_PD5, HAL_GPIO_PD6, HAL_GPIO_PD7,
    HAL_GPIO_PE0, HAL_GPIO_PE1, HAL_GPIO_PE2, HAL_GPIO_PE3, HAL_GPIO_PE4, HAL_GPIO_PE5, HAL_GPIO_PE6, HAL_GPIO_PE7,
    HAL_GPIO_PF0, HAL_GPIO_PF1, HAL_GPIO_PF2, HAL_GPIO_PF3, HAL_GPIO_PF4, HAL_GPIO_PF5, HAL_GPIO_PF6, HAL_GPIO_PF7,
    HAL_GPIO_PIN_NUM,
}hal_gpio_pin_e;

#define HAL_GPIO_PIN_NUM_MAX     16

/**
 * @brief     This function is used to initialize a logical pin group.
 * @param[in] pinMap - abstract pin array (hal_gpio_pin_e).
 * @param[in] num    - pin count, must <= HAL_GPIO_PIN_NUM_MAX.
 * @return    group index for later hal_gpio_* calls, -1 on error.
 */
int hal_gpio_init(const hal_gpio_pin_e *pinMap,_u8 num);

/**
 * @brief     These functions operate logical pin by group index + pin no.
 * @param[in] group - group index returned by hal_gpio_init.
 * @param[in] pin   - pin no. inside the group.
 * @return    none
 */
void hal_gpio_set_high(int group,_u8 pin);

void hal_gpio_set_low(int group,_u8 pin);

void hal_gpio_toggle(int group,_u8 pin);

#endif /* HAL_GPIO_H_ */
