/*
 * rf.c
 *
 *  Created on: 2024年12月4日
 *      Author: 12407
 */

#include"../../hal/rf.h"
#include"../../hal/stimer.h"
#include"driver.h"
#include"config.h"
#include"common/txCommon.h"
void(*hal_rf_cb)(_u8);


/*******************rf extra time define********************************/

//rf TX prepare delay
#define HARDWARE_1M_TX_PREPARE_DELAY         52
#define HARDWARE_2M_TX_PREPARE_DELAY         52
#define HARDWARE_CODED_TX_PREPARE_DELAY      52

//rf RX prepare delay
#define HARDWARE_1M_RX_PREPARE_DELAY         45
#define HARDWARE_2M_RX_PREPARE_DELAY         45
#define HARDWARE_CODED_RX_PREPARE_DELAY      45

//extra preamble delay
#define HARDWARE_1M_EXTRA_PREAMBLE_DELAY     8
#define HARDWARE_2M_EXTRA_PREAMBLE_DELAY     4
#define HARDWARE_CODED_EXTRA_PREAMBLE_DELAY  0

//timestamp offset from ac
#define HARDWARE_1M_TS_FROM_AC_DELAY         19
#define HARDWARE_2M_TS_FROM_AC_DELAY         13
#define HARDWARE_CODED_TS_FROM_AC_DELAY      14

/***********************Hardware used***************************/

//rf prepare
#define HARDWARE_TX_PREPARE_TIME_1M                (HARDWARE_1M_TX_PREPARE_DELAY+HARDWARE_1M_EXTRA_PREAMBLE_DELAY)
#define HARDWARE_TX_PREPARE_TIME_2M                (HARDWARE_2M_TX_PREPARE_DELAY+HARDWARE_2M_EXTRA_PREAMBLE_DELAY)
#define HARDWARE_TX_PREPARE_TIME_CODED             (HARDWARE_CODED_TX_PREPARE_DELAY+HARDWARE_CODED_EXTRA_PREAMBLE_DELAY)

#define HARDWARE_RX_PREPARE_TIME_1M                (HARDWARE_1M_RX_PREPARE_DELAY)
#define HARDWARE_RX_PREPARE_TIME_2M                (HARDWARE_2M_RX_PREPARE_DELAY)
#define HARDWARE_RX_PREPARE_TIME_CODED             (HARDWARE_CODED_RX_PREPARE_DELAY)

//preamble and access code
#define HARDWARE_AIR_TO_AC_TIME_1M                 (RF_PACKET_PREAMBLE_TIME_1M     + RF_PACKET_ACCESS_CODE_TIME_1M)
#define HARDWARE_AIR_TO_AC_TIME_2M                 (RF_PACKET_PREAMBLE_TIME_2M     + RF_PACKET_ACCESS_CODE_TIME_2M)
#define HARDWARE_AIR_TO_AC_TIME_CODED              (RF_PACKET_PREAMBLE_TIME_CODED_S2  + RF_PACKET_ACCESS_CODE_TIME_CODED_S2)
static _u16 rxAcToTs[4] = 
{
    HARDWARE_1M_TS_FROM_AC_DELAY,
    HARDWARE_2M_TS_FROM_AC_DELAY,
    HARDWARE_CODED_TS_FROM_AC_DELAY,//s2
    HARDWARE_CODED_TS_FROM_AC_DELAY,//s8
};
static _u16 rxAirToAC[4] = 
{
    HARDWARE_AIR_TO_AC_TIME_1M,
    HARDWARE_AIR_TO_AC_TIME_2M,
    HARDWARE_AIR_TO_AC_TIME_CODED,//s2
    HARDWARE_AIR_TO_AC_TIME_CODED,//s8
};

static _u16 txHWPrepeaeTime[4] = 
{
    HARDWARE_TX_PREPARE_TIME_1M,
    HARDWARE_TX_PREPARE_TIME_2M,
    HARDWARE_TX_PREPARE_TIME_CODED,//s2
    HARDWARE_TX_PREPARE_TIME_CODED,//s8
};

static _u16 rxHWPrepeaeTime[4] = 
{
    HARDWARE_RX_PREPARE_TIME_1M,
    HARDWARE_RX_PREPARE_TIME_2M,
    HARDWARE_RX_PREPARE_TIME_CODED,//s2
    HARDWARE_RX_PREPARE_TIME_CODED,//s8
};

