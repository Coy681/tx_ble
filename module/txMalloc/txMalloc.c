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
    _u16 mallocLen = ((length+3)&(~3));
    txMallocNode_t* pNode = (txMallocNode_t*)txMalloc.pStart;
    txMallocNode_t* pPreviousNode =  NULL;
    txMallocNode_t* pNext = NULL;
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
        if(pNode->size>(mallocLen+16))
        {
            pNext = (txMallocNode_t*)((_u8*)&pNode->usedFlag + 2 + mallocLen);
            pNode->next = pNext;
            pNext->previous = pNode;
            pNext->next = NULL;
            pNext->usedFlag = 0;
            pNext->size = pNode->size - mallocLen - 12;
        }
        return ((_u8*)&pNode->usedFlag+2);
    }
    else
    {
        //process,buffer middle
        pNode->usedFlag = 1;
        return ((_u8*)&pNode->usedFlag+2);
    }
}

tx_malloc_e tx_free(_u8* PfreeNode)
{
	txMallocNode_t *pNode = (txMallocNode_t*)txMalloc.pStart;
	txMallocNode_t *pPrevious = NULL;
    txMallocNode_t *pNext = NULL;
    while(pNode!=NULL)
    {
        if((_u8*)pNode+12 == PfreeNode)
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
        if(pNext->usedFlag == 0)
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
        else
        {
            pNode->usedFlag = 0;

        } 
        return TX_MALLOC_FREE_SUCCESS;
    }
    else if(pNext == NULL)
    {
        if(pPrevious->usedFlag == 0)
        {
            pPrevious->next = NULL;
            pPrevious->size = pPrevious->size + 12 +pNode->size;
            txMemsetByte((_u8*)pNode,0,12+pPrevious->size);
        }
        else
        {
            pNode->usedFlag = 0;
        }
        return TX_MALLOC_FREE_SUCCESS;
    }
    else 
    {
        //find previous and next position
        //find total size
        if(pPrevious->usedFlag == 0)
        {

        }
        else if(pNext->usedFlag == 0)
        {

        }
        else if(pPrevious->usedFlag == 0&&pNext->usedFlag == 0)
        {

        }
        else
        {

        }
        return TX_MALLOC_FREE_SUCCESS;
    }
}


