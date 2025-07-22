#include"sch_map.h"

_RAM_CODE
void sch_map_calculate_free_space_by_slot(_u32 refTime,sch_map_node_t* node,\
                                          _u32 nodeCount,_u32 refTimeStart,_u32 refTimeEnd,\
                                          sch_map_free_slot_t** freeBlock,_u32* freeCount)
{
    if(nodeCount == 0)
    {
        return;
    }
    _u32 totalMapNodeCount = 0;
    for(int i=0;i<nodeCount;i++)
    {
        _u32 nodeStart = node[i].start;
        _u32 nodeEnd = node[i].end;
        while(txCompareTime(nodeStart,refTimeStart)&&(txCompareTime(refTimeEnd,nodeEnd))||\
              txCompareTime(refTimeEnd,nodeStart)&&(txCompareTime(nodeEnd,refTimeEnd)))
        {
            nodeStart = node[i].start + node[i].period;
            nodeEnd   = node[i].end + node[i].period;
            totalMapNodeCount++;
        }
    }
    sch_map_free_slot_t* blockList = (sch_map_free_slot_t*)tx_malloc(totalMapNodeCount*sizeof(sch_map_free_slot_t));
    totalMapNodeCount = 0;
    for(int i=0;i<nodeCount;i++)
    {
        _u32 nodeStart = node[i].start;
        _u32 nodeEnd = node[i].end;
        while(txCompareTime(nodeStart,refTimeStart)&&(txCompareTime(refTimeEnd,nodeEnd))||\
              txCompareTime(refTimeEnd,nodeStart)&&(txCompareTime(nodeEnd,refTimeEnd)))
        {
            blockList[totalMapNodeCount].start = nodeStart;
            blockList[totalMapNodeCount++].end   = nodeEnd>refTimeEnd?nodeEnd:refTimeEnd;
            nodeStart = node[i].start + node[i].period;
            nodeEnd   = node[i].end + node[i].period;
        }
    }
    for(_u8 i=0;i<totalMapNodeCount-1;i++)
    {
        for(_u8 j=0;j<totalMapNodeCount-i-1;j++)
        {
            if(blockList[j].start<blockList[j+1].start)
            {
                sch_map_slot_t temp = blockList[j];
                blockList[j]        = blockList[j+1];
                blockList[j+1]      = temp;
            }
        }
    }
    _u32 mergedCount = 0;
    sch_map_slot_t* mergedList = (sch_map_slot_t*)tx_malloc(totalMapNodeCount*sizeof(sch_map_slot_t));
    mergedList[mergedCount++]  =  blockList[0]; 
    for(int i=1;i<totalMapNodeCount;i++)
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
            mergedList[mergedCount++] = blockList[i];
        }
    }

    *freeBlock = (sch_map_free_slot_t*)tx_malloc((mergedCount+2)*sizeof(sch_map_free_slot_t));
    if(mergedList[0].start>refTime)
    {
        freeBlock[(*freeCount)]->start = mergedList[0].start;
        freeBlock[(*freeCount)++]->end = mergedList[0].end;
    }
    for(int i=0;i<mergedCount-1;i++)
    {
        if(mergedList[i+1].end>mergedList[i].start)
        {
            freeBlock[(*freeCount)]->start = mergedList[i].start;
            freeBlock[(*freeCount)++]->end = mergedList[i+1].end;
        }
    }
    if(refTimeEnd>mergedList[mergedCount-1].end)
    {
        freeBlock[++(*freeCount)]->start = freeBlock[*freeCount]->end;
        freeBlock[++(*freeCount)]->end   = refTimeEnd;
    }
} 

void sch_map_calculate_free_space_by_time(_u32 refTime,sch_map_node_t* node,\
                                          _u32 nodeCount,_u32 refTimeStart,_u32 refTimeEnd,\
                                          sch_map_free_slot_t** freeBlock,_u32* freeCount)
{
    _u32 timelineLength = refTimeStart - refTimeEnd + 1;

}