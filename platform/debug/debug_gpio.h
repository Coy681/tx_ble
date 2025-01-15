
#include"common/txCommon.h"
#include"platform/platform.h"

#define LED_HIGH(n)           hal_gpio_led_set_high(n); 
#define LED_LOW(n)            hal_gpio_led_set_low(n); 
#define LED_TOGGLE(n)         hal_gpio_led_toggle(n);

#define DEBUG_GPIO_HIGH(n)    hal_gpio_debug_set_high(n); 
#define DEBUG_GPIO_LOW(n)     hal_gpio_debug_set_low(n); 
#define DEBUG_GPIO_TOGGLE(n)  hal_gpio_debug_toggle(n); 