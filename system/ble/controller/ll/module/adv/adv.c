
#include"adv.h"
#include"../scan/scan.h"
#include"../init/init.h"


#define ADV_SCHE_PRIMARY_37         BIT(0)
#define ADV_SCHE_PRIMARY_38         BIT(1)
#define ADV_SCHE_PRIMARY_39         BIT(2)

typedef enum
{
    ADV_PDU_IND                      = BIT(0),
    ADV_PDU_DIRECT_IND               = BIT(1),
    ADV_PDU_SCAN_IND                 = BIT(2),
    ADV_PDU_NONCONN_IND              = BIT(3),
    #if(LL_SUPPORT_LE_EXTENDED_ADVERTISING==1)
    ADV_PDU_EXT_IND                  = BIT(4),
    ADV_PDU_DECISION_IND             = BIT(5),
    ADV_PDU_AUX_IND                  = BIT(6),
    ADV_PDU_AUX_CHAIN_IND            = BIT(7),
    #endif
    #if(LL_SUPPORT_LE_PERIODIC_ADVERTISING==1)
    ADV_PDU_AUX_SYNC_IND             = BIT(8),
    #endif
    #if(LL_SUPPORT_PERIODIC_ADVERTISING_WITH_RESPONSES_ADVERTISER==1)
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
    adv_event_type_e            eventType;//adv mode
    adv_procedure_list_t const  *procedureList;
    _u32                        listLen;
}adv_sequence_t;

typedef void(*adv_event_sch_process_f)(_u8);
typedef void(*adv_event_phy_process_f)(_u8);
typedef struct
{
    adv_event_class_e       eventClass;
    adv_event_sch_process_f schCb;
    adv_event_sch_process_f phyCb;
}adv_event_process_t;

_RAM_CODE 
static adv_event_class_e adv_get_current_event_class(ll_ctrl_t* ll)
{
    return ADV_EVENT;
}

_RAM_CODE
static void adv_event_prepare_packet(ll_ctrl_t* ll,_u8 rsp)
{
	_u8* packet = NULL;
    if(rsp)
    {
        packet = ll_get_adv_packet(ll->txSharedPacket,6+ll->adv->scanRspDataLen,LL_ADV_TYPE_SCAN_RSP,0,ll->adv->ownAddressType?1:0,0);
        txMemcpy(((adv_type_scan_rsp_t*)packet)->advA,ll->ownAddr,6);
        txMemcpy(((adv_type_scan_rsp_t*)packet)->scanRsp,ll->adv->scanRspData,ll->adv->scanRspDataLen);
        return;
    }
	switch(ll->adv->advType)
	{
		case LL_ADV_IND:
		     packet = ll_get_adv_packet(ll->txSharedPacket,6+ll->adv->advDataLen,LL_ADV_TYPE_ADV_IND,0,ll->adv->ownAddressType?1:0,0);
		     txMemcpy(((adv_type_ind_t*)packet)->advA,ll->ownAddr,6);
		     txMemcpy(((adv_type_ind_t*)packet)->advData,ll->adv->advData,ll->adv->advDataLen);
		     break;
        case LL_ADV_DIRECT_IND_LOW_DUTY:
		case LL_ADV_DIRECT_IND_HIGH_DUTY:
			 packet = ll_get_adv_packet(ll->txSharedPacket,12,LL_ADV_TYPE_ADV_DIRECT_IND,0,ll->adv->ownAddressType?1:0,ll->adv->peerAddressType?1:0);
		     txMemcpy(((adv_type_direct_ind_t*)packet)->advA,ll->ownAddr,6);
		     txMemcpy(((adv_type_direct_ind_t*)packet)->targetA,ll->adv->peerAddress,6);
			 break;
		case LL_ADV_SCAN_IND:
		     packet = ll_get_adv_packet(ll->txSharedPacket,6+ll->adv->advDataLen,LL_ADV_TYPE_ADV_SCAN_IND,0,ll->adv->ownAddressType?1:0,0);
		     txMemcpy(((adv_type_scan_ind_t*)packet)->advA,ll->ownAddr,6);
		     txMemcpy(((adv_type_scan_ind_t*)packet)->advData,ll->adv->advData,ll->adv->advDataLen);
		     break;
		case LL_ADV_NONCONN_IND:
		     packet = ll_get_adv_packet(ll->txSharedPacket,6+ll->adv->advDataLen,LL_ADV_TYPE_ADV_NONCONN_IND,0,ll->adv->ownAddressType?1:0,0);
		     txMemcpy(((adv_type_nonConn_ind_t*)packet)->advA,ll->ownAddr,6);
		     txMemcpy(((adv_type_nonConn_ind_t*)packet)->advData,ll->adv->advData,ll->adv->advDataLen);
		     break;
	}
}

