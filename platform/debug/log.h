/*
 * trace.h
 *
 *  Created on: 2024年12月6日
 *      Author: Admin
 */

#ifndef DEBUG_LOG_H_
#define DEBUG_LOG_H_

typedef enum
{   
	LOG_HARDWARE_NONE   = 0,
	LOG_HARDWARE_UART   = 1,
	LOG_HARDWARE_USB    = 2,
}log_hardware_e;

#define LOG_OUTPUT_BUFFER_NUMBER       16
#define LOG_OUTPUT_BUFFER_SIZE         50

#define LOG_INPUT_BUFFER_NUMBER        4
#define LOG_INPUT_BUFFER_SIZE          50

typedef void(*log_input_f)(_u8* data,_u32 dataLen);

void log_init(log_mode_e traceMode,log_input_f inputCb);

#endif /* DEBUG_LOG_H_ */
