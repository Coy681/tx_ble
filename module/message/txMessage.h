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
    u32 messageMask;
    u32 messageCode;
    void(*cb)(void);
    txMessageNode_t *next;
}txMessageNode_t

void tx_message_init();

void tx_message_add();

void tx_message_process();


#endif /* MESSAGE_TXMESSAGE_H_ */
