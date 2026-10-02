/*
 * wave_id.h
 *
 *  Created on: 2026年10月2日
 *      Author: Admin
 *
 *  Wave id space (absorbed from "Firmware Log可视化升级方案"):
 *  32-bit id = [type(1) | module(7) | feature(8) | instance(8) | rsvd(8)].
 *
 *  record type (bit31):
 *      LEVEL - begin/end edge of a state or task. value: 1=begin 0=end.
 *      NUM   - one sample point, value is the plotted number.
 *  A pair of LEVEL edges on one track draws a gate in the host tool;
 *  NUM samples draw a numeric/analog curve. Marker probes are
 *  NUM records with value 0.
 *
 *  Hierarchy ("who uses, who defines"), 3 fixed levels + instance:
 *  - module: registry lives here (X-macro, one entry per subsystem).
 *  - feature: defined in each module's own header via WAVE_SUB_ENUM -
 *    e.g. adv defines LEGACY / EXT / PERIODIC (former "sub").
 *  - instance: runtime object index (conn handle, adv set handle,
 *    sch map id...), a plain runtime number, never a compile-time enum.
 *  - rsvd(8): reserved 4th level. AML practice shows instance covers
 *    the "multi-object" dimension; rsvd stays for future split.
 *
 *  Host tool greps the same X-macro lists to build the track tree:
 *  track = module > feature > instance. Unknown ids fall back to
 *  numeric display, so host and firmware never need synchronized
 *  releases.
 */

#ifndef DEBUG_WAVE_ID_H_
#define DEBUG_WAVE_ID_H_

/******************id layout**********************************/
#define WAVE_ID_INSTANCE_SHIFT  8
#define WAVE_ID_FEATURE_SHIFT   16
#define WAVE_ID_MODULE_SHIFT    24

#define WAVE_ID_TYPE_MASK       (0x01UL << 31)
#define WAVE_ID_MODULE_MASK     0x7F
#define WAVE_ID_FEATURE_MASK    0xFF
#define WAVE_ID_INSTANCE_MASK   0xFF

/* record type */
#define WAVE_TYPE_NUM           0x00    /* numeric sample: value is data */
#define WAVE_TYPE_LEVEL         (0x01UL << 31)  /* edge: value 1=begin 0=end */

/* WAVE_TYPE(t) selects edge (level) vs sample (num) at pack time */
#define WAVE_TYPE(id, t)        ((id) | (t))
/* decode helpers for host side */
#define WAVE_ID_TYPEOF(id)      ((id) & WAVE_ID_TYPE_MASK)
#define WAVE_ID_MODULE(id)      (((id) >> WAVE_ID_MODULE_SHIFT) & WAVE_ID_MODULE_MASK)
#define WAVE_ID_FEATURE(id)     (((id) >> WAVE_ID_FEATURE_SHIFT) & WAVE_ID_FEATURE_MASK)
#define WAVE_ID_INSTANCE(id)    (((id) >> WAVE_ID_INSTANCE_SHIFT) & WAVE_ID_INSTANCE_MASK)

/* runtime instance numbers (never enums): fixed lanes so all
 * instances of one feature line up on the same host tracks */
#define WAVE_INST_0             0x00
#define WAVE_INST_1             0x01
#define WAVE_INST_2             0x02
#define WAVE_INST_3             0x03

/******************paste helpers******************************/
/* two-level expansion so macro arguments expand before pasting */
#define WAVE_PASTE(a, b)            WAVE_PASTE_I(a, b)
#define WAVE_PASTE_I(a, b)          a##b
#define WAVE_PASTE3(a, b, c)        WAVE_PASTE3_I(a, b, c)
#define WAVE_PASTE3_I(a, b, c)      a##b##c

/******************id build (paste-based)*********************/
/* WAVE_ID(mod, feat): pastes to WAVE_MOD_<mod> and
 * WAVE_FEAT_<mod>_<feat> enum tokens, builds [module|feature|inst 0].
 * Instance is OR'ed in by the WAVE_*() call macros (runtime value). */
#define WAVE_ID(mod, feat)      \
    ((WAVE_PASTE(WAVE_MOD_, mod) << WAVE_ID_MODULE_SHIFT) | \
     (WAVE_PASTE(WAVE_FEAT_, WAVE_PASTE3(mod, _, feat)) << WAVE_ID_FEATURE_SHIFT))
#define WAVE_ID_I(mod, feat, inst)  (WAVE_ID(mod, feat) | ((inst) & WAVE_ID_INSTANCE_MASK))

/* feature-0 token: for module/feature level probes without a feature
 * enum (e.g. module alive heartbeat). feat value 0 is reserved. */
#define WAVE_FEAT_NONE          0x00

/* module-level num probe (no feature split needed) */
#define WAVE_MOD_ID(mod, inst)  (WAVE_ID(mod, WAVE_FEAT_NONE) | ((inst) & WAVE_ID_INSTANCE_MASK))

/******************module registry (X-macro)******************/
/* X(short, value, host_label); short name feeds the pasting above */
#define WAVE_MODULE_LIST(X)         \
    X(SCHED,    0x01, "scheduler")  \
    X(TASK,     0x02, "task")       \
    X(ADV,      0x03, "adv")        \
    X(CONN,     0x04, "conn")       \
    X(SCAN,     0x05, "scan")       \
    X(INIT,     0x06, "init")       \
    X(SYNC,     0x07, "sync")       \
    X(HCI,      0x08, "hci")        \
    X(RF,       0x09, "rf")         \
    X(DEBUG,    0x0A, "debug")      \
    X(APP,      0x0B, "app")

#define X_WAVE_ENUM(short, value, label)    WAVE_PASTE(WAVE_MOD_, short) = value,
typedef enum
{
    WAVE_MODULE_LIST(X_WAVE_ENUM)
    WAVE_MOD_MAX
} wave_module_e;
#undef X_WAVE_ENUM

/******************feature definition (paste-based, per module)*/
/* Module headers declare their own list, e.g.:
 *   #define WAVE_ADV_FEATS(X)  X(ADV, LEGACY, 0x01) X(ADV, EXT, 0x02)
 *   WAVE_FEAT_ENUM(WAVE_ADV_FEATS)
 * Generates enum tokens WAVE_FEAT_<MOD>_<FEAT> = (module<<24)|(feat<<16).
 * Values are explicit (1 ~ 255), one line per feature, review-friendly. */
#define WAVE_FEAT_ENUM_ITEM(mod, feat, value) \
    WAVE_PASTE(WAVE_FEAT_, WAVE_PASTE3(mod, _, feat)) = \
        ((WAVE_PASTE(WAVE_MOD_, mod)) << WAVE_ID_MODULE_SHIFT) | ((value) << WAVE_ID_FEATURE_SHIFT),
/* unnamed enum: no typedef, so multiple modules can each call WAVE_FEAT_ENUM */
#define WAVE_FEAT_ENUM(list)    enum { list(WAVE_FEAT_ENUM_ITEM) };

/* features owned by debug/ itself (scheduler/task track group) */
#define WAVE_CORE_FEATS(X)          \
    X(SCHED, TASK,  0x01)           \
    X(SCHED, MSG,   0x02)           \
    X(TASK,  TX,    0x01)           \
    X(TASK,  RX,    0x02)
WAVE_FEAT_ENUM(WAVE_CORE_FEATS)

#endif /* DEBUG_WAVE_ID_H_ */
