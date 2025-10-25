/*
 * buffer.c
 *
 *  Created on: 2024年12月7日
 *      Author: 12407
 */

#ifndef BUFFER_C_
#define BUFFER_C_
#include "rb.h"
#include "txType.h"
#include "txAttribute.h"

/********************define log database function***********************/
static int tx_rb_is_empty(txRb_t* rb)
{
    ASSERT(rb != NULL && rb->num > 0);
    return (((rb->wPtr - rb->rPtr) % rb->num) == 0);
}

static int tx_rb_is_full(txRb_t* rb)
{
    ASSERT(rb != NULL && rb->num > 0);
    return (((rb->wPtr - rb->rPtr) % rb->num) == rb->num - 1);
}

static _u8* tx_rb_read(txRb_t* rb)
{
    ASSERT(rb != NULL);
    return (rb->p+(rb->rPtr%rb->num)*rb->size);
}

static _u8* tx_rb_write(txRb_t* rb)
{
    ASSERT(rb != NULL);
    return (rb->p+(rb->wPtr%rb->num)*rb->size);
}

static void tx_rb_read_next(txRb_t* rb)
{
    ASSERT(rb != NULL);
    rb->rPtr++;
}

static void tx_rb_write_next(txRb_t* rb)
{
    ASSERT(rb != NULL);
    rb->wPtr++;
}

void tx_rb_init(txRb_t* rb,_u16 size,_u16 num)
{
    ASSERT(rb!=NULL);
    rb->p         = tx_malloc(size*num);
    rb->wPtr      = rb->rPtr = 0;
    rb->size      = size;
    rb->num       = num;
    rb->isEmpty   = tx_rb_is_empty;
    rb->isFull    = tx_rb_is_full;
    rb->read      = tx_rb_read;
    rb->write     = tx_rb_write;
    rb->readNext  = tx_rb_read_next;
    rb->writeNext = tx_rb_write_next;
}

#endif /* BUFFER_C_ */
