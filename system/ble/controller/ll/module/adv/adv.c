/*
 * adv1.c
 *
 *  Created on: 2025年12月15日
 *      Author: Admin
 */

#include"adv.h"
#include"adv_internal.h"
#include"system/scheduler/sch_map.h"
#include"../../channel/channel.h"

/************************************Global Variables Define***********************************/

/******* local enum define *******/
typedef enum
{
    ADV_SM_STATE_IDLE,
    ADV_SM_STATE_SENDING_ADV,
    ADV_SM_STATE_SENDING_RSP,
    ADV_SM_STATE_RECEIVING,
    #if(LL_SUPPORT_LE_EXTENDED_ADVERTISING)
    ADV_SM_STATE_SENDING_AUX_ADV,
    ADV_SM_STATE_SENDING_AUX_SCAN_RSP,
    ADV_SM_STATE_SENDING_AUX_CONNECT_RSP,
    ADV_SM_STATE_SENDING_CHAIN_ADV,
    ADV_SM_STATE_RECEIVING_AUX,
    #endif
    #if(LL_SUPPORT_LE_PERIODIC_ADVERTISING)
    ADV_SM_STATE_SENDING_PERIODIC_ADV,
    #endif
    #if(LL_SUPPORT_PERIODIC_ADVERTISING_WITH_RESPONSES_ADVERTISER)
    #endif
}adv_sm_state_e;

typedef enum
{
    ADV_CONTEXT_DEFAULT     = BIT(0),
    ADV_CONTEXT_SCANNABLE   = BIT(1),
    ADV_CONTEXT_CONNECTABLE = BIT(2),
    ADV_CONTEXT_AUXILIARY   = BIT(3),
    ADV_CONTEXT_CHAINED     = BIT(4),
}adv_context_e;



/******* local struct define *******/
typedef int(*adv_event_sm_cb)(void);
typedef struct _PACKED
{
    adv_event_sm_cb cb;
    adv_context_e   context;
    adv_sm_state_e  currentState;
    adv_sm_state_e  transSuccessState;
    adv_sm_state_e  transFailState;
    ll_sm_event_e   event;
}adv_event_sm_t;

typedef struct
{
    adv_event_class_e eventClass;
    adv_event_sm_t*   sm;
    _u32              listLen;
    _u32              context;
}adv_procedure_list_t;

typedef struct
{
    adv_event_type_e       eventType;//adv mode
    adv_procedure_list_t   *procedureList;
    _u32                   listLen;
}adv_sequence_t;

#if(LL_SUPPORT_LE_EXTENDED_ADVERTISING)
typedef struct 
{
    _u32 anchorPoint;
    _u32 targetAnchorPoint;
    _u8  phy;
    _u8  channel;
}adv_extended_header_auxInfo_t;
#endif//LL_SUPPORT_LE_EXTENDED_ADVERTISING

/******* local macro define *******/
#define ADV_PROCEDURE_LIST_LENGTH(adv_procedure_list)      (sizeof(adv_procedure_list)/sizeof(adv_procedure_list[0]))
#define ADV_SEQUENCE_LIST_LENGTH(adv_sequence_list)        (sizeof(adv_sequence_list)/sizeof(adv_sequence_list[0]))
#define ADV_SM_LIST_LENGTH(adv_sm_list)                    (sizeof(adv_sm_list)/sizeof(adv_sm_list[0]))

/******* local global variable define *******/
/*
 * local global ll state machine and adv ctrl module assigned in sch enter
 */
static ll_sm_t* LLSM;//Use uppercase letters to represent common ll state machine pointer.
static ll_internal_adv_ctrl_t* advCtrl;//adv ctrl pointer,point to current adv struct

/*
 * current adv set and current adv class assigned in 'adv_get_next_event',when next adv event is confirmed,the adv schedule need to be remapped.
 */
static ll_internal_adv_set_t* currentSet;//current used adv set
static int currentEventClass;//current event class,search for 'adv_event_class_e'


/*************************************ADV Schedule Process*******************************************/
_RAM_CODE
static void adv_set_sch_remap(ll_adv_sch_entry_t* advSch)
{
    LLSM->sch.timestamp    = advSch->anchorPoint;
    LLSM->sch.duration     = LLSM->sch.durationMin = advSch->duration;
    LLSM->sch.startLatency = advSch->startMargin;
    LLSM->sch.stopLatency  = advSch->stopMargin;
    LLSM->sch.period       = advSch->interval;
}

_RAM_CODE 
void adv_get_next_event(void)
{
	#if(LL_SUPPORT_LE_EXTENDED_ADVERTISING)
    _u32 timestamp = 0;
    for(_u8 i=0;i<BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS;i++)
    {
        if(advCtrl->set[i].enable)
        {
			if((timestamp == 0)||txCompareTime(timestamp,advCtrl->set[i].la.sch.anchorPoint))
			{
				timestamp = advCtrl->set[i].la.sch.anchorPoint|1;
				currentSet = &advCtrl->set[i];
				currentEventClass = ADV_EVENT;
				adv_set_sch_remap(&currentSet->la.sch);
			}
            if(advCtrl->set[i].schMap&ADV_SCH_MAP_AUX)
            {
                if((timestamp == 0)||txCompareTime(timestamp,advCtrl->set[i].ea->anchor))
                {
                    timestamp = advCtrl->set[i].ea->anchor|1;
                    currentSet = &advCtrl->set[i];
                    currentSet->pChain = &currentSet->ea->chain;
                    if(advCtrl->set[i].ea->anchor == advCtrl->set[i].ea->aux.sch.anchorPoint)
                    {
                        adv_set_sch_remap(&currentSet->ea->aux.sch);
                        currentEventClass = ADV_EXTENDED_EVENT;
                    }
                    else if(advCtrl->set[i].schMap&ADV_SCH_MAP_AUX_CHAIN)
                    {
                        adv_set_sch_remap(&currentSet->ea->chain.entry[currentSet->ea->chain.current].sch);
                        currentEventClass = ADV_CHAINED_EVENT;
                    }
                }
            }
        }

		#if(LL_SUPPORT_LE_PERIODIC_ADVERTISING)
		if((advCtrl->set[i].schMap&ADV_SCH_MAP_PA)&&advCtrl->set[i].pa->active)
		{
			if((timestamp == 0)||txCompareTime(timestamp,advCtrl->set[i].pa->anchor))
			{
				timestamp = advCtrl->set[i].pa->anchor|1;
				currentSet = &advCtrl->set[i];
                currentSet->pChain = &currentSet->pa->chain;
				if(advCtrl->set[i].pa->anchor == advCtrl->set[i].pa->sync.sch.anchorPoint)
				{
					currentEventClass = ADV_PERIODIC_EVENT;
					adv_set_sch_remap(&currentSet->pa->sync.sch);
				}
				else if(advCtrl->set[i].schMap&ADV_SCH_MAP_PA_CHAIN)
				{
					currentEventClass = ADV_CHAINED_EVENT;
					adv_set_sch_remap(&currentSet->pa->chain.entry[currentSet->pa->chain.current].sch);
				}
			}
		}
		#endif/*(LL_SUPPORT_LE_PERIODIC_ADVERTISING)*/
    }
    #if(LL_SUPPORT_PERIODIC_ADVERTISING_WITH_RESPONSES_ADVERTISER)
    #endif/*(LL_SUPPORT_PERIODIC_ADVERTISING_WITH_RESPONSES_ADVERTISER)*/

	#else/*(!LL_SUPPORT_LE_EXTENDED_ADVERTISING)*/
    currentSet = &advCtrl->set[0];
    currentEventClass = ADV_EVENT;
    adv_set_sch_remap(&currentSet->la.sch);
	#endif/*(LL_SUPPORT_LE_EXTENDED_ADVERTISING)*/
}

/**
 * @param timestamp - if present,timestamp is air packet anchor point,so need minus hardware prepare time
 * 				    - if not present,packet send time is not specified,so we can use system time.
 */
_RAM_CODE
static void adv_prepare_phy(ll_adv_phy_entry_t* phy,_u32 timestamp,phy_dir_e phydir)
{
    phy_obj_cast(&LLSM->phy);
    LLSM->phy.accessCode = phy->accessCode;
    LLSM->phy.crcInit    = phy->crcInit;
    LLSM->phy.mode       = phy->mode;
    LLSM->phy.chnIdx     = phy->chn;
    LLSM->phy.dir        = phydir;
    if(timestamp!=0)
    {
        LLSM->phy.timestamp  = timestamp - LLSM->phy.hw_get_prepare_time();
    }
    else
    {
        LLSM->phy.timestamp  = system_time();
    }
    if(phydir == PHY_DIR_TX)
    {
        LLSM->phy.txAddress   = phy->txAddress;
    }
    else if(phydir == PHY_DIR_RX)
    {
    	LLSM->phy.rxAddress   = phy->rxAddress;
        LLSM->phy.rxTimeout   = BLE_ADV_DEFAULT_RX_TIMEOUT_US;
        LLSM->phy.rxMaxOctets = phy->rxMaxOctets;
    }
}


_RAM_CODE
int ll_adv_task_timing_allocation(ll_internal_adv_set_t* advSet,_u32 refStart,_u32 refEnd,_u8 mapType)
{

    _u8  nodeNum    = 0;
    _u8  nodeCount  = 0;
    reCal:
        nodeCount+=20;
        nodeNum = 0;
        if(nodeCount>60)
        {
            ASSERT(0);
        }
        sch_map_node_t node[nodeCount];//20 node can cover most of the situation,if can't,plus 20 and calculate again.
        sch_node_t* schNode = sch_get_task_list(SCH_WAITING_LIST);
        for(_u8 n=0;n<2;n++)
        {
            while(POINTER_VALID(schNode))
            {
            	if(schNode->id == LLSM->sch.id)
            	{
                    schNode = schNode->next;

            		continue;
            	}
                if(TASK_START_TIME(schNode)<refEnd)
                {
                    node[nodeNum].start = TASK_START_TIME(schNode);
                    node[nodeNum].end   = TASK_STOP_TIME(schNode);
                    if(schNode->type == SCH_PERIODIC_TASK)
                    {
                        node[nodeNum].type = SCH_PERIODIC_TASK;
                        node[nodeNum].period = schNode->period;
                    }
                    else 
                    {
                        node[nodeNum].type = SCH_SPORADIC_TASK;
                    }
                    nodeNum++;

                }
                schNode = schNode->next;
                if(nodeNum>nodeCount)
                {

                    goto reCal;
                }               
            }
            schNode = sch_get_task_list(SCH_CANCELED_LIST);           
        }

	    #if(LL_SUPPORT_LE_EXTENDED_ADVERTISING)
        for(int i=0;i<BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS;i++)
        {
            if((advCtrl->set[i].handle == advSet->handle) || (advCtrl->set[i].enable == 0))
            {
                continue;
            }
            if(advCtrl->set[i].enable)
            {
                if(txCompareTime(refEnd,advCtrl->set[i].la.sch.anchorPoint))
                {
                    node[nodeNum].start = advCtrl->set[i].la.sch.anchorPoint - advCtrl->set[i].la.sch.startMargin;
                    node[nodeNum].end   = node[nodeNum].start+3*(advCtrl->set[i].la.sch.duration + advCtrl->set[i].la.sch.startMargin+advCtrl->set[i].la.sch.stopMargin);
                    node[nodeNum].type  = SCH_SPORADIC_TASK;
                    nodeNum++;
                    if(nodeNum>nodeCount)
                    {
                        goto reCal;
                    }
                }
                if(advCtrl->set[i].schMap&ADV_SCH_MAP_AUX)
                {
                    if(txCompareTime(refEnd,advCtrl->set[i].ea->aux.sch.anchorPoint))
                    {
                        node[nodeNum].start = advCtrl->set[i].ea->aux.sch.anchorPoint - advCtrl->set[i].ea->aux.sch.startMargin;
                        node[nodeNum].end   = advCtrl->set[i].ea->aux.sch.anchorPoint + advCtrl->set[i].ea->aux.sch.duration + advCtrl->set[i].ea->aux.sch.stopMargin;
                        node[nodeNum].type  = SCH_SPORADIC_TASK;
                        nodeNum++;
                        if(nodeNum>nodeCount)
                        {
                            goto reCal;
                        }
                    }
                    if(advCtrl->set[i].schMap&ADV_SCH_MAP_AUX_CHAIN)
                    {
                        for(int j=0;j<advCtrl->set[i].ea->chain.cnt;j++)
                        {
                            if(txCompareTime(refEnd,advCtrl->set[i].ea->chain.entry[j].sch.anchorPoint))
                            {
                                node[nodeNum].start = advCtrl->set[i].ea->chain.entry[j].sch.anchorPoint\
                                                    - advCtrl->set[i].ea->chain.entry[j].sch.startMargin;

                                node[nodeNum].end   = advCtrl->set[i].ea->chain.entry[j].sch.anchorPoint\
                                                    + advCtrl->set[i].ea->chain.entry[j].sch.duration\
                                                    + advCtrl->set[i].ea->chain.entry[j].sch.stopMargin;
                                node[nodeNum].type  = SCH_SPORADIC_TASK;
                                nodeNum++;
                                if(nodeNum>nodeCount)
                                {
                                    goto reCal;
                                }
                            }
                        }
                    }
                }
            }
			#if(LL_SUPPORT_LE_PERIODIC_ADVERTISING)
    		if(POINTER_VALID(advCtrl->set[i].pa)&&advCtrl->set[i].pa->active)
            {
    			if(advCtrl->set[i].schMap&ADV_SCH_MAP_PA)
    			{
    				if(txCompareTime(refEnd,advCtrl->set[i].pa->sync.sch.anchorPoint))
    				{
                        node[nodeNum].start = advCtrl->set[i].pa->sync.sch.anchorPoint - advCtrl->set[i].pa->sync.sch.startMargin;
                        node[nodeNum].end   = advCtrl->set[i].pa->sync.sch.anchorPoint + advCtrl->set[i].pa->sync.sch.duration + advCtrl->set[i].pa->sync.sch.stopMargin;
                        node[nodeNum].type  = SCH_PERIODIC_TASK;
                        nodeNum++;
                        if(nodeNum>nodeCount)
                        {
                            goto reCal;
                        }
    				}
    				if(advCtrl->set[i].schMap&ADV_SCH_MAP_PA_CHAIN)
    				{
                        for(int j=0;j<advCtrl->set[i].pa->chain.cnt;j++)
                        {
                            if(txCompareTime(refEnd,advCtrl->set[i].pa->chain.entry[j].sch.anchorPoint))
                            {
                                node[nodeNum].start = advCtrl->set[i].pa->chain.entry[j].sch.anchorPoint\
                                                    - advCtrl->set[i].pa->chain.entry[j].sch.startMargin;

                                node[nodeNum].end   = advCtrl->set[i].pa->chain.entry[j].sch.anchorPoint\
                                                    + advCtrl->set[i].pa->chain.entry[j].sch.duration\
                                                    + advCtrl->set[i].pa->chain.entry[j].sch.stopMargin;
                                node[nodeNum].type  = SCH_SPORADIC_TASK;
                                nodeNum++;
                                if(nodeNum>nodeCount)
                                {
                                    goto reCal;
                                }
                            }
                        }
    				}
    			}
            }
			#endif
        }
        #endif/*(LL_SUPPORT_LE_EXTENDED_ADVERTISING)*/
    //end of symbol "reCal"
    _u32 freeBlockCount = 0;
    sch_map_free_slot_t* freeBlock = NULL;
    sch_map_calculate_free_space_by_time(refStart,refEnd,node,nodeNum,&freeBlock,&freeBlockCount);
    int blockIndex = 0;
    if(mapType&ADV_SCH_MAP_PRI)
    {
        _u32 primarySpace = 3*(advSet->la.sch.startMargin + advSet->la.sch.stopMargin + advSet->la.sch.duration);
        for(;blockIndex<freeBlockCount;blockIndex++)
        {
            if((freeBlock[blockIndex].end - freeBlock[blockIndex].start)>(primarySpace+PACKET_T_MAFS_TIME))
            {
                advSet->la.sch.anchorPoint = freeBlock[blockIndex].start+advSet->la.sch.startMargin;
                freeBlock[blockIndex].start +=(primarySpace+PACKET_T_MAFS_TIME);//todo,check need space how much
                break;
            }
        }
    }
	#if(LL_SUPPORT_LE_EXTENDED_ADVERTISING)
    if(((mapType&ADV_SCH_MAP_AUX))&&(advSet->schMap&ADV_SCH_MAP_AUX))
    {
        _u32 secondarySpace = advSet->ea->aux.sch.startMargin\
        		            + advSet->ea->aux.sch.duration\
							+ advSet->ea->aux.sch.stopMargin;
        for(;blockIndex<freeBlockCount;blockIndex++)
        {
            if((freeBlock[blockIndex].end - freeBlock[blockIndex].start)>(secondarySpace+PACKET_T_MAFS_TIME))
            {
                advSet->ea->aux.sch.anchorPoint = freeBlock[blockIndex].start+advSet->ea->aux.sch.startMargin;
                advSet->ea->anchor = advSet->ea->aux.sch.anchorPoint;
                freeBlock[blockIndex].start +=(secondarySpace+PACKET_T_MAFS_TIME);//todo,check need space how much
                break;
            }
        }
        if((mapType&ADV_SCH_MAP_AUX_CHAIN)&&(advSet->schMap&ADV_SCH_MAP_AUX_CHAIN))
        {

            for(int j=0;j<advSet->ea->chain.cnt;j++)
            {
                _u32 chainSpace = advSet->ea->chain.entry[j].sch.startMargin\
                                + advSet->ea->chain.entry[j].sch.duration\
                                + advSet->ea->chain.entry[j].sch.stopMargin;
                for(;blockIndex<freeBlockCount;blockIndex++)
                {
                    if((freeBlock[blockIndex].end - freeBlock[blockIndex].start)>(chainSpace+PACKET_T_MAFS_TIME))
                    {
                        advSet->ea->chain.entry[j].sch.anchorPoint = freeBlock[blockIndex].start+advSet->ea->chain.entry[j].sch.startMargin;
                        freeBlock[blockIndex].start +=(chainSpace+PACKET_T_MAFS_TIME);//todo,check need space how much
                        break;
                    }
                }
            }
        }
    }
	#if(LL_SUPPORT_LE_PERIODIC_ADVERTISING)
    if(((mapType&ADV_SCH_MAP_PA))&&(advSet->schMap&ADV_SCH_MAP_PA))
    {//attention,chain of periodic sync pdu should greater than sync anchor and smaller than sync anchor plus sync interval
        if((mapType&ADV_SCH_MAP_PA_CHAIN)&&(advSet->schMap&ADV_SCH_MAP_PA_CHAIN))
        {
            for(int j=0;j<advSet->pa->chain.cnt;j++)
            {
                _u32 chainSpace = advSet->pa->chain.entry[j].sch.startMargin\
                                + advSet->pa->chain.entry[j].sch.duration\
                                + advSet->pa->chain.entry[j].sch.stopMargin;
                for(;blockIndex<freeBlockCount;blockIndex++)
                {
                    if((freeBlock[blockIndex].end - freeBlock[blockIndex].start)>(chainSpace+PACKET_T_MAFS_TIME))
                    {
                        advSet->pa->chain.entry[j].sch.anchorPoint = freeBlock[blockIndex].start+advSet->pa->chain.entry[j].sch.startMargin;
                        freeBlock[blockIndex].start +=(chainSpace+PACKET_T_MAFS_TIME);//todo,check need space how much
                        break;
                    }
                }
            }
        }
    }
	#endif/*(LL_SUPPORT_LE_PERIODIC_ADVERTISING)*/
    #endif/*(LL_SUPPORT_LE_EXTENDED_ADVERTISING)*/
    tx_free((_u8*)freeBlock);
}

#if(LL_SUPPORT_LE_EXTENDED_ADVERTISING)
_u16 adv_calculate_extended_header_length(_u8 flags)
{
    if(flags == 0)
    {   
        return 0;
    }
    _u16 extHdLen = 2;
    if(flags & ADV_EXTENDED_HEADER_FLAG_ADV_A)
    {
        extHdLen+=sizeof(adv_extended_header_subfield_advA_t);
    }
    if(flags & ADV_EXTENDED_HEADER_FLAG_TARGET_A)
    {
        extHdLen+=sizeof(adv_extended_header_subfield_targetA_t);
    }
    if(flags & ADV_EXTENDED_HEADER_FLAG_CTE_INFO)
    {
        extHdLen+=sizeof(adv_extended_header_subfield_cteInfo_t);
    }
    if(flags & ADV_EXTENDED_HEADER_FLAG_ADI)
    {
        extHdLen+=sizeof(adv_extended_header_subfield_adi_t);
    }
    if(flags & ADV_EXTENDED_HEADER_FLAG_AUX_PTR)
    {
        extHdLen+=sizeof(adv_extended_header_subfield_auxPtr_t);
    }
    if(flags & ADV_EXTENDED_HEADER_FLAG_SYNC_INFO)
    {
        extHdLen+=sizeof(adv_extended_header_subfield_syncInfo_t);
    }
    if(flags & ADV_EXTENDED_HEADER_FLAG_TX_POWER)
    {
        extHdLen+=sizeof(adv_extended_header_subfield_Tx_Power_t);
    }
    return extHdLen;
}

