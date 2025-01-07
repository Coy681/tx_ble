
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

