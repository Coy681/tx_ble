#include"adv.h"


static void adv_phy_irq_callback(_u8 type)
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

static void adv_sch_start(void)
{

}
static void adv_sch_stop(void)
{
    
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
    if(type <= SCH_TASK_PASSED)
    {
        adv_sch_process[type]();
    }
}


sch_node_t aTask2=
{
	.llId = 0x01,
	.type = SCH_PERIODIC_TASK,
   .priority = SCH_TASK_PRIORITY_A,
	.update = 0,
	.timestamp = 0,
   .period = 20000,
	.duration = 200,
	.startLatency = 50,
	.stopLatency = 50,
	.cb = adv_sch_callback,
};

int ble_ll_enter_advertising_state(ble_ll_event_e event)
{
    if(event == BLE_LL_EVENT_START_ADVERTISING)
    {
        ll_ctrl_t* ll = ll_get_current_state_machine();
        if(ll->adv == NULL)
        {   
            return 0;
        }
        ll->sch.cb = adv_sch_callback;
        ll->sch.timestamp = system_time() + 50;//maybe need planner
        ll->sch.llId = ll->id;
        ll->sch.type = SCH_PERIODIC_TASK;
        ll->sch.priority = LL_ADV_PRIORITY;
        ll->sch.duration = 500;

        LOG_TRACE(LL_LOG_TRACE,"enter advertising state",0,0);
        return 1;
    }

}
