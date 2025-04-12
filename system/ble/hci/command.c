#include"command.h"
#include"event.h"
#include"data.h"

typedef void(*hci_command_f)(_u8* data,_u8 length);

typedef struct 
{
	hci_command_link_control_e ocf;
	hci_command_f              process;
}hci_command_link_policy_t;

static const hci_command_link_policy_t hci_command_link_control[20] = 
{
    {HCI_INQUIRY_COMMAND,NULL},
    {},
}


static void hci_command_link_control(_u16 ocf,_u8 length,_u8* data)
{

}

static void hci_command_link_policy(_u16 ocf,_u8 length,_u8* data)
{

}

static void hci_command_controller_baseband(_u16 ocf,_u8 length,_u8* data)
{

}

static void hci_command_information(_u16 ocf,_u8 length,_u8* data)
{

}

static void hci_command_status(_u16 ocf,_u8 length,_u8* data)
{

}


static void hci_command_testing(_u16 ocf,_u8 length,_u8* data)
{

}

static void hci_command_le_controller(_u16 ocf,_u8 length,_u8* data)
{

}

void hci_command_packet_process(_u8* data)
{
	bt_hci_command_t* hciCommand = (bt_hci_command_t*)data;
	switch(hciCommand->ogf)
	{
		case HCI_COMMAND_LINK_CONTROL://link control commands
			hci_command_link_control(hciCommand->ocf,hciCommand->length,hciCommand->data);
			break;
		case HCI_COMMAND_LINK_POLICY://link policy commands
			hci_command_link_policy(hciCommand->ocf,hciCommand->length,hciCommand->data);
			break;
		case HCI_COMMAND_CONTROLLER_BASEBAND://controller&baseband commands
			hci_command_controller_baseband(hciCommand->ocf,hciCommand->length,hciCommand->data);
			break;
		case HCI_COMMAND_INFORMATIONAL_PARAMETERS://information parameters
			hci_command_information(hciCommand->ocf,hciCommand->length,hciCommand->data);
			break;
		case HCI_COMMAND_STATUS_PARAMETERS://status parameters
			hci_command_status(hciCommand->ocf,hciCommand->length,hciCommand->data);
			break;
		case HCI_COMMAND_TESTING://testing commands
			hci_command_testing(hciCommand->ocf,hciCommand->length,hciCommand->data);
			break;
		case HCI_COMMAND_LE_CONTROLLER://le controller commands
			hci_command_le_controller(hciCommand->ocf,hciCommand->length,hciCommand->data);
			break;
		default:
			break;
	}
}
