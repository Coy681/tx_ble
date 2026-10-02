/*
 * wave.h
 *
 *  Created on: 2026年10月2日
 *      Author: Admin
 *
 *  Software logic analyzer: structured event stream over debug_port.
 *  Each record: [u32 timestamp][u16 event_id][u32 value], little endian.
 *  Event id space is defined in wave_id.h: id = [module(8) | event(8)].
 *  Each module defines its own event values next to the emitting code;
 *  the module registry here builds the host-side track tree
 *  (track = module, sub-track = event).
 *  PC side plots per event_id as waveform tracks.
 *  Never use in uS-level ISR hot paths - gpio trace is for that.
 */

#ifndef DEBUG_WAVE_H_
#define DEBUG_WAVE_H_
#include "common/txCommon.h"
#include "config.h"
#include "wave_id.h"

#if(TX_DEBUG_WAVE_ENABLE)

/**
 * @brief     Emit a wave record with automatic timestamp (system_time()).
 *            Use in task context or non-critical code.
 * @param[in] id    - event channel id (0 ~ 65535), one track on PC side.
 * @param[in] value - event value to plot.
 * @return    none
 */
void wave_event(_u16 id, _u32 value);

/**
 * @brief     Emit a wave record with a manually supplied timestamp.
 *            Use when timestamp must come from another context, e.g. a
 *            value captured in an ISR and emitted later in task context.
 * @param[in] id    - event channel id (0 ~ 65535).
 * @param[in] timestamp - explicit timestamp, same unit as system_time().
 * @param[in] value - event value to plot.
 * @return    none
 */
void wave_event_at(_u16 id, _u32 timestamp, _u32 value);

/**
 * @brief     Emit a marker record with automatic timestamp.
 *            A zero-value, channel-tagged point: state transitions,
 *            "this code path was reached" probes.
 * @param[in] id - event channel id.
 * @return    none
 */
void wave_mark(_u16 id);

#define WAVE_EVENT(id, value)       wave_event((id), (value))
#define WAVE_EVENT_AT(id, ts, val)  wave_event_at((id), (ts), (val))
#define WAVE_MARK(id)               wave_mark((id))

#else
#define WAVE_EVENT(id, value)
#define WAVE_EVENT_AT(id, ts, val)
#define WAVE_MARK(id)
#endif//TX_DEBUG_WAVE_ENABLE

#endif /* DEBUG_WAVE_H_ */
