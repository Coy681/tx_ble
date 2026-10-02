/*
 * wave.h
 *
 *  Created on: 2026年10月2日
 *      Author: Admin
 *
 *  Software logic analyzer: structured event stream over debug_port.
 *  Each record: [u32 timestamp][u32 event_id][u32 value], little endian.
 *
 *  Two record types (bit31 of id, absorbed from the AML wave-log spec):
 *  - LEVEL edge: value 1 = begin, 0 = end. A begin/end pair on the same
 *    track draws a gate (state window) in the host tool.
 *  - NUM sample: value is plotted as a number/analog curve. Markers are
 *    NUM records with value 0.
 *
 *  Call macros:
 *      WAVE_EVENT(mod, feat, evt, value)          numeric sample
 *      WAVE_LEVEL(mod, feat, inst, on)            begin/end edge
 *      WAVE_EVENT_I(mod, feat, inst, evt, value)  numeric with instance
 *      WAVE_EVENT_AT / WAVE_LEVEL_AT              manual timestamp
 *  evt is the low 8 bits (event lane inside a feature); inst is the
 *  runtime object number (conn handle, adv set handle, 0~255).
 *
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
 * @brief     Emit a numeric record with automatic timestamp (system_time()).
 *            Use in task context or non-critical code.
 * @param[in] id    - event channel id from WAVE_ID()/WAVE_ID_I().
 * @param[in] value - event value to plot.
 * @return    none
 */
void wave_event(_u32 id, _u32 value);

/**
 * @brief     Emit a record with a manually supplied timestamp.
 *            Use when timestamp must come from another context, e.g. a
 *            value captured in an ISR and emitted later in task context.
 * @param[in] id    - event channel id from WAVE_ID()/WAVE_ID_I().
 * @param[in] timestamp - explicit timestamp, same unit as system_time().
 * @param[in] value - event value to plot.
 * @return    none
 */
void wave_event_at(_u32 id, _u32 timestamp, _u32 value);

/******************call macros********************************/
/* numeric sample, no instance lane */
#define WAVE_EVENT(mod, feat, evt, value)           \
    wave_event(WAVE_TYPE(WAVE_ID_I(mod, feat, WAVE_INST_0), WAVE_TYPE_NUM) | (evt), (value))

/* numeric sample on a specific instance */
#define WAVE_EVENT_I(mod, feat, inst, evt, value)   \
    wave_event(WAVE_TYPE(WAVE_ID_I(mod, feat, inst), WAVE_TYPE_NUM) | (evt), (value))

/* level edge: on = 1 begin / 0 end. state is the lane (like evt byte).
 * A begin/end pair on the same (track, state) draws a gate. */
#define WAVE_LEVEL(mod, feat, inst, state, on)      \
    wave_event(WAVE_TYPE(WAVE_ID_I(mod, feat, inst), WAVE_TYPE_LEVEL) | (state), (on))

/* manual-timestamp variants */
#define WAVE_EVENT_AT(mod, feat, evt, ts, val)          \
    wave_event_at(WAVE_TYPE(WAVE_ID_I(mod, feat, WAVE_INST_0), WAVE_TYPE_NUM) | (evt), (ts), (val))
#define WAVE_LEVEL_AT(mod, feat, inst, state, on, ts)   \
    wave_event_at(WAVE_TYPE(WAVE_ID_I(mod, feat, inst), WAVE_TYPE_LEVEL) | (state), (ts), (on))

#else
#define WAVE_EVENT(mod, feat, evt, value)
#define WAVE_EVENT_I(mod, feat, inst, evt, value)
#define WAVE_LEVEL(mod, feat, inst, state, on)
#define WAVE_EVENT_AT(mod, feat, evt, ts, val)
#define WAVE_LEVEL_AT(mod, feat, inst, state, on, ts)
#endif//TX_DEBUG_WAVE_ENABLE

#endif /* DEBUG_WAVE_H_ */
