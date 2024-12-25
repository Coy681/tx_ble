/*
 * txMessage.c
 *
 *  Created on: 2024年12月10日
 *      Author: 12407
 */

#include"txMessage.h"
typedef struct txMessage
{
    txMessageNode_t* pHead;
    txMessageNode_t* pTail;
}txMessage_t;

txMessage_t message;