static void adv_generate_extended_header(_u8* packet,_u8 advMode,_u8 flags,adv_extended_header_auxInfo_t* auxInfo,_u16 did)
{
    adv_extended_header_t* extHeader = (adv_extended_header_t*)packet;
    extHeader->advMode = advMode;
    extHeader->flags   = flags;
    _u8 offset = 0;
    if(flags & ADV_EXTENDED_HEADER_FLAG_ADV_A)
    {
        txMemcpy(extHeader->param,ll_get_device_address(),6);
        offset+=sizeof(adv_extended_header_subfield_advA_t);
    }
    if(flags & ADV_EXTENDED_HEADER_FLAG_TARGET_A)
    {
        txMemcpy(extHeader->param+offset,currentSet->peerAddress,6);
        offset+=sizeof(adv_extended_header_subfield_targetA_t);
    }
    if(flags & ADV_EXTENDED_HEADER_FLAG_CTE_INFO)
    {
        ((adv_extended_header_subfield_cteInfo_t*)(extHeader->param+offset))->info = 0;
        offset+=sizeof(adv_extended_header_subfield_cteInfo_t);
    }
    if(flags & ADV_EXTENDED_HEADER_FLAG_ADI)
    {
        ((adv_extended_header_subfield_adi_t*)(extHeader->param+offset))->did = did;
        ((adv_extended_header_subfield_adi_t*)(extHeader->param+offset))->sid = currentSet->sid;
        offset+=sizeof(adv_extended_header_subfield_adi_t);
    }
    if(flags & ADV_EXTENDED_HEADER_FLAG_AUX_PTR)
    {
        ((adv_extended_header_subfield_auxPtr_t*)(extHeader->param+offset))->chnInx = auxInfo->channel;
        ((adv_extended_header_subfield_auxPtr_t*)(extHeader->param+offset))->ca     = ADV_EXTENDED_CA_51_500PPM;
        ((adv_extended_header_subfield_auxPtr_t*)(extHeader->param+offset))->auxPhy = auxInfo->phy;
        if(txCompareTime(auxInfo->anchorPoint,auxInfo->targetAnchorPoint))
        {
            ((adv_extended_header_subfield_auxPtr_t*)(extHeader->param+offset))->auxOffset   = 0;
            ((adv_extended_header_subfield_auxPtr_t*)(extHeader->param+offset))->offsetUnits = 0;
        }  
        else
        {
            _u32 anchorDiff = auxInfo->targetAnchorPoint - auxInfo->anchorPoint;
            if(anchorDiff>(30*0x1fff))
            {
                ((adv_extended_header_subfield_auxPtr_t*)(extHeader->param+offset))->auxOffset   = anchorDiff/300;
                ((adv_extended_header_subfield_auxPtr_t*)(extHeader->param+offset))->offsetUnits = ADV_EXTENDED_OFFSET_UNIT_300US;
            }
            else
            {
                ((adv_extended_header_subfield_auxPtr_t*)(extHeader->param+offset))->auxOffset   = anchorDiff/30;
                ((adv_extended_header_subfield_auxPtr_t*)(extHeader->param+offset))->offsetUnits = ADV_EXTENDED_OFFSET_UNIT_30US;
            }
        }
        offset+=sizeof(adv_extended_header_subfield_auxPtr_t);
    }
    if(flags & ADV_EXTENDED_HEADER_FLAG_SYNC_INFO)
    {
		#if(LL_SUPPORT_LE_PERIODIC_ADVERTISING)
    	((adv_extended_header_subfield_syncInfo_t*)(extHeader->param+offset))->interval   = currentSet->pa->sync.sch.interval;
    	//todo with access code,crc init,sca
    	((adv_extended_header_subfield_syncInfo_t*)(extHeader->param+offset))->chMH       = 0x1f;
    	((adv_extended_header_subfield_syncInfo_t*)(extHeader->param+offset))->chML       = 0xffffffff;
    	((adv_extended_header_subfield_syncInfo_t*)(extHeader->param+offset))->AA         = 0x23411243;
    	((adv_extended_header_subfield_syncInfo_t*)(extHeader->param+offset))->crcInit[0] = 0x32;
    	((adv_extended_header_subfield_syncInfo_t*)(extHeader->param+offset))->crcInit[1] = 0x54;
    	((adv_extended_header_subfield_syncInfo_t*)(extHeader->param+offset))->crcInit[2] = 0x76;
    	((adv_extended_header_subfield_syncInfo_t*)(extHeader->param+offset))->sca        = 3;
    	_u32 stepCnt = 0;
    	_u32 anchorTime = currentSet->ea->aux.sch.anchorPoint;
    	_u32 targetTime = currentSet->pa->sync.sch.anchorPoint;
    	if(txCompare(anchorTime,targetTime))
    	{
    		stepCnt = (anchorTime-targetTime)/currentSet->pa->sync.sch.interval;
    		stepCnt++;
    		targetTime+=(stepCnt*currentSet->pa->sync.sch.interval);
    	}
        _u32 anchorDiff = targetTime - anchorTime;
        if(anchorDiff>(30*0x1fff))
        {
        	((adv_extended_header_subfield_syncInfo_t*)(extHeader->param+offset))->offsetUnits    = ADV_EXTENDED_OFFSET_UNIT_300US;
        	((adv_extended_header_subfield_syncInfo_t*)(extHeader->param+offset))->offsetBase     = anchorDiff/300;
        	((adv_extended_header_subfield_syncInfo_t*)(extHeader->param+offset))->offsetAdjust   = 0;
        }
        else
        {
        	((adv_extended_header_subfield_syncInfo_t*)(extHeader->param+offset))->offsetUnits    = ADV_EXTENDED_OFFSET_UNIT_30US;
        	((adv_extended_header_subfield_syncInfo_t*)(extHeader->param+offset))->offsetBase     = anchorDiff/30;
        	((adv_extended_header_subfield_syncInfo_t*)(extHeader->param+offset))->offsetAdjust   = 0;
        }
        ((adv_extended_header_subfield_syncInfo_t*)(extHeader->param+offset))->eventCounter   = currentSet->pa->eventCnt+stepCnt;
		#endif
        offset+=sizeof(adv_extended_header_subfield_syncInfo_t);
    }
    if(flags & ADV_EXTENDED_HEADER_FLAG_TX_POWER)
    {
        offset+=sizeof(adv_extended_header_subfield_Tx_Power_t);
    }
    extHeader->len = 1+offset;
}
#endif//LL_SUPPORT_LE_EXTENDED_ADVERTISING

//LL_ADV_TYPE_ADV_IND
static void adv_ind_pdu_prepare(void)
{
    _u8* packet = ll_adv_packet_data_prepare(currentSet->la.phy.txAddress,6+currentSet->data.len,LL_ADV_TYPE_ADV_IND);
    txMemcpy(((adv_type_ind_t*)packet)->advA,ll_get_device_address(),6);
    txMemcpy(((adv_type_ind_t*)packet)->advData,currentSet->data.addr,currentSet->data.len);
    ll_adv_packet_make(currentSet->la.phy.txAddress,LL_CHANNEL_SUPP_CSA2,currentSet->ownAddressType?1:0,0);
}
//LL_ADV_TYPE_ADV_DIRECT_IND
static void adv_direct_ind_pdu_prepare(void)
{
    _u8* packet = ll_adv_packet_data_prepare(currentSet->la.phy.txAddress,12,LL_ADV_TYPE_ADV_DIRECT_IND);
    txMemcpy(((adv_type_direct_ind_t*)packet)->advA,ll_get_device_address(),6);
    txMemcpy(((adv_type_direct_ind_t*)packet)->targetA,currentSet->peerAddress,6);
    ll_adv_packet_make(currentSet->la.phy.txAddress,LL_CHANNEL_SUPP_CSA2,currentSet->ownAddressType?1:0,currentSet->peerAddressType?1:0);
}
//LL_ADV_TYPE_ADV_NONCONN_IND
static void adv_non_conn_pdu_prepare(void)
{
    _u8* packet = ll_adv_packet_data_prepare(currentSet->la.phy.txAddress,6+currentSet->data.len,LL_ADV_TYPE_ADV_NONCONN_IND);
    txMemcpy(((adv_type_nonConn_ind_t*)packet)->advA,ll_get_device_address(),6);
    txMemcpy(((adv_type_nonConn_ind_t*)packet)->advData,currentSet->data.addr,currentSet->data.len);
    ll_adv_packet_make(currentSet->la.phy.txAddress,0,currentSet->ownAddressType?1:0,0);
}
//LL_ADV_TYPE_SCAN_RSP
static void adv_scan_rsp_pdu_prepare(void)
{
    _u8* packet = ll_adv_packet_data_prepare(currentSet->la.phy.txAddress,6+currentSet->scanRsp.len,LL_ADV_TYPE_SCAN_RSP);
    txMemcpy(((adv_type_scan_rsp_t*)packet)->advA,ll_get_device_address(),6);
    txMemcpy(((adv_type_scan_rsp_t*)packet)->scanRsp,currentSet->scanRsp.addr,currentSet->scanRsp.len);
    ll_adv_packet_make(currentSet->la.phy.txAddress,0,currentSet->ownAddressType?1:0,0);
}
//LL_ADV_TYPE_ADV_SCAN_IND
static void adv_scan_ind_pdu_prepare(void)
{
    _u8* packet = ll_adv_packet_data_prepare(currentSet->la.phy.txAddress,6+currentSet->data.len,LL_ADV_TYPE_ADV_SCAN_IND);
    txMemcpy(((adv_type_scan_ind_t*)packet)->advA,ll_get_device_address(),6);
    txMemcpy(((adv_type_scan_ind_t*)packet)->advData,currentSet->data.addr,currentSet->data.len);
    ll_adv_packet_make(currentSet->la.phy.txAddress,0,currentSet->ownAddressType?1:0,0);
}
#if(LL_SUPPORT_LE_EXTENDED_ADVERTISING)
//LL_ADV_TYPE_ADV_EXT_IND
static void adv_ext_ind_pdu_prepare(_u8 advMode,_u8 flags,adv_extended_header_auxInfo_t* auxInfo,_u16 did)
{
    _u16 headerLen = adv_calculate_extended_header_length(flags);
    _u8* packet = ll_adv_packet_data_prepare(currentSet->la.phy.txAddress,headerLen,LL_ADV_TYPE_ADV_EXT_IND);
    adv_generate_extended_header(packet,advMode,flags,auxInfo,did);
    ll_adv_packet_make(currentSet->la.phy.txAddress,0,currentSet->ownAddressType?1:0,0);
}

//LL_ADV_TYPE_AUX_ADV_IND
static void adv_aux_adv_ind_pdu_prepare(_u8 advMode,_u8 flags,adv_extended_header_auxInfo_t* auxInfo,_u16 did)
{
    _u16 headerLen = adv_calculate_extended_header_length(flags);
    _u16 dataLen = currentSet->ea->aux.data.len;
    if(currentSet->eventProperty&LL_ADV_EVENT_PROPERTY_SCANNABLE)
    {
    	dataLen = 0;
    }
    _u8* packet = ll_adv_packet_data_prepare(currentSet->ea->aux.phy.txAddress,headerLen+dataLen,LL_ADV_TYPE_AUX_ADV_IND);
    adv_generate_extended_header(packet,advMode,flags,auxInfo,did);
    if(dataLen!=0)
    {
        txMemcpy(packet+headerLen,currentSet->ea->aux.data.addr,dataLen);
    }
    ll_adv_packet_make(currentSet->ea->aux.phy.txAddress,0,currentSet->ownAddressType?1:0,0);
}
//LL_ADV_TYPE_AUX_SCAN_RSP
static void adv_aux_scan_rsp_pdu_prepare(_u8 advMode,_u8 flags,adv_extended_header_auxInfo_t* auxInfo,_u16 did)
{
    _u16 headerLen = adv_calculate_extended_header_length(flags);
    _u8* packet = ll_adv_packet_data_prepare(currentSet->ea->aux.phy.txAddress,headerLen+currentSet->ea->aux.data.len,LL_ADV_TYPE_AUX_SCAN_RSP);
    adv_generate_extended_header(packet,advMode,flags,auxInfo,did);
    if(currentSet->ea->aux.data.len!=0)
    {
        txMemcpy(packet+headerLen,currentSet->ea->aux.data.addr,currentSet->ea->aux.data.len);
    }
    ll_adv_packet_make(currentSet->ea->aux.phy.txAddress,0,currentSet->ownAddressType?1:0,0);
}
//LL_ADV_TYPE_AUX_CONNECT_RSP
static void adv_aux_conn_rsp_pdu_prepare(_u8 advMode,_u8 flags,adv_extended_header_auxInfo_t* auxInfo,_u16 did)
{
    _u16 headerLen = adv_calculate_extended_header_length(flags);
    _u8* packet = ll_adv_packet_data_prepare(currentSet->ea->aux.phy.txAddress,headerLen,LL_ADV_TYPE_AUX_CONNECT_RSP);
    adv_generate_extended_header(packet,advMode,flags,auxInfo,did);
    ll_adv_packet_make(currentSet->ea->aux.phy.txAddress,0,currentSet->ownAddressType?1:0,0);
}
//LL_ADV_TYPE_AUX_CHAIN_IND
static void adv_aux_chain_ind_pdu_prepare(_u8 advMode,_u8 flags,adv_extended_header_auxInfo_t* auxInfo,_u16 did)
{
    _u16 headerLen = adv_calculate_extended_header_length(flags);
    _u8* packet = ll_adv_packet_data_prepare(currentSet->pChain->entry[currentSet->pChain->current].phy.txAddress,headerLen+currentSet->ea->chain.entry[currentSet->pChain->current].data.len,LL_ADV_TYPE_AUX_CHAIN_IND);
    adv_generate_extended_header(packet,advMode,flags,auxInfo,did);
    if(currentSet->pChain->entry[currentSet->pChain->current].data.len!=0)
    {
        txMemcpy(packet+headerLen,currentSet->pChain->entry[currentSet->pChain->current].data.addr,currentSet->pChain->entry[currentSet->pChain->current].data.len);
    }
    ll_adv_packet_make(currentSet->pChain->entry[currentSet->pChain->current].phy.txAddress,0,currentSet->ownAddressType?1:0,0);
}
#endif
#if(LL_SUPPORT_LE_PERIODIC_ADVERTISING)
//LL_ADV_TYPE_AUX_SYNC_IND
static void adv_aux_sync_ind_pdu_prepare(_u8 advMode,_u8 flags,adv_extended_header_auxInfo_t* auxInfo,_u16 did)
{
    _u16 headerLen = adv_calculate_extended_header_length(flags);
    _u8* packet = ll_adv_packet_data_prepare(currentSet->pa->sync.phy.txAddress,headerLen+currentSet->pa->sync.data.len,LL_ADV_TYPE_AUX_SYNC_IND);
    adv_generate_extended_header(packet,advMode,flags,auxInfo,did);
    if(currentSet->pa->sync.data.len!=0)
    {
        txMemcpy(packet+headerLen,currentSet->pa->sync.data.addr,currentSet->pa->sync.data.len);
    }
    ll_adv_packet_make(currentSet->pa->sync.phy.txAddress,0,currentSet->ownAddressType?1:0,0);
}
#endif

#if(LL_SUPPORT_PERIODIC_ADVERTISING_WITH_RESPONSES_ADVERTISER)
//LL_ADV_TYPE_AUX_SYNC_SUBEVENT_IND
static void adv_aux_sync_subevent_ind_pdu_prepare(void)
{

}
//LL_ADV_TYPE_AUX_SYNC_SUBEVENT_RSP
static void adv_aux_sync_subevent_rsp_pdu_prepare(void)
{

}
#endif

_RAM_CODE
static void adv_event_connectable_scannable_undirected_packet_prapare(adv_pdu_class_e pduClass)
{
    _u8* packet = NULL;
    switch(pduClass)
    {
        case ADV_PDU_CLASS_LEG:
        {
            adv_ind_pdu_prepare();
        }
        break;
        case ADV_PDU_CLASS_SCAN_RSP:
        {   
            adv_scan_rsp_pdu_prepare();
        } 
        break;
    }
}
//ADV_EVENT_CONNECTABLE_DIRECTED
_RAM_CODE
static void adv_event_connectable_directed_packet_prapare(adv_pdu_class_e pduClass)
{
    _u8* packet = NULL;
    switch(pduClass)
    {
        case ADV_PDU_CLASS_LEG:
        {
            adv_direct_ind_pdu_prepare();
        }
        break;
    }
}
//ADV_EVENT_SCANNABLE_UNDIRECTED
_RAM_CODE
static void adv_event_scannable_undirected_packet_prapare(adv_pdu_class_e pduClass)
{
    _u8* packet = NULL;
    switch(pduClass)
    {
        case ADV_PDU_CLASS_LEG:
        {
            adv_scan_ind_pdu_prepare();
        }
        break;
        case ADV_PDU_CLASS_SCAN_RSP:
        {   
            adv_scan_rsp_pdu_prepare();
        } 
        break;
    }
}
//ADV_EVENT_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED
_RAM_CODE
static void adv_event_non_connectable_non_scannable_undirected_packet_prapare(adv_pdu_class_e pduClass)
{
    _u8* packet = NULL;
    switch(pduClass)
    {
        case ADV_PDU_CLASS_LEG:
        {
            adv_non_conn_pdu_prepare();
        }
        break;
    }
}
#if(LL_SUPPORT_LE_EXTENDED_ADVERTISING)
//ADV_EVENT_EXTENDED_CONNECTABLE_DIRECTED
_RAM_CODE
static void adv_event_extended_connectable_directed_packet_prapare(adv_pdu_class_e pduClass)
{
    _u8 flags = 0;
    _u8 advMode = 0;
    _u8 extHeaderLen = 0;
    adv_extended_header_auxInfo_t auxInfo = {0};
    switch(pduClass)
    {
        case ADV_PDU_CLASS_EXT:
        {
            flags   = ADV_EXTENDED_HEADER_FLAG_ADI|ADV_EXTENDED_HEADER_FLAG_AUX_PTR;
            advMode = 1; 
            auxInfo.anchorPoint       = currentSet->la.sch.anchorPoint;
            auxInfo.targetAnchorPoint = currentSet->ea->aux.sch.anchorPoint;
            auxInfo.channel           = currentSet->ea->aux.phy.chn;
            auxInfo.phy               = currentSet->ea->aux.phy.mode;
            adv_ext_ind_pdu_prepare(advMode,flags,&auxInfo,currentSet->ea->did);
        }
        break;
        case ADV_PDU_CLASS_AUX:
        {
            flags = ADV_EXTENDED_HEADER_FLAG_ADV_A|ADV_EXTENDED_HEADER_FLAG_TARGET_A|ADV_EXTENDED_HEADER_FLAG_ADI;
            if(currentSet->ea->power!=0x7f)
            {   
                flags|=ADV_EXTENDED_HEADER_FLAG_TX_POWER;
            }
            advMode = 1; 
            adv_aux_adv_ind_pdu_prepare(advMode,flags,&auxInfo,currentSet->ea->did);
        }
        break;
        case ADV_PDU_CLASS_CONN_RSP:
        {
            flags = ADV_EXTENDED_HEADER_FLAG_ADV_A|ADV_EXTENDED_HEADER_FLAG_TARGET_A;
            adv_aux_conn_rsp_pdu_prepare(advMode,flags,&auxInfo,currentSet->ea->did);
        }
        break;
    }
}

