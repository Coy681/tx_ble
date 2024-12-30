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

void tx_add_task(pTaskInit_f init,pTaskProcess_f process,txTaskPriority_e priority)
{

}

void tx_start_task(void)
{

}

void tx_set_task_event(_u16 taskId,_u32 event)
{

}

void tx_clear_task_event(_u16 taskId,_u32 event)
{

}
