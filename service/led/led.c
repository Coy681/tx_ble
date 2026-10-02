/*
 * led.c
 *
 *  Created on: 2026年10月2日
 *      Author: Admin
 *
 *  Led policy: pin map for this board. Hardware via platform/hal.
 *  Business logic (blink patterns etc.) extends here later.
 */

#include"led.h"
#include"platform/hal/gpio.h"

/* board config: which physical pins are leds */
static const hal_gpio_pin_e ledPinMap[LED_NUM] = {
    HAL_GPIO_PD0,HAL_GPIO_PD1,HAL_GPIO_PE6,HAL_GPIO_PE7
};

static int halGroup = -1;

static void led_init(void)
{
    halGroup = hal_gpio_init(ledPinMap,LED_NUM);
}

void led_high(_u8 n)
{
    if(halGroup >= 0) hal_gpio_set_high(halGroup,n);
}

void led_low(_u8 n)
{
    if(halGroup >= 0) hal_gpio_set_low(halGroup,n);
}

void led_toggle(_u8 n)
{
    if(halGroup >= 0) hal_gpio_toggle(halGroup,n);
}

#if(TX_DEBUG_ENABLE)
TASK_INIT(led_init);
#endif
