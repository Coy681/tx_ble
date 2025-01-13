
#include"common/txCommon.h"

typedef struct txMessageHeader_t txMessageHeader_t;

typedef struct txMessageHeader_t
{
    txMessageHeader_t* next;
    _u16               taskId;
    _u16               dataLen;
}txMessageHeader_t;

typedef struct 
{
    txMessageHeader_t header;
    _u8*              data
}txMessage_t;

typedef enum
{
    TX_MESSAGE_SUCCESS,
    TX_MESSAGE_NO_TASK,
    TX_MESSAGE_PARAM_ERROR,
    TX_MESSAGE_USED,
}txMessage_e;

#define TX_MESSAGE_NEXT(message)      ((txMessageHeader_t*)(message)-1)->next

#define TX_MESSAGE_TASK_ID(message)   ((txMessageHeader_t*)(message)-1)->taskId

_u8* tx_message_allocate(_u16 len);

txMessage_e tx_message_deallocate(_u8* message);

txMessage_e tx_message_send(_u16 destTaskId,_u8* message);

_u8* tx_message_receive(_u16 taskId);






