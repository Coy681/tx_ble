#include"common/txCommon.h"
#include"system/task/event/event.h"
#include"system/task/message/message.h"
 #ifndef TX_SCH_H_
 #define TX_SCH_H_

typedef enum
{
    SCH_STATUS_SUCCESS          = 0x00,
    SCH_STATUS_REJECTED         = 0x01,
    SCH_STATUS_TASK_NULL        = 0x02,
}sch_status_e;
typedef enum
{
    SCH_TASK_PRIORITY_0 = 0,
    SCH_TASK_PRIORITY_1 = 2,
    SCH_TASK_PRIORITY_2 = 4,
    SCH_TASK_PRIORITY_3 = 6,
    SCH_TASK_PRIORITY_4 = 8,
    SCH_TASK_PRIORITY_5 = 10,
    SCH_TASK_PRIORITY_6 = 12,
    SCH_TASK_PRIORITY_7 = 14,
    SCH_TASK_PRIORITY_8 = 16,
    SCH_TASK_PRIORITY_9 = 18,
    SCH_TASK_PRIORITY_A = 20,
    SCH_TASK_PRIORITY_B = 22,
    SCH_TASK_PRIORITY_C = 24,
    SCH_TASK_PRIORITY_D = 26,
    SCH_TASK_PRIORITY_E = 28,
    SCH_TASK_PRIORITY_F = 30,
}sch_task_priority_e;

typedef enum
{
    SCH_TASK_START,
    SCH_TASK_STOP,
    SCH_TASK_CANCELED,
    SCH_TASK_PASSED,
}sch_callback_e;

typedef enum
{
    SCH_PERIODIC_TASK  = 0x01,  //anchor point fixed
    SCH_SPORADIC_TASK  = 0x02,  //sporadic
    SCH_ASAP_TASK      = 0x03,  //no anchor point,as soon as possible.
}sch_task_type_e;

typedef void(*sch_cb_f)(_u8);
typedef struct sch_node_t  sch_node_t;
typedef struct _PACKED sch_node_t 
{
    _u8  llId;
    _u8  type;//sch_task_type_e
    _u8  priority;//sch_task_priority_e
    _u8  rsvd0;
    _u32 timestamp;//us
    _u32 period;//us
    _u32 durationMin;//min duraiton,us
    _u32 duration;//expected duration,us
    _u32 startLatency;//us
    _u32 stopLatency;//us
    sch_cb_f cb;//start or stop or passed or canceled callback.
    sch_node_t* next;
}sch_node_t;

typedef struct 
{
    sch_node_t* pWaitingList;
    sch_node_t* pRunningTask;
    sch_node_t* pCanceledList;
}sch_ctrl_t;

typedef enum
{
	SCHE_MESSAGE_TASK_ADD,
	SCHE_MESSAGE_TASK_REMOVE,
}sch_background_event_e;

typedef struct _PACKED
{
    _u8 eventType;
    _u8 message[0];
}sch_message_t;

void sch_schedule_next_task(void);
#define TASK_VALID(task)              ((task)!=NULL)
#define TASK_NOT_VALID(task)          ((task)==NULL)
#define TASK_START_TIME(task)         (task->timestamp - task->startLatency)
#define TASK_SCH_PROCESS_TIME         20
#define TASK_STOP_TIME(task)          (task->timestamp + task->duration + task->stopLatency)


#ifndef TX_SCHE_LOG_ENABLE
#define TX_SCHE_LOG_ENABLE  1
#endif
#endif//TX_SCH_H_
