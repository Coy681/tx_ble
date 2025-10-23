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

typedef struct txBuffer_t txBuffer_t;

typedef struct txBuffer_t
{
    database_t  database;//please don't access database directly
	_u8*        (*getWritePointer)(txBuffer_t*);
	_u8*        (*getReadPointer)(txBuffer_t*);
	int         (*blockAvailble)(txBuffer_t*);
	void        (*wPtrIncrease)(txBuffer_t*);
	void        (*rPtrIncrease)(txBuffer_t*);
	_u32        (*getDataLen)(txBuffer_t*,_u32 dataLen);
}txBuffer_t;


void txBuffer_init(txBuffer_t* pDatabase,_u8* pointer,_u16 num,_u32 size);

#endif /* TXBUFFER_H_ */
