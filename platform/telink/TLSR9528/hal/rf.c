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
#define HARDWARE_1M_TX_PREPARE_DELAY         99
#define HARDWARE_2M_TX_PREPARE_DELAY         103
#define HARDWARE_CODED_TX_PREPARE_DELAY      107

//rf RX prepare delay
#define HARDWARE_1M_RX_PREPARE_DELAY         85
#define HARDWARE_2M_RX_PREPARE_DELAY         85
#define HARDWARE_CODED_RX_PREPARE_DELAY      85

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
#define HARDWARE_TX_PREPARE_TIME_2M                (HARDWARE_2M_TX_PREPARE_DELAY+HARDWARE_2M_PREAMBLE_DELAY)
#define HARDWARE_TX_PREPARE_TIME_CODED             (HARDWARE_CODED_TX_PREPARE_DELAY+HARDWARE_CODED_EXTRA_PREAMBLE_DELAY)

#define HARDWARE_RX_PREPARE_TIME_1M                (HARDWARE_1M_RX_PREPARE_DELAY)
#define HARDWARE_RX_PREPARE_TIME_2M                (HARDWARE_2M_RX_PREPARE_DELAY)
#define HARDWARE_RX_PREPARE_TIME_CODED             (HARDWARE_CODED_RX_PREPARE_DELAY)

//preamble and access code
#define HARDWARE_AIR_TO_HEADER_TIME_1M             (HARDWARE_1M_PREAMBLE_DELAY     + HARDWARE_1M_AC_DELAY)
#define HARDWARE_AIR_TO_HEADER_TIME_2M             (HARDWARE_2M_PREAMBLE_DELAY     + HARDWARE_2M_AC_DELAY)
#define HARDWARE_AIR_TO_HEADER_TIME_CODED          (HARDWARE_CODED_PREAMBLE_DELAY  + HARDWARE_CODED_AC_DELAY)

//hareware from trigger to packet header time
#define HARDWARE_TX_TRIGGER_TO_HEADER_TIME_1M      (HARDWARE_TX_PREPARE_TIME_1M    + HARDWARE_AIR_TO_HEADER_TIME_1M)
#define HARDWARE_TX_TRIGGER_TO_HEADER_TIME_2M      (HARDWARE_TX_PREPARE_TIME_2M    + HARDWARE_AIR_TO_HEADER_TIME_2M)
#define HARDWARE_TX_TRIGGER_TO_HEADER_TIME_CODED   (HARDWARE_TX_PREPARE_TIME_CODED + HARDWARE_AIR_TO_HEADER_TIME_CODED)

#define HARDWARE_RX_TRIGGER_TO_HEADER_TIME_1M      (HARDWARE_RX_PREPARE_TIME_1M    + HARDWARE_AIR_TO_HEADER_TIME_1M)
#define HARDWARE_RX_TRIGGER_TO_HEADER_TIME_2M      (HARDWARE_RX_PREPARE_TIME_2M    + HARDWARE_AIR_TO_HEADER_TIME_2M)
#define HARDWARE_RX_TRIGGER_TO_HEADER_TIME_CODED   (HARDWARE_RX_PREPARE_TIME_CODED + HARDWARE_AIR_TO_HEADER_TIME_CODED)

static _u16 rxAcToTs[4] = 
{
    HARDWARE_1M_TS_FROM_AC_DELAY,
    HARDWARE_2M_TS_FROM_AC_DELAY,
    HARDWARE_CODED_TS_FROM_AC_DELAY,//s2
    HARDWARE_CODED_TS_FROM_AC_DELAY,//s8
};
static _u16 rxTriggerToHeader[4] = 
{
    HARDWARE_RX_TRIGGER_TO_HEADER_TIME_1M,
    HARDWARE_RX_TRIGGER_TO_HEADER_TIME_2M,
    HARDWARE_RX_TRIGGER_TO_HEADER_TIME_CODED,//s2
    HARDWARE_RX_TRIGGER_TO_HEADER_TIME_CODED,//s8
};

static _u16 txTriggerToHeader[4] = 
{
    HARDWARE_TX_TRIGGER_TO_HEADER_TIME_1M,
    HARDWARE_TX_TRIGGER_TO_HEADER_TIME_2M,
    HARDWARE_TX_TRIGGER_TO_HEADER_TIME_CODED,//s2
    HARDWARE_TX_TRIGGER_TO_HEADER_TIME_CODED,//s8
};

/******************hal rf access code setting*******/
void hal_rf_set_access_code(_u32 accessCode)
{
    rf_access_code_comm(accessCode);
}

/******************hal rf crc setting***************/
void hal_rf_set_crc_value(_u32 crc)
{
    rf_set_ble_crc_value(crc);
}

/******************hal rf channel setting***********/
void hal_rf_set_channel_index(_u8 chn)
{
    rf_set_ble_chn((_s8)chn);
}

/******************hal rf power setting*************/
_DATA static _u8 hal_rf_power[20]=
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
_RAM_CODE void hal_rf_set_power(_u8 power)
{
    rf_set_power_level(hal_rf_power[power]);
}

/******************hal rf phy setting***************/
void hal_rf_set_coded_phy_s2(void)
{
    rf_set_ble_500K_mode();
}

void hal_rf_set_coded_phy_s8(void)
{
    rf_set_ble_125K_mode();
}

void hal_rf_set_1M_phy(void)
{
    rf_set_ble_1M_mode();
}

void hal_rf_set_2M_phy(void)
{
    rf_set_ble_2M_mode();
}
/******************hal rf process***************/
void hal_rf_stop(void)
{
    if(read_reg8(0x170224)!=FLD_RF_STATE_MACHINE_IDLE)
    {
        reg_rf_ll_cmd = 0x80;
    }
}
/******************hal rf tx setting***************/
void hal_rf_tx(_u8* address,_u32 time)
{
    rf_start_stx(address,time*CLOCK_TICK_US);
}
/******************hal rf rx setting***************/
void hal_rf_set_rx_timeout(_u32 time)
{
    rf_set_rx_timeout(time);
}
void hal_rf_rx(_u8* address,_u32 maxOctets,_u32 time)
{
    rf_set_rx_dma(address,0,maxOctets);
    rf_start_srx(time*CLOCK_TICK_US);
}

_u32 hal_rf_get_rx_header_timestamp(_u8 phy)
{
    return (_u32)(system_switch_tick_to_time(reg_rf_timestamp) - rxAcToTs[phy]);
}

_u32 hal_rf_get_rx_trigger_to_header_time(_u8 phy)
{
    return ((_u32)rxTriggerToHeader[phy]);
}

_u32 hal_rf_get_tx_trigger_to_header_time(_u8 phy)
{
    return (_u32)txTriggerToHeader[phy];
}

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
/******************hal rf irq process**************/
_RAM_CODE void rf_irq_handler(void)
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
    else
    {
        rf_clr_irq_status(FLD_RF_IRQ_ALL);
    }
}
PLIC_ISR_REGISTER(rf_irq_handler, IRQ_ZB_RT)

/******************hal rf init setting*************/
void hal_rf_init(void(*cb)(_u8))
{
    rf_mode_init();
    rf_set_ble_1M_mode();
    rf_set_tx_dma(0, 272);
    if(cb)
    {
        hal_rf_cb = cb;
    }
}


