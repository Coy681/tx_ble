#include"planner.h"
static planner_node_t* pPlannerlist;
planner_ctrl_t gPlannerCtrl;
#define NODE_VALID(node)            (node!=NULL)

void planner_set_base_interval(planner_time_e baseInterval)
{
    gPlannerCtrl.baseInterval = baseInterval;
    gPlannerCtrl.offsetMax = baseInterval/PLANNER_UNIT_US;
    gPlannerCtrl.plannerStartTime = system_time();
    planner_node_t* node = pPlannerlist;
    while(NODE_VALID(node))
    {
        node->cb(baseInterval);
        node = node->next;
    }
}

planner_ret_e planner_insert_node(planner_node_t* pNode)
{
	ASSERT(pNode!=NULL);
	pNode->next = NULL;
	if(NODE_VALID(pPlannerlist))
	{
		planner_node_t* scan = pPlannerlist;
		while(NODE_VALID(scan->next))
		{
			scan = scan->next;
		}
		scan->next = pNode;
	}
	else
	{
		pPlannerlist = pNode;
	}
	return PLANNER_SUCCESS;
}

planner_ret_e planner_delete_node(planner_node_t* pNode)
{
	ASSERT(pNode!=NULL);
	planner_node_t* scan = pPlannerlist;
	planner_node_t* prev = NULL;
	while(NODE_VALID(scan))
	{
		if(pNode == scan)
		{
			if(prev)
			{
				prev->next = scan->next;
			}
			else
			{
				pPlannerlist = pPlannerlist->next;
			}
			return PLANNER_SUCCESS;
		}
		else
		{
			prev = scan;
			scan = scan->next;
		}
	}
	return PLANNER_NOT_FOUND;
}


volatile _u8 AAA_MAP[100];
volatile _u8 AAA_MAP_SHIFT[100];
volatile _u8 AAA_MAP_STATI[100];
volatile _u8 AAA_MAP1[100];
volatile _u8 AAA_MAP_SHIFT1[100];
volatile _u8 AAA_MAP_STATI1[100];
//
//volatile _u8 AAA_MARK1;
//volatile _u8 AAA_MARK2;
//volatile _u8 AAA_MARK3;
static void planner_generate_map_table(_u8 id,_u8* table,_u8* shiftIndex,_u8* tableStatistics,_u32 interval)
{
    _u8 shiftMax = interval/gPlannerCtrl.baseInterval;
    planner_node_t* scan = pPlannerlist;
    memcpy((_u8*)AAA_MAP,table,100);
    memcpy((_u8*)AAA_MAP_SHIFT,shiftIndex,100);
    memcpy((_u8*)AAA_MAP_STATI,tableStatistics,100);
    while(NODE_VALID(scan))
    {
        if(((scan->interval % gPlannerCtrl.baseInterval) == 0) && (scan->id!=id))
        {
            if((scan->interval>=interval)&&(scan->interval % interval == 0))
            {
                _u8 mapShift = scan->shift % shiftMax;
                for(_u32 i=scan->offset;i<(scan->offset+scan->duration);i++)
                {
                	if(table[mapShift * gPlannerCtrl.offsetMax + i] == 0)
                	{
                        table[mapShift * gPlannerCtrl.offsetMax + i] = 1;
                        tableStatistics[mapShift]--;
                	}
                }
            }
            else if((scan->interval<interval)&&(interval%scan->interval == 0))
            {
                _u8 mapShiftCnt = (interval/scan->interval);
                _u8 mapShiftNum = (scan->interval/gPlannerCtrl.baseInterval);
                for(_u32 i=0;i<mapShiftCnt;i++)
                {
                    for(_u32 j=scan->offset;j<(scan->offset+scan->duration);j++)
                    {
                    	if(table[(i*mapShiftNum+scan->shift)*gPlannerCtrl.offsetMax+j] == 0)
                    	{
                            table[(i*mapShiftNum+scan->shift)*gPlannerCtrl.offsetMax+j] = 1;
                            tableStatistics[i*mapShiftNum+scan->shift]--;
                    	}
                    }
                }
            }
            else
            {
                for(_u32 i=0;i<shiftMax;i++)
                {
                    for(_u32 j=scan->offset;j<(scan->offset+scan->duration);j++)
                    {
                    	if(table[i*gPlannerCtrl.offsetMax+j] == 0)
                    	{
                            table[i*gPlannerCtrl.offsetMax+j] = 1;
                            tableStatistics[i]--;
                    	}
                    }
                }
            }
        }
        scan = scan->next;
    }

    for(_u8 i=0;i<shiftMax;i++)
	{
		for(_u8 j=0;j<shiftMax-i-1;j++)
		{
            if(tableStatistics[j]>tableStatistics[j+1])
            {
                _u8 temp = tableStatistics[j];
                tableStatistics[j] = tableStatistics[j+1];
                tableStatistics[j+1] = temp;

                temp = shiftIndex[j];
                shiftIndex[j] = shiftIndex[j+1];  
                shiftIndex[j+1] = temp;
            }
		}
	}
    memcpy((_u8*)AAA_MAP1,table,100);
    memcpy((_u8*)AAA_MAP_SHIFT1,shiftIndex,100);
    memcpy((_u8*)AAA_MAP_STATI1,tableStatistics,100);
//    while(1);
}

