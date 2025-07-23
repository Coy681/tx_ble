#include"sch_map.h"
#include"sch.h"

_RAM_CODE
void sch_map_calculate_free_space_by_time(_u32 refTimeStart,_u32 refTimeEnd,sch_map_node_t* node,\
                                          _u32 nodeCount,sch_map_free_slot_t** freeBlock,_u32* freeCount)
{
	if(refTimeStart<=refTimeEnd)
	{
    	*freeCount = 0;
    	return;
	}
    if(POINTER_NOT_VALID(node)||nodeCount == 0)
    {
    	*freeBlock = (sch_map_free_slot_t*)tx_malloc(sizeof(sch_map_free_slot_t));
    	(*freeBlock)[0].start = refTimeStart;
    	(*freeBlock)[0].end   = refTimeEnd;
    	*freeCount = 1;
    	return;
    }
    DEBUG_GPIO_HIGH(GPIO_9);
    _u32 mapNodeCount = 0;
    for(int i=0;i<nodeCount;i++)
    {
        _u32 nodeStart = node[i].start;
        while(txCompareTime(refTimeEnd,nodeStart))
        {
            mapNodeCount++;
        	if(node[i].type != SCH_PERIODIC_TASK)
        	{
        		break;
        	}
            nodeStart += node[i].period;
        }
    }
    sch_map_slot_t blockList[mapNodeCount];
    mapNodeCount = 0;
    for(int i=0;i<nodeCount;i++)
    {
        _u32 nodeStart = node[i].start;
        _u32 nodeEnd = node[i].end;
        while(txCompareTime(refTimeEnd,nodeStart))
        {
            blockList[mapNodeCount].start = nodeStart;
            blockList[mapNodeCount++].end   = nodeEnd>refTimeEnd?refTimeEnd:nodeEnd;
        	if(node[i].type != SCH_PERIODIC_TASK)
        	{
        		break;
        	}
            nodeStart += node[i].period;
            nodeEnd   += node[i].period;
        }
    }

    for(_u8 i=0;i<mapNodeCount-1;i++)
    {
        for(_u8 j=0;j<mapNodeCount-i-1;j++)
        {
            if(blockList[j].start>blockList[j+1].start)
            {
                sch_map_slot_t temp = blockList[j];
                blockList[j]        = blockList[j+1];
                blockList[j+1]      = temp;
            }
        }
    }
    _u32 mergedCount = 0;
    sch_map_slot_t mergedList[mapNodeCount];
    mergedList[mergedCount]  =  blockList[0];
    for(int i=1;i<mapNodeCount;i++)
    {
        if(txCompareTime(mergedList[mergedCount].end,blockList[i].start))
        {
            if(txCompareTime(blockList[i].end,mergedList[mergedCount].end))
            {
                mergedList[mergedCount].end = blockList[i].end;
            }
        }
        else
        {
            mergedList[++mergedCount] = blockList[i];
        }
    }
    *freeBlock =(sch_map_free_slot_t*)tx_malloc((mergedCount+2)*sizeof(sch_map_free_slot_t));
    *freeCount = 0;
    if(mergedList[0].start>refTimeStart)
    {
    	(*freeBlock)[(*freeCount)].start = refTimeStart;
    	(*freeBlock)[(*freeCount)++].end = mergedList[0].start;
    }

    for(int i=0;i<mergedCount;i++)
    {
        if(mergedList[i+1].start>mergedList[i].end)
        {
        	(*freeBlock)[(*freeCount)].start = mergedList[i].end;
        	(*freeBlock)[(*freeCount)++].end = mergedList[i+1].start;
        }
    }
    if(refTimeEnd>mergedList[mergedCount].end)
    {
    	(*freeBlock)[(*freeCount)].start = mergedList[mergedCount].end;
    	(*freeBlock)[(*freeCount)++].end = refTimeEnd;
    }
    DEBUG_GPIO_LOW(GPIO_9);
} 

_RAM_CODE
void sch_map_calculate_free_space_by_slot(_u32 refTimeStart,_u32 refTimeEnd,sch_map_node_t* node,\
                                          _u32 nodeCount,sch_map_free_slot_t** freeBlock,_u32* freeCount,_u32 slotUnit)
{
	if(refTimeStart<=refTimeEnd||(refTimeStart-refTimeEnd<slotUnit))
	{
    	*freeCount = 0;
    	return;
	}
    _u32 SlotNum = refTimeStart-refTimeEnd
    if(POINTER_NOT_VALID(node)||nodeCount == 0)
    {
    	*freeBlock = (sch_map_free_slot_t*)tx_malloc(sizeof(sch_map_free_slot_t));
    	(*freeBlock)[0].start = refTimeStart;
    	(*freeBlock)[0].end   = refTimeEnd;
    	*freeCount = 1;
    	return;
    }

}
