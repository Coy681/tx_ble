/*
 * uart.c
 *
 *  Created on: 2024年12月6日
 *      Author: Admin
 */
#include"driver.h"
#include"../../../common/common.h"
#include"../../hal/uart.h"

hal_uart_rx_task hal_uart_rx_cb = NULL;
hal_uart_tx_task hal_uart_tx_cb = NULL;

static _u32 uartRxPin = GPIO_PC6;
static _u32 uartTxPin = GPIO_PC7;

static _u32 uartRxDma = DMA2;
static _u32 uartTxDma = DMA3;

_RAM_CODE void uart_irq_handler(void)
{
	if(uart_get_irq_status(UART0,UART_TXDONE_IRQ_STATUS))
	{
	    if(hal_uart_tx_cb)
	    {
	    	hal_uart_tx_cb();
	    }
		uart_clr_irq_status(UART0,UART_TXDONE_IRQ_STATUS);
	}
    if(uart_get_irq_status(UART0,UART_RXDONE_IRQ_STATUS))
	{
    	if(hal_uart_rx_cb)
    	{
    		hal_uart_rx_cb();
    	}
    	uart_clr_irq_status(UART0,UART_RXDONE_IRQ_STATUS);
	}
}
PLIC_ISR_REGISTER(uart_irq_handler, IRQ_UART0)

void hal_uart_register_task(hal_uart_baudrate_e baudrate,\
		                       hal_uart_rx_task rxCb,\
							   hal_uart_tx_task txCb,\
							   hal_uart_parity_e patiry,\
							   hal_uart_stopBit_e stopBit)
{
	uart_set_pin(UART0,uartTxPin,uartRxPin);
    _u16 div;
    _u8 bwpc;
	uart_cal_div_and_bwpc((_u32 )baudrate, sys_clk.pclk*1000*1000, &div, &bwpc);
    uart_set_rx_timeout(UART0, bwpc, 12, UART_BW_MUL2);
    uart_init(UART0, div, bwpc, (uart_parity_e)patiry, (uart_stop_bit_e)stopBit);
    uart_set_tx_dma_config(UART0, uartTxDma);
    uart_set_rx_dma_config(UART0, uartRxDma);
    uart_set_irq_mask(UART0, UART_TXDONE_MASK|UART_RXDONE_MASK);
    plic_interrupt_enable(IRQ_UART0);
    if(rxCb)
    {
    	hal_uart_rx_cb = rxCb;
    }
    if(txCb)
    {
    	hal_uart_tx_cb = txCb;
    }
}

void hal_uart_send_data(_u8 *data,_u32 length)
{
	uart_send_dma(UART0, data, length);
}
void hal_uart_set_receive_buffer(_u8 *buffer,_u32 length)
{
	uart_receive_dma(UART0, buffer, length);
}
