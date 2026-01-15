

#include"common/txCommon.h"
#include"system/scheduler/sch.h"
#include"../phy/phy.h"
#include"packet/packet.h"
#include"ll_feature.h"
#include"ll_property.h"
#include"channel/channel.h"
#include"accuracy/accuracy.h"
#ifndef LL_INTERNAL_H_
#define LL_INTERNAL_H_


#define LL_LOG_TRACE         1

#define LL_SM_INVALID_HANDLE    0xFFFF


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
    LL_ADV_PRIORITY         = SCH_TASK_PRIORITY_2,
    LL_ADC_AUX_PRIORITY     = SCH_TASK_PRIORITY_3,
    LL_ADV_CHAIN_PRIORITY   = SCH_TASK_PRIORITY_4,
    LL_SCAN_PRIORITY        = SCH_TASK_PRIORITY_2,
    LL_CONN_PRIORITY        = SCH_TASK_PRIORITY_2,
    LL_INIT_PRIORITY        = SCH_TASK_PRIORITY_2,
    LL_BROADCAST_PRIORITY   = SCH_TASK_PRIORITY_2,
    LL_SYNCHRONOUS_PRIORITY = SCH_TASK_PRIORITY_2,
}ll_task_priority_e;

typedef void(*reset_f)(void);

/***********************link layer event define**********************/

typedef enum
{
    LL_SCH_EVENT_BASE              = BIT(4),
    LL_SCH_EVENT_START             = LL_SCH_EVENT_BASE+SCH_TASK_START,
    LL_SCH_EVENT_STOP              = LL_SCH_EVENT_BASE+SCH_TASK_STOP,
    LL_SCH_EVENT_CANCELED          = LL_SCH_EVENT_BASE+SCH_TASK_CANCELED,
    LL_SCH_EVENT_PASSED            = LL_SCH_EVENT_BASE+SCH_TASK_PASSED,

    LL_PHY_EVENT_BASE              = BIT(5),
    LL_PHY_EVENT_SEND_FINISHED     = LL_PHY_EVENT_BASE+PHY_IRQ_TX_FINISHED,
    LL_PHY_EVENT_RECEIVE_FINISHED  = LL_PHY_EVENT_BASE+PHY_IRQ_RX_FINISHED,
    LL_PHY_EVENT_RECEIVE_TIMEOUT   = LL_PHY_EVENT_BASE+PHY_IRQ_RX_TIMEOUT,
}ll_sm_event_e;

/***********************ll standby sate**********************/
typedef struct _PACKED
{

}ll_internal_standby_ctrl_t;

/***********************ll advertising sate**********************/
#define LL_EXTENDED_ADV_INVALID_HANDLE   0xFF

typedef struct
{
    _u32 anchorPoint;
    _u32 duration;
    _u32 startMargin;
    _u32 stopMargin;
    _u32 interval;
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
   _u16 rsvd;
   _u8* addr;
}ll_adv_data_entry_t;

typedef struct
{
    ll_adv_data_entry_t data; 
    ll_adv_sch_entry_t  sch;
    ll_adv_phy_entry_t  phy; 
}ll_adv_entry_t;

typedef struct 
{
    _u8  channelCnt:2;//total adv channel count,max 3,min 1
    _u8  availableChnCnt:2;//max is channelCnt
    _u8  chnTable[3];
    _u32 eventCnt;
    ll_adv_sch_entry_t  sch;
    ll_adv_phy_entry_t  phy;
}ll_adv_type_la_t;

#if(LL_SUPPORT_LE_EXTENDED_ADVERTISING)
typedef struct
{
    _u8  cnt;
    _u8  current;
    _u16 rsvd;
    ll_adv_entry_t* entry;
}ll_adv_chain_entry_t;

typedef struct 
{
    _u16 did:12;
    _u16 rsvd:4;
    _u8  power;
    _u8  maxEvents;

    _u8  secondaryMaxSkip;
    _u8  scanReqNotifyEnable:2;
    _u8  advDatafragPerf:1;//fragment preference
    _u8  scanRspDatafragPerf:1;//fragment preference
    _u8  phyMode:3;
    _u8  rsvd1:1;
    _u16 rsvd2;

    _u32 expireTime;//unit is us
    _u32 anchor;
    _u32 eventCnt;
    ll_adv_entry_t        aux;
    ll_adv_chain_entry_t  chain;
}ll_adv_type_ea_t;
#endif

