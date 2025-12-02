
#include"adv.h"
#include"adv_internal.h"
#include"system/scheduler/sch_map.h"
#include"../../channel/channel.h"
/*****************************************ADV State Machine***********************************************/


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

typedef int(*adv_event_sm_cb)(ll_sm_t* ll,ll_internal_adv_param_t* advParam);
typedef struct _PACKED
{
    adv_event_sm_cb cb;
    adv_context_e   context;
    adv_sm_state_e  currentState;
    adv_sm_state_e  transSuccessState;
    adv_sm_state_e  transFailState;
    ll_event_e      event;
}adv_event_sm_t; 

/*****************************************ADV Procedure ***********************************************/
typedef struct
{
    adv_event_class_e eventClass;
    adv_event_sm_t*   sm;
    _u32              listLen;
    _u32              context;
}adv_procedure_list_t;

/*****************************************ADV Sequence ***********************************************/
typedef struct
{
    adv_event_type_e       eventType;//adv mode
    adv_procedure_list_t   *procedureList;
    _u32                   listLen;
}adv_sequence_t;

#define ADV_PROCEDURE_LIST_LENGTH(adv_procedure_list)      (sizeof(adv_procedure_list)/sizeof(adv_procedure_list[0]))
#define ADV_SEQUENCE_LIST_LENGTH(adv_sequence_list)        (sizeof(adv_sequence_list)/sizeof(adv_sequence_list[0]))
#define ADV_SM_LIST_LENGTH(adv_sm_list)                    (sizeof(adv_sm_list)/sizeof(adv_sm_list[0]))


static ll_internal_adv_param_t* currentAdvSet;
static int currentEventClass;

_RAM_CODE
static void adv_sub_node_remap(ll_sm_t* ll,ll_adv_sch_entry_t* advSch)
{
    ll->sch.timestamp    = advSch->anchorPoint;
    ll->sch.duration     = ll->sch.durationMin= advSch->duration;
    ll->sch.startLatency = advSch->startMargin;
    ll->sch.stopLatency  = advSch->stopMargin;
    ll->sch.period       = advSch->interval;
}

_RAM_CODE 
void adv_get_next_event(ll_sm_t* ll)
{
	#if(LL_SUPPORT_LE_EXTENDED_ADVERTISING)
    _u32 timestamp = 0;
    for(_u8 i=0;i<BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS;i++)
    {
        if(ll->adv->param[i].enable)
        {
            if(POINTER_VALID(ll->adv->param[i].la))
            {
                if((timestamp == 0)||txCompareTime(timestamp,ll->adv->param[i].la->sch.anchorPoint))
                {
                    timestamp = ll->adv->param[i].la->sch.anchorPoint|1;
                    currentAdvSet = &ll->adv->param[i];
                    currentEventClass = ADV_EVENT;
                    adv_sub_node_remap(ll,&currentAdvSet->la->sch);
                }
            }
            if(ll->adv->param[i].schMap&ADV_SCH_MAP_AUX)
            {
                if((timestamp == 0)||txCompareTime(timestamp,ll->adv->param[i].ea->anchor))
                {
                    timestamp = ll->adv->param[i].ea->anchor|1;
                    currentAdvSet = &ll->adv->param[i];
                    currentAdvSet->pChain = &currentAdvSet->ea->chain;
                    if(ll->adv->param[i].ea->anchor == ll->adv->param[i].ea->aux.sch.anchorPoint)
                    {
                        adv_sub_node_remap(ll,&currentAdvSet->ea->aux.sch);
                        currentEventClass = ADV_EXTENDED_EVENT;
                    }
                    else if(ll->adv->param[i].schMap&ADV_SCH_MAP_AUX_CHAIN)
                    {
                        adv_sub_node_remap(ll,&currentAdvSet->ea->chain.entry[currentAdvSet->ea->chain.current].sch);
                        currentEventClass = ADV_CHAINED_EVENT;
                    }
                }
            }
        }
		#if(LL_SUPPORT_LE_PERIODIC_ADVERTISING)
		if((ll->adv->param[i].schMap&ADV_SCH_MAP_PA)&&ll->adv->param[i].pa->active)
		{
			if((timestamp == 0)||txCompareTime(timestamp,ll->adv->param[i].pa->anchor))
			{
				timestamp = ll->adv->param[i].pa->anchor|1;
				currentAdvSet = &ll->adv->param[i];
                currentAdvSet->pChain = &currentAdvSet->pa->chain;
				if(ll->adv->param[i].pa->anchor == ll->adv->param[i].pa->sync.sch.anchorPoint)
				{
					currentEventClass = ADV_PERIODIC_EVENT;
					adv_sub_node_remap(ll,&currentAdvSet->pa->sync.sch);
				}
				else if(ll->adv->param[i].schMap&ADV_SCH_MAP_PA_CHAIN)
				{
					currentEventClass = ADV_CHAINED_EVENT;
					adv_sub_node_remap(ll,&currentAdvSet->pa->chain.entry[currentAdvSet->pa->chain.current].sch);
				}
			}
		}
		#endif/*(LL_SUPPORT_LE_PERIODIC_ADVERTISING)*/
    }
    #if(LL_SUPPORT_PERIODIC_ADVERTISING_WITH_RESPONSES_ADVERTISER)
    #endif/*(LL_SUPPORT_PERIODIC_ADVERTISING_WITH_RESPONSES_ADVERTISER)*/

	#else/*(!LL_SUPPORT_LE_EXTENDED_ADVERTISING)*/
    currentAdvSet = &ll->adv->param[0];
    currentEventClass = ADV_EVENT;
    adv_sub_node_remap(ll,&currentAdvSet->la->sch);
	#endif/*(LL_SUPPORT_LE_EXTENDED_ADVERTISING)*/
}

/**
 * @param timestamp - if present,timestamp is air packet anchor point,so need minus hardware prepare time
 * 				    - if not present,packet send time is not specified,so we can use system time.
 */
_RAM_CODE
static void adv_prepare_phy(ll_sm_t* ll,ll_adv_phy_entry_t* phy,_u32 timestamp,phy_dir_e phydir)
{
    phy_obj_cast(&ll->phy);
    ll->phy.accessCode = phy->accessCode;
    ll->phy.crcInit    = phy->crcInit;
    ll->phy.mode       = phy->mode;
    ll->phy.chnIdx     = phy->chn;
    ll->phy.dir        = phydir;
    if(timestamp!=0)
    {
        ll->phy.timestamp  = timestamp - ll->phy.hw_get_prepare_time();
    }
    else
    {
        ll->phy.timestamp  = system_time();
    }
    if(phydir == PHY_DIR_TX)
    {
        ll->phy.txAddress   = phy->txAddress;
    }
    else if(phydir == PHY_DIR_RX)
    {
    	ll->phy.rxAddress   = phy->rxAddress;
        ll->phy.rxTimeout   = BLE_ADV_DEFAULT_RX_TIMEOUT_US;
        ll->phy.rxMaxOctets = phy->rxMaxOctets;
    }
}

