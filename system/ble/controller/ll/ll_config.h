#include"ll_feature.h"

#ifndef BLE_PHY_ADV_MAX_TX_LEN
#define BLE_PHY_ADV_MAX_TX_LEN                     128
#endif

#if(LL_SUPPORT_LE_EXTENDED_ADVERTISING!=1)
#ifndef BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS
#define BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS       1//fixed value,can not configurable
#endif

#ifndef BLE_ADV_MAXIMUM_ADVERTISING_DATA_LENGTH
#define BLE_ADV_MAXIMUM_ADVERTISING_DATA_LENGTH    31//fixed value,can not configurable
#endif
#else

#ifndef BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS 
#define BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS       4
#endif

#ifndef BLE_ADV_MAXIMUM_ADVERTISING_DATA_LENGTH
#define BLE_ADV_MAXIMUM_ADVERTISING_DATA_LENGTH    512
#endif

#endif
