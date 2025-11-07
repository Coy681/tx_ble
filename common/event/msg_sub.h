/*
 * msg_sub.h
 *
 *  Created on: 2025年11月6日
 *      Author: Admin
 */

#ifndef EVENT_MSG_SUB_H_
#define EVENT_MSG_SUB_H_

#include"common/txType.h"
#include"common/txAttribute.h"
#include"common/txUtility.h"
#include"common/txGeneral.h"
#include"common/mmu/ml/ml.h"

typedef enum
{
	TX_MESSAGE_TYPE_CONN_EVENT_COUNTER,
}tx_msg_type_e;

typedef enum
{
	TX_MESSAGE_MODULE_CONTROLLER,
	TX_MESSAGE_MODULE_HOST,
}tx_msg_module_e;

typedef void(*txMessageHandler_f)(_u8*data,_u32 len);

typedef struct tx_msg_sub_t
{
	_u8 module;
	_u8 msgType;
	txMessageHandler_f handler;
	struct tx_msg_sub_t* next;
}tx_msg_sub_t;

int  tx_message_subscribe(tx_msg_module_e module,tx_msg_type_e type,txMessageHandler_f handler);

int  tx_message_unsubscribe(tx_msg_module_e module,tx_msg_type_e type,txMessageHandler_f handler);

void tx_message_unsubscribe_all(void);

int  tx_message_inform(tx_msg_module_e module,tx_msg_type_e type,_u8*data,_u32 dataLen);

#endif /* EVENT_MSG_SUB_H_ */
