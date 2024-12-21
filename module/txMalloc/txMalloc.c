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

typedef struct txMallocNode_t txMallocNode_t;
typedef struct txMallocNode_t
{
	txMallocNode_t* previous;
	txMallocNode_t* next;
    _u16 size;
    _u16 usedFlag;
}txMallocNode_t;

static txMalloc_t txMalloc;

void tx_malloc_init(_u8* buffer,_u32 size)
{
    txMalloc.totalSize =  size;
    txMalloc.unusedSize = size-12;//maybe used in future
    txMalloc.pStart = buffer;
    txMallocNode_t *pNode = (txMallocNode_t*)txMalloc.pStart;
    pNode->previous = NULL;
    pNode->usedFlag = 0;
    pNode->size = size-12;
    pNode->next = NULL;
}

_u8* tx_malloc(_u16 length)
{
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
        if(pNode->size>(mallocLen+16))//size enough to generate new node
        {
            pNext = (txMallocNode_t*)((_u8*)&pNode + 12 + mallocLen);
            pNode->next = pNext;
            pNext->previous = pNode;
            pNext->next = NULL;
            pNext->usedFlag = 0;
            pNext->size = pNode->size - mallocLen - 12;
        }
        return ((_u8*)pNode+12);
    }
    else
    {
        //process,buffer middle
        pNode->usedFlag = 1;
        if(pNode->size>(mallocLen+16))
        {
            pNew = (txMallocNode_t*)((_u8*)&pNode + 12 + mallocLen);
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

tx_malloc_ret_e tx_free(_u8* pFreeNode)
{
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
        txMallocNode_t* pStart = pPrevious;
        txMallocNode_t* pEnd = pNext;
        _u8* pClear = (_u8*)pNode + 12;
        _u32 clearSize = pNode->size;
        if(pPrevious->usedFlag == NULL)
        {
            pStart = pPrevious;
            pClear =  (_u8*)pNode;
            clearSize = pNode->size + 12;
        }
        if(pNext->usedFlag == NULL)
        {
            pEnd = pNext->next;
            clearSize+=(12+pNext->size);
        }
        pStart->next = pEnd;
        pEnd->previous = pStart;
        if(clearSize!=pNode->size)
        {
            pStart->size += clearSize;
        }
        txMemsetByte(pClear,0,clearSize);

        return TX_MALLOC_FREE_SUCCESS;
    }
}


