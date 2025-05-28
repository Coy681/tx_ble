#include"adv.h"


static void adv_phy_irq_tx(void)
{
	DEBUG_GPIO_HIGH(GPIO_7);
	DEBUG_GPIO_LOW(GPIO_7);
}
static void adv_phy_irq_rx(void)
{
	DEBUG_GPIO_HIGH(GPIO_7);
	DEBUG_GPIO_LOW(GPIO_7);
}
static void adv_phy_irq_rx_timeout(void)
{
	DEBUG_GPIO_HIGH(GPIO_7);
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

volatile AAA_RF_LEN = 0;

static void adv_sch_start(void)
{
    ll_ctrl_t* ll = ll_get_current_state_machine();
	DEBUG_GPIO_HIGH(GPIO_6);
    phy_obj_cast(&ll->phy);
	ll->phy.accessCode = BLE_ADV_ACCESS_CODE;
    ll->phy.chnIdx     = BLE_ADV_CHANNEL_IDX_BIT0;//ll->adv->channelMap;
    ll->phy.crcInit    = BLE_ADV_CRC_INIT;
    ll->phy.dir        = PHY_DIR_TX;
    ll->phy.chnIdx     = 37;
    ll->phy.mode       = PHY_MODE_1M;
    ll->phy.rxAddress  = ll->rxSharedPacket;
    ll->phy.txAddress  = ll->txSharedPacket;
    ll->phy.timestamp  = ll->sch.timestamp;
    if(BIT_EXIST(ll->adv->status,BLE_ADV_STATUS_CHANGE_ADV_DATA))
    {
        adv_prepare_packet(ll);
        BIT_CLR(ll->adv->status,BLE_ADV_STATUS_CHANGE_ADV_DATA);
    }
    AAA_RF_LEN = ll->adv->advDataLen;

	ll->phy.start();
	DEBUG_GPIO_LOW(GPIO_6);
}
static void adv_sch_stop(void)
{
//	DEBUG_GPIO_HIGH(GPIO_7);
//	DEBUG_GPIO_LOW(GPIO_7);
}
static void adv_sch_calceled(void)
{
    
}
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
