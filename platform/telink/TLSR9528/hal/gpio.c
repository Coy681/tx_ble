
#include"driver.h"
#include"../../../hal/gpio.h"

int hal_gpio_led[4] = {GPIO_PD0,GPIO_PD1,GPIO_PE6,GPIO_PE7};
int hal_gpio_debug[]={};

void hal_gpio_debug_init()
{

}

void hal_gpio_debug_set_high()
{

}

void hal_gpio_debug_set_low()
{

}


void hal_gpio_led_init()
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

