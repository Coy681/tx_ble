#include"ll_feature.h"
#include"../config.h"

/************************* BLE ROLE CONFIGURE *****************************/
#if(BLE_SUPPORT_ADV) 

#endif 

#if(BLE_SUPPORT_SCAN) 

#endif 

#if(BLE_SUPPORT_CEN) 

#endif 

#if(BLE_SUPPORT_PER) 

#endif 

#if(BLE_SUPPORT_PDA) 

#endif 

#if(BLE_SUPPORT_PDA_SYNC) 

#endif 

#if(BLE_SUPPORT_PAWR) 

#endif 

#if(BLE_SUPPORT_PAWR_SYNC) 

#endif 

#if(BLE_SUPPORT_ISO_CEN) 

#endif 

#if(BLE_SUPPORT_ISO_PER) 

#endif 

#define BLE_SUPPORT_CONN                               (BLE_SUPPORT_CEN|BLE_SUPPORT_PER)


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

#if(LL_SUPPORT_LE_EXTENDED_ADVERTISING!=1)
#ifndef BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS
#define BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS       1//fixed value,not configurable
#endif

#ifndef BLE_ADV_MAXIMUM_ADVERTISING_DATA_LENGTH
#define BLE_ADV_MAXIMUM_ADVERTISING_DATA_LENGTH    31//fixed value,not configurable
#endif
#else
#ifndef BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS 
#define BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS       4
#endif

#ifndef BLE_ADV_MAXIMUM_ADVERTISING_DATA_LENGTH
#define BLE_ADV_MAXIMUM_ADVERTISING_DATA_LENGTH    512
#endif

#endif
