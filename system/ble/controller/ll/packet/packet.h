 /**
 * BLE Packet BIT Ordering - LSB
 * multiple octets fields,with the exception of CRC and MIC,shall be transmit with least significant octet first.
 * each octets within multiple octets fields,with the exception of CRC,shall be transmit in LSB first order.
 */
#include"common/txCommon.h"
#include"../../phy/phy.h"
 #ifndef LL_PACKET_H_
 #define LL_PACKET_H_


 #define PACKET_DEFAULT_TIFS_TIME     150
/**************************Advertising physical channel PDU******************************/
typedef union 
{
    _u16 header;
    struct{
        _u8 pduType:4;
        _u8 rfu:1;
        _u8 chSel:1;
        _u8 txAdd:1;
        _u8 rxAdd:1;
        _u8 length;
    };
}ll_adv_packet_hdr_u;
typedef struct _PACKED
{
    ll_adv_packet_hdr_u hdr;
    _u8 data[2];
}ll_adv_packet_t;

enum
{
    LL_ADV_TYPE_ADV_IND               = 0x00,//0b0000,Primary   advertising,transmited or received on LE 1M PHY
    LL_ADV_TYPE_ADV_DIRECT_IND        = 0x01,//0b0001,Primary   advertising,transmited or received on LE 1M PHY
    LL_ADV_TYPE_ADV_NONCONN_IND       = 0x02,//0b0010,Primary   advertising,transmited or received on LE 1M PHY

    LL_ADV_TYPE_SCAN_REQ              = 0x03,//0b0011,Primary   advertising,transmited or received on LE 1M PHY
    LL_ADV_TYPE_AUX_SCAN_REQ          = 0x03,//0b0011,Secondary advertising,transmited or received on LE 1M PHY,LE 2M PHY,LE CODED PHY

    LL_ADV_TYPE_SCAN_RSP              = 0x04,//0b0100,Primary   advertising,transmited or received on LE 1M PHY

    LL_ADV_TYPE_CONNECT_IND           = 0x05,//0b0101,Primary   advertising,transmited or received on LE 1M PHY
    LL_ADV_TYPE_AUX_CONNECT_REQ       = 0x05,//0b0101,Secondary advertising,transmited or received on LE 1M PHY,LE 2M PHY,LE CODED PHY

    LL_ADV_TYPE_ADV_SCAN_IND          = 0x06,//0b0110,Primary   advertising,transmited or received on LE 1M PHY

    LL_ADV_TYPE_ADV_EXT_IND           = 0x07,//0b0111,Primary   advertising,transmited or received on LE 1M PHY,LE CODED PHY
    LL_ADV_TYPE_AUX_ADV_IND           = 0x07,//0b0111,Secondary advertising,transmited or received on LE 1M PHY,LE 2M PHY,LE CODED PHY
    LL_ADV_TYPE_AUX_SCAN_RSP          = 0x07,//0b0111,Secondary advertising,transmited or received on LE 1M PHY,LE 2M PHY,LE CODED PHY
    LL_ADV_TYPE_AUX_SYNC_IND          = 0x07,//0b0111,Periodic,             transmited or received on LE 1M PHY,LE 2M PHY,LE CODED PHY
    LL_ADV_TYPE_AUX_CHAIN_IND         = 0x07,//0b0111,Secondary advertising,transmited or received on LE 1M PHY,LE 2M PHY,LE CODED PHY
    LL_ADV_TYPE_AUX_SYNC_SUBEVENT_IND = 0x07,//0b0111,Periodic,             transmited or received on LE 1M PHY,LE 2M PHY,LE CODED PHY
    LL_ADV_TYPE_AUX_SYNC_SUBEVENT_RSP = 0x07,//0b0111,Periodic,             transmited or received on LE 1M PHY,LE 2M PHY,LE CODED PHY

    LL_ADV_TYPE_AUX_CONNECT_RSP       = 0x08,//0b1000,Secondary advertising,transmited or received on LE 1M PHY,LE 2M PHY,LE CODED PHY
    LL_ADV_TYPE_ADV_DECISION_IND      = 0x09,//0b1001,Primary   advertising,transmited or received on LE 1M PHY,LE CODED PHY
};

_u8* ll_get_adv_packet(_u8* packet,_u8 advType,_u8 chnSel,_u8 txAdd,_u8 rxAdd,_u8 length);
/**************************Data physical channel PDU******************************/
typedef union 
{
    _u16 header;
    struct{
        _u8 llId:2;
        _u8 nesn:1;
        _u8 sn:1;
        _u8 md:1;
        _u8 cp:1;
        _u8 rfu:2;
        _u8 length;
    };
}ll_data_packet_hdr_u;
typedef struct 
{
    ll_data_packet_hdr_u hdr;
    _u8 data[2];
}ll_data_packet_t;

enum
{
    LL_LLID_RESERVED               = 0x00,
    LL_LLID_CONTINUE_OR_EMPTY_PDU  = 0x01,
    LL_LLID_START_OR_COMPLETE_PDU  = 0x02,
    LL_LLID_CONTROL_PDU            = 0x03,
};
 _u8* ll_get_data_packet(_u8* packet,_u8 length,_u8 llid,_u8 nesn,_u8 sn,_u8 md);
/**************************Isochronous physical channel PDU******************************/
typedef union 
{
    _u16 header;
    struct {
        _u8 llid:2;
        _u8 nesn:1;
        _u8 sn:1;
        _u8 cie:1;
        _u8 rfu:1;
        _u8 npi:1;
        _u8 rfu1:1;
        _u8 length;
    }ci_hdr;
    struct {
        _u8 llid:2;
        _u8 cssn:3;
        _u8 cstf:1;
        _u8 rfu:2;
        _u8 length;
    }bi_hdr;
}ll_iso_packet_hdr_u;

typedef struct 
{
    ll_iso_packet_hdr_u hdr;
    _u8 data[2];
}ll_iso_packet_hdr_t;

enum
{
    LL_LLID_UNFRAMED_CIS_END_OR_COMPLETE_PDU   = 0x00,
    LL_LLID_UNFRAMED_CIS_START_OR_CONTINUE_PDU = 0x01,
    LL_LLID_FRAMED_CIS_PDU                     = 0x10,
};
_u8* ll_get_iso_cis_packet(_u8* packet,_u8 length,_u8 llid,_u8 nesn,_u8 sn,_u8 cie,_u8 npi);

enum
{
    LL_LLID_UNFRAMED_BIS_END_OR_COMPLETE_PDU   = 0x00,
    LL_LLID_UNFRAMED_BIS_START_OR_CONTINUE_PDU = 0x01,
    LL_LLID_FRAMED_BIS_PDU                     = 0x10,
};
_u8* ll_get_iso_bis_packet(_u8* packet,_u8 length,_u8 llid,_u8 cssn,_u8 cstf);

_u32 ll_get_air_packet_time(phy_mode_e phy,_u16 len,bool enc);

_u32 ll_get_packet_len(_u8 dir,_u16 dataLen,_u8 enc);//phy_dir_e

_u32 ll_get_packet_header_offset(_u8 dir);//phy_dir_e

#endif//LL_PACKET_H_
