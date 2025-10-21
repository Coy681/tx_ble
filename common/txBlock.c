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
	tx_block_node_t* temp = NULL;
	block->addr = tx_malloc(TX_DATA_BLOCK_SIZE(size)*num);
	for(int i=0;i<num;i++)
	{
		tx_block_node_t* node = (tx_block_node_t*)(block->addr+i*TX_DATA_BLOCK_SIZE(size));
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
}
