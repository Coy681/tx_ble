/*
 * rf.h
 *
 *  Created on: 2024年12月4日
 *      Author: 12407
 */

#ifndef HAL_RF_H_
#define HAL_RF_H_
#include"tx_common.h"

//preamble delay
#define HARDWARE_1M_PREAMBLE_DELAY           8
#define HARDWARE_2M_PREAMBLE_DELAY           8
#define HARDWARE_CODED_PREAMBLE_DELAY        80

//access delay offset
#define HARDWARE_1M_AC_DELAY                 32
#define HARDWARE_2M_AC_DELAY                 16
#define HARDWARE_CODED_AC_DELAY              256

//packet time for different phy
#define HARDWARE_OCTET_TIME_1M               8
#define HARDWARE_OCTET_TIME_2M               4
#define HARDWARE_OCTET_TIME_CODED_S2         16
#define HARDWARE_OCTET_TIME_CODED_S8         64


void hal_rf_set_access_code(_u32 accessCode);

void hal_rf_set_crc_value(_u32 crc);

void hal_rf_set_channel_index(_u8 chn);

enum 
{
    HAL_RF_POWER_P9dBm = 0,
    HAL_RF_POWER_P8dBm,
    HAL_RF_POWER_P7dBm,
    HAL_RF_POWER_P6dBm,
    HAL_RF_POWER_P5dBm,
    HAL_RF_POWER_P4dBm,
    HAL_RF_POWER_P3dBm,
    HAL_RF_POWER_P2dBm,
    HAL_RF_POWER_P1dBm,
    HAL_RF_POWER_0dBm ,
    HAL_RF_POWER_N1dBm,
    HAL_RF_POWER_N2dBm,
    HAL_RF_POWER_N3dBm,
    HAL_RF_POWER_N4dBm,
    HAL_RF_POWER_N5dBm,
    HAL_RF_POWER_N6dBm,
    HAL_RF_POWER_N7dBm,
    HAL_RF_POWER_N8dBm,
};
void hal_rf_set_power(_u8 power);

enum
{
    HAL_RF_MODE_1M       = 0x00,
    HAL_RF_MODE_2M       = 0x01,
    HAL_RF_MODE_CODED_S2 = 0x02,
    HAL_RF_MODE_CODED_S8 = 0x03,
};
void hal_rf_set_coded_phy_s2(void);

void hal_rf_set_coded_phy_s8(void);

void hal_rf_set_1M_phy(void);

void hal_rf_set_2M_phy(void);

void hal_rf_tx(_u8* address,_u32 time);

void hal_rf_set_rx_timeout(_u32 time);

_u32 hal_rf_get_rx_header_timestamp(_u8 phy);

_u32 hal_rf_get_rx_trigger_to_header_time(_u8 phy);

_u32 hal_rf_get_tx_trigger_to_header_time(_u8 phy);

_u32 hal_rf_is_packet_valid(_u8* packet);

void hal_rf_rx(_u8* address,_u32 maxOctets,_u32 time);

void hal_rf_stop(void);

enum
{
    HAL_RF_IRQ_TX,
    HAL_RF_IRQ_RX,
    HAL_RF_IRQ_RX_TIMEOUT,
};
void hal_rf_init(void(*cb)(_u8));

#endif /* HAL_RF_H_ */
