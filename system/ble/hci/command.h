
#include"common/txCommon.h"

enum
{
	HCI_COMMAND_LINK_CONTROL             = 0x01,
	HCI_COMMAND_LINK_POLICY              = 0x02,
	HCI_COMMAND_CONTROLLER_BASEBAND      = 0x03,
	HCI_COMMAND_INFORMATIONAL_PARAMETERS = 0x04,
	HCI_COMMAND_STATUS_PARAMETERS        = 0x05,
	HCI_COMMAND_TESTING                  = 0x06,
	HCI_COMMAND_LE_CONTROLLER            = 0x08,
};

void hci_command_packet_process(_u8* data,_u32 dataLen);
