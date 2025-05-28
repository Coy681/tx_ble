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

#define reg_bb_dbg_sel      REG_ADDR16(0x140378)
#define reg_bb_dbg_sel_l    REG_ADDR8(0x140378)
#define reg_bb_dbg_sel_h    REG_ADDR8(0x140379)
void dbg_bb_set_pin(gpio_pin_e pin)
{
    _u8 mask = 0x3F;//bit5~bit0
    _u8 val = 0x39;//function select DBG_BB

    //note:  setting pad the function  must before  setting no_gpio function, cause it will lead to uart transmit extra one byte data at begin.(confirmed by minghai&sunpeng)
    reg_gpio_func_mux(pin) = (reg_gpio_func_mux(pin) & mask) | val;

    gpio_function_dis(pin);
}
void ble_dbg_port_init(int deg_sel0)
{
    /* 1. r_dbg_sel0: 0x80170003[5],  debug port switch
     * 2. r_dbg_sel1: 0x80170003[7],  debug port switch
     *
     *          r_dbg_sel0 = 1          r_dbg_sel0 = 0          r_dbg_sel1 = 1
     *
     *  PA0:    tx_en                   tx_en
     *  PA1:    tx_on                   tx_on
     *  PA2:    rx_en                   rx_en
     *  PA3:    clk_bb                  clk_bb
     *  PA4:    hit_sync                hit_sync
     *  PB0:    sclk                    sclk
     *  PB1:    tx_data                 tx_data
     *  PB2:    rx_data_vld             rx_data_vld             rx_symb_vld
     *  PB3:    rx_data                 rx_data                 rx_symb0(for zigbee)
     *  PB4:                                                    rx_symb1(for zigbee)
     *  PB5:                                                    rx_symb2(for zigbee)
     *  PB6:                                                    rx_symb3(for zigbee)
     *  PB7:    ll_ss[0]                ll_ss[0]
     *  PC0:    ll_ss[1]                ll_ss[1]
     *  PC1:    ll_ss[2]                ll_ss[2]
     *  PC2:    rx_ss[0]                rx_ss[0]
     *  PC3:    rx_ss[1]                rx_ss[1]
     *  PC4:    rx_ss[2]                rx_ss[2]
     *  PC5:    dma_ack_tx              dma_ack_rx
     *  PC6:    reg_wr                  reg_rd
     *  PC7:    dma_eof                 dma_err
     *  PD0:    dma_sof                 dma_cyc
     *  PD1:    dma_rdy_tx              dma_rdy_rx
     *  PD2:    reg_cs                  reg_cs
     */

    /* ll_ss    state
        0       IDLE
        1       ACTIVE
        2       TXSTL
        3       TX
        4       RXWAIT
        5       RX
        6       TXWAIT

       rx_ss    state
        0       IDLE
        1       SYNC
        2       DEC
        3       HD
        4       FOOT
    */

    /*
     * sub_wr(0x80170003, 1, 5, 5)  //r_dbg_sel0 = 1                0x80140803[5]:  dbg_sel0
     */
    if(deg_sel0){
        REG_ADDR8(0x170003) |= BIT(5);//default 0x0
    }

    /*
     *  sub_wr(0x80140379, 0, 7, 4) //dbg_sel_bt1-4 = 0             0x80140379[7:4] = 0        0000xxxx
        sub_wr(0x80140378, 1, 1, 1) //dbg_sel_bb_l = 1              0x80140378[1] = 1          xxxxxx1x      enable dbg_sel_bb_l
        sub_wr(0x80140378, 1, 2, 2) //dbg_sel_bb_h = 1              0x80140378[2] = 1          xxxxx1xx      enable dbg_sel_bb_h
        sub_wr(0x80140378, 0 ,5, 5) //dbg_axon_bb_sel = 0           0x80140378[5] = 0          xx0xx11x      disable dbg_axon_bb_sel
     */
    reg_bb_dbg_sel_h &= 0x0F;       // 140379[7:4] = 0
    reg_bb_dbg_sel_h |= BIT(0);     // 140379[0] = 1; if not set, B2/B3 not work.
    reg_bb_dbg_sel_l |= BIT(1);     // 140378[1] = 1
    reg_bb_dbg_sel_l |= BIT(2);     // 140378[2] = 1
    reg_bb_dbg_sel_l &= ~BIT(5);    // 140378[5] = 0
}

void rf_enable_bb_debug(void)
{
    ble_dbg_port_init(0);//dma_rx
//  ble_dbg_port_init(1);//dma_tx

    dbg_bb_set_pin(GPIO_PA0); //tx_en
    dbg_bb_set_pin(GPIO_PA1); //tx_on
    dbg_bb_set_pin(GPIO_PA2); //rx_en
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
    rf_enable_bb_debug();
    #endif
    #if(TX_DEBUG_LED_ENABLE) 
    hal_gpio_led_init();
    #endif
}
HAREWARE_INIT(platform_init);


