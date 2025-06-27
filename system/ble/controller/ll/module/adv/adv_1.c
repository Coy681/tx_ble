
#include"adv.h"
#include"../scan/scan.h"
#include"../init/init.h"


#define ADV_SCHE_PRIMARY_37         BIT(0)
#define ADV_SCHE_PRIMARY_38         BIT(1)
#define ADV_SCHE_PRIMARY_39         BIT(2)

typedef enum
{
    ADV_EVENT_CONNECTABLE_SCANNABLE_UNDIRECTED,
    ADV_EVENT_CONNECTABLE_DIRECTED,
    ADV_EVENT_SCANNABLE_UNDIRECTED,
    ADV_EVENT_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED,
    #ifdef(LL_SUPPORT_LE_EXTENDED_ADVERTISING)
    ADV_EVENT_EXTENDED_CONNECTABLE_DIRECTED,
    ADV_EVENT_EXTENDED_CONNECTABLE_UNDIRECTED,
    ADV_EVENT_EXTENDED_SCANNABLE_DIRECTED,
    ADV_EVENT_EXTENDED_SCANNABLE_UNDIRECTED,
    ADV_EVENT_EXTENDED_NON_CONNECTABLE_NON_SCANNABLE_DIRECTED
    ADV_EVENT_EXTENDED_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED,
    ADV_EVENT_EXTENDED_PERIODIC_WITHOUT_RESPONSE,
    ADV_EVENT_EXTENDED_PERIODIC_WITH_RESPONSE,
    #endif
}adv_event_type_e;

typedef enum
{
    ADV_EVENT,
    ADV_EXTENDED_EVENT,
    ADV_PERIODIC_WITHOUT_RSP_EVENT,
    ADV_PERIODIC_WITH_RSP_EVENT,
}adv_event_class_e;

typedef enum
{
    ADV_PDU_IND                      = BIT(0),
    ADV_PDU_DIRECT_IND               = BIT(1),
    ADV_PDU_SCAN_IND                 = BIT(2),
    ADV_PDU_NONCONN_IND              = BIT(3),
    #ifdef(LL_SUPPORT_LE_EXTENDED_ADVERTISING)
    ADV_PDU_EXT_IND                  = BIT(4),
    ADV_PDU_DECISION_IND             = BIT(5),
    ADV_PDU_AUX_IND                  = BIT(6),
    ADV_PDU_AUX_SYNC_IND             = BIT(7),
    ADV_PDU_AUX_CHAIN_IND            = BIT(8),
    ADV_PDU_AUX_SYNC_SUBEVENT_IND    = BIT(9),
    ADV_PDU_AUX_SYNC_SUBEVENT_RSP    = BIT(10),
    #endif
}adv_pdu_type_e;

typedef enum
{
    ADV_PHY_TX  = BIT(0),
    ADV_PHY_RX  = BIT(1),
}adv_phy_property_e;

typedef struct
{
    adv_event_class_e eventClass;
    adv_pdu_type_e    pduType;
    _u32              phyProperty;
}adv_procedure_list_t;

typedef struct
{
    adv_event_type_e      eventType;//adv mode
    adv_procedure_list_t *procedureList;
    _u32                  listLen;
}adv_sequence_t;

#define ADV_PROCEDURE_LIST_LENGTH(adv_procedure_list)      (sizeof(adv_procedure_list)/sizeof(adv_procedure_list[0]))
#define ADV_SEQUENCE_LIST_LENGTH(adv_sequence_list)        (sizeof(adv_sequence_list)/sizeof(adv_sequence_list[0]))

static const adv_procedure_list_t adv_con_scan_undirected_procedure[] =
{
    {ADV_EVENT,ADV_PDU_IND,ADV_PHY_TX|ADV_PHY_RX},
};

static const adv_procedure_list_t adv_con_directed_procedure[]=
{
    {ADV_EVENT,ADV_PDU_DIRECT_IND,ADV_PHY_TX|ADV_PHY_RX},
};

static const adv_procedure_list_t adv_scan_undirected_procedure[]=
{
    {ADV_EVENT,ADV_PDU_SCAN_IND,ADV_PHY_TX|ADV_PHY_RX},
};

static const adv_procedure_list_t adv_non_con_non_scan_undirected_procedure[]=
{
    {ADV_EVENT,ADV_PDU_NONCONN_IND,ADV_PHY_TX},
};

#ifdef(LL_SUPPORT_LE_EXTENDED_ADVERTISING)
static const adv_procedure_list_t adv_extended_con_undirected_procedure[]=
{
    {ADV_EVENT,         ADV_PDU_EXT_IND,     ADV_PHY_TX},
    {ADV_EXTENDED_EVENT,ADV_PDU_AUX_IND,     ADV_PHY_TX|ADV_PHY_RX},
};

static const adv_procedure_list_t adv_extended_con_directed_procedure[]=
{
    {ADV_EVENT,         ADV_PDU_EXT_IND,ADV_PHY_TX},
    {ADV_EXTENDED_EVENT,ADV_PDU_AUX_IND,ADV_PHY_TX|ADV_PHY_RX},
};

static const adv_procedure_list_t adv_extended_scan_undirected_procedure[]=
{
    {ADV_EVENT,         ADV_PDU_EXT_IND,ADV_PHY_TX},
    {ADV_EXTENDED_EVENT,ADV_PDU_AUX_IND,ADV_PHY_TX|ADV_PHY_RX},
};

