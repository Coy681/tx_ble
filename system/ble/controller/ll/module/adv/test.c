/*
 * test.c
 *
 *  Created on: 2026年1月12日
 *      Author: Admin
 */
#include"common/txCommon.h"
#include"adv.h"
#include"tx_common.h"
//volatile _u32 AAA_FREE_LIST = 0;
//volatile _u32 AAA_FREE_LIST_LEN = 0;

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


	//  ll_extended_adv_param_t extendedAdvParam =
	//  {
	//  	.advHandle              = 0x00,
	//  	.advEventProperty       = LL_ADV_EVENT_PROPERTY_DIRECTED,
	//  	.filterPolicy           = LL_FILTER_LIST_NOT_USE,
	//  	.txPower                = 0x7f,
	//  	.advSid                 = 0x00,
	//  	.ownAddrType            = LL_PUBLIC_DEVICE_ADDRESS,
	//  	.peerAddrType           = LL_PUBLIC_DEVICE_OR_IDENTITY_ADDRESS,
	//  	.peerAddr               = {0x00,0x00,0x00,0x00,0x00,0x00},
	//  	.scanReqNotifyEnable    = 0,
	//  	.primaryAdvInterval     = 80,
	//  	.primaryAdvChnMap       = LL_ADV_CHN_37|LL_ADV_CHN_38|LL_ADV_CHN_39,
	//  	.primaryAdvPhy          = LL_ADV_PHY_1M,
	//  	.primaryAdvphyOptions   = 0,
	//  	.secondaryAdvMaxSkip    = 0,
	//  	.secondaryAdvPhy        = LL_ADV_PHY_2M,
	//  	.secondaryAdvphyOptions = 0,
	//  };



	//  for(int i=0;i<480;i++)
	//  {
	// 	 data2[i] = i;
	//  }
	//  ll_extended_adv_enable_subField_e advEnable =
	//  {
	//  	.advHandle = 0x00,
	//  	.duration  = 0x00,
	//  	.maxEvents = 0x00,
	//  };
	//  int status = 0;

	//  switch(data[0])
	//  {
	//  	case 1:
	//  		status = ll_set_extended_advertising_parameters(&extendedAdvParam);
	//  		LOG_TRACE(1,"set extended param",&status,4)
	//  		break;
	//  	case 2:
	// 			status = ll_set_extended_advertising_data(0x00,\
	// 											 LL_ADV_DATA_OPERATION_FIRST_FRAGMENT,\
	// 											 LL_ADV_DATA_NOT_OR_MINIMIZE_FRAGMENT,\
	// 											 48,
	// 											 data2);
	// 			LOG_TRACE(1,"set extended data1",&status,4)
	// 			status = ll_set_extended_advertising_data(0x00,\
	// 											 LL_ADV_DATA_OPERATION_INTERMEDIATE_FRAGMENT,\
	// 											 LL_ADV_DATA_NOT_OR_MINIMIZE_FRAGMENT,\
	// 											 48,
	// 											 data2+48);
	// 			LOG_TRACE(1,"set extended data2",&status,4)
	// 			status = ll_set_extended_advertising_data(0x00,\
	// 											LL_ADV_DATA_OPERATION_INTERMEDIATE_FRAGMENT,\
	// 											 LL_ADV_DATA_NOT_OR_MINIMIZE_FRAGMENT,\
	// 											 48,
	// 											 data2+96);
	// 			LOG_TRACE(1,"set extended data3",&status,4)
	// 	 		status = ll_set_extended_advertising_data(0x00,\
	// 	 										LL_ADV_DATA_OPERATION_INTERMEDIATE_FRAGMENT,\
	// 	 										 LL_ADV_DATA_NOT_OR_MINIMIZE_FRAGMENT,\
	// 	 										 48,
	// 											 data2+144);
	// 	 		LOG_TRACE(1,"set extended data4",&status,4)
	// 	 		status = ll_set_extended_advertising_data(0x00,\
	// 	 										 LL_ADV_DATA_OPERATION_INTERMEDIATE_FRAGMENT,\
	// 	 										 LL_ADV_DATA_NOT_OR_MINIMIZE_FRAGMENT,\
	// 	 										 48,
	// 											 data2+192);
	// 	 		LOG_TRACE(1,"set extended data5",&status,4)
	// 	 		status = ll_set_extended_advertising_data(0x00,\
	// 	 										 LL_ADV_DATA_OPERATION_INTERMEDIATE_FRAGMENT,\
	// 	 										 LL_ADV_DATA_NOT_OR_MINIMIZE_FRAGMENT,\
	// 	 										 48,
	// 											 data2+240);
	// 	 		LOG_TRACE(1,"set extended data6",&status,4)
	// 	 		status = ll_set_extended_advertising_data(0x00,\
	// 	 				                         LL_ADV_DATA_OPERATION_INTERMEDIATE_FRAGMENT,\
	// 	 										 LL_ADV_DATA_NOT_OR_MINIMIZE_FRAGMENT,\
	// 	 										 48,
	// 											 data2+288);
	// 	 		LOG_TRACE(1,"set extended data7",&status,4)
	// 	 		status = ll_set_extended_advertising_data(0x00,\
	// 	 								         LL_ADV_DATA_OPERATION_LAST_FRAGMENT,\
	// 	 										 LL_ADV_DATA_NOT_OR_MINIMIZE_FRAGMENT,\
	// 	 										 48,
	// 											 data2+336);
	// 	 		LOG_TRACE(1,"set extended data8",&status,4)
	//  		break;
	//  	case 3:
	//  		status = ll_set_extended_scan_response_data(0x00,\
	//  				                         LL_ADV_DATA_OPERATION_COMPLETE,\
	//  										 LL_ADV_DATA_NOT_OR_MINIMIZE_FRAGMENT,\
	//  										 sizeof(data1),
	//  										 data1);
	//  		LOG_TRACE(1,"set scan rsp data",&status,4)
	//  		break;
	//  	case 4:
	//  		status = ll_set_extended_advertising_enable(1,\
	//  				                           1,\
	//  										   &advEnable);
	//  		LOG_TRACE(1,"set adv enable",&status,4)

