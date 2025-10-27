/*
 * txBlock.h
 *
 *  Created on: 2025年10月17日
 *      Author: Admin
 */

#ifndef TXBLOCK_H_
#define TXBLOCK_H_
#include"common/txType.h"
typedef enum
{
	TX_BL_SERIAL_NUMBER_ASCENDING  = 0,
	TX_BL_SERIAL_NUMBER_DESCENDING = 1,
}tx_bl_sequence_e;

typedef struct tx_bl_t tx_bl_t;
typedef struct tx_bl_node_t tx_bl_node_t;

typedef struct tx_bl_node_t
{
	_u32             serialNum;
	tx_bl_node_t*    next;
	_u8              data[0];
}tx_bl_node_t;

typedef struct tx_bl_t
{
	_u8*            addr;
	_u8             nodeCnt;
	_u8             freeCnt;
	_u8             sequence;
	_u8             rsvd;
	tx_bl_node_t*   nodeHdr;
	tx_bl_node_t*   freeHdr;
	_u8*            (*allocNode)(tx_bl_t*,_u32);
	_u8*            (*popNodeBySerialNum)(tx_bl_t*,_u32);
	_u8*            (*popNodeInOrder)(tx_bl_t*);
	void            (*freeNode)(tx_bl_t*,_u8*);
	void            (*destory)(tx_bl_t*);

}tx_bl_t;

void tx_bl_init(tx_bl_t* block,_u16 size,_u8 num);


#endif /* TXBLOCK_H_ */
