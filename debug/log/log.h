/*
 * log.h
 *
 *  Created on: 2024年12月6日
 *      Author: Admin
 */

#ifndef DEBUG_LOG_H_
#define DEBUG_LOG_H_
#include "common/txCommon.h"
#include "config.h"

/******************define log output function*****************/

/**
 * @brief     This function is used to output a plain string.
 * @param[in] str - null terminated string.
 * @return    none
 */
void log_str(const char *str);

/**
 * @brief     This function is used to output data as hex with a prefix string.
 * @param[in] prefix - data description.
 * @param[in] data   - data need to output.
 * @param[in] len    - data length.
 * @return    none
 */
void log_hex(const char *prefix,const _u8 *data,_u32 len);

/******************define log macros**************************/
#if(TX_DEBUG_LOG_ENABLE)
#ifndef TX_DEBUG_LOG_ENABLE
#define TX_DEBUG_LOG_ENABLE     1
#endif
#define LOG_STR(EN,STR)             do { \
                                        if (EN) { log_str((const char *)(STR));} \
                                    } while (0);

#define LOG_HEX(EN,STR,DATA,LEN)    do { \
                                        if (EN) { log_hex((const char *)(STR),(const _u8 *)(DATA),(_u32)(LEN));} \
                                    } while (0);
#else
#define LOG_STR(EN,STR)
#define LOG_HEX(EN,STR,DATA,LEN)
#endif
#endif /* DEBUG_LOG_H_ */
