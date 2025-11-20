
#include"sch.h"
#include"system/task/task.h"
#include"system/task/event/event.h" 
#include"system/task/message/message.h"
#include"platform/hal/stimer.h"
#include"config.h"
#include"tx_common.h"
#include"common/txCommon.h"
sch_ctrl_t schCtrl;
#define SCH_CANCELED_TASK      1
#define SCH_WAITING_TASK       2
#define SCH_RUNNING_TASK       3
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

 _RAM_CODE static int sch_task1_conflict_with_task2(sch_node_t* task1,sch_node_t* task2)
 {
    ASSERT(task1!=NULL&&task2!=NULL);
    _u32 task1S = TASK_START_TIME(task1);
    _u32 task1E = TASK_STOP_TIME(task1);
    _u32 task2S = TASK_START_TIME(task2);
    _u32 task2E = TASK_STOP_TIME(task2);

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

 _RAM_CODE static int  sch_delete_node_from_list(sch_node_t** list,sch_node_t* node)
{
    ASSERT(TASK_VALID(list)&&TASK_VALID(node));
    sch_node_t* scan = *list;
    sch_node_t* prev = NULL;
    while(TASK_VALID(scan))
    {
        if(scan == node)
        {
            if(prev)
            {
                prev->next = scan->next;
            }
            else
            {
                *list = (*list)->next;
            }
            return 1;
        }
        prev = scan;
        scan = scan->next;
    }
    return 0;
}
 _RAM_CODE static sch_node_t* sch_serach_node_in_list(sch_node_t* list,_u8 id)
{
    sch_node_t* scan = list;
    while(TASK_VALID(scan))
    {
        if(scan->llId == id)
        {
            return scan;
        }
        scan = scan->next;
    }
    return NULL;
}

 _RAM_CODE static void sch_insert_task_to_canceled_list(sch_node_t* task)
{
    ASSERT(TASK_VALID(task));
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
    	task->next = schCtrl.pCanceledList;
        schCtrl.pCanceledList = task;
    }
}

 _RAM_CODE int sch_insert_task(sch_node_t* task)
{
    if(TASK_NOT_VALID(task))
    {
        return SCH_STATUS_TASK_NULL;
    }
    IRQ_DISABLE;
    if(sch_serach_node_in_list(schCtrl.pWaitingList,task->llId)||\
	   sch_serach_node_in_list(schCtrl.pRunningTask,task->llId)||\
	   sch_serach_node_in_list(schCtrl.pCanceledList,task->llId))
    {
        IRQ_RESTORE;
    	return SCH_STATUS_REJECTED;
    }
    sch_node_t* scan = schCtrl.pWaitingList;
    sch_node_t* prev = NULL;
    task->next = NULL;
    int status = SCH_STATUS_SUCCESS;
    if(TASK_NOT_VALID(scan))
    {
        schCtrl.pWaitingList = task;
    }
    else 
    {
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
            if(prev == NULL || cancelFirst == schCtrl.pWaitingList)
            {
                schCtrl.pWaitingList = task;
                task->next = scan;
            }
            else
            {
                prev->next = task;
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
    IRQ_RESTORE;
    return SCH_STATUS_SUCCESS;
}

_RAM_CODE void sch_start(void)
{
    if(TASK_VALID(schCtrl.pRunningTask))
    {
    	if(TASK_VALID(schCtrl.pWaitingList))
    	{
        	int conflict = sch_task1_conflict_with_task2(schCtrl.pRunningTask,schCtrl.pWaitingList);
            if(conflict!=SCH_START_BEFORE_END_BEFORE)
            {
            	if(conflict == SCH_START_AFTER_END_AFTER)
            	{
            		schCtrl.pRunningTask->next = schCtrl.pWaitingList->next;
            		schCtrl.pWaitingList->next = schCtrl.pRunningTask;
            		schCtrl.pRunningTask = NULL;
                	_u32 startTime = TASK_START_TIME(schCtrl.pWaitingList) - TASK_SCH_PROCESS_TIME;
                	hal_stimer_set_capture(startTime);
                	return;
            	}
            	else
            	{
            		if(schCtrl.pWaitingList->next->priority>schCtrl.pRunningTask->next->priority)
            		{
            			hal_stimer_set_capture(system_time()+20);
                    	return;
            		}
            	}
            }
    	}
		_u32 startTime = TASK_START_TIME(schCtrl.pRunningTask) - TASK_SCH_PROCESS_TIME;
		hal_stimer_set_capture(startTime);
    }
    else if(TASK_VALID(schCtrl.pWaitingList))
    {
    	_u32 startTime = TASK_START_TIME(schCtrl.pWaitingList) - TASK_SCH_PROCESS_TIME;
    	_u32 capTure = hal_stimer_get_capture();
    	if(startTime!=capTure)
    	{
        	hal_stimer_set_capture(startTime);
    	}
    }
}

 _RAM_CODE static void sch_process_timeout_list(_u32 currentTime,sch_node_t** list,_u8 taskType)
{
	sch_node_t* scan = *list;
	while(scan!=NULL)
	{
		if(scan->delete)
		{

			sch_node_t* deleteNode = scan;
			scan = scan->next;
			deleteNode->delete = 0;
			sch_delete_node_from_list(list,deleteNode);
			continue;
		}
		_u32 startTime = TASK_START_TIME(scan);
		if(txCompareTime(currentTime,startTime))
		{
			if(taskType == SCH_CANCELED_TASK)
			{
				scan->cb(SCH_TASK_CANCELED);;
			}
			else
			{
				scan->cb(SCH_TASK_PASSED);
			}
			sch_node_t* task = scan;
			scan = scan->next;
			sch_delete_node_from_list(list,task);
			if(task->type == SCH_PERIODIC_TASK)
			{
				sch_insert_task(task);
			}
		}
		else
		{
        	break;
		}
	}
}

 _RAM_CODE static void sch_update_timestamp(_u32 currenTime)
{
    if(TASK_VALID(schCtrl.pWaitingList))
    {
    	sch_process_timeout_list(currenTime,&schCtrl.pWaitingList,SCH_WAITING_TASK);
    }
    if(TASK_VALID(schCtrl.pCanceledList))
    {
    	sch_process_timeout_list(currenTime,&schCtrl.pCanceledList,SCH_CANCELED_TASK);
    }
}

 _RAM_CODE static sch_node_t* sch_extract_first_task(void)
{
    sch_update_timestamp(system_time());
    if(TASK_VALID(schCtrl.pWaitingList))
    {
    	sch_node_t* task = NULL;
    	if(schCtrl.pCanceledList)
    	{
        	if(sch_task1_conflict_with_task2(schCtrl.pCanceledList,schCtrl.pWaitingList) == SCH_START_BEFORE_END_BEFORE)
        	{
        		task = schCtrl.pCanceledList;
        		schCtrl.pCanceledList = schCtrl.pCanceledList->next;
        		return task;
        	}
    	}
    	task = schCtrl.pWaitingList;
    	schCtrl.pWaitingList = schCtrl.pWaitingList->next;
    	return task;
    }
    return NULL;
}

 _RAM_CODE static void sch_program_timer(void)
{
	hal_stimer_clear_irq();
    _u32 time = system_time();
    if(TASK_VALID(schCtrl.pRunningTask))
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
        hal_stimer_set_capture(time+1000000);
    }
}

 _RAM_CODE void sch_irq_process(void)
{
	DEBUG_GPIO_HIGH(GPIO_3);
    if(TASK_VALID(schCtrl.pRunningTask))
    {
        schCtrl.pRunningTask->cb(SCH_TASK_STOP);
        if(schCtrl.pRunningTask->type == SCH_PERIODIC_TASK)
        {
        	sch_node_t* pTask = schCtrl.pRunningTask;
        	schCtrl.pRunningTask = NULL;
            sch_insert_task(pTask);
        }
    }
    schCtrl.pRunningTask = sch_extract_first_task();

    if(TASK_VALID(schCtrl.pRunningTask))
    {
        schCtrl.pRunningTask->cb(SCH_TASK_START);
    }
    sch_program_timer();
	DEBUG_GPIO_LOW(GPIO_3);
}

_RAM_CODE void sch_stop_task_early(void)
{
    hal_stimer_set_capture(system_time()+10);
}

_RAM_CODE void sch_abort_current_and_process_next_task(void)
{
    if(TASK_VALID(schCtrl.pRunningTask))
    {
        if(schCtrl.pRunningTask->type == SCH_PERIODIC_TASK)
        {
        	sch_node_t* pTask = schCtrl.pRunningTask;
        	schCtrl.pRunningTask = NULL;
            sch_insert_task(pTask);
        }
    }
    hal_stimer_set_capture(system_time()+10);
}

_RAM_CODE sch_node_t* sch_get_task_list(_u8 type)
{
    if(type == SCH_WAITING_LIST)
    {
        return schCtrl.pWaitingList;
    }
    else if(type == SCH_RUNNING_LIST)
    {
        return schCtrl.pRunningTask;
    }
    else if(type == SCH_CANCELED_LIST)
    {
        return schCtrl.pCanceledList;
    }
}

_RAM_CODE _u32 sch_get_task_extended_boundary(void)
{
	if(POINTER_VALID(schCtrl.pWaitingList))
	{
		return TASK_START_TIME(schCtrl.pWaitingList);
	}
	else
	{
		return 0xffff;
	}
}

_RAM_CODE int sch_task_extended(_u32 targetTime)
{
	_u32 captureTime = system_time()+200;
	if(txCompareTime(targetTime,captureTime))
	{
		captureTime = targetTime;
	}
	else
	{
		return 0;
	}
	if(POINTER_VALID(schCtrl.pWaitingList))
	{
		_u32 taskBoundary = TASK_START_TIME(schCtrl.pWaitingList);
		if(txCompareTime(captureTime,taskBoundary))
		{
			captureTime = taskBoundary;
		}
		else
		{
			return 0;
		}
	}
	hal_stimer_set_capture(captureTime);
	return 1;
}


 _RAM_CODE int sch_remove_task(_u8 taskId)
{
	IRQ_DISABLE;
    if(TASK_VALID(schCtrl.pWaitingList))
    {
        sch_node_t* task = sch_serach_node_in_list(schCtrl.pWaitingList,taskId);
        if(TASK_VALID(task))
        {
            sch_delete_node_from_list(&schCtrl.pWaitingList,task);
        	IRQ_RESTORE;
            return 1;
        }
    }
    if(TASK_VALID(schCtrl.pCanceledList))
    {
        sch_node_t* task = sch_serach_node_in_list(schCtrl.pCanceledList,taskId);
        if(TASK_VALID(task))
        {
            sch_delete_node_from_list(&schCtrl.pWaitingList,task);
        	IRQ_RESTORE;
            return 1;
        }
    }
    if(TASK_VALID(schCtrl.pRunningTask))
    {
        if(schCtrl.pRunningTask->llId == taskId)
        {
            schCtrl.pRunningTask->cb(SCH_TASK_STOP);
            schCtrl.pRunningTask = NULL;
        	IRQ_RESTORE;
        	hal_stimer_set_capture(system_time()+20);
            return 1;
        }
    }
	IRQ_RESTORE;
    return 0;
}

 _RAM_CODE void sch_stop(void)
{
	if(TASK_VALID(schCtrl.pRunningTask))
	{
		schCtrl.pRunningTask->cb(SCH_TASK_STOP);
	}
	 schCtrl.pWaitingList  = NULL;
	 schCtrl.pRunningTask  = NULL;
	 schCtrl.pCanceledList = NULL;
	 sch_irq_process();
}

void sche_init(void)
{
    hal_stimer_register_task(sch_irq_process);
    schCtrl.pWaitingList = NULL;
    schCtrl.pRunningTask = NULL;
    schCtrl.pCanceledList = NULL;
}
#if(TX_SCHEDULER_ENABLE)
TASK_INIT(sche_init);
#endif




//void task1_callback(_u8 type);
//sch_node_t aTask1=
//{
//	.llId = 0x00,
//	.type = SCH_PERIODIC_TASK,
//    .priority = SCH_TASK_PRIORITY_A,
//	.update = 0,
//	.timestamp = 0,
//    .period = 20000,
//	.duration = 200,
//	.startLatency = 50,
//	.stopLatency = 50,
//	.cb = task1_callback,
//};
//
//_RAM_CODE void task1_callback(_u8 type)
//{
//	if(type == SCH_TASK_START)
//	{
//		aTask1.priority = SCH_TASK_PRIORITY_A;
//    	DEBUG_GPIO_HIGH(GPIO_4);
////		LOG_TRACE(1,"task 1 start",0,0)
//    	DEBUG_GPIO_LOW(GPIO_4);
//	}
//	else if(type == SCH_TASK_STOP)
//	{
////		LOG_TRACE(1,"task 1 stop",0,0)
//	}
//	else if(type == SCH_TASK_CANCELED)
//	{
////    	DEBUG_GPIO_HIGH(GPIO_5);
//		aTask1.priority++;
////		DEBUG_GPIO_LOW(GPIO_5);
////		LOG_TRACE(1,"task 1 canceled",0,0)
//	}
//	else if(type == SCH_TASK_PASSED)
//	{
////    	DEBUG_GPIO_HIGH(GPIO_6);
//		aTask1.priority++;
////		DEBUG_GPIO_LOW(GPIO_6);
////		LOG_TRACE(1,"task 1 passed",0,0)
//	}
//}
//
//void task2_callback(_u8 type);
//
//sch_node_t aTask2=
//{
//	.llId = 0x01,
//	.type = SCH_PERIODIC_TASK,
//    .priority = SCH_TASK_PRIORITY_A,
//	.update = 0,
//	.timestamp = 0,
//    .period = 20000,
//	.duration = 200,
//	.startLatency = 50,
//	.stopLatency = 50,
//	.cb = task2_callback,
//};
//
//_RAM_CODE void task2_callback(_u8 type)
//{
//	if(type == SCH_TASK_START)
//	{
//		aTask2.priority = SCH_TASK_PRIORITY_A;
//    	DEBUG_GPIO_HIGH(GPIO_5);
////		LOG_TRACE(1,"task 2 start",0,0)
//    	DEBUG_GPIO_LOW(GPIO_5);
//	}
//	else if(type == SCH_TASK_STOP)
//	{
////		LOG_TRACE(1,"task 2 stop",0,0)
//	}
//	else if(type == SCH_TASK_CANCELED)
//	{
//		aTask2.priority++;
////		LOG_TRACE(1,"task 2 canceled",0,0)
//	}
//	else if(type == SCH_TASK_PASSED)
//	{
//		aTask2.priority++;
////		LOG_TRACE(1,"task 2 passed",0,0)
//	}
//
//}
//
//
//_RAM_CODE void task3_callback(_u8 type);
//sch_node_t aTask3=
//{
//	.llId = 0x02,
//	.type = SCH_PERIODIC_TASK,
//    .priority = SCH_TASK_PRIORITY_A,
//	.update = 0,
//	.timestamp = 0,
//    .period = 20000,
//	.duration = 200,
//	.startLatency = 50,
//	.stopLatency = 50,
//	.cb = task3_callback,
//};
//
//_RAM_CODE void task3_callback(_u8 type)
//{
//	if(type == SCH_TASK_START)
//	{
//		aTask3.priority = SCH_TASK_PRIORITY_A;
//    	DEBUG_GPIO_HIGH(GPIO_6);
////		LOG_TRACE(1,"task 3 start",0,0)
//    	DEBUG_GPIO_LOW(GPIO_6);
//	}
//	else if(type == SCH_TASK_STOP)
//	{
////		LOG_TRACE(1,"task 3 stop",0,0)
//	}
//	else if(type == SCH_TASK_CANCELED)
//	{
//		aTask3.priority++;
////		LOG_TRACE(1,"task 3 canceled",0,0)
//	}
//	else if(type == SCH_TASK_PASSED)
//	{
//		aTask3.priority++;
////		LOG_TRACE(1,"task 3 passed",0,0)
//	}
//
//}
//
//_u8* message = tx_message_allocate(8);
//message[0] = SCHE_MESSAGE_TASK_ADD;
//message[1] = ((_u32)&aTask1);
//message[2] = ((_u32)&aTask1)>>8;
//message[3] = ((_u32)&aTask1)>>16;
//message[4] = ((_u32)&aTask1)>>24;
//tx_message_send(TX_TASK_ID_SCH,message);