_RAM_CODE
static void adv_event_prepare_phy(ll_ctrl_t* ll,phy_dir_e phydir,phy_mode_e mode)
{
    phy_obj_cast(&ll->phy);
    ll->phy.accessCode = BLE_ADV_ACCESS_CODE;
    ll->phy.crcInit    = BLE_ADV_CRC_INIT;
    ll->phy.mode       = mode;
    ll->phy.dir        = phydir;
    if(phydir == PHY_DIR_TX)
    {
        ll->phy.txAddress  = ll->txSharedPacket;
        ll->phy.timestamp  = ll->sch.timestamp;
    }
    else if(phydir == PHY_DIR_RX)
    {
        ll->phy.rxTimeout = PACKET_DEFAULT_TIFS_TIME+50;
        ll->phy.rxMaxOctets = BLE_ADV_MAX_LENGTH;
        ll->phy.timestamp = system_time();//start rx as soon as possible
    }
}

_RAM_CODE 
static int adv_event_received_packet_analyze(ll_ctrl_t* ll,ll_adv_packet_t* packet)
{

}

static void adv_event_sch_process(_u8 schType)
{
    ll_ctrl_t* ll = ll_get_current_state_machine();
    switch(schType)
    {
        case SCH_TASK_START:
        {
            ll->adv->instant++;
            adv_event_prepare_packet(ll,0);
            adv_event_prepare_phy(ll,PHY_DIR_TX,ll->adv->advEventPhyMode);
            ll->phy.start();
            ll->adv->availableChnCnt--;
        }
            break;
        case SCH_TASK_STOP:
        case SCH_TASK_CANCELED:
        case SCH_TASK_PASSED:
        {
            ll->adv->availableChnCnt--;
            if(ll->adv->availableChnCnt == 0)
            {
                ll->sch.timestamp+=ll->sch.period;//need to add a random value,from 0ms to 10ms.
                ll->adv->availableChnCnt = ll->adv->channelCnt;
            }
            else
            {
                ll->sch.timestamp+=(ll->sch.duration+ll->sch.startLatency+ll->sch.stopLatency);
            }
        }
            break;
    }
}

static void adv_event_phy_process(_u8 phyType)
{
    ll_ctrl_t* ll = ll_get_current_state_machine();
    switch(phyType)
    {
        case PHY_IRQ_TX_FINISHED:
        {
            _u8 eventClass = adv_get_current_event_class(ll);
            if(advSequence[ll->adv->advEventType].procedureList[eventClass].phyProperty & ADV_PHY_RX)
            {
                adv_event_prepare_phy(ll,PHY_DIR_RX,ll->adv->advEventPhyMode);
            }
            else
            {
                sch_schedule_next_task();
            }
        }
            break;
        case PHY_IRQ_RX_FINISHED:
        {
            if(ll->phy.hw_is_rx_packet_valid())
            {
                if(adv_event_received_packet_analyze(ll,(ll_adv_packet_t*)(ll->phy.rxAddress + ll_get_packet_header_offset_from_address(PHY_DIR_RX)))!=0)
                {
                    adv_event_prepare_packet(ll,1);
                    adv_event_prepare_phy(ll,PHY_DIR_TX,ll->adv->advEventPhyMode);
                    ll->phy.start();
                }
            }
        }
            break;
        case PHY_IRQ_RX_TIMEOUT:
        {
            sch_schedule_next_task();
        }
            break;
    }
}