void ll_adv_test_process(_u8* data,_u8 len)
{
	_u8 advData[] = {0x10, 0x09, 'a', 'a', 'a', 'a', 'a', 0x5f, 0x63, 0x6f, 0x6e, 0x6e, 0x5f, 0x73, 0x6e,0x69,0x66,
			       0x02,0x01,0x05,
                   0x03,0x19,0x80,0x01,
                   0x05,0x02,0x12,0x18,0x0f,0x18};
	int status = 0;
	switch(data[0])
	{
	    case 0x00:
	    {
			status = ll_set_advertising_parameters(32,LL_ADV_NONCONN_IND,LL_PUBLIC_DEVICE_ADDRESS,LL_PUBLIC_DEVICE_OR_IDENTITY_ADDRESS,0,7,LL_FILTER_LIST_NOT_USE);
	    	LOG_TRACE(1,"set adv parameters",&status,4);
	    	status = ll_set_advertising_data(advData,sizeof(advData));
	    	LOG_TRACE(1,"set adv data",&status,4);
	    	status = ll_set_scan_response_data(advData,sizeof(advData));
	    	LOG_TRACE(1,"set scan rsp data",&status,4);
	    }break;
	    case 0x01:
	    {
	    	status = ll_set_advertising_enable(1);
	    	LOG_TRACE(1,"enable adv",&status,4);
	    }break;
	    case 0x02:
	    {
	    	status = ll_set_advertising_enable(0);
	    	LOG_TRACE(1,"disable adv",&status,4);
	    }break;
	    default:
	    	break;
	}
}
