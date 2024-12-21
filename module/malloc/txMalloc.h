/*
 * txMalloc.h
 *
 *  Created on: 2024年12月10日
 *      Author: 12407
 */

#ifndef MALLOC_TXMALLOC_H_
#define MALLOC_TXMALLOC_H_
#include"common/txCommon.h"

typedef enum
{
	TX_MALLOC_FREE_SUCCESS       = 0,
	TX_MALLOC_INVALID_PARAMETER  = 1,
	TX_MALLOC_NOT_FROUND         = 2,
	TX_MALLOC_MALLOC_INVALID     = 3,
}tx_malloc_ret_e;

void tx_malloc_init(void);

_u8* tx_malloc(_u16 length);

tx_malloc_ret_e tx_free(_u8* PfreeNode);

void malloc_test(void);
#endif /* MALLOC_TXMALLOC_H_ */