volatile _u8 AAVV_PACKET[50];
//ADV_EVENT_EXTENDED_CONNECTABLE_UNDIRECTED
_RAM_CODE
static void adv_event_extended_connectable_undirected_packet_prapare(adv_pdu_class_e pduClass)
{
    _u8 flags = 0;
    _u8 advMode = 0;
    _u8 extHeaderLen = 0;
    adv_extended_header_auxInfo_t auxInfo = {0};
    switch(pduClass)
    {
        case ADV_PDU_CLASS_EXT:
        {
            flags = ADV_EXTENDED_HEADER_FLAG_ADI|ADV_EXTENDED_HEADER_FLAG_AUX_PTR;
            advMode = 1;
            auxInfo.anchorPoint       = currentSet->la.sch.anchorPoint;
            auxInfo.targetAnchorPoint = currentSet->ea->aux.sch.anchorPoint;
            auxInfo.channel           = currentSet->ea->aux.phy.chn;
            auxInfo.phy               = currentSet->ea->aux.phy.mode;
            adv_ext_ind_pdu_prepare(advMode,flags,&auxInfo,currentSet->ea->did);
        }
        break;
        case ADV_PDU_CLASS_AUX:
        {
            flags = ADV_EXTENDED_HEADER_FLAG_ADV_A|ADV_EXTENDED_HEADER_FLAG_ADI;
            advMode = 1;
            if(currentSet->ea->power!=0x7f)
            {   
                flags|=ADV_EXTENDED_HEADER_FLAG_TX_POWER;
            }
            adv_aux_adv_ind_pdu_prepare(advMode,flags,&auxInfo,currentSet->ea->did);
        }
        break;
        case ADV_PDU_CLASS_CONN_RSP:
        {
            flags = ADV_EXTENDED_HEADER_FLAG_ADV_A|ADV_EXTENDED_HEADER_FLAG_TARGET_A;
            adv_aux_conn_rsp_pdu_prepare(advMode,flags,&auxInfo,currentSet->ea->did);
        }
        break;
    }
}
//ADV_EVENT_EXTENDED_SCANNABLE_DIRECTED
_RAM_CODE
static void adv_event_extended_scannable_directed_packet_prapare(adv_pdu_class_e pduClass)
{
    _u8 flags = 0;
    _u8 advMode = 0;
    _u8 extHeaderLen = 0;
    adv_extended_header_auxInfo_t auxInfo = {0};
    switch(pduClass)
    {
        case ADV_PDU_CLASS_EXT:
        {
            flags = ADV_EXTENDED_HEADER_FLAG_ADI|ADV_EXTENDED_HEADER_FLAG_AUX_PTR;
            advMode = 2;
            auxInfo.anchorPoint       = currentSet->la.sch.anchorPoint;
            auxInfo.targetAnchorPoint = currentSet->ea->aux.sch.anchorPoint;
            auxInfo.channel           = currentSet->ea->aux.phy.chn;
            auxInfo.phy               = currentSet->ea->aux.phy.mode;
            adv_ext_ind_pdu_prepare(advMode,flags,&auxInfo,currentSet->ea->did);
        }
        break;
        case ADV_PDU_CLASS_AUX:
        {
            flags = ADV_EXTENDED_HEADER_FLAG_ADV_A|ADV_EXTENDED_HEADER_FLAG_TARGET_A|ADV_EXTENDED_HEADER_FLAG_ADI;
            if(currentSet->ea->power!=0x7f)
            {   
                flags|=ADV_EXTENDED_HEADER_FLAG_TX_POWER;
            }
            advMode = 2;
            adv_aux_adv_ind_pdu_prepare(advMode,flags,&auxInfo,currentSet->ea->did);
        }
        break;
        case ADV_PDU_CLASS_SCAN_RSP:
        {
            flags = ADV_EXTENDED_HEADER_FLAG_ADV_A|ADV_EXTENDED_HEADER_FLAG_ADI;
            if(currentSet->ea->power!=0x7f)
            {   
                flags|=ADV_EXTENDED_HEADER_FLAG_TX_POWER;
            }
            if(currentSet->pChain->cnt!=0)
            {
                flags|= ADV_EXTENDED_HEADER_FLAG_AUX_PTR;
                auxInfo.anchorPoint       = currentSet->ea->aux.sch.anchorPoint;
                auxInfo.targetAnchorPoint = currentSet->pChain->entry[0].sch.anchorPoint;
                auxInfo.channel           = currentSet->pChain->entry[0].phy.chn;
                auxInfo.phy               = currentSet->pChain->entry[0].phy.mode;
            }
            adv_aux_scan_rsp_pdu_prepare(advMode,flags,&auxInfo,currentSet->ea->did);
        }
        break;
        case ADV_PDU_CLASS_CHAIN:
        {
            flags = ADV_EXTENDED_HEADER_FLAG_ADI;
            if(currentSet->pChain->cnt>(currentSet->pChain->current+1))
            {
                flags|=ADV_EXTENDED_HEADER_FLAG_AUX_PTR;
                auxInfo.anchorPoint       = currentSet->pChain->entry[currentSet->pChain->current].sch.anchorPoint;
                auxInfo.targetAnchorPoint = currentSet->pChain->entry[currentSet->pChain->current+1].sch.anchorPoint;
                auxInfo.channel           = currentSet->pChain->entry[currentSet->pChain->current+1].phy.chn;
                auxInfo.phy               = currentSet->pChain->entry[currentSet->pChain->current+1].phy.mode;
            }
            adv_aux_chain_ind_pdu_prepare(advMode,flags,&auxInfo,currentSet->ea->did);
        }
        break;
    }
}
//ADV_EVENT_EXTENDED_SCANNABLE_UNDIRECTED
_RAM_CODE
static void adv_event_extended_scannable_undirected_packet_prapare(adv_pdu_class_e pduClass)
{
    _u8 flags = 0;
    _u8 advMode = 0;
    _u8 extHeaderLen = 0;
    adv_extended_header_auxInfo_t auxInfo = {0};
    switch(pduClass)
    {
        case ADV_PDU_CLASS_EXT:
        {
            flags = ADV_EXTENDED_HEADER_FLAG_ADI|ADV_EXTENDED_HEADER_FLAG_AUX_PTR;
            advMode = 2;
            auxInfo.anchorPoint       = currentSet->la.sch.anchorPoint;
            auxInfo.targetAnchorPoint = currentSet->ea->aux.sch.anchorPoint;
            auxInfo.channel           = currentSet->ea->aux.phy.chn;
            auxInfo.phy               = currentSet->ea->aux.phy.mode;
            adv_ext_ind_pdu_prepare(advMode,flags,&auxInfo,currentSet->ea->did);
        }
        break;
        case ADV_PDU_CLASS_AUX:
        {
            flags = ADV_EXTENDED_HEADER_FLAG_ADV_A|ADV_EXTENDED_HEADER_FLAG_ADI;
            advMode = 2;
            if(currentSet->ea->power!=0x7f)
            {   
                flags|=ADV_EXTENDED_HEADER_FLAG_TX_POWER;
            }
            adv_aux_adv_ind_pdu_prepare(advMode,flags,&auxInfo,currentSet->ea->did);
        }
        break;
        case ADV_PDU_CLASS_SCAN_RSP:
        {
            flags = ADV_EXTENDED_HEADER_FLAG_ADV_A|ADV_EXTENDED_HEADER_FLAG_ADI;
            if(currentSet->ea->power!=0x7f)
            {   
                flags|=ADV_EXTENDED_HEADER_FLAG_TX_POWER;
            }
            if(currentSet->pChain->cnt!=0)
            {
                flags|= ADV_EXTENDED_HEADER_FLAG_AUX_PTR;
                auxInfo.anchorPoint       = currentSet->ea->aux.sch.anchorPoint;
                auxInfo.targetAnchorPoint = currentSet->pChain->entry[0].sch.anchorPoint;
                auxInfo.channel           = currentSet->pChain->entry[0].phy.chn;
                auxInfo.phy               = currentSet->pChain->entry[0].phy.mode;
            }
            adv_aux_scan_rsp_pdu_prepare(advMode,flags,&auxInfo,currentSet->ea->did);
        }
        break;
        case ADV_PDU_CLASS_CHAIN:
        {
            flags = ADV_EXTENDED_HEADER_FLAG_ADI;
            if(currentSet->pChain->cnt>(currentSet->pChain->current+1))
            {
                flags|=ADV_EXTENDED_HEADER_FLAG_AUX_PTR;
                auxInfo.anchorPoint       = currentSet->pChain->entry[currentSet->pChain->current].sch.anchorPoint;
                auxInfo.targetAnchorPoint = currentSet->pChain->entry[currentSet->pChain->current+1].sch.anchorPoint;
                auxInfo.channel           = currentSet->pChain->entry[currentSet->pChain->current+1].phy.chn;
                auxInfo.phy               = currentSet->pChain->entry[currentSet->pChain->current+1].phy.mode;
            }
            adv_aux_chain_ind_pdu_prepare(advMode,flags,&auxInfo,currentSet->ea->did);
        }
        break;
    }
}
//ADV_EVENT_EXTENDED_NON_CONNECTABLE_NON_SCANNABLE_DIRECTED_WITH_AUXILIARY
_RAM_CODE
static void adv_event_extended_non_connectable_non_scannable_directed_with_auxiliary_packet_prapare(adv_pdu_class_e pduClass)
{
    _u8 flags = 0;
    _u8 advMode = 0;
    _u8 extHeaderLen = 0;
    adv_extended_header_auxInfo_t auxInfo = {0};
    switch(pduClass)
    {
        case ADV_PDU_CLASS_EXT:
        {
            flags = ADV_EXTENDED_HEADER_FLAG_ADI|ADV_EXTENDED_HEADER_FLAG_AUX_PTR;
            auxInfo.anchorPoint       = currentSet->la.sch.anchorPoint;
            auxInfo.targetAnchorPoint = currentSet->ea->aux.sch.anchorPoint;
            auxInfo.channel           = currentSet->ea->aux.phy.chn;
            auxInfo.phy               = currentSet->ea->aux.phy.mode;
            adv_ext_ind_pdu_prepare(advMode,flags,&auxInfo,currentSet->ea->did);
        }
        break;
        case ADV_PDU_CLASS_AUX:
        {
            flags = ADV_EXTENDED_HEADER_FLAG_ADV_A|ADV_EXTENDED_HEADER_FLAG_TARGET_A|ADV_EXTENDED_HEADER_FLAG_ADI;
            if(currentSet->ea->power!=0x7f)
            {   
                flags|=ADV_EXTENDED_HEADER_FLAG_TX_POWER;
            }
            if(currentSet->pChain->cnt!=0)
            {
                flags|=ADV_EXTENDED_HEADER_FLAG_AUX_PTR;
                auxInfo.anchorPoint       = currentSet->ea->aux.sch.anchorPoint;
                auxInfo.targetAnchorPoint = currentSet->pChain->entry[0].sch.anchorPoint;
                auxInfo.channel           = currentSet->pChain->entry[0].phy.chn;
                auxInfo.phy               = currentSet->pChain->entry[0].phy.mode;
            }
            //sync info optional
            adv_aux_adv_ind_pdu_prepare(advMode,flags,&auxInfo,currentSet->ea->did);
        }
        break;
        case ADV_PDU_CLASS_CHAIN:
        {
            flags = ADV_EXTENDED_HEADER_FLAG_ADI;
            if(currentSet->ea->power!=0x7f)
            {   
                flags|=ADV_EXTENDED_HEADER_FLAG_TX_POWER;
            }
            if(currentSet->pChain->cnt>(currentSet->pChain->current+1))
            {
                flags|=ADV_EXTENDED_HEADER_FLAG_AUX_PTR;
                auxInfo.anchorPoint       = currentSet->pChain->entry[currentSet->pChain->current].sch.anchorPoint;
                auxInfo.targetAnchorPoint = currentSet->pChain->entry[currentSet->pChain->current+1].sch.anchorPoint;
                auxInfo.channel           = currentSet->pChain->entry[currentSet->pChain->current+1].phy.chn;
                auxInfo.phy               = currentSet->pChain->entry[currentSet->pChain->current+1].phy.mode;
            }
            adv_aux_chain_ind_pdu_prepare(advMode,flags,&auxInfo,currentSet->ea->did);
        }
        break;
    }
}
//ADV_EVENT_EXTENDED_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED_WITH_AUXILIARY
_RAM_CODE
static void adv_event_extended_non_connectable_non_scannable_undirected_with_auxiliary_packet_prapare(adv_pdu_class_e pduClass)
{
    _u8 flags = 0;
    _u8 advMode = 0;
    _u8 extHeaderLen = 0;
    adv_extended_header_auxInfo_t auxInfo = {0};
    switch(pduClass)
    {
        case ADV_PDU_CLASS_EXT:
        {
            flags = ADV_EXTENDED_HEADER_FLAG_ADI|ADV_EXTENDED_HEADER_FLAG_AUX_PTR;
            auxInfo.anchorPoint       = currentSet->la.sch.anchorPoint;
            auxInfo.targetAnchorPoint = currentSet->ea->aux.sch.anchorPoint;
            auxInfo.channel           = currentSet->ea->aux.phy.chn;
            auxInfo.phy               = currentSet->ea->aux.phy.mode;
            adv_ext_ind_pdu_prepare(advMode,flags,&auxInfo,currentSet->ea->did);
        }
        break;
        case ADV_PDU_CLASS_AUX:
        {
            flags = ADV_EXTENDED_HEADER_FLAG_ADV_A|ADV_EXTENDED_HEADER_FLAG_ADI;
            if(currentSet->ea->power!=0x7f)
            {   
                flags|=ADV_EXTENDED_HEADER_FLAG_TX_POWER;
            }
            if(currentSet->pChain->cnt!=0)
            {
                flags|=ADV_EXTENDED_HEADER_FLAG_AUX_PTR;
                auxInfo.anchorPoint       = currentSet->ea->aux.sch.anchorPoint;
                auxInfo.targetAnchorPoint = currentSet->pChain->entry[0].sch.anchorPoint;
                auxInfo.channel           = currentSet->pChain->entry[0].phy.chn;
                auxInfo.phy               = currentSet->pChain->entry[0].phy.mode;
            }   
            //sync info optional
            adv_aux_adv_ind_pdu_prepare(advMode,flags,&auxInfo,currentSet->ea->did);
        }
        break;
        case ADV_PDU_CLASS_CHAIN:
        {
            flags = ADV_EXTENDED_HEADER_FLAG_ADI;
            if(currentSet->ea->power!=0x7f)
            {   
                flags|=ADV_EXTENDED_HEADER_FLAG_TX_POWER;
            }
            if(currentSet->pChain->cnt>(currentSet->pChain->current+1))
            {
                flags|=ADV_EXTENDED_HEADER_FLAG_AUX_PTR;
                auxInfo.anchorPoint       = currentSet->pChain->entry[currentSet->pChain->current].sch.anchorPoint;
                auxInfo.targetAnchorPoint = currentSet->pChain->entry[currentSet->pChain->current+1].sch.anchorPoint;
                auxInfo.channel           = currentSet->pChain->entry[currentSet->pChain->current+1].phy.chn;
                auxInfo.phy               = currentSet->pChain->entry[currentSet->pChain->current+1].phy.mode;
            }
            adv_aux_chain_ind_pdu_prepare(advMode,flags,&auxInfo,currentSet->ea->did);
        }
        break;
    }
}
//ADV_EVENT_EXTENDED_NON_CONNECTABLE_NON_SCANNABLE_DIRECTED_WITHOUT_AUXILIARY
_RAM_CODE
static void adv_event_extended_non_connectable_non_scannable_directed_without_auxiliary_packet_prapare(adv_pdu_class_e pduClass)
{
    _u8 flags = 0;
    _u8 advMode = 0;
    _u8 extHeaderLen = 0;
    adv_extended_header_auxInfo_t auxInfo = {0};
    switch(pduClass)
    {
        case ADV_PDU_CLASS_EXT:
        {
            flags = ADV_EXTENDED_HEADER_FLAG_ADV_A|ADV_EXTENDED_HEADER_FLAG_TARGET_A;
            if(currentSet->ea->power!=0x7f)
            {   
                flags|=ADV_EXTENDED_HEADER_FLAG_TX_POWER;
            }
            adv_ext_ind_pdu_prepare(advMode,flags,&auxInfo,currentSet->ea->did);
        }
    }
}
//ADV_EVENT_EXTENDED_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED_WITHOUT_AUXILIARY
_RAM_CODE
static void adv_event_extended_non_connectable_non_scannable_undirected_without_auxiliary_packet_prapare(adv_pdu_class_e pduClass)
{
    _u8 flags = 0;
    _u8 advMode = 0;
    _u8 extHeaderLen = 0;
    adv_extended_header_auxInfo_t auxInfo = {0};
    switch(pduClass)
    {
        case ADV_PDU_CLASS_EXT:
        {
            flags = ADV_EXTENDED_HEADER_FLAG_ADV_A;
            if(currentSet->ea->power!=0x7f)
            {   
                flags|=ADV_EXTENDED_HEADER_FLAG_TX_POWER;
            }
            adv_ext_ind_pdu_prepare(advMode,flags,&auxInfo,currentSet->ea->did);
        }
    }
}
#endif
#if(LL_SUPPORT_LE_PERIODIC_ADVERTISING)
//ADV_EVENT_EXTENDED_PERIODIC
_RAM_CODE
static void adv_event_extended_periodic_packet_prapare(adv_pdu_class_e pduClass)
{
    _u8 flags = 0;
    _u8 advMode = 0;
    _u8 extHeaderLen = 0;
    adv_extended_header_auxInfo_t auxInfo = {0};
    switch(pduClass)
    {
        case ADV_PDU_CLASS_SYNC:
        {
        	if(currentSet->pa->includeAdi)
        	{
                flags |= ADV_EXTENDED_HEADER_FLAG_ADI;
        	}
        	if(currentSet->pa->includeTxPower)
        	{
                flags |= ADV_EXTENDED_HEADER_FLAG_TX_POWER;
        	}
        	if(currentSet->schMap&ADV_SCH_MAP_PA_CHAIN)
        	{
        		flags |= ADV_EXTENDED_HEADER_FLAG_AUX_PTR;
        		auxInfo.anchorPoint       = currentSet->pa->sync.sch.anchorPoint;
        		auxInfo.targetAnchorPoint = currentSet->pa->chain.entry[0].sch.anchorPoint;
          		auxInfo.phy               = currentSet->pa->chain.entry[0].phy.mode;
          		auxInfo.channel           = currentSet->pa->chain.entry[0].phy.chn;
        	}
        	adv_aux_sync_ind_pdu_prepare(advMode,flags,&auxInfo,currentSet->pa->did);
        }
    }
}
#endif

#if(LL_SUPPORT_PERIODIC_ADVERTISING_WITH_RESPONSES_ADVERTISER)
//ADV_EVENT_EXTENDED_PERIODIC_WITH_RESPONSE
_RAM_CODE
static void adv_event_extended_periodic_with_response_packet_prapare(adv_pdu_class_e pduClass)
{
    
}
#endif

void(*adv_prepare_packet[])(adv_pdu_class_e pduClass)  =
{
    adv_event_connectable_scannable_undirected_packet_prapare,
    adv_event_connectable_directed_packet_prapare,
    adv_event_scannable_undirected_packet_prapare,
    adv_event_non_connectable_non_scannable_undirected_packet_prapare,
    #if(LL_SUPPORT_LE_EXTENDED_ADVERTISING)
    adv_event_extended_connectable_directed_packet_prapare,
    adv_event_extended_connectable_undirected_packet_prapare,
    adv_event_extended_scannable_directed_packet_prapare,
    adv_event_extended_scannable_undirected_packet_prapare,
    adv_event_extended_non_connectable_non_scannable_directed_with_auxiliary_packet_prapare,
    adv_event_extended_non_connectable_non_scannable_undirected_with_auxiliary_packet_prapare,
    adv_event_extended_non_connectable_non_scannable_directed_without_auxiliary_packet_prapare,
    adv_event_extended_non_connectable_non_scannable_undirected_without_auxiliary_packet_prapare,
    #endif
    #if(LL_SUPPORT_LE_PERIODIC_ADVERTISING)
    adv_event_extended_periodic_packet_prapare,
    #endif
    #if(LL_SUPPORT_PERIODIC_ADVERTISING_WITH_RESPONSES_ADVERTISER)
    adv_event_extended_periodic_with_response_packet_prapare,
    #endif
};

/********* ADV EVENT PROCESS *********/
_RAM_CODE
static int adv_event_step_phy_send_advertising(void)
{
    if(currentSet->la.availableChnCnt==0||currentSet->enable == 0)
    {
        return 0;
    }
    phy_obj_cast(&LLSM->phy);
    currentSet->la.eventCnt++;
    currentSet->la.phy.chn = currentSet->la.chnTable[(currentSet->la.eventCnt%currentSet->la.channelCnt)];
    #if(LL_SUPPORT_LE_EXTENDED_ADVERTISING)
    if(POINTER_VALID(currentSet->ea))
    {
        if(currentSet->eventProperty&LL_ADV_EVENT_PROPERTY_LEGACY_PDU)
        {
            adv_prepare_packet[currentSet->eventType](ADV_PDU_CLASS_LEG);
        } 
        else
        {
            adv_prepare_packet[currentSet->eventType](ADV_PDU_CLASS_EXT);
        }
    }
    else
    {
        adv_prepare_packet[currentSet->eventType](ADV_PDU_CLASS_LEG); 
    }
    #else
    adv_prepare_packet[currentSet->eventType](ADV_PDU_CLASS_LEG);
    #endif
    adv_prepare_phy(&currentSet->la.phy,currentSet->la.sch.anchorPoint,PHY_DIR_TX);
    LLSM->phy.start();
    return 1;
}
_RAM_CODE
static int adv_event_step_phy_start_listen(void)
{
    phy_obj_cast(&LLSM->phy);
    //packet data length is 6 mac address plus additional data length.
    _u32 lastAirPacketTime = ll_get_air_packet_time(currentSet->la.phy.mode,6+currentSet->data.len,0);
    _u32 receiveWindowStart = currentSet->la.sch.anchorPoint + lastAirPacketTime + PACKET_DEFAULT_TIFS_TIME;
    _u32 receiveWindowEnd   = receiveWindowStart + BLE_ADV_DEFAULT_RX_TIMEOUT_US;
	_u32 timestamp = receiveWindowStart - ll_ca_cal_window_winden(LL_CA_TYPE_ACTIVE,0,ll_get_local_ppm(),currentSet->la.sch.anchorPoint,receiveWindowEnd);
	adv_prepare_phy(&currentSet->la.phy,timestamp,PHY_DIR_RX);
	LLSM->phy.start();
	return 1;
}

