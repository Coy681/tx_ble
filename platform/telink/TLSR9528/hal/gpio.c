
#include"driver.h"
#include"../../../hal/gpio.h"

static int hal_gpio_led[4] = {GPIO_PD0,GPIO_PD1,GPIO_PE6,GPIO_PE7};
static int hal_gpio_debug[8]={GPIO_PB0,GPIO_PB1,GPIO_PB2,GPIO_PB3,GPIO_PB4,GPIO_PB5,GPIO_PB6,GPIO_PB7};

void hal_gpio_debug_init(void)
{
	for(unsigned char i=0;i<8;i++)
	{
		gpio_function_en(hal_gpio_debug[i]);
		gpio_output_en(hal_gpio_debug[i]);
		gpio_input_dis(hal_gpio_debug[i]);
	}
}

void hal_gpio_debug_set_high(hal_gpio_debug_e gpio)
{
	gpio_set_high_level(hal_gpio_debug[gpio]);
}

void hal_gpio_debug_set_low(hal_gpio_debug_e gpio)
{
	gpio_set_low_level(hal_gpio_debug[gpio]);
}

void hal_gpio_debug_toggle(hal_gpio_debug_e gpio)
{
	gpio_toggle(hal_gpio_debug[gpio]);
}


void hal_gpio_led_init(void)
{
	for(unsigned char i=0;i<4;i++)
	{
		gpio_function_en(hal_gpio_led[i]);
		gpio_output_en(hal_gpio_led[i]);
		gpio_input_dis(hal_gpio_led[i]);
	}
}

void hal_gpio_led_toggle(hal_gpio_led_e gpio)
{
    gpio_toggle(hal_gpio_led[gpio]);
}

void hal_gpio_led_set_high(hal_gpio_led_e gpio)
{
    gpio_set_high_level(hal_gpio_led[gpio]);
}

void hal_gpio_led_set_low(hal_gpio_led_e gpio)
{
    gpio_set_low_level(hal_gpio_led[gpio]);
}