/******************hal rf access code setting*******/
_RAM_CODE
void hal_rf_set_access_code(_u32 accessCode)
{
    rf_access_code_comm(accessCode);
}

/******************hal rf crc setting***************/
_RAM_CODE
void hal_rf_set_crc_value(_u32 crc)
{
    rf_set_ble_crc_value(crc);
}

/******************hal rf channel setting***********/
_RAM_CODE
void hal_rf_set_channel_index(_u8 chn)
{
    rf_set_ble_chn((_s8)chn);
}

/******************hal rf power setting*************/
_DATA static _u8 hal_rf_power[24]=
{
    RF_POWER_P9p15dBm,
    RF_POWER_P8p25dBm,
    RF_POWER_P7p00dBm,
    RF_POWER_P6p32dBm,
    RF_POWER_P5p21dBm,
    RF_POWER_P4p02dBm,
    RF_POWER_P3p00dBm,
    RF_POWER_P2p01dBm,
    RF_POWER_P1p03dBm,
    RF_POWER_P0p01dBm,
    RF_POWER_N1p15dBm,
    RF_POWER_N1p52dBm,
    RF_POWER_N2p51dBm,
    RF_POWER_N3p95dBm,
    RF_POWER_N5p94dBm,
    RF_POWER_N9p03dBm,
    RF_POWER_N30dBm,
};

_RAM_CODE
void hal_rf_set_power(_u8 power)
{
    rf_set_power_level(hal_rf_power[power]);
}

/******************hal rf phy setting***************/
_RAM_CODE
void hal_rf_set_coded_phy_s2(void)
{
    rf_set_ble_500K_mode();
}

_RAM_CODE
void hal_rf_set_coded_phy_s8(void)
{
    rf_set_ble_125K_mode();
}

_RAM_CODE
void hal_rf_set_1M_phy(void)
{
    rf_set_ble_1M_mode();
}

_RAM_CODE
void hal_rf_set_2M_phy(void)
{
    rf_set_ble_2M_mode();
}
/******************hal rf process***************/
_RAM_CODE
void hal_rf_stop(void)
{
    reg_rf_ll_cmd = 0x80;
}


/******************hal rf tx setting***************/

_RAM_CODE
void hal_rf_tx(_u8* address,_u32 time)
{
	*(_u32*)address = rf_tx_packet_dma_len((_u16)address[5]+2);
	rf_set_tx_settle_time(52);
    rf_start_stx(address,time*CLOCK_TICK_US);
}
/******************hal rf rx setting***************/
_RAM_CODE
void hal_rf_set_rx_timeout(_u32 time)
{
	reg_rf_ll_rx_fst_timeout = time;
}

_RAM_CODE
void hal_rf_set_rx_max_len(_u8 len)
{
    rf_set_rx_maxlen(len);
}

_RAM_CODE
void hal_rf_rx(_u8* address,_u32 maxOctets,_u32 time)
{
    rf_set_rx_dma(address,0,maxOctets);
    rf_set_rx_settle_time(45);
    rf_start_srx(time*CLOCK_TICK_US);
}

_RAM_CODE
_u32 hal_rf_get_rx_air_timestamp(_u8 phy)
{
    return (_u32)(system_switch_tick_to_time(reg_rf_timestamp) - rxAcToTs[phy] - rxAirToAC[phy]);
}

_RAM_CODE
_u32 hal_rf_get_rx_hw_prepare_time(_u8 phy)
{
    return ((_u32)rxHWPrepeaeTime[phy]);
}

_RAM_CODE
_u32 hal_rf_get_tx_hw_prepare_time(_u8 phy)
{
    return (_u32)txHWPrepeaeTime[phy];
}

_RAM_CODE
_u32 hal_rf_is_packet_valid(_u8* packet)
{
    if(packet)
    {
        if(*((unsigned int *)packet) == (packet[5]+13) && (0x00 == (packet[packet[5]+5+11]&0x01)))
        {
            return 1;
        }
    }
    return 0;
}
/******************hal rf packet setting***************/
_RAM_CODE
_u32 hal_rf_get_hw_packet_tx_extra_len(void)
{
	return 4;// 4 dma length offset
}

