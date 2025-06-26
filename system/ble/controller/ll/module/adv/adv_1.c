
#include"adv.h"
#include"../scan/scan.h"
#include"../init/init.h"


#define ADV_SCHE_PRIMARY_37         BIT(0)
#define ADV_SCHE_PRIMARY_38         BIT(1)
#define ADV_SCHE_PRIMARY_39         BIT(2)
#ifdef(LL_SUPPORT_LE_EXTENDED_ADVERTISING)
#define ADV_SCHE_SECONDARY_AUX      BIT(3)
#define ADV_SCHE_SECONDARY_SYNC     BIT(4)
#endif

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

typedef struct
{
    adv_pdu_type_e pdu;

}adv_procedure_list_t

static const adv_procedure_list_t adv_con_scan_undirected[] =
{
    
}

static const adv_procedure_list_t adv_con_directed[]=
{
    
}

static const adv_procedure_list_t adv_scan_undirected[]=
{
    
}

static const adv_procedure_list_t adv_non_con_non_scan_undirected[]=
{
    
}

static const adv_procedure_list_t adv_extended_con_undirected[]=
{
    
}

static const adv_procedure_list_t adv_extended_con_directed[]=
{
    
}

static const adv_procedure_list_t adv_extended_scan_undirected[]=
{
    
}

static const adv_procedure_list_t adv_extended_scan_directed[]=
{
    
}

static const adv_procedure_list_t adv_extended_non_con_non_scan_undirected[]=
{
    
}

static const adv_procedure_list_t adv_extended_non_con_non_scan_directed[]=
{
    
}

static const adv_procedure_list_t adv_extended_periodic_with_rsp[]=
{
    
}
static const adv_procedure_list_t adv_extended_periodic_without_rsp[]=
{
    
}

typedef struct
{
    adv_event_e eventType;//adv mode
    
}adv_sequence_t;

static adv_sequence_t advTrain[] = 
{
    {ADV_EVENT_CONNECTABLE_SCANNABLE_UNDIRECTED,                  ADV_PDU_IND,                                       },
    {ADV_EVENT_CONNECTABLE_DIRECTED,                              ADV_PDU_DIRECT_IND,                                },
    {ADV_EVENT_SCANNABLE_UNDIRECTED,                              ADV_PDU_SCAN_IND,                                  },
    {ADV_EVENT_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED,          ADV_PDU_NONCONN_IND,                               },
    #ifdef(LL_SUPPORT_LE_EXTENDED_ADVERTISING)
    {ADV_EVENT_EXTENDED_CONNECTABLE_DIRECTED,                     ADV_PDU_EXT_IND|ADV_PDU_AUX_IND,},
    {ADV_EVENT_EXTENDED_CONNECTABLE_UNDIRECTED,                   ADV_PDU_EXT_IND|ADV_PDU_AUX_IND|ADV_PDU_DECISION_IND},
    {ADV_EVENT_EXTENDED_SCANNABLE_DIRECTED,                       ADV_PDU_EXT_IND|ADV_PDU_AUX_IND,},
    {ADV_EVENT_EXTENDED_SCANNABLE_UNDIRECTED,                     ADV_PDU_EXT_IND|ADV_PDU_AUX_IND|ADV_PDU_DECISION_IND,},
    {ADV_EVENT_EXTENDED_NON_CONNECTABLE_NON_SCANNABLE_DIRECTED    ADV_PDU_EXT_IND|ADV_PDU_AUX_IND},
    {ADV_EVENT_EXTENDED_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED, ADV_PDU_EXT_IND|ADV_PDU_AUX_IND|ADV_PDU_DECISION_IND},
    {ADV_EVENT_EXTENDED_PERIODIC_WITHOUT_RESPONSE,                ADV_PDU_AUX_IND|ADV_PDU_AUX_CHAIN_IND},
    {ADV_EVENT_EXTENDED_PERIODIC_WITH_RESPONSE,                   },

    #endif
}

_RAM_CODE
static void adv_phy_send_finished(void)
{

}

_RAM_CODE
static void adv_phy_receive_finished(void)
{

}

_RAM_CODE
static void adv_phy_receive_timeout(void)
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
    adv_phy_send_finished,
    adv_phy_receive_finished,
    adv_phy_receive_timeout,   
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