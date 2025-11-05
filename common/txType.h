/*
 * type.h
 *
 *  Created on: 2024年12月7日
 *      Author: Admin
 */

#ifndef TXTYPE_H_
#define TXTYPE_H_

#if defined NULL
#undef NULL
#define NULL   (0)
#else
#define NULL   (0)
#endif

#if defined bool
#undef bool
#define bool   unsigned char
#else
#define bool   unsigned char
#endif

typedef unsigned long long   _u64;
typedef unsigned long        _u32;
typedef unsigned short       _u16;
typedef unsigned char        _u8;

typedef   signed long long   _s64;
typedef   signed long        _s32;
typedef   signed short       _s16;
typedef   signed char        _s8;



#endif /* TXTYPE_H_ */
