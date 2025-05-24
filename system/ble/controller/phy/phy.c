#include"phy.h"

phy_obj_t* phyCtrl;

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

_u32  phy_get_rx_timestamp(void)
{
    return hal_rf_get_rx_timestamp(phyCtrl->phy);
}
_u32  phy_rx_packet_valid(void)
{
    if(phyCtrl->rxAddress)
    {
        return hal_rf_is_packet_valid(phyCtrl->rxAddress);
    }
    return 0;
}

static void phy_irq_callback(_u8 type)
{
    if(phyCtrl)
    {
        phyCtrl->irq_cb(type);
    }
}
static void phy_init(void)
{
    hal_rf_init(phy_irq_callback);
}

void phy_obj_init(phy_obj_t* phy)
{
    phy->start = phy_start;
    phy->stop  = phy_stop;
    phy->get_rx_timestamp = phy_get_rx_timestamp;
    phy->rx_packet_valid  = phy_rx_packet_valid;
}

