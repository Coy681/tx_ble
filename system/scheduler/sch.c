
#include"sch.h"
#include"system/task/task.h"
#include"system/task/event/event.h" 
#include"system/task/message/message.h"
#include"platform/hal/stimer.h"
#include"config.h"
#include"tx_common.h"

sch_ctrl_t schCtrl;

/** Case A:new task happens before current
 *                            --------------------------------
 *                            |            Current           |
 *                            --------------------------------
 * —————————————————————————————————————————————————————————————————————→clock
 * ---------------------
 * |        NEW        |
 * ---------------------
 */

/** Case B:new task starts before and finishes during current
 *                            --------------------------------
 *                            |            Current           |
 *                            --------------------------------
 * —————————————————————————————————————————————————————————————————————→clock
 *               ---------------------
 *               |        NEW        |
 *               ---------------------
 */

/** Case C:new task starts before and finishes after current
 *                            --------------------------------
 *                            |            Current           |
 *                            --------------------------------
 * —————————————————————————————————————————————————————————————————————→clock
 *               ---------------------------------------------------
 *               |                      NEW                        |
 *               ---------------------------------------------------
 */

/** Case D:new task starts after and finishes before current
 *                            --------------------------------
 *                            |            Current           |
 *                            --------------------------------
 * —————————————————————————————————————————————————————————————————————→clock
 *                                 -----------------
 *                                 |      NEW      |
 *                                 -----------------
 */

 /** Case E:new task starts during and finishes after current
 *                            --------------------------------
 *                            |            Current           |
 *                            --------------------------------
 * —————————————————————————————————————————————————————————————————————→clock
 *                                     -------------------------------
 *                                     |             NEW              |
 *                                     -------------------------------
 */

  /** Case F:new task starts and finishes after current
 *                            --------------------------------
 *                            |            Current           |
 *                            --------------------------------
 * —————————————————————————————————————————————————————————————————————————————————————————→clock
 *                                                                   ---------------------
 *                                                                   |         NEW       |
 *                                                                   ---------------------
 */
 enum
 {
    SCH_START_BEFORE_END_BEFORE = 0x01, //Case A
    SCH_START_BEFORE_END_DURING = 0x02, //Case B
    SCH_START_BEFORE_END_AFTER  = 0x03,  //Case C
    SCH_START_DURING_END_DURING = 0x04, //Case D
    SCH_START_DURING_END_AFTER  = 0x05,  //Case E
    SCH_START_AFTER_END_AFTER   = 0x06   //Case F
 };

 static int sch_task1_conflict_with_task2(sch_node_t* task1,sch_node_t* task2)
 {
    ASSERT(task1!=NULL&&task2!=NULL);
    _u32 task1S = task1->timestamp - task1->startLatency;
    _u32 task1E = task1->timestamp + task1->duration + task1->stopLatency;
    _u32 task2S = task2->timestamp - task2->startLatency;
    _u32 task2E = task2->timestamp + task2->duration + task2->stopLatency;
 
    if (txCompareTime(task2S,task1S))
    {
        if(txCompareTime(task2S,task1E))
        {
            return SCH_START_BEFORE_END_BEFORE; //case A,task1E<task2S
        }
        else 
        {
            if(txCompareTime(task2E,task1E))
            {
                return SCH_START_BEFORE_END_DURING;//case B,task1S<task2S<task1E<task2E,
            }
            else
            {
                return SCH_START_BEFORE_END_AFTER;//case C,task1S<task2E<task1E,
            }
        }
    }
    else
    {
        if(txCompareTime(task2E,task1E)) 
        {
            return SCH_START_DURING_END_DURING;//case D,task2S<task1S<task1E<task2E
        }
        else 
        {
            if(txCompareTime(task2E,task1S))
            {
                return SCH_START_DURING_END_AFTER;//case E,task2S<task1S<task2E<task1E
            }
            else
            {
                return SCH_START_AFTER_END_AFTER;//case F,task2E<task1S
            }
        }
    }
 }

static void sch_insert_task_to_canceled_list(sch_node_t* task)
{
    ASSERT(IS_TASK_VALID(task));
    task->next = NULL;
    sch_node_t* prev = NULL;
    sch_node_t* scan = schCtrl.pCanceledList;
    _u32 taskTime = task->timestamp-task->startLatency;
    while(scan!=NULL)
    {
        _u32 startTime = scan->timestamp-scan->startLatency;
        if(txCompareTime(taskTime,startTime))
        {
            break;
        }
        prev = scan;
        scan = scan->next;
    }
    if(prev)
    {
        prev->next = task;
        task->next = scan;
    }
    else
    {
        schCtrl.pCanceledList = task;
    }
}