_RAM_CODE
_u32 hal_rf_get_hw_tx_header_offset(void)
{
	return 4;// 4 dma length offset
}

_RAM_CODE
_u32 hal_rf_get_hw_packet_rx_extra_len(void)
{
	return 15;// 4 dma length offset,3 crc offset,4 timestamp offset,4 extra info
}

_RAM_CODE
_u32 hal_rf_get_hw_rx_header_offset(void)
{
	return 4;// 4 dma length offset
}

/******************hal rf irq process**************/
_RAM_CODE
void rf_irq_handler(void)
{
    if(rf_get_irq_status(FLD_RF_IRQ_TX))
    {
        if(hal_rf_cb)
        {
            hal_rf_cb(HAL_RF_IRQ_TX);
        }
        rf_clr_irq_status(FLD_RF_IRQ_TX);
    }
    else if(rf_get_irq_status(FLD_RF_IRQ_RX))
    {
        if(hal_rf_cb)
        {
            hal_rf_cb(HAL_RF_IRQ_RX);
        }
        rf_clr_irq_status(FLD_RF_IRQ_RX);
    }
    else if(rf_get_irq_status(FLD_RF_IRQ_RX_TIMEOUT))
    {
        if(hal_rf_cb)
        {
            hal_rf_cb(HAL_RF_IRQ_RX_TIMEOUT);
        }
        rf_clr_irq_status(FLD_RF_IRQ_RX_TIMEOUT);
    }
    else if(rf_get_irq_status(FLD_RF_IRQ_FIRST_TIMEOUT))
    {
        if(hal_rf_cb)
        {
            hal_rf_cb(HAL_RF_IRQ_RX_TIMEOUT);
        }
        rf_clr_irq_status(FLD_RF_IRQ_FIRST_TIMEOUT);
    }
    else
    {
        rf_clr_irq_status(FLD_RF_IRQ_ALL);
    }
}
PLIC_ISR_REGISTER(rf_irq_handler, IRQ_ZB_RT)

/******************hal rf init setting*************/

typedef struct
{
    unsigned short cal_tbl[81];
    rf_ldo_trim_t   ldo_trim;
    rf_rccal_cal_t  rccal_cal;
    unsigned char tx_fast_en;
    unsigned char rx_fast_en;
}Fast_Settle;

Fast_Settle fast_settle_1M;
Fast_Settle fast_settle_2M;
Fast_Settle fast_settle_S2;
Fast_Settle fast_settle_S8;

void blt_rf_fast_settle_get_val(rf_tx_fast_settle_time_e tx_settle_us, rf_rx_fast_settle_time_e rx_settle_us, rf_fast_settle_t *fs_cv)
{
    //tx
    rf_set_tx_rx_off(); //STOP_RF_STATE_MACHINE;
    rf_clr_irq_status(FLD_RF_IRQ_ALL);
    rf_set_tx_settle_time(113);        //adjust TX settle time
    for (unsigned char f_chn = 0; f_chn <= 80; f_chn++)
    {
        rf_set_chn(f_chn);
        rf_set_txmode();
        delay_us(113); //Wait for calibration to stabilize
        rf_tx_fast_settle_get_cal_val(tx_settle_us, f_chn, fs_cv);

        rf_set_tx_rx_off(); //STOP_RF_STATE_MACHINE;
        rf_clr_irq_status(FLD_RF_IRQ_ALL);
    }
    //rx
    rf_set_rx_settle_time(85); //adjust RX settle time
    rf_set_rxmode();
    delay_us(85); //Wait for the rx packetization action to complete
    rf_rx_fast_settle_get_cal_val(rx_settle_us, 0, fs_cv);
    rf_set_tx_rx_off(); //STOP_RF_STATE_MACHINE;
    rf_clr_irq_status(FLD_RF_IRQ_ALL);
}

