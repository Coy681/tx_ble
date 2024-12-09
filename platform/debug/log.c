/*
 * trace.c
 *
 *  Created on: 2024年12月6日
 *      Author: Admin
 */

#include "log.h"

#include "../../common/txCommon.h"
#include"../../platform/platform.h"

/**************************define log status*****************************/
typedef void(*log_status_f)(_u8);
typedef struct
{
	_u8  value;
	_u8  reserved0;
	_u16 reserved1;
	log_status_f statusOprtation;
}log_status_t;
/**************************define log database*****************************/
typedef void(*log_database_f)(_u8 object);
typedef struct
{
	buffer_t       buffer;
	log_database_f databaseInit;
	log_database_f databaseGetWptr;
	log_database_f databaseGetWptr;
}log_database_t;

typedef struct
{
	log_status_t   status;
	log_database_t database;
    _u8 setDataBase;
    _u8 function;
    _u8 process;
}log_object_t;



//
//typedef void(*log_status_f)(_u8);
///***************************define log input***************************/
//static   _u8      logOutputDatabase[LOG_OUTPUT_BUFFER_NUMBER*LOG_OUTPUT_BUFFER_SIZE];
//typedef void(*log_output_f)(_u8*,_u8*,_u32);
//typedef struct
//{
//    _u8            status;
//    buffer_t       database;
//    log_status_f   statusOperation;
//    log_output_f   output;
//}log_output_t;
///***************************define log output***************************/
//static   _u8      logInputBuffer[LOG_INPUT_BUFFER_NUMBER*LOG_INPUT_BUFFER_SIZE];
//typedef struct
//{
//    _u8            status;
//    buffer_t       database;
//    log_status_f   statusOperation;
//    log_input_f    input;
//}log_input_t;
//
///***************************define log object****************************/
//#define LOG_DIR_INPUT               1
//#define LOG_DIR_OUTPUT              2
//#define LOG_STATUS_BUSY             BIT(0)
//
//#define LOG_STATUS_SET              1
//#define LOG_STATUS_GET              2
//#define LOG_STATUS_CLEAR            3
//
//#define LOG_BUFFER_POINTER_WRITE    1
//#define LOG_BUFFER_POINTER_READ     1
//
//typedef void(*log_process_f)(void);
//typedef struct
//{
//	log_hardware_e hardware;
//	log_output_t   output;
//	log_input_t    input;
//	log_process_f  process;
//}log_object_t;
//
//static  log_object_t  log;
//volatile _u8      strToHex[] = "0123456789abcdef";
//void log_init(log_hardware_e logHardware,log_input_t inputCb)
//{
//    if(logHardware == LOG_HARDWARE_UART)
//    {
//    	log.hardware = LOG_HARDWARE_UART;
//    	hal_uart_register_task(HAL_UART_BAUDRATE_115200,\
//    			               log_input_task,log_output_done,\
//							   HAL_UART_PARITY_NONW,HAL_UART_STOP_BITE_ONE);
//    }
//    //output initial
//    log_output_t* pLogOutput   = &log.output;
//    pLogOutput->status         = 0;
//    pLogOutput->statusOperation= log_output_status_operaiton;
//    pLogOutput->output         = log_output();
//	pLogOutput->database.p     = logOutputDatabase;
//    pLogOutput->database.wptr  = pLogOutput->database.rptr = 0;
//    pLogOutput->database.size  = LOG_OUTPUT_BUFFER_SIZE;
//    pLogOutput->database.num   = LOG_OUTPUT_BUFFER_NUMBER;
//    //input initial
//    log_input_t* pLogInput     = &log.input;
//    pLogInput->status          = 0;
//    pLogInput->statusOperation = log_input_status_operaiton;
//    pLogInput->input           = log_input;
//	pLogInput->database.p      = logInputBuffer;
//    pLogInput->database.wptr   = pLogInput->database.rptr = 0;
//    pLogInput->database.size   = LOG_OUTPUT_BUFFER_SIZE;
//    pLogInput->database.num    = LOG_OUTPUT_BUFFER_NUMBER;
//    //log object initial
//    log.process = log_loop;
//}
//
//int log_input_status_operaiton(_u8 operation)
//{
//	if(operation == LOG_STATUS_SET)
//	{
//		log.input.status |= operation;
//	}
//	else if(operation == LOG_STATUS_GET)
//	{
//        return log.input.status;
//	}
//	else if(operation == LOG_STATUS_CLEAR)
//	{
//		log.input.status &= (~operation);
//	}
//	return 0;
//}
//
//int log_output_status_operaiton(_u8 operation)
//{
//	if(operation == LOG_STATUS_SET)
//	{
//		log.output.status |= operation;
//	}
//	else if(operation == LOG_STATUS_GET)
//	{
//        return log.output.status;
//	}
//	else if(operation == LOG_STATUS_CLEAR)
//	{
//		log.output.status &= (~operation);
//	}
//	return 0;
//}
//
//
//void log_output_done(void)
//{
//    hal_gpio_led_toggle(LED_2);
//}
//
//void log_input_task(void)
//{
//    hal_gpio_led_toggle(LED_3);
//}
//
//static _u8* log_get_buffer_pointer(_u8 logDir,_u8 pointerType)
//{
//    buffer_t* pLogBuffer = NULL;
//    if(logDir == LOG_DIR_INPUT)
//    {
//        pLogBuffer = &log.input.database;
//    }
//    else
//    {
//        pLogBuffer = &log.output.database;
//    }
//    if(pointerType == LOG_BUFFER_POINTER_WRITE)
//    {
//        return (pLogBuffer->p + (pLogBuffer->wptr&(pLogBuffer->num-1))*pLogBuffer->size);
//    }
//    else
//    {
//        return (pLogBuffer->p + (pLogBuffer->rptr&(pLogBuffer->num-1))*pLogBuffer->size);
//    }
//}
//
//void log_output(_u8* pString,_u8* pData,_u32 dataLen)
//{
//    if(logObject.mode == LOG_HARDWARE_NONE)
//    {
//        return;
//    }
//
//    _u32 calStrLen = txStringLength(pString);
//    _u32 calDataLen = dataLen;
//    _u32 totalLen = 0;
//
//    buffer_t* pLogOutputBuffer = &logObject.database.outputbuffer;
//    _u8 *pLog = log_get_buffer_pointer(LOG_DIR_OUTPUT,LOG_BUFFER_POINTER_WRITE);
//
//    if((calStrLen+4)>=pLogOutputBuffer->size)
//    {
//        calStrLen = pLogOutputBuffer->size - 4；
//        calDataLen = 0；
//    }
//    else if(calStrLen+(3*calDataLen)+4)>pLogOutputBuffer.size)
//    {
//        calDataLen = (pLogOutputBuffer.size - calStrLen - 4)/3;
//    }
//    totalLen = calStrLen+(3*calDataLen)+2;
//W
//    *pLog++ = totalLen*0xff;
//    *pLog++ = (totalLen>>8)*0xff;
//
//    while(calStrLen--)
//    {
//    	*pLog++ = *pString++;
//    }
//    *pLog++ = ":";
//	for (int i = 0; i < calDataLen; i++)
//	{
//		*pLog++ = ' ';
//        *pLog++ = strToHex[*pData>>8];
//        *pLog++ = strToHex[*pData&0x0f];
//        pData++;
//	}
//	*pLog++ = '\n';
//	pLogOutputBuffer.wptr++;
//}
//
//void log_input()
//{
//
//}
//
//void log_output_task()
//{
//     _u8* pData = log_get_buffer_pointer((LOG_DIR_OUTPUT,LOG_BUFFER_POINTER_WRITE);
//     _u16 dataLen = (u16)pData;
//     hal_uart_send_data(pData+2,(u32)dataLen);
//
//}
//
//void log_loop(void)
//{
//	if(logObject.mode == LOG_HARDWARE_NONE)
//	{
//		return;
//	}
//    logDatabase_t* pLogDatabase = &logObject.database;
//    if(!pLogDatabase->outputBusyFlag && pLogDatabase->outputbuffer.wptr!=pLogDatabase->outputbuffer.rptr)
//    {
//        log_output_task();
//    }
//    if(!pLogDatabase->inputBusyFlag && pLogDatabase->inputBuffer.wptr!=pLogDatabase->inputBuffer.rptr)
//    {
//        log_input_task();
//    }
//}