static int sch_insert_task(sch_node_t* task)
{
    if(TASK_NOT_VALID(task))
    {
        return SCH_STATUS_TASK_NULL;
    }
    sch_node_t* scan = schCtrl.pWaitingList;
    sch_node_t* prev = NULL;
    task->next = NULL;
    if(TASK_NOT_VALID(scan))
    {
        schCtrl.pWaitingList = task;
    }
    else
    {
        int status = SCH_STATUS_SUCCESS;
        sch_node_t* cancelFirst = NULL;
        sch_node_t* cancelLast  = NULL;
        while(scan!=NULL)
        {
            int conflict = sch_task1_conflict_with_task2(task,scan);
            if(conflict == SCH_START_BEFORE_END_BEFORE)
            {
                break;
            }
            else if(conflict == SCH_START_AFTER_END_AFTER)
            {
                //continue to search
            }
            else
            {//should process asap task.
                if(scan->priority>task->priority)
                {
                    status = SCH_STATUS_REJECTED;
                    break;
                }
                else
                {
                    if(TASK_NOT_VALID(cancelFirst))
                    {
                        cancelFirst = scan;
                    }
                    cancelLast = scan;
                }
            }
            prev = scan;
            scan = scan->next;
        }
        if(status == SCH_STATUS_SUCCESS)
        {
            if(prev)
            {
                prev->next = task;
                task->next = scan;
            }
            else
            {
                schCtrl.pWaitingList = task;
                task->next = scan;
            }
            if(cancelFirst)
            {
                sch_insert_task_to_canceled_list(cancelFirst);
                while(cancelFirst!=cancelLast)
                {
                    cancelFirst = cancelFirst->next;
                    if(cancelFirst)
                    {
                        sch_insert_task_to_canceled_list(cancelFirst);
                    }
                }
            }
        }
        else
        {
            sch_insert_task_to_canceled_list(task);
        }
    }
    return SCH_STATUS_SUCCESS;
}
static sch_node_t* sch_extract_first_task(void)
{
    if(IS_TASK_VALID(schCtrl.pWaitingList))
    {
    	sch_node_t* task = schCtrl.pWaitingList;
    	schCtrl.pWaitingList = schCtrl.pWaitingList->next;
    	return task;
    }
    return NULL;
}
static void sch_program_timer(void)
{
	hal_stimer_clear_irq();
    _u32 time = system_time();
    if(IS_TASK_VALID(schCtrl.pRunningTask))
    {
        _u32 capture = schCtrl.pRunningTask->timestamp + schCtrl.pRunningTask->duration;
        if(txCompareTime(capture,time))
        {
            hal_stimer_set_capture(capture);
        }
        else
        {
            hal_stimer_set_capture(time+10);
        }
    }
    else
    {
        hal_stimer_set_capture(time+10000);
    }
}
static int sch_task_start_time_enough(sch_node_t* task)
{

    _u32 time = system_time();
    _u32 target = task->timestamp - task->startLatency;
    if(txCompareTime(target,time))
    {
        return 1;
    }
    return 0;
}

static void sch_schedule_next_task(void)
{
    if(IS_TASK_VALID(schCtrl.pRunningTask))
    {
        schCtrl.pRunningTask->cb(SCH_TASK_STOP);
        if(schCtrl.pRunningTask->type == SCH_PERIODIC_TASK)
        {
            sch_insert_task(schCtrl.pRunningTask);
        }
    }
    schCtrl.pRunningTask = sch_extract_first_task();
    if(IS_TASK_VALID(schCtrl.pRunningTask))
    {
        if(sch_task_start_time_enough(schCtrl.pRunningTask))
        {
            schCtrl.pRunningTask->cb(SCH_TASK_START);
        }
        else
        {
            schCtrl.pRunningTask->cb(SCH_TASK_PASSED);
            if(schCtrl.pRunningTask->type == SCH_PERIODIC_TASK)
            {
                sch_insert_task(schCtrl.pRunningTask);
            }
        }
    }
    sch_program_timer();
}
static void sch_remove_task(_u8 taskId)
{
    //todo
}
static void sche_task_init(void)
{
    schCtrl.pWaitingList = NULL;
    schCtrl.pRunningTask = NULL;
    schCtrl.pCanceledList = NULL;
}

_u32 sche_event_process(_u16 taskId,_u32 event)
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
                    sch_insert_task((sch_node_t*)address);
                }
                    break;
                case SCHE_MESSAGE_TASK_REMOVE:
                {
                	LOG_TRACE(TX_SCHE_LOG_ENABLE,"remove task",0,0);
                    sch_remove_task(pMessage->message[0]);
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

void sche_init(void)
{
    _u32 ret = tx_task_add(sche_task_init,sche_event_process,TX_TASK_ID_SCH,TX_TASK_PRIORITY_15);
    hal_stimer_register_task(sch_schedule_next_task);
}
#if(TX_SCHEDULER_ENABLE)
TASK_INIT(sche_init);
#endif
