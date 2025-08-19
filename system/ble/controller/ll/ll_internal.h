

#include"common/txCommon.h"
#include"system/scheduler/sch.h"
#include"../phy/phy.h"
#include"packet/packet.h"
#include"ll_feature.h"
#include"ll_config.h"

 #ifndef LL_INTERNAL_H_
 #define LL_INTERNAL_H_

#ifndef LL_LOG_TRACE
#define LL_LOG_TRACE 1
#endif

/**
 * BLE Link Layer States
 * standby state
 * advertising state                - advertiser
 * scanning state                   - scanner         
 * initiating state                 - initiator
 * connection state                 - central/peripheral
 * synchronization state            - synchronized receiver(isochronous)
 * isochronous broadcasting state   - isochronous broadcaster
 */



/**
 * link layer state machine maybe have multiple instances,and only one state to be active at a time.
 */
typedef enum
{
    BLE_LL_STATE_STANDBY,
    BLE_LL_STATE_ADVERTISING,
    BLE_LL_STATE_SCANNING,
    BLE_LL_STATE_INITIATING,
    BLE_LL_STATE_CONNECTION,
    BLE_LL_STATE_SYNCHRONIZATION,
    BLE_LL_STATE_BROADCASTING,
}ble_ll_state_e;

typedef enum
{
    BLE_LL_STATE_SUCCESS,
    BLE_LL_STATE_INVALID_PARAMETER,
    BLE_LL_STATE_TRANSITION_NOT_ALLOWED,
}ble_ll_state_status_e;

typedef enum
{
    BLE_LL_EVENT_START_ADVERTISING,
    BLE_LL_EVENT_STOP_ADVERTISING,

    BLE_LL_EVENT_START_SCANNING,
    BLE_LL_EVENT_STOP_SCANNING,

    BLE_LL_EVENT_START_INITIATING,
    BLE_LL_EVENT_STOP_INITIATING,

    BLE_LL_EVENT_START_SYNCHRONIZATION,
    BLE_LL_EVENT_STOP_SYNCHRONIZATION,

    BLE_LL_EVENT_START_BROADCASTING,
    BLE_LL_EVENT_STOP_BROADCASTING,

    BLE_LL_EVENT_START_CONNECTION,
    BLE_LL_EVENT_STOP_CONNECTION,

	BLE_LL_EVENT_MAX,
}ble_ll_event_e;

typedef int(*ble_ll_event_cb)(ble_ll_event_e);

typedef enum
{
    LL_ADV_PRIORITY         = SCH_TASK_PRIORITY_2,
    LL_ADC_AUX_PRIORITY     = SCH_TASK_PRIORITY_3,
    LL_ADV_CHAIN_PRIORITY   = SCH_TASK_PRIORITY_4,
    LL_SCAN_PRIORITY        = SCH_TASK_PRIORITY_2,
    LL_CONN_PRIORITY        = SCH_TASK_PRIORITY_2,
    LL_INIT_PRIORITY        = SCH_TASK_PRIORITY_2,
    LL_BROADCAST_PRIORITY   = SCH_TASK_PRIORITY_2,
    LL_SYNCHRONOUS_PRIORITY = SCH_TASK_PRIORITY_2,
}ll_task_priority_e;

/***********************ll standby sate**********************/
typedef struct _PACKED
{

}ll_internal_standby_ctrl_t;

/***********************ll advertising sate**********************/
#define LL_EXTENDED_ADV_INVALID_HANDLE   0xFF

/**
 * sch entry,phy entry,data entry maybe can expand to all ll state.
 */
typedef struct
{
    _u32 anchorPoint;
    _u32 duration;
    _u32 startMargin;
    _u32 stopMargin;
    _u32 interval;
    _u32 eventCnt;
}ll_adv_sch_entry_t;

typedef struct 
{
    _u8  mode;
    _u8  chn;
    _u16 rxMaxOctets;
    _u32 accessCode;
    _u32 crcInit;
    _u8* txAddress;
    _u8* rxAddress;
}ll_adv_phy_entry_t;

typedef struct 
{
   _u16 len;//to be compatible with adv and extended adv,use 2 byte to store data.
   _u8* addr;
}ll_adv_data_entry_t;
#if(LL_SUPPORT_LE_EXTENDED_ADVERTISING==1)
typedef struct 
{
    ll_adv_data_entry_t data; 
    ll_adv_sch_entry_t  sch;
    ll_adv_phy_entry_t  phy; 
}ll_adv_ea_chain_t;
#endif

typedef struct 
{
    _u8  channelCnt:2;//total adv channel count,max 3,min 1
    _u8  availableChnCnt:2;//max is channelCnt
    _u8  chnTable[3];
    ll_adv_sch_entry_t  sch;
    ll_adv_phy_entry_t  phy;
}ll_adv_set_t;

