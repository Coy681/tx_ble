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
_u8* tx_buffer_database_get_write_pointer(txBuffer_t* pDatabase)
{
	_u8* pRet = pDatabase->database.pointer+(pDatabase->database.blockWptr&(pDatabase->database.blockNum-1))*pDatabase->database.blockSize;
    return pRet;
}
_RAM_CODE
_u8* tx_buffer_database_get_read_pointer(txBuffer_t* pDatabase)
{
	_u8* pRet = pDatabase->database.pointer+(pDatabase->database.blockRptr&(pDatabase->database.blockNum-1))*pDatabase->database.blockSize;
    return pRet;
}
_RAM_CODE
int tx_buffer_database_block_availble(txBuffer_t* pDatabase)
{
    if(pDatabase->database.blockWptr!=pDatabase->database.blockRptr)
    {
        return 1;
    }
    return 0;
}
_RAM_CODE
void tx_buffer_write_pointer_increase(txBuffer_t* pDatabase)
{
    pDatabase->database.blockWptr++;
}

_RAM_CODE
void tx_buffer_read_pointer_increase(txBuffer_t* pDatabase)
{
    pDatabase->database.blockRptr++;
}

_u32 tx_buffer_get_availble_data_length(txBuffer_t* pDatabase,_u32 dataLen)
{
    _u32 len = (dataLen>pDatabase->database.blockSize?pDatabase->database.blockSize:dataLen);
    return len;
}

void txBuffer_init(txBuffer_t* pDatabase,_u8* pointer,_u16 num,_u32 size)
{
	pDatabase->database.pointer         = pointer;
	pDatabase->database.blockNum        = num;
	pDatabase->database.blockSize       = size;
	pDatabase->database.blockWptr       = pDatabase->database.blockRptr = 0;
	pDatabase->getReadPointer           = tx_buffer_database_get_read_pointer;
	pDatabase->getWritePointer          = tx_buffer_database_get_write_pointer;
    pDatabase->blockAvailble            = tx_buffer_database_block_availble;
    pDatabase->wPtrIncrease             = tx_buffer_write_pointer_increase;
    pDatabase->rPtrIncrease             = tx_buffer_read_pointer_increase;
    pDatabase->getDataLen               = tx_buffer_get_availble_data_length;
}


#endif /* BUFFER_C_ */
