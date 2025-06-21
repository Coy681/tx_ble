#include"tx_common.h"

#define TLK_PLANNER_UNIT_US            625

#define TLK_PLANNER_MODE_CONCENTRATED  0
#define TLK_PLANNER_MODE_SCATTERED     1

typedef enum
{
	TLK_PLANNER_SUCCESS,
	TLK_PLANNER_BW_FULL,
	TLK_PLANNER_BW_CONFLICT,
	TLK_PLANNER_NOT_FOUND,
}tlk_planner_ret_e;

typedef enum
{
    TLK_PLANNER_0MS     = 0,
    TLK_PLANNER_1P25MS  = 2,
    TLK_PLANNER_2P5MS   = 4,
    TLK_PLANNER_3P75MS  = 6,
    TLK_PLANNER_5MS     = 8,
    TLK_PLANNER_6P25MS  = 10,
    TLK_PLANNER_7P5MS   = 12,
    TLK_PLANNER_8P75MS  = 14,
    TLK_PLANNER_10MS    = 16,
    TLK_PLANNER_12P5MS  = 20,
    TLK_PLANNER_15MS    = 24,
    TLK_PLANNER_17P5MS  = 28,
    TLK_PLANNER_20MS    = 32,
    TLK_PLANNER_22P5MS  = 36,
    TLK_PLANNER_25MS    = 40,
    TLK_PLANNER_27P5MS  = 44,
    TLK_PLANNER_30MS    = 48,
    TLK_PLANNER_35MS    = 56,
    TLK_PLANNER_40MS    = 64,
    TLK_PLANNER_45MS    = 72,
    TLK_PLANNER_50MS    = 80,
}tlk_planner_time_e;

typedef void(*tlk_planner_cb_f)(_u16);
typedef struct tlk_planner_node_t tlk_planner_node_t;
typedef struct __attribute__((packed)) tlk_planner_node_t
{
    _u8  id;
    _u8  offset;//*TLK_PLANNER_UNIT_US
    _u8  shift; //*gPlannerCtrl.offsetMax
    _u8  rsvd;
    _u32 interval;//us
    _u16 duration;//*TLK_PLANNER_UNIT_US
    tlk_planner_cb_f    cb;
    tlk_planner_node_t* next;
}tlk_planner_node_t;

typedef struct __attribute__((packed))
{
    _u16 baseInterval;
    _u16 offsetMax;
    _u32 plannerStartTick; 
}tlk_planner_ctrl_t;
extern tlk_planner_ctrl_t gPlannerCtrl;

void              tlk_planner_set_base_interval(tlk_planner_time_e baseInterval);
tlk_planner_ret_e tlk_planner_insert_node(tlk_planner_node_t* pNode);
tlk_planner_ret_e tlk_planner_delete_node(tlk_planner_node_t* pNode);
tlk_planner_ret_e tlk_planner_parameter_check(tlk_planner_node_t* pNode);
tlk_planner_ret_e tlk_planner_parameter_request(_u8 mode,_u32 interval,_u16 durationMin,_u16 durationMax,tlk_planner_node_t* pNode);
void              tlk_planner_update_map_point_tick(void);
_u32               tlk_planner_get_ahchor_tick(tlk_planner_node_t* pNode,_u32 refTick);

