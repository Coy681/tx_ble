/*
 * trace.c
 *
 *  Created on: 2024年12月6日
 *      Author: Admin
 */

#include "log.h"
#include"../../platform/platform.h"

volatile static _u8  strToHex[] = "0123456789abcdef";

#define LOG_STATUS_BUSY         BIT(0)

#define LOG_GET_STATUS          1
#define LOG_SET_STATUS          2
#define LOG_CLEAR_STATUS        3

_u8 inputBuffer[LOG_INPUT_BUFFER_NUMBER*LOG_INPUT_BUFFER_SIZE];
_u8 outputBuffer[LOG_OUTPUT_BUFFER_NUMBER*LOG_OUTPUT_BUFFER_SIZE];
txBuffer_t logInputBuffer;
txBuffer_t logOutputBuffer;


void log_output(_u8* pString,_u8* pData,_u32 dataLen)
{
    if(logObject.mode == LOG_HARDWARE_NONE)
    {
        return;
    }

    _u32 calStrLen = txStringLength(pString);
    _u32 calDataLen = dataLen;
    _u32 totalLen = 0;

    buffer_t* pLogOutputBuffer = &logObject.database.outputbuffer;
    _u8 *pLog = log_get_buffer_pointer(LOG_DIR_OUTPUT,LOG_BUFFER_POINTER_WRITE);

    if((calStrLen+4)>=pLogOutputBuffer->size)
    {
        calStrLen = pLogOutputBuffer->size - 4；
        calDataLen = 0；
    }
    else if(calStrLen+(3*calDataLen)+4)>pLogOutputBuffer.size)
    {
        calDataLen = (pLogOutputBuffer.size - calStrLen - 4)/3;
    }
    totalLen = calStrLen+(3*calDataLen)+2;
W
    *pLog++ = totalLen*0xff;
    *pLog++ = (totalLen>>8)*0xff;

    while(calStrLen--)
    {
    	*pLog++ = *pString++;
    }
    *pLog++ = ":";
	for (int i = 0; i < calDataLen; i++)
	{
		*pLog++ = ' ';
        *pLog++ = strToHex[*pData>>8];
        *pLog++ = strToHex[*pData&0x0f];
        pData++;
	}
	*pLog++ = '\n';
	pLogOutputBuffer.wptr++;
}
