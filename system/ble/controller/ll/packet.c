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

_u8* ll_get_adv_packet(_u8* packet,_u8 length,_u8 advType,_u8 chnSel,_u8 txAdd,_u8 rxAdd)
{
    ll_adv_packet_t* pkt = (ll_adv_packet_t*)packet;
    pkt->hdr.pduType = advType;
    pkt->hdr.chSel   = chnSel;
    pkt->hdr.txAdd   = txAdd;
    pkt->hdr.rxAdd   = rxAdd;
    pkt->hdr.length  = length;
    return pkt->data;
}

_u8* ll_get_data_packet(_u8* packet,_u8 length,_u8 llid,_u8 nesn,_u8 sn,_u8 md)
{
    ll_data_packet_t* pkt = (ll_data_packet_t*)packet;
    pkt->hdr.length = length;
    pkt->hdr.llId   = llid;
    pkt->hdr.nesn   = nesn;
    pkt->hdr.sn     = sn;
    pkt->hdr.md     = md;
    pkt->hdr.rfu    = 0;
    pkt->hdr.cp     = 0;//not support now
    return pkt->data;
}

_u8* ll_get_iso_cis_packet(_u8* packet,_u8 length,_u8 llid,_u8 nesn,_u8 sn,_u8 cie,_u8 npi)
{
    ll_iso_packet_hdr_t* pkt = (ll_iso_packet_hdr_t*)packet;
    pkt->hdr.ci_hdr.length = length;
    pkt->hdr.ci_hdr.llid   = llid;
    pkt->hdr.ci_hdr.nesn   = nesn;
    pkt->hdr.ci_hdr.sn     = sn;
    pkt->hdr.ci_hdr.cie    = cie;
    pkt->hdr.ci_hdr.npi    = npi;
    return pkt->data;
}

_u8* ll_get_iso_bis_packet(_u8* packet,_u8 length,_u8 llid,_u8 cssn,_u8 cstf)
{
    ll_iso_packet_hdr_t* pkt = (ll_iso_packet_hdr_t*)packet;
    pkt->hdr.bi_hdr.length = length;
    pkt->hdr.bi_hdr.llid   = llid;
    pkt->hdr.bi_hdr.cssn   = cssn;
    pkt->hdr.bi_hdr.cstf   = cstf;
    return pkt->data;
}