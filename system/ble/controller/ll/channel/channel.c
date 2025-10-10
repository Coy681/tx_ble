
#include"channel.h"
/*
 * ble channel selection algorithm 1,
 * key points
 * 1- unmapped channel iteration,unmapped channel = (lastUnmappedChannel+hop)%37
 * 2- unmapped channel mapped to used channels.
 *    if unmapped channel is used channels,use directly.
 *    is unmapped channel is not used channels,remap it to used channels,channel = usedChn[unmapped channel%usedNum]
 */
void ll_csa1_calculate_channel_table(ll_conn_chn_info_t* chnInfo)
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
}

#define LL_CSA2_MAM(a,b)                 ((unsigned short)(17*a+b))

#define LL_CSA2_PERMUTATION_OPERATION(x) ((BIT_REVERSE_8((x>>8)&0xff)<<8) | BIT_REVERSE_8(x&0xFF))

_u8 ll_csa2_calculate_channel_index(ll_conn_chn_info_t* chnInfo,_u16 counter,_u16 chnId,_u8 seNum)
{
	if(seNum == 1)
	{
		_u16 inputS = counter^chnId;
		inputS = LL_CSA2_PERMUTATION_OPERATION(inputS);
		inputS = LL_CSA2_MAM(inputS,chnId);
		inputS = LL_CSA2_PERMUTATION_OPERATION(inputS);
		inputS = LL_CSA2_MAM(inputS,chnId);
		inputS = LL_CSA2_PERMUTATION_OPERATION(inputS);
		inputS = LL_CSA2_MAM(inputS,chnId);
		_u16 prn_e = inputS^chnId;
		_u8  unMappedChn = prn_e%37;
		chnInfo->lastUsedPrn = inputS;
        if(BIT_N_VALID(chnInfo->map,unMappedChn))
		{
        	chnInfo->lastSeIdx = unMappedChn;
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
		inputE       = LL_CSA2_MAM(inputE,chnId);
		_u16 prnSeLu = inputE;
		_u16 prnSeSe = inputE^chnId;
		chnInfo->lastUsedPrn = prnSeLu;
		_u8  seIdx = (chnInfo->lastSeIdx+chnInfo->d+(prnSeSe*(chnInfo->usedNum-2*chnInfo->d+1)>>16))%chnInfo->usedNum;
		chnInfo->lastSeIdx = prnSeLu;
		return chnInfo->table[seIdx];;
	}
}