_RAM_CODE
int ll_extended_adv_map_out_task(ll_sm_t* ll,ll_internal_adv_param_t* advParam,_u32 refStart,_u32 refEnd,_u8 mapType)
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
            	if(schNode->llId == ll->sch.llId)
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
            if((ll->adv->param[i].handle == advParam[i].handle) || (ll->adv->param[i].enable == 0))
            {
                continue;
            }
            if(ll->adv->param[i].enable)
            {
                if(txCompareTime(refEnd,ll->adv->param[i].la->sch.anchorPoint))
                {
                    node[nodeNum].start = ll->adv->param[i].la->sch.anchorPoint - ll->adv->param[i].la->sch.startMargin;
                    node[nodeNum].end   = node[nodeNum].start+3*(ll->adv->param[i].la->sch.duration + ll->adv->param[i].la->sch.startMargin+ll->adv->param[i].la->sch.stopMargin);
                    node[nodeNum].type  = SCH_SPORADIC_TASK;
                    nodeNum++;
                    if(nodeNum>nodeCount)
                    {
                        goto reCal;
                    }
                }
                if(ll->adv->param[i].schMap&ADV_SCH_MAP_AUX)
                {
                    if(txCompareTime(refEnd,ll->adv->param[i].ea->aux.sch.anchorPoint))
                    {
                        node[nodeNum].start = ll->adv->param[i].ea->aux.sch.anchorPoint - ll->adv->param[i].ea->aux.sch.startMargin;
                        node[nodeNum].end   = ll->adv->param[i].ea->aux.sch.anchorPoint + ll->adv->param[i].ea->aux.sch.duration + ll->adv->param[i].ea->aux.sch.stopMargin;
                        node[nodeNum].type  = SCH_SPORADIC_TASK;
                        nodeNum++;
                        if(nodeNum>nodeCount)
                        {
                            goto reCal;
                        }
                    }
                    if(ll->adv->param[i].schMap&ADV_SCH_MAP_AUX_CHAIN)
                    {
                        for(int j=0;j<ll->adv->param[i].ea->chain.cnt;j++)
                        {
                            if(txCompareTime(refEnd,ll->adv->param[i].ea->chain.entry[j].sch.anchorPoint))
                            {
                                node[nodeNum].start = ll->adv->param[i].ea->chain.entry[j].sch.anchorPoint\
                                                    - ll->adv->param[i].ea->chain.entry[j].sch.startMargin;

                                node[nodeNum].end   = ll->adv->param[i].ea->chain.entry[j].sch.anchorPoint\
                                                    + ll->adv->param[i].ea->chain.entry[j].sch.duration\
                                                    + ll->adv->param[i].ea->chain.entry[j].sch.stopMargin;
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
    		if(POINTER_VALID(ll->adv->param[i].pa)&&ll->adv->param[i].pa->active)
            {
    			if(ll->adv->param[i].schMap&ADV_SCH_MAP_PA)
    			{
    				if(txCompareTime(refEnd,ll->adv->param[i].pa->sync.sch.anchorPoint))
    				{
                        node[nodeNum].start = ll->adv->param[i].pa->sync.sch.anchorPoint - ll->adv->param[i].pa->sync.sch.startMargin;
                        node[nodeNum].end   = ll->adv->param[i].pa->sync.sch.anchorPoint + ll->adv->param[i].pa->sync.sch.duration + ll->adv->param[i].pa->sync.sch.stopMargin;
                        node[nodeNum].type  = SCH_PERIODIC_TASK;
                        nodeNum++;
                        if(nodeNum>nodeCount)
                        {
                            goto reCal;
                        }
    				}
    				if(ll->adv->param[i].schMap&ADV_SCH_MAP_PA_CHAIN)
    				{
                        for(int j=0;j<ll->adv->param[i].pa->chain.cnt;j++)
                        {
                            if(txCompareTime(refEnd,ll->adv->param[i].pa->chain.entry[j].sch.anchorPoint))
                            {
                                node[nodeNum].start = ll->adv->param[i].pa->chain.entry[j].sch.anchorPoint\
                                                    - ll->adv->param[i].pa->chain.entry[j].sch.startMargin;

                                node[nodeNum].end   = ll->adv->param[i].pa->chain.entry[j].sch.anchorPoint\
                                                    + ll->adv->param[i].pa->chain.entry[j].sch.duration\
                                                    + ll->adv->param[i].pa->chain.entry[j].sch.stopMargin;
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
    DEBUG_GPIO_HIGH(GPIO_15);
    sch_map_calculate_free_space_by_time(refStart,refEnd,node,nodeNum,&freeBlock,&freeBlockCount);
    DEBUG_GPIO_LOW(GPIO_15);
    int blockIndex = 0;
    if(mapType&ADV_SCH_MAP_PRI)
    {
        _u32 primarySpace = 3*(advParam->la->sch.startMargin + advParam->la->sch.stopMargin + advParam->la->sch.duration);
        for(;blockIndex<freeBlockCount;blockIndex++)
        {
            if((freeBlock[blockIndex].end - freeBlock[blockIndex].start)>(primarySpace+PACKET_T_MAFS_TIME))
            {
                advParam->la->sch.anchorPoint = freeBlock[blockIndex].start+advParam->la->sch.startMargin;
                freeBlock[blockIndex].start +=(primarySpace+PACKET_T_MAFS_TIME);//todo,check need space how much
                break;
            }
        }
    }
	#if(LL_SUPPORT_LE_EXTENDED_ADVERTISING)
    if(((mapType&ADV_SCH_MAP_AUX))&&(advParam->schMap&ADV_SCH_MAP_AUX))
    {
        _u32 secondarySpace = advParam->ea->aux.sch.startMargin\
        		            + advParam->ea->aux.sch.duration\
							+ advParam->ea->aux.sch.stopMargin;
        for(;blockIndex<freeBlockCount;blockIndex++)
        {
            if((freeBlock[blockIndex].end - freeBlock[blockIndex].start)>(secondarySpace+PACKET_T_MAFS_TIME))
            {
                advParam->ea->aux.sch.anchorPoint = freeBlock[blockIndex].start+advParam->ea->aux.sch.startMargin;
                advParam->ea->anchor = advParam->ea->aux.sch.anchorPoint;
                freeBlock[blockIndex].start +=(secondarySpace+PACKET_T_MAFS_TIME);//todo,check need space how much
                break;
            }
        }
        if((mapType&ADV_SCH_MAP_AUX_CHAIN)&&(advParam->schMap&ADV_SCH_MAP_AUX_CHAIN))
        {

            for(int j=0;j<advParam->ea->chain.cnt;j++)
            {
                _u32 chainSpace = advParam->ea->chain.entry[j].sch.startMargin\
                                + advParam->ea->chain.entry[j].sch.duration\
                                + advParam->ea->chain.entry[j].sch.stopMargin;
                for(;blockIndex<freeBlockCount;blockIndex++)
                {
                    if((freeBlock[blockIndex].end - freeBlock[blockIndex].start)>(chainSpace+PACKET_T_MAFS_TIME))
                    {
                        advParam->ea->chain.entry[j].sch.anchorPoint = freeBlock[blockIndex].start+advParam->ea->chain.entry[j].sch.startMargin;
                        freeBlock[blockIndex].start +=(chainSpace+PACKET_T_MAFS_TIME);//todo,check need space how much
                        break;
                    }
                }
            }
        }
    }
	#if(LL_SUPPORT_LE_PERIODIC_ADVERTISING)
    if(((mapType&ADV_SCH_MAP_PA))&&(advParam->schMap&ADV_SCH_MAP_PA))
    {//attention,chain of periodic sync pdu should greater than sync anchor and smaller than sync anchor plus sync interval
        if((mapType&ADV_SCH_MAP_PA_CHAIN)&&(advParam->schMap&ADV_SCH_MAP_PA_CHAIN))
        {
            for(int j=0;j<advParam->pa->chain.cnt;j++)
            {
                _u32 chainSpace = advParam->pa->chain.entry[j].sch.startMargin\
                                + advParam->pa->chain.entry[j].sch.duration\
                                + advParam->pa->chain.entry[j].sch.stopMargin;
                for(;blockIndex<freeBlockCount;blockIndex++)
                {
                    if((freeBlock[blockIndex].end - freeBlock[blockIndex].start)>(chainSpace+PACKET_T_MAFS_TIME))
                    {
                        advParam->pa->chain.entry[j].sch.anchorPoint = freeBlock[blockIndex].start+advParam->pa->chain.entry[j].sch.startMargin;
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
_RAM_CODE 

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

typedef struct 
{
    _u32 anchorPoint;
    _u32 targetAnchorPoint;
    _u8  phy;
    _u8  channel;
}adv_extended_header_auxInfo_t;

static void adv_generate_extended_header(ll_sm_t* ll,ll_internal_adv_param_t* advParam,_u8* packet,_u8 advMode,_u8 flags,adv_extended_header_auxInfo_t* auxInfo,_u16 did)
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
        txMemcpy(extHeader->param+offset,advParam->peerAddress,6);
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
        ((adv_extended_header_subfield_adi_t*)(extHeader->param+offset))->sid = advParam->sid;
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
    	((adv_extended_header_subfield_syncInfo_t*)(extHeader->param+offset))->interval   = advParam->pa->sync.sch.interval;
    	//todo with access code,crc init,sca
    	((adv_extended_header_subfield_syncInfo_t*)(extHeader->param+offset))->chMH       = 0x1f;
    	((adv_extended_header_subfield_syncInfo_t*)(extHeader->param+offset))->chML       = 0xffffffff;
    	((adv_extended_header_subfield_syncInfo_t*)(extHeader->param+offset))->AA         = 0x23411243;
    	((adv_extended_header_subfield_syncInfo_t*)(extHeader->param+offset))->crcInit[0] = 0x32;
    	((adv_extended_header_subfield_syncInfo_t*)(extHeader->param+offset))->crcInit[1] = 0x54;
    	((adv_extended_header_subfield_syncInfo_t*)(extHeader->param+offset))->crcInit[2] = 0x76;
    	((adv_extended_header_subfield_syncInfo_t*)(extHeader->param+offset))->sca        = 3;
    	_u32 stepCnt = 0;
    	_u32 anchorTime = advParam->ea->aux.sch.anchorPoint;
    	_u32 targetTime = advParam->pa->sync.sch.anchorPoint;
    	if(txCompare(anchorTime,targetTime))
    	{
    		stepCnt = (anchorTime-targetTime)/advParam->pa->sync.sch.interval;
    		stepCnt++;
    		targetTime+=(stepCnt*advParam->pa->sync.sch.interval);
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
        ((adv_extended_header_subfield_syncInfo_t*)(extHeader->param+offset))->eventCounter   = advParam->pa->eventCnt+stepCnt;
		#endif
        offset+=sizeof(adv_extended_header_subfield_syncInfo_t);
    }
    if(flags & ADV_EXTENDED_HEADER_FLAG_TX_POWER)
    {
        offset+=sizeof(adv_extended_header_subfield_Tx_Power_t);
    }
    extHeader->len = 1+offset;
}
#endif

//ADV_EVENT_CONNECTABLE_SCANNABLE_UNDIRECTED

//LL_ADV_TYPE_ADV_IND
static void adv_ind_pdu_prepare(ll_sm_t* ll,ll_internal_adv_param_t* advParam)
{
    _u8* packet = ll_adv_packet_data_prepare(advParam->la->phy.txAddress,6+advParam->data.len,LL_ADV_TYPE_ADV_IND);
    txMemcpy(((adv_type_ind_t*)packet)->advA,ll_get_device_address(),6);
    txMemcpy(((adv_type_ind_t*)packet)->advData,advParam->data.addr,advParam->data.len);
    ll_adv_packet_make(advParam->la->phy.txAddress,LL_CHANNEL_SUPP_CSA2,advParam->ownAddressType?1:0,0);
}
//LL_ADV_TYPE_ADV_DIRECT_IND
static void adv_direct_ind_pdu_prepare(ll_sm_t* ll,ll_internal_adv_param_t* advParam)
{
    _u8* packet = ll_adv_packet_data_prepare(advParam->la->phy.txAddress,12,LL_ADV_TYPE_ADV_DIRECT_IND);
    txMemcpy(((adv_type_direct_ind_t*)packet)->advA,ll_get_device_address(),6);
    txMemcpy(((adv_type_direct_ind_t*)packet)->targetA,advParam->peerAddress,6);
    ll_adv_packet_make(advParam->la->phy.txAddress,LL_CHANNEL_SUPP_CSA2,advParam->ownAddressType?1:0,advParam->peerAddressType?1:0);
}
//LL_ADV_TYPE_ADV_NONCONN_IND
static void adv_non_conn_pdu_prepare(ll_sm_t* ll,ll_internal_adv_param_t* advParam)
{
    _u8* packet = ll_adv_packet_data_prepare(advParam->la->phy.txAddress,6+advParam->data.len,LL_ADV_TYPE_ADV_NONCONN_IND);
    txMemcpy(((adv_type_nonConn_ind_t*)packet)->advA,ll_get_device_address(),6);
    txMemcpy(((adv_type_nonConn_ind_t*)packet)->advData,advParam->data.addr,advParam->data.len);
    ll_adv_packet_make(advParam->la->phy.txAddress,0,advParam->ownAddressType?1:0,0);
}
//LL_ADV_TYPE_SCAN_RSP
static void adv_scan_rsp_pdu_prepare(ll_sm_t* ll,ll_internal_adv_param_t* advParam)
{
    _u8* packet = ll_adv_packet_data_prepare(advParam->la->phy.txAddress,6+advParam->scanRsp.len,LL_ADV_TYPE_SCAN_RSP);
    txMemcpy(((adv_type_scan_rsp_t*)packet)->advA,ll_get_device_address(),6);
    txMemcpy(((adv_type_scan_rsp_t*)packet)->scanRsp,advParam->scanRsp.addr,advParam->scanRsp.len);
    ll_adv_packet_make(advParam->la->phy.txAddress,0,advParam->ownAddressType?1:0,0);
}
//LL_ADV_TYPE_ADV_SCAN_IND
static void adv_scan_ind_pdu_prepare(ll_sm_t* ll,ll_internal_adv_param_t* advParam)
{
    _u8* packet = ll_adv_packet_data_prepare(advParam->la->phy.txAddress,6+advParam->data.len,LL_ADV_TYPE_ADV_SCAN_IND);
    txMemcpy(((adv_type_scan_ind_t*)packet)->advA,ll_get_device_address(),6);
    txMemcpy(((adv_type_scan_ind_t*)packet)->advData,advParam->data.addr,advParam->data.len);
    ll_adv_packet_make(advParam->la->phy.txAddress,0,advParam->ownAddressType?1:0,0);
}
#if(LL_SUPPORT_LE_EXTENDED_ADVERTISING)
//LL_ADV_TYPE_ADV_EXT_IND
static void adv_ext_ind_pdu_prepare(ll_sm_t* ll,ll_internal_adv_param_t* advParam,_u8 advMode,_u8 flags,adv_extended_header_auxInfo_t* auxInfo,_u16 did)
{
    _u16 headerLen = adv_calculate_extended_header_length(flags);
    _u8* packet = ll_adv_packet_data_prepare(advParam->la->phy.txAddress,headerLen,LL_ADV_TYPE_ADV_EXT_IND);
    adv_generate_extended_header(ll,advParam,packet,advMode,flags,auxInfo,did);
    ll_adv_packet_make(advParam->la->phy.txAddress,0,advParam->ownAddressType?1:0,0);
}

//LL_ADV_TYPE_AUX_ADV_IND
static void adv_aux_adv_ind_pdu_prepare(ll_sm_t* ll,ll_internal_adv_param_t* advParam,_u8 advMode,_u8 flags,adv_extended_header_auxInfo_t* auxInfo,_u16 did)
{
    _u16 headerLen = adv_calculate_extended_header_length(flags);
    _u16 dataLen = advParam->ea->aux.data.len;
    if(advParam->eventProperty&LL_ADV_EVENT_PROPERTY_SCANNABLE)
    {
    	dataLen = 0;
    }
    _u8* packet = ll_adv_packet_data_prepare(advParam->ea->aux.phy.txAddress,headerLen+dataLen,LL_ADV_TYPE_AUX_ADV_IND);
    adv_generate_extended_header(ll,advParam,packet,advMode,flags,auxInfo,did);
    if(dataLen!=0)
    {
        txMemcpy(packet+headerLen,advParam->ea->aux.data.addr,dataLen);
    }
    ll_adv_packet_make(advParam->ea->aux.phy.txAddress,0,advParam->ownAddressType?1:0,0);
}
//LL_ADV_TYPE_AUX_SCAN_RSP
static void adv_aux_scan_rsp_pdu_prepare(ll_sm_t* ll,ll_internal_adv_param_t* advParam,_u8 advMode,_u8 flags,adv_extended_header_auxInfo_t* auxInfo,_u16 did)
{
    _u16 headerLen = adv_calculate_extended_header_length(flags);
    _u8* packet = ll_adv_packet_data_prepare(advParam->ea->aux.phy.txAddress,headerLen+advParam->ea->aux.data.len,LL_ADV_TYPE_AUX_SCAN_RSP);
    adv_generate_extended_header(ll,advParam,packet,advMode,flags,auxInfo,did);
    if(advParam->ea->aux.data.len!=0)
    {
        txMemcpy(packet+headerLen,advParam->ea->aux.data.addr,advParam->ea->aux.data.len);
    }
    ll_adv_packet_make(advParam->ea->aux.phy.txAddress,0,advParam->ownAddressType?1:0,0);
}
//LL_ADV_TYPE_AUX_CONNECT_RSP
static void adv_aux_conn_rsp_pdu_prepare(ll_sm_t* ll,ll_internal_adv_param_t* advParam,_u8 advMode,_u8 flags,adv_extended_header_auxInfo_t* auxInfo,_u16 did)
{
    _u16 headerLen = adv_calculate_extended_header_length(flags);
    _u8* packet = ll_adv_packet_data_prepare(advParam->ea->aux.phy.txAddress,headerLen,LL_ADV_TYPE_AUX_CONNECT_RSP);
    adv_generate_extended_header(ll,advParam,packet,advMode,flags,auxInfo,did);
    ll_adv_packet_make(advParam->ea->aux.phy.txAddress,0,advParam->ownAddressType?1:0,0);
}
//LL_ADV_TYPE_AUX_CHAIN_IND
static void adv_aux_chain_ind_pdu_prepare(ll_sm_t* ll,ll_internal_adv_param_t* advParam,_u8 advMode,_u8 flags,adv_extended_header_auxInfo_t* auxInfo,_u16 did)
{
    _u16 headerLen = adv_calculate_extended_header_length(flags);
    _u8* packet = ll_adv_packet_data_prepare(advParam->pChain->entry[advParam->pChain->current].phy.txAddress,headerLen+advParam->ea->chain.entry[advParam->pChain->current].data.len,LL_ADV_TYPE_AUX_CHAIN_IND);
    adv_generate_extended_header(ll,advParam,packet,advMode,flags,auxInfo,did);
    if(advParam->pChain->entry[advParam->pChain->current].data.len!=0)
    {
        txMemcpy(packet+headerLen,advParam->pChain->entry[advParam->pChain->current].data.addr,advParam->pChain->entry[advParam->pChain->current].data.len);
    }
    ll_adv_packet_make(advParam->pChain->entry[advParam->pChain->current].phy.txAddress,0,advParam->ownAddressType?1:0,0);
}
#endif
#if(LL_SUPPORT_LE_PERIODIC_ADVERTISING)
//LL_ADV_TYPE_AUX_SYNC_IND
static void adv_aux_sync_ind_pdu_prepare(ll_sm_t* ll,ll_internal_adv_param_t* advParam,_u8 advMode,_u8 flags,adv_extended_header_auxInfo_t* auxInfo,_u16 did)
{
    _u16 headerLen = adv_calculate_extended_header_length(flags);
    _u8* packet = ll_adv_packet_data_prepare(advParam->pa->sync.phy.txAddress,headerLen+advParam->pa->sync.data.len,LL_ADV_TYPE_AUX_SYNC_IND);
    adv_generate_extended_header(ll,advParam,packet,advMode,flags,auxInfo,did);
    if(advParam->pa->sync.data.len!=0)
    {
        txMemcpy(packet+headerLen,advParam->pa->sync.data.addr,advParam->pa->sync.data.len);
    }
    ll_adv_packet_make(advParam->pa->sync.phy.txAddress,0,advParam->ownAddressType?1:0,0);
}
#endif

#if(LL_SUPPORT_PERIODIC_ADVERTISING_WITH_RESPONSES_ADVERTISER)
//LL_ADV_TYPE_AUX_SYNC_SUBEVENT_IND
static void adv_aux_sync_subevent_ind_pdu_prepare(ll_sm_t* ll,ll_internal_adv_param_t* advParam)
{

}
//LL_ADV_TYPE_AUX_SYNC_SUBEVENT_RSP
static void adv_aux_sync_subevent_rsp_pdu_prepare(ll_sm_t* ll,ll_internal_adv_param_t* advParam)
{

}
#endif
_RAM_CODE
static void adv_event_connectable_scannable_undirected_packet_prapare(ll_sm_t* ll,ll_internal_adv_param_t* advParam,adv_pdu_class_e pduClass)
{
    _u8* packet = NULL;
    switch(pduClass)
    {
        case ADV_PDU_CLASS_LEG:
        {
            adv_ind_pdu_prepare(ll,advParam);
        }
        break;
        case ADV_PDU_CLASS_SCAN_RSP:
        {   
            adv_scan_rsp_pdu_prepare(ll,advParam);
        } 
        break;
    }
}
//ADV_EVENT_CONNECTABLE_DIRECTED
_RAM_CODE
static void adv_event_connectable_directed_packet_prapare(ll_sm_t* ll,ll_internal_adv_param_t* advParam,adv_pdu_class_e pduClass)
{
    _u8* packet = NULL;
    switch(pduClass)
    {
        case ADV_PDU_CLASS_LEG:
        {
            adv_direct_ind_pdu_prepare(ll,advParam);
        }
        break;
    }
}
//ADV_EVENT_SCANNABLE_UNDIRECTED
_RAM_CODE
static void adv_event_scannable_undirected_packet_prapare(ll_sm_t* ll,ll_internal_adv_param_t* advParam,adv_pdu_class_e pduClass)
{
    _u8* packet = NULL;
    switch(pduClass)
    {
        case ADV_PDU_CLASS_LEG:
        {
            adv_scan_ind_pdu_prepare(ll,advParam);
        }
        break;
        case ADV_PDU_CLASS_SCAN_RSP:
        {   
            adv_scan_rsp_pdu_prepare(ll,advParam);
        } 
        break;
    }
}
//ADV_EVENT_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED
_RAM_CODE
static void adv_event_non_connectable_non_scannable_undirected_packet_prapare(ll_sm_t* ll,ll_internal_adv_param_t* advParam,adv_pdu_class_e pduClass)
{
    _u8* packet = NULL;
    switch(pduClass)
    {
        case ADV_PDU_CLASS_LEG:
        {
            adv_non_conn_pdu_prepare(ll,advParam);
        }
        break;
    }
}
#if(LL_SUPPORT_LE_EXTENDED_ADVERTISING)
//ADV_EVENT_EXTENDED_CONNECTABLE_DIRECTED
_RAM_CODE
static void adv_event_extended_connectable_directed_packet_prapare(ll_sm_t* ll,ll_internal_adv_param_t* advParam,adv_pdu_class_e pduClass)
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
            auxInfo.anchorPoint       = advParam->la->sch.anchorPoint;
            auxInfo.targetAnchorPoint = advParam->ea->aux.sch.anchorPoint;
            auxInfo.channel           = advParam->ea->aux.phy.chn;
            auxInfo.phy               = advParam->ea->aux.phy.mode;
            adv_ext_ind_pdu_prepare(ll,advParam,advMode,flags,&auxInfo,advParam->ea->did);
        }
        break;
        case ADV_PDU_CLASS_AUX:
        {
            flags = ADV_EXTENDED_HEADER_FLAG_ADV_A|ADV_EXTENDED_HEADER_FLAG_TARGET_A|ADV_EXTENDED_HEADER_FLAG_ADI;
            if(advParam->ea->power!=0x7f)
            {   
                flags|=ADV_EXTENDED_HEADER_FLAG_TX_POWER;
            }
            advMode = 1; 
            adv_aux_adv_ind_pdu_prepare(ll,advParam,advMode,flags,&auxInfo,advParam->ea->did);
        }
        break;
        case ADV_PDU_CLASS_CONN_RSP:
        {
            flags = ADV_EXTENDED_HEADER_FLAG_ADV_A|ADV_EXTENDED_HEADER_FLAG_TARGET_A;
            adv_aux_conn_rsp_pdu_prepare(ll,advParam,advMode,flags,&auxInfo,advParam->ea->did);
        }
        break;
    }
}

volatile _u8 AAVV_PACKET[50];
//ADV_EVENT_EXTENDED_CONNECTABLE_UNDIRECTED
_RAM_CODE
static void adv_event_extended_connectable_undirected_packet_prapare(ll_sm_t* ll,ll_internal_adv_param_t* advParam,adv_pdu_class_e pduClass)
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
            auxInfo.anchorPoint       = advParam->la->sch.anchorPoint;
            auxInfo.targetAnchorPoint = advParam->ea->aux.sch.anchorPoint;
            auxInfo.channel           = advParam->ea->aux.phy.chn;
            auxInfo.phy               = advParam->ea->aux.phy.mode;
            adv_ext_ind_pdu_prepare(ll,advParam,advMode,flags,&auxInfo,advParam->ea->did);
        }
        break;
        case ADV_PDU_CLASS_AUX:
        {
            flags = ADV_EXTENDED_HEADER_FLAG_ADV_A|ADV_EXTENDED_HEADER_FLAG_ADI;
            advMode = 1;
            if(advParam->ea->power!=0x7f)
            {   
                flags|=ADV_EXTENDED_HEADER_FLAG_TX_POWER;
            }
            adv_aux_adv_ind_pdu_prepare(ll,advParam,advMode,flags,&auxInfo,advParam->ea->did);
        }
        break;
        case ADV_PDU_CLASS_CONN_RSP:
        {
            flags = ADV_EXTENDED_HEADER_FLAG_ADV_A|ADV_EXTENDED_HEADER_FLAG_TARGET_A;
            adv_aux_conn_rsp_pdu_prepare(ll,advParam,advMode,flags,&auxInfo,advParam->ea->did);
        }
        break;
    }
}
//ADV_EVENT_EXTENDED_SCANNABLE_DIRECTED
_RAM_CODE
static void adv_event_extended_scannable_directed_packet_prapare(ll_sm_t* ll,ll_internal_adv_param_t* advParam,adv_pdu_class_e pduClass)
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
            auxInfo.anchorPoint       = advParam->la->sch.anchorPoint;
            auxInfo.targetAnchorPoint = advParam->ea->aux.sch.anchorPoint;
            auxInfo.channel           = advParam->ea->aux.phy.chn;
            auxInfo.phy               = advParam->ea->aux.phy.mode;
            adv_ext_ind_pdu_prepare(ll,advParam,advMode,flags,&auxInfo,advParam->ea->did);
        }
        break;
        case ADV_PDU_CLASS_AUX:
        {
            flags = ADV_EXTENDED_HEADER_FLAG_ADV_A|ADV_EXTENDED_HEADER_FLAG_TARGET_A|ADV_EXTENDED_HEADER_FLAG_ADI;
            if(advParam->ea->power!=0x7f)
            {   
                flags|=ADV_EXTENDED_HEADER_FLAG_TX_POWER;
            }
            advMode = 2;
            adv_aux_adv_ind_pdu_prepare(ll,advParam,advMode,flags,&auxInfo,advParam->ea->did);
        }
        break;
        case ADV_PDU_CLASS_SCAN_RSP:
        {
            flags = ADV_EXTENDED_HEADER_FLAG_ADV_A|ADV_EXTENDED_HEADER_FLAG_ADI;
            if(advParam->ea->power!=0x7f)
            {   
                flags|=ADV_EXTENDED_HEADER_FLAG_TX_POWER;
            }
            if(advParam->pChain->cnt!=0)
            {
                flags|= ADV_EXTENDED_HEADER_FLAG_AUX_PTR;
                auxInfo.anchorPoint       = advParam->ea->aux.sch.anchorPoint;
                auxInfo.targetAnchorPoint = advParam->pChain->entry[0].sch.anchorPoint;
                auxInfo.channel           = advParam->pChain->entry[0].phy.chn;
                auxInfo.phy               = advParam->pChain->entry[0].phy.mode;
            }
            adv_aux_scan_rsp_pdu_prepare(ll,advParam,advMode,flags,&auxInfo,advParam->ea->did);
        }
        break;
        case ADV_PDU_CLASS_CHAIN:
        {
            flags = ADV_EXTENDED_HEADER_FLAG_ADI;
            if(advParam->pChain->cnt>(advParam->pChain->current+1))
            {
                flags|=ADV_EXTENDED_HEADER_FLAG_AUX_PTR;
                auxInfo.anchorPoint       = advParam->pChain->entry[advParam->pChain->current].sch.anchorPoint;
                auxInfo.targetAnchorPoint = advParam->pChain->entry[advParam->pChain->current+1].sch.anchorPoint;
                auxInfo.channel           = advParam->pChain->entry[advParam->pChain->current+1].phy.chn;
                auxInfo.phy               = advParam->pChain->entry[advParam->pChain->current+1].phy.mode;
            }
            adv_aux_chain_ind_pdu_prepare(ll,advParam,advMode,flags,&auxInfo,advParam->ea->did);
        }
        break;
    }
}
//ADV_EVENT_EXTENDED_SCANNABLE_UNDIRECTED
_RAM_CODE
static void adv_event_extended_scannable_undirected_packet_prapare(ll_sm_t* ll,ll_internal_adv_param_t* advParam,adv_pdu_class_e pduClass)
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
            auxInfo.anchorPoint       = advParam->la->sch.anchorPoint;
            auxInfo.targetAnchorPoint = advParam->ea->aux.sch.anchorPoint;
            auxInfo.channel           = advParam->ea->aux.phy.chn;
            auxInfo.phy               = advParam->ea->aux.phy.mode;
            adv_ext_ind_pdu_prepare(ll,advParam,advMode,flags,&auxInfo,advParam->ea->did);
        }
        break;
        case ADV_PDU_CLASS_AUX:
        {
            flags = ADV_EXTENDED_HEADER_FLAG_ADV_A|ADV_EXTENDED_HEADER_FLAG_ADI;
            advMode = 2;
            if(advParam->ea->power!=0x7f)
            {   
                flags|=ADV_EXTENDED_HEADER_FLAG_TX_POWER;
            }
            adv_aux_adv_ind_pdu_prepare(ll,advParam,advMode,flags,&auxInfo,advParam->ea->did);
        }
        break;
        case ADV_PDU_CLASS_SCAN_RSP:
        {
            flags = ADV_EXTENDED_HEADER_FLAG_ADV_A|ADV_EXTENDED_HEADER_FLAG_ADI;
            if(advParam->ea->power!=0x7f)
            {   
                flags|=ADV_EXTENDED_HEADER_FLAG_TX_POWER;
            }
            if(advParam->pChain->cnt!=0)
            {
                flags|= ADV_EXTENDED_HEADER_FLAG_AUX_PTR;
                auxInfo.anchorPoint       = advParam->ea->aux.sch.anchorPoint;
                auxInfo.targetAnchorPoint = advParam->pChain->entry[0].sch.anchorPoint;
                auxInfo.channel           = advParam->pChain->entry[0].phy.chn;
                auxInfo.phy               = advParam->pChain->entry[0].phy.mode;
            }
            adv_aux_scan_rsp_pdu_prepare(ll,advParam,advMode,flags,&auxInfo,advParam->ea->did);
        }
        break;
        case ADV_PDU_CLASS_CHAIN:
        {
            flags = ADV_EXTENDED_HEADER_FLAG_ADI;
            if(advParam->pChain->cnt>(advParam->pChain->current+1))
            {
                flags|=ADV_EXTENDED_HEADER_FLAG_AUX_PTR;
                auxInfo.anchorPoint       = advParam->pChain->entry[advParam->pChain->current].sch.anchorPoint;
                auxInfo.targetAnchorPoint = advParam->pChain->entry[advParam->pChain->current+1].sch.anchorPoint;
                auxInfo.channel           = advParam->pChain->entry[advParam->pChain->current+1].phy.chn;
                auxInfo.phy               = advParam->pChain->entry[advParam->pChain->current+1].phy.mode;
            }
            adv_aux_chain_ind_pdu_prepare(ll,advParam,advMode,flags,&auxInfo,advParam->ea->did);
        }
        break;
    }
}
//ADV_EVENT_EXTENDED_NON_CONNECTABLE_NON_SCANNABLE_DIRECTED_WITH_AUXILIARY
_RAM_CODE
static void adv_event_extended_non_connectable_non_scannable_directed_with_auxiliary_packet_prapare(ll_sm_t* ll,ll_internal_adv_param_t* advParam,adv_pdu_class_e pduClass)
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
            auxInfo.anchorPoint       = advParam->la->sch.anchorPoint;
            auxInfo.targetAnchorPoint = advParam->ea->aux.sch.anchorPoint;
            auxInfo.channel           = advParam->ea->aux.phy.chn;
            auxInfo.phy               = advParam->ea->aux.phy.mode;
            adv_ext_ind_pdu_prepare(ll,advParam,advMode,flags,&auxInfo,advParam->ea->did);
        }
        break;
        case ADV_PDU_CLASS_AUX:
        {
            flags = ADV_EXTENDED_HEADER_FLAG_ADV_A|ADV_EXTENDED_HEADER_FLAG_TARGET_A|ADV_EXTENDED_HEADER_FLAG_ADI;
            if(advParam->ea->power!=0x7f)
            {   
                flags|=ADV_EXTENDED_HEADER_FLAG_TX_POWER;
            }
            if(advParam->pChain->cnt!=0)
            {
                flags|=ADV_EXTENDED_HEADER_FLAG_AUX_PTR;
                auxInfo.anchorPoint       = advParam->ea->aux.sch.anchorPoint;
                auxInfo.targetAnchorPoint = advParam->pChain->entry[0].sch.anchorPoint;
                auxInfo.channel           = advParam->pChain->entry[0].phy.chn;
                auxInfo.phy               = advParam->pChain->entry[0].phy.mode;
            }
            //sync info optional
            adv_aux_adv_ind_pdu_prepare(ll,advParam,advMode,flags,&auxInfo,advParam->ea->did);
        }
        break;
        case ADV_PDU_CLASS_CHAIN:
        {
            flags = ADV_EXTENDED_HEADER_FLAG_ADI;
            if(advParam->ea->power!=0x7f)
            {   
                flags|=ADV_EXTENDED_HEADER_FLAG_TX_POWER;
            }
            if(advParam->pChain->cnt>(advParam->pChain->current+1))
            {
                flags|=ADV_EXTENDED_HEADER_FLAG_AUX_PTR;
                auxInfo.anchorPoint       = advParam->pChain->entry[advParam->pChain->current].sch.anchorPoint;
                auxInfo.targetAnchorPoint = advParam->pChain->entry[advParam->pChain->current+1].sch.anchorPoint;
                auxInfo.channel           = advParam->pChain->entry[advParam->pChain->current+1].phy.chn;
                auxInfo.phy               = advParam->pChain->entry[advParam->pChain->current+1].phy.mode;
            }
            adv_aux_chain_ind_pdu_prepare(ll,advParam,advMode,flags,&auxInfo,advParam->ea->did);
        }
        break;
    }
}
//ADV_EVENT_EXTENDED_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED_WITH_AUXILIARY
_RAM_CODE
static void adv_event_extended_non_connectable_non_scannable_undirected_with_auxiliary_packet_prapare(ll_sm_t* ll,ll_internal_adv_param_t* advParam,adv_pdu_class_e pduClass)
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
            auxInfo.anchorPoint       = advParam->la->sch.anchorPoint;
            auxInfo.targetAnchorPoint = advParam->ea->aux.sch.anchorPoint;
            auxInfo.channel           = advParam->ea->aux.phy.chn;
            auxInfo.phy               = advParam->ea->aux.phy.mode;
            adv_ext_ind_pdu_prepare(ll,advParam,advMode,flags,&auxInfo,advParam->ea->did);
        }
        break;
        case ADV_PDU_CLASS_AUX:
        {
            flags = ADV_EXTENDED_HEADER_FLAG_ADV_A|ADV_EXTENDED_HEADER_FLAG_ADI;
            if(advParam->ea->power!=0x7f)
            {   
                flags|=ADV_EXTENDED_HEADER_FLAG_TX_POWER;
            }
            if(advParam->pChain->cnt!=0)
            {
                flags|=ADV_EXTENDED_HEADER_FLAG_AUX_PTR;
                auxInfo.anchorPoint       = advParam->ea->aux.sch.anchorPoint;
                auxInfo.targetAnchorPoint = advParam->pChain->entry[0].sch.anchorPoint;
                auxInfo.channel           = advParam->pChain->entry[0].phy.chn;
                auxInfo.phy               = advParam->pChain->entry[0].phy.mode;
            }   
            //sync info optional
            adv_aux_adv_ind_pdu_prepare(ll,advParam,advMode,flags,&auxInfo,advParam->ea->did);
        }
        break;
        case ADV_PDU_CLASS_CHAIN:
        {
            flags = ADV_EXTENDED_HEADER_FLAG_ADI;
            if(advParam->ea->power!=0x7f)
            {   
                flags|=ADV_EXTENDED_HEADER_FLAG_TX_POWER;
            }
            if(advParam->pChain->cnt>(advParam->pChain->current+1))
            {
                flags|=ADV_EXTENDED_HEADER_FLAG_AUX_PTR;
                auxInfo.anchorPoint       = advParam->pChain->entry[advParam->pChain->current].sch.anchorPoint;
                auxInfo.targetAnchorPoint = advParam->pChain->entry[advParam->pChain->current+1].sch.anchorPoint;
                auxInfo.channel           = advParam->pChain->entry[advParam->pChain->current+1].phy.chn;
                auxInfo.phy               = advParam->pChain->entry[advParam->pChain->current+1].phy.mode;
            }
            adv_aux_chain_ind_pdu_prepare(ll,advParam,advMode,flags,&auxInfo,advParam->ea->did);
        }
        break;
    }
}
//ADV_EVENT_EXTENDED_NON_CONNECTABLE_NON_SCANNABLE_DIRECTED_WITHOUT_AUXILIARY
_RAM_CODE
static void adv_event_extended_non_connectable_non_scannable_directed_without_auxiliary_packet_prapare(ll_sm_t* ll,ll_internal_adv_param_t* advParam,adv_pdu_class_e pduClass)
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
            if(advParam->ea->power!=0x7f)
            {   
                flags|=ADV_EXTENDED_HEADER_FLAG_TX_POWER;
            }
            adv_ext_ind_pdu_prepare(ll,advParam,advMode,flags,&auxInfo,advParam->ea->did);
        }
    }
}
//ADV_EVENT_EXTENDED_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED_WITHOUT_AUXILIARY
_RAM_CODE
static void adv_event_extended_non_connectable_non_scannable_undirected_without_auxiliary_packet_prapare(ll_sm_t* ll,ll_internal_adv_param_t* advParam,adv_pdu_class_e pduClass)
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
            if(advParam->ea->power!=0x7f)
            {   
                flags|=ADV_EXTENDED_HEADER_FLAG_TX_POWER;
            }
            adv_ext_ind_pdu_prepare(ll,advParam,advMode,flags,&auxInfo,advParam->ea->did);
        }
    }
}
#endif
#if(LL_SUPPORT_LE_PERIODIC_ADVERTISING)
//ADV_EVENT_EXTENDED_PERIODIC
_RAM_CODE
static void adv_event_extended_periodic_packet_prapare(ll_sm_t* ll,ll_internal_adv_param_t* advParam,adv_pdu_class_e pduClass)
{
    _u8 flags = 0;
    _u8 advMode = 0;
    _u8 extHeaderLen = 0;
    adv_extended_header_auxInfo_t auxInfo = {0};
    switch(pduClass)
    {
        case ADV_PDU_CLASS_SYNC:
        {
        	if(advParam->pa->includeAdi)
        	{
                flags |= ADV_EXTENDED_HEADER_FLAG_ADI;
        	}
        	if(advParam->pa->includeTxPower)
        	{
                flags |= ADV_EXTENDED_HEADER_FLAG_TX_POWER;
        	}
        	if(advParam->schMap&ADV_SCH_MAP_PA_CHAIN)
        	{
        		flags |= ADV_EXTENDED_HEADER_FLAG_AUX_PTR;
        		auxInfo.anchorPoint       = advParam->pa->sync.sch.anchorPoint;
        		auxInfo.targetAnchorPoint = advParam->pa->chain.entry[0].sch.anchorPoint;
          		auxInfo.phy               = advParam->pa->chain.entry[0].phy.mode;
          		auxInfo.channel           = advParam->pa->chain.entry[0].phy.chn;
        	}
        	adv_aux_sync_ind_pdu_prepare(ll,advParam,advMode,flags,&auxInfo,advParam->pa->did);
        }
    }
}
#endif

