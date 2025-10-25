/*
 * fifo.h
 *
 *  Created on: 2024年12月7日
 *      Author: 12407
 */

#ifndef TXBUFFER_H_
#define TXBUFFER_H_
#include"txType.h"
#include"common/txAssert.h"
#include"../ml/ml.h"
/**********************define tx buffer database**************************/
typedef struct txRb_t  txRb_t;
typedef struct txRb_t
{
	_u8* p;
	_u16 size;
	_u16 num;
	_u16 wPtr;
	_u16 rPtr;
	int  (*isEmpty)(txRb_t*);
	int  (*isFull)(txRb_t*);
	_u8* (*read)(txRb_t*);
	_u8* (*write)(txRb_t*);
	void (*readNext)(txRb_t*);
	void (*writeNext)(txRb_t*);
}txRb_t;

void tx_rb_init(txRb_t* rb,_u16 size,_u16 num);

#endif /* TXBUFFER_H_ */
