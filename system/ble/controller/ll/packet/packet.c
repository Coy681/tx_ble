/**
 * PDU Type
 * (1)Advertising Physical channel PDU,include
 *    > primary advertising physical channel
 *    > secondary advertising physical channel
 *    > periodic physical channel
 * (2)Data Physical Channel PDU
 * (3)Isochronous Physical Channel PDU
 */
#include"packet.h"

_RAM_CODE
_u32 ll_get_packet_len(_u8 dir,_u16 dataLen,bool enc)
{
	_u16 len = (enc?(dataLen+2+4):(dataLen+2));
	if(dir == PHY_DIR_TX)
	{
		return (len + hal_rf_get_hw_packet_tx_extra_len());
	}
	else
	{
		return (len + hal_rf_get_hw_packet_rx_extra_len());
	}
}

_RAM_CODE
_u32 ll_get_packet_header_offset_from_address(_u8 dir)
{
	if(dir == PHY_DIR_TX)
	{
		return hal_rf_get_hw_tx_header_offset();
	}
	else
	{
		return hal_rf_get_hw_rx_header_offset();
	}
}

_RAM_CODE
_u8* ll_adv_packet_data_prepare(_u8* addr,_u8 length,_u8 advType)
{
    ll_adv_packet_t* pkt = (ll_adv_packet_t*)(addr+ll_get_packet_header_offset_from_address(PHY_DIR_TX));
    pkt->hdr.pduType = advType;
    pkt->hdr.length  = length;
    return pkt->data;
}

_RAM_CODE
void ll_adv_packet_make(_u8* addr,_u8 chnSel,_u8 txAdd,_u8 rxAdd)
{
    ll_adv_packet_t* pkt = (ll_adv_packet_t*)(addr+ll_get_packet_header_offset_from_address(PHY_DIR_TX));
    pkt->hdr.chSel   = chnSel;
    pkt->hdr.txAdd   = txAdd;
    pkt->hdr.rxAdd   = rxAdd;
}

_RAM_CODE
_u8* ll_acl_packet_data_prepare(_u8* addr,_u8 length,_u8 llid)
{
	ll_acl_packet_t* pkt = (ll_acl_packet_t*)(addr+ll_get_packet_header_offset_from_address(PHY_DIR_TX));
    pkt->hdr.length       = length;
    pkt->hdr.llId         = llid;
    return pkt->data;
}

_RAM_CODE
void ll_acl_packet_make(_u8* addr,_u8 nesn,_u8 sn,_u8 md)
{
	ll_acl_packet_t* pkt = (ll_acl_packet_t*)(addr+ll_get_packet_header_offset_from_address(PHY_DIR_TX));
    pkt->hdr.nesn         = nesn;
    pkt->hdr.sn           = sn;
    pkt->hdr.md           = md;
}

_RAM_CODE
_u8* ll_iso_cis_packet_data_prepare(_u8* addr,_u8 length,_u8 llid)
{
    ll_iso_packet_hdr_t* pkt = (ll_iso_packet_hdr_t*)(addr+ll_get_packet_header_offset_from_address(PHY_DIR_TX));
    pkt->hdr.ci_hdr.length   = length;
    pkt->hdr.ci_hdr.llid     = llid;
    return pkt->data;
}

_RAM_CODE
void ll_iso_cis_packet_make(_u8* addr,_u8 nesn,_u8 sn,_u8 cie,_u8 npi)
{
    ll_iso_packet_hdr_t* pkt = (ll_iso_packet_hdr_t*)(addr+ll_get_packet_header_offset_from_address(PHY_DIR_TX));
    pkt->hdr.ci_hdr.nesn   = nesn;
    pkt->hdr.ci_hdr.sn     = sn;
    pkt->hdr.ci_hdr.cie    = cie;
    pkt->hdr.ci_hdr.npi    = npi;
}

_RAM_CODE
_u8* ll_iso_bis_packet_data_prepare(_u8* addr,_u8 length,_u8 llid)
{
    ll_iso_packet_hdr_t* pkt = (ll_iso_packet_hdr_t*)(addr+ll_get_packet_header_offset_from_address(PHY_DIR_TX));
    pkt->hdr.bi_hdr.length = length;
    pkt->hdr.bi_hdr.llid   = llid;
    return pkt->data;
}

_RAM_CODE
void ll_iso_bis_packet_make(_u8* addr,_u8 cssn,_u8 cstf)
{
    ll_iso_packet_hdr_t* pkt = (ll_iso_packet_hdr_t*)(addr+ll_get_packet_header_offset_from_address(PHY_DIR_TX));
    pkt->hdr.bi_hdr.cssn   = cssn;
    pkt->hdr.bi_hdr.cstf   = cstf;
}


static _u16 packetExcludePduTime[4] = {
    RF_PACKET_PREAMBLE_TIME_1M+RF_PACKET_ACCESS_CODE_TIME_1M+RF_PACKET_CRC_TIME_1M,
    RF_PACKET_PREAMBLE_TIME_2M+RF_PACKET_ACCESS_CODE_TIME_2M+RF_PACKET_CRC_TIME_2M,
    RF_PACKET_PREAMBLE_TIME_CODED_S2+RF_PACKET_ACCESS_CODE_TIME_CODED_S2+RF_PACKET_CRC_TIME_CODED_S2+RF_PACKET_CI_TIME_CODED_S2+\
    RF_PACKET_TERM1_TIME_CODED_S2+RF_PACKET_TERM2_TIME_CODED_S2,
    RF_PACKET_PREAMBLE_TIME_CODED_S8+RF_PACKET_ACCESS_CODE_TIME_CODED_S8+RF_PACKET_CRC_TIME_CODED_S8+RF_PACKET_CI_TIME_CODED_S8+\
    RF_PACKET_TERM1_TIME_CODED_S8+RF_PACKET_TERM2_TIME_CODED_S8,
};
static _u16 packetOctetTime[4] = {
    RF_PACKET_OCTET_TIME_1M,
    RF_PACKET_OCTET_TIME_2M,
    RF_PACKET_OCTET_TIME_CODED_S2,
    RF_PACKET_OCTET_TIME_CODED_S8,
};

_RAM_CODE
_u32 ll_get_air_packet_time(phy_mode_e phy,_u16 len,bool enc)
{
    _u16 pduLen = (enc?len+4:len)+2;
    return (packetExcludePduTime[phy]+pduLen*packetOctetTime[phy]);
}


