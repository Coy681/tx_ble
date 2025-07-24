#include"tx_common.h"
#include"platform/platform.h"
#include"common/txCommon.h"
/**
 * sweep line map algorithm,powered by tianxiang huang,2025
 */

typedef struct
{
    _u32 start;
    _u32 end;
    _u32 period:24;
    _u32 type:8;
}sch_map_node_t;

typedef struct 
{
    _u32 start;
    _u32 end;
}sch_map_free_slot_t,sch_map_slot_t;

void sch_map_calculate_free_space_by_time(_u32 refTimeStart,_u32 refTimeEnd,sch_map_node_t* node,\
                                          _u32 nodeCount,sch_map_free_slot_t** freeBlock,_u32* freeCount);

void sch_map_calculate_free_space_by_slot(_u32 refTimeStart,_u32 refTimeEnd,sch_map_node_t* node,\
                                          _u32 nodeCount,sch_map_free_slot_t** freeBlock,_u32* freeCount,_u32 slotUnit);
