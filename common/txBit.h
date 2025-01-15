/*
 * bit.h
 *
 *  Created on: 2024年12月9日
 *      Author: Admin
 */

#ifndef TXBIT_H_
#define TXBIT_H_

#include"txType.h"

#define BIT(n)                          ( 1<<(n))


#define BYTE_TO_U16(n,p)                {n = ((_u16)p[0] + ((_u16)p[1]<<8));}

#define BYTE_TO_U24(n,p)                {n = ((_u32)p[0] + ((_u32)p[1]<<8) + ((_u32)p[2]<<16));}

#define BYTE_TO_U32(n,p)                {n = ((_u32)p[0] + ((_u32)p[1]<<8) + ((_u32)p[2]<<16) + ((_u32)p[3]<<24));}


#define STREAM_TO_U16(n,p)              {BYTE_TO_U16(n,p);p+=2;}

#define STREAM_TO_U24(n,p)              {BYTE_TO_U24(n,p);p+=3;}

#define STREAM_TO_U32(n,p)              {BYTE_TO_U32(n,p);p+=4;}


#define U8_TO_STREAM(p,n)               {*p++ = (_u8)n;}

#define U16_TO_STREAM(p,n)              {*p++ = (_u8)n; *p++ = (_u8)n>>8}

#define U24_TO_STREAM(p,n)              {*p++ = (_u8)n; *p++ = (_u8)n>>8; *p++ = (_u8)n>>16;}

#define U32_TO_STREAM(p,n)              {*p++ = (_u8)n; *p++ = (_u8)n>>8; *p++ = (_u8)n>>16; *p++ = (_u8)n>>24;}



#endif /* TXBIT_H_ */
