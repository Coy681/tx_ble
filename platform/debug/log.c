/*
 * trace.c
 *
 *  Created on: 2024年12月6日
 *      Author: Admin
 */

#include "log.h"
#include"platform/platform.h"
#include"system/task/task.h"
#include"system/task/event/event.h"
#include"system/task/message/message.h"
#include"platform/debug/debug_gpio.h"

volatile static _u8  strToHex[] = "0123456789abcdef";
/**************************log input process*******************************/
txBuffer_t       logInputBuffer;
log_receive_cb_f logRxCb;

void log_register_rx_callback(log_receive_cb_f cb)
{
    logRxCb = cb;
}

_RAM_CODE static void log_hardware_rx_irq(int len)
{
    _u8* p = logInputBuffer.getWritePointer(&logInputBuffer);
    U32_TO_STREAM(p,len);
    logInputBuffer.wPtrIncrease(&logInputBuffer);
    tx_task_set_event(TX_TASK_ID_LOG_RX,LOG_TASK_EVENT_RX);
    _u8* pReveive = logInputBuffer.getWritePointer(&logInputBuffer);
    hal_uart_set_receive_buffer(pReveive+4,logInputBuffer.database.blockSize-4);
}

static _u32 log_task_event_rx(_u16 taskId,_u32 event)
{
    if(logInputBuffer.blockAvailble(&logInputBuffer))
    {
        _u32 dataLen = 0;
        _u8* data = logInputBuffer.getReadPointer(&logInputBuffer);
        STREAM_TO_U32(dataLen,data);
        if(logRxCb)
        {
            logRxCb(data,dataLen);
        }
        logInputBuffer.rPtrIncrease(&logInputBuffer);
    }
    return (event^LOG_TASK_EVENT_RX);
}

static _u32 log_task_input_event_message(_u16 taskId,_u32 event)
{
    _u8* messsage = tx_message_receive(taskId);
    while(messsage)
    {
        messsage = tx_message_receive(taskId);
    }
    return (event^TX_TASK_EVENT_MESSAGE);
}

static _u32 log_task_input_event_process(_u16 taskId,_u32 event)
{

    if(event&TX_TASK_EVENT_MESSAGE)
    {
        return log_task_input_event_message(taskId,event);
    }
    if(event&LOG_TASK_EVENT_RX)
    {
        return log_task_event_rx(taskId,event);
    }
    return 0;
}
void log_input_init(void)
{
	_u8* plog = tx_malloc(LOG_INPUT_BUFFER_NUMBER*LOG_INPUT_BUFFER_SIZE);
    txBuffer_init(&logInputBuffer,plog,LOG_INPUT_BUFFER_NUMBER,LOG_INPUT_BUFFER_SIZE);
    _u8* pReveive = logInputBuffer.getWritePointer(&logInputBuffer);
    hal_uart_set_receive_buffer(pReveive+4,logInputBuffer.database.blockSize-4);
}

/**************************log output process*******************************/
txBuffer_t       logOutputBuffer;

_RAM_CODE static void log_hardware_tx_irq(void)
{
    if(logOutputBuffer.blockAvailble(&logOutputBuffer))
    {
        _u32 dataLen = 0;
        _u8* pData = logOutputBuffer.getReadPointer(&logOutputBuffer);
        STREAM_TO_U32(dataLen,pData);
        hal_uart_send_data(pData,dataLen);
        logOutputBuffer.rPtrIncrease(&logOutputBuffer);
    }
}

void log_output(_u8* pString,_u8* pData,_u32 dataLen)
{
    if(logOutputBuffer.database.pointer == NULL)
    {
        return;
    }
    _u8* pLog = logOutputBuffer.getWritePointer(&logOutputBuffer);
    _u32 calStrLen = txStringLength(pString);
    _u32 calDataLen = dataLen;
    _u32 totalLen = 0;

    if(calStrLen+6 >logOutputBuffer.database.blockSize)
    {
        calStrLen = logOutputBuffer.database.blockSize - 6;
        calDataLen = 0;
    }
    else if(calStrLen+3*calDataLen+6>logOutputBuffer.database.blockSize)
    {
        calDataLen = (logOutputBuffer.database.blockSize - calStrLen - 6)/3;

    }

    totalLen = calStrLen + 3*calDataLen + 2;
    U32_TO_STREAM(pLog,totalLen);

    while(calStrLen--)
    {
        *pLog++ = *pString++;
    }
    *pLog++ = ':';
    for(int i=0;i<calDataLen;i++)
    {
        *pLog++ = ' ';
        *pLog++ = strToHex[((*pData)>>4)&0x0f];
        *pLog++ = strToHex[(*pData)&0x0f];
        pData++;
    }
    *pLog++ = '\r';

    logOutputBuffer.wPtrIncrease(&logOutputBuffer);
    tx_task_set_event(TX_TASK_ID_LOG_TX,LOG_TASK_EVENT_TX);
}

static _u32 log_task_output_event_message(_u16 taskId,_u32 event)
{
    _u8* messsage = tx_message_receive(taskId);
    while(messsage)
    {
        messsage = tx_message_receive(taskId);
    }
    return (event^TX_TASK_EVENT_MESSAGE);
}
static _u32 log_task_event_tx(_u16 taskId,_u32 event)
{
    if(logOutputBuffer.blockAvailble(&logOutputBuffer))
    {
        _u32 dataLen = 0;
        _u8* pData = logOutputBuffer.getReadPointer(&logOutputBuffer);
        STREAM_TO_U32(dataLen,pData);
        hal_uart_send_data(pData,dataLen);
        logOutputBuffer.rPtrIncrease(&logOutputBuffer);
    }
    return (event^LOG_TASK_EVENT_TX);
}

static _u32 log_task_output_event_process(_u16 taskId,_u32 event)
{

    if(event&TX_TASK_EVENT_MESSAGE)
    {
        return log_task_output_event_message(taskId,event);
    }
    if(event&LOG_TASK_EVENT_TX)
    {
        return log_task_event_tx(taskId,event);
    }
    return 0;
}

void log_output_init(void)
{
    _u8* plog = tx_malloc(LOG_OUTPUT_BUFFER_NUMBER*LOG_OUTPUT_BUFFER_SIZE);
    txBuffer_init(&logOutputBuffer,plog,LOG_OUTPUT_BUFFER_NUMBER,LOG_OUTPUT_BUFFER_SIZE);
}

/***************************************log init**********************************************/
void log_task_init(void)
{
	hal_uart_register_task(HAL_UART_BAUDRATE_1000000,log_hardware_rx_irq,log_hardware_tx_irq,HAL_UART_PARITY_NONE,HAL_UART_STOP_BITE_ONE);
    tx_task_add(log_output_init,log_task_output_event_process,TX_TASK_ID_LOG_TX,TX_TASK_PRIORITY_0);
    tx_task_add(log_input_init,log_task_input_event_process,TX_TASK_ID_LOG_RX,TX_TASK_PRIORITY_0);
}
TASK_INIT(log_task_init);

