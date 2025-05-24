/*
 * system.c
 *
 *  Created on: 2024年12月4日
 *      Author: 12407
 */

#include"driver.h"
#include"../../platform.h"


int irq_disable(void)
{
    unsigned int _irq_state = core_interrupt_disable();
    return _irq_state;
}
void irq_restore(unsigned int _irq_state)
{
    core_restore_interrupt(_irq_state);
}

void irq_enable(void)
{
    core_interrupt_enable();
}



void platform_init(void)
{
	sys_init(LDO_1P4_LDO_2P0, VBAT_MAX_VALUE_GREATER_THAN_3V6, GPIO_VOLTAGE_3V3, INTERNAL_CAP_XTAL24M);
    gpio_set_up_down_res(GPIO_SWS, GPIO_PIN_PULLUP_1M);
    wd_32k_stop();
    wd_stop();
    flash_plic_preempt_config(1, 1);
    plic_preempt_feature_en(CORE_PREEMPT_PRI_MODE0);
    plic_set_threshold(IRQ_PRI_NUM0);
    plic_set_priority(IRQ_SYSTIMER, 2);
    plic_set_priority(IRQ_UART0, 1);
    core_interrupt_enable();
    #if(TX_DEBUG_GPIO_ENABLE) 
    hal_gpio_debug_init();
    #endif
    #if(TX_DEBUG_LED_ENABLE) 
    hal_gpio_led_init();
    #endif
}
HAREWARE_INIT(platform_init);


