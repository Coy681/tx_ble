#include"common/txCommon.h"

typedef struct 
{
    _u8  llId;
    _u8  priority;
    _u32 start;
    _u32 duration;
    sch_node_t* next;
}sch_node_t;
