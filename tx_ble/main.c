

#include"driver.h"

#include"platform/platform.h"

#include"tx_common.h"

#include"common/txCommon.h"

#include"system/task/event/event.h"

#include"system/task/message/message.h"

#include"system/scheduler/sch.h"
#include"system/scheduler/sch_map.h"

#include"system/ble/controller/ll/ll.h"

volatile _u32 AAA_Mcause = 0;
volatile _u32 AAA_Mtval = 0;
volatile _u32 AAA_Mpec = 0;
volatile _u32 AAA_Mstatus = 0;
volatile _u32 AAA_Mdcause = 0;
volatile _u32 AAA_Ra = 0;

/**
 * @brief       This function service to handle all the platform pre-defined interrupt or exception.
 * @return      none
 */
void trap_entry(void) __attribute__((interrupt("machine"), aligned(4)));

void trap_entry(void)
{
	AAA_Mtval   = read_csr(NDS_MTVAL);
	AAA_Mpec    = read_csr(NDS_MEPC);
	AAA_Mstatus = read_csr(NDS_MSTATUS);
	AAA_Mcause  = read_csr(NDS_MCAUSE);
    AAA_Mdcause = read_csr(NDS_MDCAUSE);
}
volatile _u32 AAA_FREE_LIST = 0;
volatile _u32 AAA_FREE_LIST_LEN = 0;
void app_rx_cmd(_u8* data,_u32 len)
{
	LOG_TRACE(1,"rx data",data,len)
	_u8 data1[] = {0x10, 0x09, 'a', 'a', 'a', 'a', 'a', 0x5f, 0x63, 0x6f, 0x6e, 0x6e, 0x5f, 0x73, 0x6e,0x69,0x66,
			       0x02,0x01,0x05,
                   0x03,0x19,0x80,0x01,
                   0x05,0x02,0x12,0x18,0x0f,0x18};
	ll_extended_adv_param_t extendedAdvParam =
	{
		.advHandle              = 0x00,
		.advEventProperty       = LL_ADV_EVENT_PROPERTY_LEGACY_PDU|LL_ADV_EVENT_PROPERTY_SCANNABLE,
		.filterPolicy           = LL_FILTER_LIST_NOT_USE,
		.txPower                = 0x7f,
		.advSid                 = 0x00,
		.ownAddrType            = LL_PUBLIC_DEVICE_ADDRESS,
		.peerAddrType           = LL_PUBLIC_DEVICE_OR_IDENTITY_ADDRESS,
		.peerAddr               = {0x00,0x00,0x00,0x00,0x00,0x00},
		.scanReqNotifyEnable    = 0,
		.primaryAdvInterval     = 16,
		.primaryAdvChnMap       = LL_ADV_CHN_37|LL_ADV_CHN_38|LL_ADV_CHN_39,
		.primaryAdvPhy          = LL_ADV_PHY_1M,
		.primaryAdvphyOptions   = 0,
		.secondaryAdvMaxSkip    = 0,
		.secondaryAdvPhy        = 0,
		.secondaryAdvphyOptions = 0,
	};
	ll_extended_adv_enable_subField_e advEnable =
	{
		.advHandle = 0x00,
		.duration  = 0x00,
		.maxEvents = 0x00,
	};
	int status = 0;

	switch(data[0])
	{
		case 1:
			status = ll_set_extended_advertising_parameters(&extendedAdvParam);
			LOG_TRACE(1,"set extended param",&status,4)
			break;
		case 2:
			status = ll_set_extended_advertising_data(0x00,\
					                         LL_ADV_DATA_OPERATION_COMPLETE,\
											 LL_ADV_DATA_NOT_OR_MINIMIZE_FRAGMENT,\
											 sizeof(data1),
											 data1);
			LOG_TRACE(1,"set extended data",&status,4)
			break;
		case 3:
			status = ll_set_extended_scan_response_data(0x00,\
					                         LL_ADV_DATA_OPERATION_COMPLETE,\
											 LL_ADV_DATA_NOT_OR_MINIMIZE_FRAGMENT,\
											 sizeof(data1),
											 data1);
			LOG_TRACE(1,"set scan rsp data",&status,4)
			break;
		case 4:
			status = ll_set_extended_advertising_enable(1,\
					                           1,\
											   &advEnable);
			LOG_TRACE(1,"set adv enable",&status,4)
			break;
		default:
			break;
	}
//	 switch(data[0])
//	 {
//	 	case 1:
//	 		ll_set_advertising_parameters(16,LL_ADV_NONCONN_IND,LL_PUBLIC_DEVICE_ADDRESS,LL_PUBLIC_DEVICE_OR_IDENTITY_ADDRESS,0,7,LL_FILTER_LIST_NOT_USE);
//	 		break;
//	 	case 2:
//	 		ll_set_advertising_data(data1,sizeof(data1));
//	 		break;
//	 	case 3:
//	 	    ll_set_scan_response_data(data1,sizeof(data1));
//	 		break;
//	 	case 4:
//	 		ll_set_advertising_enable(1);
//	 		break;
//	 	case 5:
//	 		ll_set_advertising_enable(0);
//	 		break;
//	 	default:
//	 		break;
//	 }

//     _u32 systemTime = 0x100;
// 	 sch_map_node_t node[5]=
// 	 {
// 	   {.start = systemTime+0x300,  .end = systemTime+0x800,  .period = 0x2000,.type = SCH_PERIODIC_TASK},
// 	   {.start = systemTime+0x900,  .end = systemTime+0xc00,  .period = 0x2000,.type = SCH_SPORADIC_TASK},
// 	   {.start = systemTime+0x1200, .end = systemTime+0x2300, .period = 0x3000,.type = SCH_SPORADIC_TASK},
// 	   {.start = systemTime+0x700,  .end = systemTime+0x900,  .period = 0x4000,.type = SCH_PERIODIC_TASK},
// 	   {.start = systemTime+0xc00,  .end = systemTime+0x1500, .period = 0x5000,.type = SCH_PERIODIC_TASK},
// 	 };
// 	sch_map_free_slot_t* freeSlot = NULL;
// 	_u32 freeCount = 0;
//	 switch(data[0])
//	 {
//		case 1:
//			sch_map_calculate_free_space_by_slot(systemTime+0x100,systemTime+0x1000,node,5,&freeSlot,&freeCount,300);
//			AAA_FREE_LIST = (_u32)freeSlot;
//			AAA_FREE_LIST_LEN =freeCount;
//			while(1);
//			tx_free((_u8*)freeSlot);
//			break;
//		default:
//			break;
//	 }
}


int main(void)
{
	//init call function
	initcall_f pInit;
	_u32 *pTemp;
	extern _u32 _INIT_START;
    extern _u32 _INIT_END;
    for(pTemp = &_INIT_START;pTemp<&_INIT_END; pTemp++)
    {
    	pInit = (initcall_f)(*pTemp);
    	pInit();
    }
	LOG_TRACE(1,"system start",0,0)
	hal_stimer_set_capture(system_time()+1000);
	log_register_rx_callback(app_rx_cmd);
	tx_task_start();
    return 0;
}
