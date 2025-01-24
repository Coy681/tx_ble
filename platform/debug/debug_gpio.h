
#include"common/txCommon.h"
#include"config.h"

#ifndef PLATFORM_FEATURE_DEBUG_GPIO
#define PLATFORM_FEATURE_DEBUG_GPIO    1
#endif

#ifndef PLATFORM_FEATURE_LED
#define PLATFORM_FEATURE_LED           1
#endif


#if(PLATFORM_FEATURE_DEBUG_GPIO)
#define DEBUG_GPIO_HIGH(n)    hal_gpio_debug_set_high(n);
#define DEBUG_GPIO_LOW(n)     hal_gpio_debug_set_low(n);
#define DEBUG_GPIO_TOGGLE(n)  hal_gpio_debug_toggle(n);
#else
#define DEBUG_GPIO_HIGH(n)
#define DEBUG_GPIO_LOW(n)
#define DEBUG_GPIO_TOGGLE(n)
#endif

#if(PLATFORM_FEATURE_LED)
#define LED_HIGH(n)           hal_gpio_led_set_high(n);
#define LED_LOW(n)            hal_gpio_led_set_low(n);
#define LED_TOGGLE(n)         hal_gpio_led_toggle(n);
#else
#define LED_HIGH(n)
#define LED_LOW(n)
#define LED_TOGGLE(n)
#endif




