
#include"system/task/task.h"
#include"system/task//event/event.h"
#include"system/task//message/message.h"
#include"platform/platform.h"
#include"hci.h"
ble_hci_ctrl_t *hciCtrl;

typedef void(*hci_flow_type_f)(_u8*);

hci_flow_type_f hci_packet_distribute[6]=
{
	0,
    hci_command_packet_process,
	hci_acl_data_packet_process,
	0,//hci_synchronous_data_packet_process,bt use,no process now
	0,//hci_event_packet_process,host process event.
	hci_iso_packet_process,
};


/******************************ble hci tx process********************************/
void ble_hci_send_data(ble_hci_data_type_e type,_u8* data,_u32 dataLen)
{
	if(type<BLE_HCI_DATA_TYPE_MAX)
	{
		_u8* pBuffer = hciCtrl->txBuffer.getWritePointer(&hciCtrl->txBuffer);
		U32_TO_STREAM(pBuffer,(dataLen+1));
	    *pBuffer++ = (_u8)type;
		txMemcpy(pBuffer,data,dataLen);
		hciCtrl->txBuffer.wPtrIncrease(&hciCtrl->txBuffer);
	}
}

static void ble_hci_hardware_tx_irq()
{
	if(hciCtrl->txBuffer.blockAvailble(&hciCtrl->rxBuffer))
	{
		tx_task_set_event(TX_TASK_ID_HCI_CONTROLLER_TX,BLE_HCI_EVENT_TX);
	}
}
static _u32 ble_hci_tx_event(_u16 taskId,_u32 event)
{
	if(hciCtrl->txBuffer.blockAvailble(&hciCtrl->txBuffer))
	{
		_u8* pData = hciCtrl->txBuffer.getReadPointer(&hciCtrl->txBuffer);
		_u32 dataLen = 0;
        STREAM_TO_U32(dataLen,pData);
        hal_uart_send_data(pData,dataLen);
        hciCtrl->txBuffer.rPtrIncrease(&hciCtrl->txBuffer);
	}
	return event^BLE_HCI_EVENT_TX;
}

static _u32 ble_hci_tx_evet_process(_u16 taskId,_u32 event)
{
	if(event&TX_TASK_EVENT_MESSAGE)
	{
		return event^TX_TASK_EVENT_MESSAGE;//no process now,maybe use in future
	}
	else if(event&BLE_HCI_EVENT_TX)
	{
		return ble_hci_tx_event(taskId,event);
	}
	return 0;
}

static void ble_hci_tx_init()
{
    _u8* buffer = tx_malloc(BLE_HCI_TX_BUFFER_NUM*BLE_HCI_TX_BUFFER_SIZE);
    if(hciCtrl->txBuffer.database.pointer == NULL)
    {
    	//assert
    }
    txBuffer_init(&hciCtrl->txBuffer,buffer,BLE_HCI_TX_BUFFER_NUM,BLE_HCI_TX_BUFFER_SIZE);
}

/******************************ble hci rx process********************************/
void ble_hci_data_distribute(_u8* data,_u32 dataLen)
{
    if(data[0]<BLE_HCI_DATA_TYPE_MAX && hci_packet_distribute[data[0]]!=NULL)
    {
    	hci_packet_distribute[data[0]](data+1);
    }
}

static void ble_hci_hardware_rx_irq(int len)
{
	_u8* p = hciCtrl->rxBuffer.getWritePointer(&hciCtrl->rxBuffer);
	U32_TO_STREAM(p,len);
	tx_task_set_event(TX_TASK_ID_HCI_CONTROLLER_RX,BLE_HCI_EVENT_RX);
	hciCtrl->rxBuffer.wPtrIncrease(&hciCtrl->rxBuffer);
    _u8* pReveive = hciCtrl->rxBuffer.getWritePointer(&hciCtrl->rxBuffer);
    hal_uart_set_receive_buffer(pReveive+4,hciCtrl->rxBuffer.database.blockSize-4);
}

static _u32 ble_hci_rx_event(_u16 taskId,_u32 event)
{
	while(hciCtrl->rxBuffer.blockAvailble(&hciCtrl->rxBuffer))
	{
		_u8* data = hciCtrl->rxBuffer.getReadPointer(&hciCtrl->rxBuffer);
		_u32 dataLen = 0;
		STREAM_TO_U32(dataLen,data);
		ble_hci_data_distribute(data,dataLen);
		hciCtrl->rxBuffer.rPtrIncrease(&hciCtrl->rxBuffer);
	}
	return event^BLE_HCI_EVENT_RX;
}

static _u32 ble_hci_rx_event_process(_u16 taskId,_u32 event)
{
	if(event&TX_TASK_EVENT_MESSAGE)
	{
		return event^TX_TASK_EVENT_MESSAGE;//no process now
	}
	else if(event&BLE_HCI_EVENT_RX)
	{
		return ble_hci_rx_event(taskId,event);
	}
	return 0;
}

static void ble_hci_rx_init()
{
	_u8* buffer = tx_malloc(BLE_HCI_RX_BUFFER_NUM*BLE_HCI_RX_BUFFER_SIZE);
    if(hciCtrl->rxBuffer.database.pointer == NULL)
    {
    	//assert
    }
    txBuffer_init(&hciCtrl->rxBuffer,buffer,BLE_HCI_RX_BUFFER_NUM,BLE_HCI_RX_BUFFER_SIZE);
    _u8* pReveive = hciCtrl->rxBuffer.getWritePointer(&hciCtrl->rxBuffer);
    hal_uart_set_receive_buffer(pReveive+4,hciCtrl->rxBuffer.database.blockSize-4);
}

/******************************ble hci init********************************/

static void ble_hci_init(void)
{
	hal_uart_register_task(HAL_UART_BAUDRATE_1000000,ble_hci_hardware_rx_irq,ble_hci_hardware_tx_irq,HAL_UART_PARITY_NONE,HAL_UART_STOP_BITE_ONE);
    tx_task_add(ble_hci_rx_init,ble_hci_rx_event_process,TX_TASK_ID_HCI_CONTROLLER_RX,TX_TASK_PRIORITY_15);
    tx_task_add(ble_hci_tx_init,ble_hci_tx_evet_process,TX_TASK_ID_HCI_CONTROLLER_TX,TX_TASK_PRIORITY_15);
    //tx buffer malloc
    hciCtrl = (ble_hci_ctrl_t*)tx_malloc(sizeof(ble_hci_ctrl_t));
    if(hciCtrl == NULL)
    {
    	//assert
    }
}
TASK_INIT(ble_hci_init);
