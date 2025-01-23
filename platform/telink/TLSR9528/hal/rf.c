/*
 * rf.c
 *
 *  Created on: 2024年12月4日
 *      Author: 12407
 */

#include"../../hal/rf.h"
#include"driver.h"

static hal_rf_t *rf;

void hal_rf_object_cast(hal_rf_t* object)
{
    rf = object;
}

/******************hal rf access code setting************** */
static void hal_rf_set_access_code(void)
{
    rf_access_code_comm(rf->param.accessCode);
}

/******************hal rf channel setting************** */
static void hal_rf_set_channel(void)
{
    rf_set_ble_chn((_s8)rf->param.channel);
}

/******************hal rf crc setting************** */
static void hal_rf_set_crc(void)
{
    rf_set_ble_crc_value(rf->param.crc&0xffffff);
}

/******************hal rf phy setting************** */
hal_rf_f hal_rf_phy[4]=
{
    rf_set_ble_1M_mode,
    rf_set_ble_2M_mode,
    rf_set_ble_500K_mode,
    rf_set_ble_125K_mode,
};
void hal_rf_set_phy(void)
{
    static _u8 phyMode = 0xff;
    if(phyMode!=rf->param.phy)
    {
        phyMode = rf->param.phy;
        hal_rf_phy[rf->param.phy]();
    }
}


/******************hal rf power setting************** */
static _u8 hal_rf_power[20]=
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
}
void hal_rf_set_power(void)
{
    static _u8 power = RF_POWER_P0p01dBm;
    if(power!=rf->param.power)
    {
        power = rf->param.power;
        rf_set_power_level(hal_rf_power[power]);
    }
}

/*******************define rf object function*********************/
void hal_rf_update_parameter(hal_rf_parameter_e param,void value)
{
    if(param>HAL_RF_PARAM_MAX)
    {
        return;
    }
    switch(param)
    {
        case HAL_RF_PARAM_ACCESS_CODE:
            rf->param.accessCode = (_u32)value;
            break;
        case HAL_RF_PARAM_CRC:
            rf->param.crc = (_u32)value;
            break;
        case HAL_RF_PARAM_PHY:
            rf->param.power = (_u32)value;
            break;
        case HAL_RF_PARAM_POWER:
            rf->param.phy = (_u32)value;
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

void hal_rf_prepare(void)
{
    hal_rf_set_power();
    hal_rf_set_channel();
    hal_rf_set_access_code();
    hal_rf_set_crc();
}

void hal_rf_tx(_u32 txClock,_u8* txAddress)
{
    rf_start_stx(txAddress,txClock);
}

void hal_rf_rx(_u32 rxClock,_u32 expireUs,_u8* rxAddress)
{
    rf_set_rx_timeout(expireUs);
    rf_set_rx_dma(rxAddress,0,rf->param.maxRxSize);
    rf_start_srx(rxClock);
}

void hal_rf_register_task(hal_rf_t* object,_u32 accessCode,_u32 crc)
{
	object->param.accessCode = accessCode;
	object->param.crc        = crc;
	object->rfrepare         = hal_rf_prepare;
	object->rx               = hal_rf_rx;
	object->tx               = hal_rf_tx;
	object->updateParam      = hal_rf_update_parameter();
}

void hal_rf_hardware_init(void)
{
    rf_mode_init();
    rf_set_tx_dma(0, 272);
}



