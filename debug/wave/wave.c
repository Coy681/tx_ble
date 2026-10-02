/*
 * wave.c
 *
 *  Created on: 2026年10月2日
 *      Author: Admin
 *
 *  Software logic analyzer: packs [timestamp][id][value] records and
 *  hands them to debug_port. All formatting/streaming lives here,
 *  callers never touch buffers directly.
 */

#include "wave.h"
#include "debug/debug_port.h"
#include "platform/platform.h"

#if(TX_DEBUG_WAVE_ENABLE)

/******************wave record format*************************/
/* payload layout, little endian, 12 bytes:
 * [0..3]  u32 timestamp
 * [4..7]  u32 event id = [module(8)|sub(8)|event(8)|rsvd]
 * [8..11] u32 value
 */
#define WAVE_RECORD_SIZE        12

#define WAVE_ID_OFFSET          4
#define WAVE_VALUE_OFFSET       8

/******************wave internal******************************/
static void wave_pack(_u8 *buf, _u32 timestamp, _u32 id, _u32 value)
{
    U32_TO_STREAM(buf, timestamp);
    U32_TO_STREAM(buf + WAVE_ID_OFFSET, id);
    U32_TO_STREAM(buf + WAVE_VALUE_OFFSET, value);
}

void wave_event_at(_u16 id, _u32 timestamp, _u32 value)
{
    _u8 buf[WAVE_RECORD_SIZE];
    wave_pack(buf, timestamp, id, value);
    debug_port_write(buf, WAVE_RECORD_SIZE);
}

void wave_event(_u16 id, _u32 value)
{
    wave_event_at(id, system_time(), value);
}

void wave_mark(_u16 id)
{
    wave_event(id, 0);
}

#endif//TX_DEBUG_WAVE_ENABLE
