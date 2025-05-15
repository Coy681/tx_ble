
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
    SCH_START_DURING_END_DURING = 0x03, //Case D
    SCH_START_DURING_END_AFTER  = 0x04,  //Case E
    SCH_START_AFTER_END_AFTER   = 0x05   //Case F
 };

 static int sch_task1_conflict_with_task2(sch_node_t* task1,sch_node_t* task2)
 {
    ASSERT(task1==NULL||task2==NULL);
    _u32 task1S = task1->timestamp - task1->startLatency;
    _u32 task1E = task1->timestamp + task1->duration + task1->stopLatency;
    _u32 task2S = task2->timestamp - task2->startLatency;
    _u32 task2E = task2->timestamp + task2->duration + task2->stopLatency;
 
    if (tick1_exceed_tick2(task2S,task1S))
    {
        if(tick1_exceed_tick2(task2S,task1E))
        {
            return SCH_START_BEFORE_END_BEFORE; //case A,task1E<task2S
        }
        else 
        {
            if(tick1_exceed_tick2(task2E,task1E))
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
        if(tick1_exceed_tick2(task2E,task1E)) 
        {
            return SCH_START_DURING_END_DURING;//case D,task2S<task1S<task1E<task2E
        }
        else 
        {
            if(tick1_exceed_tick2(task2E,task1S))
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



static void sch_schedule_task(sch_node_t* task)
{

}
static sch_node_t* sch_extract_task(void)
{

}

static void sch_timer_task(void)
{
    if(schCtrl.pRunningTask!=NULL)
    {
        schCtrl.pRunningTask->cb(SCH_TASK_STOP);
        if(schCtrl.pRunningTask->type == SCH_PERIODIC_TASK)
        {
            sch_schedule_task(schCtrl.pRunningTask);
        }
    }
    schCtrl.pRunningTask = sch_extract_task();
    if(schCtrl.pRunningTask!=NULL)
    {
        schCtrl.pRunningTask->cb(SCH_TASK_START);
    }
}

static void sch_insert_task(sch_node_t* task)
{

}
static void sch_remove_task(_u8 taskId)
{

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
    hal_stimer_register_task(sch_timer_task);
}
#if(TX_SCHEDULER_ENABLE)
TASK_INIT(sche_init);
#endif