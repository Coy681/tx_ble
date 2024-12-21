/*
 * txMalloc.h
 *
 *  Created on: 2024年12月10日
 *      Author: 12407
 */

#ifndef TXMALLOC_TXMALLOC_H_
#define TXMALLOC_TXMALLOC_H_

#define TX_MALLOC_BUFFER_SIZE     20*1024

typedef enum
{
	TX_MALLOC_FREE_SUCCESS       = 0,
	TX_MALLOC_INVALID_PARAMETER  = 1,
	TX_MALLOC_NOT_FROUND         = 2,

}tx_malloc_ret_e;

_u8* tx_malloc(_u16 length);

tx_malloc_ret_e tx_free(_u8* PfreeNode);
#endif /* TXMALLOC_TXMALLOC_H_ */
