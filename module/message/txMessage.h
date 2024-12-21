/*
 * txMessage.h
 *
 *  Created on: 2024年12月10日
 *      Author: 12407
 */

#ifndef MESSAGE_TXMESSAGE_H_
#define MESSAGE_TXMESSAGE_H_

typedef struct txMessageNode_t txMessageNode_t

typedef struct txMessageNode_t
{
    u32 messageType;
    void(*cb)(void);
    txMessageNode_t *next;
}txMessageNode_t

#endif /* MESSAGE_TXMESSAGE_H_ */
