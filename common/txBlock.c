/*
 * txBlock.c
 *
 *  Created on: 2025年10月17日
 *      Author: Admin
 */

#include"txBlock.h"


static _u8* tx_block_alloc_and_insert_node(tx_block_ctrl_t* block,_u32 serialNum)
{
	if(block->freeCnt)
	{
		tx_block_node_t* elem = block->freeHdr;
		block->freeHdr = block->freeHdr->next;
		block->freeCnt--;
		elem->serialNum = serialNum;
		elem->next = NULL;
		tx_block_node_t* prev = NULL;
		if(POINTER_NOT_VALID(block->nodeHdr))
		{
			block->nodeHdr = elem;
			return elem->data;
		}
		tx_block_node_t* curNode = block->nodeHdr;
		while(POINTER_VALID(curNode))
		{
			if(curNode->serialNum>elem->serialNum)
			{
				break;
			}
			prev    = curNode;
			curNode = curNode->next;
		}
		if(POINTER_VALID(prev))
		{
			prev->next   = elem;
			elem->next = curNode;
		}
		else
		{
			elem->next   = curNode;
			block->nodeHdr = elem;
		}
		block->nodeCnt++;
		return elem->data;
	}
	else
	{
		return NULL;
	}
}

static _u8* tx_block_pop_node_in_serialNum(tx_block_ctrl_t* block,_u32 serialNum)
{
	tx_block_node_t* curNode  = block->nodeHdr;
	tx_block_node_t* prevNode = NULL;
	while(POINTER_VALID(curNode))
	{
		if(curNode->serialNum == serialNum)
		{
			break;
		}
		prevNode = curNode;
		curNode  = curNode->next;
	}
	if(POINTER_VALID(prevNode))
	{

	}
	else
	{

	}
}

static _u8* tx_block_pop_node_in_order(tx_block_ctrl_t* block)
{

}



void tx_block_destory(tx_block_ctrl_t* block)
{
	if(POINTER_VALID(block->addr))
	{
		tx_free(block->addr);
	}
	block->freeHdr = NULL;
	block->nodeHdr = NULL;
	block->nodeCnt = NULL;
	block->freeCnt = NULL;
	block->sequence = 0;
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
	block->nodeCnt            = num;
	block->freeCnt            = 0;
	block->destory            = tx_block_destory;
	block->allocAndInsertNode = tx_block_alloc_and_insert_node;
	block->popNodeInOrder     = tx_block_pop_node_in_order;
	block->popNodeInSerialNum = tx_block_pop_node_in_serialNum;
}
