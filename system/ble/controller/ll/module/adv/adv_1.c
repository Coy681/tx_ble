
#include"adv.h"
#include"../scan/scan.h"
#include"../init/init.h"


//adv event sequence
{
    ADV_EVENT_SEQUENCE_CONNECTABLE_SCANNABLE_UNDIRECTED,
    ADV_EVENT_SEQUENCE_CONNECTABLE_UNDIRECTED,
    ADV_EVENT_SEQUENCE_CONNECTABLE_DIRECTED,
    ADV_EVENT_SEQUENCE_SCANNABLE_UNDIRECTED,
    ADV_EVENT_SEQUENCE_SCANNABLE_DIRECTED,
    ADV_EVENT_SEQUENCE_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED,
    ADV_EVENT_SEQUENCE_NON_CONNECTABLE_NON_SCANNABLE_DIRECTED,
}

//adv schcdule sequence
{
    ADV_SCHEDULE_SEQUENCE_LEGACY,
    ADV_SCHEDULE_SEQUENCE_EXTENDED_WITHOUT_AUXILIARY,
    ADV_SCHEDULE_SEQUENCE_EXTENDED_WITH_AUXILIARY,
    ADV_SCHEDULE_SEQUENCE_EXTENDED_SYNC_WITHOUT_RESPONSE,
    ADV_SCHEDULE_SEQUENCE_EXTENDED_SYNC_WITH_RESPONSE,
}



_RAM_CODE
static void adv_phy_irq_tx(void)
{

}

_RAM_CODE
static void adv_phy_irq_rx(void)
{

}

_RAM_CODE
static void adv_phy_irq_rx_timeout(void)
{

}

_RAM_CODE
static void adv_sch_start(void)
{

}

_RAM_CODE
static void adv_sch_stop(void)
{

}

_RAM_CODE
static void adv_sch_calceled(void)
{
    
}

_RAM_CODE
static void adv_sch_passed(void)
{
    
}

static void(*adv_phy_irq_cb[3])(void)=
{
    adv_phy_irq_tx,
    adv_phy_irq_rx,
    adv_phy_irq_rx_timeout,   
};

static void(*adv_sch_cb[4])(void)={
    adv_sch_start,
    adv_sch_stop,
    adv_sch_calceled,
    adv_sch_passed,
};

_RAM_CODE
static void adv_phy_irq_callback(_u8 type)
{   
    adv_phy_irq_cb[type]();
}
_RAM_CODE
static void adv_sch_callback(_u8 type)
{
    adv_sch_cb[type]();
}

int ble_ll_enter_advertising_state(ble_ll_event_e event)
{
    if(event == BLE_LL_EVENT_START_ADVERTISING)
    {
        ll_ctrl_t* ll = ll_get_current_state_machine();
        if(ll->adv == NULL)
        {   
            return 0;
        }
        //phy init
        ll->phy.mode       = PHY_MODE_1M;
        ll->phy.hw_irq_cb  = adv_phy_irq_callback;
        ll->ownAddr[0] = ll->ownAddr[1] = ll->ownAddr[2] = ll->ownAddr[3]= ll->ownAddr[4] =ll->ownAddr[5]= 0x12;
        ll->adv->instant = 0;
        ll->adv->channelCnt = 0;
        _u8 chnCount = 0;
        for(_u8 i=0;i<3;i++)
        {
            if(ll->adv->channelMap&BIT(i))
            {
                advPriChannel[ll->adv->channelCnt++] = advPriChannel[i];
            }
        }
        phy_obj_cast(&ll->phy);
        phy_obj_init(&ll->phy);
        //sch init
        ll->sch.llId = ll->id;
        ll->sch.type = SCH_PERIODIC_TASK;
        ll->sch.priority = LL_ADV_PRIORITY;
        ll->sch.timestamp = system_time() + 50;//maybe need planner
        ll->sch.period    = ll->adv->interval*BLE_ADV_INTERVAL_UNIT;
        ll->sch.duration = ll->phy.hw_get_prepare_time()+3*ll_get_air_packet_time(ll->phy.mode,BLE_ADV_MAX_LENGTH,0)+2*PACKET_DEFAULT_TIFS_TIME;
        ll->sch.startLatency = 50;
        ll->sch.stopLatency  = 50;
        ll->sch.cb = adv_sch_callback;
        sch_message_t* message = (sch_message_t*)tx_message_allocate(8);
        message->eventType = SCHE_MESSAGE_TASK_ADD;
        message->message[0] = ((_u32)&ll->sch);
        message->message[1] = ((_u32)&ll->sch)>>8;
        message->message[2] = ((_u32)&ll->sch)>>16;
        message->message[3] = ((_u32)&ll->sch)>>24;
        tx_message_send(TX_TASK_ID_SCH,(_u8*)message);
        LOG_TRACE(LL_LOG_TRACE,"enter advertising state",0,0);
        return 1;
    }
}