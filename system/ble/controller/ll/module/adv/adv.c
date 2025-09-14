
#include"adv.h"
#include"system/scheduler/sch_map.h"
/*****************************************ADV State Machine***********************************************/
typedef enum
{
    ADV_SM_SCH_EVENT_BASE              = 0x10,
    ADV_SM_SCH_EVENT_START             = ADV_SM_SCH_EVENT_BASE+SCH_TASK_START,
    ADV_SM_SCH_EVENT_STOP              = ADV_SM_SCH_EVENT_BASE+SCH_TASK_STOP,             
    ADV_SM_SCH_EVENT_CANCELED          = ADV_SM_SCH_EVENT_BASE+SCH_TASK_CANCELED,
    ADV_SM_SCH_EVENT_PASSED            = ADV_SM_SCH_EVENT_BASE+SCH_TASK_PASSED,

    ADV_SM_PHY_EVENT_BASE              = 0x20,
    ADV_SM_PHY_EVENT_SEND_FINISHED     = ADV_SM_PHY_EVENT_BASE+PHY_IRQ_TX_FINISHED,
    ADV_SM_PHY_EVENT_RECEIVE_FINISHED  = ADV_SM_PHY_EVENT_BASE+PHY_IRQ_RX_FINISHED,
    ADV_SM_PHY_EVENT_RECEIVE_TIMEOUT   = ADV_SM_PHY_EVENT_BASE+PHY_IRQ_RX_TIMEOUT,
}adv_sm_event_e;

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
    SM_SCH_EVENT = BIT(1),
    SM_PHY_EVENT = BIT(2),
}sm_event_type_e;

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
    adv_sm_event_e  event;
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
    _u8* packet = ll_get_adv_packet(advParam->la->phy.txAddress,6+advParam->data.len,LL_ADV_TYPE_ADV_IND,0,advParam->ownAddressType?1:0,0);
    txMemcpy(((adv_type_ind_t*)packet)->advA,ll_get_device_address(),6);
    txMemcpy(((adv_type_ind_t*)packet)->advData,advParam->data.addr,advParam->data.len);
}
//LL_ADV_TYPE_ADV_DIRECT_IND
static void adv_direct_ind_pdu_prepare(ll_sm_t* ll,ll_internal_adv_param_t* advParam)
{
    _u8* packet = ll_get_adv_packet(advParam->la->phy.txAddress,12,LL_ADV_TYPE_ADV_DIRECT_IND,0,advParam->ownAddressType?1:0,advParam->peerAddressType?1:0);
    txMemcpy(((adv_type_direct_ind_t*)packet)->advA,ll_get_device_address(),6);
    txMemcpy(((adv_type_direct_ind_t*)packet)->targetA,advParam->peerAddress,6);
}
//LL_ADV_TYPE_ADV_NONCONN_IND
static void adv_non_conn_pdu_prepare(ll_sm_t* ll,ll_internal_adv_param_t* advParam)
{
    _u8* packet = ll_get_adv_packet(advParam->la->phy.txAddress,6+advParam->data.len,LL_ADV_TYPE_ADV_NONCONN_IND,0,advParam->ownAddressType?1:0,0);
    txMemcpy(((adv_type_nonConn_ind_t*)packet)->advA,ll_get_device_address(),6);
    txMemcpy(((adv_type_nonConn_ind_t*)packet)->advData,advParam->data.addr,advParam->data.len);
}
//LL_ADV_TYPE_SCAN_RSP
static void adv_scan_rsp_pdu_prepare(ll_sm_t* ll,ll_internal_adv_param_t* advParam)
{
    _u8* packet = ll_get_adv_packet(advParam->la->phy.txAddress,6+advParam->scanRsp.len,LL_ADV_TYPE_SCAN_RSP,0,advParam->ownAddressType?1:0,0);
    txMemcpy(((adv_type_scan_rsp_t*)packet)->advA,ll_get_device_address(),6);
    txMemcpy(((adv_type_scan_rsp_t*)packet)->scanRsp,advParam->scanRsp.addr,advParam->scanRsp.len);
}
//LL_ADV_TYPE_ADV_SCAN_IND
static void adv_scan_ind_pdu_prepare(ll_sm_t* ll,ll_internal_adv_param_t* advParam)
{
    _u8* packet = ll_get_adv_packet(advParam->la->phy.txAddress,6+advParam->data.len,LL_ADV_TYPE_ADV_SCAN_IND,0,advParam->ownAddressType?1:0,0);
    txMemcpy(((adv_type_scan_ind_t*)packet)->advA,ll_get_device_address(),6);
    txMemcpy(((adv_type_scan_ind_t*)packet)->advData,advParam->data.addr,advParam->data.len);
}
#if(LL_SUPPORT_LE_EXTENDED_ADVERTISING)
//LL_ADV_TYPE_ADV_EXT_IND
static void adv_ext_ind_pdu_prepare(ll_sm_t* ll,ll_internal_adv_param_t* advParam,_u8 advMode,_u8 flags,adv_extended_header_auxInfo_t* auxInfo,_u16 did)
{
    _u16 headerLen = adv_calculate_extended_header_length(flags);
    _u8* packet = ll_get_adv_packet(advParam->la->phy.txAddress,headerLen,LL_ADV_TYPE_ADV_EXT_IND,0,advParam->ownAddressType?1:0,0);
    adv_generate_extended_header(ll,advParam,packet,advMode,flags,auxInfo,did);
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
    _u8* packet = ll_get_adv_packet(advParam->ea->aux.phy.txAddress,headerLen+dataLen,LL_ADV_TYPE_AUX_ADV_IND,0,advParam->ownAddressType?1:0,0);
    adv_generate_extended_header(ll,advParam,packet,advMode,flags,auxInfo,did);
    if(dataLen!=0)
    {
        txMemcpy(packet+headerLen,advParam->ea->aux.data.addr,dataLen);
    }
}
//LL_ADV_TYPE_AUX_SCAN_RSP
static void adv_aux_scan_rsp_pdu_prepare(ll_sm_t* ll,ll_internal_adv_param_t* advParam,_u8 advMode,_u8 flags,adv_extended_header_auxInfo_t* auxInfo,_u16 did)
{
    _u16 headerLen = adv_calculate_extended_header_length(flags);
    _u8* packet = ll_get_adv_packet(advParam->ea->aux.phy.txAddress,headerLen+advParam->ea->aux.data.len,LL_ADV_TYPE_AUX_SCAN_RSP,0,advParam->ownAddressType?1:0,0);
    adv_generate_extended_header(ll,advParam,packet,advMode,flags,auxInfo,did);
    if(advParam->ea->aux.data.len!=0)
    {
        txMemcpy(packet+headerLen,advParam->ea->aux.data.addr,advParam->ea->aux.data.len);
    }
}
//LL_ADV_TYPE_AUX_CONNECT_RSP
static void adv_aux_conn_rsp_pdu_prepare(ll_sm_t* ll,ll_internal_adv_param_t* advParam,_u8 advMode,_u8 flags,adv_extended_header_auxInfo_t* auxInfo,_u16 did)
{
    _u16 headerLen = adv_calculate_extended_header_length(flags);
    _u8* packet = ll_get_adv_packet(advParam->ea->aux.phy.txAddress,headerLen,LL_ADV_TYPE_AUX_CONNECT_RSP,0,advParam->ownAddressType?1:0,0);
    adv_generate_extended_header(ll,advParam,packet,advMode,flags,auxInfo,did);
}
//LL_ADV_TYPE_AUX_CHAIN_IND
static void adv_aux_chain_ind_pdu_prepare(ll_sm_t* ll,ll_internal_adv_param_t* advParam,_u8 advMode,_u8 flags,adv_extended_header_auxInfo_t* auxInfo,_u16 did)
{
    _u16 headerLen = adv_calculate_extended_header_length(flags);
    _u8* packet = ll_get_adv_packet(advParam->pChain->entry[advParam->pChain->current].phy.txAddress,headerLen+advParam->ea->chain.entry[advParam->pChain->current].data.len,LL_ADV_TYPE_AUX_CHAIN_IND,0,advParam->ownAddressType?1:0,0);
    adv_generate_extended_header(ll,advParam,packet,advMode,flags,auxInfo,did);
    if(advParam->pChain->entry[advParam->pChain->current].data.len!=0)
    {
        txMemcpy(packet+headerLen,advParam->pChain->entry[advParam->pChain->current].data.addr,advParam->pChain->entry[advParam->pChain->current].data.len);
    }
}
#endif
#if(LL_SUPPORT_LE_PERIODIC_ADVERTISING)
//LL_ADV_TYPE_AUX_SYNC_IND
static void adv_aux_sync_ind_pdu_prepare(ll_sm_t* ll,ll_internal_adv_param_t* advParam,_u8 advMode,_u8 flags,adv_extended_header_auxInfo_t* auxInfo,_u16 did)
{
    _u16 headerLen = adv_calculate_extended_header_length(flags);
    _u8* packet = ll_get_adv_packet(advParam->pa->sync.phy.txAddress,headerLen+advParam->pa->sync.data.len,LL_ADV_TYPE_AUX_SYNC_IND,0,advParam->ownAddressType?1:0,0);
    adv_generate_extended_header(ll,advParam,packet,advMode,flags,auxInfo,did);
    if(advParam->pa->sync.data.len!=0)
    {
        txMemcpy(packet+headerLen,advParam->pa->sync.data.addr,advParam->pa->sync.data.len);
    }
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
static int adv_event_step_received_packet_analyze(ll_sm_t* ll)
{
	 ll_adv_packet_t* packet = (ll_adv_packet_t*)(ll->phy.rxAddress + ll_get_packet_header_offset_from_address(PHY_DIR_RX));


	 if(packet->hdr.pduType == LL_ADV_TYPE_SCAN_REQ && packet->hdr.length == sizeof(scan_type_scan_req_t))
	 {
		 //scan req process
		 scan_type_scan_req_t* scanReq = (scan_type_scan_req_t*)(packet->data);
		 if(txMemcmp(ll_get_device_address(),scanReq->advA,6) == 0)
		 {
			 return 1;
		 }
	 }
	 else if(packet->hdr.pduType == LL_ADV_TYPE_CONNECT_IND && packet->hdr.length == sizeof(init_type_connectInd_t))
	 {
		 //connect ind process
		 init_type_connectInd_t* connInd = (init_type_connectInd_t*)(packet->data);
		 //ll state machine transform
	 }
	return 0;
}

_RAM_CODE
static int adv_event_step_phy_send_scan_rsp(ll_sm_t* ll,ll_internal_adv_param_t* advParam)
{
	if(ll->phy.hw_is_rx_packet_valid()&&(adv_event_step_received_packet_analyze(ll)!=0))//packet analyze shall be execute first
	{
        adv_prepare_packet[advParam->eventType](ll,advParam,ADV_PDU_CLASS_SCAN_RSP);
        _u32 timestamp = ll->phy.hw_get_rx_air_ts() + ll_get_air_packet_time(advParam->la->phy.mode,sizeof(scan_type_scan_req_t),0)+PACKET_DEFAULT_TIFS_TIME;
        adv_prepare_phy(ll,&advParam->la->phy,timestamp,PHY_DIR_TX);
        ll->phy.start();
        return 1;
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

_RAM_CODE
static int adv_event_step_phy_send_rsp_finished(ll_sm_t* ll,ll_internal_adv_param_t* advParam)
{
    return 1;
}

_DATA
static adv_event_sm_t adv_event_state_machine[]=
{
    {adv_event_step_phy_send_advertising, ADV_CONTEXT_DEFAULT|ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE,ADV_SM_STATE_IDLE,       ADV_SM_STATE_SENDING_ADV, ADV_SM_STATE_IDLE, ADV_SM_SCH_EVENT_START},
    {adv_event_step_sch_stop,             ADV_CONTEXT_DEFAULT|ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE,ADV_SM_STATE_IDLE,       ADV_SM_STATE_IDLE,        ADV_SM_STATE_IDLE, ADV_SM_SCH_EVENT_STOP},
    {adv_event_step_sch_canceled,         ADV_CONTEXT_DEFAULT|ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE,ADV_SM_STATE_IDLE,       ADV_SM_STATE_IDLE,        ADV_SM_STATE_IDLE, ADV_SM_SCH_EVENT_CANCELED},
    {adv_event_step_sch_passed,           ADV_CONTEXT_DEFAULT|ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE,ADV_SM_STATE_IDLE,       ADV_SM_STATE_IDLE,        ADV_SM_STATE_IDLE, ADV_SM_SCH_EVENT_PASSED},

    {adv_event_step_phy_start_listen,     ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE,                    ADV_SM_STATE_SENDING_ADV,ADV_SM_STATE_RECEIVING,   ADV_SM_STATE_IDLE, ADV_SM_PHY_EVENT_SEND_FINISHED},
    {adv_event_step_sch_stop,             ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE,                    ADV_SM_STATE_SENDING_ADV,ADV_SM_STATE_IDLE,        ADV_SM_STATE_IDLE, ADV_SM_SCH_EVENT_STOP},
    {adv_event_step_default_process,      ADV_CONTEXT_DEFAULT,                                              ADV_SM_STATE_SENDING_ADV,ADV_SM_STATE_IDLE,        ADV_SM_STATE_IDLE, ADV_SM_PHY_EVENT_SEND_FINISHED},

    {adv_event_step_phy_send_scan_rsp,    ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE,                    ADV_SM_STATE_RECEIVING,  ADV_SM_STATE_SENDING_RSP, ADV_SM_STATE_IDLE, ADV_SM_PHY_EVENT_RECEIVE_FINISHED},
    {adv_event_step_sch_stop,             ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE,                    ADV_SM_STATE_RECEIVING,  ADV_SM_STATE_IDLE,        ADV_SM_STATE_IDLE, ADV_SM_SCH_EVENT_STOP},

    {adv_event_step_phy_send_rsp_finished,ADV_CONTEXT_SCANNABLE,                                            ADV_SM_STATE_SENDING_RSP,ADV_SM_STATE_IDLE,        ADV_SM_STATE_IDLE, ADV_SM_PHY_EVENT_SEND_FINISHED},
    {adv_event_step_sch_stop,             ADV_CONTEXT_SCANNABLE,                                            ADV_SM_STATE_SENDING_RSP,ADV_SM_STATE_IDLE,        ADV_SM_STATE_IDLE, ADV_SM_SCH_EVENT_STOP},
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
            //scan req process
			init_type_connectInd_t* connInd = (init_type_connectInd_t*)(packet->data);
            if(txMemcmp(ll_get_device_address(),connInd->advA,6) == 0)
            {
                init_type_ll_data_t* llData = (init_type_ll_data_t*)connInd->llData;
                if(POINTER_NOT_VALID(ll->conn))
                {
                    ll->conn = (ll_internal_connection_ctrl_t*)tx_malloc(sizeof(ll_internal_connection_ctrl_t));
                    
                }
            }
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
    {adv_extended_event_step_phy_send_aux_advertising,  ADV_CONTEXT_AUXILIARY|ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE,ADV_SM_STATE_IDLE,                  ADV_SM_STATE_SENDING_AUX_ADV,        ADV_SM_STATE_IDLE, ADV_SM_SCH_EVENT_START},
    {adv_extended_event_step_sch_stop,                  ADV_CONTEXT_AUXILIARY|ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE,ADV_SM_STATE_IDLE,                  ADV_SM_STATE_IDLE,                   ADV_SM_STATE_IDLE, ADV_SM_SCH_EVENT_STOP},
    {adv_extended_event_step_sch_canceled,              ADV_CONTEXT_AUXILIARY|ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE,ADV_SM_STATE_IDLE,                  ADV_SM_STATE_IDLE,                   ADV_SM_STATE_IDLE, ADV_SM_SCH_EVENT_CANCELED},
    {adv_extended_event_step_sch_passed,                ADV_CONTEXT_AUXILIARY|ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE,ADV_SM_STATE_IDLE,                  ADV_SM_STATE_IDLE,                   ADV_SM_STATE_IDLE, ADV_SM_SCH_EVENT_PASSED},

    {adv_extended_event_step_phy_start_listen_aux,      ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE,                      ADV_SM_STATE_SENDING_AUX_ADV,       ADV_SM_STATE_RECEIVING_AUX,          ADV_SM_STATE_IDLE, ADV_SM_PHY_EVENT_SEND_FINISHED},
    {adv_extended_event_step_sch_stop,                  ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE,                      ADV_SM_STATE_SENDING_AUX_ADV,       ADV_SM_STATE_IDLE,                   ADV_SM_STATE_IDLE, ADV_SM_SCH_EVENT_STOP},
    {adv_extended_event_step_default_process,           ADV_CONTEXT_AUXILIARY,                                              ADV_SM_STATE_SENDING_AUX_ADV,       ADV_SM_STATE_IDLE,                   ADV_SM_STATE_IDLE, ADV_SM_PHY_EVENT_SEND_FINISHED},
    {adv_extended_event_step_default_process,           ADV_CONTEXT_AUXILIARY,                                              ADV_SM_STATE_SENDING_AUX_ADV,       ADV_SM_STATE_IDLE,                   ADV_SM_STATE_IDLE, ADV_SM_PHY_EVENT_SEND_FINISHED},

    {adv_extended_event_step_phy_send_aux_scan_rsp,     ADV_CONTEXT_CONNECTABLE,                                            ADV_SM_STATE_RECEIVING_AUX,         ADV_SM_STATE_SENDING_AUX_SCAN_RSP,   ADV_SM_STATE_IDLE, ADV_SM_PHY_EVENT_RECEIVE_FINISHED},
    {adv_extended_event_step_phy_send_aux_connect_rsp,  ADV_CONTEXT_SCANNABLE,                                              ADV_SM_STATE_RECEIVING_AUX,         ADV_SM_STATE_SENDING_AUX_CONNECT_RSP,ADV_SM_STATE_IDLE, ADV_SM_PHY_EVENT_RECEIVE_FINISHED},
    {adv_extended_event_step_sch_stop,                  ADV_CONTEXT_SCANNABLE|ADV_CONTEXT_CONNECTABLE,                      ADV_SM_STATE_RECEIVING_AUX,         ADV_SM_STATE_IDLE,                   ADV_SM_STATE_IDLE, ADV_SM_SCH_EVENT_STOP},
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
	{adv_chained_event_step_phy_send_chain_advertising,ADV_CONTEXT_CHAINED,ADV_SM_STATE_IDLE,             ADV_SM_STATE_SENDING_CHAIN_ADV,ADV_SM_STATE_IDLE,ADV_SM_SCH_EVENT_START},
	{adv_chained_event_step_sch_stop,                  ADV_CONTEXT_CHAINED,ADV_SM_STATE_IDLE,             ADV_SM_STATE_IDLE,             ADV_SM_STATE_IDLE,ADV_SM_SCH_EVENT_STOP},
	{adv_chained_event_step_sch_canceled,              ADV_CONTEXT_CHAINED,ADV_SM_STATE_IDLE,             ADV_SM_STATE_IDLE,             ADV_SM_STATE_IDLE,ADV_SM_SCH_EVENT_CANCELED},
	{adv_chained_event_step_sch_passed,                ADV_CONTEXT_CHAINED,ADV_SM_STATE_IDLE,             ADV_SM_STATE_IDLE,             ADV_SM_STATE_IDLE,ADV_SM_SCH_EVENT_PASSED},

	{adv_chained_event_step_default_process,           ADV_CONTEXT_CHAINED,ADV_SM_STATE_SENDING_CHAIN_ADV,ADV_SM_STATE_IDLE,             ADV_SM_STATE_IDLE,ADV_SM_PHY_EVENT_SEND_FINISHED},
	{adv_chained_event_step_sch_stop,                  ADV_CONTEXT_CHAINED,ADV_SM_STATE_SENDING_CHAIN_ADV,ADV_SM_STATE_IDLE,             ADV_SM_STATE_IDLE,ADV_SM_SCH_EVENT_STOP},
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
    {adv_periodic_event_step_phy_send_sync_advertising,ADV_CONTEXT_DEFAULT,ADV_SM_STATE_IDLE,                ADV_SM_STATE_SENDING_PERIODIC_ADV,ADV_SM_STATE_IDLE,ADV_SM_SCH_EVENT_START},
    {adv_periodic_event_step_sch_stop,                 ADV_CONTEXT_DEFAULT,ADV_SM_STATE_IDLE,                ADV_SM_STATE_IDLE,                ADV_SM_STATE_IDLE,ADV_SM_SCH_EVENT_STOP},
    {adv_periodic_event_step_sch_passed,               ADV_CONTEXT_DEFAULT,ADV_SM_STATE_IDLE,                ADV_SM_STATE_IDLE,                ADV_SM_STATE_IDLE,ADV_SM_SCH_EVENT_PASSED},
    {adv_periodic_event_step_sch_canceled,             ADV_CONTEXT_DEFAULT,ADV_SM_STATE_IDLE,                ADV_SM_STATE_IDLE,                ADV_SM_STATE_IDLE,ADV_SM_SCH_EVENT_CANCELED},

    {adv_periodic_event_step_sch_stop,                 ADV_CONTEXT_DEFAULT,ADV_SM_STATE_SENDING_PERIODIC_ADV,ADV_SM_STATE_IDLE,                ADV_SM_STATE_IDLE,ADV_SM_PHY_EVENT_SEND_FINISHED},
    {adv_periodic_event_step_sch_stop,                 ADV_CONTEXT_DEFAULT,ADV_SM_STATE_SENDING_PERIODIC_ADV,ADV_SM_STATE_IDLE,                ADV_SM_STATE_IDLE,ADV_SM_SCH_EVENT_STOP},
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
    if(type == SM_SCH_EVENT)
    {
        smEventType = ADV_SM_SCH_EVENT_BASE+event;
    }
    else
    {
    	smEventType = ADV_SM_PHY_EVENT_BASE+event;
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
                if(type == SM_SCH_EVENT&&currentAdvSet->state == ADV_SM_STATE_IDLE)
                {
                    adv_get_next_event(ll);
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
    adv_sequence_process(SM_PHY_EVENT,type);
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
    adv_sequence_process(SM_SCH_EVENT,type);
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