#if(LL_SUPPORT_LE_EXTENDED_ADVERTISING==1)
static void adv_extended_event_sch_process(_u8 schType)
{
    
}
static void adv_extended_event_phy_process(_u8 schType)
{
    
}
#endif

#if(LL_SUPPORT_LE_PERIODIC_ADVERTISING==1)
static void adv_periodic_event_sch_process(_u8 schType)
{
    
}
static void adv_periodic_event_phy_process(_u8 schType)
{
    
}
#endif

#if(LL_SUPPORT_PERIODIC_ADVERTISING_WITH_RESPONSES_ADVERTISER==1)
static void adv_periodic_with_rsp_event_sch_process(_u8 schType)
{
    
}
static void adv_periodic_with_rsp_event_phy_process(_u8 schType)
{
    
}
#endif


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

#if(LL_SUPPORT_LE_EXTENDED_ADVERTISING==1)
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
#endif

#if(LL_SUPPORT_LE_PERIODIC_ADVERTISING==1)
static const adv_procedure_list_t adv_extended_periodic_procedure[]=
{
    {ADV_EVENT,                     ADV_PDU_EXT_IND,     ADV_PHY_TX},
    {ADV_EXTENDED_EVENT,            ADV_PDU_AUX_IND,     ADV_PHY_TX},
    {ADV_PERIODIC_EVENT,            ADV_PDU_AUX_SYNC_IND,ADV_PHY_TX|ADV_PHY_RX},
};
#endif

#if(LL_SUPPORT_PERIODIC_ADVERTISING_WITH_RESPONSES_ADVERTISER==1)
static const adv_procedure_list_t adv_extended_periodic_with_rsp_procedure[]=
{
    {ADV_EVENT,                  ADV_PDU_EXT_IND,              ADV_PHY_TX},
    {ADV_EXTENDED_EVENT,         ADV_PDU_AUX_IND,              ADV_PHY_TX},
    {ADV_PERIODIC_WITH_RSP_EVENT,ADV_PDU_AUX_SYNC_SUBEVENT_IND,ADV_PHY_TX|ADV_PHY_RX},
};
#endif

static adv_event_process_t advProcess[] = 
{
    {ADV_EVENT,                      adv_event_sch_process,                      adv_event_phy_process},
    #if(LL_SUPPORT_LE_EXTENDED_ADVERTISING==1)
    {ADV_EXTENDED_EVENT,             adv_extended_event_sch_process,             adv_extended_event_phy_process},
    #endif
    #if(LL_SUPPORT_LE_PERIODIC_ADVERTISING==1)
    {ADV_PERIODIC_EVENT,             adv_periodic_event_sch_process,             adv_periodic_event_phy_process},
    #endif
    #if(LL_SUPPORT_PERIODIC_ADVERTISING_WITH_RESPONSES_ADVERTISER==1)
    {ADV_PERIODIC_WITH_RSP_EVENT,    adv_periodic_with_rsp_event_sch_process,    adv_periodic_with_rsp_event_phy_process},
    #endif
};

