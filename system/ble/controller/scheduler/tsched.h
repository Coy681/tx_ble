#include"common/txCommon.h"

typedef enum
{
    SCH_FIXED_PERIODIC_TASK,  //anchor point fixed
    SCH_DYNAMIC_PERIODIC_TASK,//anchor point dynamic
    SCH_SPORADIC_TASK,        //sporadic
    SCH_INSERT_TASK,          //no anchor point,as more as possible.
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

typedef struct 
{
    _u8  llId;
    _u8  type;
    _u8  priority;
    _u8  rsvd;
    _u32 start;
    _u32 duration;
    sch_node_t* next;
}sch_node_t;

typedef struct 
{
    
}shc_ctrl_t;



