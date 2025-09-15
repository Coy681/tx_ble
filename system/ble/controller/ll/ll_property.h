
#ifndef BLE_CONTROLLER_LL_LL_PROPERTY_H_
#define BLE_CONTROLLER_LL_LL_PROPERTY_H_
#include"ll_feature.h"
/*********************** BLE PROPERTY CONFIGURE ***************************/
#ifndef BLE_FILTER_ACCEPT_LIST_SIZE
#define BLE_FILTER_ACCEPT_LIST_SIZE                    4
#endif

#ifndef BLE_ADV_PRI_PHY_MAX_TX_LEN
#define BLE_ADV_PRI_PHY_MAX_TX_LEN                     37//fix value,37 octets.
#endif

#ifndef BLE_ADV_SEC_PHY_MAX_TX_LEN
#define BLE_ADV_SEC_PHY_MAX_TX_LEN                     128//Configurable,min is adv extended header length,which is 64 octets.
#endif

#ifndef BLE_ADV_PRI_PHY_MAX_RX_LEN
#define BLE_ADV_PRI_PHY_MAX_RX_LEN                     37//fix value,37 octets.
#endif

#ifndef BLE_ADV_SEC_PHY_MAX_RX_LEN
#define BLE_ADV_SEC_PHY_MAX_RX_LEN                     200//Configurable,min is adv extended header length,which is 64 octets.
#endif

#ifndef BLE_PHY_DEFAULT_TX_POWER
#define BLE_PHY_DEFAULT_TX_POWER                       0
#endif

#if(LL_SUPPORT_LE_EXTENDED_ADVERTISING)

#ifndef BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS
#define BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS           4
#endif

#ifndef BLE_ADV_MAXIMUM_ADVERTISING_DATA_LENGTH
#define BLE_ADV_MAXIMUM_ADVERTISING_DATA_LENGTH        512
#endif

#else/*(!LL_SUPPORT_LE_EXTENDED_ADVERTISING)*/
#ifndef BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS
#define BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS           1//fixed value,not configurable
#endif

#ifndef BLE_ADV_MAXIMUM_ADVERTISING_DATA_LENGTH
#define BLE_ADV_MAXIMUM_ADVERTISING_DATA_LENGTH        31//fixed value,not configurable
#endif
#endif/*(LL_SUPPORT_LE_EXTENDED_ADVERTISING)*/



#if defined(BLE_SUPPORT_CONN)

#ifndef BLE_CONN_TX_MAX_OCTETS
#define BLE_CONN_TX_MAX_OCTETS                         27
#endif

#ifndef BLE_CONN_RX_MAX_OCTETS
#define BLE_CONN_RX_MAX_OCTETS                         27
#endif

#endif



#endif /* BLE_CONTROLLER_LL_LL_PROPERTY_H_ */