#if(LL_SUPPORT_PERIODIC_ADVERTISING_WITH_RESPONSES_ADVERTISER)
//ADV_EVENT_EXTENDED_PERIODIC_WITH_RESPONSE
_RAM_CODE
static void adv_event_extended_periodic_with_response_packet_prapare(ll_sm_t* ll,ll_internal_adv_param_t* advParam,adv_pdu_class_e pduClass)
{
    
}
#endif

void(*adv_prepare_packet[])(ll_sm_t* ll,ll_internal_adv_param_t* advParam,adv_pdu_class_e pduClass)  =
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


/*****************************************ADV Event Process ***********************************************/
_RAM_CODE
static int adv_event_step_phy_send_advertising(ll_sm_t* ll,ll_internal_adv_param_t* advParam)
{
    if(advParam->la->availableChnCnt==0)
    {
        return 0;
    }
    phy_obj_cast(&ll->phy);
    advParam->la->eventCnt++;
    advParam->la->phy.chn = advParam->la->chnTable[(advParam->la->eventCnt%advParam->la->channelCnt)];
    #if(LL_SUPPORT_LE_EXTENDED_ADVERTISING)
    if(POINTER_VALID(advParam->ea))
    {
        if(advParam->eventProperty&LL_ADV_EVENT_PROPERTY_LEGACY_PDU)
        {
            adv_prepare_packet[advParam->eventType](ll,advParam,ADV_PDU_CLASS_LEG);
        } 
        else
        {
            adv_prepare_packet[advParam->eventType](ll,advParam,ADV_PDU_CLASS_EXT);
        }
    }
    else
    {
        adv_prepare_packet[advParam->eventType](ll,advParam,ADV_PDU_CLASS_LEG); 
    }
    #else
    adv_prepare_packet[advParam->eventType](ll,advParam,ADV_PDU_CLASS_LEG);
    #endif
    adv_prepare_phy(ll,&advParam->la->phy,advParam->la->sch.anchorPoint,PHY_DIR_TX);
    ll->phy.start();
    return 1;
}

