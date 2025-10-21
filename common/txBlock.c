/*
 * txBlock.c
 *
 *  Created on: 2025年10月17日
 *      Author: Admin
 */

#include"txBlock.h"


static txBlockNode_t* tx_block_alloc_node(_u8 priority)
{
	return NULL;
}

static void tx_block_free_node_in_order(void)
{

}

static void tx_block_free_node_by_id(_u8 priority)
{

}


void tx_block_init(tx_block_ctrl_t* block,_u16 size,_u8 num)
{
	block->freeHdr = NULL;
	block->nodeHdr = NULL;
	txBlockNode_t* temp = NULL;
	for(int i=0;i<num;i++)
	{
		txBlockNode_t* node = (txBlockNode_t*)(block->addr+i*size);
		node->next = NULL;
		if(i==0)
		{
			block->nodeHdr = node;
		}
		else
		{
			temp->next = node;
		}
		temp = node;
	}
	block->nodeCnt         = num;
	block->freeCnt         = 0;
	block->allocNode       = tx_block_alloc_node;
	block->freeNodeById    = tx_block_free_node_by_id;
	block->freeNodeInOrder = tx_block_free_node_in_order;
}
