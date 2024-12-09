/*
 * fifo.h
 *
 *  Created on: 2024年12月7日
 *      Author: 12407
 */

#ifndef TXBUFFER_H_
#define TXBUFFER_H_

typedef struct {
    unsigned int        size;
    unsigned short      num;
    unsigned char       wptr;
    unsigned char       rptr;
    unsigned char*      p;
}buffer_t;


#endif /* TXBUFFER_H_ */