_RAM_CODE
static int adv_event_step_phy_start_listen(ll_sm_t* ll,ll_internal_adv_param_t* advParam)
{
    phy_obj_cast(&ll->phy);
//    	_u32 timestamp = advParam->la->sch.anchorPoint + ll_get_air_packet_time(advParam->la->phy.mode,6+advParam->data.len,0)+PACKET_DEFAULT_TIFS_TIME;
	adv_prepare_phy(ll,&advParam->la->phy,0,PHY_DIR_RX);
	ll->phy.start();
	return 1;
}


_RAM_CODE
static int adv_event_step_phy_send_scan_rsp(ll_sm_t* ll,ll_internal_adv_param_t* advParam)
{
	if(ll->phy.hw_is_rx_packet_valid())//packet analyze shall be execute first
	{
		ll_adv_packet_t* packet = (ll_adv_packet_t*)(ll->phy.rxAddress + ll_get_packet_header_offset_from_address(PHY_DIR_RX));
		if(packet->hdr.pduType == LL_ADV_TYPE_SCAN_REQ && packet->hdr.length == sizeof(scan_type_scan_req_t))
		{
			 scan_type_scan_req_t* scanReq = (scan_type_scan_req_t*)(packet->data);
			 if((txMemcmp(ll_get_device_address(),scanReq->advA,6) == 0))
			 {
				adv_prepare_packet[advParam->eventType](ll,advParam,ADV_PDU_CLASS_SCAN_RSP);
				_u32 timestamp = ll->phy.hw_get_rx_air_ts() + ll_get_air_packet_time(advParam->la->phy.mode,sizeof(scan_type_scan_req_t),0)+PACKET_DEFAULT_TIFS_TIME;
				adv_prepare_phy(ll,&advParam->la->phy,timestamp,PHY_DIR_TX);
				ll->phy.start();
				return 1;
			 }
		}
		else if(packet->hdr.pduType == LL_ADV_TYPE_CONNECT_IND && packet->hdr.length == sizeof(init_type_connectInd_t))
		{
			//connect ind process
			init_type_connectInd_t* connInd = (init_type_connectInd_t*)(packet->data);
            if(POINTER_NOT_VALID(ll->conn))
            {
                ll->conn = (ll_internal_connection_ctrl_t*)tx_malloc(sizeof(ll_internal_connection_ctrl_t));
            }
			//ll state machine transform
            if(ble_ll_process_event(ll,BLE_LL_EVENT_START_CONNECTION) == BLE_LL_STATE_SUCCESS)
            {
                init_type_ll_data_t* llData = (init_type_ll_data_t*)(connInd->llData);
                ll->conn->peer.sca = llData->sca;
                ll->conn->timeout  = llData->timeout;
                ll->conn->latency  = llData->latency;
                ll->conn->csa.hop  = llData->hop;
                if(packet->hdr.chSel && LL_CHANNEL_SUPP_CSA2)
                {
                	ll->conn->csa.mode = LL_CSA_2;
                }
                else
                {
                	ll->conn->csa.mode = LL_CSA_1;
                }
                ll->conn->csa.counter = 0;
                txMemcpy(ll->conn->csa.map,llData->chnMap,5);
                ll_csa_init(&ll->conn->csa);
                //transmit window delay is 1.25ms when connect ind used
                _u32 connIndAnchor = ll->phy.hw_get_rx_air_ts() + ll_get_air_packet_time(advParam->la->phy.mode,sizeof(init_type_connectInd_t),0);
                ll->conn->anchor   = connIndAnchor +1250+llData->winOffset*1250;
                ll->conn->duration = llData->winSize*1250;
                ll->conn->interval = llData->interval*1250;

                ll->sch.timestamp  = ll->conn->anchor;
                ll->sch.duration   = ll->sch.durationMin = ll->conn->duration;
                ll->sch.period     = ll->conn->interval;
                ll->sch.startLatency = 100;
                ll->sch.stopLatency  = 100;
                ll->phy.mode       = PHY_MODE_1M;
                ll->phy.accessCode = llData->AA;
                ll->phy.crcInit    = llData->crcInit;
                sch_abort_current_and_process_next_task();
            }
            else 
            {
                tx_free((_u8*)ll->conn);
            }
		}
	}
	return 0;
}

_RAM_CODE
static int adv_event_step_default_process(ll_sm_t* ll,ll_internal_adv_param_t* advParam)
{
	ll->phy.stop();
	return 1;
}

_RAM_CODE
static int adv_event_step_sch_stop(ll_sm_t* ll,ll_internal_adv_param_t* advParam)
{
	ll->phy.stop();
    if(advParam->la->availableChnCnt)
    {
    	advParam->la->availableChnCnt--;
    }
    if(advParam->la->availableChnCnt)
    {
        advParam->la->sch.anchorPoint += (advParam->la->sch.duration+advParam->la->sch.stopMargin + advParam->la->sch.startMargin);

    }
    else
    {
        advParam->la->availableChnCnt = advParam->la->channelCnt;
        advParam->la->sch.anchorPoint += (advParam->la->sch.interval + 30*(random_byte()|0x0f));
        #if(LL_SUPPORT_LE_EXTENDED_ADVERTISING)
        if((!(advParam->schMap&ADV_SCH_MAP_AUX))\
          ||((advParam->schMap&ADV_SCH_MAP_AUX)&&txCompareTime(advParam->ea->anchor,advParam->la->sch.anchorPoint+advParam->la->sch.interval)))
        #endif
        {
            ll_extended_adv_map_out_task(ll,advParam,advParam->la->sch.anchorPoint,advParam->la->sch.anchorPoint+advParam->la->sch.interval,ADV_SCH_MAP_PRI);
        }
    }
    return 1;
}

_RAM_CODE
static int adv_event_step_sch_passed(ll_sm_t* ll,ll_internal_adv_param_t* advParam)
{
     _u32 systemTime = system_time();
     _u32 periodicDiff = (systemTime - advParam->la->sch.anchorPoint)/advParam->la->sch.interval;
     advParam->la->sch.anchorPoint += (periodicDiff+1)*advParam->la->sch.interval;
     advParam->la->availableChnCnt = advParam->la->channelCnt;
     #if(LL_SUPPORT_LE_EXTENDED_ADVERTISING)
     if((!(advParam->schMap&ADV_SCH_MAP_AUX))||((advParam->schMap&ADV_SCH_MAP_AUX)&&txCompareTime(advParam->ea->anchor,advParam->la->sch.anchorPoint+advParam->la->sch.interval)))
     #endif
     {
         ll_extended_adv_map_out_task(ll,advParam,advParam->la->sch.anchorPoint,advParam->la->sch.anchorPoint+advParam->la->sch.interval,ADV_SCH_MAP_PRI);
     }
     return 1;
}



