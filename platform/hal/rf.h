/*
 * rf.h
 *
 *  Created on: 2024年12月4日
 *      Author: 12407
 */

#ifndef HAL_RF_H_
#define HAL_RF_H_
#include"tx_common.h"

//octet time for different phy
#define RF_PACKET_OCTET_TIME_1M                8
#define RF_PACKET_OCTET_TIME_2M                4
#define RF_PACKET_OCTET_TIME_CODED_S2          16
#define RF_PACKET_OCTET_TIME_CODED_S8          64

//preamble len and time define 
#define RF_PACKET_PREAMBLE_LEN_1M              1
#define RF_PACKET_PREAMBLE_LEN_2M              2
#define RF_PACKET_PREAMBLE_LEN_CODED_S2        10
#define RF_PACKET_PREAMBLE_LEN_CODED_S8        10
#define RF_PACKET_PREAMBLE_TIME_1M             8
#define RF_PACKET_PREAMBLE_TIME_2M             8
#define RF_PACKET_PREAMBLE_TIME_CODED_S2       80
#define RF_PACKET_PREAMBLE_TIME_CODED_S8       80

//access code len and time define 
#define RF_PACKET_ACCESS_CODE_LEN              4
#define RF_PACKET_ACCESS_CODE_TIME_1M          32
#define RF_PACKET_ACCESS_CODE_TIME_2M          16
#define RF_PACKET_ACCESS_CODE_TIME_CODED_S2    256
#define RF_PACKET_ACCESS_CODE_TIME_CODED_S8    256

//crc time len and time define 
#define RF_PACKET_CRC_LEN                      3
#define RF_PACKET_CRC_TIME_1M                  24
#define RF_PACKET_CRC_TIME_2M                  12
#define RF_PACKET_CRC_TIME_CODED_S2            48
#define RF_PACKET_CRC_TIME_CODED_S8            192

//coded ci time define
#define RF_PACKET_CI_TIME_CODED_S2             16
#define RF_PACKET_CI_TIME_CODED_S8             16

//term1 time define 
#define RF_PACKET_TERM1_TIME_CODED_S2          24
#define RF_PACKET_TERM1_TIME_CODED_S8          24

//term2 time define
#define RF_PACKET_TERM2_TIME_CODED_S2          6
#define RF_PACKET_TERM2_TIME_CODED_S8          24

/*******************************config rf *********************************/
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

void hal_rf_set_rx_timeout(_u32 time);

void hal_rf_set_rx_max_len(_u8 len);

/*******************************get rf info *********************************/
_u32 hal_rf_get_rx_hw_prepare_time(_u8 phy);

_u32 hal_rf_get_tx_hw_prepare_time(_u8 phy);

_u32 hal_rf_get_rx_air_timestamp(_u8 phy);

_u32 hal_rf_is_packet_valid(_u8* packet);

_u32 hal_rf_get_hw_packet_tx_extra_len(void);

_u32 hal_rf_get_hw_tx_header_offset(void);

_u32 hal_rf_get_hw_packet_rx_extra_len(void);

_u32 hal_rf_get_hw_rx_header_offset(void);

/*******************************operate rf *********************************/
void hal_rf_tx(_u8* address,_u32 time);

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
