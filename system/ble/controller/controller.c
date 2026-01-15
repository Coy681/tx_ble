/*
 * controller.c
 *
 *  Created on: 2026年1月15日
 *      Author: Admin
 */

#include"ll/ll.h"

void ble_controller_init(void)
{
	ll_init_state_machine(1);

	ll_device_info_t info ={0};
	info.ppm = 50;
	ll_device_info_init(&info);
}
