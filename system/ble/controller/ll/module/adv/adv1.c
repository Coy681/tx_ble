/*
 * adv1.c
 *
 *  Created on: 2025年12月15日
 *      Author: Admin
 */

#include"adv.h"
#include"adv_internal.h"
#include"system/scheduler/sch_map.h"
#include"../../channel/channel.h"

/************************************Global Variables Define***********************************/
//here use uppercase letters to distinguish between global and local items.
ll_sm_t* LLSM;
//adv ctrl pointer,point to current adv struct
ll_internal_adv_ctrl_t* advCtrl;

/*************************************ADV Schedule Process*******************************************/
_RAM_CODE
static void adv_phy_irq_callback(_u8 type)
{
    if(type == PHY_IRQ_TX_FINISHED)
    {
        DEBUG_GPIO_HIGH(GPIO_8);
        DEBUG_GPIO_LOW(GPIO_8);
    }
    else if(type == PHY_IRQ_RX_FINISHED)
    {
        DEBUG_GPIO_HIGH(GPIO_9);
        DEBUG_GPIO_LOW(GPIO_9);
    }
    else if(type == PHY_IRQ_RX_TIMEOUT)
    {
        DEBUG_GPIO_HIGH(GPIO_10);
        DEBUG_GPIO_LOW(GPIO_10);
    }
//    adv_sequence_process(LL_PHY_EVENT,type);
}

_RAM_CODE
static void adv_sch_callback(_u8 type)
{
    if(type == SCH_TASK_START)
    {
        DEBUG_GPIO_HIGH(GPIO_11);
        DEBUG_GPIO_LOW(GPIO_11);
    }
    else if(type == SCH_TASK_STOP)
    {
        DEBUG_GPIO_HIGH(GPIO_12);
        DEBUG_GPIO_LOW(GPIO_12);
    }
    else if(type == SCH_TASK_CANCELED)
    {
        DEBUG_GPIO_HIGH(GPIO_13);
        DEBUG_GPIO_LOW(GPIO_13);
    }
    else if(type == SCH_TASK_PASSED)
    {
        DEBUG_GPIO_HIGH(GPIO_14);
        DEBUG_GPIO_LOW(GPIO_14);
    }
//    adv_sequence_process(LL_SCH_EVENT,type);
}

/*************************************LL APIs Define*******************************************/
static void ble_ll_adv_reset(void)
{
	LLSM = ll_get_sm_entity_by_state(BLE_LL_STATE_ADVERTISING,LL_SM_UNVALID_HANDLE,0);
	ASSERT(POINTER_VALID(LLSM));
	advCtrl = (ll_internal_adv_ctrl_t*)LLSM->entity;
	ASSERT(POINTER_VALID(advCtrl));
	for(int i=0;i<BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS;i++)
	{
		if(POINTER_VALID(advCtrl->param[i].la))
		{
			tx_free((_u8*)advCtrl->param[i].la);
			advCtrl->param[i].la = NULL;
		}
		if(POINTER_VALID(advCtrl->param[i].data.addr))
		{
			tx_free(advCtrl->param[i].data.addr);
			advCtrl->param[i].data.addr = NULL;
		}
		if(POINTER_VALID(advCtrl->param[i].scanRsp.addr))
		{
			tx_free(advCtrl->param[i].scanRsp.addr);
			advCtrl->param[i].scanRsp.addr = NULL;
		}
		#if(LL_SUPPORT_LE_EXTENDED_ADVERTISING)
		if(POINTER_VALID(advCtrl->param[i].ea))
		{
			if(POINTER_VALID(advCtrl->param[i].ea->chain.entry))
			{
				tx_free((_u8*)advCtrl->param[i].ea->chain.entry);
				advCtrl->param[i].ea->chain.entry = NULL;
			}
			tx_free((_u8*)advCtrl->param[i].ea);
			advCtrl->param[i].ea = NULL;
		}
		#endif
		#if(LL_SUPPORT_LE_PERIODIC_ADVERTISING)
		#endif
		#if(LL_SUPPORT_PERIODIC_ADVERTISING_WITH_RESPONSES_ADVERTISER)
		#endif
	}
}

void ble_ll_enter_advertising_state(void)
{
	LLSM = ll_get_sm_entity_by_state(BLE_LL_STATE_ADVERTISING,LL_SM_UNVALID_HANDLE,0);
	ASSERT(POINTER_VALID(LLSM));
	advCtrl = (ll_internal_adv_ctrl_t*)LLSM->entity;
	ASSERT(POINTER_VALID(advCtrl));
	advCtrl->reset = ble_ll_adv_reset;
	//phy init
	LLSM->phy.hw_irq_cb  = adv_phy_irq_callback;

	for(int i=0;i<BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS;i++)
	{
		if(advCtrl->param[i].enable)
		{
			ll_internal_adv_param_t* advParam = &advCtrl->param[i];
			//sm init
			advParam->state = ADV_SM_STATE_IDLE;
			ll_extended_adv_map_out_task(ll,&advCtrl->param[i],system_time()+500,system_time()+500+advCtrl->param[i].la->sch.interval,ADV_SCH_MAP_ALL);
		}
	}
	adv_get_next_event(ll);
	//ll entity sch init
	LLSM->sch.id           = LLSM->id;
	LLSM->sch.type         = SCH_PERIODIC_TASK;
	LLSM->sch.priority     = LL_ADV_PRIORITY;
	LLSM->sch.timestamp    = currentAdvSet->la->sch.anchorPoint;//maybe need planner
	LLSM->sch.period       = currentAdvSet->la->sch.interval;
	LLSM->sch.duration     = currentAdvSet->la->sch.duration;
	LLSM->sch.startLatency = currentAdvSet->la->sch.startMargin;
	LLSM->sch.stopLatency  = currentAdvSet->la->sch.stopMargin;
	LLSM->sch.cb           = adv_sch_callback;
	if(sch_insert_task(&LLSM->sch)==SCH_STATUS_SUCCESS)
	{
		sch_start();
	}
	return 1;
}
