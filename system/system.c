/*
 * sche.c
 *
 *  Created on: 2024年12月4日
 *      Author: 12407
 */

#include"platform/platform.h"
#include"common/txCommon.h"
#include"tx_common.h"

hal_rf_t ble_rf;

//static void system_timer_task(void);
//static void system_rf_task(void)


unsigned char  ble_tx_packet[48] __attribute__ ((aligned (4))) ={3,0,0,0,
		                                                         0x00,0x24,
																 0x67, 0x67, 0x67, 0x67, 0x67, 0x67,
																 0x10, 0x09, 'a', 'a', 'a', 'a', 'a', 0x5f, 0x63, 0x6f, 0x6e, 0x6e, 0x5f, 0x73, 0x6e,0x69,0x66,
																 0x02,0x01,0x05,
                                                                 0x03,0x19,0x80,0x01,
                                                                 0x05,0x02,0x12,0x18,0x0f,0x18};

void system_init()
{
	tx_malloc_init();
//	hal_stimer_register_task(system_timer_task);
//	hal_rf_register_task(&ble_rf,0xd6be898e,0x555555);
}
ARCH_INIT(system_init);

// static void system_timer_task(void)
// {
// 	DEBUG_GPIO_HIGH(GPIO_0);
// 	DEBUG_GPIO_LOW(GPIO_0);
//     unsigned char rf_data_len  = 0x26;
//     ble_tx_packet[4]           = 0;
//     ble_tx_packet[5]           = 0x24;
//     unsigned int rf_tx_dma_len = rf_tx_packet_dma_len(rf_data_len);
//     ble_tx_packet[3]           = (rf_tx_dma_len >> 24) & 0xff;
//     ble_tx_packet[2]           = (rf_tx_dma_len >> 16) & 0xff;
//     ble_tx_packet[1]           = (rf_tx_dma_len >> 8) & 0xff;
//     ble_tx_packet[0]           = rf_tx_dma_len & 0xff;

//     hal_rf_object_cast(&ble_rf);
// 	ble_rf.updateParam(HAL_RF_PARAM_CHANNEL,37);
// 	ble_rf.updateParam(HAL_RF_PARAM_PHY,HAL_RF_PHY_1M);
// //	ble_rf.updateParam(HAL_RF_PARAM_ACCESS_CODE,0xd6be898e);
// //	ble_rf.updateParam(HAL_RF_PARAM_CRC,0x555555);
// 	ble_rf.rfrepare();

// 	DEBUG_GPIO_HIGH(GPIO_2);
// 	ble_rf.tx(system_clock()+300*SYSTEM_TIME_US,ble_tx_packet);
//     while (!(rf_get_irq_status(FLD_RF_IRQ_TX)));
//     rf_clr_irq_status(FLD_RF_IRQ_TX);
// 	DEBUG_GPIO_LOW(GPIO_2);
//     system_delay_ms(10);


// 	hal_stimer_set_capture(system_clock()+20*SYSTEM_TIME_MS);
// }

// static void system_rf_task(void)
// {

// }
