/*
 * trace.c
 *
 *  Created on: 2024年12月6日
 *      Author: Admin
 */

#include"log.h"
#include"platform/platform.h"
#include"system/task/task.h"
#include"system/task/event/event.h"
#include"system/task/message/message.h"
#include"platform/debug/debug_gpio.h"
#include"common/mmu/mmu.h"
volatile static _u8  strToHex[24] = "0123456789abcdef";
/**************************log input process*******************************/
tx_rb_t          logRbInput;
log_receive_cb_f logRxCb;

#define LOG_STATUS_BUSY         BIT(0)

#define LOG_GET_STATUS          1
#define LOG_SET_STATUS          2
#define LOG_CLEAR_STATUS        3

void log_register_rx_callback(log_receive_cb_f cb)
{
    logRxCb = cb;
}

_RAM_CODE static void log_hardware_rx_irq(int len)
{
    _u8* p = logRbInput.getWritePtr(&logRbInput);
    U32_TO_STREAM(p,len);
    logRbInput.moveWritePtr(&logRbInput);
    tx_task_set_event(TX_TASK_ID_LOG_RX,LOG_TASK_EVENT_RX);
    _u8* pReveive = logRbInput.getWritePtr(&logRbInput);
    hal_uart_set_receive_buffer(pReveive+4,LOG_INPUT_BUFFER_SIZE-4);
}

static _u32 log_task_event_rx(_u16 taskId,_u32 event)
{
    if(!logRbInput.isEmpty(&logRbInput))
    {
        _u32 dataLen = 0;
        _u8* data = logRbInput.getReadPtr(&logRbInput);
        STREAM_TO_U32(dataLen,data);
        if(logRxCb)
        {
            logRxCb(data,dataLen);
        }
        logRbInput.moveReadPtr(&logRbInput);
    }
    return (event^LOG_TASK_EVENT_RX);
}
static _u32 log_task_input_event_message(_u16 taskId,_u32 event)
{
    _u8* messsage = tx_message_receive(taskId);
    if(messsage)
    {
        LOG_TRACE(1,"log rx event",messsage,4);
    }
    while(messsage!=NULL)
    {
        messsage = tx_message_receive(taskId);
        if(messsage)
        {
            LOG_TRACE(1,"log rx event",messsage,4);
        }
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
    tx_rb_init(&logRbInput,LOG_INPUT_BUFFER_SIZE,LOG_INPUT_BUFFER_NUMBER);
    _u8* pReveive = logRbInput.getReadPtr(&logRbInput);
    hal_uart_set_receive_buffer(pReveive+4,LOG_INPUT_BUFFER_SIZE-4);
} 

/**************************log output process*******************************/
tx_rb_t       logOutputRb;

_RAM_CODE static int log_status_operation(_u8 operation)
{

    static _u8 status = 0;
    if(operation == LOG_SET_STATUS)
    {
    	status |= LOG_STATUS_BUSY;
    }
    else if(operation == LOG_GET_STATUS)
    {
        return status;
    }
    else if(operation == LOG_CLEAR_STATUS)
    {
    	status &= (~LOG_STATUS_BUSY);
    }
    return 0;
}

_RAM_CODE static void log_hardware_tx_irq(void)
{
    log_status_operation(LOG_CLEAR_STATUS);
    if(!logOutputRb.isEmpty(&logOutputRb))
    {
		tx_task_set_event(TX_TASK_ID_LOG_TX,LOG_TASK_EVENT_TX);
    }
}

void log_output(_u8* pString,_u8* pData,_u32 dataLen)
{
    if(logOutputRb.p == NULL||logOutputRb.isFull(&logOutputRb))
    {
        return;
    }
    _u8* pLog = logOutputRb.getWritePtr(&logOutputRb);
    _u32 calStrLen = txStringLength(pString);
    _u32 calDataLen = dataLen;
    _u32 totalLen = 0;

    if(calStrLen+6 >LOG_OUTPUT_BUFFER_SIZE)
    {
        calStrLen = LOG_OUTPUT_BUFFER_SIZE - 6;
        calDataLen = 0;
    }
    else if(calStrLen+3*calDataLen+6>LOG_OUTPUT_BUFFER_SIZE)
    {
        calDataLen = (LOG_OUTPUT_BUFFER_SIZE - calStrLen - 6)/3;
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

    logOutputRb.moveWritePtr(&logOutputRb);
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
    if(!(logOutputRb.isEmpty(&logOutputRb))&&(!log_status_operation(LOG_GET_STATUS)))
    {
        _u32 dataLen = 0;
        _u8* pData = logOutputRb.getReadPtr(&logOutputRb);
        STREAM_TO_U32(dataLen,pData);
        hal_uart_send_data(pData,dataLen);
        logOutputRb.moveReadPtr(&logOutputRb);
        log_status_operation(LOG_SET_STATUS);
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
    tx_rb_init(&logOutputRb,LOG_OUTPUT_BUFFER_SIZE,LOG_OUTPUT_BUFFER_NUMBER);
}

/***************************************log init**********************************************/
void log_task_init(void)
{
	hal_uart_register_task(HAL_UART_BAUDRATE_1000000,log_hardware_rx_irq,log_hardware_tx_irq,HAL_UART_PARITY_NONE,HAL_UART_STOP_BITE_ONE);
    tx_task_add(log_output_init,log_task_output_event_process,TX_TASK_ID_LOG_TX,TX_TASK_PRIORITY_0);
    tx_task_add(log_input_init,log_task_input_event_process,TX_TASK_ID_LOG_RX,TX_TASK_PRIORITY_0);
}
#if(TX_DEBUG_LOG_ENABLE)
TASK_INIT(log_task_init);
#endif
