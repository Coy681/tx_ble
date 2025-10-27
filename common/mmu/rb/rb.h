/*
 * fifo.h
 *
 *  Created on: 2024年12月7日
 *      Author: 12407
 */

#ifndef TXBUFFER_H_
#define TXBUFFER_H_
#include"common/txType.h"
/**********************define tx buffer database**************************/
typedef struct tx_rb_t  tx_rb_t;
typedef struct tx_rb_t
{
	_u8* p;
	_u16 size;
	_u16 num;
	_u16 wPtr;
	_u16 rPtr;
	int  (*isEmpty)(tx_rb_t*);
	int  (*isFull)(tx_rb_t*);
	_u8* (*getReadPtr)(tx_rb_t*);
	_u8* (*getWritePtr)(tx_rb_t*);
	void (*moveReadPtr)(tx_rb_t*);
	void (*moveWritePtr)(tx_rb_t*);
}tx_rb_t;

void tx_rb_init(tx_rb_t* rb,_u16 size,_u16 num);

#endif /* TXBUFFER_H_ */
