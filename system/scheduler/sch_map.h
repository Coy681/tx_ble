#include"tx_common.h"
#include"platform/platform.h"
#include"common/txCommon.h"
typedef struct
{
    _u32 start;
    _u32 end;
    _u32 period;
}sch_map_node_t;

typedef struct 
{
    _u32 start;
    _u32 end;
}sch_map_free_slot_t,sch_map_slot_t;