planner_ret_e planner_parameter_check(planner_node_t* pNode)
{
	ASSERT(pNode!=NULL);
	if((pNode->interval%gPlannerCtrl.baseInterval)!=0)
	{
		return PLANNER_BW_CONFLICT;
	}
    _u8 shiftMax = pNode->interval/gPlannerCtrl.baseInterval;
    _u8 plannerTable[shiftMax*gPlannerCtrl.offsetMax];
    _u8 plannerShiftIndex[shiftMax];
    _u8 plannerShiftStatistics[shiftMax];
    for(_u32 i=0;i<shiftMax;i++)
    {
        plannerShiftStatistics[i] = gPlannerCtrl.offsetMax;
        plannerShiftIndex[i] = i;
        for(_u32 j=0;j<gPlannerCtrl.offsetMax;j++)
        {
            plannerTable[i*gPlannerCtrl.offsetMax+j] = 0;
        }
    }
    planner_generate_map_table(pNode->id,plannerTable, plannerShiftIndex, plannerShiftStatistics, pNode->interval);
    for(_u32 i=pNode->offset;i<(pNode->offset+pNode->duration);i++)
    {
    	if(i>=gPlannerCtrl.offsetMax)
    	{
    		return PLANNER_BW_CONFLICT;
    	}
        if(plannerTable[pNode->shift * gPlannerCtrl.offsetMax + i] == 1)
        {
            return PLANNER_BW_CONFLICT;
        }
    }
    return PLANNER_SUCCESS;
}


planner_ret_e planner_parameter_request(_u8 mode,_u32 interval,_u16 durationMin,_u16 durationMax,planner_node_t* pNode)
{
	ASSERT(pNode!=NULL);
    if((interval%gPlannerCtrl.baseInterval)!=0)
	{
		return PLANNER_BW_CONFLICT;
	}
    _u8 shiftMax = interval/gPlannerCtrl.baseInterval;
    _u8 plannerTable[shiftMax*gPlannerCtrl.offsetMax];
    _u8 plannerShiftIndex[shiftMax];
    _u8 plannerShiftStatistics[shiftMax];
    for(_u32 i=0;i<shiftMax;i++)
    {
        plannerShiftStatistics[i] = gPlannerCtrl.offsetMax;
        plannerShiftIndex[i] = i;
        for(_u32 j=0;j<gPlannerCtrl.offsetMax;j++)
        {
            plannerTable[i*gPlannerCtrl.offsetMax+j] = 0;
        }
    }
    planner_generate_map_table(pNode->id,plannerTable, plannerShiftIndex, plannerShiftStatistics, interval);
    _u8 duration = durationMax;

    if(mode == PLANNER_MODE_CONCENTRATED || mode == PLANNER_MODE_SCATTERED)
    {
        for(int k=0;k<2;k++)//first try durationMax,then try durationMin.
        {
            for(_s32 i = (mode == PLANNER_MODE_CONCENTRATED ? 0 : shiftMax - 1);
                (mode == PLANNER_MODE_CONCENTRATED ? i < shiftMax : i >= 0);
                i += (mode == PLANNER_MODE_CONCENTRATED ? 1 : -1))
            {
                if(plannerShiftStatistics[i] >= duration)
                {
                    _u8 offsetSpace = 0;
                    for(_u8 j = 0; j < gPlannerCtrl.offsetMax; j++)
                    {
                        if(plannerTable[plannerShiftIndex[i] * gPlannerCtrl.offsetMax + j] == 0)
                        {
                            offsetSpace++;
                        }
                        else
                        {
                            offsetSpace = 0;
                        }
                        if(offsetSpace >= duration)
                        {
                            pNode->shift = plannerShiftIndex[i];
                            pNode->offset = j + 1 - duration ;
                            pNode->interval = interval;
                            pNode->duration = duration;
                            return PLANNER_SUCCESS;
                        }
                    }
                }
            }
            if(durationMax>durationMin)
            {
            	duration = durationMin;
            }
        }
    }
    return PLANNER_BW_FULL;
}


_u32 planner_get_ahchor_point(planner_node_t* pNode,_u32 refTime)
{
	_u32 anchorPoint = 0;
	_u32 mapAnchorPoint = gPlannerCtrl.plannerStartTime + (pNode->shift*gPlannerCtrl.offsetMax + pNode->offset)*PLANNER_UNIT_US;

	if(txCompareTime(refTime,mapAnchorPoint))
	{
		_u32 mod = (refTime-mapAnchorPoint)%pNode->interval;
		if(mod)
		{
			anchorPoint = refTime+(pNode->interval-mod);
		}
		else
		{
			anchorPoint = refTime;
		}
	}
	else
	{
		anchorPoint = mapAnchorPoint;
	}
	return anchorPoint;
}

void              planner_update_map_point_point(void)
{
	_u32 baseIntervalTime = gPlannerCtrl.baseInterval * PLANNER_UNIT_US;
    /* calculating planner start tick procedure should be protected, because IRQ will use planner start tick */
    _u32 r = irq_disable();

    _u32 clockTime = system_time();
    _u32 div  = (clockTime - gPlannerCtrl.plannerStartTime)/baseIntervalTime;
    _u32 mod  = (clockTime - gPlannerCtrl.plannerStartTime)%baseIntervalTime;

    gPlannerCtrl.plannerStartTime = clockTime-mod;
	planner_node_t* scan = pPlannerlist;
    while(NODE_VALID(scan))
    {
    	_u32 scanMaxShift  = scan->interval/gPlannerCtrl.baseInterval;
    	_u32 extraShift    = scanMaxShift - (div%scanMaxShift);
    	scan->shift = (scan->shift+extraShift)%scanMaxShift;
    	scan = scan->next;
    }
    irq_restore(r);
}
