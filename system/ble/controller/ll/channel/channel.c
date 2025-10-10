
#include"channel.h"

void ll_csa1_calculate_channel_table(ll_conn_chn_info_t* info)
{
    _u8 usedChn[37]={0};
    _u8 usedNum = 0;
    for(int i=0;i<37;i++)
    {   
        if(BIT_N_VALID(info->map,i))
        {
        	usedChn[usedNum++]=i;;
        }
    }
    _u8 lastUnMappedChn = 0;
    _u8 unMappedChn = 0;
    for(int i=0;i<37;i++)
    {
        unMappedChn = (lastUnMappedChn+info->hop)%37;
        lastUnMappedChn = unMappedChn;
        if(!BIT_N_VALID(info->map,unMappedChn))
        {
            unMappedChn = usedChn[unMappedChn%usedNum];
        }
        info->table[i] = unMappedChn;
    }
}
