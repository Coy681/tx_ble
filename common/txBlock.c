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
			if(block->sequence == TX_BLOCK_SERIAL_NUMBER_ASCENDING)
			{
				if(curNode->serialNum>=elem->serialNum)
				{
					break;
				}
			}
			else
			{
				if(curNode->serialNum<=elem->serialNum)
				{
					break;
				}
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

static _u8* tx_block_pop_node_by_serialNum(tx_block_ctrl_t* block,_u32 serialNum)
{
	tx_block_node_t* curNode  = block->nodeHdr;
	tx_block_node_t* prevNode = NULL;
	if(POINTER_NOT_VALID(block->nodeHdr))
	{
		return NULL;
	}
	while(POINTER_VALID(curNode))
	{
		if(curNode->serialNum == serialNum)
		{
			block->nodeCnt--;
			if(POINTER_VALID(prevNode))
			{
				prevNode->next = curNode->next;
				return curNode->data;
			}
			else
			{
				_u8* data = curNode->data;
				block->nodeHdr = curNode->next;
				return data;
			}
		}
		prevNode = curNode;
		curNode  = curNode->next;
	}
	return NULL;
}

static _u8* tx_block_pop_node_in_order(tx_block_ctrl_t* block)
{
	if(POINTER_NOT_VALID(block->nodeHdr))
	{
		return NULL;
	}
	tx_block_node_t* node = block->nodeHdr;
	block->nodeHdr = block->nodeHdr->next;
	block->nodeCnt--;
	return node->data;
}


static void tx_block_free_node(tx_block_ctrl_t* block,_u8* data)
{
	ASSERT(data>block->addr);
	tx_block_node_t* node = (tx_block_node_t*)(data - sizeof(tx_block_node_t));
	node->next = block->freeHdr;
	block->freeHdr = node;
	block->freeCnt++;
}

void tx_block_destory(tx_block_ctrl_t* block)
{
	if(POINTER_VALID(block->addr))
	{
		tx_free(block->addr);
		block->addr = NULL;
	}
	block->freeHdr = NULL;
	block->nodeHdr = NULL;
	block->nodeCnt = 0;
	block->freeCnt = 0;
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
			block->freeHdr = node;
		}
		else
		{
			temp->next = node;
		}
		temp = node;
	}
	block->nodeCnt            = 0;
	block->freeCnt            = num;
	block->destory            = tx_block_destory;
	block->allocAndInsertNode = tx_block_alloc_and_insert_node;
	block->popNodeInOrder     = tx_block_pop_node_in_order;
	block->popNodeBySerialNum = tx_block_pop_node_by_serialNum;
	block->freeNode           = tx_block_free_node;
}
