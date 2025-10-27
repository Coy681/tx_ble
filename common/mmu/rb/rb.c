/*
 * buffer.c
 *
 *  Created on: 2024年12月7日
 *      Author: 12407
 */

#ifndef BUFFER_C_
#define BUFFER_C_
#include "rb.h"
#include "common/txAttribute.h"
#include "common/txAssert.h"
#include"../ml/ml.h"
/********************define log database function***********************/
static int tx_rb_is_empty(tx_rb_t* rb)
{
    ASSERT(rb != NULL && rb->num > 0);
    return (((rb->wPtr - rb->rPtr) % rb->num) == 0);
}

static int tx_rb_is_full(tx_rb_t* rb)
{
    ASSERT(rb != NULL && rb->num > 0);
    return (((rb->wPtr - rb->rPtr) % rb->num) == rb->num - 1);
}

static _u8* tx_rb_get_read_pointer(tx_rb_t* rb)
{
    ASSERT(rb != NULL);
    return (rb->p+(rb->rPtr%rb->num)*rb->size);
}

static _u8* tx_rb_get_write_pointer(tx_rb_t* rb)
{
    ASSERT(rb != NULL);
    return (rb->p+(rb->wPtr%rb->num)*rb->size);
}

static void tx_rb_move_read_pointer(tx_rb_t* rb)
{
    ASSERT(rb != NULL);
    rb->rPtr++;
}

static void tx_rb_move_write_pointer(tx_rb_t* rb)
{
    ASSERT(rb != NULL);
    rb->wPtr++;
}

void tx_rb_init(tx_rb_t* rb,_u16 size,_u16 num)
{
    ASSERT(rb!=NULL);
    rb->p           = tx_malloc(size*num);
    rb->wPtr        = rb->rPtr = 0;
    rb->size        = size;
    rb->num         = num;
    rb->isEmpty     = tx_rb_is_empty;
    rb->isFull      = tx_rb_is_full;
    rb->getReadPtr  = tx_rb_get_read_pointer;
    rb->getWritePtr = tx_rb_get_write_pointer;
    rb->moveReadPtr = tx_rb_move_read_pointer;
    rb->moveWritePtr= tx_rb_move_write_pointer;
}

#endif /* BUFFER_C_ */