void blc_ll_initFastSettle(_u8 tx_fast_en, _u8 rx_fast_en)
{
    if (tx_fast_en || rx_fast_en)
    {
        /* default 1M */
    	hal_rf_set_1M_phy();
        blt_rf_fast_settle_get_val(TX_SETTLE_TIME_50US, RX_SETTLE_TIME_45US, (rf_fast_settle_t *)&fast_settle_1M);

        /* get 2M fast settle calib value */
        hal_rf_set_2M_phy();
        blt_rf_fast_settle_get_val(TX_SETTLE_TIME_50US, RX_SETTLE_TIME_45US, (rf_fast_settle_t *)&fast_settle_2M);

        /* get S2 fast settle calib value */
        hal_rf_set_coded_phy_s2();
        blt_rf_fast_settle_get_val(TX_SETTLE_TIME_50US, RX_SETTLE_TIME_45US, (rf_fast_settle_t *)&fast_settle_S2);

        /* get S8 fast settle calib value */
        hal_rf_set_coded_phy_s8();
        blt_rf_fast_settle_get_val(TX_SETTLE_TIME_50US, RX_SETTLE_TIME_45US, (rf_fast_settle_t *)&fast_settle_S8);
    }
    extern rf_fast_settle_t* g_fast_settle_cal_val_ptr;
    g_fast_settle_cal_val_ptr = (rf_fast_settle_t *)&fast_settle_1M;
    rf_tx_fast_settle_set_cal_val(TX_SETTLE_TIME_50US, 0, (rf_fast_settle_t *)&fast_settle_1M);
    rf_rx_fast_settle_set_cal_val(RX_SETTLE_TIME_45US, 0, (rf_fast_settle_t *)&fast_settle_1M);

    /* reserve here */
    reg_rf_ll_cmd = 0x80;
    rf_set_tx_rx_off();
    CLEAR_ALL_RFIRQ_STATUS;


    rf_fast_settle_config(TX_SETTLE_TIME_50US, RX_SETTLE_TIME_45US);
    if (tx_fast_en) {
        rf_set_tx_settle_time(TX_SETTLE_TIME_50US);
        rf_tx_fast_settle_en();
    }

    if (rx_fast_en) {
        rf_set_rx_settle_time(RX_SETTLE_TIME_45US);
        rf_rx_fast_settle_en();
    }

	hal_rf_set_1M_phy();

    /* make sure "fast_settle.tx_fast_en" set after "rf_set ble_channel" above,
         * because  "rf_set ble_channel" internal will judge if fast settle prepared OK */
    fast_settle_1M.tx_fast_en = tx_fast_en;
    fast_settle_1M.rx_fast_en = rx_fast_en;

    fast_settle_2M.tx_fast_en = tx_fast_en;
    fast_settle_2M.rx_fast_en = rx_fast_en;

    fast_settle_S2.tx_fast_en = tx_fast_en;
    fast_settle_S2.rx_fast_en = rx_fast_en;

    fast_settle_S8.tx_fast_en = tx_fast_en;
    fast_settle_S8.rx_fast_en = rx_fast_en;

    fence_iorw;
    CLEAR_ALL_RFIRQ_STATUS;
    fence_iorw;
}


#define reg_bb_dbg_sel      REG_ADDR16(0x140378)
#define reg_bb_dbg_sel_l    REG_ADDR8(0x140378)
#define reg_bb_dbg_sel_h    REG_ADDR8(0x140379)
void dbg_bb_set_pin(gpio_pin_e pin)
{
    unsigned char mask = 0x3F;//bit5~bit0
    unsigned char val = 0x39;//function select DBG_BB

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

_RAM_CODE
void hal_rf_init(void(*cb)(_u8))
{
    rf_mode_init();
    rf_set_ble_1M_mode();
	hal_rf_set_power(HAL_RF_POWER_P4dBm);
	rf_set_tx_wait_time(0);
	rf_set_rx_wait_time(0);
	blc_ll_initFastSettle(1,1);
    plic_set_priority(IRQ_ZB_RT, 2);
    rf_set_irq_mask(FLD_RF_IRQ_RX|FLD_RF_IRQ_TX|FLD_RF_IRQ_RX_TIMEOUT|FLD_RF_IRQ_FIRST_TIMEOUT);
    plic_interrupt_enable(IRQ_ZB_RT);
    rf_clr_irq_status(FLD_RF_IRQ_ALL);
    rf_set_tx_dma(0, 272);
    if(cb)
    {
        hal_rf_cb = cb;
    }
}


