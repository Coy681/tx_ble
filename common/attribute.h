/*
 * attribute.h
 *
 *  Created on: 2024年12月7日
 *      Author: Admin
 */

#ifndef ATTRIBUTE_H_
#define ATTRIBUTE_H_

#define _RAM_CODE                __attribute__((section(".ram_code")))

#define _DATA                    __attribute__((section(".data")))

#define _FLASH_CODE              __attribute__((section(".text")))

#define _ALWAYS_INLINE           inline __attribute__((always_inline))

#define _NO_INLINE               __attribute__((noinline)) 

#endif /* ATTRIBUTE_H_ */
