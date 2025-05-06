/*
 * trace.h
 *
 *  Created on: 2024年12月6日
 *      Author: Admin
 */

#ifndef DEBUG_LOG_H_
#define DEBUG_LOG_H_
#include "common/txCommon.h"
#include "config.h"
/******************define log input and output buffer*********/
#define LOG_OUTPUT_BUFFER_NUMBER       16
#define LOG_OUTPUT_BUFFER_SIZE         240

#define LOG_INPUT_BUFFER_NUMBER        4
#define LOG_INPUT_BUFFER_SIZE          240

/******************define log event type**********************/
#define LOG_TASK_EVENT_TX               BIT(0)
#define LOG_TASK_EVENT_RX				BIT(0)

/******************define log rx callback function typ********/
typedef void(*log_receive_cb_f)(_u8* data,_u32 dataLen);

/******************define log rx function*********************/

/**
 * @brief     This function is used to register log rx callback form hardware.
 * @param[in] cb - log rx callback from upper layer.
 * @return    none
 */
void log_register_rx_callback(log_receive_cb_f cb);

/**
 * @brief     This function is used to output log.
 * @param[in] pString - data description.
 * @param[in] pData   - data need to output.
 * @param[in] dataLen - data length.
 * @return    none
 */
void log_output(_u8* pString,_u8* pData,_u32 dataLen);

/******************define log trace***************************/
#if(TX_DEBUG_LOG_ENABLE)
#define LOG_TRACE(EN,STR,DATA,LEN)     if(EN){log_output((_u8*)STR,(_u8*)DATA,(_u32)LEN);};
#else
#define LOG_TRACE(EN,STR,DATA,LEN)      
#endif
#endif /* DEBUG_LOG_H_ */
