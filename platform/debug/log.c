/*
 * trace.c
 *
 *  Created on: 2024年12月6日
 *      Author: Admin
 */

#include "log.h"
#include"../../platform/platform.h"
#include"../../common/common.h"

volatile _u8      strToHex[] = "0123456789abcdef";

/***************************define log database************************/
#define LOG_DIR_INPUT               1
#define LOG_DIR_OUTPUT              2

#define LOG_BUFFER_POINTER_WRITE    1
#define LOG_BUFFER_POINTER_READ     2

#define LOG_STATUS_BUSY             1
#define LOG_STATUS_FREE             2
static   _u8      logInputBuffer[LOG_INPUT_BUFFER_NUMBER*LOG_INPUT_BUFFER_SIZE];
static   _u8      logOutputBuffer[LOG_OUTPUT_BUFFER_NUMBER*LOG_OUTPUT_BUFFER_SIZE];
typedef struct
{
    buffer_t   outputbuffer;
	buffer_t   inputBuffer;
}logDatabase_t;
/***************************define log function*************************/ 
typedef void(*log_optput)(_u8*,_u8*,_u32);
typedef void(*log_loop)(void);
typedef void(*log_status)(_u8 status);


//todo,define a totally status change function
typedef struct
{
    log_loop   loop;
    log_optput output;
    log_input  input;
    log_status setStatus;
    log_status clearStatus;
}logFunction_t;
/****************************define log object ********************************/
#define LOG_STATUS_NONE             BIT(0)
#define LOG_STATUS_INPUT_BUSY       BIT(1)
#define LOG_STATUS_OUTPUT_BUSY      BIT(2)

typedef struct
{
    log_mode_e     mode;
    _u8            status;
    logDatabase_t  database;
    logFunction_t  function;
}logObject_t;
static  logObject_t  logObject;

void log_init(log_mode_e logHardware,log_input inputCb)
{
    if(logHardware == LOG_HARDWARE_UART)
    {
    	logObject.mode = LOG_HARDWARE_UART;
    	hal_uart_register_task(HAL_UART_BAUDRATE_115200,\
    			               log_input_task,log_output_done,\
							   HAL_UART_PARITY_NONW,HAL_UART_STOP_BITE_ONE);
    }
    logObject.status = LOG_STATUS_NONE;
    logDatabase_t* pLogDatabase = &logObject.database;
    //log database init
    //output 
    pLogDatabase->outputBuffer.p    = logOutputBuffer;
    pLogDatabase->outputBuffer.wptr = logObject.outputBuffer.rptr = 0;
    pLogDatabase->outputBuffer.size = LOG_OUTPUT_BUFFER_SIZE;
    pLogDatabase->outputBuffer.num  = LOG_OUTPUT_BUFFER_NUMBER;
    //input      
    pLogDatabase->inpurtBuffer.p    = logInputBuffer;
    pLogDatabase->inpurtBuffer.wptr = logObject.inpurtBuffer.rptr = 0;
    pLogDatabase->inpurtBuffer.size = LOG_OUTPUT_BUFFER_SIZE;
    pLogDatabase->inpurtBuffer.num  = LOG_OUTPUT_BUFFER_NUMBER;
    //log function init
    logFunction_t *pLogFunction     = &logObject.function;
    pLogFunction->loop              = log_loop;
    pLogFunction->output            = log_output;
    pLogFunction->input             = inputCb;
    pLogFunction->setStatus         = log_set_status;
    pLogFunction->clearStatus       = log_clear_status;
}           

void log_set_status(_u8 status)
{
    logObject|=status;
}

void log_clear_status(_u8 status)
{
    logObject&=(~status);
}


void log_output_done(void)
{
    hal_gpio_led_toggle(LED_2);
}

void log_input_task(void)
{
    hal_gpio_led_toggle(LED_3);
}

static _u8* log_get_buffer_pointer(_u8 logDir,_u8 pointerType)
{
    buffer_t* pLogBuffer = NULL;
    if(logDir == LOG_DIR_INPUT)
    {
        pLogBuffer = &logObject.database.inputBuffer;
    }
    else
    {
        pLogBuffer = &logObject.database.outputbuffer;
    }
    if(pointerType == LOG_BUFFER_POINTER_WRITE)
    {
        return (pLogBuffer->p + (pLogBuffer->wptr&(pLogBuffer->num-1))*pLogBuffer->size);
    }
    else
    {
        return (pLogBuffer->p + (pLogBuffer->rptr&(pLogBuffer->num-1))*pLogBuffer->size);
    }
}

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

void log_input_task()
{

}

void log_output_task()
{
     _u8* pData = log_get_buffer_pointer((LOG_DIR_OUTPUT,LOG_BUFFER_POINTER_WRITE);
     _u16 dataLen = (u16)pData;
     hal_uart_send_data(pData+2,(u32)dataLen);

}

void log_loop(void)
{
	if(logObject.mode == LOG_HARDWARE_NONE)
	{
		return;
	}
    logDatabase_t* pLogDatabase = &logObject.database;
    if(!pLogDatabase->outputBusyFlag && pLogDatabase->outputbuffer.wptr!=pLogDatabase->outputbuffer.rptr)
    {
        log_output_task();
    }
    if(!pLogDatabase->inputBusyFlag && pLogDatabase->inputBuffer.wptr!=pLogDatabase->inputBuffer.rptr)
    {
        log_input_task();
    }
}
