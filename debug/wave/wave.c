/*
 * wave.c
 *
 *  Created on: 2026年10月2日
 *      Author: Admin
 *
 *  Software logic analyzer: packs [timestamp][id][value] records and
 *  hands them to debug_port. All formatting/streaming lives here,
 *  callers never touch buffers directly. Record type (level/num)
 *  travels inside the id bit31; packing is type-agnostic.
 */

#include "wave.h"
#include "debug/debug_port.h"
#include "platform/platform.h"

#if(TX_DEBUG_WAVE_ENABLE)

/******************wave record format*************************/
/* payload layout, little endian, 12 bytes:
 * [0..3]  u32 timestamp
 * [4..7]  u32 id = [type(1)|module(7)|feature(8)|instance(8)|evt(8)]
 *         evt byte is 0xFF-passthrough for feature-level probes
 * [8..11] u32 value  (LEVEL: 1=begin/0=end; NUM: data)
 */
#define WAVE_RECORD_SIZE        12

#define WAVE_ID_OFFSET          4
#define WAVE_VALUE_OFFSET       8

/******************wave internal******************************/
_RAM_CODE
static void wave_pack(_u8 *buf, _u32 timestamp, _u32 id, _u32 value)
{
    _u8 *p = buf;
    U32_TO_STREAM(p, timestamp);
    U32_TO_STREAM(p, id);
    U32_TO_STREAM(p, value);
}

_RAM_CODE
void wave_event_at(_u32 id, _u32 timestamp, _u32 value)
{
    _u8 buf[WAVE_RECORD_SIZE];
    wave_pack(buf, timestamp, id, value);
    /* block type in the wire header routes this to the host wave view */
    debug_port_write_typed(DEBUG_BLK_WAVE, buf, WAVE_RECORD_SIZE);
}
_RAM_CODE
void wave_event(_u32 id, _u32 value)
{
    wave_event_at(id, system_time(), value);
}

#endif//TX_DEBUG_WAVE_ENABLE
