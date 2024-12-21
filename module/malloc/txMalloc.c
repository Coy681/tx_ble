/*
 * txMalloc.c
 *
 *  Created on: 2024年12月10日
 *      Author: 12407
 */
#include"txMalloc.h"
#include"config.h"
typedef struct
{
    _u32 reserved;
    _u32 totalSize;
    _u32 unusedSize;
    _u8* pStart;
}txMalloc_t;

typedef struct txMallocNode_t txMallocNode_t;
typedef struct txMallocNode_t
{
	txMallocNode_t* previous;
	txMallocNode_t* next;
    _u16 size;
    _u16 usedFlag;
}txMallocNode_t;

static _u8 txMallocBuffer[TX_MALLOC_BUFFER_SIZE];

static txMalloc_t txMalloc;

void tx_malloc_init(void)
{
    txMalloc.totalSize =  TX_MALLOC_BUFFER_SIZE;
    txMalloc.unusedSize = txMalloc.totalSize-12;//now not use,maybe used in future
    txMalloc.pStart = txMallocBuffer;
    txMallocNode_t *pNode = (txMallocNode_t*)txMalloc.pStart;
    pNode->previous = NULL;
    pNode->usedFlag = 0;
    pNode->size = txMalloc.totalSize-12;
    pNode->next = NULL;
}

_u8* tx_malloc(_u16 length)
{
	if(txMalloc.pStart == NULL)
	{
        return NULL;
	}
    _u16 mallocLen = ((length+3)&(~3));
    txMallocNode_t* pNode = (txMallocNode_t*)txMalloc.pStart;
    txMallocNode_t* pPreviousNode =  NULL;
    txMallocNode_t* pNext = NULL;
    txMallocNode_t* pNew = NULL;
    while(pNode != NULL)
    {
        if(!pNode->usedFlag&&pNode->size>mallocLen)
        {
            break;
        }
        pPreviousNode = pNode;
        pNode = pNode->next;
    }
    if(pNode == NULL)
    {
        return NULL;
    }
    pNext = pNode->next;
    if(pNext == NULL)
    {
        pNode->usedFlag = 1;
        if(pNode->size>=(mallocLen+16))//size enough to generate new node
        {
            pNext = (txMallocNode_t*)((_u8*)pNode + 12 + mallocLen);
            pNode->next = pNext;
            pNext->previous = pNode;
            pNext->next = NULL;
            pNext->usedFlag = 0;
            pNext->size = pNode->size - mallocLen - 12;
            pNode->size = mallocLen;
        }
        return ((_u8*)pNode+12);
    }
    else
    {
        //process,buffer middle
        pNode->usedFlag = 1;
        if(pNode->size>=(mallocLen+16))
        {
            pNew = (txMallocNode_t*)((_u8*)pNode + 12 + mallocLen);
            pNew->usedFlag = 0;
            pNew->next = pNode->next;
            pNew->previous = pNode;
            pNew->size = pNode->size - 12 - mallocLen;
            pNode->size = mallocLen;
            pNode->next = pNew;
            pNext->previous = pNew;
        }
        return ((_u8*)pNode+12);
    }
}
volatile _u16 AAA_SIZE = 0;
tx_malloc_ret_e tx_free(_u8* pFreeNode)
{
	if(txMalloc.pStart == NULL)
	{
        return TX_MALLOC_MALLOC_INVALID;
	}
	txMallocNode_t *pNode = (txMallocNode_t*)txMalloc.pStart;
	txMallocNode_t *pPrevious = NULL;
    txMallocNode_t *pNext = NULL;
    while(pNode!=NULL)
    {
        if(((_u8*)pNode+12) == pFreeNode)
        {
            break;
        }
        pPrevious = pNode;
        pNode = pNode->next;
    }
    if(pNode == NULL)
    {
        return TX_MALLOC_NOT_FROUND;
    }
    pNext = pNode->next;
    if(pPrevious == NULL)
    {
        if(pNext!=NULL && pNext->usedFlag == 0)
        {
            txMallocNode_t* pNextNext = pNext->next;
            pNode->next = pNextNext;
            if(pNextNext!=NULL)
            {
                pNextNext->previous = pNode;
            }
            pNode->size = pNode->size + 12 + pNext->size;
            txMemsetByte((_u8*)pNext,0,12+pNext->size);
        }
        pNode->usedFlag = 0;
        return TX_MALLOC_FREE_SUCCESS;
    }
    else if(pNext == NULL)
    {
        if(pPrevious->usedFlag == 0)
        {
            pPrevious->next = NULL;
            pPrevious->size = pPrevious->size + 12 +pNode->size;
            txMemsetByte((_u8*)pNode,0,12+pNode->size);
        }
        else
        {
            pNode->usedFlag = 0;

            txMemsetByte((_u8*)pNode+12,0,pNode->size);
        }
        return TX_MALLOC_FREE_SUCCESS;
    }
    else // previous and next not NULL
    {
        txMallocNode_t* pStart = pNode;
        txMallocNode_t* pEnd = pNext;
        _u8* pClear = (_u8*)pNode + 12;
        _u32 clearSize = pNode->size;
        _u32 memorySize = pNode->size;
        pNode->usedFlag = 0;
        if(pPrevious->usedFlag == NULL)
        {
            pStart = pPrevious;
            pClear =  (_u8*)pNode;
            clearSize = pNode->size + 12;
            memorySize = pPrevious->size + 12 + pNode->size;
        }
        if(pNext->usedFlag == NULL)
        {
            pEnd = pNext->next;
            clearSize+=(12+pNext->size);
            memorySize += (12+pNext->size);
        }
        pStart->next = pEnd;
        if(pEnd != NULL)
        {
            pEnd->previous = pStart;
        }
        pStart->size = memorySize;
        AAA_SIZE = clearSize;
        txMemsetByte(pClear,0,clearSize);

        return TX_MALLOC_FREE_SUCCESS;
    }
}


void malloc_test(void)
{
//    _u8* p = tx_malloc(4);
//    _u8* p1 = tx_malloc(4);
//    _u8* p2 = tx_malloc(4);
//    _u8* p3 = tx_malloc(4);
//    _u8* p4 = tx_malloc(4);
//	tx_free(p4);
//	tx_free(p3);
//	tx_free(p2);
//	tx_free(p1);
//	tx_free(p);

	_u8* p = tx_malloc(34);
	_u8* p1 = tx_malloc(4);
	tx_free(p);
	_u8* p2 = tx_malloc(20);
}