#if(LL_SUPPORT_LE_EXTENDED_ADVERTISING==1)
typedef struct 
{
    _u16 sid:4;
    _u16 did:12;
    _u8  power;
    _u8  maxEvents;

    _u8  secondaryMaxSkip;
    _u8  scanReqNotifyEnable:2;
    _u8  advDatafragPerf:1;//fragment preference
    _u8  scanRspDatafragPerf:1;//fragment preference
    _u8  auxFix;
    _u8  chainInx:3;
    _u8  chainCnt;
    _u8  dataLen;

    _u32 expireTime;//unit is us

    ll_adv_sch_entry_t sch;
    ll_adv_phy_entry_t phy;
    ll_adv_ea_chain_t* chain;
}ll_adv_ea_set_t;

#endif

#if(LL_SUPPORT_LE_PERIODIC_ADVERTISING==1)
typedef struct 
{

}ll_adv_pa_set_t;
#endif

#if(LL_SUPPORT_PERIODIC_ADVERTISING_WITH_RESPONSES_ADVERTISER==1)
typedef struct 
{

}ll_adv_pawr_set_t;
#endif


typedef struct
{
    _u8  enable:1;
    _u8  state:5;//state machine sate
    _u8  eventType:4;//adv event type,search"adv_event_type_e"
    _u8  processingEvent:4;//to prevent re-retrance
    #if(LL_SUPPORT_LE_EXTENDED_ADVERTISING==1)
    _u8  handle;
    _u8  auxiliary:1;
    _u8  chained:1;
    _u16 eventProperty;
    #else
    _u8  rsvd1;
    #endif
    _u8  ownAddressType:2;//'ll_own_address_type_e'
    _u8  peerAddressType:2;//'ll_peer_address_type_e'
    _u8  filterPolicy:2;//'ll_advertising_filter_policy_e'
    _u8  rsvd2:2;

    _u8  peerAddress[6];
    _u8  randomAddress[6];

    ll_adv_data_entry_t data;
    ll_adv_data_entry_t scanRsp;

    ll_adv_set_t*      la;//legacy advertising
    #if(LL_SUPPORT_LE_EXTENDED_ADVERTISING==1)
    ll_adv_ea_set_t*   ea;//extended advertising
    #endif
    #if(LL_SUPPORT_LE_PERIODIC_ADVERTISING==1)
    ll_adv_pa_set_t*   pa;//periodic advertising
    #endif
    #if(LL_SUPPORT_PERIODIC_ADVERTISING_WITH_RESPONSES_ADVERTISER==1)
    ll_adv_pawr_set_t* pawr//periodic advertising with response
    #endif
}ll_internal_adv_param_t;

typedef struct
{
    ll_internal_adv_param_t param[BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS];
}ll_internal_adv_ctrl_t;

#if(LL_SUPPORT_LE_EXTENDED_ADVERTISING==1)
ll_internal_adv_param_t* ll_extended_adv_get_entity(_u8 handle,_u8 allocate);
int                      ll_extended_adv_get_current_active_set_number(void);
int                      ll_extended_adv_get_current_set_number(void);
#endif
/***********************ll connection sate**********************/
typedef struct _PACKED
{
     
}ll_internal_connection_ctrl_t;


/***********************ll scanning sate**********************/
typedef struct _PACKED
{
    
}ll_internal_scan_ctrl_t;

/***********************ll initiating sate**********************/
typedef struct _PACKED
{
    
}ll_internal_initiating_ctrl_t;

/***********************ll synchronization sate**********************/
typedef struct _PACKED
{
    
}ll_internal_synchronous_ctrl_t;

/***********************ll broadcasting sate**********************/
typedef struct _PACKED
{
    
}ll_internal_broadcast_ctrl_t;

/***********************ll sate**********************/

typedef struct _PACKED
{
    _u8  id;
    _u8  state;
    _u16 rsvd;
    phy_obj_t  phy;
    sch_node_t sch;
    ll_internal_standby_ctrl_t*         standby;
    ll_internal_adv_ctrl_t*             adv;
    ll_internal_connection_ctrl_t*      conn;
    ll_internal_scan_ctrl_t*            scan;
    ll_internal_initiating_ctrl_t*      initiating;
    ll_internal_synchronous_ctrl_t*     synchronous;
    ll_internal_broadcast_ctrl_t*       broadcast;
}ll_sm_t;

typedef struct _PACKED
{
    _u8      txAddr[128];
    _u8      rxAddr[128];
    _u8      addr[6];
    _u8      rsvd[2];
    _u64     eventMask;
    _u64     leEventMask;
    ll_sm_t* sm;
}ll_t;

/************************get state machine****************************/
ll_sm_t* ll_get_idle_state_machine(void);
ll_sm_t* ll_get_state_machine_by_id(_u8 id);
ll_sm_t* ll_get_current_state_machine(void);
_u8*     ll_get_device_address(void);
_u8*     ll_get_shared_phy_tx_address(void);
_u8*     ll_get_shared_phy_rx_address(void);


ble_ll_state_status_e ble_ll_process_event(ll_sm_t* sm,ble_ll_event_e event);



#endif//LL_INTERNAL_H_
