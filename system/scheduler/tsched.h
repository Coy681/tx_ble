#include"common/txCommon.h"

typedef enum
{
    SCH_FIXED_PERIODIC_TASK,  //anchor point fixed
    SCH_DYNAMIC_PERIODIC_TASK,//anchor point dynamic
    SCH_SPORADIC_TASK,        //sporadic
    SCH_INSERT_TASK,          //no anchor point,as more as possible.
    SCH_TASK_MAX,
}sch_task_type_e;

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

typedef void(*sch_cb_f)(_u8);

typedef struct sch_node_t  sch_node_t;
typedef struct sch_node_t
{
    _u8  llId;
    _u8  type;//sch_task_type_e
    _u8  priority;//sch_task_priority_e
    _u8  update;//schdule task
    _u32 timestamp;//us
    _u32 period;//us
    _u32 duration;//us
    _u32 startLatency;//us
    _u32 stopLatency;//us
    sch_cb_f cb;//start or stop or passed or canceled callback.
    sch_node_t* tNext;//task list
    sch_node_t* rNext;//running list
    sch_node_t* cNext;//canceled list
}sch_node_t;

typedef struct
{
	sch_node_t* pTaskList;
	sch_node_t* pRunningList;
	sch_node_t* pCanceledList;
}sch_ctrl_t;

typedef enum
{
	SCHE_MESSAGE_TASK_ADD,
	SCHE_MESSAGE_TASK_REMOVE,
	SCHE_MESSAGE_TASK_UPDATE,
}sch_background_event_e;

typedef struct _PACKED
{
    _u8 eventType;
    _u8 message[0];
}sch_message_t;

#ifndef TX_SCHE_LOG_ENABLE
#define TX_SCHE_LOG_ENABLE  1
#endif