_RAM_CODE
static int adv_switch_to_conn_state(ll_adv_packet_t* packet)
{
	// //connect ind process
	// init_type_connectInd_t* connInd = (init_type_connectInd_t*)(packet->data);
    // if(POINTER_NOT_VALID(ll->conn))
    // {
    //     ll->conn = (ll_internal_connection_ctrl_t*)tx_malloc(sizeof(ll_internal_connection_ctrl_t));
    // }
	// //ll state machine transform
    // if(ble_ll_process_event(ll,BLE_LL_EVENT_START_CONNECTION) == BLE_LL_STATE_SUCCESS)
    // {
    //     init_type_ll_data_t* llData = (init_type_ll_data_t*)(connInd->llData);
    //     ll->conn->peer.sca = llData->sca;
    //     ll->conn->timeout  = llData->timeout;
    //     ll->conn->latency  = llData->latency;
    //     ll->conn->csa.hop  = llData->hop;
    //     if(packet->hdr.chSel && LL_CHANNEL_SUPP_CSA2)
    //     {
    //     	ll->conn->csa.mode = LL_CSA_2;
    //     }
    //     else
    //     {
    //     	ll->conn->csa.mode = LL_CSA_1;
    //     }
    //     ll->conn->csa.counter = 0;
    //     txMemcpy(ll->conn->csa.map,llData->chnMap,5);
    //     ll_csa_init(&ll->conn->csa);
    //     //transmit window delay is 1.25ms when connect ind used
    //     _u32 connIndAnchor = ll->phy.hw_get_rx_air_ts() + ll_get_air_packet_time(advParam->la.phy.mode,sizeof(init_type_connectInd_t),0);
    //     ll->conn->anchor   = connIndAnchor +1250+llData->winOffset*1250;
    //     ll->conn->duration = llData->winSize*1250;
    //     ll->conn->interval = llData->interval*1250;

    //     ll->sch.timestamp  = ll->conn->anchor;
    //     ll->sch.duration   = ll->sch.durationMin = ll->conn->duration;
    //     ll->sch.period     = ll->conn->interval;
    //     ll->sch.startLatency = 100;
    //     ll->sch.stopLatency  = 100;
    //     ll->phy.mode       = PHY_MODE_1M;
    //     ll->phy.accessCode = llData->AA;
    //     ll->phy.crcInit    = llData->crcInit;
    //     sch_abort_current_and_process_next_task();
	//     return 1;//switch to conn state success
    // }
    // else
    // {
        // tx_free((_u8*)ll->conn);
    // }
}

_RAM_CODE
static int adv_event_step_phy_send_scan_rsp(void)
{
	if(LLSM->phy.hw_is_rx_packet_valid())//packet analyze shall be execute first
	{
		ll_adv_packet_t* packet = (ll_adv_packet_t*)(LLSM->phy.rxAddress + ll_get_packet_header_offset_from_address(PHY_DIR_RX));
		if(packet->hdr.pduType == LL_ADV_TYPE_SCAN_REQ && packet->hdr.length == sizeof(scan_type_scan_req_t))
		{
			 scan_type_scan_req_t* scanReq = (scan_type_scan_req_t*)(packet->data);
			 if((txMemcmp(ll_get_device_address(),scanReq->advA,6) == 0))
			 {
				adv_prepare_packet[currentSet->eventType](ADV_PDU_CLASS_SCAN_RSP);
				_u32 timestamp = LLSM->phy.hw_get_rx_air_ts() + ll_get_air_packet_time(currentSet->la.phy.mode,sizeof(scan_type_scan_req_t),0)+PACKET_DEFAULT_TIFS_TIME;
				adv_prepare_phy(&currentSet->la.phy,timestamp,PHY_DIR_TX);
				LLSM->phy.start();
				return 1;
			 }
		}
		else if(packet->hdr.pduType == LL_ADV_TYPE_CONNECT_IND && packet->hdr.length == sizeof(init_type_connectInd_t))
		{
			if(adv_switch_to_conn_state(packet))
			{
				return 1;
			}
		}
	}
	return 0;
}

_RAM_CODE
static int adv_event_step_default_process(void)
{
	LLSM->phy.stop();
	return 1;
}

_RAM_CODE
static int adv_event_step_sch_stop(void)
{
	LLSM->phy.stop();
    if(currentSet->la.availableChnCnt)
    {
    	currentSet->la.availableChnCnt--;
    }
    if(currentSet->la.availableChnCnt)
    {
        currentSet->la.sch.anchorPoint += (currentSet->la.sch.duration+currentSet->la.sch.stopMargin + currentSet->la.sch.startMargin);
    }
    else
    {
        currentSet->la.availableChnCnt = currentSet->la.channelCnt;
        currentSet->la.sch.anchorPoint += (currentSet->la.sch.interval + 30*(random_byte()|0x0f));
        #if(LL_SUPPORT_LE_EXTENDED_ADVERTISING)
        if((!(currentSet->schMap&ADV_SCH_MAP_AUX))\
          ||((currentSet->schMap&ADV_SCH_MAP_AUX)&&txCompareTime(currentSet->ea->anchor,currentSet->la.sch.anchorPoint+currentSet->la.sch.interval)))
        #endif
        {
            ll_adv_task_timing_allocation(currentSet,currentSet->la.sch.anchorPoint,currentSet->la.sch.anchorPoint+currentSet->la.sch.interval,ADV_SCH_MAP_PRI);
        }
    }
    return 1;
}

_RAM_CODE
static int adv_event_step_sch_passed(void)
{
     _u32 systemTime = system_time();
     _u32 periodicDiff = (systemTime - currentSet->la.sch.anchorPoint)/currentSet->la.sch.interval;
     currentSet->la.sch.anchorPoint += (periodicDiff+1)*currentSet->la.sch.interval;
     currentSet->la.availableChnCnt = currentSet->la.channelCnt;
     #if(LL_SUPPORT_LE_EXTENDED_ADVERTISING)
     if((!(currentSet->schMap&ADV_SCH_MAP_AUX))||((currentSet->schMap&ADV_SCH_MAP_AUX)&&txCompareTime(currentSet->ea->anchor,currentSet->la.sch.anchorPoint+currentSet->la.sch.interval)))
     #endif
     {
         ll_adv_task_timing_allocation(currentSet,currentSet->la.sch.anchorPoint,currentSet->la.sch.anchorPoint+currentSet->la.sch.interval,ADV_SCH_MAP_PRI);
     }
     return 1;
}



_RAM_CODE
static int adv_event_step_sch_canceled(void)
{
    _u32 systemTime = system_time();
    _u32 periodicDiff = (systemTime - currentSet->la.sch.anchorPoint)/currentSet->la.sch.interval;
    currentSet->la.sch.anchorPoint += (periodicDiff+1)*currentSet->la.sch.interval;
    currentSet->la.availableChnCnt = currentSet->la.channelCnt;

    #if(LL_SUPPORT_LE_EXTENDED_ADVERTISING)
    if((!(currentSet->schMap&ADV_SCH_MAP_AUX))||((currentSet->schMap&ADV_SCH_MAP_AUX)&&txCompareTime(currentSet->ea->anchor,currentSet->la.sch.anchorPoint+currentSet->la.sch.interval)))
    #endif
    {
        ll_adv_task_timing_allocation(currentSet,currentSet->la.sch.anchorPoint,currentSet->la.sch.anchorPoint+currentSet->la.sch.interval,ADV_SCH_MAP_PRI);
    }
    return 1;
}

_DATA
static adv_event_sm_t adv_event_state_machine[]=
{
    {adv_event_step_phy_send_advertising, ADV_CONTEXT_DEFAULT|ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE,ADV_SM_STATE_IDLE,       ADV_SM_STATE_SENDING_ADV, ADV_SM_STATE_IDLE, LL_SCH_EVENT_START},
    {adv_event_step_sch_stop,             ADV_CONTEXT_DEFAULT|ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE,ADV_SM_STATE_IDLE,       ADV_SM_STATE_IDLE,        ADV_SM_STATE_IDLE, LL_SCH_EVENT_STOP},
    {adv_event_step_sch_canceled,         ADV_CONTEXT_DEFAULT|ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE,ADV_SM_STATE_IDLE,       ADV_SM_STATE_IDLE,        ADV_SM_STATE_IDLE, LL_SCH_EVENT_CANCELED},
    {adv_event_step_sch_passed,           ADV_CONTEXT_DEFAULT|ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE,ADV_SM_STATE_IDLE,       ADV_SM_STATE_IDLE,        ADV_SM_STATE_IDLE, LL_SCH_EVENT_PASSED},

    {adv_event_step_phy_start_listen,     ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE,                    ADV_SM_STATE_SENDING_ADV,ADV_SM_STATE_RECEIVING,   ADV_SM_STATE_IDLE, LL_PHY_EVENT_SEND_FINISHED},
    {adv_event_step_sch_stop,             ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE,                    ADV_SM_STATE_SENDING_ADV,ADV_SM_STATE_IDLE,        ADV_SM_STATE_IDLE, LL_SCH_EVENT_STOP},
    {adv_event_step_default_process,      ADV_CONTEXT_DEFAULT,                                              ADV_SM_STATE_SENDING_ADV,ADV_SM_STATE_IDLE,        ADV_SM_STATE_IDLE, LL_PHY_EVENT_SEND_FINISHED},

    {adv_event_step_phy_send_scan_rsp,    ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE,                    ADV_SM_STATE_RECEIVING,  ADV_SM_STATE_SENDING_RSP, ADV_SM_STATE_IDLE, LL_PHY_EVENT_RECEIVE_FINISHED},
    {adv_event_step_default_process,      ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE,                    ADV_SM_STATE_RECEIVING,  ADV_SM_STATE_IDLE,        ADV_SM_STATE_IDLE, LL_PHY_EVENT_RECEIVE_TIMEOUT},
	{adv_event_step_sch_stop,             ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE,                    ADV_SM_STATE_RECEIVING,  ADV_SM_STATE_IDLE,        ADV_SM_STATE_IDLE, LL_SCH_EVENT_STOP},

    {adv_event_step_default_process,      ADV_CONTEXT_SCANNABLE,                                            ADV_SM_STATE_SENDING_RSP,ADV_SM_STATE_IDLE,        ADV_SM_STATE_IDLE, LL_PHY_EVENT_SEND_FINISHED},
    {adv_event_step_sch_stop,             ADV_CONTEXT_SCANNABLE,                                            ADV_SM_STATE_SENDING_RSP,ADV_SM_STATE_IDLE,        ADV_SM_STATE_IDLE, LL_SCH_EVENT_STOP},
};

static adv_procedure_list_t adv_con_scan_undirected_procedure[] =
{
    {ADV_EVENT,adv_event_state_machine,ADV_SM_LIST_LENGTH(adv_event_state_machine),ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE},
};

static adv_procedure_list_t adv_con_directed_procedure[]=
{
    {ADV_EVENT,adv_event_state_machine,ADV_SM_LIST_LENGTH(adv_event_state_machine),ADV_CONTEXT_CONNECTABLE},
};

static adv_procedure_list_t adv_scan_undirected_procedure[]=
{
    {ADV_EVENT,adv_event_state_machine,ADV_SM_LIST_LENGTH(adv_event_state_machine),ADV_CONTEXT_SCANNABLE},
};

static adv_procedure_list_t adv_non_con_non_scan_undirected_procedure[]=
{
    {ADV_EVENT,adv_event_state_machine,ADV_SM_LIST_LENGTH(adv_event_state_machine),ADV_CONTEXT_DEFAULT},
};

/********* ADV EXTENDED EVENT PROCESS *********/
#if(LL_SUPPORT_LE_EXTENDED_ADVERTISING)

ll_internal_adv_set_t* ll_extended_adv_get_adv_set(_u8 handle,_u8 allocate)
{
	ll_sm_t* llsm = (ll_sm_t*)ll_get_sm_entity_by_state(BLE_LL_STATE_ADVERTISING,LL_SM_INVALID_HANDLE,0);
	if(POINTER_NOT_VALID(llsm)||POINTER_NOT_VALID(llsm->entity))
	{
		return NULL;
	}
	ll_internal_adv_ctrl_t* adv = (ll_internal_adv_ctrl_t*)llsm->entity;

	for(int i=0;i<BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS;i++)
	{
		if(adv->set[i].handle == handle)
		{
			return &adv->set[i];
		}
	}
	if(allocate==0)
	{
		return NULL;
	}

	for(int i=0;i<BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS;i++)
	{
        if(adv->set[i].handle == LL_EXTENDED_ADV_INVALID_HANDLE)
        {
        	adv->set[i].handle = handle;
            return &adv->set[i];
        }
    }
    return NULL;
}

int ll_extended_adv_get_current_active_set_number(void)
{
	ll_sm_t* llsm = (ll_sm_t*)ll_get_sm_entity_by_state(BLE_LL_STATE_ADVERTISING,LL_SM_INVALID_HANDLE,0);
	if(POINTER_NOT_VALID(llsm)||POINTER_NOT_VALID(llsm->entity))
	{
		return 0;
	}
	ll_internal_adv_ctrl_t* adv = (ll_internal_adv_ctrl_t*)llsm->entity;
    _u8 count = 0;
	for(int i=0;i<BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS;i++)
	{
        if((adv->set[i].handle!=LL_EXTENDED_ADV_INVALID_HANDLE && adv->set[i].enable == 1)
			#if(LL_SUPPORT_LE_PERIODIC_ADVERTISING)
			||(POINTER_VALID(adv->pa)&&adv->pa.enable)
			#endif
          )
        {
            count++;
        }

    }
    return count;
}

int ll_extended_adv_get_current_set_number(void)
{
	ll_sm_t* llsm = (ll_sm_t*)ll_get_sm_entity_by_state(BLE_LL_STATE_ADVERTISING,LL_SM_INVALID_HANDLE,0);
	if(POINTER_NOT_VALID(llsm)||POINTER_NOT_VALID(llsm->entity))
	{
		return 0;
	}
	ll_internal_adv_ctrl_t* adv = (ll_internal_adv_ctrl_t*)llsm->entity;
    _u8 count = 0;
	for(int i=0;i<BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS;i++)
	{
        if(adv->set[i].handle!=LL_EXTENDED_ADV_INVALID_HANDLE)
        {
            count++;
        }
    }
    return count;
}

static int adv_extended_event_step_phy_send_aux_advertising(void)
{
    phy_obj_cast(&LLSM->phy);
    //prepare packet
    adv_prepare_packet[currentSet->eventType](ADV_PDU_CLASS_AUX);
    currentSet->ea->aux.phy.chn = 35;//todo
    //prepare phy
    adv_prepare_phy(&currentSet->ea->aux.phy,currentSet->ea->aux.sch.anchorPoint,PHY_DIR_TX);
    LLSM->phy.start();
	return 1;
}
static int adv_extended_event_step_phy_start_listen_aux(void)
{
    phy_obj_cast(&LLSM->phy);
    // _u32 timestamp = advParam->ea->sch.anchor + ll_get_air_packet_time(advParam->la.phy.mode,6+advParam->data.len,0)+PACKET_DEFAULT_TIFS_TIME;
	adv_prepare_phy(&currentSet->ea->aux.phy,0,PHY_DIR_RX);
	LLSM->phy.start();
	return 1;
}
static int adv_extended_event_step_phy_send_aux_scan_rsp(void)
{
	if(LLSM->phy.hw_is_rx_packet_valid())
	{
		ll_adv_packet_t* packet = (ll_adv_packet_t*)(currentSet->ea->aux.phy.rxAddress + ll_get_packet_header_offset_from_address(PHY_DIR_RX));
		if((packet->hdr.pduType == LL_ADV_TYPE_AUX_SCAN_REQ)&&(packet->hdr.length == sizeof(scan_type_aux_scan_req_t)))
		{
			//scan req process
			scan_type_aux_scan_req_t* scanReq = (scan_type_aux_scan_req_t*)(packet->data);
			if((currentSet->scanRsp.len!=0)&&(txMemcmp(ll_get_device_address(),scanReq->advA,6) == 0))
			{
				adv_prepare_packet[currentSet->eventType](ADV_PDU_CLASS_SCAN_RSP);
				_u32 timestamp = LLSM->phy.hw_get_rx_air_ts() + ll_get_air_packet_time(currentSet->ea->aux.phy.mode,sizeof(scan_type_aux_scan_req_t),0)+PACKET_DEFAULT_TIFS_TIME;
				adv_prepare_phy(&currentSet->ea->aux.phy,timestamp,PHY_DIR_TX);
				LLSM->phy.start();
				return 1;
	        }
		}
        else if((packet->hdr.pduType == LL_ADV_TYPE_CONNECT_IND)&&(packet->hdr.length == sizeof(init_type_connectInd_t)))
        {

        }
	}

	return 0;
}
static int adv_extended_event_step_phy_send_aux_connect_rsp(void)
{
	if(LLSM->phy.hw_is_rx_packet_valid())
	{
		ll_adv_packet_t* packet = (ll_adv_packet_t*)(currentSet->ea->aux.phy.rxAddress + ll_get_packet_header_offset_from_address(PHY_DIR_RX));
		if((packet->hdr.pduType == LL_ADV_TYPE_AUX_CONNECT_REQ)&&(packet->hdr.length == sizeof(init_type_auxConnectReq_t)))
		{
			//scan req process
			init_type_auxConnectReq_t* scanReq = (init_type_auxConnectReq_t*)(packet->data);
//			if(txMemcmp(ll_get_device_address(),scanReq->advA,6) == 0)
//			{
				adv_prepare_packet[currentSet->eventType](ADV_PDU_CLASS_CONN_RSP);
				_u32 timestamp = LLSM->phy.hw_get_rx_air_ts() + ll_get_air_packet_time(currentSet->ea->aux.phy.mode,sizeof(init_type_auxConnectReq_t),0)+PACKET_DEFAULT_TIFS_TIME;
				adv_prepare_phy(&currentSet->ea->aux.phy,timestamp,PHY_DIR_TX);
				LLSM->phy.start();
				return 1;
//	        }
		}
	}
	return 0;
}
static int adv_extended_event_step_default_process(void)
{
	return 1;
}
static int adv_extended_event_step_sch_stop(void)
{
    if(currentSet->state == ADV_SM_STATE_SENDING_AUX_SCAN_RSP)
    {
        if(currentSet->schMap&ADV_SCH_MAP_AUX_CHAIN)
        {
            return 1; 
        }
    }
    else if(currentSet->schMap&ADV_SCH_MAP_AUX_CHAIN)
    {
    	currentSet->ea->anchor = currentSet->ea->chain.entry[0].sch.anchorPoint;
        return 1;
    }

    ll_adv_task_timing_allocation(currentSet,currentSet->la.sch.anchorPoint,currentSet->la.sch.anchorPoint+currentSet->la.sch.interval,ADV_SCH_MAP_PRI|ADV_SCH_MAP_AUX);
	return 1;
}
static int adv_extended_event_step_sch_canceled(void)
{
    ll_adv_task_timing_allocation(currentSet,currentSet->la.sch.anchorPoint,currentSet->la.sch.anchorPoint+currentSet->la.sch.interval,ADV_SCH_MAP_ALL);
    //todo
	return 1;
}
static int adv_extended_event_step_sch_passed(void)
{
    ll_adv_task_timing_allocation(currentSet,currentSet->la.sch.anchorPoint,currentSet->la.sch.anchorPoint+currentSet->la.sch.interval,ADV_SCH_MAP_ALL);
    //todo
	return 1;
}

_DATA
static adv_event_sm_t adv_extended_event_state_machine[]= 
{
    {adv_extended_event_step_phy_send_aux_advertising,  ADV_CONTEXT_AUXILIARY|ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE,ADV_SM_STATE_IDLE,                  ADV_SM_STATE_SENDING_AUX_ADV,        ADV_SM_STATE_IDLE, LL_SCH_EVENT_START},
    {adv_extended_event_step_sch_stop,                  ADV_CONTEXT_AUXILIARY|ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE,ADV_SM_STATE_IDLE,                  ADV_SM_STATE_IDLE,                   ADV_SM_STATE_IDLE, LL_SCH_EVENT_STOP},
    {adv_extended_event_step_sch_canceled,              ADV_CONTEXT_AUXILIARY|ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE,ADV_SM_STATE_IDLE,                  ADV_SM_STATE_IDLE,                   ADV_SM_STATE_IDLE, LL_SCH_EVENT_CANCELED},
    {adv_extended_event_step_sch_passed,                ADV_CONTEXT_AUXILIARY|ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE,ADV_SM_STATE_IDLE,                  ADV_SM_STATE_IDLE,                   ADV_SM_STATE_IDLE, LL_SCH_EVENT_PASSED},

    {adv_extended_event_step_phy_start_listen_aux,      ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE,                      ADV_SM_STATE_SENDING_AUX_ADV,       ADV_SM_STATE_RECEIVING_AUX,          ADV_SM_STATE_IDLE, LL_PHY_EVENT_SEND_FINISHED},
    {adv_extended_event_step_sch_stop,                  ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE,                      ADV_SM_STATE_SENDING_AUX_ADV,       ADV_SM_STATE_IDLE,                   ADV_SM_STATE_IDLE, LL_SCH_EVENT_STOP},
    {adv_extended_event_step_default_process,           ADV_CONTEXT_AUXILIARY,                                              ADV_SM_STATE_SENDING_AUX_ADV,       ADV_SM_STATE_IDLE,                   ADV_SM_STATE_IDLE, LL_PHY_EVENT_SEND_FINISHED},
    {adv_extended_event_step_default_process,           ADV_CONTEXT_AUXILIARY,                                              ADV_SM_STATE_SENDING_AUX_ADV,       ADV_SM_STATE_IDLE,                   ADV_SM_STATE_IDLE, LL_PHY_EVENT_SEND_FINISHED},

    {adv_extended_event_step_phy_send_aux_scan_rsp,     ADV_CONTEXT_CONNECTABLE,                                            ADV_SM_STATE_RECEIVING_AUX,         ADV_SM_STATE_SENDING_AUX_SCAN_RSP,   ADV_SM_STATE_IDLE, LL_PHY_EVENT_RECEIVE_FINISHED},
    {adv_extended_event_step_phy_send_aux_connect_rsp,  ADV_CONTEXT_SCANNABLE,                                              ADV_SM_STATE_RECEIVING_AUX,         ADV_SM_STATE_SENDING_AUX_CONNECT_RSP,ADV_SM_STATE_IDLE, LL_PHY_EVENT_RECEIVE_FINISHED},
    {adv_extended_event_step_sch_stop,                  ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE,                      ADV_SM_STATE_RECEIVING_AUX,         ADV_SM_STATE_IDLE,                   ADV_SM_STATE_IDLE, LL_SCH_EVENT_STOP},
};


