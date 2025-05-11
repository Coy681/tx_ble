#include"tsched.h"
#include"system/task/task.h"
#include"system/task/event/event.h" 
#include"system/task/message/message.h"
#include"platform/hal/stimer.h"
#include"config.h"
#include"tx_common.h"
sch_ctrl_t schCtrl;
// sche_isr

typedef enum
{
    SCH_TASK_TIMESTAMP_UPDATE,
    SCH_TASK_SCHEDULING,
}sch_task_step_e;
typedef void(*sch_task_process_f)(_u8,sch_node_t*);

//todo,merge three fucntion to one.
static void sch_remove_task_from_task_list(sch_node_t** list,sch_node_t* pTask)
{
    if(pTask == NULL || *list == NULL)
    {
        return;
    }
    sch_node_t* pTravese = *list;
    sch_node_t* pPrev = NULL;
    while(pTravese!=NULL)
    {
        if(pTravese == pTask)
        {
            if(pPrev == NULL)
            {
                *list = pTravese->tNext;
            }
            else
            {
                pPrev->tNext = pTravese->tNext;
            }
            pTask->tNext = NULL;
            return;
        }
        else
        {
            pPrev = pTravese;
            pTravese = pTravese->tNext;
        }
    }
}

static void sch_remove_task_from_running_list(sch_node_t** list,sch_node_t* pTask)
{
    if(pTask == NULL || *list == NULL)
    {
        return;
    }
    sch_node_t* pTravese = *list;
    sch_node_t* pPrev = NULL;
    while(pTravese!=NULL)
    {
        if(pTravese == pTask)
        {
            if(pPrev == NULL)
            {
                *list = pTravese->rNext;
            }
            else
            {
                pPrev->rNext = pTravese->rNext;
            }
            pTask->rNext = NULL;
            return;
        }
        else
        {
            pPrev = pTravese;
            pTravese = pTravese->rNext;
        }
    }
}

static void sch_remove_task_from_canceled_list(sch_node_t** list,sch_node_t* pTask)
{
    if(pTask == NULL || *list == NULL)
    {
        return;
    }
    sch_node_t* pTravese = *list;
    sch_node_t* pPrev = NULL;
    while(pTravese!=NULL)
    {
        if(pTravese == pTask)
        {
            if(pPrev == NULL)
            {
                *list = pTravese->cNext;
            }
            else
            {
                pPrev->cNext = pTravese->cNext;
            }
            pTask->cNext = NULL;
            return;
        }
        else
        {
            pPrev = pTravese;
            pTravese = pTravese->cNext;
        }
    }
}

static int sch_task1_conflict_with_task2(_u32 tick1S,_u32 tick1E,_u32 tick2S,_u32 tick2E)
{
    if (tick1_exceed_tick2(tick2S,tick1E) || tick1_exceed_tick2(tick1S,tick2E))
    {
        return 0; //no conflict
    }
    return 1;//conflict
}

static int sch_insert_task_to_running_list(sch_node_t* pTask)
{
    sch_node_t* pL = NULL;
    sch_node_t* pR = NULL;
    pL = pR = schCtrl.pRunningList;
    _u32 taskS = pTask->timestamp - pTask->startLatency;
    _u32 taskE = pTask->timestamp + pTask->stopLatency;
    // left
    while (pL != NULL && tick1_exceed_tick2(taskS, pL->timestamp + pL->stopLatency))
    {
        pL = pL->rNext;
    }

    // right
    while (pR != NULL && tick1_exceed_tick2(pR->timestamp - pR->startLatency, taskE))
    {
        pR = pR->rNext;
    }

    sch_node_t* pTravese = pL;
    _u8 arbitrateFailFlag = 0;
    while(pTravese!=pR)
    {
        if(pTravese->priority>pTask->priority)
        {
            arbitrateFailFlag = 1;
            break;
        }
        pTravese = pTravese->rNext;
    }
    if(arbitrateFailFlag)
    {
        return 1;
    }
    else
    {
        sch_node_t* pTravese = pL;
        while(pTravese!=pR)
        {
            if(pTravese->cb)
            {
                pTravese->cb((_u8)SCH_TASK_CANCELED);
            }
            pTravese = pTravese->rNext;
        }
        if(pL == NULL || pL == schCtrl.pRunningList)
        {
            schCtrl.pRunningList = pTask;
        }
        else
        {
            pL->rNext = (struct sch_node_t*)pTask;
        }
        pTask->rNext = (struct sch_node_t*)pR;
        return 0;
    }
}

