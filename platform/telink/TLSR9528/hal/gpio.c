
#include"driver.h"
#include"../../../hal/gpio.h"

/* abstract pin (hal_gpio_pin_e) -> chip pin (gpio_pin_e), the only place
 * chip pin encoding appears in this module */
static const gpio_pin_e hal_gpio_pin_lut[HAL_GPIO_PIN_NUM] =
{
    GPIO_PA0, GPIO_PA1, GPIO_PA2, GPIO_PA3, GPIO_PA4, GPIO_PA5, GPIO_PA6, GPIO_PA7,
    GPIO_PB0, GPIO_PB1, GPIO_PB2, GPIO_PB3, GPIO_PB4, GPIO_PB5, GPIO_PB6, GPIO_PB7,
    GPIO_PC0, GPIO_PC1, GPIO_PC2, GPIO_PC3, GPIO_PC4, GPIO_PC5, GPIO_PC6, GPIO_PC7,
    GPIO_PD0, GPIO_PD1, GPIO_PD2, GPIO_PD3, GPIO_PD4, GPIO_PD5, GPIO_PD6, GPIO_PD7,
    GPIO_PE0, GPIO_PE1, GPIO_PE2, GPIO_PE3, GPIO_PE4, GPIO_PE5, GPIO_PE6, GPIO_PE7,
    GPIO_PF0, GPIO_PF1, GPIO_PF2, GPIO_PF3, GPIO_PF4, GPIO_PF5, GPIO_PF6, GPIO_PF7,
};

static hal_gpio_pin_e hal_gpio_group[HAL_GPIO_PIN_NUM_MAX];
static _u8  hal_gpio_group_num = 0;

int hal_gpio_init(const hal_gpio_pin_e *pinMap,_u8 num)
{
    if(pinMap == NULL || num == 0 || num > HAL_GPIO_PIN_NUM_MAX ||
       hal_gpio_group_num >= HAL_GPIO_PIN_NUM_MAX)
    {
        return -1;
    }
    for(_u8 i = 0; i < num; i++)
    {
        if(pinMap[i] < 0 || pinMap[i] >= HAL_GPIO_PIN_NUM)
        {
            return -1;
        }
        hal_gpio_group[hal_gpio_group_num + i] = pinMap[i];
        gpio_function_en(hal_gpio_pin_lut[pinMap[i]]);
        gpio_output_en(hal_gpio_pin_lut[pinMap[i]]);
        gpio_input_dis(hal_gpio_pin_lut[pinMap[i]]);
    }
    int group = hal_gpio_group_num;
    hal_gpio_group_num += num;
    return group;
}

_RAM_CODE void hal_gpio_set_high(int group,_u8 pin)
{
    gpio_set_high_level(hal_gpio_pin_lut[hal_gpio_group[group + pin]]);
}

_RAM_CODE void hal_gpio_set_low(int group,_u8 pin)
{
    gpio_set_low_level(hal_gpio_pin_lut[hal_gpio_group[group + pin]]);
}

_RAM_CODE void hal_gpio_toggle(int group,_u8 pin)
{
    gpio_toggle(hal_gpio_pin_lut[hal_gpio_group[group + pin]]);
}
