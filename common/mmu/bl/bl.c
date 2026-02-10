/*
 * txBlock.c
 *
 *  Created on: 2025年10月17日
 *      Author: Admin
 */

#include"bl.h"
#include"../ml/ml.h"

static _u8* tx_bl_alloc_node(tx_bl_t* block,_u32 serialNum,_u8 serialDuplicateDetect)
{
	tx_bl_node_t* elem = NULL;
	if(serialDuplicateDetect!=0)
	{
		elem = block->usedHdr;
		while(POINTER_VALID(elem))
		{
			if(elem->serialNum == serialNum)
			{
				return NULL;
			}
			elem = elem->next;
		}
	}
	if(block->freeCnt)
	{
		elem = block->freeHdr;
		block->freeHdr = block->freeHdr->next;
		block->freeCnt--;
		elem->serialNum = serialNum;
		elem->next = NULL;
		tx_bl_node_t* prev = NULL;
		block->usedCnt++;
		if(POINTER_NOT_VALID(block->usedHdr))
		{
			block->usedHdr = elem;
			return elem->data;
		}
		tx_bl_node_t* curNode = block->usedHdr;
		while(POINTER_VALID(curNode))
		{
			if(block->sequence == TX_BL_SERIAL_NUMBER_ASCENDING)
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
			block->usedHdr = elem;
		}
		return elem->data;
	}
	else
	{
		return NULL;
	}
}

static _u8* tx_bl_pop_node_by_serialNum(tx_bl_t* block,_u32 serialNum)
{
	tx_bl_node_t* curNode  = block->usedHdr;
	tx_bl_node_t* prevNode = NULL;
	if(POINTER_NOT_VALID(block->usedHdr))
	{
		return NULL;
	}
	while(POINTER_VALID(curNode))
	{
		if(curNode->serialNum == serialNum)
		{
			block->usedCnt--;
			if(POINTER_VALID(prevNode))
			{
				prevNode->next = curNode->next;
				return curNode->data;
			}
			else
			{
				_u8* data = curNode->data;
				block->usedHdr = curNode->next;
				return data;
			}
		}
		prevNode = curNode;
		curNode  = curNode->next;
	}
	return NULL;
}

static _u8* tx_bl_pop_node_in_order(tx_bl_t* block)
{
	if(POINTER_NOT_VALID(block->usedHdr))
	{
		return NULL;
	}
	tx_bl_node_t* node = block->usedHdr;
	block->usedHdr = block->usedHdr->next;
	block->usedCnt--;
	return node->data;
}
static void tx_bl_free_node(tx_bl_t* block,_u8* data)
{
	ASSERT(data>block->addr);
	tx_bl_node_t* node = (tx_bl_node_t*)(data - sizeof(tx_bl_node_t));
	node->next = block->freeHdr;
	block->freeHdr = node;
	block->freeCnt++;
}
#include"tx_common.h"
void tx_bl_destory(tx_bl_t* block)
{
	if(POINTER_VALID(block->addr))
	{
		tx_malloc_ret_e ret = tx_free(block->addr);
		LOG_TRACE(1,"destory",&ret,4);
		block->addr = NULL;
	}
	block->freeHdr            = NULL;
	block->usedHdr            = NULL;
	block->usedCnt            = 0;
	block->freeCnt            = 0;
	block->sequence           = 0;
}

void tx_bl_init(tx_bl_t* block,_u16 size,_u8 num)
{
	block->freeHdr = NULL;
	block->usedHdr = NULL;
	tx_bl_node_t* temp = NULL;
	_u32 nodeSize = TX_ALIGN_UP_TO_4(sizeof(tx_bl_node_t)+size);
	block->addr = tx_malloc(nodeSize*num);
	ASSERT(block->addr!=NULL);
	for(int i=0;i<num;i++)
	{

		tx_bl_node_t* node = (tx_bl_node_t*)(block->addr+i*nodeSize);
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
	block->usedCnt            = 0;
	block->freeCnt            = num;
	block->destory            = tx_bl_destory;
	block->allocNode          = tx_bl_alloc_node;
	block->popNodeInOrder     = tx_bl_pop_node_in_order;
	block->popNodeBySerialNum = tx_bl_pop_node_by_serialNum;
	block->freeNode           = tx_bl_free_node;
}

#if(0)
volatile tx_bl_t  AACC_BlockTest;
volatile _u32     AACC_ADDR;
volatile _u32     AACC_NODE_ADDR;
void tx_bl_test(_u8* data,_u8 len)
{
	  switch(data[0])
	  {
		case 0x01:
			tx_bl_init(&AACC_BlockTest,48,8);
			LOG_TRACE(1,"block list init",0,0)
			break;
		case 0x02:
			AACC_ADDR =  (_u32)AACC_BlockTest.allocNode(&AACC_BlockTest,0,0);
			LOG_TRACE(1,"block list alloc node",&AACC_ADDR,4)
			AACC_ADDR =  (_u32)AACC_BlockTest.allocNode(&AACC_BlockTest,1,0);
			LOG_TRACE(1,"block list alloc node",&AACC_ADDR,4)
			AACC_ADDR =  (_u32)AACC_BlockTest.allocNode(&AACC_BlockTest,2,0);
			LOG_TRACE(1,"block list alloc node",&AACC_ADDR,4)
			AACC_ADDR =  (_u32)AACC_BlockTest.allocNode(&AACC_BlockTest,3,0);
			LOG_TRACE(1,"block list alloc node",&AACC_ADDR,4)
			break;
		case 0x03:
			AACC_NODE_ADDR = (_u32)AACC_BlockTest.popNodeInOrder(&AACC_BlockTest);
			LOG_TRACE(1,"block list free node",&AACC_NODE_ADDR,4)
			AACC_BlockTest.freeNode(&AACC_BlockTest,AACC_NODE_ADDR);
			break;
		case 0x04:
			AACC_NODE_ADDR = (_u32)AACC_BlockTest.popNodeBySerialNum(&AACC_BlockTest,2);
			LOG_TRACE(1,"block list free node",&AACC_NODE_ADDR,4)
			AACC_BlockTest.freeNode(&AACC_BlockTest,AACC_NODE_ADDR);
			break;
		case 0x05:
			AACC_BlockTest.destory(&AACC_BlockTest);
			break;
		default:
			break;
	  }
}
#endif
