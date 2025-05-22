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

/******************hal rf access code setting*******/
_RAM_CODE void hal_rf_set_access_code(_u32 accessCode)
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

/******************hal rf phy setting************** */
void hal_rf_set_coded_phy(_u8 CI)
{
    if(CI == (_u8)HAL_RF_CODED_PHY_S2)
    {
        rf_set_ble_500K_mode();
    }
    else if(CI == (_u8)HAL_RF_CODED_PHY_S8)
    {
        rf_set_ble_125K_mode();
    }
}

void hal_rf_set_1M_phy(void)
{
    rf_set_ble_1M_mode();
}

void hal_rf_set_2M_phy(void)
{
    rf_set_ble_2M_mode();
}

/******************hal rf tx setting************** */
void hal_rf_tx(_u8* address,_u32 time)
{
    rf_start_stx(address,time*CLOCK_TICK_US);
}
/******************hal rf rx setting************** */
void hal_rf_set_rx_timeout(_u32 time)
{
    rf_set_rx_timeout(time);
}
void hal_rf_rx(_u8* address,_u32 maxOctets,_u32 time)
{
    rf_set_rx_dma(address,0,maxOctets);
    rf_start_srx(time*CLOCK_TICK_US);
}

/******************hal rf init setting************** */
void hal_rf_init(void)
{
    rf_mode_init();
    rf_set_ble_1M_mode();
    rf_set_tx_dma(0, 272);
}


