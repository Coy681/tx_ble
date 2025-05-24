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

#define HARDWARE_DELAY_1M  (19+16+32)
#define HARDWARE_DELAY_2M  (13+24+16)
#define HARDWARE_DELAY_S2  (14+80+256)
#define HARDWARE_DELAY_S8  (14+80+256)

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

_u32 hal_rf_delay[4] = {HARDWARE_DELAY_1M,
                        HARDWARE_DELAY_2M,
                        HARDWARE_DELAY_S2,
                        HARDWARE_DELAY_S8};

_u32 hal_rf_get_rx_timestamp(_u8 phy)
{
    return (_u32)(system_switch_tick_to_time(reg_rf_timestamp) - hal_rf_delay[phy]);
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


