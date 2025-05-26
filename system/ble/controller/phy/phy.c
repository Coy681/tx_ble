#include"phy.h"

phy_obj_t* phyCtrl;

static _u32 phyOctetTime[4] = {
    HARDWARE_OCTET_TIME_1M,
    HARDWARE_OCTET_TIME_2M,
    HARDWARE_OCTET_TIME_CODED_S2,
    HARDWARE_OCTET_TIME_CODED_S8,
};
static _u32 phyPacketTimeExcludePdu[4] = {
    40, //HEADER(16)+CRC(24)
    20, //HEADER(8) +CRC(12)
    126,//CI(16)+TERM1(24)+HEADER(32)+CRC(48)+TERM2(6)
    384,//CI(16)+TERM1(24)+HEADER(128)+CRC(192)+TERM2(24)
};

void phy_obj_cast(phy_obj_t* phy)
{
    ASSERT(phy!=NULL);
    if(phy)
    {
        phyCtrl = phy;
    }
}
void(*phy_mode[4])(void)=
{
    hal_rf_set_1M_phy,
    hal_rf_set_2M_phy,
    hal_rf_set_coded_phy_s2,
    hal_rf_set_coded_phy_s8,
};
static void phy_start(void)
{
    static _u8 lastPhy = 0xff;
    hal_rf_set_access_code(phyCtrl->accessCode);
    hal_rf_set_crc_value(phyCtrl->crcInit);
    hal_rf_set_channel_index(phyCtrl->chnIdx);
    if(lastPhy!= phyCtrl->phy)
    {
        phy_mode[phyCtrl->phy];
        lastPhy = phyCtrl->phy;
    }
    if(phyCtrl->dir == PHY_DIR_TX)
    {
        hal_rf_tx(phyCtrl->txAddress,phyCtrl->timestamp);
    }
    else if(phyCtrl->dir == PHY_DIR_RX)
    {
        hal_rf_set_rx_timeout(phyCtrl->rxTimeout);
        hal_rf_rx(phyCtrl->rxAddress,phyCtrl->rxMaxOctets,phyCtrl->timestamp);
    }
}
static void phy_stop(void)
{
    hal_rf_stop();
}

_u32  phy_hw_get_rx_header_timestamp(void)
{
    return hal_rf_get_rx_header_timestamp(phyCtrl->phy);
}

_u32  phy_hw_get_packet_time_from_trigger_to_end(_u8 len,_u8 enc)
{
    _u8  pduLen = (enc?len+4:4);
    _u32 packetTime = phyPacketTimeExcludePdu[phyCtrl->phy] + pduLen*phyOctetTime[phyCtrl->phy];
    if(phyCtrl->dir == PHY_DIR_TX)
    {
        return (packetTime+=hal_rf_get_tx_trigger_to_header_time(phyCtrl->phy));
    }
    else
    {
        return (packetTime+=hal_rf_get_rx_trigger_to_header_time(phyCtrl->phy));
    }
}

_u32  phy_hw_if_rx_packet_valid(void)
{
    if(phyCtrl->rxAddress)
    {
        return hal_rf_is_packet_valid(phyCtrl->rxAddress);
    }
    return 0;
}

static void phy_hw_irq_callback(_u8 type)
{
    if(phyCtrl)
    {
        phyCtrl->hw_irq_cb(type);
    }
}
static void phy_init(void)
{
    hal_rf_init(phy_hw_irq_callback);
}

void phy_obj_init(phy_obj_t* phy)
{
    phy->start = phy_start;
    phy->stop  = phy_stop;
    phy->hw_get_rx_header_timestamp         = phy_hw_get_rx_header_timestamp;
    phy->hw_packet_from_trigger_to_end_time = phy_hw_get_packet_time_from_trigger_to_end;
    phy->hw_if_rx_packet_valid              = phy_hw_if_rx_packet_valid;
    phy->hw_irq_cb                          = phy_hw_irq_callback;
}