static int adv_chained_event_step_phy_send_chain_advertising(void)
{
    phy_obj_cast(&LLSM->phy);
    //prepare packet
    adv_prepare_packet[currentSet->eventType](ADV_PDU_CLASS_CHAIN);
    currentSet->pChain->entry[currentSet->pChain->current].phy.chn = 35;//todo
    //prepare phy
    adv_prepare_phy(&currentSet->pChain->entry[currentSet->pChain->current].phy,currentSet->pChain->entry[currentSet->pChain->current].sch.anchorPoint,PHY_DIR_TX);
    LLSM->phy.start();
	return 1;
}

static int adv_chained_event_step_sch_stop(void)
{
    LLSM->phy.stop();
    currentSet->pChain->current++;
    if(currentSet->pChain->current==currentSet->pChain->cnt)
    {
    	currentSet->pChain->current = 0;
        ll_adv_task_timing_allocation(currentSet,currentSet->la.sch.anchorPoint,currentSet->la.sch.anchorPoint+currentSet->la.sch.interval,ADV_SCH_MAP_PRI|ADV_SCH_MAP_AUX|ADV_SCH_MAP_AUX_CHAIN);
    }
    else
    {
        currentSet->ea->anchor = currentSet->ea->chain.entry[currentSet->pChain->current].sch.anchorPoint;
    }
}
static int adv_chained_event_step_default_process(void)
{
	return 1;
}
static int adv_chained_event_step_sch_canceled(void)
{
    ll_adv_task_timing_allocation(currentSet,currentSet->la.sch.anchorPoint,currentSet->la.sch.anchorPoint+currentSet->la.sch.interval,ADV_SCH_MAP_PRI|ADV_SCH_MAP_AUX|ADV_SCH_MAP_AUX_CHAIN);
}

static int adv_chained_event_step_sch_passed(void)
{
    ll_adv_task_timing_allocation(currentSet,currentSet->la.sch.anchorPoint,currentSet->la.sch.anchorPoint+currentSet->la.sch.interval,ADV_SCH_MAP_PRI|ADV_SCH_MAP_AUX|ADV_SCH_MAP_AUX_CHAIN);
}

_DATA
static adv_event_sm_t adv_chained_event_state_machine[] =
{
	{adv_chained_event_step_phy_send_chain_advertising,ADV_CONTEXT_CHAINED,ADV_SM_STATE_IDLE,             ADV_SM_STATE_SENDING_CHAIN_ADV,ADV_SM_STATE_IDLE,LL_SCH_EVENT_START},
	{adv_chained_event_step_sch_stop,                  ADV_CONTEXT_CHAINED,ADV_SM_STATE_IDLE,             ADV_SM_STATE_IDLE,             ADV_SM_STATE_IDLE,LL_SCH_EVENT_STOP},
	{adv_chained_event_step_sch_canceled,              ADV_CONTEXT_CHAINED,ADV_SM_STATE_IDLE,             ADV_SM_STATE_IDLE,             ADV_SM_STATE_IDLE,LL_SCH_EVENT_CANCELED},
	{adv_chained_event_step_sch_passed,                ADV_CONTEXT_CHAINED,ADV_SM_STATE_IDLE,             ADV_SM_STATE_IDLE,             ADV_SM_STATE_IDLE,LL_SCH_EVENT_PASSED},

	{adv_chained_event_step_default_process,           ADV_CONTEXT_CHAINED,ADV_SM_STATE_SENDING_CHAIN_ADV,ADV_SM_STATE_IDLE,             ADV_SM_STATE_IDLE,LL_PHY_EVENT_SEND_FINISHED},
	{adv_chained_event_step_sch_stop,                  ADV_CONTEXT_CHAINED,ADV_SM_STATE_SENDING_CHAIN_ADV,ADV_SM_STATE_IDLE,             ADV_SM_STATE_IDLE,LL_SCH_EVENT_STOP},
};

static adv_procedure_list_t adv_extended_con_undirected_procedure[]=
{
    {ADV_EVENT,         adv_event_state_machine,         ADV_SM_LIST_LENGTH(adv_event_state_machine),         ADV_CONTEXT_DEFAULT},
    {ADV_EXTENDED_EVENT,adv_extended_event_state_machine,ADV_SM_LIST_LENGTH(adv_extended_event_state_machine),ADV_CONTEXT_CONNECTABLE},
};

static adv_procedure_list_t adv_extended_con_directed_procedure[]=
{
    {ADV_EVENT,         adv_event_state_machine,         ADV_SM_LIST_LENGTH(adv_event_state_machine),         ADV_CONTEXT_DEFAULT},
    {ADV_EXTENDED_EVENT,adv_extended_event_state_machine,ADV_SM_LIST_LENGTH(adv_extended_event_state_machine),ADV_CONTEXT_CONNECTABLE},
};

static adv_procedure_list_t adv_extended_scan_undirected_procedure[]=
{
    {ADV_EVENT,         adv_event_state_machine,         ADV_SM_LIST_LENGTH(adv_event_state_machine),         ADV_CONTEXT_DEFAULT},
    {ADV_EXTENDED_EVENT,adv_extended_event_state_machine,ADV_SM_LIST_LENGTH(adv_extended_event_state_machine),ADV_CONTEXT_SCANNABLE},
    {ADV_CHAINED_EVENT, adv_chained_event_state_machine, ADV_SM_LIST_LENGTH(adv_chained_event_state_machine), ADV_CONTEXT_CHAINED},
};

static adv_procedure_list_t adv_extended_scan_directed_procedure[]=
{
    {ADV_EVENT,         adv_event_state_machine,         ADV_SM_LIST_LENGTH(adv_event_state_machine),         ADV_CONTEXT_DEFAULT},
    {ADV_EXTENDED_EVENT,adv_extended_event_state_machine,ADV_SM_LIST_LENGTH(adv_extended_event_state_machine),ADV_CONTEXT_SCANNABLE},
    {ADV_CHAINED_EVENT, adv_chained_event_state_machine, ADV_SM_LIST_LENGTH(adv_chained_event_state_machine), ADV_CONTEXT_CHAINED},
};

static adv_procedure_list_t adv_extended_non_con_non_scan_undirected_procedure_with_auxiliary[]=
{
    {ADV_EVENT,         adv_event_state_machine,         ADV_SM_LIST_LENGTH(adv_event_state_machine),         ADV_CONTEXT_DEFAULT},
    {ADV_EXTENDED_EVENT,adv_extended_event_state_machine,ADV_SM_LIST_LENGTH(adv_extended_event_state_machine),ADV_CONTEXT_AUXILIARY},
    {ADV_CHAINED_EVENT, adv_chained_event_state_machine, ADV_SM_LIST_LENGTH(adv_chained_event_state_machine), ADV_CONTEXT_CHAINED},
};

static adv_procedure_list_t adv_extended_non_con_non_scan_directed_procedure_with_auxiliary[]=
{
    {ADV_EVENT,         adv_event_state_machine,         ADV_SM_LIST_LENGTH(adv_event_state_machine),         ADV_CONTEXT_DEFAULT},
    {ADV_EXTENDED_EVENT,adv_extended_event_state_machine,ADV_SM_LIST_LENGTH(adv_extended_event_state_machine),ADV_CONTEXT_AUXILIARY},
    {ADV_CHAINED_EVENT, adv_chained_event_state_machine, ADV_SM_LIST_LENGTH(adv_chained_event_state_machine), ADV_CONTEXT_CHAINED},
};

static adv_procedure_list_t adv_extended_non_con_non_scan_undirected_procedure_without_auxiliary[]=
{
    {ADV_EVENT,         adv_event_state_machine,         ADV_SM_LIST_LENGTH(adv_event_state_machine),         ADV_CONTEXT_DEFAULT},
};

static adv_procedure_list_t adv_extended_non_con_non_scan_directed_procedure_without_auxiliary[]=
{
    {ADV_EVENT,         adv_event_state_machine,         ADV_SM_LIST_LENGTH(adv_event_state_machine),         ADV_CONTEXT_DEFAULT},
};
#endif

/************* ADV Periodic Event Process **********/
#if(LL_SUPPORT_LE_PERIODIC_ADVERTISING)
static int adv_periodic_event_step_phy_send_sync_advertising(void)
{
    phy_obj_cast(&LLSM->phy);
    //prepare packet
    adv_prepare_packet[ADV_EVENT_EXTENDED_PERIODIC](ADV_PDU_CLASS_SYNC);
    currentSet->pa->sync.phy.chn = 35;//todo
    //prepare phy
    adv_prepare_phy(&currentSet->pa->sync.phy,currentSet->pa->sync.sch.anchorPoint,PHY_DIR_TX);
    LLSM->phy.start();
    currentSet->pa->eventCnt++;
	return 1;
}
static int adv_periodic_event_step_sch_stop(void)
{
    LLSM->phy.stop();
    currentSet->pa->sync.sch.anchorPoint+=currentSet->pa->sync.sch.interval;
}
static int adv_periodic_event_step_sch_passed(void)
{
    currentSet->pa->eventCnt++;
    currentSet->pa->sync.sch.anchorPoint+=currentSet->pa->sync.sch.interval;
}
static int adv_periodic_event_step_sch_canceled(void)
{
    currentSet->pa->eventCnt++;
    currentSet->pa->sync.sch.anchorPoint+=currentSet->pa->sync.sch.interval;
}
static adv_event_sm_t adv_periodic_event_state_machine[]= 
{
    {adv_periodic_event_step_phy_send_sync_advertising,ADV_CONTEXT_DEFAULT,ADV_SM_STATE_IDLE,                ADV_SM_STATE_SENDING_PERIODIC_ADV,ADV_SM_STATE_IDLE,LL_SCH_EVENT_START},
    {adv_periodic_event_step_sch_stop,                 ADV_CONTEXT_DEFAULT,ADV_SM_STATE_IDLE,                ADV_SM_STATE_IDLE,                ADV_SM_STATE_IDLE,LL_SCH_EVENT_STOP},
    {adv_periodic_event_step_sch_passed,               ADV_CONTEXT_DEFAULT,ADV_SM_STATE_IDLE,                ADV_SM_STATE_IDLE,                ADV_SM_STATE_IDLE,LL_SCH_EVENT_PASSED},
    {adv_periodic_event_step_sch_canceled,             ADV_CONTEXT_DEFAULT,ADV_SM_STATE_IDLE,                ADV_SM_STATE_IDLE,                ADV_SM_STATE_IDLE,LL_SCH_EVENT_CANCELED},

    {adv_periodic_event_step_sch_stop,                 ADV_CONTEXT_DEFAULT,ADV_SM_STATE_SENDING_PERIODIC_ADV,ADV_SM_STATE_IDLE,                ADV_SM_STATE_IDLE,LL_PHY_EVENT_SEND_FINISHED},
    {adv_periodic_event_step_sch_stop,                 ADV_CONTEXT_DEFAULT,ADV_SM_STATE_SENDING_PERIODIC_ADV,ADV_SM_STATE_IDLE,                ADV_SM_STATE_IDLE,LL_SCH_EVENT_STOP},
};
static adv_procedure_list_t adv_extended_periodic_procedure[]=
{
    {ADV_EVENT,         adv_event_state_machine,         ADV_SM_LIST_LENGTH(adv_event_state_machine),         ADV_CONTEXT_DEFAULT},
    {ADV_EXTENDED_EVENT,adv_extended_event_state_machine,ADV_SM_LIST_LENGTH(adv_extended_event_state_machine),ADV_CONTEXT_DEFAULT},
    {ADV_PERIODIC_EVENT,adv_periodic_event_state_machine,ADV_SM_LIST_LENGTH(adv_periodic_event_state_machine),ADV_CONTEXT_DEFAULT},
    {ADV_CHAINED_EVENT, adv_chained_event_state_machine, ADV_SM_LIST_LENGTH(adv_chained_event_state_machine), ADV_CONTEXT_CHAINED},
};
#endif

/**************ADV Periodic With Rsp Process ************/
#if(LL_SUPPORT_PERIODIC_ADVERTISING_WITH_RESPONSES_ADVERTISER)
static adv_event_sm_t adv_periodic_with_rsp_event_state_machine[]= 
{

};
static adv_procedure_list_t adv_extended_periodic_with_rsp_procedure[]=
{
    {ADV_EVENT,                  adv_event_state_machine,                  ADV_SM_LIST_LENGTH(adv_event_state_machine),                  ADV_CONTEXT_DEFAULT},
    {ADV_EXTENDED_EVENT,         adv_extended_event_state_machine,         ADV_SM_LIST_LENGTH(adv_extended_event_state_machine),         ADV_CONTEXT_DEFAULT},
    {ADV_PERIODIC_WITH_RSP_EVENT,adv_periodic_with_rsp_event_state_machine,ADV_SM_LIST_LENGTH(adv_periodic_with_rsp_event_state_machine),ADV_CONTEXT_DEFAULT},
};
#endif

_DATA static adv_sequence_t advSequence[] =
{
    {ADV_EVENT_CONNECTABLE_SCANNABLE_UNDIRECTED,                                    adv_con_scan_undirected_procedure,                                   ADV_PROCEDURE_LIST_LENGTH(adv_con_scan_undirected_procedure)},
    {ADV_EVENT_CONNECTABLE_DIRECTED,                                                adv_con_directed_procedure,                                          ADV_PROCEDURE_LIST_LENGTH(adv_con_directed_procedure)},
    {ADV_EVENT_SCANNABLE_UNDIRECTED,                                                adv_scan_undirected_procedure,                                       ADV_PROCEDURE_LIST_LENGTH(adv_scan_undirected_procedure)},
    {ADV_EVENT_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED,                            adv_non_con_non_scan_undirected_procedure,                           ADV_PROCEDURE_LIST_LENGTH(adv_non_con_non_scan_undirected_procedure)},
    #if(LL_SUPPORT_LE_EXTENDED_ADVERTISING)
    {ADV_EVENT_EXTENDED_CONNECTABLE_UNDIRECTED,                                     adv_extended_con_undirected_procedure,                               ADV_PROCEDURE_LIST_LENGTH(adv_extended_con_undirected_procedure)},
    {ADV_EVENT_EXTENDED_CONNECTABLE_DIRECTED,                                       adv_extended_con_directed_procedure,                                 ADV_PROCEDURE_LIST_LENGTH(adv_extended_con_directed_procedure)},
    {ADV_EVENT_EXTENDED_SCANNABLE_UNDIRECTED,                                       adv_extended_scan_undirected_procedure,                              ADV_PROCEDURE_LIST_LENGTH(adv_extended_scan_undirected_procedure)},
    {ADV_EVENT_EXTENDED_SCANNABLE_DIRECTED,                                         adv_extended_scan_directed_procedure,                                ADV_PROCEDURE_LIST_LENGTH(adv_extended_scan_directed_procedure)},
    {ADV_EVENT_EXTENDED_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED_WITH_AUXILIARY,    adv_extended_non_con_non_scan_undirected_procedure_with_auxiliary,   ADV_PROCEDURE_LIST_LENGTH(adv_extended_non_con_non_scan_undirected_procedure_with_auxiliary)},
    {ADV_EVENT_EXTENDED_NON_CONNECTABLE_NON_SCANNABLE_DIRECTED_WITH_AUXILIARY,      adv_extended_non_con_non_scan_directed_procedure_with_auxiliary,     ADV_PROCEDURE_LIST_LENGTH(adv_extended_non_con_non_scan_directed_procedure_with_auxiliary)},
    {ADV_EVENT_EXTENDED_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED_WITHOUT_AUXILIARY, adv_extended_non_con_non_scan_undirected_procedure_without_auxiliary,ADV_PROCEDURE_LIST_LENGTH(adv_extended_non_con_non_scan_undirected_procedure_without_auxiliary)},
    {ADV_EVENT_EXTENDED_NON_CONNECTABLE_NON_SCANNABLE_DIRECTED_WITHOUT_AUXILIARY,   adv_extended_non_con_non_scan_directed_procedure_without_auxiliary,  ADV_PROCEDURE_LIST_LENGTH(adv_extended_non_con_non_scan_directed_procedure_without_auxiliary)},
    #endif
    #if(LL_SUPPORT_LE_PERIODIC_ADVERTISING)
    {ADV_EVENT_EXTENDED_PERIODIC,                                                   adv_extended_periodic_procedure,                                     ADV_PROCEDURE_LIST_LENGTH(adv_extended_periodic_procedure)},
    #endif
    #if(LL_SUPPORT_PERIODIC_ADVERTISING_WITH_RESPONSES_ADVERTISER)
    {ADV_EVENT_EXTENDED_PERIODIC_WITH_RESPONSE,                                     adv_extended_periodic_with_rsp_procedure,                            ADV_PROCEDURE_LIST_LENGTH(adv_extended_periodic_with_rsp_procedure)},
    #endif
};

_RAM_CODE
static void adv_sequence_process(ll_sm_event_e smEventType)
{
    _u8 advEventType     = currentSet->eventType;//current adv set assigned in 'adv_get_next_event'
    _u8 advEventClass    = currentEventClass;
    if(advSequence[advEventType].listLen>advEventClass)//make sure event can be processed,
    {
        for(_u8 i=0;i<advSequence[advEventType].procedureList[advEventClass].listLen;i++)
        {
            if(currentSet->state  == advSequence[advEventType].procedureList[advEventClass].sm[i].currentState\
            && smEventType  == advSequence[advEventType].procedureList[advEventClass].sm[i].event\
		    &&(advSequence[advEventType].procedureList[advEventClass].context & advSequence[advEventType].procedureList[advEventClass].sm[i].context))
            {
                if(currentSet->processingEvent == smEventType)
                {
                    return;//reload same event,return
                }
                if(advSequence[advEventType].procedureList[advEventClass].sm[i].cb != NULL)
                {
                	currentSet->processingEvent = smEventType;//use member value to avoid many entity conflict
                    int ret = advSequence[advEventType].procedureList[advEventClass].sm[i].cb();
                    if(ret)
                    {
                    	currentSet->state = advSequence[advEventType].procedureList[advEventClass].sm[i].transSuccessState;
                    }
                    else
                    {
                    	currentSet->state = advSequence[advEventType].procedureList[advEventClass].sm[i].transFailState;
                    }
                    currentSet->processingEvent = 0;
                }
                if((smEventType & LL_SCH_EVENT_BASE) \
//                   && ll->state == BLE_LL_STATE_ADVERTISING //todo,when enter connect state,exit
                   && currentSet->enable\
                   && currentSet->state == ADV_SM_STATE_IDLE)
                {
                    adv_get_next_event();
                }
                if((smEventType & LL_PHY_EVENT_BASE)\
//                  && ll->state  == BLE_LL_STATE_ADVERTISING//can it works ok?
                  && currentSet->enable\
                  && currentSet->state == ADV_SM_STATE_IDLE)
                {
                	sch_stop_task_early();
                }
                break;               
            }
        }
    }
}

_RAM_CODE
static void adv_phy_irq_callback(_u8 type)
{
    if(type == PHY_IRQ_TX_FINISHED)
    {
        DEBUG_GPIO_HIGH(GPIO_8);
        DEBUG_GPIO_LOW(GPIO_8);
    }
    else if(type == PHY_IRQ_RX_FINISHED)
    {
        DEBUG_GPIO_HIGH(GPIO_9);
        DEBUG_GPIO_LOW(GPIO_9);
    }
    else if(type == PHY_IRQ_RX_TIMEOUT)
    {
        DEBUG_GPIO_HIGH(GPIO_10);
        DEBUG_GPIO_LOW(GPIO_10);
    }
    adv_sequence_process(LL_PHY_EVENT_BASE+type);
}

_RAM_CODE
static void adv_sch_callback(_u8 type,_u8 id)
{
    if(type == SCH_TASK_START)
    {
        DEBUG_GPIO_HIGH(GPIO_2);
        DEBUG_GPIO_LOW(GPIO_2);
    }
    else if(type == SCH_TASK_STOP)
    {
        DEBUG_GPIO_HIGH(GPIO_3);
        DEBUG_GPIO_LOW(GPIO_3);
    }
    else if(type == SCH_TASK_CANCELED)
    {
        DEBUG_GPIO_HIGH(GPIO_4);
        DEBUG_GPIO_LOW(GPIO_4);
    }
    else if(type == SCH_TASK_PASSED)
    {
        DEBUG_GPIO_HIGH(GPIO_5);
        DEBUG_GPIO_LOW(GPIO_5);
    }

	LLSM = (ll_sm_t*)ll_get_sm_entity_by_id(id);
	ASSERT(POINTER_VALID(LLSM));
	advCtrl = (ll_internal_adv_ctrl_t*)LLSM->entity;
	ASSERT(POINTER_VALID(advCtrl));
    adv_sequence_process(LL_SCH_EVENT_BASE+type);
}

