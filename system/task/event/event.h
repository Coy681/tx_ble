/*
 * event.h
 *
 *  Created on: 2024年12月30日
 *      Author: 12407
 */

#ifndef TASK_EVENT_EVENT_H_
#define TASK_EVENT_EVENT_H_

#include"task.h"

typedef void(*pTaskInit_f)(void);
typedef void(*pTaskProcess_f)(_32,_u32);
typedef struct txEvent txEvent_t;

typedef struct txEvent
{
	txEvent_t*     next;
	pTaskInit_f    init;
	pTaskProcess_f process;
    _u16           taskId;
    _u16           taskPriority;
    _u32           eventMask;
}txTask_t;

void tx_task_init(void);
void tx_add_task(pTaskInit_f init,pTaskProcess_f process,txTaskPriority_e priority);
void tx_start_task(void);
void tx_set_task_event(_u16 taskId,_u32 event);
void tx_clear_task_event(_u16 taskId,_u32 event);

#endif /* TASK_EVENT_EVENT_H_ */
