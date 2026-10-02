/*
 * debug.h
 *
 *  Created on: 2026年10月2日
 *      Author: Admin
 *
 *  Debug subsystem top header.
 *  Submodules: port / log / gpio / wave
 */

#ifndef DEBUG_H_
#define DEBUG_H_

/* master switch: turning TX_DEBUG_ENABLE off kills all submodules */
#ifndef TX_DEBUG_ENABLE
#define TX_DEBUG_ENABLE         1
#endif

#if(TX_DEBUG_ENABLE)
#ifndef TX_DEBUG_LOG_ENABLE
#define TX_DEBUG_LOG_ENABLE     1
#endif//TX_DEBUG_LOG_ENABLE
#ifndef TX_DEBUG_GPIO_ENABLE
#define TX_DEBUG_GPIO_ENABLE    1
#endif//TX_DEBUG_GPIO_ENABLE
#ifndef TX_DEBUG_WAVE_ENABLE
#define TX_DEBUG_WAVE_ENABLE    1
#endif//TX_DEBUG_WAVE_ENABLE
#else//TX_DEBUG_ENABLE
#define TX_DEBUG_LOG_ENABLE     0
#define TX_DEBUG_GPIO_ENABLE    0
#define TX_DEBUG_WAVE_ENABLE    0
#endif//TX_DEBUG_ENABLE

#include "debug_port.h"
#include "log/log.h"
#include "gpio/gpio.h"
#include "wave/wave.h"

#endif /* DEBUG_H_ */
