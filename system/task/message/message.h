
#include"common/txCommon.h"

#ifndef TASK_MESSAGE_EVENT_H_
#define TASK_MESSAGE_EVENT_H_

typedef struct txMessageHeader_t
{
    _u8*   next;
    _u16   taskId;
    _u16   dataLen;
}txMessageHeader_t;

typedef enum
{
    TX_MESSAGE_SUCCESS,
    TX_MESSAGE_NO_TASK,
    TX_MESSAGE_PARAM_ERROR,
    TX_MESSAGE_USED,
}txMessage_e;

#ifndef  TX_MESSGAE_LOG_ENABLE
#define  TX_MESSGAE_LOG_ENABLE 1
#endif

#define TX_MESSAGE_NEXT(message)      ((txMessageHeader_t*)(message)-1)->next

#define TX_MESSAGE_TASK_ID(message)   ((txMessageHeader_t*)(message)-1)->taskId

#define TX_MESSAGE_LEN(message)       ((txMessageHeader_t*)(message)-1)->dataLen

_u8*        tx_message_allocate(_u16 len);

txMessage_e tx_message_deallocate(_u8* message);

txMessage_e tx_message_send(_u16 destTaskId,_u8* message);

txMessage_e tx_message_enqueue(_u8* message);

_u8*        tx_message_receive(_u16 taskId);

#endif

