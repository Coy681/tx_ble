/*
 * txUtility.h
 *
 *  Created on: 2025年3月28日
 *      Author: Admin
 */

#ifndef TXUTILITY_H_
#define TXUTILITY_H_

#define ARRAY_SIZE(A)                  (sizeof(A)/sizeof(*A))

#define TRUE                           (1)
#define FALSE                          (0)

#define CONDITION_TRUE(c)              ((c) != FALSE)
#define CONDITION_FALSE(c)             ((c) == FALSE)

#define POINTER_VALID(node)            (node != 0)
 
#define POINTER_NOT_VALID(node)        (node == 0)

#define TX_MAX(a, b)                   (((a) > (b)) ? (a) : (b))

#define TX_MIN(a, b)                   (((a) < (b)) ? (a) : (b))

#define TX_ALIGN_UP_TO_4(num)          (((num) + 3) & ~3)

#endif /* TXUTILITY_H_ */
