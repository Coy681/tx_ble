/*
 * rf.h
 *
 *  Created on: 2024年12月4日
 *      Author: 12407
 */

#ifndef HAL_RF_H_
#define HAL_RF_H_
#include"tx_common.h"

void hal_rf_set_access_code(_u32 accessCode);

void hal_rf_set_crc_value(_u32 crc);

void hal_rf_set_channel_index(_u8 chn);

typedef enum 
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
    HAL_RF_CODED_PHY_S2 = 0x00,
    HAL_RF_CODED_PHY_S8 = 0x01,
};
void hal_rf_set_coded_phy(_u8 CI);

void hal_rf_set_1M_phy(void);

void hal_rf_set_2M_phy(void);

void hal_rf_tx(_u8* address,_u32 time);

void hal_rf_set_rx_timeout(_u32 time);

void hal_rf_rx(_u8* address,_u32 maxOctets,_u32 time);

void hal_rf_init(void);



#endif /* HAL_RF_H_ */
