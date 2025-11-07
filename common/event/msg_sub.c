/*
 * msg_sub.c
 *
 *  Created on: 2025年11月6日
 *      Author: Admin
 */

#include"msg_sub.h"

static tx_msg_sub_t* hdr = NULL;

int tx_message_subscribe(tx_msg_module_e module,tx_msg_type_e type,txMessageHandler_f handler)
{
	if(handler == NULL)
	{
		return 1;
	}
	tx_msg_sub_t* node = hdr;
	while(node)
	{
		if(module == node->module\
		 &&type == node->msgType\
		 &&handler == node->handler)
		{
			return 1;
		}
		node = node->next;
	}
	node = (tx_msg_sub_t*)tx_malloc(sizeof(tx_msg_sub_t));
	node->handler = handler;
	node->module  = module;
	node->msgType = type;
	node->next    = hdr;
	hdr           = node;
	return 0;
}

int tx_message_unsubscribe(tx_msg_module_e module,tx_msg_type_e type,txMessageHandler_f handler)
{
	if(handler == NULL)
	{
		return 1;
	}
	tx_msg_sub_t* cur = hdr;
	tx_msg_sub_t*prev = NULL;
	while(cur)
	{
		if(module == cur->module\
		 &&type == cur->msgType\
		 &&handler == cur->handler)
		{
			if(POINTER_NOT_VALID(prev))
			{
				hdr = hdr->next;
			}
			else
			{
				prev->next = cur->next;
			}
			tx_free((_u8*)cur);
			return 0;
		}
		prev = cur;
		cur = cur->next;
	}
	return 1;
}

void tx_message_unsubscribe_all(void)
{
	tx_msg_sub_t* cur  = hdr;
	tx_msg_sub_t* temp = NULL;
	while(cur)
	{
		temp = cur;
		cur  = cur->next;
		tx_free((_u8*)temp);
	}
	hdr = NULL;
}

int tx_message_inform(tx_msg_module_e module,tx_msg_type_e type,_u8*data,_u32 dataLen)
{
	tx_msg_sub_t* cur = hdr;
	tx_msg_sub_t* next = NULL;
	while(cur)
	{
		next = cur->next;
		if(module == cur->module\
		 &&type == cur->msgType)
		{
			cur->handler(data,dataLen);
		}
		cur = next;
	}
	return 0;
}
