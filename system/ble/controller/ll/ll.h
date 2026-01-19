
 #ifndef LL_H_
 #define LL_H_

#include"module/adv/adv.h"
#include"module/scan/scan.h"
#include"module/conn/conn.h"
#include"module/init/init.h"
#include"module/sync/sync.h"
#include"module/brd/brd.h"

/***********************Bluetooth LE LL config **************************/


typedef struct
{
	int ppm;
}ll_device_info_t;
void ll_device_info_init(ll_device_info_t* info);

void ll_init_state_machine(_u8 number);

void ll_get_feature(_u8* feature,_u8 len);

controller_error_code_e ll_set_default_channel_table(_u8* chnTable);

controller_error_code_e ll_set_host_feature(_u16 bitNum,_u8 bitValue);

controller_error_code_e ll_reset(void);

controller_error_code_e ll_set_event_mask(_u64 eventMask);

controller_error_code_e ll_set_event_mask2(_u64 eventMask);

controller_error_code_e ll_set_le_event_mask(_u64 eventMask);

controller_error_code_e ll_set_random_address(_u8* addr);

controller_error_code_e ll_add_device_to_filter_accept_list(_u8 addrType,_u8* addr);

controller_error_code_e ll_remove_device_from_filter_accept_list(_u8 addrType,_u8* addr);

controller_error_code_e ll_clear_filter_accept_list(void);


#if(LL_SUPPORT_CONNECTION_SUBRATING)
controller_error_code_e ll_set_default_subrate(_u16 subrateMin,_u16 subrateMax,_u16 maxLatency,_u16 continuation,_u16 timeout);
#endif


#endif//LL_H_


