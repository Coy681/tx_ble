/*
 * debug_port.h
 *
 *  Created on: 2026年10月2日
 *      Author: Admin
 *
 *  Debug transport layer: ring buffer + tx task + hardware link.
 *  log / wave write bytes here, never touch UART directly.
 */

#ifndef DEBUG_PORT_H_
#define DEBUG_PORT_H_
#include "common/txCommon.h"
#include "config.h"

/******************define port buffer*************************/
#define DEBUG_PORT_OUTPUT_BUFFER_NUMBER    32
#define DEBUG_PORT_OUTPUT_BUFFER_SIZE      240

#define DEBUG_PORT_INPUT_BUFFER_NUMBER     4
#define DEBUG_PORT_INPUT_BUFFER_SIZE       240

/******************define port rx callback********************/
typedef void(*debug_port_rx_cb_f)(_u8* data,_u32 dataLen);

/**
 * @brief     This function is used to register rx callback from upper layer.
 * @param[in] cb - callback invoked in debug task when rx data arrives.
 * @return    none
 */
void debug_port_rx_register(debug_port_rx_cb_f cb);

/******************block type*********************************/
/* wire frame (in-band, the only thing that reaches the host):
 * [0xA5 sync][type:1][len:1][payload...], len = payload bytes.
 * The 4-byte header inside the ring buffer is hardware-only and
 * never appears on the wire. */
#define PORT_FRAME_SYNC         0xA5U
#define PORT_FRAME_SIZE         3
#define DEBUG_BLK_LOG           0x0U    /* text payload (log.c) */
#define DEBUG_BLK_WAVE          0x1U    /* wave records, 12B each */

/**
 * @brief     This function is used to write data into port output buffer.
 *            Data is sent on uart by debug tx task asynchronously.
 * @param[in] data - data to send.
 * @param[in] len  - data length.
 * @return    none
 */
void debug_port_write(const _u8 *data,_u32 len);

/**
 * @brief     Typed variant of debug_port_write. The block type is put
 *            into the wire header (bits 30..31) so the host can route
 *            log text and wave records to different views.
 */
void debug_port_write_typed(_u32 type,const _u8 *data,_u32 len);

#endif /* DEBUG_PORT_H_ */
