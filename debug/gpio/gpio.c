/*
 * gpio.c
 *
 *  Created on: 2026年10月2日
 *      Author: Admin
 *
 *  Debug gpio policy: pin map for this board + on/off switch.
 *  Hardware operation is delegated to platform/hal.
 */

#include"gpio.h"
#include"platform/hal/gpio.h"

#if(TX_DEBUG_GPIO_ENABLE)

/* board config: which physical pins are debug pins */
static const hal_gpio_pin_e debugPinMap[DEBUG_GPIO_NUM] = {
    HAL_GPIO_PB0,HAL_GPIO_PB1,HAL_GPIO_PB2,HAL_GPIO_PB3,
    HAL_GPIO_PB4,HAL_GPIO_PB5,HAL_GPIO_PB6,HAL_GPIO_PB7,
    HAL_GPIO_PC0,HAL_GPIO_PC3,HAL_GPIO_PE0,HAL_GPIO_PE1,
    HAL_GPIO_PE2,HAL_GPIO_PE3,HAL_GPIO_PE4,HAL_GPIO_PE5
};

static int halGroup = -1;

static void debug_gpio_init(void)
{
    halGroup = hal_gpio_init(debugPinMap,DEBUG_GPIO_NUM);
    ASSERT(halGroup >= 0);
}

void debug_gpio_high(_u8 n)
{
    ASSERT(n < DEBUG_GPIO_NUM);
    if (halGroup >= 0)
    {
        hal_gpio_set_high(halGroup,n);
    }
}

void debug_gpio_low(_u8 n)
{
    ASSERT(n < DEBUG_GPIO_NUM);
    if (halGroup >= 0)
    {
        hal_gpio_set_low(halGroup,n);
    }
}

void debug_gpio_toggle(_u8 n)
{
    ASSERT(n < DEBUG_GPIO_NUM);
    if (halGroup >= 0)
    {
        hal_gpio_toggle(halGroup,n);
    }
}

TASK_INIT(debug_gpio_init);
#endif//TX_DEBUG_GPIO_ENABLE
