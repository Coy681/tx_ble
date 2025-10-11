
#include"channel.h"


void ll_csa_init(ll_csa_ctrl_t* chnInfo)
{
	/*
	 * ble channel selection algorithm 1,
	 * key points
	 * 1- unmapped channel iteration,unmapped channel = (lastUnmappedChannel+hop)%37
	 * 2- unmapped channel mapped to used channels.
	 *    if unmapped channel is used channels,use directly.
	 *    is unmapped channel is not used channels,remap it to used channels,channel = usedChn[unmapped channel%usedNum]
	 */
	if(chnInfo->mode == LL_CSA_1)
	{
	    _u8 usedChn[37]={0};
	    chnInfo->usedNum = 0;
	    for(int i=0;i<37;i++)
	    {
	        if(BIT_N_VALID(chnInfo->map,i))
	        {
	        	usedChn[chnInfo->usedNum++]=i;;
	        }
	    }
	    _u8 unMappedChn = 0;
	    for(int i=0;i<37;i++)
	    {
	        unMappedChn = (unMappedChn+chnInfo->hop)%37;
	        if(BIT_N_VALID(chnInfo->map,unMappedChn))
	        {
	        	chnInfo->table[i] = unMappedChn;
	        }
	        else
	        {
	        	chnInfo->table[i] = usedChn[unMappedChn%chnInfo->usedNum];
	        }
	    }
	    chnInfo->ready = 1;
	}
	#if(LL_SUPPORT_CHANNEL_SELECTION_ALGORITHM_2)
	else if(chnInfo->mode == LL_CSA_2)
	{
	    chnInfo->usedNum = 0;
	    for(int i=0;i<37;i++)
	    {
	        if(BIT_N_VALID(chnInfo->map,i))
	        {
	        	chnInfo->table[chnInfo->usedNum++]=i;;
	        }
	    }
	    chnInfo->d = TX_MAX(1,TX_MAX(TX_MIN(3,chnInfo->usedNum-5),TX_MIN(11,(chnInfo->usedNum-10)>>1)));
	    chnInfo->ready = 1;
	}
	#endif
}


#define LL_CSA2_MAM(a,b)                 ((unsigned short)(17*a+b))

#define LL_CSA2_PERMUTATION_OPERATION(x) ((BIT_REVERSE_8((x>>8)&0xff)<<8) | BIT_REVERSE_8(x&0xFF))

_u8  ll_csa_get_next_channel_index(ll_csa_ctrl_t* chnInfo)
{
	if(!chnInfo->ready)
	{
		return 0;
	}
	if(chnInfo->mode == LL_CSA_1)
	{
		return chnInfo->table[chnInfo->counter%37];
	}
	#if(LL_SUPPORT_CHANNEL_SELECTION_ALGORITHM_2)
	else if(chnInfo->mode == LL_CSA_2)
	{
		if(chnInfo->seNum == 1)
		{
			_u16 inputS = chnInfo->counter^chnInfo->chnId;
			inputS = LL_CSA2_PERMUTATION_OPERATION(inputS);
			inputS = LL_CSA2_MAM(inputS,chnInfo->chnId);
			inputS = LL_CSA2_PERMUTATION_OPERATION(inputS);
			inputS = LL_CSA2_MAM(inputS,chnInfo->chnId);
			inputS = LL_CSA2_PERMUTATION_OPERATION(inputS);
			inputS = LL_CSA2_MAM(inputS,chnInfo->chnId);
			_u16 prn_e = inputS^chnInfo->chnId;
			_u8  unMappedChn = prn_e%37;
			chnInfo->lastUsedPrn = inputS;
	        if(BIT_N_VALID(chnInfo->map,unMappedChn))
			{
	        	_u8 left  = 0;
	        	_u8 right = chnInfo->usedNum-1;
	        	_u8 mid = 0;
	        	while(left<=right)
	        	{
	        		mid = (left+right)>>1;
	        		if(chnInfo->table[mid] == unMappedChn)
	        		{
	        			chnInfo->lastSeIdx = mid;
	        			break;
	        		}
	        		else if(chnInfo->table[mid] > unMappedChn)
	        		{
	        			right = mid-1;
	        		}
	        		else
	        		{
	        			left  = mid+1;
	        		}
	        	}
	        	return unMappedChn;
			}
	        else
	        {
	        	_u8 remapIdx = (chnInfo->usedNum*prn_e)>>16;
	        	chnInfo->lastSeIdx = remapIdx;
	        	return chnInfo->table[remapIdx];
	        }
		}
		else
		{
			_u16 inputE  = LL_CSA2_PERMUTATION_OPERATION(chnInfo->lastUsedPrn);
			inputE       = LL_CSA2_MAM(inputE,chnInfo->chnId);
			_u16 prnSeLu = inputE;
			_u16 prnSeSe = inputE^chnInfo->chnId;
			chnInfo->lastUsedPrn = prnSeLu;
			_u8  seIdx = (chnInfo->lastSeIdx+chnInfo->d+(prnSeSe*(chnInfo->usedNum-2*chnInfo->d+1)>>16))%chnInfo->usedNum;
			chnInfo->lastSeIdx = seIdx;
			return chnInfo->table[seIdx];;
		}
	}
	#endif
	return 0;
}
