/*
 * fifo.h
 *
 *  Created on: 2024年12月7日
 *      Author: 12407
 */

#ifndef TXBUFFER_H_
#define TXBUFFER_H_
#include"txType.h"
/**********************define tx buffer database**************************/

typedef struct  
{
	_u8*        pointer;
    _u32        blockSize;
    _u16        blockNum;
    _u8         blockWptr;
    _u8         blockRptr;
}database_t;
typedef struct
{
    database_t  database;//please don't access database directly
	_u8*        (*getWritePointer)();
	_u8*        (*getReadPointer)();
	int         (*blockAvailble)();
	void        (*blockPush)(_u8*,_u32);//parameter:data pointer,data length
	_u8*        (*blockPop)();
}txBuffer_t;


void txBuffer_database_init(_u8* pointer,_u16 num,_u32 size);

txBuffer_t* txBuffer_cast(txBuffer_t*);//please don't use this function

#define TX_BUFFER_CAST(object)  (txBuffer_cast(object))//hide pThis
#endif /* TXBUFFER_H_ */
