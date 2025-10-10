/*
 * BLE Radio frequency tolerance
 * The deviation of center frequency shall be ± 150kHz
 * The frequency drift shall be less than 50kHz
 * The frequency drift rate shall be less than 400kHz/us
 */
/**
 * Link Layer use one physical channel at a given time.
 * 
 * Channel Group 1: 3 channels as primary advertising channel 
 *                  | center frequency | channel index |
 *                  |      2402        |       37      |
 *                  |      2426        |       38      |
 *                  |      2480        |       39      |         
 * Channel Group 2: 37 channels as general purpose channel.
 *                  | center frequency | channel index |
 *                  |      2404        |       0       |
 *                  |      2406        |       1       |
 *                  |      ....        |      ....     |
 *                  |      2424        |       10      |
 *                  |      2428        |       11      |
 *                  |      2430        |       12      |
 *                  |      ....        |      ....     |
 *                  |      2478        |       36      |    
 * 
 * two groups of rf channel are used in four LE physical channels 
 *    -advertising :uses both groups,which act as primary advertising channel and secondary advertising channel
 *    -periodic    :uses general purpose channel.
 *    -isochronous :uses general purpose channel.
 *    -data        :uses general purpose channel.
 */
#include"../ll_internal.h"
#include"common/txCommon.h"
typedef enum
{
    LL_CHANNEL_UNKNOWN,
    LL_CHANNEL_BAD,
    LL_CHANNEL_GOOD,
}ll_channel_classfication_e;

void ll_csa1_calculate_channel_table(ll_conn_chn_info_t* chnInfo);

_u8  ll_csa2_calculate_channel_index(ll_conn_chn_info_t* chnInfo,_u16 counter,_u16 chnId,_u8 seNum);
