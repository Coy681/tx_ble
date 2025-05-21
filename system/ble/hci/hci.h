#ifndef HCI_H_
#define HCI_H_

#include"../controller/ll/ll.h"
#include"data.h"
#include"command_event.h"


#ifndef BLE_HCI_TX_BUFFER_NUM
#define BLE_HCI_TX_BUFFER_NUM 8
#endif

#ifndef BLE_HCI_RX_BUFFER_NUM
#define BLE_HCI_RX_BUFFER_NUM 8
#endif

#ifndef BLE_HCI_TX_BUFFER_SIZE
#define BLE_HCI_TX_BUFFER_SIZE 256
#endif

#ifndef BLE_HCI_RX_BUFFER_SIZE
#define BLE_HCI_RX_BUFFER_SIZE 256
#endif

#define BLE_HCI_EVENT_TX       BIT(0)
#define BLE_HCI_EVENT_RX       BIT(0)

typedef enum
{
	BLE_HCI_COMMAND_PACKET          = 0x01,
	BLE_HCI_ACL_DATA_PACKET         = 0x02,
	BLE_HCI_SYNCHRONOUS_DATA_PACKET = 0x03,
	BLE_HCI_EVENT_PACKET            = 0x04,
	BLE_HCI_ISO_DATA_PACKET         = 0x05,
	BLE_HCI_DATA_TYPE_MAX           = 0x06,
}ble_hci_data_type_e;

typedef struct _PACKET
{
	txBuffer_t txBuffer;
	txBuffer_t rxBuffer;
}ble_hci_ctrl_t;

void ble_hci_send_data(ble_hci_data_type_e type,_u8* data,_u32 dataLen);

#endif/*HCI_H_*/