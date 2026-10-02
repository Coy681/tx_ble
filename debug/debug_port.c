/*
 * debug_port.c
 *
 *  Created on: 2026�?0�?�? *      Author: Admin
 *
 *  Debug transport layer: ring buffer + tx task + uart hardware link.
 *  Block format: [4-byte length][payload...], uart sends from byte 5.
 */

#include "debug_port.h"
#include "platform/platform.h"
#include "system/task/task.h"
#include "system/task/event/event.h"
#include "system/task/message/message.h"

/******************define port event type*********************/
#define PORT_TASK_EVENT_TX               BIT(0)
#define PORT_TASK_EVENT_RX               BIT(0)

#define PORT_STATUS_BUSY                 BIT(0)

#define PORT_GET_STATUS                  1
#define PORT_SET_STATUS                  2
#define PORT_CLEAR_STATUS                3

/**************************port input process*******************************/
tx_rb_t          portRbInput;
debug_port_rx_cb_f portRxCb;

void debug_port_rx_register(debug_port_rx_cb_f cb)
{
    portRxCb = cb;
}

_RAM_CODE static void port_hardware_rx_irq(int len)
{
    _u8* p = portRbInput.getWritePtr(&portRbInput);
    U32_TO_STREAM(p,len);
    portRbInput.moveWritePtr(&portRbInput);
    tx_task_set_event(TX_TASK_ID_LOG_RX,PORT_TASK_EVENT_RX);
    _u8* pReveive = portRbInput.getWritePtr(&portRbInput);
    hal_uart_set_receive_buffer(pReveive+4,DEBUG_PORT_INPUT_BUFFER_SIZE-4);
}

static _u32 port_task_event_rx(_u16 taskId,_u32 event)
{
    if(!portRbInput.isEmpty(&portRbInput))
    {
        _u32 dataLen = 0;
        _u8* data = portRbInput.getReadPtr(&portRbInput);
        STREAM_TO_U32(dataLen,data);
        if(portRxCb)
        {
            portRxCb(data,dataLen);
        }
        portRbInput.moveReadPtr(&portRbInput);
    }
    return (event^PORT_TASK_EVENT_RX);
}

static _u32 port_task_input_event_message(_u16 taskId,_u32 event)
{
    _u8* messsage = tx_message_receive(taskId);
    while(messsage!=NULL)
    {
        messsage = tx_message_receive(taskId);
    }
    return (event^TX_TASK_EVENT_MESSAGE);
}

static _u32 port_task_input_event_process(_u16 taskId,_u32 event)
{
    if(event&TX_TASK_EVENT_MESSAGE)
    {
        return port_task_input_event_message(taskId,event);
    }
    if(event&PORT_TASK_EVENT_RX)
    {
        return port_task_event_rx(taskId,event);
    }
    return 0;
}

static void port_input_init(void)
{
    tx_rb_init(&portRbInput,DEBUG_PORT_INPUT_BUFFER_SIZE,DEBUG_PORT_INPUT_BUFFER_NUMBER);
    _u8* pReveive = portRbInput.getReadPtr(&portRbInput);
    hal_uart_set_receive_buffer(pReveive+4,DEBUG_PORT_INPUT_BUFFER_SIZE-4);
}

/**************************port output process******************************/
tx_rb_t       portOutputRb;

_RAM_CODE static int port_status_operation(_u8 operation)
{
    static _u8 status = 0;
    if(operation == PORT_SET_STATUS)
    {
        status |= PORT_STATUS_BUSY;
    }
    else if(operation == PORT_GET_STATUS)
    {
        return status;
    }
    else if(operation == PORT_CLEAR_STATUS)
    {
        status &= (~PORT_STATUS_BUSY);
    }
    return 0;
}

_RAM_CODE static void port_hardware_tx_irq(void)
{
    port_status_operation(PORT_CLEAR_STATUS);
    if(!portOutputRb.isEmpty(&portOutputRb))
    {
        tx_task_set_event(TX_TASK_ID_LOG_TX,PORT_TASK_EVENT_TX);
    }
}

void debug_port_write(const _u8 *data,_u32 len)
{
    if(portOutputRb.p == NULL || data == NULL ||
       len == 0 || len > DEBUG_PORT_OUTPUT_BUFFER_SIZE - 4)
    {
        return;
    }
    if(portOutputRb.isFull(&portOutputRb))
    {
        return;
    }
    _u8* pLog = portOutputRb.getWritePtr(&portOutputRb);
    U32_TO_STREAM(pLog, len);
    _u8* buf = pLog;
    while(len--)
    {
        *buf++ = *data++;
    }
    portOutputRb.moveWritePtr(&portOutputRb);
    tx_task_set_event(TX_TASK_ID_LOG_TX,PORT_TASK_EVENT_TX);
}

static _u32 port_task_event_tx(_u16 taskId,_u32 event)
{
    if(!(portOutputRb.isEmpty(&portOutputRb))&&(!port_status_operation(PORT_GET_STATUS)))
    {
        _u32 dataLen = 0;
        _u8* pData = portOutputRb.getReadPtr(&portOutputRb);
        STREAM_TO_U32(dataLen,pData);
        /* STREAM_TO_U32 already advanced pData past the 4-byte length header */
        hal_uart_send_data(pData,dataLen);
        portOutputRb.moveReadPtr(&portOutputRb);
        port_status_operation(PORT_SET_STATUS);
    }
    return (event^PORT_TASK_EVENT_TX);
}

static _u32 port_task_output_event_message(_u16 taskId,_u32 event)
{
    _u8* messsage = tx_message_receive(taskId);
    while(messsage)
    {
        messsage = tx_message_receive(taskId);
    }
    return (event^TX_TASK_EVENT_MESSAGE);
}

static _u32 port_task_output_event_process(_u16 taskId,_u32 event)
{
    if(event&TX_TASK_EVENT_MESSAGE)
    {
        return port_task_output_event_message(taskId,event);
    }
    if(event&PORT_TASK_EVENT_TX)
    {
        return port_task_event_tx(taskId,event);
    }
    return 0;
}

void port_output_init(void)
{
    tx_rb_init(&portOutputRb,DEBUG_PORT_OUTPUT_BUFFER_SIZE,DEBUG_PORT_OUTPUT_BUFFER_NUMBER);
}

/***************************************port init**********************************************/
void port_task_init(void)
{
    hal_uart_register_task(HAL_UART_BAUDRATE_1000000,port_hardware_rx_irq,port_hardware_tx_irq,HAL_UART_PARITY_NONE,HAL_UART_STOP_BIT_ONE);
    tx_task_add(port_output_init,port_task_output_event_process,TX_TASK_ID_LOG_TX,TX_TASK_PRIORITY_0);
    tx_task_add(port_input_init,port_task_input_event_process,TX_TASK_ID_LOG_RX,TX_TASK_PRIORITY_0);
}
#if(TX_DEBUG_ENABLE)
TASK_INIT(port_task_init);
#endif
