/*
 * uart.h
 *
 *  Created on: 2024年12月6日
 *      Author: Admin
 */

#ifndef HAL_UART_H_
#define HAL_UART_H_
#include "../../common/txCommon.h"
typedef enum
{
	HAL_UART_BAUDRATE_9600     = 9600,
	HAL_UART_BAUDRATE_19200    = 19200,
	HAL_UART_BAUDRATE_38400    = 38400,
	HAL_UART_BAUDRATE_115200   = 115200,
	HAL_UART_BAUDRATE_1000000  = 1000000,
	HAL_UART_BAUDRATE_1500000  = 1500000,
	HAL_UART_BAUDRATE_2000000  = 2000000,
	HAL_UART_BAUDRATE_3000000  = 3000000,
}hal_uart_baudrate_e;

typedef enum
{
	HAL_UART_PARITY_NONW       = UART_PARITY_NONE,
	HAL_UART_PARITY_EVEN       = UART_PARITY_EVEN,
	HAL_UART_PARITY_ODD        = UART_PARITY_ODD,
}hal_uart_parity_e;

typedef enum
{
	HAL_UART_STOP_BITE_ONE     = UART_STOP_BIT_ONE,
	HAL_UART_STOP_BITE_TWO     = UART_STOP_BIT_TWO,
}hal_uart_stopBit_e;

typedef void (*hal_uart_rx_task)(void);
typedef void (*hal_uart_tx_task)(void);

void hal_uart_register_task(hal_uart_baudrate_e baudrate,\
		                       hal_uart_rx_task rxCb,\
							   hal_uart_tx_task txCb,\
							   hal_uart_parity_e patiry,\
							   hal_uart_stopBit_e stopBit);


void hal_uart_send_data(_u8 *data,_u32 length);

void hal_uart_set_receive_buffer(_u8 *buffer,_u32 length);

#endif /* HAL_UART_H_ */