static void sch_insert_task_to_canceled_list(sch_node_t* pTask)
{
    if (pTask == NULL)
    {
        return; 
    }
    pTask->cNext = NULL; 
    if (schCtrl.pCanceledList == NULL)
    {
        schCtrl.pCanceledList = pTask;
        return;
    }

    sch_node_t* pTraverse = schCtrl.pCanceledList;
    sch_node_t* pPrev = NULL;

    _u32 taskEndTime = pTask->timestamp + pTask->stopLatency;

    while (pTraverse != NULL)
    {
        _u32 traverseEndTime = pTraverse->timestamp + pTraverse->stopLatency;

        if (taskEndTime < traverseEndTime)
        {
            break;
        }
        pPrev = pTraverse;
        pTraverse = pTraverse->cNext;
    }
    if (pPrev == NULL)
    {
        pTask->cNext = (struct sch_node_t*)schCtrl.pCanceledList;
        schCtrl.pCanceledList = pTask;
    }
    else
    {
        pTask->cNext = (struct sch_node_t*)pTraverse;
        pPrev->cNext = (struct sch_node_t*)pTask;
    }
}
static void sch_fixed_task_process(_u8 step,sch_node_t* pTask)
{
    switch(step)
    {
        case SCH_TASK_TIMESTAMP_UPDATE:
        {
            _u32 clockTick = system_clock();
            _u32 targetTick  = (pTask->timestamp-pTask->startLatency)*SYSTEM_TIME_US;
            while(tick1_exceed_tick2(clockTick,targetTick))
            {
                DEBUG_GPIO_HIGH(GPIO_6);
                pTask->timestamp+=pTask->period;
                targetTick+=(pTask->period*SYSTEM_TIME_US);
                if(pTask->cb)
                {
                    pTask->cb((_u8)SCH_TASK_PASSED);
                }
                pTask->update = 1;
                DEBUG_GPIO_LOW(GPIO_6);
            }
            if(pTask->update)
            {
                sch_remove_task_from_task_list(&schCtrl.pRunningList,pTask);
            }
        }
        break;
        case SCH_TASK_SCHEDULING:
        {
            while(sch_insert_task_to_running_list(pTask))
            {
                if(pTask->cb)
                {
                    pTask->cb(SCH_TASK_CANCELED);
                }
                else
                {
                    pTask->priority++;
                }
                pTask->timestamp += pTask->period;
            }
        }
        break;
    }
}
static void sch_dynamic_task_process(_u8 step,sch_node_t* pTask)
{
    switch(step)
    {
        case SCH_TASK_TIMESTAMP_UPDATE:
        {
            _u32 clockTick = system_clock();
            _u32 targetTick  = (pTask->timestamp-pTask->startLatency)*SYSTEM_TIME_US;
            while(tick1_exceed_tick2(clockTick,targetTick))
            {
                pTask->timestamp+=pTask->period;
                targetTick+=(pTask->period*SYSTEM_TIME_US);
                if(pTask->cb)
                {
                    pTask->cb((_u8)SCH_TASK_PASSED);
                }
                pTask->update = 1;
            }
            if(pTask->update)
            {
                sch_remove_task_from_task_list(&schCtrl.pRunningList,pTask);
            }
        }
        break;
        case SCH_TASK_SCHEDULING:
        {
            while(sch_insert_task_to_running_list(pTask))
            {
                if(pTask->cb)
                {
                    pTask->cb(SCH_TASK_CANCELED);
                }
                else
                {
                    pTask->priority++;
                }
                pTask->timestamp += pTask->period;
            }
        }
        break;
    }
}
static void sch_sporadic_task_process(_u8 step,sch_node_t* pTask)
{
    switch(step)
    {
        case SCH_TASK_TIMESTAMP_UPDATE:
        {
            _u32 clockTick = system_clock();
            _u32 targetTick  = (pTask->timestamp-pTask->startLatency)*SYSTEM_TIME_US;
            if(tick1_exceed_tick2(clockTick,targetTick))
            {
                if(pTask->cb)
                {
                    pTask->cb((_u8)SCH_TASK_PASSED);
                }
                sch_remove_task_from_task_list(&schCtrl.pRunningList,pTask);
                sch_remove_task_from_task_list(&schCtrl.pTaskList,pTask);
            }
        }
        break;
        case SCH_TASK_SCHEDULING:
        {
            if(sch_insert_task_to_running_list(pTask))
            {
                if(pTask->cb)
                {
                    pTask->cb(SCH_TASK_CANCELED);
                }
            }
        }
        break;
    }
}
static void sch_insert_task_process(_u8 step,sch_node_t* pTask)
{
    switch(step)
    {
        case SCH_TASK_TIMESTAMP_UPDATE:
        {

        }
        break;
        case SCH_TASK_SCHEDULING:
        {

        }
        break;
    }
}
sch_task_process_f sch_task_process[SCH_TASK_MAX] = 
{
    sch_fixed_task_process,
    sch_dynamic_task_process,
    sch_sporadic_task_process,
    sch_insert_task_process,
};



