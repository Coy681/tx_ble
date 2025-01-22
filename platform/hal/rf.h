/*
 * rf.h
 *
 *  Created on: 2024年12月4日
 *      Author: 12407
 */

#ifndef HAL_RF_H_
#define HAL_RF_H_
#include"tx_common.h"

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
    
}hal_rf_power_e;

typedef enum
{
    HAL_RF_PHY_1M = 0,
    HAL_RF_PHY_2M,
    HAL_RF_PHY_CODED_S2,
    HAL_RF_PHY_CODED_S8,
}hal_rf_phy_e;

typedef struct 
{
    _u32           accessCode;
    _u32           crc;
    hal_rf_power_e power;
    hal_rf_phy_e   phy;
    _u8*           txAddress;
    _u8*           rxAddress;
    _u16           updateMask;
}hal_rf_parameter_t;

typedef enum
{
    HAL_RF_PARAM_ACCESS_CODE,
    HAL_RF_PARAM_CRC,
    HAL_RF_PARAM_PHY,
    HAL_RF_PARAM_POWER,
    HAL_RF_PARAM_TX_ADDRESS,
    HAL_RF_PARAM_RX_ADDRESS,
}hal_rf_parameter_e;

typedef struct hal_rf_t
{
    hal_rf_parameter_t param;
    void               (*updateParam)(hal_rf_parameter_e param,void* value);
    void               (*txPrepare)(_u8*);
    void               (*rxPrepare)(_u8*);
    void               (*tx)(_u32);
    void               (*rx)(_u32,_u32);
}hal_rf_t;

#define HAL_RF_SET_ADDRESS      0
#define HAL_RF_GET_ADDRESS      1

void hal_rf_object_cast(hal_rf_t* object);

void hal_rf_init(hal_rf_t* object,_u32 accessCode,_u32 crc);


#endif /* HAL_RF_H_ */
