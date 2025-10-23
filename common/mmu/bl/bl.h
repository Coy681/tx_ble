/*
 * txBlock.h
 *
 *  Created on: 2025年10月17日
 *      Author: Admin
 */

#ifndef TXBLOCK_H_
#define TXBLOCK_H_
#include"common/txType.h"
#include"../ml/ml.h"

typedef enum
{
	TX_BLOCK_SERIAL_NUMBER_ASCENDING  = 0,
	TX_BLOCK_SERIAL_NUMBER_DESCENDING = 1,
}tx_block_sequence_e;

typedef struct tx_block_node_t tx_block_node_t;
typedef struct tx_block_node_t
{
	_u32             serialNum;
	tx_block_node_t* next;
	_u8              data[0];
}tx_block_node_t;

#define TX_DATA_BLOCK_SIZE(size)      ((sizeof(tx_block_node_t)+size+3)&(~3))
typedef struct tx_block_ctrl_t tx_block_ctrl_t;
typedef struct tx_block_ctrl_t
{
	_u8                nodeCnt;
	_u8                freeCnt;
	_u8                sequence;
	_u8                rsvd;
	_u8*               (*allocAndInsertNode)(tx_block_ctrl_t*,_u32);
	_u8*               (*popNodeBySerialNum)(tx_block_ctrl_t*,_u32);
	_u8*               (*popNodeInOrder)(tx_block_ctrl_t*);
	void               (*freeNode)(tx_block_ctrl_t*,_u8*);
	void               (*destory)(tx_block_ctrl_t*);
	_u8*               addr;
	tx_block_node_t*   nodeHdr;
	tx_block_node_t*   freeHdr;
}tx_block_ctrl_t;

void tx_block_init(tx_block_ctrl_t* block,_u16 size,_u8 num);

#endif /* TXBLOCK_H_ */