static void sch_timer_task(void)
{

	DEBUG_GPIO_HIGH(GPIO_3);
//    _u8 static index = 0;
//	while(index==1);
//	index = 1;

    //process stop
    sch_node_t* pStopTask = schCtrl.pRunningList;
    if((pStopTask!=NULL)&&(pStopTask->cb!=NULL))
    {
    	DEBUG_GPIO_HIGH(GPIO_4);
        pStopTask->cb((_u8)SCH_TASK_STOP);
        pStopTask->timestamp+=pStopTask->period;
        sch_remove_task_from_task_list(&schCtrl.pRunningList,pStopTask);
        pStopTask->update = 1;
    	DEBUG_GPIO_LOW(GPIO_4);
    }

    //process timestamp
    _u32 clockTick  = system_clock();
    sch_node_t* pTraverse  = schCtrl.pTaskList;
    while(pTraverse!=NULL)
    {
    	DEBUG_GPIO_HIGH(GPIO_5);
        sch_task_process[pTraverse->type]((_u8)SCH_TASK_TIMESTAMP_UPDATE,pTraverse);
        DEBUG_GPIO_LOW(GPIO_5);
        if(pTraverse->update)
        {
            sch_task_process[pTraverse->type]((_u8)SCH_TASK_SCHEDULING,pTraverse);
        }
        pTraverse = pTraverse->tNext
    }
    //process start
    sch_node_t* pStartTask = schCtrl.pRunningList;
    if((pStartTask!=NULL)&&(pStartTask->cb!=NULL))
    {
    	DEBUG_GPIO_HIGH(GPIO_7);
        pStartTask->cb((_u8)SCH_TASK_START);
        DEBUG_GPIO_LOW(GPIO_7);
    }

    _u32 targetTick = (pStartTask->timestamp-pStartTask->startLatency)*SYSTEM_TIME_US;
	_u32 clock = system_clock();
//	LOG_TRACE(1,"targetTick",&targetTick,4)
//	LOG_TRACE(1,"clock",&clock,4)
	hal_stimer_set_capture(targetTick);
	DEBUG_GPIO_LOW(GPIO_3);
}

