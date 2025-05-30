#include"adv.h"
#include"../scan/scan.h"
#include"../init/init.h"


#define ADV_PROCESS_ADV          1
#define ADV_PROCESS_SCAN         2
#define ADV_PROCESS_CONNECT      3

_RAM_CODE
static void adv_phy_irq_tx(void)
{
	DEBUG_GPIO_HIGH(GPIO_5);
    ll_ctrl_t* ll = ll_get_current_state_machine();
    if(ll->adv->process == ADV_PROCESS_ADV)
    {
        if(ll->adv->advType != LL_ADV_NONCONN_IND)
        {
            //rx prepare
            ll->phy.rxTimeout = PACKET_DEFAULT_TIFS_TIME+50;
            ll->phy.rxMaxOctets = BLE_ADV_MAX_LENGTH;
            ll->phy.dir = PHY_DIR_RX;
            ll->phy.timestamp = system_time();
            ll->phy.start();
        }
    }
	//adv train continue
	if((ll->adv->instant%ll->adv->channelCnt)!=0)
	{
		ll->sch.timestamp+=1000;
		sch_schedule_next_task();
	}
	DEBUG_GPIO_LOW(GPIO_5);
}
volatile _u8 AAA_ADV_BUFFER[100];
_RAM_CODE
static void adv_rx_packet_analyze(ll_ctrl_t* ll,ll_adv_packet_t* packet)
{
	txMemcpy(AAA_ADV_BUFFER,ll->rxSharedPacket,100);
    if(packet->hdr.pduType == LL_ADV_TYPE_SCAN_REQ && packet->hdr.length == sizeof(scan_type_scan_req_t))
    {
        //scan req process
        scan_type_scan_req_t* scanReq = (scan_type_scan_req_t*)(packet->data);
        ll->adv->process = ADV_PROCESS_SCAN;
    }
    else if(packet->hdr.pduType == LL_ADV_TYPE_CONNECT_IND && packet->hdr.length == sizeof(init_type_connectInd_t))
    {
        //connect ind process
        init_type_connectInd_t* connInd = (init_type_connectInd_t*)(packet->data);
        ll->adv->process = ADV_PROCESS_CONNECT;
    }
    while(1);
}
_RAM_CODE
static void adv_phy_irq_rx(void)
{
	DEBUG_GPIO_HIGH(GPIO_6);
    ll_ctrl_t* ll = ll_get_current_state_machine();
    if(ll->phy.hw_is_rx_packet_valid())
    {
        adv_rx_packet_analyze(ll,(ll_adv_packet_t*)(ll->phy.rxAddress + ll_get_packet_header_offset(PHY_DIR_RX)));
    }
    else
    {
        if((ll->adv->instant%ll->adv->channelCnt)!=0)
        {
            ll->sch.timestamp+=1000;
            sch_schedule_next_task();
        }
    }
	DEBUG_GPIO_LOW(GPIO_6);
}
_RAM_CODE
static void adv_phy_irq_rx_timeout(void)
{
	DEBUG_GPIO_HIGH(GPIO_7);
    ll_ctrl_t* ll = ll_get_current_state_machine();
    if((ll->adv->instant%ll->adv->channelCnt)!=0)
    {
        ll->sch.timestamp+=1000;
        sch_schedule_next_task();
    }
	DEBUG_GPIO_LOW(GPIO_7);
}

static void(*adv_phy_irq_cb[3])(void)=
{
    adv_phy_irq_tx,
    adv_phy_irq_rx,
    adv_phy_irq_rx_timeout,   
};

static void adv_phy_irq_callback(_u8 type)
{   
    adv_phy_irq_cb[type]();
}
static _u8 advPriChannel[3]={
	BLE_ADV_CHANNEL_IDX_BIT0,
	BLE_ADV_CHANNEL_IDX_BIT1,
	BLE_ADV_CHANNEL_IDX_BIT2
};

_RAM_CODE
static void adv_prepare_packet(ll_ctrl_t* ll)
{
	_u8* packet = NULL;
	switch(ll->adv->advType)
	{
		case LL_ADV_IND:
		     packet = ll_get_adv_packet(ll->txSharedPacket,6+ll->adv->advDataLen,LL_ADV_TYPE_ADV_IND,0,ll->adv->ownAddressType?1:0,0);
		     txMemcpy(((adv_type_ind_t*)packet)->advA,ll->ownAddr,6);
		     txMemcpy(((adv_type_ind_t*)packet)->advData,ll->adv->advData,ll->adv->advDataLen);
		     break;
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
		case LL_ADV_DIRECT_IND_LOW_DUTY:
			 packet = ll_get_adv_packet(ll->txSharedPacket,12,LL_ADV_TYPE_ADV_DIRECT_IND,0,ll->adv->ownAddressType?1:0,ll->adv->peerAddressType?1:0);
		     txMemcpy(((adv_type_direct_ind_t*)packet)->advA,ll->ownAddr,6);
		     txMemcpy(((adv_type_direct_ind_t*)packet)->targetA,ll->adv->peerAddress,6);
		     break;
	}
}

_RAM_CODE
static void adv_sch_start(void)
{
    ll_ctrl_t* ll = ll_get_current_state_machine();
	DEBUG_GPIO_HIGH(GPIO_3);
    ll->adv->instant++;
    phy_obj_cast(&ll->phy);
	ll->phy.accessCode = BLE_ADV_ACCESS_CODE;
    ll->phy.chnIdx     = BLE_ADV_CHANNEL_IDX_BIT0;//ll->adv->channelMap;
    ll->phy.crcInit    = BLE_ADV_CRC_INIT;
    ll->phy.dir        = PHY_DIR_TX;
    ll->phy.chnIdx     = advPriChannel[ll->adv->instant%3];
    ll->phy.mode       = PHY_MODE_1M;
    ll->phy.rxAddress  = ll->rxSharedPacket;
    ll->phy.txAddress  = ll->txSharedPacket;
    ll->phy.timestamp  = ll->sch.timestamp;
    if(BIT_EXIST(ll->adv->status,BLE_ADV_STATUS_CHANGE_ADV_DATA))
    {
        adv_prepare_packet(ll);
        BIT_CLR(ll->adv->status,BLE_ADV_STATUS_CHANGE_ADV_DATA);
    }
	ll->phy.start();
	ll->adv->process = ADV_PROCESS_ADV;
	DEBUG_GPIO_LOW(GPIO_3);
}

_RAM_CODE
static void adv_sch_stop(void)
{
	DEBUG_GPIO_HIGH(GPIO_4);
    ll_ctrl_t* ll = ll_get_current_state_machine();
    if((ll->adv->instant%ll->adv->channelCnt)==0)//adv train stop
    {
        ll->sch.timestamp+=ll->sch.period;
    }
	DEBUG_GPIO_LOW(GPIO_4);
}

_RAM_CODE
static void adv_sch_calceled(void)
{
    
}

_RAM_CODE
static void adv_sch_passed(void)
{
    
}

static void(*adv_sch_process[4])(void)={
    adv_sch_start,
    adv_sch_stop,
    adv_sch_calceled,
    adv_sch_passed,
};

static void adv_sch_callback(_u8 type)
{
    adv_sch_process[type]();
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
