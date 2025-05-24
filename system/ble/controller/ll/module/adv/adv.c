#include"adv.h"


static void adv_irq_callback(_u8 type)
{
    if(type == PHY_IRQ_TX)
    {

    }
    else if(type == PHY_IRQ_RX)
    {

    }
    else if(type == PHY_IRQ_RX_TIMEOUT)
    {

    }
}

int ble_ll_enter_advertising_state(ble_ll_event_e event)
{
    LOG_TRACE(LL_LOG_TRACE,"enter advertising state",0,0)
    if(event == BLE_LL_EVENT_START_ADVERTISING)
    {

    }
    return 1;
}
