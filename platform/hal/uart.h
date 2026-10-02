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

/* abstract values, chip-independent. Chip mapping is done in hal implementation. */
typedef enum
{
	HAL_UART_PARITY_NONE       = 0,
	HAL_UART_PARITY_EVEN,
	HAL_UART_PARITY_ODD,
}hal_uart_parity_e;

typedef enum
{
	HAL_UART_STOP_BIT_ONE      = 0,
	HAL_UART_STOP_BIT_TWO,
}hal_uart_stopbit_e;

typedef void (*hal_uart_rx_task)(int len);
typedef void (*hal_uart_tx_task)(void);

/**
 * @brief     This function is used to init uart and register rx/tx irq callbacks.
 * @param[in] baudrate - uart baudrate.
 * @param[in] rxCb - callback invoked in uart irq when rx dma done.
 * @param[in] txCb - callback invoked in uart irq when tx done.
 * @param[in] parity - parity mode.
 * @param[in] stopBit - stop bit length.
 * @return    none
 */
void hal_uart_register_task(hal_uart_baudrate_e baudrate,\
		                       hal_uart_rx_task rxCb,\
							   hal_uart_tx_task txCb,\
							   hal_uart_parity_e parity,\
							   hal_uart_stopbit_e stopBit);

/**
 * @brief     This function is used to send data by dma asynchronously.
 * @param[in] data - data to send.
 * @param[in] length - data length in bytes.
 * @return    none
 */
void hal_uart_send_data(_u8 *data,_u32 length);

/**
 * @brief     This function is used to arm rx dma with caller-owned buffer.
 * @param[in] buffer - receive buffer.
 * @param[in] length - buffer length in bytes.
 * @return    none
 */
void hal_uart_set_receive_buffer(_u8 *buffer,_u32 length);

#endif /* HAL_UART_H_ */
