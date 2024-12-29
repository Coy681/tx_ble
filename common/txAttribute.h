/*
 * attribute.h
 *
 *  Created on: 2024年12月7日
 *      Author: Admin
 */

#ifndef TXATTRIBUTE_H_
#define TXATTRIBUTE_H_

#define _RAM_CODE                __attribute__((section(".ram_code")))

#define _DATA                    __attribute__((section(".data")))

#define _FLASH_CODE              __attribute__((section(".text")))

#define _ALWAYS_INLINE           inline __attribute__((always_inline))

#define _NO_INLINE               __attribute__((noinline)) 

/******************define init call functions**********************/
typedef void(*initcall_f)(void);
#define _initcall_name(fn,level)    _initcall_##fn##_##level
#define INITCALL_SECTION(level)     __attribute__((used,section(".initcall."#level".init")))
#define _initcall(fn,level)    static const initcall_f INITCALL_SECTION(level) _initcall_name(fn,level) = fn;

#define HAREWARE_INIT(fn)           _initcall(fn,0)
#define ARCH_INIT(fn)               _initcall(fn,1)
#define DEV_INIT(fn)                _initcall(fn,2)
#define BROAD_INIT(fn)              _initcall(fn,3)
#define OS_INIT(fn)                 _initcall(fn,4)
#define APP_INIT(fn)                _initcall(fn,5)



#endif /* TXATTRIBUTE_H_ */