/*************************************LL APIs Define*******************************************/
static void ble_ll_adv_reset(void)
{
	LLSM = (ll_sm_t*)ll_get_sm_entity_by_state(BLE_LL_STATE_ADVERTISING,LL_SM_INVALID_HANDLE,0);
	ASSERT(POINTER_VALID(LLSM));
	advCtrl = (ll_internal_adv_ctrl_t*)LLSM->entity;
	ASSERT(POINTER_VALID(advCtrl));
	for(int i=0;i<BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS;i++)
	{
		if(POINTER_VALID(advCtrl->set[i].data.addr))
		{
			tx_free(advCtrl->set[i].data.addr);
			advCtrl->set[i].data.addr = NULL;
		}
		if(POINTER_VALID(advCtrl->set[i].scanRsp.addr))
		{
			tx_free(advCtrl->set[i].scanRsp.addr);
			advCtrl->set[i].scanRsp.addr = NULL;
		}
		#if(LL_SUPPORT_LE_EXTENDED_ADVERTISING)
		if(POINTER_VALID(advCtrl->set[i].ea))
		{
			if(POINTER_VALID(advCtrl->set[i].ea->chain.entry))
			{
				tx_free((_u8*)advCtrl->set[i].ea->chain.entry);
				advCtrl->set[i].ea->chain.entry = NULL;
			}
			tx_free((_u8*)advCtrl->set[i].ea);
			advCtrl->set[i].ea = NULL;
		}
		#endif
		#if(LL_SUPPORT_LE_PERIODIC_ADVERTISING)
		#endif
		#if(LL_SUPPORT_PERIODIC_ADVERTISING_WITH_RESPONSES_ADVERTISER)
		#endif
	}
}

int ble_ll_enter_advertising_state(void)
{
	ll_sm_t* llsm = (ll_sm_t*)ll_get_sm_entity_by_state(BLE_LL_STATE_ADVERTISING,LL_SM_INVALID_HANDLE,0);
	if(POINTER_NOT_VALID(llsm)||POINTER_NOT_VALID(llsm->entity))
	{
		return 1;
	}
	LLSM = llsm;
	advCtrl = (ll_internal_adv_ctrl_t*)llsm->entity;
	//need process multiple advertising set
	for(int i=0;i<BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS;i++)
	{
		if(advCtrl->set[i].enable&&!advCtrl->set[i].inSch)//can't reallocate all asv set,some adv set maybe running.
		{
			advCtrl->set[i].inSch = 1;
			advCtrl->set[i].state = ADV_SM_STATE_IDLE;
			ll_adv_task_timing_allocation(&advCtrl->set[i],system_time()+500,system_time()+500+advCtrl->set[i].la.sch.interval,ADV_SCH_MAP_ALL);
		}
	}
	if(!advCtrl->active)
	{
		llsm->sch.id         = llsm->id;
		advCtrl->reset       = ble_ll_adv_reset;
		llsm->phy.hw_irq_cb  = adv_phy_irq_callback;
		llsm->sch.cb         = adv_sch_callback;
		llsm->sch.type       = SCH_PERIODIC_TASK;
		llsm->sch.priority   = LL_ADV_PRIORITY;
		adv_get_next_event();
		if(sch_insert_task(&llsm->sch)==SCH_STATUS_SUCCESS)
		{
			sch_start();
		}
		else
		{
			return 2;
		}
		advCtrl->active = 1;
	}
	return 0;
}

int ble_ll_exit_advertising_state(void)
{
	ll_sm_t* llsm = (ll_sm_t*)ll_get_sm_entity_by_state(BLE_LL_STATE_ADVERTISING,LL_SM_INVALID_HANDLE,0);
	ASSERT(POINTER_VALID(llsm));
	advCtrl = (ll_internal_adv_ctrl_t*)llsm->entity;
	ASSERT(POINTER_VALID(advCtrl));
	if(advCtrl->active)
	{
		advCtrl->active = 0;
		sch_remove_task(llsm->sch.id);
	}
	return 0;
}

/*************************************Bluetooth LE Advertising LL APIS******************************/

controller_error_code_e ll_set_advertising_parameters(_u16 interval,\
	                                                  ll_advertising_type_e type,\
													  ll_own_address_type_e ownAddressType,\
	                                                  ll_peer_address_type_e peerAddressType,\
													  _u8* peerAddress,_u8 channelMap,\
	                                                  ll_advertising_filter_policy_e policy)
{

	ll_sm_t* llsm = (ll_sm_t*)ll_get_sm_entity_by_state(BLE_LL_STATE_ADVERTISING,LL_SM_INVALID_HANDLE,1);
	if(POINTER_NOT_VALID(llsm))
	{
		return MEMORY_CAPACITY_EXCEEDED;//the spec not specify error code in this scenario
	}
	//legacy adv mode only one set is allowed,thus only set[0] can be used.
	if(POINTER_NOT_VALID(llsm->entity))
	{
		llsm->entity = tx_malloc(sizeof(ll_internal_adv_ctrl_t));
        ASSERT(POINTER_VALID(llsm->entity));
	}
	ll_internal_adv_ctrl_t* adv = (ll_internal_adv_ctrl_t*)llsm->entity;
	adv->set[0].ownAddressType  = ownAddressType;
	adv->set[0].peerAddressType = peerAddressType;
	adv->set[0].filterPolicy = policy;
	txMemcpy(adv->set[0].peerAddress,peerAddress,6);

	adv->set[0].la.sch.interval = interval*625;
	adv->set[0].la.channelCnt   = 0;
    for(int i=0;i<3;i++)
    {
    	if(channelMap&BIT(i))
    	{
    		adv->set[0].la.chnTable[adv->set[0].la.channelCnt++] = 37+i;
    	}
    }
	switch(type)
	{
		case LL_ADV_IND:
		{
			adv->set[0].eventType = ADV_EVENT_CONNECTABLE_SCANNABLE_UNDIRECTED;
		}break;
		case LL_ADV_DIRECT_IND_HIGH_DUTY:
		case LL_ADV_DIRECT_IND_LOW_DUTY:
		{
			adv->set[0].eventType = ADV_EVENT_CONNECTABLE_DIRECTED;
		}break;
		case LL_ADV_SCAN_IND:
		{
			adv->set[0].eventType = ADV_EVENT_SCANNABLE_UNDIRECTED;
		}break;
		case LL_ADV_NONCONN_IND:
		{
			adv->set[0].eventType = ADV_EVENT_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED;
		}break;
	}
	LOG_TRACE(BLE_ADV_TRACE_ENABLE,"ll set adv param",0,0)
	return SUCCESS;
}

controller_error_code_e ll_set_advertising_data(_u8* data,_u8 length)
{
	ll_sm_t* llsm = (ll_sm_t*)ll_get_sm_entity_by_state(BLE_LL_STATE_ADVERTISING,LL_SM_INVALID_HANDLE,1);
	if(POINTER_NOT_VALID(llsm))
	{
		return MEMORY_CAPACITY_EXCEEDED;//the spec not specify error code in this scenario
	}

	if(POINTER_NOT_VALID(llsm->entity))
	{
		llsm->entity = tx_malloc(sizeof(ll_internal_adv_ctrl_t));
        ASSERT(POINTER_VALID(llsm->entity));
	}
	ll_internal_adv_ctrl_t* adv = (ll_internal_adv_ctrl_t*)llsm->entity;
    if(adv->set[0].data.addr)
	{
		tx_free(adv->set[0].data.addr);
		adv->set[0].data.addr = NULL;
	}
    adv->set[0].data.len  = length;
    adv->set[0].data.addr = tx_malloc(length);
	txMemcpy(adv->set[0].data.addr,data,length);
	LOG_TRACE(BLE_ADV_TRACE_ENABLE,"ll set adv data",adv->set[0].data.addr,adv->set[0].data.len)
	return SUCCESS;
}

controller_error_code_e ll_set_scan_response_data(_u8* data,_u8 length)
{
	ll_sm_t* llsm = (ll_sm_t*)ll_get_sm_entity_by_state(BLE_LL_STATE_ADVERTISING,LL_SM_INVALID_HANDLE,1);
	if(POINTER_NOT_VALID(llsm))
	{
		return MEMORY_CAPACITY_EXCEEDED;//the spec not specify error code in this scenario
	}
	if(POINTER_NOT_VALID(llsm->entity))
	{
		llsm->entity = tx_malloc(sizeof(ll_internal_adv_ctrl_t));
        ASSERT(POINTER_VALID(llsm->entity));
	}
	ll_internal_adv_ctrl_t* adv = (ll_internal_adv_ctrl_t*)llsm->entity;
    if(adv->set[0].scanRsp.addr)
	{
		tx_free(adv->set[0].scanRsp.addr);
		adv->set[0].scanRsp.addr = NULL;
	}
    adv->set[0].scanRsp.len  = length;
    adv->set[0].scanRsp.addr = tx_malloc(length);
	txMemcpy(adv->set[0].scanRsp.addr,data,length);
	LOG_TRACE(BLE_ADV_TRACE_ENABLE,"ll set scan rsp data",adv->set[0].scanRsp.addr,adv->set[0].scanRsp.len)
	return SUCCESS;
}
controller_error_code_e ll_set_advertising_enable(_u8 enable)
{
	ll_sm_t* llsm = (ll_sm_t*)ll_get_sm_entity_by_state(BLE_LL_STATE_ADVERTISING,LL_SM_INVALID_HANDLE,0);
	if(POINTER_NOT_VALID(llsm) ||POINTER_NOT_VALID(llsm->entity))
	{
		return IVALID_HCI_COMMAND_PARAMETERS;//the spec not specify error code in this scenario
	}
    ll_internal_adv_ctrl_t* adv = (ll_internal_adv_ctrl_t*)llsm->entity;
    if(adv->set[0].enable == enable)
	{
		return SUCCESS;//enable adv that already enabled,maybe change the random address
	}
	LOG_TRACE(BLE_ADV_TRACE_ENABLE,"set scan enable",&enable,1)
	if(adv->set[0].enable!=enable)
	{
		adv->set[0].enable = enable;
		if(adv->set[0].enable == LL_ADVERTISING_ENABLE)
		{
	        phy_obj_cast(&llsm->phy);
	        phy_obj_init(&llsm->phy);
	        ll_internal_adv_set_t* advSet = &adv->set[0];
			//sch init
	        advSet->la.availableChnCnt     = advSet->la.channelCnt;
	        advSet->la.eventCnt            = 0;
	        advSet->la.sch.anchorPoint     = system_time() + 500;
			if(advSet->eventType == ADV_EVENT_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED)
			{
				advSet->la.sch.duration = llsm->phy.hw_get_prepare_time()+ll_get_air_packet_time(PHY_MODE_1M,BLE_ADV_PRI_PHY_MAX_TX_LEN,0);
			}
			else
			{
				advSet->la.sch.duration = llsm->phy.hw_get_prepare_time()+3*ll_get_air_packet_time(PHY_MODE_1M,BLE_ADV_PRI_PHY_MAX_TX_LEN,0)+2*PACKET_DEFAULT_TIFS_TIME;
			}
			advSet->la.sch.startMargin     = 100;
			advSet->la.sch.stopMargin      = 75;
			//phy init
			advSet->la.phy.mode            = PHY_MODE_1M;
			advSet->la.phy.crcInit         = BLE_ADV_CRC_INIT;
			advSet->la.phy.accessCode      = BLE_ADV_ACCESS_CODE;
			advSet->la.phy.rxMaxOctets     = BLE_ADV_PRI_PHY_MAX_RX_LEN;
			advSet->la.phy.rxAddress       = ll_get_shared_phy_rx_address();
			advSet->la.phy.txAddress       = ll_get_shared_phy_tx_address();
			if(ble_ll_enter_advertising_state())
			{
				return IVALID_HCI_COMMAND_PARAMETERS;
			}
		}
		else//LL_ADVERTISING_DISABLE
		{
			if(ble_ll_exit_advertising_state())
			{
				return IVALID_HCI_COMMAND_PARAMETERS;
			}
		}
	}
	return SUCCESS;
}



#if(LL_SUPPORT_LE_EXTENDED_ADVERTISING)
controller_error_code_e ll_set_extended_advertising_parameters(ll_extended_adv_param_t* pParam)
{
	ll_sm_t* llsm = (ll_sm_t*)ll_get_sm_entity_by_state(BLE_LL_STATE_ADVERTISING,LL_SM_INVALID_HANDLE,1);
	if(POINTER_NOT_VALID(llsm))
	{
		return MEMORY_CAPACITY_EXCEEDED;
	}
	if(POINTER_NOT_VALID(llsm->entity))
	{
		llsm->entity = tx_malloc(sizeof(ll_internal_adv_ctrl_t));
		ASSERT(POINTER_VALID(llsm->entity));
		for(int i=0;i<BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS;i++)
		{
			((ll_internal_adv_ctrl_t*)llsm->entity)->set[i].handle = LL_EXTENDED_ADV_INVALID_HANDLE;
			((ll_internal_adv_ctrl_t*)llsm->entity)->set[i].sid = i;
		}
	}

	ll_internal_adv_set_t* advSet = ll_extended_adv_get_adv_set(pParam->advHandle,1);
	if(POINTER_NOT_VALID(advSet))
	{
		return MEMORY_CAPACITY_EXCEEDED;
	}
	advSet->eventProperty     = pParam->advEventProperty;
	advSet->filterPolicy      = pParam->filterPolicy;
	advSet->ownAddressType    = pParam->ownAddrType;
	advSet->peerAddressType   = pParam->peerAddrType;
	advSet->la.sch.interval   = pParam->primaryAdvInterval * 625;
	advSet->la.channelCnt     = 0;
    for(int i=0;i<3;i++)
    {
    	if(pParam->primaryAdvChnMap&BIT(i))
    	{
    		advSet->la.chnTable[advSet->la.channelCnt++] = 37+i;
    	}
    }
	if(pParam->advEventProperty&LL_ADV_EVENT_PROPERTY_LEGACY_PDU)
	{
		switch(pParam->advEventProperty)
		{
			case (LL_ADV_EVENT_PROPERTY_LEGACY_PDU|LL_ADV_EVENT_PROPERTY_CONNECTED|LL_ADV_EVENT_PROPERTY_SCANNABLE):
			{
				advSet->eventType = ADV_EVENT_CONNECTABLE_SCANNABLE_UNDIRECTED;
			}
				break;
			case (LL_ADV_EVENT_PROPERTY_LEGACY_PDU|LL_ADV_EVENT_PROPERTY_CONNECTED|LL_ADV_EVENT_PROPERTY_DIRECTED):
			case (LL_ADV_EVENT_PROPERTY_LEGACY_PDU|LL_ADV_EVENT_PROPERTY_CONNECTED|LL_ADV_EVENT_PROPERTY_DIRECTED|LL_ADV_EVENT_PROPERTY_HIGH_DUTY_CONNECTED):
			{
				//todo,high duty cycle
				advSet->eventType = ADV_EVENT_CONNECTABLE_DIRECTED;
			}
				break;
			case (LL_ADV_EVENT_PROPERTY_LEGACY_PDU|LL_ADV_EVENT_PROPERTY_SCANNABLE):
			{
				advSet->eventType = ADV_EVENT_SCANNABLE_UNDIRECTED;
			}
				break;
			case (LL_ADV_EVENT_PROPERTY_LEGACY_PDU):
			{
				advSet->eventType = ADV_EVENT_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED;
			}
		}
	}
	else
	{
		if(POINTER_NOT_VALID(advSet->ea))
		{
			advSet->ea = (ll_adv_type_ea_t*)tx_malloc(sizeof(ll_adv_type_ea_t));
		}

		switch(pParam->advEventProperty&(LL_ADV_EVENT_PROPERTY_CONNECTED|LL_ADV_EVENT_PROPERTY_SCANNABLE|LL_ADV_EVENT_PROPERTY_DIRECTED))
		{
			case(LL_ADV_EVENT_PROPERTY_CONNECTED|LL_ADV_EVENT_PROPERTY_DIRECTED):
			{
				advSet->eventType = ADV_EVENT_EXTENDED_CONNECTABLE_DIRECTED;
				advSet->schMap |= ADV_SCH_MAP_AUX;
			}
				break;
			case(LL_ADV_EVENT_PROPERTY_CONNECTED):
			{
				advSet->eventType = ADV_EVENT_EXTENDED_CONNECTABLE_UNDIRECTED;
				advSet->schMap |= ADV_SCH_MAP_AUX;
			}
				break;
			case(LL_ADV_EVENT_PROPERTY_SCANNABLE|LL_ADV_EVENT_PROPERTY_DIRECTED):
			{
				advSet->eventType = ADV_EVENT_EXTENDED_SCANNABLE_DIRECTED;
				advSet->schMap |= ADV_SCH_MAP_AUX;
			}
				break;
			case(LL_ADV_EVENT_PROPERTY_SCANNABLE):
			{
				advSet->eventType = ADV_EVENT_EXTENDED_SCANNABLE_UNDIRECTED;
				advSet->schMap |= ADV_SCH_MAP_AUX;
			}
				break;
			case(LL_ADV_EVENT_PROPERTY_DIRECTED):
			{
				advSet->eventType = ADV_EVENT_EXTENDED_NON_CONNECTABLE_NON_SCANNABLE_DIRECTED_WITHOUT_AUXILIARY;
			}
				break;
			case(0):
			{
				advSet->eventType = ADV_EVENT_EXTENDED_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED_WITHOUT_AUXILIARY;
			}
				break;
		}
		advSet->sid                      = pParam->advSid;
		advSet->ea->scanReqNotifyEnable  = pParam->scanReqNotifyEnable;
		advSet->ea->secondaryMaxSkip     = pParam->secondaryAdvMaxSkip;
		advSet->ea->power  = 0;
		if(pParam->txPower!=0x7F)
		{
			advSet->ea->power = pParam->txPower;
		}
		if(pParam->secondaryAdvPhy == LL_ADV_PHY_CODED)
		{
			if(pParam->secondaryAdvphyOptions == LL_ADV_PHY_OPTIONS_PREFER_S2||\
			pParam->secondaryAdvphyOptions == LL_ADV_PHY_OPTIONS_REQUIRE_S2)
			{
				advSet->ea->phyMode = PHY_MODE_CODED_S2;
			}
			else if(pParam->secondaryAdvphyOptions == LL_ADV_PHY_OPTIONS_PREFER_S8||\
					pParam->secondaryAdvphyOptions == LL_ADV_PHY_OPTIONS_REQUIRE_S8)
			{
				advSet->ea->phyMode = (_u8)PHY_MODE_CODED_S8;
			}
			else
			{
				advSet->ea->phyMode = (_u8)PHY_MODE_CODED_S8;
			}
		}
		else if(pParam->secondaryAdvPhy == LL_ADV_PHY_1M)
		{
			advSet->ea->phyMode = PHY_MODE_1M;
		}
		else
		{
			advSet->ea->phyMode = PHY_MODE_2M;
		}
	}
	if(pParam->primaryAdvPhy == LL_ADV_PHY_CODED)
	{
		if(pParam->primaryAdvphyOptions == LL_ADV_PHY_OPTIONS_PREFER_S2||\
	       pParam->primaryAdvphyOptions == LL_ADV_PHY_OPTIONS_REQUIRE_S2)
		{
			advSet->la.phy.mode = PHY_MODE_CODED_S2;
		}
		else if(pParam->primaryAdvphyOptions == LL_ADV_PHY_OPTIONS_PREFER_S8||\
	            pParam->primaryAdvphyOptions == LL_ADV_PHY_OPTIONS_REQUIRE_S8)
		{
			advSet->la.phy.mode = (_u8)PHY_MODE_CODED_S8;
		}
		else
		{
			advSet->la.phy.mode = (_u8)PHY_MODE_CODED_S8;
		}
	}
	else
	{
		advSet->la.phy.mode = PHY_MODE_1M;
	}
	return SUCCESS;
}



