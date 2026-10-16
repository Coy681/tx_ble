/*
 * wave_id.h
 *
 *  Created on: 2026年10月2日
 *      Author: Admin
 *
 *  32-bit id = [type(1) | module(7) | feature(8) | evt(8) | instance(4) | rsvd(4)].
 *                bit31    bit30-24    bit23-16     bit15-8     bit7-4     bit3-0
 *
 *  High 24 bits (type/module/feature/evt) are the compile-time static
 *  identity of a track; low 8 bits (instance/rsvd) are runtime fields
 *  and can be masked off with 0xFF.
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
 *  - evt: event lane inside a feature (compile-time, 0xFF = feature
 *    level probe). Sits next to feature so (feature<<16 | evt<<8)
 *    packs contiguously as the static track identity.
 *  - instance: runtime object index (conn handle, adv set handle,
 *    sch map id...), a plain runtime number, never a compile-time
 *    enum. 4 bits = 16 lanes per feature, enough for concurrent
 *    objects (few ACLs / adv sets); expand into rsvd if ever needed.
 *  - rsvd(4): reserved. Expansion buffer for evt or instance; grows
 *    without creating holes in existing enum values.
 *
 *  Host tool greps the same X-macro lists to build the track tree:
 *  track = module > feature > instance. Unknown ids fall back to
 *  numeric display, so host and firmware never need synchronized
 *  releases.
 */

#ifndef DEBUG_WAVE_ID_H_
#define DEBUG_WAVE_ID_H_

/******************id layout**********************************/
#define WAVE_ID_EVT_SHIFT       8
#define WAVE_ID_INSTANCE_SHIFT  4
#define WAVE_ID_FEATURE_SHIFT   16
#define WAVE_ID_MODULE_SHIFT    24

#define WAVE_ID_TYPE_MASK       (0x01UL << 31)
#define WAVE_ID_MODULE_MASK     0x7F
#define WAVE_ID_FEATURE_MASK    0xFF
#define WAVE_ID_EVT_MASK        0xFF
#define WAVE_ID_INSTANCE_MASK   0x0F
#define WAVE_ID_RSVD_MASK       0x0F
/* record type */

#define WAVE_TYPE_NUM           0x00    /* numeric sample: value is data */
#define WAVE_TYPE_LEVEL         (0x01UL << 31)  /* edge: value 1=begin 0=end */

/* WAVE_TYPE(t) selects edge (level) vs sample (num) at pack time */
#define WAVE_TYPE(id, t)        ((id) | (t))
/* decode helpers for host side */
#define WAVE_ID_TYPEOF(id)      ((id) & WAVE_ID_TYPE_MASK)
#define WAVE_ID_MODULE(id)      (((id) >> WAVE_ID_MODULE_SHIFT) & WAVE_ID_MODULE_MASK)
#define WAVE_ID_FEATURE(id)     (((id) >> WAVE_ID_FEATURE_SHIFT) & WAVE_ID_FEATURE_MASK)
#define WAVE_ID_EVT(id)         (((id) >> WAVE_ID_EVT_SHIFT) & WAVE_ID_EVT_MASK)
#define WAVE_ID_INSTANCE(id)    (((id) >> WAVE_ID_INSTANCE_SHIFT) & WAVE_ID_INSTANCE_MASK)

/* runtime instance numbers (never enums): fixed lanes so all
 * instances of one feature line up on the same host tracks
 * (4-bit field: 0 ~ 15) */
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
#define WAVE_PASTE4(a, b, c, d)     WAVE_PASTE4_I(a, b, c, d)
#define WAVE_PASTE4_I(a, b, c, d)   a##b##c##d

/******************id build (paste-based)*********************/
/* WAVE_ID(mod, feat, evt): pastes to WAVE_FEAT_<mod>_<feat>, whose enum
 * value already carries [module<<24 | feature<<16] (see WAVE_FEAT_ENUM),
 * then ORs the event lane at bit15-8. */
#define WAVE_ID(mod, feat, evt)     \
    (WAVE_PASTE(WAVE_FEAT_, WAVE_PASTE3(mod, _, feat)) | \
     (((evt) & WAVE_ID_EVT_MASK) << WAVE_ID_EVT_SHIFT))
/* argument order mirrors the id layout: evt (higher bits) before inst */
#define WAVE_ID_I(mod, feat, evt, inst) \
    (WAVE_ID(mod, feat, evt) | \
     (((inst) & WAVE_ID_INSTANCE_MASK) << WAVE_ID_INSTANCE_SHIFT))

#define WAVE_EVT_ID(mod, feat, evt)     \
    (WAVE_PASTE4(WAVE_EVT_, WAVE_PASTE3(mod, _, feat), _, evt))
/* evt token variant with runtime instance; token enum value already
 * carries [module<<24 | feature<<16 | evt<<8] (see WAVE_EVT_ENUM). */
#define WAVE_EVT_ID_I(mod, feat, evt, inst) \
    (WAVE_EVT_ID(mod, feat, evt) | \
     (((inst) & WAVE_ID_INSTANCE_MASK) << WAVE_ID_INSTANCE_SHIFT))

/* feature-0 token: for module/feature level probes without a feature
 * enum (e.g. module alive heartbeat). feat value 0 is reserved. */
#define WAVE_FEAT_NONE          0x00

/* module-level num probe (no feature split needed): build
 * [module<<24 | 0<<16] from the module registry directly */
#define WAVE_MOD_ID(mod, inst)  \
    ((WAVE_PASTE(WAVE_MOD_, mod) << WAVE_ID_MODULE_SHIFT) | \
     (((inst) & WAVE_ID_INSTANCE_MASK) << WAVE_ID_INSTANCE_SHIFT))

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

/******************evt definition (paste-based, per feature)***/
/* Feature headers declare their own evt list, one table per feature
 * ("who uses, who defines"), e.g. in sch.h:
 *   #define WAVE_SCHED_STIMER_EVTS(X) \
 *       X(SCHED, STIMER, WINDOW, 0x01)
 *   WAVE_EVT_ENUM(WAVE_SCHED_STIMER_EVTS)
 * Generates enum tokens WAVE_EVT_<MOD>_<FEAT>_<EVT> whose value carries
 * [module<<24 | feature<<16 | evt<<8]. The EVT token doubles as the
 * host-side display name (lowercased by the PC tool), so one column
 * serves both firmware and host. Values are explicit, one line per
 * evt, review-friendly. evt 0xFF stays reserved for probes. */
#define WAVE_EVT_ENUM_ITEM(mod, feat, evt, value) \
    WAVE_PASTE4(WAVE_EVT_, WAVE_PASTE3(mod, _, feat), _, evt) = \
        (WAVE_PASTE(WAVE_FEAT_, WAVE_PASTE3(mod, _, feat)) | \
         ((value) << WAVE_ID_EVT_SHIFT)),
/* unnamed enum: no typedef, multiple features can each call it */
#define WAVE_EVT_ENUM(list)     enum { list(WAVE_EVT_ENUM_ITEM) };

#endif /* DEBUG_WAVE_ID_H_ */
