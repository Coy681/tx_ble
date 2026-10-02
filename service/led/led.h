/*
 * led.h
 *
 *  Created on: 2026年10月2日
 *      Author: Admin
 *
 *  Led service: business-level led control (connection indication,
 *  charging hint, breathing effects...). Not a debug facility, works
 *  even when debug is fully disabled.
 */

#ifndef SYSTEM_LED_H_
#define SYSTEM_LED_H_
#include "common/txCommon.h"
#include "config.h"

#define LED_NUM      4

/**
 * @brief     These functions control led by led number.
 * @param[in] n - led number, 0 ~ LED_NUM-1.
 * @return    none
 */
void led_high(_u8 n);

void led_low(_u8 n);

void led_toggle(_u8 n);

#endif /* SYSTEM_LED_H_ */
