#include"ll_feature.h"



#if(LL_SUPPORT_LE_EXTENDED_ADVERTISING!=1)
#ifndef BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS
#define BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS       1//fixed value,not configurable
#endif

#ifndef BLE_ADV_MAXIMUM_ADVERTISING_DATA_LENGTH
#define BLE_ADV_MAXIMUM_ADVERTISING_DATA_LENGTH    31//fixed value,not configurable
#endif


#ifndef BLE_PHY_ADV_MAX_TX_LEN
#define BLE_PHY_ADV_MAX_TX_LEN                     37//legacy mode max tx len is fix,37 octets.
#endif

#else

#ifndef BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS 
#define BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS       4
#endif

#ifndef BLE_ADV_MAXIMUM_ADVERTISING_DATA_LENGTH
#define BLE_ADV_MAXIMUM_ADVERTISING_DATA_LENGTH    512
#endif

#ifndef BLE_PHY_ADV_MAX_TX_LEN
#define BLE_PHY_ADV_MAX_TX_LEN                     128//min is adv extended header length,64 octets.
#endif

#endif
