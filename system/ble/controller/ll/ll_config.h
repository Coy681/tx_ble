#include"../config.h"

#ifndef BLE_CONTROLLER_LL_LL_CONFIG_H_
#define BLE_CONTROLLER_LL_LL_CONFIG_H_
/************************* BLE ROLE CONFIGURE *****************************/
#if defined (BLE_SUPPORT_ADV) 

#endif 

#if defined(BLE_SUPPORT_SCAN) 

#endif 

#if defined(BLE_SUPPORT_CEN) 

#endif 

#if defined(BLE_SUPPORT_PER) 

#ifndef BLE_SUPPORT_ADV
#define BLE_SUPPORT_ADV
#endif

#endif 

#if defined(BLE_SUPPORT_PDA) 

#ifndef BLE_SUPPORT_ADV
#define BLE_SUPPORT_ADV
#endif

#ifndef BLE_SUPPORT_EXTENDED_ADVERTISING
#define BLE_SUPPORT_EXTENDED_ADVERTISING
#endif

#ifndef BLE_SUPPORT_PERIODIC_ADVERTISING
#define BLE_SUPPORT_PERIODIC_ADVERTISING
#endif

#endif 

#if defined(BLE_SUPPORT_PDA_SYNC) 
#ifndef BLE_SUPPORT_SCAN
#define BLE_SUPPORT_SCAN
#endif

#ifndef BLE_SUPPORT_EXTENDED_ADVERTISING
#define BLE_SUPPORT_EXTENDED_ADVERTISING
#endif

#ifndef BLE_SUPPORT_PERIODIC_ADVERTISING
#define BLE_SUPPORT_PERIODIC_ADVERTISING
#endif
#endif 

#if defined(BLE_SUPPORT_PAWR) 


#endif 

#if defined(BLE_SUPPORT_PAWR_SYNC) 

#endif 

#if defined(BLE_SUPPORT_ISO_CEN) 

#endif 

#if defined(BLE_SUPPORT_ISO_PER) 

#endif 

#if defined(BLE_SUPPORT_ISO_BRD) 

#endif 

#if defined(BLE_SUPPORT_ISO_SYNC) 

#endif 

#if defined(BLE_SUPPORT_CS) 

#endif 

#if defined(BLE_SUPPORT_CEN)||defined(BLE_SUPPORT_PER)
#define BLE_SUPPORT_CONN             y
#endif

#if defined(BLE_SUPPORT_ISO_CEN)||defined(BLE_SUPPORT_ISO_PER)||defined(BLE_SUPPORT_ISO_BRD)||defined(BLE_SUPPORT_ISO_SYNC)
#define BLE_SUPPORT_ISO
#endif


#endif /* BLE_CONTROLLER_LL_LL_CONFIG_H_ */
