
typedef enum
{
    GPIO_0 = 0,
	GPIO_1,
    GPIO_2,
	GPIO_3,
    GPIO_4,
	GPIO_5,
    GPIO_6,
	GPIO_7,
}hal_gpio_debug_e;

void hal_gpio_debug_init(void);

void hal_gpio_debug_set_high(hal_gpio_debug_e gpio);

void hal_gpio_debug_set_low(hal_gpio_debug_e gpio);

void hal_gpio_debug_toggle(hal_gpio_debug_e gpio);

typedef enum
{
    LED_0 = 0,
	LED_1,
	LED_2,
	LED_3,
}hal_gpio_led_e;
void hal_gpio_led_init(void);

void hal_gpio_led_set_high(hal_gpio_led_e gpio);

void hal_gpio_led_set_low(hal_gpio_led_e gpio);

void hal_gpio_led_toggle(hal_gpio_led_e gpio);
