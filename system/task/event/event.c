/*
 * event.c
 *
 *  Created on: 2024年12月30日
 *      Author: 12407
 */

#include"event.h"
#include"platform/debug/debug_gpio.h"
txTask_t* pHeader = NULL;
void tx_task_init(void)
{
    txTask_t* pActive = pHeader;
    while(pActive)
    {
        if(pActive->init)
   	    {
   		    pActive->init();
   	    }
   	    pActive = pActive->next;
    }
}
OS_INIT(tx_task_init);

txTask_e tx_task_add(pTaskInit_f init,pTaskProcess_f process,txTaskId_e taskId,txTaskPriority_e priority)
{
	txTask_t* pNew = (txTask_t*)tx_malloc(sizeof(txTask_t));
	if(pNew)
	{
		pNew->init         = init;
		pNew->process      = process;
		pNew->taskPriority = priority;
		pNew->taskId       = taskId;
		pNew->eventMask    = 0;
		pNew->next         = NULL;
		if(pHeader)
		{
			txTask_t* pSearch = pHeader;
			txTask_t* pPrev   = NULL;
            while((pNew->taskPriority>=pSearch->taskPriority)&&pSearch)
            {
				pPrev = pHeader;
           	    pSearch = pSearch->next;
            }
			pNew->next = pSearch;
			if(pPrev)
			{
				pPrev->next = pNew;
			}
		}
		else
		{
			pHeader = pNew;
		}
		return TX_TASK_SUCCESS;
	}
	else
	{
		return TX_TASK_MEMORY_INVALID;
	}
}

txTask_t* tx_task_next_active(void)
{
	txTask_t *pActive = pHeader;
    while(pActive)
	{
		if(pActive->eventMask)
		{
			return pActive;
		}
		pActive = pActive->next;
	}
	return NULL;
}

_u32 AAA_TEST_MASK = 0;
_u32 AAA_TEST_MASK1 = 0;
void tx_task_start(void)
{
   txTask_t* pTask = NULL;
	while(1)
	{
		pTask = tx_task_next_active();
		if(pTask)
		{
			_u32 event = pTask->eventMask;
			AAA_TEST_MASK = event;
			pTask->eventMask = 0;
			if(pTask->process)
			{
				DEBUG_GPIO_HIGH(GPIO_0);
				DEBUG_GPIO_LOW(GPIO_0);
				_u32 retEvent = pTask->process(pTask->taskId,event);
				AAA_TEST_MASK1 = retEvent;
               pTask->eventMask = retEvent;
			}
		}
	}
}

txTask_t* tx_task_search(_u16 taskId)
{
   txTask_t* pSearch = pHeader;
	while(pSearch)
	{
       if(pSearch->taskId == taskId)
		{
			return pSearch;
		}
		pSearch = pSearch->next;
	}
	return NULL;
}

_RAM_CODE txTask_e tx_task_set_event(_u16 taskId,_u32 event)
{
   txTask_t* pTarget = tx_task_search(taskId);
	if(pTarget)
	{
		pTarget->eventMask |= event;
		return TX_TASK_SUCCESS;
	}
	return TX_TASK_NO_TASK;
}

_RAM_CODE txTask_e tx_task_clear_event(_u16 taskId,_u32 event)
{
   txTask_t* pTarget = tx_task_search(taskId);
	if(pTarget)
	{
		pTarget->eventMask &= (~event);
		return TX_TASK_SUCCESS;
	}
	return TX_TASK_NO_TASK;
}