#if(LL_SUPPORT_LE_PERIODIC_ADVERTISING)
typedef struct 
{
    _u8  enable:1;
    _u8  includeTxPower:1;
    _u8  includeAdi:1;
    _u8  active:1;
    _u8  rsvd:4;
    _u16 did:12;
    _u16 rsvd2:4;
    _u8  rsvd1;

    _u32 eventCnt;
    _u32 anchor;
    ll_adv_data_entry_t   data;

    ll_adv_entry_t        sync;
    ll_adv_chain_entry_t  chain;
}ll_adv_type_pa_t;
#endif

#if(LL_SUPPORT_PERIODIC_ADVERTISING_WITH_RESPONSES_ADVERTISER)
typedef struct 
{

}ll_adv_type_pawr_t;
#endif


typedef struct
{
    _u8  enable:1;
    _u8  state:5;//state machine sate
    _u8  rsvd0:2;
    _u8  eventType:4;//adv event type,search"adv_event_type_e"
    _u8  processingEvent:4;//to prevent adv sub state machine re-retrance

    #if(LL_SUPPORT_LE_EXTENDED_ADVERTISING)
    _u8  handle;
    _u8  schMap;

    ll_adv_chain_entry_t* pChain;

    _u8  sid:4;
    _u8  rsvd1;
    _u16 eventProperty;
	#else
    _u8  schMap:4;//adv set maybe have many sch type,such as adv,auy,pda,chain
    _u8  inSch;//adv set in schedule.
    _u8  rsvd1:4;
    #endif
    _u8  ownAddressType:2;//'ll_own_address_type_e'
    _u8  peerAddressType:2;//'ll_peer_address_type_e'
    _u8  filterPolicy:2;//'ll_advertising_filter_policy_e'
    _u8  rsvd3:2;

    _u8  peerAddress[6];
    _u8  randomAddress[6];

    ll_adv_data_entry_t data;
    ll_adv_data_entry_t scanRsp;

    ll_adv_type_la_t    la;//advertising
    #if(LL_SUPPORT_LE_EXTENDED_ADVERTISING)
    ll_adv_type_ea_t*   ea;//extended advertising
    #endif
    #if(LL_SUPPORT_LE_PERIODIC_ADVERTISING)
    ll_adv_type_pa_t*   pa;//periodic advertising
    #endif
    #if(LL_SUPPORT_PERIODIC_ADVERTISING_WITH_RESPONSES_ADVERTISER)
    ll_adv_type_pawr_t* pawr;//periodic advertising with response
    #endif
}ll_internal_adv_set_t;

typedef struct
{
	_u8     active:1;
	_u8     rsvd:7;
	_u8     rsvd1;
	_u16    rsvd2;

	reset_f reset;
	ll_internal_adv_set_t set[BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS];
}ll_internal_adv_ctrl_t;

#if(LL_SUPPORT_LE_EXTENDED_ADVERTISING)
ll_internal_adv_set_t*   ll_extended_adv_get_adv_set(_u8 handle,_u8 allocate);
int                      ll_extended_adv_get_current_active_set_number(void);
int                      ll_extended_adv_get_current_set_number(void);
#endif
/***********************ll connection sate**********************/
typedef struct _PACKED
{
	_u8  sca:3;
	_u8  sn:1;
	_u8  nesn:1;
	_u8  md:1;
	_u8  rsvd:2;
	_u8  rsvd1;
	_u16 rsvd2;

	_u16 rsvd3;
	_u8  address[6];
}ll_conn_peer_t;
typedef struct _PACKED
{
	_u16 maxTxOctets;
	_u16 maxRxOctets;
	_u16 maxTxTime;
	_u16 maxRxTime;

	_u16 ownMaxTxOctets;
	_u16 ownMaxRxOctets;
	_u16 ownMaxTxTime;
	_u16 ownMaxRxTime;

	_u8* pdu;

	tx_rb_t in;
	tx_rb_t out;
}ll_conn_data_t;

typedef struct _PACKED
{
    _u8     pending:1;
    _u8     rsvd:7;

    tx_bl_t in;
    tx_bl_t out;
}ll_conn_ctrl_t;

typedef struct _PACKED
{

}ll_conn_cen_t;

typedef struct _PACKED
{
	_u32 lastSyncTime;
}ll_conn_peri_t;

