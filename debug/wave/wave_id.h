/*
 * wave_id.h
 *
 *  Created on: 2026年10月2日
 *      Author: Admin
 *
 *  Wave id space: 32-bit id = [module(8) | sub(8) | event(8)] + top byte reserved.
 *
 *  Hierarchy ("who uses, who defines"):
 *  - MODULE registry lives here (X-macro, one entry per subsystem).
 *  - SUB values are defined in each module's own header, using its
 *    module base: e.g. adv has WAVE_SUB_ADV_LEGACY / _EXT / _PERIODIC.
 *  - EVENT values likewise, per sub: event 0xFF means "sub-level marker".
 *  - Host tool parses these tables to build the track tree:
 *    track = module > sub-track = sub > lane = event.
 *    Unknown sub/event ids fall back to numeric display, so host and
 *    firmware never need synchronized releases.
 */

#ifndef DEBUG_WAVE_ID_H_
#define DEBUG_WAVE_ID_H_

/******************id layout**********************************/
#define WAVE_ID_SUB_SHIFT       8
#define WAVE_ID_MODULE_SHIFT    16
#define WAVE_ID_EVENT_MASK      0xFF
#define WAVE_ID_SUB_MASK        0xFF
#define WAVE_ID_MODULE_MASK     0xFF

#define WAVE_ID(module, sub, event)  \
    (((module) << WAVE_ID_MODULE_SHIFT) | ((sub) << WAVE_ID_SUB_SHIFT) | ((event) & WAVE_ID_EVENT_MASK))
#define WAVE_ID_MOD(id)     (((id) >> WAVE_ID_MODULE_SHIFT) & WAVE_ID_MODULE_MASK)
#define WAVE_ID_SUB(id)     (((id) >> WAVE_ID_SUB_SHIFT) & WAVE_ID_SUB_MASK)
#define WAVE_ID_EVT(id)     ((id) & WAVE_ID_EVENT_MASK)

/* sub-level marker: event byte unused, record describes the sub itself */
#define WAVE_EVT_MARKER     0xFF

/******************module registry (X-macro)******************/
/* X(name, value, host_label) */
#define WAVE_MODULE_LIST(X)         \
    X(WAVE_MOD_SCHED,   0x01, "scheduler")  \
    X(WAVE_MOD_TASK,    0x02, "task")       \
    X(WAVE_MOD_ADV,     0x03, "adv")        \
    X(WAVE_MOD_CONN,    0x04, "conn")       \
    X(WAVE_MOD_SCAN,    0x05, "scan")       \
    X(WAVE_MOD_INIT,    0x06, "init")       \
    X(WAVE_MOD_SYNC,    0x07, "sync")       \
    X(WAVE_MOD_HCI,     0x08, "hci")        \
    X(WAVE_MOD_RF,      0x09, "rf")         \
    X(WAVE_MOD_DEBUG,   0x0A, "debug")      \
    X(WAVE_MOD_APP,     0x0B, "app")

#define X_WAVE_ENUM(name, value, label)     name = value,
typedef enum
{
    WAVE_MODULE_LIST(X_WAVE_ENUM)
    WAVE_MOD_MAX
} wave_module_e;
#undef X_WAVE_ENUM

/******************sub registry (X-macro, per module)*********/
/* Sub ids are module-local (0 ~ 255). Each module owns its list in its
 * own header; the entries below are the ones debug/ itself needs.
 * X(name, module, value, host_label) */
#define WAVE_SUB_LIST(X)            \
    X(WAVE_SUB_SCHED_TASK,  WAVE_MOD_SCHED, 0x01, "sch_task")   \
    X(WAVE_SUB_SCHED_MSG,   WAVE_MOD_SCHED, 0x02, "sch_msg")    \
    X(WAVE_SUB_TASK_TX,     WAVE_MOD_TASK,  0x01, "task_tx")    \
    X(WAVE_SUB_TASK_RX,     WAVE_MOD_TASK,  0x02, "task_rx")

#define X_WAVE_SUB_ENUM(name, module, value, label)     name = ((module) << WAVE_ID_MODULE_SHIFT) | (value),
typedef enum
{
    WAVE_SUB_LIST(X_WAVE_SUB_ENUM)
} wave_sub_e;
#undef X_WAVE_SUB_ENUM

#endif /* DEBUG_WAVE_ID_H_ */
