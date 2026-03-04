/*
 * ll_test.c
 *
 *  Created on: 2026年1月13日
 *      Author: Admin
 */

#include"ll_test.h"
#include"system/scheduler/sch.h"
#include"tx_common.h"

/**
 * ll test cmd define
 * switch data[0]:
 * 0x11 - adv module
 * 0x22 - brd module
 * 0x33 - conn module
 * 0x44 - init module
 * 0x55 - scam module
 * 0x66 - sync module
 */
void ll_test_process(_u8* data,_u8 len)
{
	switch(data[0])
	{
		case 0x11:
		{
			extern void ll_adv_test_process(_u8* data,_u8 len);
			ll_adv_test_process(&data[1],len-1);
		}break;
		case 0x22:
		{
			extern void ll_brd_test_process(_u8* data,_u8 len);
			ll_brd_test_process(&data[1],len-1);
		}break;
		case 0x33:
		{
			extern void ll_conn_test_process(_u8* data,_u8 len);
			ll_conn_test_process(&data[1],len-1);
		}break;
		case 0x44:
		{
			extern void ll_init_test_process(_u8* data,_u8 len);
			ll_init_test_process(&data[1],len-1);
		}break;
		case 0x55:
		{
			extern void ll_scan_test_process(_u8* data,_u8 len);
			ll_scan_test_process(&data[1],len-1);
		}break;
		case 0x66:
		{
			extern void ll_scan_test_process(_u8* data,_u8 len);
			ll_scan_test_process(&data[1],len-1);
		}break;
		default:
			break;
	}
}



//void task1_callback(_u8 type,_u8 id);
//sch_node_t aTask1=
//{
//	.id = 0x66,
//	.type = SCH_PERIODIC_TASK,
//    .priority = SCH_TASK_PRIORITY_A,
//	.timestamp = 0,
//    .period = 20000,
//	.duration = 200,
//	.startLatency = 50,
//	.stopLatency = 50,
//	.cb = task1_callback,
//};
//
//
//_RAM_CODE void task1_callback(_u8 type,_u8 id)
//{
//	if(type == SCH_TASK_START)
//	{
//    	DEBUG_GPIO_HIGH(GPIO_6);
////		LOG_TRACE(1,"task 1 start",0,0)
//
//	}
//	else if(type == SCH_TASK_STOP)
//	{
//    	DEBUG_GPIO_LOW(GPIO_6);
//		aTask1.timestamp = aTask1.timestamp+aTask1.period;
////		LOG_TRACE(1,"task 1 stop",0,0)
//	}
//	else if(type == SCH_TASK_CANCELED)
//	{
//    	DEBUG_GPIO_HIGH(GPIO_8);
//    	DEBUG_GPIO_LOW(GPIO_8);
////    	DEBUG_GPIO_HIGH(GPIO_5);
//
////		DEBUG_GPIO_LOW(GPIO_5);
////		LOG_TRACE(1,"task 1 canceled",0,0)
//	}
//	else if(type == SCH_TASK_PASSED)
//	{
//    	DEBUG_GPIO_HIGH(GPIO_9);
//    	DEBUG_GPIO_LOW(GPIO_9);
////    	DEBUG_GPIO_HIGH(GPIO_6);
//		aTask1.timestamp = system_time()+500;
////		DEBUG_GPIO_LOW(GPIO_6);
////		LOG_TRACE(1,"task 1 passed",0,0)
//	}
//}
//
//void task2_callback(_u8 type,_u8 id);
//
//sch_node_t aTask2=
//{
//	.id = 0x67,
//	.type = SCH_PERIODIC_TASK,
//    .priority = SCH_TASK_PRIORITY_A,
//	.timestamp = 0,
//    .period = 20000,
//	.duration = 200,
//	.startLatency = 50,
//	.stopLatency = 50,
//	.cb = task2_callback,
//};
//
//_RAM_CODE void task2_callback(_u8 type,_u8 id)
//{
//	if(type == SCH_TASK_START)
//	{
//    	DEBUG_GPIO_HIGH(GPIO_7);
////		LOG_TRACE(1,"task 2 start",0,0)
//
//	}
//	else if(type == SCH_TASK_STOP)
//	{
//    	DEBUG_GPIO_LOW(GPIO_7);
//    	aTask2.timestamp = aTask2.timestamp+aTask1.period;
////		LOG_TRACE(1,"task 2 stop",0,0)
//	}
//	else if(type == SCH_TASK_CANCELED)
//	{
//    	DEBUG_GPIO_HIGH(GPIO_10);
//    	DEBUG_GPIO_LOW(GPIO_10);
//		aTask2.priority++;
////		LOG_TRACE(1,"task 2 canceled",0,0)
//	}
//	else if(type == SCH_TASK_PASSED)
//	{
//    	DEBUG_GPIO_HIGH(GPIO_11);
//    	DEBUG_GPIO_LOW(GPIO_11);
//		aTask2.timestamp = system_time()+500;
////		LOG_TRACE(1,"task 2 passed",0,0)
//	}
//
//}


//  		break;
//  	case 5:

// 		{
// 	 		_u8* message = tx_message_allocate(8);
// 	 		message[0] = SCHE_MESSAGE_TASK_ADD;
// 	 		message[1] = ((_u32)&aTask1);
// 	 		message[2] = ((_u32)&aTask1)>>8;
// 	 		message[3] = ((_u32)&aTask1)>>16;
// 	 		message[4] = ((_u32)&aTask1)>>24;
// 	 		tx_message_send(TX_TASK_ID_SCH,message);
// 	 		LOG_TRACE(1,"task add",&status,4)
// 		}

//  		break;
//  	case 6:

// 		{
// 	 		_u8* message = tx_message_allocate(8);
// 	 		message[0] = SCHE_MESSAGE_TASK_ADD;
// 	 		message[1] = ((_u32)&aTask2);
// 	 		message[2] = ((_u32)&aTask2)>>8;
// 	 		message[3] = ((_u32)&aTask2)>>16;
// 	 		message[4] = ((_u32)&aTask2)>>24;
// 	 		tx_message_send(TX_TASK_ID_SCH,message);
// 	 		LOG_TRACE(1,"task add",&status,4)
// 		}

//  		break;



//  	default:
//  		break;
//  }


