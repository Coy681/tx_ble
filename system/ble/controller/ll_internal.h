#include"state.h"
#include"common/txCommon.h"
/***********************Bluetooth LE Advertising **************************/

typedef struct _PACKED
{

}ll_internal_standby_ctrl_t;

typedef struct _PACKED
{
    _u16 interval;//number of 1.25ms

    _u8  advType:3;//search for enum 'll_advertising_type_e'
    _u8  channelMap:3;  
    _u8  filterPolicy:2;//'ll_advertising_filter_policy_e'

    _u8  ownAddressType:2;//'ll_own_address_type_e'
    _u8  peerAddressType:2;//'ll_peer_address_type_e'
    _u8  enable;

    _u8  peerAddress[6];

    _u8* advData;
    _u8  advDataLen;

    _u8* scanRspData;
    _u8  scanRspDataLen;
}ll_internal_adv_ctrl_t;

typedef struct _PACKED
{
    
}ll_internal_connection_ctrl_t;

typedef struct _PACKED
{
    
}ll_internal_scan_ctrl_t;

typedef struct _PACKED
{
    
}ll_internal_initiating_ctrl_t;

typedef struct _PACKED
{
    
}ll_internal_synchronous_ctrl_t;

typedef struct _PACKED
{
    
}ll_internal_broadcast_ctrl_t;

typedef struct _PACKED
{
    _u8  state;
    _u8  id;
    _u16 rsvd;
    ll_internal_standby_ctrl_t*         standby;
    ll_internal_adv_ctrl_t*             adv;
    ll_internal_connection_ctrl_t*      conn;
    ll_internal_scan_ctrl_t*            scan;
    ll_internal_initiating_ctrl_t*      initiating;
    ll_internal_synchronous_ctrl_t*     synchronous;
    ll_internal_broadcast_ctrl_t*       broadcast;
}ll_ctrl_t;

