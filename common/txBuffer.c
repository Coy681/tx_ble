/*
 * buffer.c
 *
 *  Created on: 2024年12月7日
 *      Author: 12407
 */

#ifndef BUFFER_C_
#define BUFFER_C_
#include "txBuffer.h"
#include "txType.h"
#include "txAttribute.h"

static txBuffer_t* pDatabase = NULL;
/********************define log database function***********************/
_RAM_CODE
txBuffer_t *txBuffer_cast(txBuffer_t* pCast)
{
	pDatabase = pCast;
	return pDatabase;
}

_RAM_CODE
_u8* tx_buffer_database_get_write_pointer(void)
{
	_u8* pRet = pDatabase->database.pointer+(pDatabase->database.blockWptr&(pDatabase->database.blockNum-1))*pDatabase->database.blockSize;
    return pRet;
}
_RAM_CODE
_u8* tx_buffer_database_get_read_pointer(void)
{
	_u8* pRet = pDatabase->database.pointer+(pDatabase->database.blockRptr&(pDatabase->database.blockNum-1))*pDatabase->database.blockSize;
    return pRet;
}
_RAM_CODE
int tx_buffer_database_block_availble(void)
{
    if(pDatabase->database.blockWptr!=pDatabase->database.blockRptr)
    {
        return 1;
    }
    return 0;
}
_RAM_CODE
void tx_buffer_database_push_data(_u8* data,_u32 dataLen)
{
    _u8* pLog = pDatabase->getWritePointer();
    _u32 len = (dataLen>pDatabase->database.blockSize?pDatabase->database.blockSize:dataLen);
    while(len--)
    {
    	*pLog++ = *data++;
    }
    pDatabase->database.blockWptr++;
}
_RAM_CODE
_u8* tx_buffer_database_pop_data(void)
{
    _u8* pLog = pDatabase->getReadPointer();
    pDatabase->database.blockRptr++;
    return pLog;
}

void txBuffer_database_init(_u8* pointer,_u16 num,_u32 size)
{
	pDatabase->database.pointer         = pointer;
	pDatabase->database.blockNum        = num;
	pDatabase->database.blockSize       = size;
	pDatabase->database.blockWptr       = pDatabase->database.blockRptr = 0;
	pDatabase->getReadPointer           = tx_buffer_database_get_read_pointer;
	pDatabase->getWritePointer          = tx_buffer_database_get_write_pointer;
    pDatabase->blockAvailble            = tx_buffer_database_block_availble;
    pDatabase->blockPush                = tx_buffer_database_push_data;
    pDatabase->blockPop                 = tx_buffer_database_pop_data;
}


#endif /* BUFFER_C_ */