controller_error_code_e ll_set_extended_advertising_data(_u8 advHandle,\
														ll_advertising_data_operation_e operation,\
														ll_advertising_data_fragment_perference_e fragPref,\
														_u8 dataLen,\
														_u8* data)
{

	ll_internal_adv_set_t* advSet = ll_extended_adv_get_adv_set(advHandle,0);
	if(POINTER_NOT_VALID(advSet))
	{
		return UNKNOWN_ADVERTISING_IDENTIFIER;
	}
	if(advSet->eventProperty & LL_ADV_EVENT_PROPERTY_LEGACY_PDU)
	{
		if(dataLen > 31||\
		   operation!=LL_ADV_DATA_OPERATION_COMPLETE||\
		   advSet->eventType == ADV_EVENT_CONNECTABLE_DIRECTED)//direct pdu don't have advertising data
		{
			return IVALID_HCI_COMMAND_PARAMETERS;
		}
	}

	if(operation == LL_ADV_DATA_OPERATION_UNCHANGED)
	{
		if(advSet->enable == 0||\
		   advSet->data.len == 0||\
		   dataLen != 0)
		{
			return IVALID_HCI_COMMAND_PARAMETERS;
		}
	}

	if(operation!=LL_ADV_DATA_OPERATION_COMPLETE&&\
	   operation!=LL_ADV_DATA_OPERATION_UNCHANGED&&\
	   dataLen == 0)
	{
		return IVALID_HCI_COMMAND_PARAMETERS;
	}

	if(advSet->enable)
	{
		if(operation!=LL_ADV_DATA_OPERATION_COMPLETE&&\
		   operation!=LL_ADV_DATA_OPERATION_UNCHANGED)
		{
			return COMMAND_DISALLOWED;
		}
	}

	if(advSet->eventType == ADV_EVENT_EXTENDED_SCANNABLE_DIRECTED||\
	   advSet->eventType == ADV_EVENT_EXTENDED_SCANNABLE_UNDIRECTED)
	{
		return IVALID_HCI_COMMAND_PARAMETERS;//mode don't have advertising data
	}

	if(dataLen > BLE_ADV_MAXIMUM_ADVERTISING_DATA_LENGTH)
	{
		return MEMORY_CAPACITY_EXCEEDED;
	}

	static _u32 dataFillOffset = 0;
	if(operation == LL_ADV_DATA_OPERATION_FIRST_FRAGMENT||\
	   operation == LL_ADV_DATA_OPERATION_COMPLETE||\
	   operation == LL_ADV_DATA_OPERATION_UNCHANGED)
	{
		dataFillOffset = 0;
	}
	if(!(advSet->eventProperty&LL_ADV_EVENT_PROPERTY_LEGACY_PDU))
	{
		if(ll_get_air_packet_time(advSet->ea->phyMode,dataLen,0)>(advSet->la.sch.interval*3/4))
		{
			return PACKET_TOO_LONG;
		}
		if(advSet->eventProperty & (LL_ADV_EVENT_PROPERTY_CONNECTED))
		{
			if((dataFillOffset+dataLen)>BLE_ADV_SEC_PHY_MAX_TX_LEN)
			{
				dataFillOffset = 0;
				return MEMORY_CAPACITY_EXCEEDED;
			}
		}
	}

	if((operation == LL_ADV_DATA_OPERATION_INTERMEDIATE_FRAGMENT) || (operation == LL_ADV_DATA_OPERATION_LAST_FRAGMENT))
	{
		if((dataFillOffset+dataLen)>BLE_ADV_MAXIMUM_ADVERTISING_DATA_LENGTH)
		{
			dataFillOffset = 0;
			return MEMORY_CAPACITY_EXCEEDED;
		}
		if(ll_get_air_packet_time(advSet->ea->phyMode,dataFillOffset+dataLen,0)>(advSet->la.sch.interval*3/4))
		{
			dataFillOffset = 0;
			return PACKET_TOO_LONG;
		}
		if(dataFillOffset == 0)
		{
			return COMMAND_DISALLOWED;
		}
	}
	//when start configure adv data,buffer size set earlier maybe not inappropriate,shall reset.
	if(operation == LL_ADV_DATA_OPERATION_FIRST_FRAGMENT
	  ||operation == LL_ADV_DATA_OPERATION_COMPLETE)
	{
		if(POINTER_VALID(advSet->data.addr))
		{
			tx_free(advSet->data.addr);
			advSet->data.addr = NULL;
		}
	}
	if(POINTER_NOT_VALID(advSet->data.addr))
	{
		if(operation == LL_ADV_DATA_OPERATION_COMPLETE)
		{
			advSet->data.addr = tx_malloc(dataLen);
		}
		else
		{
			advSet->data.addr = tx_malloc(BLE_ADV_MAXIMUM_ADVERTISING_DATA_LENGTH);
		}
	}

	if(advSet->eventType == ADV_EVENT_EXTENDED_NON_CONNECTABLE_NON_SCANNABLE_DIRECTED_WITHOUT_AUXILIARY)
	{
		advSet->eventType = ADV_EVENT_EXTENDED_NON_CONNECTABLE_NON_SCANNABLE_DIRECTED_WITH_AUXILIARY;
		advSet->schMap |= ADV_SCH_MAP_AUX;
	}
	else if(advSet->eventType == ADV_EVENT_EXTENDED_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED_WITHOUT_AUXILIARY)
	{
		advSet->eventType = ADV_EVENT_EXTENDED_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED_WITH_AUXILIARY;
		advSet->schMap |= ADV_SCH_MAP_AUX;
	}

	switch(operation)
	{
		case LL_ADV_DATA_OPERATION_INTERMEDIATE_FRAGMENT:
		{
			txMemcpy4(advSet->data.addr+dataFillOffset,data,dataLen);
			dataFillOffset+=dataLen;
		}
			break;
		case LL_ADV_DATA_OPERATION_FIRST_FRAGMENT:
		{
			txMemcpy4(advSet->data.addr,data,dataLen);
			dataFillOffset = dataLen;
		}
			break;
		case LL_ADV_DATA_OPERATION_LAST_FRAGMENT:
		{
			txMemcpy4(advSet->data.addr+dataFillOffset,data,dataLen);
			advSet->ea->did++;
			advSet->data.len = dataFillOffset + dataLen;
		}
			break;
		case LL_ADV_DATA_OPERATION_COMPLETE:
		{
			txMemcpy(advSet->data.addr,data,dataLen);
			advSet->ea->did++;
			advSet->data.len = dataLen;
		}
			break;
		case LL_ADV_DATA_OPERATION_UNCHANGED:
		{
			advSet->ea->did++;
		}
			break;
	}
	if((!(advSet->eventProperty&LL_ADV_EVENT_PROPERTY_LEGACY_PDU))&&(operation == LL_ADV_DATA_OPERATION_LAST_FRAGMENT||operation == LL_ADV_DATA_OPERATION_COMPLETE))
	{
		if(POINTER_VALID(advSet->ea->chain.entry))
		{
			tx_free((_u8*)advSet->ea->chain.entry);
			advSet->ea->chain.entry = NULL;
		}
		advSet->ea->aux.data.addr= advSet->data.addr;
		//process data fragment
		if(advSet->data.len<=(BLE_ADV_SEC_PHY_MAX_TX_LEN - BLE_ADV_EXTENDED_HEADER_MAX_LEN))
		{
			advSet->ea->chain.cnt    = 0;//only aux packet exist
			advSet->ea->aux.data.len = advSet->data.len;
		}
		else
		{
			_u8 remainLen          = ((advSet->data.len-(BLE_ADV_SEC_PHY_MAX_TX_LEN - BLE_ADV_EXTENDED_HEADER_MAX_LEN)))%BLE_ADV_SEC_PHY_MAX_TX_LEN;
			_u8 chainCnt           = ((advSet->data.len-(BLE_ADV_SEC_PHY_MAX_TX_LEN - BLE_ADV_EXTENDED_HEADER_MAX_LEN)))/BLE_ADV_SEC_PHY_MAX_TX_LEN + (remainLen==0?0:1);
			advSet->schMap          |= ADV_SCH_MAP_AUX_CHAIN;
			advSet->ea->chain.entry  = (ll_adv_entry_t*)tx_malloc(chainCnt*sizeof(ll_adv_entry_t));
			advSet->ea->chain.cnt    = chainCnt;
			advSet->ea->aux.data.len = BLE_ADV_SEC_PHY_MAX_TX_LEN - BLE_ADV_EXTENDED_HEADER_MAX_LEN;
			_u16 offset            = advSet->ea->aux.data.len;
			for(_u8 i=0;i<chainCnt-1;i++)
			{
				advSet->ea->chain.entry[i].data.len = BLE_ADV_SEC_PHY_MAX_TX_LEN;
				advSet->ea->chain.entry[i].data.addr= (advSet->data.addr+offset);
				offset+=BLE_ADV_SEC_PHY_MAX_TX_LEN;
			}
			if(remainLen!=0)
			{
				advSet->ea->chain.entry[chainCnt-1].data.len = remainLen;
				advSet->ea->chain.entry[chainCnt-1].data.addr= (advSet->data.addr+offset);
			}
		}
	}
	advSet->ea->advDatafragPerf = fragPref;
	return SUCCESS;
}


controller_error_code_e ll_set_extended_scan_response_data(_u8 advHandle,\
															ll_advertising_data_operation_e operation,\
															ll_advertising_data_fragment_perference_e fragPref,\
															_u8 dataLen,\
															_u8* data)
{
	ll_internal_adv_set_t* advSet = ll_extended_adv_get_adv_set(advHandle,0);
	if(POINTER_NOT_VALID(advSet))
	{
		return UNKNOWN_ADVERTISING_IDENTIFIER;
	}

	if(advSet->eventProperty & LL_ADV_EVENT_PROPERTY_LEGACY_PDU)
	{
		if(dataLen > 31||\
		   operation!=LL_ADV_DATA_OPERATION_COMPLETE||\
		   ((advSet->eventType != ADV_EVENT_CONNECTABLE_SCANNABLE_UNDIRECTED)&&(advSet->eventType != ADV_EVENT_SCANNABLE_UNDIRECTED)))
		{
			return IVALID_HCI_COMMAND_PARAMETERS;
		}
	}
	else
	{
		if(advSet->eventType != ADV_EVENT_EXTENDED_SCANNABLE_DIRECTED&&\
		   advSet->eventType != ADV_EVENT_EXTENDED_SCANNABLE_UNDIRECTED&&\
		   advSet->eventType != ADV_EVENT_CONNECTABLE_SCANNABLE_UNDIRECTED&&\
		   advSet->eventType != ADV_EVENT_SCANNABLE_UNDIRECTED)
		{
			return IVALID_HCI_COMMAND_PARAMETERS;
		}
	}

	if(operation!=LL_ADV_DATA_OPERATION_COMPLETE)
	{
		if(dataLen == 0)
		{
			return IVALID_HCI_COMMAND_PARAMETERS;
		}
		if(advSet->enable)
		{
			return COMMAND_DISALLOWED;
		}
	}

	if(advSet->enable)
	{
		if(dataLen == 0)
		{
			return COMMAND_DISALLOWED;
		}
	}

	if(dataLen > BLE_ADV_MAXIMUM_ADVERTISING_DATA_LENGTH)
	{
		return MEMORY_CAPACITY_EXCEEDED;
	}

	static _u32 dataFillOffset = 0;
	if(operation == LL_ADV_DATA_OPERATION_FIRST_FRAGMENT||\
	   operation == LL_ADV_DATA_OPERATION_COMPLETE||\
	   operation == LL_ADV_DATA_OPERATION_UNCHANGED)
	{
		dataFillOffset = 0;
	}

	if(!(advSet->eventProperty&LL_ADV_EVENT_PROPERTY_LEGACY_PDU))
	{
		if(ll_get_air_packet_time(advSet->ea->phyMode,dataLen,0)>(advSet->la.sch.interval*3/4))
		{
			return PACKET_TOO_LONG;
		}
		if(advSet->eventProperty & (LL_ADV_EVENT_PROPERTY_SCANNABLE))
		{
			if((dataFillOffset+dataLen)>BLE_ADV_SEC_PHY_MAX_TX_LEN)
			{
				dataFillOffset = 0;
				return MEMORY_CAPACITY_EXCEEDED;
			}
		}
	}
	if((operation == LL_ADV_DATA_OPERATION_INTERMEDIATE_FRAGMENT) || (operation == LL_ADV_DATA_OPERATION_LAST_FRAGMENT))
	{
		if((dataFillOffset+dataLen)>BLE_ADV_MAXIMUM_ADVERTISING_DATA_LENGTH)
		{
			dataFillOffset = 0;
			return MEMORY_CAPACITY_EXCEEDED;
		}
		if(ll_get_air_packet_time(advSet->ea->phyMode,dataFillOffset+dataLen,0)>(advSet->la.sch.interval*3/4))
		{
			return PACKET_TOO_LONG;
		}
		if(dataFillOffset == 0)
		{
			return COMMAND_DISALLOWED;
		}
	}
	//when start configure adv data,buffer size set earlier maybe not inappropriate,shall reset.
	if(operation == LL_ADV_DATA_OPERATION_FIRST_FRAGMENT
	  ||operation == LL_ADV_DATA_OPERATION_COMPLETE)
	{
		if(POINTER_VALID(advSet->scanRsp.addr))
		{
			tx_free(advSet->scanRsp.addr);
			advSet->scanRsp.addr = NULL;
		}
	}

	if(POINTER_NOT_VALID(advSet->scanRsp.addr))
	{
		if(operation == LL_ADV_DATA_OPERATION_COMPLETE)
		{
			advSet->scanRsp.addr = tx_malloc(dataLen);
		}
		else
		{
			advSet->scanRsp.addr = tx_malloc(BLE_ADV_MAXIMUM_ADVERTISING_DATA_LENGTH);
		}
	}
	switch(operation)
	{
		case LL_ADV_DATA_OPERATION_INTERMEDIATE_FRAGMENT:
		{
			txMemcpy4(advSet->scanRsp.addr+dataFillOffset,data,dataLen);
			dataFillOffset+=dataLen;
		}
			break;
		case LL_ADV_DATA_OPERATION_FIRST_FRAGMENT:
		{
			txMemcpy4(advSet->scanRsp.addr,data,dataLen);
			dataFillOffset=dataLen;
		}
			break;
		case LL_ADV_DATA_OPERATION_LAST_FRAGMENT:
		{
			txMemcpy4(advSet->scanRsp.addr+dataFillOffset,data,dataLen);
			advSet->scanRsp.len = dataFillOffset+dataLen;
			dataFillOffset = 0;

		}
			break;
		case LL_ADV_DATA_OPERATION_COMPLETE:
		{
			txMemcpy(advSet->scanRsp.addr,data,dataLen);
			advSet->scanRsp.len = dataLen;
		}
			break;
	}
	if(!(advSet->eventProperty&LL_ADV_EVENT_PROPERTY_LEGACY_PDU)&&(operation == LL_ADV_DATA_OPERATION_LAST_FRAGMENT||operation == LL_ADV_DATA_OPERATION_COMPLETE))
	{
		if(POINTER_VALID(advSet->ea->chain.entry))
		{
			tx_free((_u8*)advSet->ea->chain.entry);
			advSet->ea->chain.entry = NULL;
		}
		advSet->ea->aux.data.addr= advSet->scanRsp.addr;
		//process data fragment
		if(advSet->scanRsp.len<=(BLE_ADV_SEC_PHY_MAX_TX_LEN - BLE_ADV_EXTENDED_HEADER_MAX_LEN))
		{
			advSet->ea->chain.cnt= 0;//only aux packet exist
			advSet->ea->aux.data.len = advSet->scanRsp.len;
		}
		else
		{
			_u8 remainLen         = ((advSet->scanRsp.len-(BLE_ADV_SEC_PHY_MAX_TX_LEN - BLE_ADV_EXTENDED_HEADER_MAX_LEN)))%BLE_ADV_SEC_PHY_MAX_TX_LEN;
			_u8 chainCnt          = ((advSet->scanRsp.len-(BLE_ADV_SEC_PHY_MAX_TX_LEN - BLE_ADV_EXTENDED_HEADER_MAX_LEN)))/BLE_ADV_SEC_PHY_MAX_TX_LEN + (remainLen==0?0:1);
			advSet->schMap         |= ADV_SCH_MAP_AUX_CHAIN;
			advSet->ea->chain.entry = (ll_adv_entry_t*)tx_malloc(chainCnt*sizeof(ll_adv_entry_t));
			advSet->ea->chain.cnt   = chainCnt;
			advSet->ea->aux.data.len = BLE_ADV_SEC_PHY_MAX_TX_LEN - BLE_ADV_EXTENDED_HEADER_MAX_LEN;
			_u16 offset           = advSet->ea->aux.data.len;
			for(_u8 i=0;i<chainCnt-1;i++)
			{
				advSet->ea->chain.entry[i].data.len = BLE_ADV_SEC_PHY_MAX_TX_LEN;
				advSet->ea->chain.entry[i].data.addr= (advSet->scanRsp.addr+offset);
				offset+=BLE_ADV_SEC_PHY_MAX_TX_LEN;
			}
			if(remainLen!=0)
			{
				advSet->ea->chain.entry[chainCnt-1].data.len = remainLen;
				advSet->ea->chain.entry[chainCnt-1].data.addr= (advSet->scanRsp.addr+offset);
			}
		}
	}
	advSet->ea->scanRspDatafragPerf = fragPref;
	return SUCCESS;
}

/**
 * Advertising disabled condition
 * 1.Host send "ll_set_extended_advertising_enable" with 'enable' field set to 0
 * 2.A connection is created using the advertising set.
 * 3.The duration field expires
 * 4.The number of extended advertising events for the set exceed the MAX_EXTENDED_ADVERTISING_EVENT.
 */
