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
#define BIT64(n)                        (1ULL<<(n))

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


#define BIT_SET(var, bit)               ((var) |=  (1U << (bit)))

#define BIT_EXIST(var, bit)             ((var) & (1U << (bit)))

#define BIT_CLR(var, bit)               ((var) &= ~(1U << (bit)))

#define BIT_N_VALID(p,n)                (p[n>>3]&BIT(n&0x07))

#define BIT_REVERSE_8(x) \
    ( ((x) & 0x01) << 7 |  /* bit0→bit7 */ \
      ((x) & 0x02) << 5 |  /* bit1→bit6 */ \
      ((x) & 0x04) << 3 |  /* bit2→bit5 */ \
      ((x) & 0x08) << 1 |  /* bit3→bit4 */ \
      ((x) & 0x10) >> 1 |  /* bit4→bit3 */ \
      ((x) & 0x20) >> 3 |  /* bit5→bit2 */ \
      ((x) & 0x40) >> 5 |  /* bit6→bit1 */ \
      ((x) & 0x80) >> 7 )  /* bit7→bit0 */

static inline int count_bits_one(unsigned int n)
{
    int count = 0;
    while (n)
    {
        n &= n - 1;
        count++;
    }
    return count;
}


#endif /* TXBIT_H_ */
