/*
 * txBlock.h
 *
 *  Created on: 2025年10月17日
 *      Author: Admin
 */

#ifndef TXBLOCK_H_
#define TXBLOCK_H_
#include"txType.h"



typedef enum
{
	TX_BLOCK_PEIORITY_LEVEL_0,
	TX_BLOCK_PEIORITY_LEVEL_1,
	TX_BLOCK_PEIORITY_LEVEL_2,
	TX_BLOCK_PEIORITY_LEVEL_3,
	TX_BLOCK_PEIORITY_LEVEL_4,
	TX_BLOCK_PEIORITY_LEVEL_5,
	TX_BLOCK_PEIORITY_LEVEL_6,
	TX_BLOCK_PEIORITY_LEVEL_7,
};

typedef struct
{
	_u8  priority;
	_u8  id;
	_u16 rsvd;
	_u8* next;
	_u8  data[0];
}txBlockNode_t;

#define TX_DATA_BLOCK_SIZE(size)      (sizeof(txBlockNode_t)+size+3)&(~3))

typedef struct
{
	_u8                freeCnt;
	_u8                nodeCnt;
	txBlockNode_t*    (*allocNode)(_u8);//priority
	void              (*freeNodeById)(_u8);
	void              (*freeNodeInOrder)(void);
	_u8*               addr;
	txBlockNode_t*     freeHdr;
	txBlockNode_t*     nodeHdr;
}txBlockCtrl_t;

void tx_block_init(txBlockCtrl_t* block,_u16 size,_u8 num);

#endif /* TXBLOCK_H_ */
