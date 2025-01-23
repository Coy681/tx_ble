/*
 * platform.h
 *
 *  Created on: 2024年12月4日
 *      Author: 12407
 */

#ifndef PLATFORM_H_
#define PLATFORM_H_

void platform_init(void);


#ifndef PLATFORM_FEATURE_DEBUG_GPIO
#define PLATFORM_FEATURE_DEBUG_GPIO 1
#endif

#ifndef PLATFORM_FEATURE_LED
#define PLATFORM_FEATURE_LED 1
#endif

#ifndef PLATFORM_FEATURE_UART
#define PLATFORM_FEATURE_UART 1
#endif

#ifndef PLATFORM_FEATURE_RF
#define PLATFORM_FEATURE_RF 1
#endif



#endif /* PLATFORM_H_ */