_RAM_CODE
static int adv_event_step_sch_canceled(ll_sm_t* ll,ll_internal_adv_param_t* advParam)
{
    _u32 systemTime = system_time();
    _u32 periodicDiff = (systemTime - advParam->la->sch.anchorPoint)/advParam->la->sch.interval;
    advParam->la->sch.anchorPoint += (periodicDiff+1)*advParam->la->sch.interval;
    advParam->la->availableChnCnt = advParam->la->channelCnt;

    #if(LL_SUPPORT_LE_EXTENDED_ADVERTISING)
    if((!(advParam->schMap&ADV_SCH_MAP_AUX))||((advParam->schMap&ADV_SCH_MAP_AUX)&&txCompareTime(advParam->ea->anchor,advParam->la->sch.anchorPoint+advParam->la->sch.interval)))
    #endif
    {
        ll_extended_adv_map_out_task(ll,advParam,advParam->la->sch.anchorPoint,advParam->la->sch.anchorPoint+advParam->la->sch.interval,ADV_SCH_MAP_PRI);
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

/*****************************************ADV Extended Event Process ***********************************************/
#if(LL_SUPPORT_LE_EXTENDED_ADVERTISING)

ll_internal_adv_param_t* ll_extended_adv_get_entity(_u8 handle,_u8 allocate)
{
    ll_sm_t* ll     = ll_get_current_state_machine();
	for(int i=0;i<BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS;i++)
	{
		if(POINTER_VALID(ll->adv->param[i].la)&&ll->adv->param[i].handle == handle)
		{
			return &ll->adv->param[i];
		}
	}
	if(allocate==0)
	{
		return NULL;
	}

	for(int i=0;i<BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS;i++)
	{
        if(ll->adv->param[i].handle == LL_EXTENDED_ADV_INVALID_HANDLE)
        {
        	ll->adv->param[i].handle = handle;
            if(POINTER_NOT_VALID(ll->adv->param[i].la))
            {
                ll->adv->param[i].la = (ll_adv_set_t*)tx_malloc(sizeof(ll_adv_set_t));
            }
            return &ll->adv->param[i];
        }
    }
    return NULL;
}

int ll_extended_adv_get_current_active_set_number(void)
{
    ll_sm_t* ll = ll_get_current_state_machine();
    _u8 count = 0;
	for(int i=0;i<BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS;i++)
	{
        if(ll->adv->param[i].handle!=LL_EXTENDED_ADV_INVALID_HANDLE && ll->adv->param[i].enable == 1)
        {
            count++;
        }
    }
    return count;
}

int ll_extended_adv_get_current_set_number(void)
{
    ll_sm_t* ll = ll_get_current_state_machine();
    _u8 count = 0;
	for(int i=0;i<BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS;i++)
	{
        if(ll->adv->param[i].handle!=LL_EXTENDED_ADV_INVALID_HANDLE)
        {
            count++;
        }
    }
    return count;
}

static int adv_extended_event_step_phy_send_aux_advertising(ll_sm_t* ll,ll_internal_adv_param_t* advParam)
{
    phy_obj_cast(&ll->phy);
    //prepare packet
    adv_prepare_packet[advParam->eventType](ll,advParam,ADV_PDU_CLASS_AUX);
    advParam->ea->aux.phy.chn = 35;//todo
    //prepare phy
    adv_prepare_phy(ll,&advParam->ea->aux.phy,advParam->ea->aux.sch.anchorPoint,PHY_DIR_TX);
    ll->phy.start();
	return 1;
}
static int adv_extended_event_step_phy_start_listen_aux(ll_sm_t* ll,ll_internal_adv_param_t* advParam)
{
    phy_obj_cast(&ll->phy);
    // _u32 timestamp = advParam->ea->sch.anchor + ll_get_air_packet_time(advParam->la->phy.mode,6+advParam->data.len,0)+PACKET_DEFAULT_TIFS_TIME;
	adv_prepare_phy(ll,&advParam->ea->aux.phy,0,PHY_DIR_RX);
	ll->phy.start();
	return 1;
}
static int adv_extended_event_step_phy_send_aux_scan_rsp(ll_sm_t* ll,ll_internal_adv_param_t* advParam)
{
	if(ll->phy.hw_is_rx_packet_valid())
	{
		ll_adv_packet_t* packet = (ll_adv_packet_t*)(advParam->ea->aux.phy.rxAddress + ll_get_packet_header_offset_from_address(PHY_DIR_RX));
		if((packet->hdr.pduType == LL_ADV_TYPE_AUX_SCAN_REQ)&&(packet->hdr.length == sizeof(scan_type_aux_scan_req_t)))
		{
			//scan req process
			scan_type_aux_scan_req_t* scanReq = (scan_type_aux_scan_req_t*)(packet->data);
			if((advParam->scanRsp.len!=0)&&(txMemcmp(ll_get_device_address(),scanReq->advA,6) == 0))
			{
				adv_prepare_packet[advParam->eventType](ll,advParam,ADV_PDU_CLASS_SCAN_RSP);
				_u32 timestamp = ll->phy.hw_get_rx_air_ts() + ll_get_air_packet_time(advParam->ea->aux.phy.mode,sizeof(scan_type_aux_scan_req_t),0)+PACKET_DEFAULT_TIFS_TIME;
				adv_prepare_phy(ll,&advParam->ea->aux.phy,timestamp,PHY_DIR_TX);
				ll->phy.start();
				return 1;
	        }
		}
        else if((packet->hdr.pduType == LL_ADV_TYPE_CONNECT_IND)&&(packet->hdr.length == sizeof(init_type_connectInd_t)))
        {

        }
	}

	return 0;
}
static int adv_extended_event_step_phy_send_aux_connect_rsp(ll_sm_t* ll,ll_internal_adv_param_t* advParam)
{
	if(ll->phy.hw_is_rx_packet_valid())
	{
		ll_adv_packet_t* packet = (ll_adv_packet_t*)(advParam->ea->aux.phy.rxAddress + ll_get_packet_header_offset_from_address(PHY_DIR_RX));
		if((packet->hdr.pduType == LL_ADV_TYPE_AUX_CONNECT_REQ)&&(packet->hdr.length == sizeof(init_type_auxConnectReq_t)))
		{
			//scan req process
			init_type_auxConnectReq_t* scanReq = (init_type_auxConnectReq_t*)(packet->data);
//			if(txMemcmp(ll_get_device_address(),scanReq->advA,6) == 0)
//			{
				adv_prepare_packet[advParam->eventType](ll,advParam,ADV_PDU_CLASS_CONN_RSP);
				_u32 timestamp = ll->phy.hw_get_rx_air_ts() + ll_get_air_packet_time(advParam->ea->aux.phy.mode,sizeof(init_type_auxConnectReq_t),0)+PACKET_DEFAULT_TIFS_TIME;
				adv_prepare_phy(ll,&advParam->ea->aux.phy,timestamp,PHY_DIR_TX);
				ll->phy.start();
				return 1;
//	        }
		}
	}
	return 0;
}
static int adv_extended_event_step_default_process(ll_sm_t* ll,ll_internal_adv_param_t* advParam)
{
	return 1;
}
static int adv_extended_event_step_sch_stop(ll_sm_t* ll,ll_internal_adv_param_t* advParam)
{
    if(advParam->state == ADV_SM_STATE_SENDING_AUX_SCAN_RSP)
    {
        if(advParam->schMap&ADV_SCH_MAP_AUX_CHAIN)
        {
            return 1; 
        }
    }
    else if(advParam->schMap&ADV_SCH_MAP_AUX_CHAIN)
    {
        advParam->ea->anchor = advParam->ea->chain.entry[0].sch.anchorPoint;
        return 1;
    }

    ll_extended_adv_map_out_task(ll,advParam,advParam->la->sch.anchorPoint,advParam->la->sch.anchorPoint+advParam->la->sch.interval,ADV_SCH_MAP_PRI|ADV_SCH_MAP_AUX);
	return 1;
}
static int adv_extended_event_step_sch_canceled(ll_sm_t* ll,ll_internal_adv_param_t* advParam)
{
    ll_extended_adv_map_out_task(ll,advParam,advParam->la->sch.anchorPoint,advParam->la->sch.anchorPoint+advParam->la->sch.interval,ADV_SCH_MAP_ALL);
    //todo
	return 1;
}
static int adv_extended_event_step_sch_passed(ll_sm_t* ll,ll_internal_adv_param_t* advParam)
{
    ll_extended_adv_map_out_task(ll,advParam,advParam->la->sch.anchorPoint,advParam->la->sch.anchorPoint+advParam->la->sch.interval,ADV_SCH_MAP_ALL);
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


static int adv_chained_event_step_phy_send_chain_advertising(ll_sm_t* ll,ll_internal_adv_param_t* advParam)
{
    phy_obj_cast(&ll->phy);
    //prepare packet
    adv_prepare_packet[advParam->eventType](ll,advParam,ADV_PDU_CLASS_CHAIN);
    advParam->pChain->entry[advParam->pChain->current].phy.chn = 35;//todo
    //prepare phy
    adv_prepare_phy(ll,&advParam->pChain->entry[advParam->pChain->current].phy,advParam->pChain->entry[advParam->pChain->current].sch.anchorPoint,PHY_DIR_TX);
    ll->phy.start();
	return 1;
}

static int adv_chained_event_step_sch_stop(ll_sm_t* ll,ll_internal_adv_param_t* advParam)
{
    ll->phy.stop();
    advParam->pChain->current++;
    if(advParam->pChain->current==advParam->pChain->cnt)
    {
    	advParam->pChain->current = 0;
        ll_extended_adv_map_out_task(ll,advParam,advParam->la->sch.anchorPoint,advParam->la->sch.anchorPoint+advParam->la->sch.interval,ADV_SCH_MAP_PRI|ADV_SCH_MAP_AUX|ADV_SCH_MAP_AUX_CHAIN);
    }
    else
    {
        advParam->ea->anchor = advParam->ea->chain.entry[advParam->pChain->current].sch.anchorPoint;
    }
}
static int adv_chained_event_step_default_process(ll_sm_t* ll,ll_internal_adv_param_t* advParam)
{
	return 1;
}
static int adv_chained_event_step_sch_canceled(ll_sm_t* ll,ll_internal_adv_param_t* advParam)
{
    ll_extended_adv_map_out_task(ll,advParam,advParam->la->sch.anchorPoint,advParam->la->sch.anchorPoint+advParam->la->sch.interval,ADV_SCH_MAP_PRI|ADV_SCH_MAP_AUX|ADV_SCH_MAP_AUX_CHAIN);
}

static int adv_chained_event_step_sch_passed(ll_sm_t* ll,ll_internal_adv_param_t* advParam)
{
    ll_extended_adv_map_out_task(ll,advParam,advParam->la->sch.anchorPoint,advParam->la->sch.anchorPoint+advParam->la->sch.interval,ADV_SCH_MAP_PRI|ADV_SCH_MAP_AUX|ADV_SCH_MAP_AUX_CHAIN);
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
/*****************************************ADV Periodic Event Process ***********************************************/
#if(LL_SUPPORT_LE_PERIODIC_ADVERTISING)
static int adv_periodic_event_step_phy_send_sync_advertising(ll_sm_t* ll,ll_internal_adv_param_t* advParam)
{
    phy_obj_cast(&ll->phy);
    //prepare packet
    adv_prepare_packet[ADV_EVENT_EXTENDED_PERIODIC](ll,advParam,ADV_PDU_CLASS_SYNC);
    advParam->pa->sync.phy.chn = 35;//todo
    //prepare phy
    adv_prepare_phy(ll,&advParam->pa->sync.phy,advParam->pa->sync.sch.anchorPoint,PHY_DIR_TX);
    ll->phy.start();
    advParam->pa->eventCnt++;
	return 1;
}
static int adv_periodic_event_step_sch_stop(ll_sm_t* ll,ll_internal_adv_param_t* advParam)
{
    ll->phy.stop();
    advParam->pa->sync.sch.anchorPoint+=advParam->pa->sync.sch.interval;
}
static int adv_periodic_event_step_sch_passed(ll_sm_t* ll,ll_internal_adv_param_t* advParam)
{
    advParam->pa->eventCnt++;
    advParam->pa->sync.sch.anchorPoint+=advParam->pa->sync.sch.interval;
}
static int adv_periodic_event_step_sch_canceled(ll_sm_t* ll,ll_internal_adv_param_t* advParam)
{
    advParam->pa->eventCnt++;
    advParam->pa->sync.sch.anchorPoint+=advParam->pa->sync.sch.interval;
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
/*****************************************ADV Periodic With Rsp Process ***********************************************/
#if(LL_SUPPORT_PERIODIC_ADVERTISING_WITH_RESPONSES_ADVERTISER)
typedef enum
{
    ADV_PERIODIC_WITH_RSP_EVENT_NONE,
}adv_periodic_with_rsp_event_property_e;
static adv_event_sm_t adv_periodic_with_rsp_event_state_machine[]= 
{

};
static adv_procedure_list_t adv_extended_periodic_with_rsp_procedure[]=
{
    {ADV_EVENT,                  adv_event_state_machine,                  ADV_SM_LIST_LENGTH(adv_event_state_machine),                  ADV_EVENT_NONE},
    {ADV_EXTENDED_EVENT,         adv_extended_event_state_machine,         ADV_SM_LIST_LENGTH(adv_extended_event_state_machine),         ADV_EXTENDED_EVENT_NONE},
    {ADV_PERIODIC_WITH_RSP_EVENT,adv_periodic_with_rsp_event_state_machine,ADV_SM_LIST_LENGTH(adv_periodic_with_rsp_event_state_machine),ADV_PERIODIC_WITH_RSP_EVENT_NONE},
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
static void adv_sequence_process(sm_event_type_e type,_u8 event)
{
    _u8 smEventType = 0; 
    if(type == LL_SCH_EVENT)
    {
        smEventType = LL_SCH_EVENT_BASE+event;
    }
    else
    {
    	smEventType = LL_PHY_EVENT_BASE+event;
    }
    ll_sm_t* ll     = ll_get_current_state_machine();
    _u8 eventType     = currentAdvSet->eventType;
    _u8 eventClass    = currentEventClass;
    if(advSequence[eventType].listLen>eventClass)
    {
        for(_u8 i=0;i<advSequence[eventType].procedureList[eventClass].listLen;i++)
        {
            if(currentAdvSet->state  == advSequence[eventType].procedureList[eventClass].sm[i].currentState\
                && smEventType  == advSequence[eventType].procedureList[eventClass].sm[i].event\
				&& (advSequence[eventType].procedureList[eventClass].context & advSequence[eventType].procedureList[eventClass].sm[i].context))
            {
                if(currentAdvSet->processingEvent == smEventType)
                {
                    return;//reload same event,return
                }
                if(advSequence[eventType].procedureList[eventClass].sm[i].cb != NULL)
                {
                	currentAdvSet->processingEvent = smEventType;//use member value to avoid many entity conflict
                    int ret = advSequence[eventType].procedureList[eventClass].sm[i].cb(ll,currentAdvSet);
                    if(ret)
                    {
                    	currentAdvSet->state = advSequence[eventType].procedureList[eventClass].sm[i].transSuccessState;
                    }
                    else
                    {
                    	currentAdvSet->state = advSequence[eventType].procedureList[eventClass].sm[i].transFailState;
                    }
                    currentAdvSet->processingEvent = 0;
                }
                if(type         == LL_SCH_EVENT \
                   && ll->state == BLE_LL_STATE_ADVERTISING \
                   && currentAdvSet->state == ADV_SM_STATE_IDLE)
                {
                    adv_get_next_event(ll);
                }
                if(type         == LL_PHY_EVENT\
                  && ll->state  == BLE_LL_STATE_ADVERTISING \
                  && currentAdvSet->state == ADV_SM_STATE_IDLE)
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
    adv_sequence_process(LL_PHY_EVENT,type);
}

_RAM_CODE
static void adv_sch_callback(_u8 type)
{
    if(type == SCH_TASK_START)
    {
        DEBUG_GPIO_HIGH(GPIO_11);
        DEBUG_GPIO_LOW(GPIO_11);
    }
    else if(type == SCH_TASK_STOP)
    {
        DEBUG_GPIO_HIGH(GPIO_12);
        DEBUG_GPIO_LOW(GPIO_12);
    }
    else if(type == SCH_TASK_CANCELED)
    {
        DEBUG_GPIO_HIGH(GPIO_13);
        DEBUG_GPIO_LOW(GPIO_13);
    }
    else if(type == SCH_TASK_PASSED)
    {
        DEBUG_GPIO_HIGH(GPIO_14);
        DEBUG_GPIO_LOW(GPIO_14);
    }
    adv_sequence_process(LL_SCH_EVENT,type);
}

static void ble_ll_adv_reset(void)
{
    ll_sm_t* ll = ll_get_current_state_machine();
	for(int i=0;i<BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS;i++)
	{
		if(POINTER_VALID(ll->adv->param[i].la))
		{
			tx_free((_u8*)ll->adv->param[i].la);
			ll->adv->param[i].la = NULL;
		}
		if(POINTER_VALID(ll->adv->param[i].data.addr))
		{
			tx_free(ll->adv->param[i].data.addr);
			ll->adv->param[i].data.addr = NULL;
		}
		if(POINTER_VALID(ll->adv->param[i].scanRsp.addr))
		{
			tx_free(ll->adv->param[i].scanRsp.addr);
			ll->adv->param[i].scanRsp.addr = NULL;
		}
		#if(LL_SUPPORT_LE_EXTENDED_ADVERTISING)
		if(POINTER_VALID(ll->adv->param[i].ea))
		{
			if(POINTER_VALID(ll->adv->param[i].ea->chain.entry))
			{
				tx_free((_u8*)ll->adv->param[i].ea->chain.entry);
				ll->adv->param[i].ea->chain.entry = NULL;
			}
			tx_free((_u8*)ll->adv->param[i].ea);
			ll->adv->param[i].ea = NULL;
		}
		#endif
		#if(LL_SUPPORT_LE_PERIODIC_ADVERTISING)
		#endif
		#if(LL_SUPPORT_PERIODIC_ADVERTISING_WITH_RESPONSES_ADVERTISER)
		#endif
	}
}

int ble_ll_enter_advertising_state(ble_ll_event_e event)
{
    if(event == BLE_LL_EVENT_START_ADVERTISING)
    {
        ll_sm_t* ll = ll_get_current_state_machine();
        if(POINTER_NOT_VALID(ll->adv))
        {   
            return 0;
        }
        ll->adv->reset = ble_ll_adv_reset;
        //phy init
        ll->phy.hw_irq_cb  = adv_phy_irq_callback;

        for(int i=0;i<BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS;i++)
        {
            if(ll->adv->param[i].enable)
            {
                ll_internal_adv_param_t* advParam = &ll->adv->param[i];
                //sm init
                advParam->state = ADV_SM_STATE_IDLE;
                ll_extended_adv_map_out_task(ll,&ll->adv->param[i],system_time()+500,system_time()+500+ll->adv->param[i].la->sch.interval,ADV_SCH_MAP_ALL);
            }
        }
        adv_get_next_event(ll);
        //ll entity sch init
        ll->sch.llId         = ll->id;
        ll->sch.type         = SCH_PERIODIC_TASK;
        ll->sch.priority     = LL_ADV_PRIORITY;
        ll->sch.timestamp    = currentAdvSet->la->sch.anchorPoint;//maybe need planner
        ll->sch.period       = currentAdvSet->la->sch.interval;
        ll->sch.duration     = currentAdvSet->la->sch.duration;
        ll->sch.startLatency = currentAdvSet->la->sch.startMargin;
        ll->sch.stopLatency  = currentAdvSet->la->sch.stopMargin;
        ll->sch.delete       = 0;
        ll->sch.cb           = adv_sch_callback;
        if(sch_insert_task(&ll->sch)==SCH_STATUS_SUCCESS)
        {
            sch_start();
        }
        return 1;
    }
    return 0;
}


/*************************************Bluetooth LE Advertising ******************************/


controller_error_code_e ll_set_advertising_parameters(_u16 interval,\
	                                                  ll_advertising_type_e type,\
													  ll_own_address_type_e ownAddressType,\
	                                                  ll_peer_address_type_e peerAddressType,\
													  _u8* peerAddress,_u8 channelMap,\
	                                                  ll_advertising_filter_policy_e policy)
{
	ll_sm_t* ll = ll_get_current_state_machine();
	_u8 index = 0;
	if(ll->adv == NULL)
	{
		ll->adv = (ll_internal_adv_ctrl_t*)tx_malloc(sizeof(ll_internal_adv_ctrl_t));
		ll->adv->param[index].la = (ll_adv_set_t*)tx_malloc(sizeof(ll_adv_set_t));
	}
	ll->adv->param[index].ownAddressType  = ownAddressType;
	ll->adv->param[index].peerAddressType = peerAddressType;
	ll->adv->param[index].filterPolicy = policy;
	txMemcpy(ll->adv->param[index].peerAddress,peerAddress,6);

	ll->adv->param[index].la->sch.interval = interval*625;
	ll->adv->param[index].la->channelCnt   = 0;
    for(int i=0;i<3;i++)
    {
    	if(channelMap&BIT(i))
    	{
    		ll->adv->param[index].la->chnTable[ll->adv->param[index].la->channelCnt++] = 37+i;
    	}
    }
	switch(type)
	{
		case LL_ADV_IND:
		{
			ll->adv->param[index].eventType = ADV_EVENT_CONNECTABLE_SCANNABLE_UNDIRECTED;
		}break;
		case LL_ADV_DIRECT_IND_HIGH_DUTY:
		case LL_ADV_DIRECT_IND_LOW_DUTY:
		{
			ll->adv->param[index].eventType = ADV_EVENT_CONNECTABLE_DIRECTED;
		}break;
		case LL_ADV_SCAN_IND:
		{
			ll->adv->param[index].eventType = ADV_EVENT_SCANNABLE_UNDIRECTED;
		}break;
		case LL_ADV_NONCONN_IND:
		{
			ll->adv->param[index].eventType = ADV_EVENT_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED;
		}break;
	}
	LOG_TRACE(1,"set adv param",0,0)
	return SUCCESS;
}

controller_error_code_e ll_set_advertising_data(_u8* data,_u8 length)
{
	ll_sm_t* ll = ll_get_current_state_machine();
	_u8 index = 0;
	if(ll->adv == NULL)
	{
		ll->adv = (ll_internal_adv_ctrl_t*)tx_malloc(sizeof(ll_internal_adv_ctrl_t));
		ll->adv->param[index].la = (ll_adv_set_t*)tx_malloc(sizeof(ll_adv_set_t));
	}
    if(ll->adv->param[index].data.addr)
	{
		tx_free(ll->adv->param[index].data.addr);
	}
    ll->adv->param[index].data.len  = length;
    ll->adv->param[index].data.addr = tx_malloc(length);
	txMemcpy(ll->adv->param[index].data.addr,data,length);
	LOG_TRACE(1,"set adv data",0,0)
	return SUCCESS;
}

controller_error_code_e ll_set_scan_response_data(_u8* data,_u8 length)
{
	ll_sm_t* ll = ll_get_current_state_machine();
	_u8 index = 0;
	if(ll->adv == NULL)
	{
		ll->adv = (ll_internal_adv_ctrl_t*)tx_malloc(sizeof(ll_internal_adv_ctrl_t));
		ll->adv->param[index].la = (ll_adv_set_t*)tx_malloc(sizeof(ll_adv_set_t));
	}
    if(ll->adv->param[index].scanRsp.addr)
	{
		tx_free(ll->adv->param[index].scanRsp.addr);
	}
    ll->adv->param[index].scanRsp.len  = length;
    ll->adv->param[index].scanRsp.addr = tx_malloc(length);
	txMemcpy(ll->adv->param[index].scanRsp.addr,data,length);
	LOG_TRACE(1,"set scan rsp data",0,0)
	return SUCCESS;
}

controller_error_code_e ll_set_advertising_enable(_u8 enable)
{
	ll_sm_t* ll = ll_get_current_state_machine();
	_u8 index = 0;
	//assert ll->adv == NULL
    if(ll->adv->param[index].enable == enable)
	{
		return SUCCESS;
	}
	LOG_TRACE(1,"set scan enable",&enable,1)
	if(ll->adv->param[index].enable!=enable)
	{
		ll->adv->param[index].enable = enable;
		if(ll->adv->param[index].enable == LL_ADVERTISING_ENABLE)
		{
	        phy_obj_cast(&ll->phy);
	        phy_obj_init(&ll->phy);
	        ll_internal_adv_param_t* advParam = &ll->adv->param[0];
			//sch init
			advParam->la->availableChnCnt     = advParam->la->channelCnt;
			advParam->la->eventCnt        = 0;
			advParam->la->sch.anchorPoint     = system_time() + 500;
			if(advParam->eventType == ADV_EVENT_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED)
			{
				advParam->la->sch.duration = ll->phy.hw_get_prepare_time()+ll_get_air_packet_time(PHY_MODE_1M,BLE_ADV_PRI_PHY_MAX_TX_LEN,0);
			}
			else
			{
				advParam->la->sch.duration = ll->phy.hw_get_prepare_time()+3*ll_get_air_packet_time(PHY_MODE_1M,BLE_ADV_PRI_PHY_MAX_TX_LEN,0)+2*PACKET_DEFAULT_TIFS_TIME;
			}
			advParam->la->sch.startMargin     = 100;
			advParam->la->sch.stopMargin      = 75;
			//phy init
			advParam->la->phy.mode            = PHY_MODE_1M;
			advParam->la->phy.crcInit         = BLE_ADV_CRC_INIT;
			advParam->la->phy.accessCode      = BLE_ADV_ACCESS_CODE;
			advParam->la->phy.rxMaxOctets     = BLE_ADV_PRI_PHY_MAX_RX_LEN;
			advParam->la->phy.rxAddress       = ll_get_shared_phy_rx_address();
			advParam->la->phy.txAddress       = ll_get_shared_phy_tx_address();
			if(BLE_LL_STATE_SUCCESS!=ble_ll_process_event(ll,BLE_LL_EVENT_START_ADVERTISING))
			{
				return IVALID_HCI_COMMAND_PARAMETERS;
			}
		}
		else//LL_ADVERTISING_DISABLE
		{
			IRQ_DISABLE;
			ll->sch.delete=1;
			IRQ_RESTORE;
			if(BLE_LL_STATE_SUCCESS!=ble_ll_process_event(ll,BLE_LL_EVENT_STOP_ADVERTISING))
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
	ll_sm_t* ll = ll_get_current_state_machine();
	if(POINTER_NOT_VALID(ll->adv))
	{
		ll->adv = (ll_internal_adv_ctrl_t*)tx_malloc(sizeof(ll_internal_adv_param_t)*BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS);
		ASSERT(ll->adv != NULL);
		for(int i=0;i<BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS;i++)
		{
			ll->adv->param[i].handle = LL_EXTENDED_ADV_INVALID_HANDLE;
			ll->adv->param[i].sid = i;
		}
	}

	ll_internal_adv_param_t* pAdv = ll_extended_adv_get_entity(pParam->advHandle,1);
	if(POINTER_NOT_VALID(pAdv))
	{
		return MEMORY_CAPACITY_EXCEEDED;
	}
	pAdv->eventProperty     = pParam->advEventProperty;
	pAdv->filterPolicy      = pParam->filterPolicy;
	pAdv->ownAddressType    = pParam->ownAddrType;
	pAdv->peerAddressType   = pParam->peerAddrType;
	pAdv->la->sch.interval  = pParam->primaryAdvInterval * 625;
	pAdv->la->channelCnt    = 0;
    for(int i=0;i<3;i++)
    {
    	if(pParam->primaryAdvChnMap&BIT(i))
    	{
    		pAdv->la->chnTable[pAdv->la->channelCnt++] = 37+i;
    	}
    }
	if(pParam->advEventProperty&LL_ADV_EVENT_PROPERTY_LEGACY_PDU)
	{
		switch(pParam->advEventProperty)
		{
			case (LL_ADV_EVENT_PROPERTY_LEGACY_PDU|LL_ADV_EVENT_PROPERTY_CONNECTED|LL_ADV_EVENT_PROPERTY_SCANNABLE):
			{
				pAdv->eventType = ADV_EVENT_CONNECTABLE_SCANNABLE_UNDIRECTED;
			}
				break;
			case (LL_ADV_EVENT_PROPERTY_LEGACY_PDU|LL_ADV_EVENT_PROPERTY_CONNECTED|LL_ADV_EVENT_PROPERTY_DIRECTED):
			case (LL_ADV_EVENT_PROPERTY_LEGACY_PDU|LL_ADV_EVENT_PROPERTY_CONNECTED|LL_ADV_EVENT_PROPERTY_DIRECTED|LL_ADV_EVENT_PROPERTY_HIGH_DUTY_CONNECTED):
			{
				//todo,high duty cycle
				pAdv->eventType = ADV_EVENT_CONNECTABLE_DIRECTED;
			}
				break;
			case (LL_ADV_EVENT_PROPERTY_LEGACY_PDU|LL_ADV_EVENT_PROPERTY_SCANNABLE):
			{
				pAdv->eventType = ADV_EVENT_SCANNABLE_UNDIRECTED;
			}
				break;
			case (LL_ADV_EVENT_PROPERTY_LEGACY_PDU):
			{
				pAdv->eventType = ADV_EVENT_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED;
			}
		}
	}
	else
	{
		if(POINTER_NOT_VALID(pAdv->ea))
		{
			pAdv->ea = (ll_adv_ea_set_t*)tx_malloc(sizeof(ll_adv_ea_set_t));
		}

		switch(pParam->advEventProperty&(LL_ADV_EVENT_PROPERTY_CONNECTED|LL_ADV_EVENT_PROPERTY_SCANNABLE|LL_ADV_EVENT_PROPERTY_DIRECTED))
		{
			case(LL_ADV_EVENT_PROPERTY_CONNECTED|LL_ADV_EVENT_PROPERTY_DIRECTED):
			{
				pAdv->eventType = ADV_EVENT_EXTENDED_CONNECTABLE_DIRECTED;
				pAdv->schMap |= ADV_SCH_MAP_AUX;
			}
				break;
			case(LL_ADV_EVENT_PROPERTY_CONNECTED):
			{
				pAdv->eventType = ADV_EVENT_EXTENDED_CONNECTABLE_UNDIRECTED;
				pAdv->schMap |= ADV_SCH_MAP_AUX;
			}
				break;
			case(LL_ADV_EVENT_PROPERTY_SCANNABLE|LL_ADV_EVENT_PROPERTY_DIRECTED):
			{
				pAdv->eventType = ADV_EVENT_EXTENDED_SCANNABLE_DIRECTED;
				pAdv->schMap |= ADV_SCH_MAP_AUX;
			}
				break;
			case(LL_ADV_EVENT_PROPERTY_SCANNABLE):
			{
				pAdv->eventType = ADV_EVENT_EXTENDED_SCANNABLE_UNDIRECTED;
				pAdv->schMap |= ADV_SCH_MAP_AUX;
			}
				break;
			case(LL_ADV_EVENT_PROPERTY_DIRECTED):
			{
				pAdv->eventType = ADV_EVENT_EXTENDED_NON_CONNECTABLE_NON_SCANNABLE_DIRECTED_WITHOUT_AUXILIARY;
			}
				break;
			case(0):
			{
				pAdv->eventType = ADV_EVENT_EXTENDED_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED_WITHOUT_AUXILIARY;
			}
				break;
		}
		pAdv->sid                      = pParam->advSid;
		pAdv->ea->scanReqNotifyEnable  = pParam->scanReqNotifyEnable;
		pAdv->ea->secondaryMaxSkip     = pParam->secondaryAdvMaxSkip;
		pAdv->ea->power  = 0;
		if(pParam->txPower!=0x7F)
		{
			pAdv->ea->power = pParam->txPower;
		}
		if(pParam->secondaryAdvPhy == LL_ADV_PHY_CODED)
		{
			if(pParam->secondaryAdvphyOptions == LL_ADV_PHY_OPTIONS_PREFER_S2||\
			pParam->secondaryAdvphyOptions == LL_ADV_PHY_OPTIONS_REQUIRE_S2)
			{
				pAdv->ea->phyMode = PHY_MODE_CODED_S2;
			}
			else if(pParam->secondaryAdvphyOptions == LL_ADV_PHY_OPTIONS_PREFER_S8||\
					pParam->secondaryAdvphyOptions == LL_ADV_PHY_OPTIONS_REQUIRE_S8)
			{
				pAdv->ea->phyMode = (_u8)PHY_MODE_CODED_S8;
			}
			else
			{
				pAdv->ea->phyMode = (_u8)PHY_MODE_CODED_S8;
			}
		}
		else if(pParam->secondaryAdvPhy == LL_ADV_PHY_1M)
		{
			pAdv->ea->phyMode = PHY_MODE_1M;
		}
		else
		{
			pAdv->ea->phyMode = PHY_MODE_2M;
		}
	}
	if(pParam->primaryAdvPhy == LL_ADV_PHY_CODED)
	{
		if(pParam->primaryAdvphyOptions == LL_ADV_PHY_OPTIONS_PREFER_S2||\
	       pParam->primaryAdvphyOptions == LL_ADV_PHY_OPTIONS_REQUIRE_S2)
		{
			pAdv->la->phy.mode = PHY_MODE_CODED_S2;
		}
		else if(pParam->primaryAdvphyOptions == LL_ADV_PHY_OPTIONS_PREFER_S8||\
	            pParam->primaryAdvphyOptions == LL_ADV_PHY_OPTIONS_REQUIRE_S8)
		{
			pAdv->la->phy.mode = (_u8)PHY_MODE_CODED_S8;
		}
		else
		{
			pAdv->la->phy.mode = (_u8)PHY_MODE_CODED_S8;
		}
	}
	else
	{
		pAdv->la->phy.mode = PHY_MODE_1M;
	}
	return SUCCESS;
}

controller_error_code_e ll_set_extended_advertising_data(_u8 advHandle,\
														ll_advertising_data_operation_e operation,\
														ll_advertising_data_fragment_perference_e fragPref,\
														_u8 dataLen,\
														_u8* data)
{
	ll_internal_adv_param_t* pAdv = ll_extended_adv_get_entity(advHandle,0);
	if(POINTER_NOT_VALID(pAdv))
	{
		return UNKNOWN_ADVERTISING_IDENTIFIER;
	}
	if(pAdv->eventProperty & LL_ADV_EVENT_PROPERTY_LEGACY_PDU)
	{
		if(dataLen > 31||\
		   operation!=LL_ADV_DATA_OPERATION_COMPLETE||\
		   pAdv->eventType == ADV_EVENT_CONNECTABLE_DIRECTED)//direct pdu don't have advertising data
		{
			return IVALID_HCI_COMMAND_PARAMETERS;
		}
	}

	if(operation == LL_ADV_DATA_OPERATION_UNCHANGED)
	{
		if(pAdv->enable == 0||\
		   pAdv->data.len == 0||\
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

	if(pAdv->enable)
	{
		if(operation!=LL_ADV_DATA_OPERATION_COMPLETE&&\
		   operation!=LL_ADV_DATA_OPERATION_UNCHANGED)
		{
			return COMMAND_DISALLOWED;
		}
	}

	if(pAdv->eventType == ADV_EVENT_EXTENDED_SCANNABLE_DIRECTED||\
	   pAdv->eventType == ADV_EVENT_EXTENDED_SCANNABLE_UNDIRECTED)
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
	if(!(pAdv->eventProperty&LL_ADV_EVENT_PROPERTY_LEGACY_PDU))
	{
		if(ll_get_air_packet_time(pAdv->ea->phyMode,dataLen,0)>(pAdv->la->sch.interval*3/4))
		{
			return PACKET_TOO_LONG;
		}
		if(pAdv->eventProperty & (LL_ADV_EVENT_PROPERTY_CONNECTED))
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
		if(ll_get_air_packet_time(pAdv->ea->phyMode,dataFillOffset+dataLen,0)>(pAdv->la->sch.interval*3/4))
		{
			dataFillOffset = 0;
			return PACKET_TOO_LONG;
		}
		if(dataFillOffset == 0)
		{
			return COMMAND_DISALLOWED;
		}
	}

	if(POINTER_NOT_VALID(pAdv->data.addr))
	{
		if(operation == LL_ADV_DATA_OPERATION_COMPLETE)
		{
			pAdv->data.addr = tx_malloc(dataLen);
		}
		else
		{
			pAdv->data.addr = tx_malloc(BLE_ADV_MAXIMUM_ADVERTISING_DATA_LENGTH);
		}
	}

	if(pAdv->eventType == ADV_EVENT_EXTENDED_NON_CONNECTABLE_NON_SCANNABLE_DIRECTED_WITHOUT_AUXILIARY)
	{
		pAdv->eventType = ADV_EVENT_EXTENDED_NON_CONNECTABLE_NON_SCANNABLE_DIRECTED_WITH_AUXILIARY;
		pAdv->schMap |= ADV_SCH_MAP_AUX;
	}
	else if(pAdv->eventType == ADV_EVENT_EXTENDED_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED_WITHOUT_AUXILIARY)
	{
		pAdv->eventType = ADV_EVENT_EXTENDED_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED_WITH_AUXILIARY;
		pAdv->schMap |= ADV_SCH_MAP_AUX;
	}

	switch(operation)
	{
		case LL_ADV_DATA_OPERATION_INTERMEDIATE_FRAGMENT:
		{
			txMemcpy4(pAdv->data.addr+dataFillOffset,data,dataLen);
			dataFillOffset+=dataLen;
		}
			break;
		case LL_ADV_DATA_OPERATION_FIRST_FRAGMENT:
		{
			txMemcpy4(pAdv->data.addr,data,dataLen);
			dataFillOffset = dataLen;
		}
			break;
		case LL_ADV_DATA_OPERATION_LAST_FRAGMENT:
		{
			txMemcpy4(pAdv->data.addr+dataFillOffset,data,dataLen);
			pAdv->ea->did++;
			pAdv->data.len = dataFillOffset + dataLen;
		}
			break;
		case LL_ADV_DATA_OPERATION_COMPLETE:
		{
			txMemcpy(pAdv->data.addr,data,dataLen);
			pAdv->ea->did++;
			pAdv->data.len = dataLen;
		}
			break;
		case LL_ADV_DATA_OPERATION_UNCHANGED:
		{
			pAdv->ea->did++;
		}
			break;
	}
	if((!(pAdv->eventProperty&LL_ADV_EVENT_PROPERTY_LEGACY_PDU))&&(operation == LL_ADV_DATA_OPERATION_LAST_FRAGMENT||operation == LL_ADV_DATA_OPERATION_COMPLETE))
	{
		if(POINTER_VALID(pAdv->ea->chain.entry))
		{
			tx_free((_u8*)pAdv->ea->chain.entry);
		}
		pAdv->ea->aux.data.addr= pAdv->data.addr;
		//process data fragment
		if(pAdv->data.len<=(BLE_ADV_SEC_PHY_MAX_TX_LEN - BLE_ADV_EXTENDED_HEADER_MAX_LEN))
		{
			pAdv->ea->chain.cnt    = 0;//only aux packet exist
			pAdv->ea->aux.data.len = pAdv->data.len;
		}
		else
		{
			_u8 remainLen          = ((pAdv->data.len-(BLE_ADV_SEC_PHY_MAX_TX_LEN - BLE_ADV_EXTENDED_HEADER_MAX_LEN)))%BLE_ADV_SEC_PHY_MAX_TX_LEN;
			_u8 chainCnt           = ((pAdv->data.len-(BLE_ADV_SEC_PHY_MAX_TX_LEN - BLE_ADV_EXTENDED_HEADER_MAX_LEN)))/BLE_ADV_SEC_PHY_MAX_TX_LEN + (remainLen==0?0:1);
			pAdv->schMap          |= ADV_SCH_MAP_AUX_CHAIN;
			pAdv->ea->chain.entry  = (ll_adv_entry_t*)tx_malloc(chainCnt*sizeof(ll_adv_entry_t));
			pAdv->ea->chain.cnt    = chainCnt;
			pAdv->ea->aux.data.len = BLE_ADV_SEC_PHY_MAX_TX_LEN - BLE_ADV_EXTENDED_HEADER_MAX_LEN;
			_u16 offset            = pAdv->ea->aux.data.len;
			for(_u8 i=0;i<chainCnt-1;i++)
			{
				pAdv->ea->chain.entry[i].data.len = BLE_ADV_SEC_PHY_MAX_TX_LEN;
				pAdv->ea->chain.entry[i].data.addr= (pAdv->data.addr+offset);
				offset+=BLE_ADV_SEC_PHY_MAX_TX_LEN;
			}
			if(remainLen!=0)
			{
				pAdv->ea->chain.entry[chainCnt-1].data.len = remainLen;
				pAdv->ea->chain.entry[chainCnt-1].data.addr= (pAdv->data.addr+offset);
			}
		}
	}
	pAdv->ea->advDatafragPerf = fragPref;
	return SUCCESS;
}


controller_error_code_e ll_set_extended_scan_response_data(_u8 advHandle,\
															ll_advertising_data_operation_e operation,\
															ll_advertising_data_fragment_perference_e fragPref,\
															_u8 dataLen,\
															_u8* data)
{
	ll_internal_adv_param_t* pAdv = ll_extended_adv_get_entity(advHandle,0);
	if(POINTER_NOT_VALID(pAdv))
	{
		return UNKNOWN_ADVERTISING_IDENTIFIER;
	}

	if(pAdv->eventProperty & LL_ADV_EVENT_PROPERTY_LEGACY_PDU)
	{
		if(dataLen > 31||\
		   operation!=LL_ADV_DATA_OPERATION_COMPLETE||\
		   ((pAdv->eventType != ADV_EVENT_CONNECTABLE_SCANNABLE_UNDIRECTED)&&(pAdv->eventType != ADV_EVENT_SCANNABLE_UNDIRECTED)))
		{
			return IVALID_HCI_COMMAND_PARAMETERS;
		}
	}
	else
	{
		if(pAdv->eventType != ADV_EVENT_EXTENDED_SCANNABLE_DIRECTED&&\
		   pAdv->eventType != ADV_EVENT_EXTENDED_SCANNABLE_UNDIRECTED&&\
		   pAdv->eventType != ADV_EVENT_CONNECTABLE_SCANNABLE_UNDIRECTED&&\
		   pAdv->eventType != ADV_EVENT_SCANNABLE_UNDIRECTED)
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
		if(pAdv->enable)
		{
			return COMMAND_DISALLOWED;
		}
	}

	if(pAdv->enable)
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

	if(!(pAdv->eventProperty&LL_ADV_EVENT_PROPERTY_LEGACY_PDU))
	{
		if(ll_get_air_packet_time(pAdv->ea->phyMode,dataLen,0)>(pAdv->la->sch.interval*3/4))
		{
			return PACKET_TOO_LONG;
		}
		if(pAdv->eventProperty & (LL_ADV_EVENT_PROPERTY_SCANNABLE))
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
		if(ll_get_air_packet_time(pAdv->ea->phyMode,dataFillOffset+dataLen,0)>(pAdv->la->sch.interval*3/4))
		{
			return PACKET_TOO_LONG;
		}
		if(dataFillOffset == 0)
		{
			return COMMAND_DISALLOWED;
		}
	}
	if(POINTER_NOT_VALID(pAdv->scanRsp.addr))
	{
		if(operation == LL_ADV_DATA_OPERATION_COMPLETE)
		{
			pAdv->scanRsp.addr = tx_malloc(dataLen);
		}
		else
		{
			pAdv->scanRsp.addr = tx_malloc(BLE_ADV_MAXIMUM_ADVERTISING_DATA_LENGTH);
		}
	}
	switch(operation)
	{
		case LL_ADV_DATA_OPERATION_INTERMEDIATE_FRAGMENT:
		{
			txMemcpy4(pAdv->scanRsp.addr+dataFillOffset,data,dataLen);
			dataFillOffset+=dataLen;
		}
			break;
		case LL_ADV_DATA_OPERATION_FIRST_FRAGMENT:
		{
			txMemcpy4(pAdv->scanRsp.addr,data,dataLen);
			dataFillOffset=dataLen;
		}
			break;
		case LL_ADV_DATA_OPERATION_LAST_FRAGMENT:
		{
			txMemcpy4(pAdv->scanRsp.addr+dataFillOffset,data,dataLen);
			pAdv->scanRsp.len = dataFillOffset+dataLen;
			dataFillOffset = 0;

		}
			break;
		case LL_ADV_DATA_OPERATION_COMPLETE:
		{
			txMemcpy(pAdv->scanRsp.addr,data,dataLen);
			pAdv->scanRsp.len = dataLen;
		}
			break;
	}
	if(!(pAdv->eventProperty&LL_ADV_EVENT_PROPERTY_LEGACY_PDU)&&(operation == LL_ADV_DATA_OPERATION_LAST_FRAGMENT||operation == LL_ADV_DATA_OPERATION_COMPLETE))
	{
		if(POINTER_VALID(pAdv->ea->chain.entry))
		{
			tx_free((_u8*)pAdv->ea->chain.entry);
		}
		pAdv->ea->aux.data.addr= pAdv->scanRsp.addr;
		//process data fragment
		if(pAdv->scanRsp.len<=(BLE_ADV_SEC_PHY_MAX_TX_LEN - BLE_ADV_EXTENDED_HEADER_MAX_LEN))
		{
			pAdv->ea->chain.cnt= 0;//only aux packet exist
			pAdv->ea->aux.data.len = pAdv->scanRsp.len;
		}
		else
		{
			_u8 remainLen         = ((pAdv->scanRsp.len-(BLE_ADV_SEC_PHY_MAX_TX_LEN - BLE_ADV_EXTENDED_HEADER_MAX_LEN)))%BLE_ADV_SEC_PHY_MAX_TX_LEN;
			_u8 chainCnt          = ((pAdv->scanRsp.len-(BLE_ADV_SEC_PHY_MAX_TX_LEN - BLE_ADV_EXTENDED_HEADER_MAX_LEN)))/BLE_ADV_SEC_PHY_MAX_TX_LEN + (remainLen==0?0:1);
			pAdv->schMap         |= ADV_SCH_MAP_AUX_CHAIN;
			pAdv->ea->chain.entry = (ll_adv_entry_t*)tx_malloc(chainCnt*sizeof(ll_adv_entry_t));
			pAdv->ea->chain.cnt   = chainCnt;
			pAdv->ea->aux.data.len = BLE_ADV_SEC_PHY_MAX_TX_LEN - BLE_ADV_EXTENDED_HEADER_MAX_LEN;
			_u16 offset           = pAdv->ea->aux.data.len;
			for(_u8 i=0;i<chainCnt-1;i++)
			{
				pAdv->ea->chain.entry[i].data.len = BLE_ADV_SEC_PHY_MAX_TX_LEN;
				pAdv->ea->chain.entry[i].data.addr= (pAdv->scanRsp.addr+offset);
				offset+=BLE_ADV_SEC_PHY_MAX_TX_LEN;
			}
			if(remainLen!=0)
			{
				pAdv->ea->chain.entry[chainCnt-1].data.len = remainLen;
				pAdv->ea->chain.entry[chainCnt-1].data.addr= (pAdv->scanRsp.addr+offset);
			}
		}
	}
	pAdv->ea->scanRspDatafragPerf = fragPref;
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
	ll_sm_t* ll = ll_get_current_state_machine();

	if(POINTER_NOT_VALID(ll->adv))
	{
		return COMMAND_DISALLOWED;
	}
	if((enable == 0) && (numSets == 0))
	{
		for(int i=0;i<BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS;i++)
		{
			if(ll->adv->param[i].handle <= 0xEF)
			{
				if((ll->adv->param[i].eventProperty & LL_ADV_EVENT_PROPERTY_SCANNABLE)&&ll->adv->param[i].scanRsp.len == 0)
				{
					return COMMAND_DISALLOWED;
				}
				if((ll->adv->param[i].eventProperty & LL_ADV_EVENT_PROPERTY_CONNECTED)&&ll->adv->param[i].data.len == 0)
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
			ll_internal_adv_param_t* pAdv = ll_extended_adv_get_entity(pEnableSubFiled[i].advHandle,0);
			if(POINTER_NOT_VALID(pAdv))
			{
				return IVALID_HCI_COMMAND_PARAMETERS;
			}
			if((pAdv->eventProperty & LL_ADV_EVENT_PROPERTY_SCANNABLE)&&pAdv->scanRsp.len == 0)
			{
				return COMMAND_DISALLOWED;
			}
			if((pAdv->eventProperty & LL_ADV_EVENT_PROPERTY_CONNECTED)&&pAdv->data.len == 0)
			{
				return IVALID_HCI_COMMAND_PARAMETERS;//?,need check if valid
			}
		}
	}
	if((enable == 0) && (numSets == 0))
	{
		for(int i=0;i<BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS;i++)
		{
			if(ll->adv->param[i].handle <= 0xEF)
			{
				ll->adv->param[i].enable   = enable;
				if(POINTER_VALID(ll->adv->param[i].ea))
				{
					if(pEnableSubFiled[i].duration!=0)
					{
						ll->adv->param[i].ea->expireTime =  system_time() + ll->adv->param[i].la->sch.interval + pEnableSubFiled[i].duration*10000;
					}
					else
					{
						ll->adv->param[i].ea->expireTime = 0;
					}

					if(pEnableSubFiled[i].maxEvents!=0)
					{
						ll->adv->param[i].ea->maxEvents    = pEnableSubFiled[i].maxEvents;
						ll->adv->param[i].ea->eventCnt = 0;
					}
					else
					{
						ll->adv->param[i].ea->maxEvents = 0;
					}
				}
			}
		}
	}
	else
	{
		for(_u8 i=0;i<numSets;i++)
		{
			ll_internal_adv_param_t* pAdv = ll_extended_adv_get_entity(pEnableSubFiled[i].advHandle,0);
			if(pEnableSubFiled[i].duration!=0)
			{
				pAdv->ea->expireTime =  system_time() + pAdv->la->sch.interval + pEnableSubFiled[i].duration*10000;
			}
			else
			{
				pAdv->ea->expireTime = 0;
			}

			if(pEnableSubFiled[i].maxEvents!=0)
			{
				pAdv->ea->maxEvents    = pEnableSubFiled[i].maxEvents;
				pAdv->ea->eventCnt = 0;
			}
			else
			{
				pAdv->ea->maxEvents = 0;
			}
			if(!pAdv->enable && enable)
			{
				pAdv->enable = enable;
				pAdv->schMap|=ADV_SCH_MAP_PRI;
				//la phy init
				pAdv->la->phy.crcInit         = BLE_ADV_CRC_INIT;
				pAdv->la->phy.accessCode      = BLE_ADV_ACCESS_CODE;
				pAdv->la->phy.rxMaxOctets     = BLE_ADV_SEC_PHY_MAX_TX_LEN;
                pAdv->la->phy.rxAddress       = ll_get_shared_phy_rx_address();
                pAdv->la->phy.txAddress       = ll_get_shared_phy_tx_address();
				//la sch init
				pAdv->la->availableChnCnt = 0;
				pAdv->la->availableChnCnt = pAdv->la->channelCnt;
				pAdv->la->eventCnt    = 0;
				pAdv->la->sch.startMargin = 100;
				pAdv->la->sch.stopMargin  = 100;
		        phy_obj_cast(&ll->phy);
		        phy_obj_init(&ll->phy);
                if(pAdv->eventProperty&LL_ADV_EVENT_PROPERTY_LEGACY_PDU)
                {
                    if(pAdv->eventType == ADV_EVENT_NON_CONNECTABLE_NON_SCANNABLE_UNDIRECTED)
                    {
                    	pAdv->la->sch.duration = ll->phy.hw_get_prepare_time()+ll_get_air_packet_time(pAdv->la->phy.mode,BLE_ADV_PRI_PHY_MAX_TX_LEN,0)+PACKET_DEFAULT_TIFS_TIME;
                    }
                    else
                    {
                    	pAdv->la->sch.duration = ll->phy.hw_get_prepare_time()+3*ll_get_air_packet_time(pAdv->la->phy.mode,BLE_ADV_PRI_PHY_MAX_TX_LEN,0)+2*PACKET_DEFAULT_TIFS_TIME;
                    }
                }
                else
                {
					if(POINTER_NOT_VALID(pAdv->ea->chain.entry))
					{
						pAdv->ea->chain.cnt = 0;
					}
					pAdv->la->sch.duration = ll->phy.hw_get_prepare_time()+ll_get_air_packet_time(pAdv->la->phy.mode,BLE_ADV_PRI_PHY_MAX_TX_LEN,0)+PACKET_DEFAULT_TIFS_TIME;
                	pAdv->ea->aux.phy.crcInit         = BLE_ADV_CRC_INIT;
                	pAdv->ea->aux.phy.accessCode      = BLE_ADV_ACCESS_CODE;
                	pAdv->ea->aux.phy.rxMaxOctets     = BLE_ADV_SEC_PHY_MAX_TX_LEN;
                	pAdv->ea->aux.phy.rxAddress       = ll_get_shared_phy_rx_address();
                	pAdv->ea->aux.phy.txAddress       = ll_get_shared_phy_tx_address();
					pAdv->ea->eventCnt = 0;
					pAdv->ea->aux.phy.mode            = pAdv->ea->phyMode;
                    //ea sch and phy init
                	if(pAdv->eventProperty&LL_ADV_EVENT_PROPERTY_CONNECTED)
                	{
						pAdv->ea->aux.sch.duration = ll->phy.hw_get_prepare_time()+ll_get_air_packet_time(pAdv->ea->aux.phy.mode,2+BLE_ADV_EXTENDED_HEADER_MAX_LEN+pAdv->ea->aux.data.len,0)+PACKET_DEFAULT_TIFS_TIME\
												        + ll_get_air_packet_time(pAdv->ea->aux.phy.mode,3+sizeof(scan_type_aux_scan_req_t),0)+PACKET_DEFAULT_TIFS_TIME\
												        + ll_get_air_packet_time(pAdv->ea->aux.phy.mode,3+sizeof(init_type_auxConnectRsp_t),0);
					}
					else if(pAdv->eventProperty&LL_ADV_EVENT_PROPERTY_SCANNABLE)
                	{
						pAdv->ea->aux.sch.duration = ll->phy.hw_get_prepare_time()+ll_get_air_packet_time(pAdv->ea->aux.phy.mode,2+BLE_ADV_EXTENDED_HEADER_MAX_LEN,0)+PACKET_DEFAULT_TIFS_TIME\
											            + ll_get_air_packet_time(pAdv->ea->aux.phy.mode,3+sizeof(scan_type_aux_scan_req_t),0)+PACKET_DEFAULT_TIFS_TIME\
												        + ll_get_air_packet_time(pAdv->ea->aux.phy.mode,2+BLE_ADV_EXTENDED_HEADER_MAX_LEN+pAdv->ea->aux.data.len,0);
					}
					else
					{
						pAdv->ea->aux.sch.duration = ll->phy.hw_get_prepare_time()+ll_get_air_packet_time(pAdv->ea->aux.phy.mode,2+BLE_ADV_EXTENDED_HEADER_MAX_LEN+pAdv->ea->aux.data.len,0);
					}

                    if(pAdv->schMap&ADV_SCH_MAP_AUX_CHAIN)
                    {
                        for(int i=0;i<pAdv->ea->chain.cnt;i++)
                        {
                        	pAdv->ea->chain.entry[i].sch.startMargin = 100;
                            if(i==pAdv->ea->chain.cnt)
                            {
                            	pAdv->ea->chain.entry[i].sch.stopMargin  = 200;
                            }
                            else
                            {
                            	pAdv->ea->chain.entry[i].sch.stopMargin  = 100;
                            }
                            pAdv->ea->chain.entry[i].phy.crcInit    = BLE_ADV_CRC_INIT;
                            pAdv->ea->chain.entry[i].phy.accessCode = BLE_ADV_ACCESS_CODE;
                            pAdv->ea->chain.entry[i].phy.mode       = pAdv->ea->phyMode;
                            pAdv->ea->chain.entry[i].phy.txAddress  = ll_get_shared_phy_tx_address();
							pAdv->ea->chain.entry[i].sch.duration   = ll->phy.hw_get_prepare_time()+ll_get_air_packet_time(pAdv->ea->chain.entry[i].phy.mode,2+BLE_ADV_EXTENDED_HEADER_MAX_LEN+pAdv->ea->chain.entry[i].data.len,0)+PACKET_DEFAULT_TIFS_TIME;
                        }
                    	pAdv->ea->aux.sch.startMargin     = 100;
                    	pAdv->ea->aux.sch.stopMargin      = 100;//shall location the next event
                    }
                    else
                    {
                    	pAdv->ea->aux.sch.startMargin     = 100;
                    	pAdv->ea->aux.sch.stopMargin      = 200;//shall location the next event
                    }
                }
				#if(LL_SUPPORT_LE_PERIODIC_ADVERTISING)
                if(POINTER_VALID(pAdv->pa))
                {
                	if(pAdv->pa->enable&&(!pAdv->pa->active))
                	{
                		pAdv->pa->active = 1;
                	}
                }
				#endif
			}
			else if(pAdv->enable && !enable)
			{
				pAdv->enable = enable;
			}
		}
	}
	_u8 gEnable = 0;
	for(int i=0;i<BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS;i++)
	{
		if(ll->adv->param[i].handle!=LL_EXTENDED_ADV_INVALID_HANDLE)
		{
			if(ll->adv->param[i].enable)
			{
				gEnable = 1;
			}
		}
	}
	if(gEnable)
	{
		ble_ll_process_event(ll,BLE_LL_EVENT_START_ADVERTISING);
	}
	else
	{
		IRQ_DISABLE;
		ll->sch.delete=1;
		IRQ_RESTORE;
		ble_ll_process_event(ll,BLE_LL_EVENT_STOP_ADVERTISING);
	}

	return SUCCESS;
}

controller_error_code_e ll_set_adv_set_random_address(_u8 advHandle,_u8* address)
{
	ll_internal_adv_param_t* pAdv = ll_extended_adv_get_entity(advHandle,0);

	if(POINTER_NOT_VALID(pAdv))
	{
		return UNKNOWN_ADVERTISING_IDENTIFIER;
	}
	if(pAdv->enable &&\
	  (pAdv->eventProperty&LL_ADV_EVENT_PROPERTY_CONNECTED))
	{
		return COMMAND_DISALLOWED;
	}
	txMemcpy(pAdv->randomAddress,address,6);
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
	ll_internal_adv_param_t* pAdv = ll_extended_adv_get_entity(advHandle,0);
	if(POINTER_NOT_VALID(pAdv))
	{
		return IVALID_HCI_COMMAND_PARAMETERS;
	}
	if(pAdv->enable)
	{
		return COMMAND_DISALLOWED;
	}
	if(POINTER_VALID(pAdv->data.addr))
	{
		tx_free(pAdv->data.addr);
	}
	if(POINTER_VALID(pAdv->scanRsp.addr))
	{
		tx_free(pAdv->scanRsp.addr);
	}
	if(POINTER_VALID(pAdv->ea->chain.entry))
	{
		tx_free((_u8*)pAdv->ea->chain.entry);
	}
	if(POINTER_VALID(pAdv->la))
	{
		tx_free((_u8*)pAdv->la);
	}
	if(POINTER_VALID(pAdv->ea))
	{
		tx_free((_u8*)pAdv->ea);
	}
	txMemsetByte((_u8*)pAdv,0,sizeof(ll_internal_adv_param_t));
	pAdv->handle = LL_EXTENDED_ADV_INVALID_HANDLE;

	if(ll_extended_adv_get_current_set_number()==0)
	{
		ll_sm_t* ll = ll_get_current_state_machine();
		ble_ll_process_event(ll,BLE_LL_EVENT_STOP_ADVERTISING);
	}
	return SUCCESS;
}

controller_error_code_e ll_clear_advertising_sets(void)
{
	ll_sm_t* ll = ll_get_current_state_machine();

	for(int i=0;i<BLE_ADV_SUPPORTED_NUMBER_OF_ADV_SETS;i++)
	{
		if(ll->adv->param[i].handle!=LL_EXTENDED_ADV_INVALID_HANDLE)
		{
			ll_internal_adv_param_t* pAdv = &ll->adv->param[i];
			if(ll->adv->param[i].enable)
			{
				return COMMAND_DISALLOWED;
			}
			if(POINTER_VALID(pAdv->data.addr))
			{
				tx_free(pAdv->data.addr);
			}
			if(POINTER_VALID(pAdv->scanRsp.addr))
			{
				tx_free(pAdv->scanRsp.addr);
			}
			if(POINTER_VALID(pAdv->ea->chain.entry))
			{
				tx_free((_u8*)pAdv->ea->chain.entry);
			}
			if(POINTER_VALID(pAdv->la))
			{
				tx_free((_u8*)pAdv->la);
			}
			if(POINTER_VALID(pAdv->ea))
			{
				tx_free((_u8*)pAdv->ea);
			}
		}
	}
	ble_ll_process_event(ll,BLE_LL_EVENT_STOP_ADVERTISING);
	return SUCCESS;
}

#endif/*(LL_SUPPORT_LE_EXTENDED_ADVERTISING)*/

#if(LL_SUPPORT_LE_PERIODIC_ADVERTISING)

controller_error_code_e ll_set_periodic_advertising_paramters(_u8 advHandle,_u8 interval,_u16 property)
{
	ll_internal_adv_param_t* pAdv = ll_extended_adv_get_entity(advHandle,0);
	if(POINTER_NOT_VALID(pAdv))
	{
		return UNKNOWN_ADVERTISING_IDENTIFIER;
	}
	if(POINTER_NOT_VALID(pAdv->pa))
	{
		pAdv->pa = (ll_adv_pa_set_t*)tx_malloc(sizeof(ll_adv_pa_set_t));
	}
	if(property&LL_ADV_EVENT_PROPERTY_INCLUDE_TX_POWER)
	{
		pAdv->pa->includeTxPower = 1;
	}
	pAdv->pa->sync.sch.interval = 1250*interval;
	return SUCCESS;
}

controller_error_code_e ll_set_periodic_advertising_data(_u8 advHandle,ll_advertising_data_operation_e operation,_u8 dataLen,_u8* data)
{
	ll_internal_adv_param_t* pAdv = ll_extended_adv_get_entity(advHandle,0);
	if(POINTER_NOT_VALID(pAdv))
	{
		return UNKNOWN_ADVERTISING_IDENTIFIER;
	}
	if((!pAdv->schMap&ADV_SCH_MAP_PA))
	{
		return COMMAND_DISALLOWED;
	}
	#if (LL_SUPPORT_PERIODIC_ADVERTISING_WITH_RESPONSES_ADVERTISER)
	if(pAdv->schMap&ADV_SCH_MAP_PAWR)
	{
		return COMMAND_DISALLOWED;
	}
	#endif
	static _u32 dataFillOffset = 0;
	if(dataLen > BLE_ADV_MAXIMUM_ADVERTISING_DATA_LENGTH || (dataLen+dataFillOffset>BLE_ADV_MAXIMUM_ADVERTISING_DATA_LENGTH))
	{
		return MEMORY_CAPACITY_EXCEEDED;
	}
	if(ll_get_air_packet_time(pAdv->ea->phyMode,dataFillOffset+dataLen,0)>(pAdv->pa->sync.sch.interval*3/4))
	{
		return PACKET_TOO_LONG;
	}
	if(operation == LL_ADV_DATA_OPERATION_UNCHANGED)
	{
		if(pAdv->pa->enable == 0\
		||POINTER_NOT_VALID(pAdv->pa->data.addr)\
	    ||pAdv->pa->data.len==0\
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
	if(pAdv->pa->enable\
	&&(operation!=LL_ADV_DATA_OPERATION_COMPLETE)\
	&&(operation!=LL_ADV_DATA_OPERATION_UNCHANGED))
	{
		return COMMAND_DISALLOWED;
	}
	if(POINTER_NOT_VALID(pAdv->pa->data.addr))
	{
		if(operation == LL_ADV_DATA_OPERATION_COMPLETE)
		{
			pAdv->pa->data.addr = tx_malloc(dataLen);
		}
		else
		{
			pAdv->pa->data.addr = tx_malloc(BLE_ADV_MAXIMUM_ADVERTISING_DATA_LENGTH);
		}
	}
	switch(operation)
	{
		case LL_ADV_DATA_OPERATION_INTERMEDIATE_FRAGMENT:
		{
			txMemcpy4(pAdv->pa->data.addr+dataFillOffset,data,dataLen);
			dataFillOffset+=dataLen;
		}
			break;
		case LL_ADV_DATA_OPERATION_FIRST_FRAGMENT:
		{
			txMemcpy4(pAdv->pa->data.addr,data,dataLen);
			dataFillOffset=dataLen;
		}
			break;
		case LL_ADV_DATA_OPERATION_LAST_FRAGMENT:
		{
			txMemcpy4(pAdv->pa->data.addr+dataFillOffset,data,dataLen);
			pAdv->pa->data.len = dataFillOffset+dataLen;
			pAdv->pa->did++;
			dataFillOffset = 0;
		}
			break;
		case LL_ADV_DATA_OPERATION_COMPLETE:
		{
			txMemcpy(pAdv->pa->data.addr,data,dataLen);
			pAdv->pa->did++;
			pAdv->pa->data.len = dataLen;
		}
			break;
		case LL_ADV_DATA_OPERATION_UNCHANGED:
		{
			pAdv->pa->did++;
		}
			break;
	}
	if(operation == LL_ADV_DATA_OPERATION_LAST_FRAGMENT||operation == LL_ADV_DATA_OPERATION_COMPLETE)
	{
		if(POINTER_VALID(pAdv->pa->chain.entry))
		{
			tx_free((_u8*)pAdv->pa->chain.entry);
		}
		pAdv->pa->sync.data.addr= pAdv->pa->data.addr;
		//process data fragment
		if(pAdv->pa->data.len<=(BLE_ADV_SEC_PHY_MAX_TX_LEN - BLE_ADV_EXTENDED_HEADER_MAX_LEN))
		{
			pAdv->pa->chain.cnt= 0;//only aux packet exist
			pAdv->pa->sync.data.len = pAdv->pa->data.len;
		}
		else
		{
			_u8 remainLen         = ((pAdv->pa->data.len-(BLE_ADV_SEC_PHY_MAX_TX_LEN - BLE_ADV_EXTENDED_HEADER_MAX_LEN)))%BLE_ADV_SEC_PHY_MAX_TX_LEN;
			_u8 chainCnt          = ((pAdv->pa->data.len-(BLE_ADV_SEC_PHY_MAX_TX_LEN - BLE_ADV_EXTENDED_HEADER_MAX_LEN)))/BLE_ADV_SEC_PHY_MAX_TX_LEN + (remainLen==0?0:1);
			pAdv->schMap         |= ADV_SCH_MAP_PA_CHAIN;
			pAdv->pa->chain.entry = (ll_adv_entry_t*)tx_malloc(chainCnt*sizeof(ll_adv_entry_t));
			pAdv->pa->chain.cnt   = chainCnt;
			pAdv->pa->sync.data.len = BLE_ADV_SEC_PHY_MAX_TX_LEN - BLE_ADV_EXTENDED_HEADER_MAX_LEN;
			_u16 offset           = pAdv->pa->sync.data.len;
			for(_u8 i=0;i<chainCnt-1;i++)
			{
				pAdv->pa->chain.entry[i].data.len = BLE_ADV_SEC_PHY_MAX_TX_LEN;
				pAdv->pa->chain.entry[i].data.addr= (pAdv->pa->data.addr+offset);
				offset+=BLE_ADV_SEC_PHY_MAX_TX_LEN;
			}
			if(remainLen!=0)
			{
				pAdv->pa->chain.entry[chainCnt-1].data.len = remainLen;
				pAdv->pa->chain.entry[chainCnt-1].data.addr= (pAdv->pa->data.addr+offset);
			}
		}
	}
	return SUCCESS;
}

controller_error_code_e ll_set_periodic_advertising_enable(_u8 enable,_u8 advHandle)
{
	ll_internal_adv_param_t* pAdv = ll_extended_adv_get_entity(advHandle,0);
	ll_sm_t* ll = ll_get_current_state_machine();
	if(POINTER_NOT_VALID(pAdv))
	{
		return UNKNOWN_ADVERTISING_IDENTIFIER;
	}
	if((!pAdv->schMap&ADV_SCH_MAP_PA))
	{
		return COMMAND_DISALLOWED;
	}
	if(pAdv->eventProperty&(LL_ADV_EVENT_PROPERTY_LEGACY_PDU|LL_ADV_EVENT_PROPERTY_CONNECTED|LL_ADV_EVENT_PROPERTY_SCANNABLE|LL_ADV_EVENT_PROPERTY_ANONYMOUS_ADV))
	{
		return COMMAND_DISALLOWED;
	}
	if(pAdv->pa->enable&&(enable&BIT(0)))
	{
		//random address change
	}
	if((pAdv->pa->enable == 0)&&(enable&BIT(0)))
	{
		pAdv->pa->enable = 1;
		if(pAdv->enable)
		{
			pAdv->pa->active = 1;
		}
		pAdv->pa->includeAdi = (enable&BIT(1)==0?0:1);
		pAdv->schMap |= ADV_SCH_MAP_PA;
		phy_obj_cast(&ll->phy);
		pAdv->pa->sync.phy.mode       = pAdv->ea->phyMode;
		pAdv->pa->sync.phy.rxMaxOctets= BLE_ADV_PRI_PHY_MAX_RX_LEN;
		pAdv->pa->sync.phy.accessCode = 0x89762349;
		pAdv->pa->sync.phy.crcInit    = 0x192874;
		pAdv->pa->sync.phy.txAddress  = ll_get_shared_phy_tx_address();;
		pAdv->pa->sync.phy.rxAddress  = ll_get_shared_phy_rx_address();


		pAdv->pa->sync.sch.startMargin= 100;
		pAdv->pa->sync.sch.stopMargin = 100;
		pAdv->pa->sync.sch.duration   = ll->phy.hw_get_prepare_time()+ll_get_air_packet_time(pAdv->pa->sync.phy.mode,2+BLE_ADV_SEC_PHY_MAX_TX_LEN,0);
		pAdv->pa->sync.sch.anchorPoint= system_time()+2000;//todo,periodic task get anchor point from planner
	}
	if((pAdv->pa->enable == 1)&&(!(enable&BIT(0))))
	{
		pAdv->pa->enable = 0;
		pAdv->pa->active = 0;
		pAdv->schMap &= (~ADV_SCH_MAP_PA);
		//disable
	}
}

#endif

