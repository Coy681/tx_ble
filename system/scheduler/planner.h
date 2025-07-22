#include"tx_common.h"
#include"platform/platform.h"
#define PLANNER_UNIT_US            625

#define PLANNER_MODE_CONCENTRATED  0
#define PLANNER_MODE_SCATTERED     1

typedef enum
{
	PLANNER_SUCCESS,
	PLANNER_BW_FULL,
	PLANNER_BW_CONFLICT,
	PLANNER_NOT_FOUND,
}planner_ret_e;

typedef enum
{
    PLANNER_0MS     = 0,
    PLANNER_1P25MS  = 2,
    PLANNER_2P5MS   = 4,
    PLANNER_3P75MS  = 6,
    PLANNER_5MS     = 8,
    PLANNER_6P25MS  = 10,
    PLANNER_7P5MS   = 12,
    PLANNER_8P75MS  = 14,
    PLANNER_10MS    = 16,
    PLANNER_12P5MS  = 20,
    PLANNER_15MS    = 24,
    PLANNER_17P5MS  = 28,
    PLANNER_20MS    = 32,
    PLANNER_22P5MS  = 36,
    PLANNER_25MS    = 40,
    PLANNER_27P5MS  = 44,
    PLANNER_30MS    = 48,
    PLANNER_35MS    = 56,
    PLANNER_40MS    = 64,
    PLANNER_45MS    = 72,
    PLANNER_50MS    = 80,
}planner_time_e;

typedef void(*planner_cb_f)(_u16);
typedef struct planner_node_t planner_node_t;
typedef struct __attribute__((packed)) planner_node_t
{
    _u8  id;
    _u8  offset;//*PLANNER_UNIT_US
    _u8  shift; //*gPlannerCtrl.offsetMax
    _u8  rsvd;
    _u32 interval;//us
    _u16 duration;//*PLANNER_UNIT_US
    planner_cb_f    cb;
    planner_node_t* next;
}planner_node_t;

typedef struct __attribute__((packed))
{
    _u16 baseInterval;//us
    _u16 offsetMax;//unit is PLANNER_UNIT_US
    _u32 plannerStartTime; //us
}planner_ctrl_t;
extern planner_ctrl_t gPlannerCtrl;

void              planner_set_base_interval(planner_time_e baseInterval);
planner_ret_e     planner_insert_node(planner_node_t* pNode);
planner_ret_e     planner_delete_node(planner_node_t* pNode);
planner_ret_e     planner_parameter_check(planner_node_t* pNode);
planner_ret_e     planner_parameter_request(_u8 mode,_u32 interval,_u16 durationMin,_u16 durationMax,planner_node_t* pNode);
void              planner_update_map_point_point(void);
_u32              planner_get_ahchor_point(planner_node_t* pNode,_u32 refTime);