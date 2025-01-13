/*
 * event.c
 *
 *  Created on: 2024年12月30日
 *      Author: 12407
 */

#include"event.h"

txTask_t* pHeader = NULL;

void tx_task_init(void)
{
	txTask_t* pActive = pHeader;
   if(pActive)
   {
   	if(pActive->init)
   	{
   		pActive->init();
   	}
   	pActive = pActive->next;
   }
}

txTask_e tx_task_add(pTaskInit_f init,pTaskProcess_f process,txTaskPriority_e priority)
{
	txTask_t* pNew = (txTask_t*)tx_malloc(sizeof(txTask_t));
	if(pNew)
	{
		pNew->init         = init;
		pNew->process      = process;
		pNew->taskPriority = priority;
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

void tx_task_start(void)
{
   txTask_t* pTask = NULL;
	while(1)
	{
		pTask = tx_task_next_active();
		if(pTask)
		{
			_u16 event = pTask->eventMask;
			pTask->eventMask = 0;
			if(pTask->process)
			{
				_u16 retEvent = pTask->process(pTask->taskId,event);
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

txTask_e tx_task_set_event(_u16 taskId,_u32 event)
{
   txTask_t* pTarget = tx_task_search(taskId);
	if(pTarget)
	{
		pTarget->eventMask |= event;
		return TX_TASK_SUCCESS;
	}
	return TX_TASK_NO_TASK;
}

txTask_e tx_task_clear_event(_u16 taskId,_u32 event)
{
   txTask_t* pTarget = tx_task_search(taskId);
	if(pTarget)
	{
		pTarget->eventMask &= (~event);
		return TX_TASK_SUCCESS;
	}
	return TX_TASK_NO_TASK;
}
