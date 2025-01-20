/*
 * rf.c
 *
 *  Created on: 2024年12月4日
 *      Author: 12407
 */
//access code
//crc
//mic
//channel
//send data
//receive data
//phy
//power
//different object

#include"../../hal/rf.h"
#include"driver.h"

static hal_rf_t *rf;
void hal_rf_object_cast(hal_rf_t* object)
{
    rf = object;
}

void hal_rf_set_access_code(void)
{

    rf_access_code_comm(rf->accessCode);
}

void hal_rf_set_channel(_u8 channel)
{
    rf_set_ble_chn((_s8)channel);
}

void hal_rf_set_crc(_u32 crc)
{
    rf_set_ble_crc_value(crc&0xffffff);
}



void hal_rf_set_


