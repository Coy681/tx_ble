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

void sch_map_calculate_free_space_by_slot(_u32 refTime,sch_map_node_t* node,\
                                          _u32 nodeCount,_u32 refTimeStart,_u32 refTimeEnd,\
                                          sch_map_free_slot_t** freeBlock,_u32* freeCount)

                                          