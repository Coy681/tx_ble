/*
 * rf.c
 *
 *  Created on: 2024年12月4日
 *      Author: 12407
 */

#include"../../hal/rf.h"
#include"driver.h"
#include"config.h"
typedef void(*hal_rf_f)(void);

static hal_rf_t *rf;

_RAM_CODE void hal_rf_object_cast(hal_rf_t* object)
{
    rf = object;
}

/******************hal rf access code setting************** */
_RAM_CODE static void hal_rf_set_access_code(void)
{
	LOG_TRACE(1,"access code value",(_u8*)&rf->param.accessCode,4);
    rf_access_code_comm(rf->param.accessCode);
}

/******************hal rf channel setting************** */
_RAM_CODE static void hal_rf_set_channel(void)
{
	LOG_TRACE(1,"channel value",(_u8*)&rf->param.channel,2);
    rf_set_ble_chn((_s8)rf->param.channel);
}

/******************hal rf crc setting************** */
_RAM_CODE static void hal_rf_set_crc(void)
{
	LOG_TRACE(1,"crc value",(_u8*)&rf->param.crc,4);
    rf_set_ble_crc_value(rf->param.crc);
}

/******************hal rf phy setting************** */
_DATA static hal_rf_f hal_rf_phy[4]=
{
    rf_set_ble_1M_mode,
    rf_set_ble_2M_mode,
    rf_set_ble_500K_mode,
    rf_set_ble_125K_mode,
};
_RAM_CODE static void hal_rf_set_phy(void)
{
    static _u8 phyMode = 0xff;
    if(phyMode!=rf->param.phy)
    {
    	LOG_TRACE(1,"phy value",(_u8*)&rf->param.phy,4);
        phyMode = rf->param.phy;
        hal_rf_phy[rf->param.phy]();
    }
}

/******************hal rf power setting************** */
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
_RAM_CODE static void hal_rf_set_power(void)
{
    static _u8 power = RF_POWER_P0p01dBm;
    if(power!=rf->param.power)
    {
        power = rf->param.power;
        rf_set_power_level(hal_rf_power[power]);
    }
}

/*******************define rf object function*********************/
_RAM_CODE static void hal_rf_update_parameter(hal_rf_parameter_e param,_u32 value)
{
    if(param>HAL_RF_PARAM_MAX)
    {
        return;
    }
    switch(param)
    {
        case HAL_RF_PARAM_ACCESS_CODE:
            rf->param.accessCode = value;
            break;
        case HAL_RF_PARAM_CRC:
            rf->param.crc = value;
            break;
        case HAL_RF_PARAM_PHY:
            rf->param.power = value;
            break;
        case HAL_RF_PARAM_POWER:
            rf->param.phy = value;
            break;
        case HAL_RF_PARAM_CHANNEL:
            rf->param.channel = (_u16)value;
            break;
        case HAL_RF_PARAM_MAX_RX_SIZE:
            rf->param.maxRxSize = (_u16)value;
            break;
        default:
            break;
    }
}

_RAM_CODE static void hal_rf_prepare(void)
{
    hal_rf_set_power();
    hal_rf_set_channel();
    hal_rf_set_access_code();
    hal_rf_set_crc();
    hal_rf_set_phy();
}

_RAM_CODE static void hal_rf_tx(_u32 txClock,_u8* txAddress)
{
    rf_start_stx(txAddress,txClock);
}

_RAM_CODE static void hal_rf_rx(_u32 rxClock,_u32 expireUs,_u8* rxAddress)
{
    rf_set_rx_timeout(expireUs);
    rf_set_rx_dma(rxAddress,0,rf->param.maxRxSize);
    rf_start_srx(rxClock);
}

_RAM_CODE void hal_rf_register_task(hal_rf_t* object,_u32 accessCode,_u32 crc)
{
	object->param.accessCode = accessCode;
	object->param.crc        = crc;
	object->rfrepare         = hal_rf_prepare;
	object->rx               = hal_rf_rx;
	object->tx               = hal_rf_tx;
	object->updateParam      = hal_rf_update_parameter;
}

_RAM_CODE void hal_rf_hardware_init(void)
{
    rf_mode_init();
    rf_set_ble_1M_mode();
    rf_set_tx_dma(0, 272);

}