controller_error_code_e ll_set_extended_advertising_enable(_u8 enable,\
														   _u8 numSets,\
														   ll_extended_adv_enable_subField_e* pEnableSubFiled)
{
	//if the ll_set_extended_advertising_enable set again when the adv set is enabled,the timer for duration
	//and the number of events counter are reset,any change to random address shall take effect


	//disable advertising set will not disable periodic advertising

	//disable a disabled advertising set have no effect

	//if adv set is marked by more than one advHandle[i] parameter,should return error code 0x12
	if((enable != 0) && (numSets == 0))
	{
		return IVALID_HCI_COMMAND_PARAMETERS;
	}
	if(numSets>BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS)
	{
		return IVALID_HCI_COMMAND_PARAMETERS;
	}
	ll_sm_t* llsm = (ll_sm_t*)ll_get_sm_entity_by_state(BLE_LL_STATE_ADVERTISING,LL_SM_INVALID_HANDLE,0);
	if(POINTER_NOT_VALID(llsm)||POINTER_NOT_VALID(llsm->entity))
	{
		return MEMORY_CAPACITY_EXCEEDED;
	}
    ll_internal_adv_ctrl_t* adv = (ll_internal_adv_ctrl_t*)llsm->entity;
	if((enable == 0) && (numSets == 0))
	{
		for(int i=0;i<BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS;i++)
		{
			if(adv->set[i].handle <= 0xEF)
			{
				if((adv->set[i].eventProperty & LL_ADV_EVENT_PROPERTY_SCANNABLE)&&adv->set[i].scanRsp.len == 0)
				{
					return COMMAND_DISALLOWED;
				}
				if((adv->set[i].eventProperty & LL_ADV_EVENT_PROPERTY_CONNECTED)&&adv->set[i].data.len == 0)
				{
					return IVALID_HCI_COMMAND_PARAMETERS;//?,need check if valid
				}
			}
		}
	}
	else
	{
		for(_u8 i=0;i<numSets;i++)
		{
			ll_internal_adv_set_t* advSet = ll_extended_adv_get_adv_set(pEnableSubFiled[i].advHandle,0);
			if(POINTER_NOT_VALID(advSet))
			{
				return IVALID_HCI_COMMAND_PARAMETERS;
			}
			if((advSet->eventProperty & LL_ADV_EVENT_PROPERTY_SCANNABLE)&&advSet->scanRsp.len == 0)
			{
				return COMMAND_DISALLOWED;
			}
			if((advSet->eventProperty & LL_ADV_EVENT_PROPERTY_CONNECTED)&&advSet->data.len == 0)
			{
				return IVALID_HCI_COMMAND_PARAMETERS;//?,need check if valid
			}
		}
	}
	if((enable == 0) && (numSets == 0))
	{
		for(int i=0;i<BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS;i++)
		{
			if(adv->set[i].handle <= 0xEF)
			{
				adv->set[i].enable   = enable;
				if(POINTER_VALID(adv->set[i].ea))
				{
					if(pEnableSubFiled[i].duration!=0)
					{
						adv->set[i].ea->expireTime =  system_time() + adv->set[i].la.sch.interval + pEnableSubFiled[i].duration*10000;
					}
					else
					{
						adv->set[i].ea->expireTime = 0;
					}

					if(pEnableSubFiled[i].maxEvents!=0)
					{
						adv->set[i].ea->maxEvents    = pEnableSubFiled[i].maxEvents;
						adv->set[i].ea->eventCnt = 0;
					}
					else
					{
						adv->set[i].ea->maxEvents = 0;
					}
				}
			}
		}
	}
	else
	{
		for(_u8 i=0;i<numSets;i++)
		{
			ll_internal_adv_set_t* advSet = ll_extended_adv_get_adv_set(pEnableSubFiled[i].advHandle,0);
			if(pEnableSubFiled[i].duration!=0)
			{
				advSet->ea->expireTime =  system_time() + advSet->la.sch.interval + pEnableSubFiled[i].duration*10000;
			}
			else
			{
				advSet->ea->expireTime = 0;
			}

			if(pEnableSubFiled[i].maxEvents!=0)
			{
				advSet->ea->maxEvents    = pEnableSubFiled[i].maxEvents;
				advSet->ea->eventCnt = 0;
			}
			else
			{
				advSet->ea->maxEvents = 0;
			}
			if(!advSet->enable && enable)
			{
				advSet->enable = enable;
				advSet->schMap|=ADV_SCH_MAP_PRI;
				//la phy init
				advSet->la.phy.crcInit         = BLE_ADV_CRC_INIT;
				advSet->la.phy.accessCode      = BLE_ADV_ACCESS_CODE;
				advSet->la.phy.rxMaxOctets     = BLE_ADV_SEC_PHY_MAX_TX_LEN;
				advSet->la.phy.rxAddress       = ll_get_shared_phy_rx_address();
				advSet->la.phy.txAddress       = ll_get_shared_phy_tx_address();
				//la sch init
				advSet->la.availableChnCnt = advSet->la.channelCnt;
				advSet->la.eventCnt    = 0;
				advSet->la.sch.startMargin = 100;
				advSet->la.sch.stopMargin  = 100;
		        phy_obj_cast(&llsm->phy);
		        phy_obj_init(&llsm->phy);
                if(advSet->eventProperty&LL_ADV_EVENT_PROPERTY_LEGACY_PDU)
                {
                    if(advSet->eventType == ADV_EVENT_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED)
                    {
                    	advSet->la.sch.duration = llsm->phy.hw_get_prepare_time()+ll_get_air_packet_time(advSet->la.phy.mode,BLE_ADV_PRI_PHY_MAX_TX_LEN,0)+PACKET_DEFAULT_TIFS_TIME;
                    }
                    else
                    {
                    	advSet->la.sch.duration = llsm->phy.hw_get_prepare_time()+3*ll_get_air_packet_time(advSet->la.phy.mode,BLE_ADV_PRI_PHY_MAX_TX_LEN,0)+2*PACKET_DEFAULT_TIFS_TIME;
                    }
                }
                else
                {
					if(POINTER_NOT_VALID(advSet->ea->chain.entry))
					{
						advSet->ea->chain.cnt = 0;
					}
					advSet->la.sch.duration = llsm->phy.hw_get_prepare_time()+ll_get_air_packet_time(advSet->la.phy.mode,BLE_ADV_PRI_PHY_MAX_TX_LEN,0)+PACKET_DEFAULT_TIFS_TIME;
					advSet->ea->aux.phy.crcInit         = BLE_ADV_CRC_INIT;
					advSet->ea->aux.phy.accessCode      = BLE_ADV_ACCESS_CODE;
					advSet->ea->aux.phy.rxMaxOctets     = BLE_ADV_SEC_PHY_MAX_TX_LEN;
					advSet->ea->aux.phy.rxAddress       = ll_get_shared_phy_rx_address();
					advSet->ea->aux.phy.txAddress       = ll_get_shared_phy_tx_address();
					advSet->ea->eventCnt = 0;
					advSet->ea->aux.phy.mode            = advSet->ea->phyMode;
                    //ea sch and phy init
                	if(advSet->eventProperty&LL_ADV_EVENT_PROPERTY_CONNECTED)
                	{
                		advSet->ea->aux.sch.duration = llsm->phy.hw_get_prepare_time()+ll_get_air_packet_time(advSet->ea->aux.phy.mode,2+BLE_ADV_EXTENDED_HEADER_MAX_LEN+advSet->ea->aux.data.len,0)+PACKET_DEFAULT_TIFS_TIME\
												        + ll_get_air_packet_time(advSet->ea->aux.phy.mode,3+sizeof(scan_type_aux_scan_req_t),0)+PACKET_DEFAULT_TIFS_TIME\
												        + ll_get_air_packet_time(advSet->ea->aux.phy.mode,3+sizeof(init_type_auxConnectRsp_t),0);
					}
					else if(advSet->eventProperty&LL_ADV_EVENT_PROPERTY_SCANNABLE)
                	{
						advSet->ea->aux.sch.duration = llsm->phy.hw_get_prepare_time()+ll_get_air_packet_time(advSet->ea->aux.phy.mode,2+BLE_ADV_EXTENDED_HEADER_MAX_LEN,0)+PACKET_DEFAULT_TIFS_TIME\
											            + ll_get_air_packet_time(advSet->ea->aux.phy.mode,3+sizeof(scan_type_aux_scan_req_t),0)+PACKET_DEFAULT_TIFS_TIME\
												        + ll_get_air_packet_time(advSet->ea->aux.phy.mode,2+BLE_ADV_EXTENDED_HEADER_MAX_LEN+advSet->ea->aux.data.len,0);
					}
					else
					{
						advSet->ea->aux.sch.duration = llsm->phy.hw_get_prepare_time()+ll_get_air_packet_time(advSet->ea->aux.phy.mode,2+BLE_ADV_EXTENDED_HEADER_MAX_LEN+advSet->ea->aux.data.len,0);
					}

                    if(advSet->schMap&ADV_SCH_MAP_AUX_CHAIN)
                    {
                        for(int j=0;j<advSet->ea->chain.cnt;j++)
                        {
                        	advSet->ea->chain.entry[j].sch.startMargin = 100;
                            if(j == advSet->ea->chain.cnt-1)
                            {
                            	advSet->ea->chain.entry[j].sch.stopMargin  = 200;
                            }
                            else
                            {
                            	advSet->ea->chain.entry[j].sch.stopMargin  = 100;
                            }
                            advSet->ea->chain.entry[j].phy.crcInit    = BLE_ADV_CRC_INIT;
                            advSet->ea->chain.entry[j].phy.accessCode = BLE_ADV_ACCESS_CODE;
                            advSet->ea->chain.entry[j].phy.mode       = advSet->ea->phyMode;
                            advSet->ea->chain.entry[j].phy.txAddress  = ll_get_shared_phy_tx_address();
                            advSet->ea->chain.entry[j].sch.duration   = llsm->phy.hw_get_prepare_time()
                            		                                  + ll_get_air_packet_time(advSet->ea->chain.entry[j].phy.mode,2+BLE_ADV_EXTENDED_HEADER_CHAIN_PDU_MAX_LEN+advSet->ea->chain.entry[j].data.len,0);
                        }
                        advSet->ea->aux.sch.startMargin     = 100;
                        advSet->ea->aux.sch.stopMargin      = 100;//shall location the next event
                    }
                    else
                    {
                    	advSet->ea->aux.sch.startMargin     = 100;
                    	advSet->ea->aux.sch.stopMargin      = 200;//shall location the next event
                    }
                }
				#if(LL_SUPPORT_LE_PERIODIC_ADVERTISING)
                if(POINTER_VALID(advSet->pa))
                {
                	if(advSet->pa->enable&&(!advSet->pa->active))
                	{
                		advSet->pa->active = 1;
                	}
                }
				#endif
			}
			else if(advSet->enable && !enable)
			{
				advSet->enable = enable;
				advSet->inSch  = 0;
				advSet->state  = ADV_SM_STATE_IDLE;
			}
		}
	}
	if(enable)
	{
		if(ble_ll_enter_advertising_state())
		{
			return IVALID_HCI_COMMAND_PARAMETERS;
		}
	}
	else
	{
		_u8 gDisable = 1;
		for(int i=0;i<BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS;i++)
		{
			if(adv->set[i].handle!=LL_EXTENDED_ADV_INVALID_HANDLE)
			{
				if(adv->set[i].enable)
				{
					gDisable = 0;
				}
			}
		}
		if(gDisable)
		{
			if(ble_ll_exit_advertising_state())
			{
				return IVALID_HCI_COMMAND_PARAMETERS;
			}
		}
	}
	return SUCCESS;
}
controller_error_code_e ll_set_adv_set_random_address(_u8 advHandle,_u8* address)
{
	ll_internal_adv_set_t* advSet = ll_extended_adv_get_adv_set(advHandle,0);
	if(POINTER_NOT_VALID(advSet))
	{
		return UNKNOWN_ADVERTISING_IDENTIFIER;
	}
	if(advSet->enable &&\
	  (advSet->eventProperty&LL_ADV_EVENT_PROPERTY_CONNECTED))
	{
		return COMMAND_DISALLOWED;
	}
	txMemcpy(advSet->randomAddress,address,6);
	return SUCCESS;
}

controller_error_code_e ll_read_maximum_advertising_data_length(_u16* length)
{
	*length = BLE_ADV_MAXIMUM_ADVERTISING_DATA_LENGTH;
	return SUCCESS;
}

controller_error_code_e ll_read_number_of_supported_advertising_sets(_u8* number)
{
	*number = BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS;
	return SUCCESS;
}

controller_error_code_e ll_remove_advertising_sets(_u8 advHandle)
{
	ll_internal_adv_set_t* advSet = ll_extended_adv_get_adv_set(advHandle,0);
	if(POINTER_NOT_VALID(advSet))
	{
		return IVALID_HCI_COMMAND_PARAMETERS;
	}
	if(advSet->enable
	#if(LL_SUPPORT_LE_PERIODIC_ADVERTISING)
	  ||(POINTER_VALID(advSet->pa)&&advSet->pa->enable)
	#endif
	   )
	{
		return COMMAND_DISALLOWED;
	}
	if(POINTER_VALID(advSet->data.addr))
	{
		tx_free(advSet->data.addr);
		advSet->data.addr = NULL;
	}
	if(POINTER_VALID(advSet->scanRsp.addr))
	{
		tx_free(advSet->scanRsp.addr);
		advSet->scanRsp.addr = NULL;
	}
	if(POINTER_VALID(advSet->ea))
	{
		if(POINTER_VALID(advSet->ea->chain.entry))
		{
			tx_free((_u8*)advSet->ea->chain.entry);
			advSet->ea->chain.entry = NULL;
		}
		tx_free((_u8*)advSet->ea);
		advSet->ea = NULL;
	}
	txMemsetByte((_u8*)advSet,0,sizeof(ll_internal_adv_set_t));
	advSet->handle = LL_EXTENDED_ADV_INVALID_HANDLE;
	if(ll_extended_adv_get_current_set_number()==0)
	{
		ble_ll_exit_advertising_state();
	}
	return SUCCESS;
}

controller_error_code_e ll_clear_advertising_sets(void)
{
	ll_sm_t* llsm = (ll_sm_t*)ll_get_sm_entity_by_state(BLE_LL_STATE_ADVERTISING,LL_SM_INVALID_HANDLE,0);
	if(POINTER_NOT_VALID(llsm)||POINTER_NOT_VALID(llsm->entity))
	{
		return MEMORY_CAPACITY_EXCEEDED;
	}
	ble_ll_exit_advertising_state();
	ll_internal_adv_ctrl_t* adv = (ll_internal_adv_ctrl_t*)llsm->entity;
	for(int i=0;i<BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS;i++)
	{
		if(adv->set[i].handle!=LL_EXTENDED_ADV_INVALID_HANDLE)
		{
			ll_internal_adv_set_t* advSet = &adv->set[i];
			if(advSet->enable
			#if(LL_SUPPORT_LE_PERIODIC_ADVERTISING)
			  ||(POINTER_VALID(advSet->pa)&&advSet->pa->enable)
			#endif
			   )
			{
				return COMMAND_DISALLOWED;
			}
			if(POINTER_VALID(advSet->data.addr))
			{
				tx_free(advSet->data.addr);
				advSet->data.addr = NULL;
			}
			if(POINTER_VALID(advSet->scanRsp.addr))
			{
				tx_free(advSet->scanRsp.addr);
				advSet->scanRsp.addr = NULL;
			}
			if(POINTER_VALID(advSet->ea))
			{
				if(POINTER_VALID(advSet->ea->chain.entry))
				{
					tx_free((_u8*)advSet->ea->chain.entry);
					advSet->ea->chain.entry = NULL;
				}
				tx_free((_u8*)advSet->ea);
				advSet->ea = NULL;
			}
			txMemsetByte((_u8*)advSet,0,sizeof(ll_internal_adv_set_t));
			advSet->handle = LL_EXTENDED_ADV_INVALID_HANDLE;
		}
	}
	return SUCCESS;
}

#endif//LL_SUPPORT_LE_EXTENDED_ADVERTISING

#if(LL_SUPPORT_LE_PERIODIC_ADVERTISING)
#error"la don't need malloc,need process"
controller_error_code_e ll_set_periodic_advertising_paramters(_u8 advHandle,_u8 interval,_u16 property)
{
	ll_internal_adv_set_t* advSet = ll_extended_adv_get_adv_set(advHandle,0);
	if(POINTER_NOT_VALID(advSet))
	{
		return UNKNOWN_ADVERTISING_IDENTIFIER;
	}
	if(POINTER_NOT_VALID(advSet->pa))
	{
		advSet->pa = (ll_adv_type_pa_t*)tx_malloc(sizeof(ll_adv_type_pa_t));
	}
	if(property&LL_ADV_EVENT_PROPERTY_INCLUDE_TX_POWER)
	{
		advSet->pa->includeTxPower = 1;
	}
	advSet->pa->sync.sch.interval = 1250*interval;
	return SUCCESS;
}

controller_error_code_e ll_set_periodic_advertising_data(_u8 advHandle,ll_advertising_data_operation_e operation,_u8 dataLen,_u8* data)
{
	ll_internal_adv_set_t* advSet = ll_extended_adv_get_adv_set(advHandle,0);
	if(POINTER_NOT_VALID(advSet))
	{
		return UNKNOWN_ADVERTISING_IDENTIFIER;
	}
	if((!advSet->schMap&ADV_SCH_MAP_PA))
	{
		return COMMAND_DISALLOWED;
	}
	#if (LL_SUPPORT_PERIODIC_ADVERTISING_WITH_RESPONSES_ADVERTISER)
	if(advSet->schMap&ADV_SCH_MAP_PAWR)
	{
		return COMMAND_DISALLOWED;
	}
	#endif
	static _u32 dataFillOffset = 0;
	if(dataLen > BLE_ADV_MAXIMUM_ADVERTISING_DATA_LENGTH || (dataLen+dataFillOffset>BLE_ADV_MAXIMUM_ADVERTISING_DATA_LENGTH))
	{
		return MEMORY_CAPACITY_EXCEEDED;
	}
	if(ll_get_air_packet_time(advSet->ea->phyMode,dataFillOffset+dataLen,0)>(advSet->pa->sync.sch.interval*3/4))
	{
		return PACKET_TOO_LONG;
	}
	if(operation == LL_ADV_DATA_OPERATION_UNCHANGED)
	{
		if(advSet->pa->enable == 0\
		||POINTER_NOT_VALID(advSet->pa->data.addr)\
	    ||advSet->pa->data.len==0\
	    ||dataLen!=0)
		{
			return IVALID_HCI_COMMAND_PARAMETERS;
		}
	}
	if(dataLen==0\
	&&(operation!=LL_ADV_DATA_OPERATION_COMPLETE)\
	&&(operation!=LL_ADV_DATA_OPERATION_UNCHANGED))
	{
		return IVALID_HCI_COMMAND_PARAMETERS;
	}
	if(advSet->pa->enable\
	&&(operation!=LL_ADV_DATA_OPERATION_COMPLETE)\
	&&(operation!=LL_ADV_DATA_OPERATION_UNCHANGED))
	{
		return COMMAND_DISALLOWED;
	}
	if(POINTER_NOT_VALID(advSet->pa->data.addr))
	{
		if(operation == LL_ADV_DATA_OPERATION_COMPLETE)
		{
			advSet->pa->data.addr = tx_malloc(dataLen);
		}
		else
		{
			advSet->pa->data.addr = tx_malloc(BLE_ADV_MAXIMUM_ADVERTISING_DATA_LENGTH);
		}
	}
	switch(operation)
	{
		case LL_ADV_DATA_OPERATION_INTERMEDIATE_FRAGMENT:
		{
			txMemcpy4(advSet->pa->data.addr+dataFillOffset,data,dataLen);
			dataFillOffset+=dataLen;
		}
			break;
		case LL_ADV_DATA_OPERATION_FIRST_FRAGMENT:
		{
			txMemcpy4(advSet->pa->data.addr,data,dataLen);
			dataFillOffset=dataLen;
		}
			break;
		case LL_ADV_DATA_OPERATION_LAST_FRAGMENT:
		{
			txMemcpy4(advSet->pa->data.addr+dataFillOffset,data,dataLen);
			advSet->pa->data.len = dataFillOffset+dataLen;
			advSet->pa->did++;
			dataFillOffset = 0;
		}
			break;
		case LL_ADV_DATA_OPERATION_COMPLETE:
		{
			txMemcpy(advSet->pa->data.addr,data,dataLen);
			advSet->pa->did++;
			advSet->pa->data.len = dataLen;
		}
			break;
		case LL_ADV_DATA_OPERATION_UNCHANGED:
		{
			advSet->pa->did++;
		}
			break;
	}
	if(operation == LL_ADV_DATA_OPERATION_LAST_FRAGMENT||operation == LL_ADV_DATA_OPERATION_COMPLETE)
	{
		if(POINTER_VALID(advSet->pa->chain.entry))
		{
			tx_free((_u8*)advSet->pa->chain.entry);
			advSet->pa->chain.entry = NULL;
		}
		advSet->pa->sync.data.addr= advSet->pa->data.addr;
		//process data fragment
		if(advSet->pa->data.len<=(BLE_ADV_SEC_PHY_MAX_TX_LEN - BLE_ADV_EXTENDED_HEADER_MAX_LEN))
		{
			advSet->pa->chain.cnt= 0;//only aux packet exist
			advSet->pa->sync.data.len = advSet->pa->data.len;
		}
		else
		{
			_u8 remainLen         = ((advSet->pa->data.len-(BLE_ADV_SEC_PHY_MAX_TX_LEN - BLE_ADV_EXTENDED_HEADER_MAX_LEN)))%BLE_ADV_SEC_PHY_MAX_TX_LEN;
			_u8 chainCnt          = ((advSet->pa->data.len-(BLE_ADV_SEC_PHY_MAX_TX_LEN - BLE_ADV_EXTENDED_HEADER_MAX_LEN)))/BLE_ADV_SEC_PHY_MAX_TX_LEN + (remainLen==0?0:1);
			advSet->schMap         |= ADV_SCH_MAP_PA_CHAIN;
			advSet->pa->chain.entry = (ll_adv_entry_t*)tx_malloc(chainCnt*sizeof(ll_adv_entry_t));
			advSet->pa->chain.cnt   = chainCnt;
			advSet->pa->sync.data.len = BLE_ADV_SEC_PHY_MAX_TX_LEN - BLE_ADV_EXTENDED_HEADER_MAX_LEN;
			_u16 offset           = advSet->pa->sync.data.len;
			for(_u8 i=0;i<chainCnt-1;i++)
			{
				advSet->pa->chain.entry[i].data.len = BLE_ADV_SEC_PHY_MAX_TX_LEN;
				advSet->pa->chain.entry[i].data.addr= (advSet->pa->data.addr+offset);
				offset+=BLE_ADV_SEC_PHY_MAX_TX_LEN;
			}
			if(remainLen!=0)
			{
				advSet->pa->chain.entry[chainCnt-1].data.len = remainLen;
				advSet->pa->chain.entry[chainCnt-1].data.addr= (advSet->pa->data.addr+offset);
			}
		}
	}
	return SUCCESS;
}

controller_error_code_e ll_set_periodic_advertising_enable(_u8 enable,_u8 advHandle)
{
	ll_internal_adv_set_t* advSet = ll_extended_adv_get_adv_set(advHandle,0);
	ll_sm_t* ll = ll_get_current_state_machine();
	if(POINTER_NOT_VALID(advSet))
	{
		return UNKNOWN_ADVERTISING_IDENTIFIER;
	}
	if((!advSet->schMap&ADV_SCH_MAP_PA))
	{
		return COMMAND_DISALLOWED;
	}
	if(advSet->eventProperty&(LL_ADV_EVENT_PROPERTY_LEGACY_PDU|LL_ADV_EVENT_PROPERTY_CONNECTED|LL_ADV_EVENT_PROPERTY_SCANNABLE|LL_ADV_EVENT_PROPERTY_ANONYMOUS_ADV))
	{
		return COMMAND_DISALLOWED;
	}
	if(advSet->pa->enable&&(enable&BIT(0)))
	{
		//random address change
	}
	if((advSet->pa->enable == 0)&&(enable&BIT(0)))
	{
		advSet->pa->enable = 1;
		if(advSet->enable)
		{
			advSet->pa->active = 1;
		}
		advSet->pa->includeAdi = (enable&BIT(1)==0?0:1);
		advSet->schMap |= ADV_SCH_MAP_PA;
		phy_obj_cast(&ll->phy);
		advSet->pa->sync.phy.mode       = advSet->ea->phyMode;
		advSet->pa->sync.phy.rxMaxOctets= BLE_ADV_PRI_PHY_MAX_RX_LEN;
		advSet->pa->sync.phy.accessCode = 0x89762349;
		advSet->pa->sync.phy.crcInit    = 0x192874;
		advSet->pa->sync.phy.txAddress  = ll_get_shared_phy_tx_address();;
		advSet->pa->sync.phy.rxAddress  = ll_get_shared_phy_rx_address();


		advSet->pa->sync.sch.startMargin= 100;
		advSet->pa->sync.sch.stopMargin = 100;
		advSet->pa->sync.sch.duration   = ll->phy.hw_get_prepare_time()+ll_get_air_packet_time(advSet->pa->sync.phy.mode,2+BLE_ADV_SEC_PHY_MAX_TX_LEN,0);
		advSet->pa->sync.sch.anchorPoint= system_time()+2000;//todo,periodic task get anchor point from planner
	}
	if((advSet->pa->enable == 1)&&(!(enable&BIT(0))))
	{
		advSet->pa->enable = 0;
		advSet->pa->active = 0;
		advSet->schMap &= (~ADV_SCH_MAP_PA);
		//disable
	}
}

#endif//LL_SUPPORT_LE_PERIODIC_ADVERTISING
