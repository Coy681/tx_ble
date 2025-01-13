
#include"common/txCommon.h"

typedef struct txMessage_t txMessage_t;

typedef struct txMessage_t
{
    txMessage_t* next;
    _u8*         data;
    _u16         taskId;
    _u16         rsvd;
    _u32         event;
}txMessage_t;

typedef enum
{
    TX_MESSAGE_SUCCESS,
    TX_MESSAGE_NO_TASK,
}txMessage_e;


_u8* tx_message_allocate(_u16 len);

_u8* tx_message_deallocate(_u8* message);

txMessage_e tx_message_send(_u16 destTaskId,_u8* message);

txMessage_t* tx_message_receive(_u16 taskId);

txMessage_e tx_message_push(void);






