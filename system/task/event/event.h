/*
 * event.h
 *
 *  Created on: 2024年12月30日
 *      Author: 12407
 */

#ifndef TASK_EVENT_EVENT_H_
#define TASK_EVENT_EVENT_H_

#include"../task.h"

typedef void(*pTaskInit_f)(void);
typedef _u32(*pTaskProcess_f)(_u16,_u32);
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

typedef enum
{
   TX_TASK_SUCCESS        = 0,
   TX_TASK_NO_TASK        = 1,
   TX_TASK_MEMORY_INVALID = 2,
}txTask_e;

void      tx_task_init(void);
txTask_e  tx_task_add(pTaskInit_f init,pTaskProcess_f process,txTaskId_e taskId,txTaskPriority_e priority);
void      tx_task_start(void);
txTask_t* tx_task_next_active(void);
txTask_e  tx_task_set_event(_u16 taskId,_u32 event);
txTask_e  tx_task_clear_event(_u16 taskId,_u32 event);

#endif /* TASK_EVENT_EVENT_H_ */