volatile _u32 AAA_ADDRESS = 0;
// sche_background_
void sche_background_insert_task(sch_node_t *pTask)
{
    if(pTask == NULL)
    {
        return;
    }
    LOG_TRACE(TX_SCHE_LOG_ENABLE,"background task insert-address",&pTask,4);
    sch_node_t* pTraverse = NULL;
    if(schCtrl.pTaskList == NULL)
    {
        pTask->tNext= NULL;
        schCtrl.pTaskList = pTask;
//        _u32 targetTick = (pTask->timestamp - pTask->startLatency)*SYSTEM_TIME_US;
		_u32 targetTick = (pTask->timestamp)*SYSTEM_TIME_US;
        _u32 clockTick  = system_clock();
        if(tick1_exceed_tick2(clockTick,targetTick))
        {
            if(pTask->period!=0)
            {
                while(tick1_exceed_tick2(clockTick,targetTick))
                {
                    DEBUG_GPIO_HIGH(GPIO_6);
                    pTask->timestamp+= pTask->period;
                    targetTick += (pTask->period)*SYSTEM_TIME_US;
                    DEBUG_GPIO_LOW(GPIO_6);
                }
            }
            else
            {

                targetTick = system_clock() + (10+pTask->startLatency)*SYSTEM_TIME_US;
                pTask->timestamp = (targetTick/SYSTEM_TIME_US);
            }
        }
		LOG_TRACE(1,"timestamp1",&pTask->timestamp,4)
		LOG_TRACE(1,"clockTick",&clockTick,4)
        LOG_TRACE(TX_SCHE_LOG_ENABLE,"first task,stime start",&targetTick,4);
        hal_stimer_set_capture(targetTick);
    }
    else
    {
        pTraverse = schCtrl.pTaskList;
        if(pTraverse->llId == pTask->llId)
        {
            LOG_TRACE(TX_SCHE_LOG_ENABLE,"task repeat,return",&pTask->llId,1);
        	return;//task repeat
        }
        while(pTraverse->tNext!=NULL)
        {
            if(pTraverse->llId == pTask->llId)
            {
                LOG_TRACE(TX_SCHE_LOG_ENABLE,"task repeat,return",&pTask->llId,1);
            	return;//task repeat
            }
            pTraverse = pTraverse->tNext;
        }
        pTask->tNext = NULL;
        pTraverse->tNext = pTask;
        pTask->update = 1;
    }
}


void sche_background_remove_task(_u8 llId)
{
    sch_node_t* pTraverse = NULL;
    sch_node_t* pPrev = NULL;
    if(schCtrl.pTaskList == NULL)
    {
        return;
    }
    pTraverse = schCtrl.pTaskList;
    while(pTraverse!=NULL)
    {
        if(pTraverse->llId == llId)
        {
            if(pPrev == NULL)
            {
                schCtrl.pTaskList = pTraverse->next;
            }
            else
            {
                pPrev->next = pTraverse->next;
            }
            LOG_TRACE(TX_SCHE_LOG_ENABLE,"remove task",&pTraverse,4);
            break;
        }
        pPrev = pTraverse;  
        pTraverse = pTraverse->next;
    }
}

_u32 sche_background_event_process(_u16 taskId,_u32 event)
{
    if(event & TX_TASK_EVENT_MESSAGE)
    {
        sch_message_t* pMessage = (sch_message_t*)tx_message_receive(taskId);
        while(pMessage)
        {
            switch(pMessage->eventType)
            {
                case SCHE_MESSAGE_TASK_ADD:
                {
                    _u32 address = 0;
                    BYTE_TO_U32(address,pMessage->message);
                	LOG_TRACE(TX_SCHE_LOG_ENABLE,"schedule add task",&address,4);
                    sche_background_insert_task((sch_node_t*)address);
                }
                    break;
                case SCHE_MESSAGE_TASK_REMOVE:
                {
                    sche_background_remove_task(pMessage->message[0]);
                	LOG_TRACE(TX_SCHE_LOG_ENABLE,"remove task",0,0);
                }
                    break;
                default:
                    break;
            }
            tx_message_deallocate((_u8*)pMessage);
            pMessage = (sch_message_t*)tx_message_receive(taskId);
        }
        return event^TX_TASK_EVENT_MESSAGE;
    }
    return 0;
}
//sche init

static void sche_task_init(void)
{
    schCtrl.pTaskList = NULL;
    schCtrl.pRunningList = NULL;
    schCtrl.pCanceledList = NULL;
}

void sche_init(void)
{
    _u32 ret = tx_task_add(sche_task_init,sche_background_event_process,TX_TASK_ID_SCH,TX_TASK_PRIORITY_13);
    hal_stimer_register_task(sch_timer_task);
}
#if(TX_SCHEDULER_ENABLE)
TASK_INIT(sche_init);
#endif
