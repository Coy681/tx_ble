/*
 * txMalloc.c
 *
 *  Created on: 2024年12月10日
 *      Author: 12407
 */
#include"common/txType.h"
#include"common/txGeneral.h"
#include"txMalloc.h"
_u8 txMallocBuffer[TX_MALLOC_BUFFER_SIZE];

typedef struct
{
    _u32 reserved;
    _u32 totalSize;
    _u32 unusedSize;
    _u8* pStart;
}txMalloc_t;
txMalloc_t txMalloc;
typedef struct
{
    _u8* previous;
    _u8* next;
    _u16 size;
    _u16 usedFlag;
}txMallocNode_t;

void tx_malloc_init(_u8* buffer,_u32 size)
{
    txMalloc.totalSize =  size;
    txMalloc.unusedSize = size-12;
    txMalloc.pStart = buffer;
    txMallocNode_t *pNode = (txMallocNode_t*)txMalloc.pStart;
    pNode->previous = NULL;
    pNode->usedFlag = 0;
    pNode->size = size-12;
    pNode->next = ((_u8*)&pNode->previous + 2);
}

_u8* tx_malloc(_u16 length)
{
    _u16 lenMalloc = ((length+3)&(~3));
    txMallocNode_t *pNode = (txMallocNode_t*)txMalloc.pStart;
    txMallocNode_t *pPreviousNode = pNode;
    pNode = pNode->next;
    while(pNode != NULL)
    {
        if(!pNode->usedFlag&&pNode->size>lenMalloc)
        {
            break;
        }
        pNode = pNode->next;
        pPreviousNode = pPreviousNode->next;
    }
    if(pNode == NULL)
    {
        return NULL;
    }
    if(pNode->next == NULL)
    {
        //process,buffer tail
        pNode->usedFlag = 1;
        pNode->previous = pPreviousNode;
        pNode->next = ((_u8*)&pNode->usedFlag+lenMalloc);
        txMallocNode_t *pNext = (txMallocNode_t*)pNode->next;
        pNext->size = pNode->size - 12 - lenMalloc;
        pNode->size = lenMalloc;
        return ((_u8*)&pNode->usedFlag+1);
    }
    else
    {
        //process,buffer middle
        pNode->usedFlag = 1;
        pNode->previous = pPreviousNode;
        if(pNode->size>(lenMalloc+16))
        {
        	txMallocNode_t *pNext = (txMallocNode_t*)pNode->next;
        	txMallocNode_t *pNew  = ((_u8*)&pNode->usedFlag+lenMalloc);
        	pNew->usedFlag = 0;
        	pNew->size = pNode->size - 12 - lenMalloc;
        	pNew->next = pNext;
        	pNode->next = pNew;
        	pNext->previous = pNew;
        	pNew->previous = pNode;
        }
        else
        {
            pNode->usedFlag = 1;
        }
        return ((_u8*)&pNode->usedFlag+2);
    }
}

tx_malloc_e tx_free(_u8* PfreeNode)
{
	txMallocNode_t *pNode = (txMallocNode_t*)txMalloc->pStart;
	txMallocNode_t *pPrevious = (txMallocNode_t*)pNode;
	pNode = pNode->next;
	while(pNode!=NULL)
	{
		pNode = pNode->next;
		pPrevious = pPrevious->next;
		if(((_u8*)pNode+12)==PfreeNode)
		{
            break;
		}
	}
	if(pNode == NULL)
	{
        return TX_MALLOC_NOT_FROUND;
	}
	if(pNode->next == NULL)
	{
        if(pPrevious->usedFlag==0)
        {
		    pPrevious->next = NULL;
            pPrevious+=(12+pNode->size);
		    txMemsetByte((_u8*)pNode,0,12+pNode->size);
        }
        else
        {
            pNode->usedFlag = 0;
            pNode->size = 0;
            txMemsetByte((_u8*)pNode+12,0,pNode->size);
        }
	}
	else
	{
        if(pPrevious->usedFlag == 0)
        {

        }
        txMallocNode_t *pNext = pNode->next;
        if(pNode->next.usedFlag == 0)
        {
            
        }

		pPrevious->next = pNext;
		pNext->previous = pPrevious;
		pPrevious->size+=(12+pNode->size);
		txMemsetByte((_u8*)pNode,0,12+pNode->size);
	}
	return TX_MALLOC_FREE_SUCCESS;
}