static adv_sequence_t advSequence[] = 
{
    {ADV_EVENT_CONNECTABLE_SCANNABLE_UNDIRECTED,                  adv_con_scan_undirected_procedure,                 ADV_PROCEDURE_LIST_LENGTH(adv_con_scan_undirected_procedure)},
    {ADV_EVENT_CONNECTABLE_DIRECTED,                              adv_con_directed_procedure,                        ADV_PROCEDURE_LIST_LENGTH(adv_con_directed_procedure)},
    {ADV_EVENT_SCANNABLE_UNDIRECTED,                              adv_scan_undirected_procedure,                     ADV_PROCEDURE_LIST_LENGTH(adv_scan_undirected_procedure)},
    {ADV_EVENT_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED,          adv_non_con_non_scan_undirected_procedure,         ADV_PROCEDURE_LIST_LENGTH(adv_non_con_non_scan_undirected_procedure)},
    #if(LL_SUPPORT_LE_EXTENDED_ADVERTISING==1)
    {ADV_EVENT_EXTENDED_CONNECTABLE_DIRECTED,                     adv_extended_con_undirected_procedure,             ADV_PROCEDURE_LIST_LENGTH(adv_extended_con_undirected_procedure)},
    {ADV_EVENT_EXTENDED_CONNECTABLE_UNDIRECTED,                   adv_extended_con_directed_procedure,               ADV_PROCEDURE_LIST_LENGTH(adv_extended_con_directed_procedure)},
    {ADV_EVENT_EXTENDED_SCANNABLE_DIRECTED,                       adv_extended_scan_undirected_procedure,            ADV_PROCEDURE_LIST_LENGTH(adv_extended_scan_undirected_procedure)},
    {ADV_EVENT_EXTENDED_SCANNABLE_UNDIRECTED,                     adv_extended_scan_directed_procedure,              ADV_PROCEDURE_LIST_LENGTH(adv_extended_scan_directed_procedure)},
    {ADV_EVENT_EXTENDED_NON_CONNECTABLE_NON_SCANNABLE_DIRECTED,   adv_extended_non_con_non_scan_undirected_procedure,ADV_PROCEDURE_LIST_LENGTH(adv_extended_non_con_non_scan_undirected_procedure)},
    {ADV_EVENT_EXTENDED_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED, adv_extended_non_con_non_scan_directed_procedure,  ADV_PROCEDURE_LIST_LENGTH(adv_extended_non_con_non_scan_directed_procedure)},
    #endif
    #if(LL_SUPPORT_LE_PERIODIC_ADVERTISING==1)
    {ADV_EVENT_EXTENDED_PERIODIC,                                 adv_extended_periodic_procedure,                   ADV_PROCEDURE_LIST_LENGTH(adv_extended_periodic_procedure)},
    #endif
    #if(LL_SUPPORT_PERIODIC_ADVERTISING_WITH_RESPONSES_ADVERTISER==1)
    {ADV_EVENT_EXTENDED_PERIODIC_WITH_RESPONSE,                   adv_extended_periodic_with_rsp_procedure,          ADV_PROCEDURE_LIST_LENGTH(adv_extended_periodic_with_rsp_procedure)},
    #endif
};

_RAM_CODE
static void adv_phy_irq_callback(_u8 type)
{   
    ll_ctrl_t* ll = ll_get_current_state_machine();
    for(_u32 i=0;i<advSequence[ll->adv->advEventType].listLen;i++)
    {
        if(ll->adv->advProcessingEventClass == advSequence[ll->adv->advEventType].procedureList[i].eventClass)
        {
            advProcess[i].phyCb(type);
        }
    }
}

_RAM_CODE
static void adv_sch_callback(_u8 type)
{
    ll_ctrl_t* ll = ll_get_current_state_machine();
    if(type == SCH_TASK_START)
    {
        ll->adv->advProcessingEventClass = adv_get_current_event_class(ll);
    }
    _u8 eventClass = adv_get_current_event_class(ll);
    advProcess[eventClass].schCb(type);
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
        ll->phy.hw_irq_cb  = adv_phy_irq_callback;
        ll->ownAddr[0] = ll->ownAddr[1] = ll->ownAddr[2] = ll->ownAddr[3]= ll->ownAddr[4] =ll->ownAddr[5]= 0x12;
        ll->adv->instant = 0;
        ll->adv->channelCnt = count_bits_one((_u32)ll->adv->channelMap);
        ll->adv->availableChnCnt = ll->adv->channelCnt;
        ll->adv->advEventPhyMode = PHY_MODE_1M;
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