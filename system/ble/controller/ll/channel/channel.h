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
#include"common/txCommon.h"
#include"../ll_feature.h"
#ifndef LL_CHANNEL_H_
#define LL_CHANNEL_H_
typedef enum
{
    LL_CHANNEL_UNKNOWN,
    LL_CHANNEL_BAD,
    LL_CHANNEL_GOOD,
}ll_channel_classfication_e;

typedef enum
{
	LL_CSA_1 = 0x00,
	LL_CSA_2 = 0x01,
}ll_csa_mode_e;

typedef struct _PACKED
{
	_u8  mode:3;     //input,'ll_csa_mode_e'
	_u8  hop:5;      //input,from 5 to 16，only used for csa1;
    _u8  usedNum:6;  //output,used channel number,calculate from map[5]
    _u8  ready:1;    //output,if csa inited.
    _u8  rsvd:1;
    _u16 counter;    /* input
    				    csa1:it is connectionEventCounter
    				    csa2:
    						  acl connections - it is connectionEventCounter
                        	  pa              - it is paEventCounter
                        	  pawr            - it is pawrEventCounter^pawrSubEventCounter
							  iso links       - it is bit0-bit15 of bigEventCounter or cigEventCounter
                     */
    _u8  map[5];     //input,channel map
    _u8  table[37];  /* output
    				    csa1:it is calculated periodic channel index table,use directly in order.
    				    csa2:it is used channels in ascending order
    				  */
	#if(LL_SUPPORT_CHANNEL_SELECTION_ALGORITHM_2)
    _u8  seNum;      //input,subevent number,
    _u8  d;          //one of csa2 parameter.
    _u8  lastSeIdx;  //dynamic
    _u16 lastUsedPrn;//dynamic
    _u16 chnId;      //channel identifier,AccessAddress31-16 ^ AccessAddress15-0

	#endif
}ll_csa_ctrl_t;

void ll_csa_init(ll_csa_ctrl_t* chnInfo);

_u8  ll_csa_get_next_channel_index(ll_csa_ctrl_t* chnInfo);


#endif/*LL_CHANNEL_H_*/