static const adv_procedure_list_t adv_extended_scan_directed_procedure[]=
{
    {ADV_EVENT,         ADV_PDU_EXT_IND,ADV_PHY_TX},
    {ADV_EXTENDED_EVENT,ADV_PDU_AUX_IND,ADV_PHY_TX|ADV_PHY_RX},
};

static const adv_procedure_list_t adv_extended_non_con_non_scan_undirected_procedure[]=
{
    {ADV_EVENT,         ADV_PDU_EXT_IND,ADV_PHY_TX},
    {ADV_EXTENDED_EVENT,ADV_PDU_AUX_IND,ADV_PHY_TX},
};

static const adv_procedure_list_t adv_extended_non_con_non_scan_directed_procedure[]=
{
    {ADV_EVENT,         ADV_PDU_EXT_IND,ADV_PHY_TX},
    {ADV_EXTENDED_EVENT,ADV_PDU_AUX_IND,ADV_PHY_TX},
};

static const adv_procedure_list_t adv_extended_periodic_with_rsp_procedure[]=
{
    {ADV_EVENT,                  ADV_PDU_EXT_IND,              ADV_PHY_TX},
    {ADV_EXTENDED_EVENT,         ADV_PDU_AUX_IND,              ADV_PHY_TX},
    {ADV_PERIODIC_WITH_RSP_EVENT,ADV_PDU_AUX_SYNC_SUBEVENT_IND,ADV_PHY_TX|ADV_PHY_RX},
};

static const adv_procedure_list_t adv_extended_periodic_without_rsp_procedure[]=
{
    {ADV_EVENT,                     ADV_PDU_EXT_IND,     ADV_PHY_TX},
    {ADV_EXTENDED_EVENT,            ADV_PDU_AUX_IND,     ADV_PHY_TX},
    {ADV_PERIODIC_WITHOUT_RSP_EVENT,ADV_PDU_AUX_SYNC_IND,ADV_PHY_TX|ADV_PHY_RX},
};
#endif

static adv_sequence_t advTrain[] = 
{
    {ADV_EVENT_CONNECTABLE_SCANNABLE_UNDIRECTED,                  adv_con_scan_undirected_procedure,                 ADV_PROCEDURE_LIST_LENGTH(adv_con_scan_undirected_procedure)},
    {ADV_EVENT_CONNECTABLE_DIRECTED,                              adv_con_directed_procedure,                        ADV_PROCEDURE_LIST_LENGTH(adv_con_directed_procedure)},
    {ADV_EVENT_SCANNABLE_UNDIRECTED,                              adv_scan_undirected_procedure,                     ADV_PROCEDURE_LIST_LENGTH(adv_scan_undirected_procedure)},
    {ADV_EVENT_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED,          adv_non_con_non_scan_undirected_procedure,         ADV_PROCEDURE_LIST_LENGTH(adv_non_con_non_scan_undirected_procedure)},
    #ifdef(LL_SUPPORT_LE_EXTENDED_ADVERTISING)
    {ADV_EVENT_EXTENDED_CONNECTABLE_DIRECTED,                     adv_extended_con_undirected_procedure,             ADV_PROCEDURE_LIST_LENGTH(adv_extended_con_undirected_procedure)},
    {ADV_EVENT_EXTENDED_CONNECTABLE_UNDIRECTED,                   adv_extended_con_directed_procedure,               ADV_PROCEDURE_LIST_LENGTH(adv_extended_con_directed_procedure)},
    {ADV_EVENT_EXTENDED_SCANNABLE_DIRECTED,                       adv_extended_scan_undirected_procedure,            ADV_PROCEDURE_LIST_LENGTH(adv_extended_scan_undirected_procedure)},
    {ADV_EVENT_EXTENDED_SCANNABLE_UNDIRECTED,                     adv_extended_scan_directed_procedure,              ADV_PROCEDURE_LIST_LENGTH(adv_extended_scan_directed_procedure)},
    {ADV_EVENT_EXTENDED_NON_CONNECTABLE_NON_SCANNABLE_DIRECTED    adv_extended_non_con_non_scan_undirected_procedure,ADV_PROCEDURE_LIST_LENGTH(adv_extended_non_con_non_scan_undirected_procedure)},
    {ADV_EVENT_EXTENDED_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED, adv_extended_non_con_non_scan_directed_procedure,  ADV_PROCEDURE_LIST_LENGTH(adv_extended_non_con_non_scan_directed_procedure)},
    {ADV_EVENT_EXTENDED_PERIODIC_WITHOUT_RESPONSE,                adv_extended_periodic_with_rsp_procedure,          ADV_PROCEDURE_LIST_LENGTH(adv_extended_periodic_with_rsp_procedure)},
    {ADV_EVENT_EXTENDED_PERIODIC_WITH_RESPONSE,                   adv_extended_periodic_without_rsp_procedure,       ADV_PROCEDURE_LIST_LENGTH(adv_extended_periodic_without_rsp_procedure)},
    #endif
}

_RAM_CODE
static void adv_phy_irq_callback(_u8 type)
{   

}

_RAM_CODE
static void adv_sch_callback(_u8 type)
{

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