typedef struct _PACKED
{
	_u16 factor;//0x01-0x1f4(1-500)
	_u16 latency;//0x00-0x1f3(0-499)
	_u16 conNum;//0x00-0x1f3(0-499)
	_u16 timeout;//0x0a-0xc80,time = N*10ms,from 100ms to 32s
}ll_conn_subrate_t;

typedef struct _PACKED
{
	_u8  role:1;
	_u8  state:3;
    _u8  sn:1;
    _u8  nesn:1;
    _u8  md:1;
    _u8  enc:1;
    _u8  lastPduType:3;
    _u8  txPhyMode:2;//phy_mode_e
    _u8  rxPhyMode:2;//phy_mode_e
	_u8  rsvd1:1;
	_u16 eventCounter;

	_u16 handle;
	_u16 rsvd2;

	_u16 latency;
	_u16 timeout;

	_u32 anchor;
	_u32 duration;
	_u32 interval;

	_u16 tifs_cp;
	_u16 tifs_pc;

	_u32 connPhyTs;


	#if(LL_SUPPORT_CONNECTION_SUBRATING)
	ll_conn_subrate_t     sr;//subrate
	#endif
	ll_conn_peer_t        peer;
	ll_conn_data_t        data;
    ll_conn_ctrl_t        ctrl;
	ll_csa_ctrl_t         csa;
	_u8*                  info;
	reset_f reset;
}ll_internal_connection_ctrl_t;




/***********************ll scanning sate**********************/
typedef struct _PACKED
{
	reset_f reset;
}ll_internal_scan_ctrl_t;

/***********************ll initiating sate**********************/
typedef struct _PACKED
{
	reset_f reset;
}ll_internal_initiating_ctrl_t;

/***********************ll synchronization sate**********************/
typedef struct _PACKED
{
	_u16 handle;
	_u16 rsvd;
	reset_f reset;
}ll_internal_synchronous_ctrl_t;

/***********************ll broadcasting sate**********************/
typedef struct _PACKED
{
	_u16 handle;
	_u16 rsvd;
	reset_f reset;
}ll_internal_broadcast_ctrl_t;

/***********************ll sate**********************/

//typedef struct _PACKED
//{
//    _u8  id;
//    _u8  state;
//    _u16 rsvd;
//    phy_obj_t  phy;
//    sch_node_t sch;
//    ll_internal_standby_ctrl_t*         standby;
//    ll_internal_adv_ctrl_t*             adv;
//    ll_internal_connection_ctrl_t*      conn;
//    ll_internal_scan_ctrl_t*            scan;
//    ll_internal_initiating_ctrl_t*      initiating;
//    ll_internal_synchronous_ctrl_t*     synchronous;
//    ll_internal_broadcast_ctrl_t*       broadcast;
//}ll_sm_t;

typedef struct _PACKED
{
	_u8  id;
	_u8  state;
	_u16 rsvd1;
	//the reason why put phy and sch object here:coupling common object and specific module
    phy_obj_t  phy;
    sch_node_t sch;
    _u8* entity;//maybe standby/adv/conn/scan/initiating/synchronous/broadcast
}ll_sm_t;

typedef struct _PACKED
{
    _u8      txAddr[256];
    _u8      rxAddr[256];
    _u8      addr[6];
    _u8      addrType;
    _u8      smNum;
    _u64     eventMask;
    _u64     eventMask2;
    _u64     leEventMask;
    struct 
    {
        _u8 occupy;
        _u8 addrType;
        _u8 addr[6];
    }filterAcceptList[BLE_FILTER_ACCEPT_LIST_SIZE];
    #if(LL_SUPPORT_CONNECTION_SUBRATING)
	_u16 srMin;//0x01-0x01f4(1-500)
	_u16 srMax;//0x01-0x01f4(1-500)
	_u16 srLatency;//0x00-0x1f3(0-499)
	_u16 srConNum;//0x00-0x1f3(0-499)
	_u16 srTimeout;//0x0a-0xc80,time = N*10ms,from 100ms to 32s
    #endif
    ll_sm_t* sm;
    _u32 localPPM;
}ll_t;

/************************get state machine****************************/
_u8* ll_get_sm_entity_by_id(_u8 id);
_u8* ll_get_sm_entity_by_state(ble_ll_state_e state,_u16 handle,_u8 allocate);

_u8* ll_get_device_address(void);
_u32 ll_get_local_ppm(void);
_u8* ll_get_shared_phy_tx_address(void);
_u8* ll_get_shared_phy_rx_address(void);

#endif//LL_INTERNAL_H_
