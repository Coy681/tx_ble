/*
 * fifo.h
 *
 *  Created on: 2024年12月7日
 *      Author: 12407
 */

#ifndef BUFFER_H_
#define BUFFER_H_

typedef struct {
    unsigned int        size;
    unsigned short      num;
    unsigned char       wptr;
    unsigned char       rptr;
    unsigned char*      p;
}buffer_t;


#endif /* BUFFER_H_ */
