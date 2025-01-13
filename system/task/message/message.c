#include"message.h"
#include"../task.h"
#include"../event/event.h"
_u8* txMsgHdr;

_u8* tx_message_allocate(_u16 len)
{
    if(len == 0)
    {
        return NULL;
    }
    txMessageHeader_t* pMessage = (txMessageHeader_t*)tx_malloc(len + sizeof(txMessageHeader_t));;
    if(pMessage)
    {
        pMessage->next = NULL;
        pMessage->dataLen = len;
        pMessage->taskId = 0;
        return ((_u8*)(pMessage+1));
    }
    else
    {
        return NULL;
    }

}

txMessage_e tx_message_deallocate(_u8* message)
{
    if(message == NULL)
    {
        return TX_MESSAGE_PARAM_ERROR;
    }
    if(TX_MESSAGE_TASK_ID(message)!=0)
    {
        return TX_MESSAGE_USED;
    }
    _u8* pfree = (_u8*)((txMessageHeader_t*)(message)-1);
    tx_free(pfree);
    return TX_MESSAGE_SUCCESS;
}

static txMessage_e tx_message_enqueue(_u8* message)
{
    if(message == NULL)
    {
        return TX_MESSAGE_PARAM_ERROR;
    }
    if(txMsgHdr == NULL)
    {
        txMsgHdr = message;
    }
    else
    {
        _u8* list = NULL;
        for(list = txMsgHdr;TX_MESSAGE_NEXT(list)!=NULL;list = TX_MESSAGE_NEXT(list));
        TX_MESSAGE_NEXT(list) = message;
    }
    return TX_MESSAGE_SUCCESS;
}
static void tx_message_extract(_u8* message,_u8* prevMessage)
{
    if(message == txMsgHdr)
    {
        txMsgHdr = TX_MESSAGE_NEXT(message);
    }
    else
    {
        TX_MESSAGE_NEXT(prevMessage) = TX_MESSAGE_NEXT(message);
    }
    TX_MESSAGE_NEXT(message)    = NULL;
    TX_MESSAGE_TASK_ID(message) = 0;
}

txMessage_e tx_message_send(_u16 destTaskId,_u8* message)
{
    if(destTaskId == 0 || message == NULL)
    {
        return TX_MESSAGE_PARAM_ERROR;
    }
    if(TX_MESSAGE_TASK_ID(message)!=0 || TX_MESSAGE_NEXT(message)!=NULL)
    {
        tx_message_deallocate(message);
    }
    TX_MESSAGE_TASK_ID(message) = destTaskId;
    if(tx_message_enqueue(message)!=TX_MESSAGE_SUCCESS)
    {
        //assert
    }
    tx_task_set_event(destTaskId,TX_TASK_EVENT_MESSAGE);
    return TX_MESSAGE_SUCCESS;
}

_u8* tx_message_receive(_u16 taskId)
{
    _u8* prevMessage = NULL;
    _u8* message     = NULL;
    _u8* list        = txMsgHdr;

    while(list!=NULL)
    {
        if(TX_MESSAGE_TASK_ID(list) == taskId)
        {
            if(message = NULL)
            {
                message = list;
            }
            else
            {
                break;
            }
        }
        if(message == NULL)
        {
            prevMessage = list;
        }
        list = TX_MESSAGE_NEXT(list);
    }
    if(list != NULL)
    {
        tx_task_set_event(taskId,TX_TASK_EVENT_MESSAGE);
    }
    else
    {
        tx_task_clear_event(taskId,TX_TASK_EVENT_MESSAGE);
    }
    if(message!=NULL)
    {
        tx_message_extract(message,prevMessage);
    }
    return message;
}