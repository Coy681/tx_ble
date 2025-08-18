#include"common/txCommon.h"   
#ifndef HCI_COMMAND_H_
#define HCI_COMMAND_H_
typedef enum
{
	HCI_COMMAND_LINK_CONTROL             = 0x01,
	HCI_COMMAND_LINK_POLICY              = 0x02,
	HCI_COMMAND_CONTROLLER_BASEBAND      = 0x03,
	HCI_COMMAND_INFORMATIONAL_PARAMETERS = 0x04,
	HCI_COMMAND_STATUS_PARAMETERS        = 0x05,
	HCI_COMMAND_TESTING                  = 0x06,
	HCI_COMMAND_LE_CONTROLLER            = 0x08,
    HCI_COMMAND_VENDOR_SPECIFIC          = 0x3f,
}hci_command_e;

typedef enum
{
	HCI_INQUIRY_COMMAND                                              = 0x01,
    // The HCI_Inquiry command will cause the BR/EDR Controller to enter Inquiry Mode. Inquiry Mode is used to discovery other nearby BR/EDR Controllers.
	HCI_INQUIRY_CANCEL_COMMAND                                       = 0x02,
    // The HCI_Inquiry_Cancel command will cause the BR/EDR Controller to stop the current Inquiry if the BR/EDR Controller is in Inquiry Mode.
	HCI_PERIODIC_INQUIRY_MODE_COMMAND                                = 0x03,  
	HCI_EXIT_PERIODIC_INQUIRY_MODE_COMMAND                           = 0x04,
    // The HCI_Exit_Periodic_Inquiry_Mode command is used to end the Periodic Inquiry mode when the local device is in Periodic Inquiry Mode.   
	HCI_CREATE_CONNECTION_COMMAND                                    = 0x05,  
    // The HCI_Create_Connection command will cause the BR/EDR Link Manager to create an ACL connection to the BR/EDR Controller with the BD_ADDR specified by the parameters. 
	HCI_DISCONNECT_COMMAND                                           = 0x06,   
    // The HCI_Disconnect command is used to terminate an existing BR/EDR or LE connection.
	HCI_CREATE_CONNECTION_CANCEL_COMMAND                             = 0x08,  
    // The HCI_Create_Connection_Cancel command is used to cancel an ongoing Create Connection. 
	HCI_ACCEPT_CONNECTION_REQUEST_COMMAND                            = 0x09,
    // The HCI_Accept_Connection_Request command is used to accept a new incoming BR/EDR connection request.   
	HCI_REJECT_CONNECTION_REQUEST_COMMAND                            = 0x0A,   
	HCI_LINK_KEY_REQUEST_REPLY_COMMAND                               = 0x0B,   
	HCI_LINK_KEY_REQUEST_NEGATIVE_REPLY_COMMAND                      = 0x0C,   
	HCI_PIN_CODE_REQUEST_REPLY_COMMAND                               = 0x0D,   
	HCI_PIN_CODE_REQUEST_NEGATIVE_REPLY_COMMAND                      = 0x0E,  
	HCI_CHANGE_CONNECTION_PACKET_TYPE_COMMAND                        = 0x0F, 
     // The HCI_Change_Connection_Packet_Type command is used to change which packet types can be used for a connection that is currently established.  
	HCI_AUTHENTICATION_REQUESTED_COMMAND                             = 0x11,  
    // The HCI_Authentication_Requested command is used to establish authentication between the two devices associated with the specified Connection_Handle.
	HCI_SET_CONNECTION_ENCRYPTION_COMMAND                            = 0x13,  
	HCI_CHANGE_CONNECTION_LINK_KEY_COMMAND                           = 0x15,   
    // The HCI_Change_Connection_Link_Key command is used to force both devices of a connection associated to the Connection_Handle, to generate a new link key.
	HCI_LINK_KEY_SELECTION_COMMAND                                   = 0x17,
	HCI_REMOTE_NAME_REQUEST_COMMAND                                  = 0x19,
	HCI_READ_REMOTE_SUPPORTED_FEATURES_COMMAND                       = 0x1A,   
	HCI_READ_REMOTE_EXTENDED_FEATURES_COMMAND                        = 0x1C,   
	HCI_READ_REMOTE_VERSION_INFORMATION_COMMAND                      = 0x1D, 
	HCI_READ_CLOCK_OFFSET_COMMAND                                    = 0x1F,   
	HCI_READ_LMP_HANDLE_COMMAND                                      = 0x20,   
	HCI_SETUP_SYNCHRONOUS_CONNECTION_COMMAND                         = 0x28,   
	HCI_ACCEPT_SYNCHRONOUS_CONNECTION_REQUEST_COMMAND                = 0x29,
    // The HCI_Accept_Synchronous_Connection_Request command is used to accept an incoming request for a synchronous connection and to inform the local Link Manager about the acceptable parameter values for the synchronous connection.
	HCI_REJECT_SYNCHRONOUS_CONNECTION_REQUEST_COMMAND                = 0x2A, 
	HCI_IO_CAPABILITY_REQUEST_REPLY_COMMAND                          = 0x2B,   
    // The HCI_IO_Capability_Request_Reply command is used to reply to an HCI_IO_Capability_Request event from the Controller, and specifies the current IO capabilities of the Host.
	HCI_USER_CONFIRMATION_REQUEST_REPLY_COMMAND                      = 0x2C,   
	HCI_USER_CONFIRMATION_REQUEST_NEGATIVE_REPLY_COMMAND             = 0x2D, 
	HCI_USER_PASSKEY_REQUEST_REPLY_COMMAND                           = 0x2E,   
	HCI_USER_PASSKEY_REQUEST_NEGATIVE_REPLY_COMMAND                  = 0x2F, 
	HCI_REMOTE_OOB_DATA_REQUEST_REPLY_COMMAND                        = 0x30,   
	HCI_REMOTE_OOB_DATA_REQUEST_NEGATIVE_REPLY_COMMAND               = 0x33, 
	HCI_IO_CAPABILITY_REQUEST_NEGATIVE_REPLY_COMMAND                 = 0x34, 
    // The HCI_IO_Capability_Request_Negative_Reply command is used to reject a pairing attempt after an HCI_IO_Capability_Request event has been received by the Host.
    HCI_ENHANCED_SETUP_SYNCHRONOUS_CONNECTION_COMMAND                = 0x3D,
    // The HCI_Enhanced_Setup_Synchronous_Connection command adds a new or modifies an existing synchronous logical transport (SCO or eSCO) on a physical link depending on the Connection_Handle parameter specified. 
    HCI_ENHANCED_ACCEPT_SYNCHRONOUS_CONNECTION_REQUEST_COMMAND       = 0x3E,
    // The HCI_Enhanced_Accept_Synchronous_Connection_Request command is used to accept an incoming request for a synchronous connection and to inform the local Link Manager about the acceptable parameter values for the synchronous connection. 
    HCI_TRUNCATED_PAGE_COMMAND                                       = 0x3F,   
    HCI_TRUNCATED_PAGE_CANCEL_COMMAND                                = 0x40,   
    HCI_SET_CONNECTIONLESS_PERIPHERAL_BROADCAST_COMMAND              = 0x41,
	HCI_SET_CONNECTIONLESS_PERIPHERAL_BROADCAST_RECEIVE_COMMAND      = 0x42,
    HCI_START_SYNCHRONIZATION_TRAIN_COMMAND                          = 0x43,   
    HCI_RECEIVE_SYNCHRONIZATION_TRAIN_COMMAND                        = 0x44,   
    HCI_REMOTE_OOB_EXTENSION_DATA_REQUEST_REPLY_COMMAND              = 0x45,   
}hci_command_link_control_e;

typedef enum
{
	HCI_HOLD_MODE_COMMAND                                            = 0x01,
    // The HCI_Hold_Mode command is used to initiate Hold mode.
	HCI_SNIFF_MODE_COMMAND                                           = 0x03,
	HCI_EXIT_SNIFF_MODE_COMMAND                                      = 0x04,
    // The HCI_Exit_Sniff_Mode command is used to end Sniff mode for a Connection_Handle which is currently in Sniff mode.
	HCI_QOS_SETUP_COMMAND                                            = 0x07,
	HCI_ROLE_DISCOVERY_COMMAND                                       = 0x09,
	HCI_SWITCH_ROLE_COMMAND                                          = 0x0b,
	HCI_READ_LINK_POLICY_SETTINGS_COMMAND                            = 0x0c,
	HCI_WRITE_LINK_POLICY_SETTINGS_COMMAND                           = 0x0d,
	HCI_READ_DEFAULT_LINK_POLICY_SETTINGS_COMMAND                    = 0x0e,
	HCI_WRITE_DEFAULT_LINK_POLICY_SETTINGS_COMMAND                   = 0x0f,
	HCI_FLOW_SPECIFICATION_COMMAND                                   = 0x10,
    // The HCI_Flow_Specification command is used to specify the flow parameters for the traffic carried over the ACL connection identified by the Connection_Handle.
	HCI_SNIFF_SUBRATING_COMMAND                                      = 0x11,
}hci_command_link_policy_e;

typedef enum
{
    HCI_SET_EVENT_MASK_COMMAND                                       = 0x01,
    HCI_RESET_COMMAND                                                = 0x03,
    HCI_SET_EVENT_FILTER_COMMAND                                     = 0x05,
    HCI_FLUSH_COMMAND                                                = 0x08,
    // The HCI_Flush command is used to discard all data that is currently pending for transmission in the Controller for the specified Connection_Handle.
    HCI_READ_PIN_TYPE_COMMAND                                        = 0x09,
    HCI_WRITE_PIN_TYPE_COMMAND                                       = 0x0a,
    HCI_READ_STORED_LINK_KEY_COMMAND                                 = 0x0d,
    HCI_WRITE_STORED_LINK_KEY_COMMAND                                = 0x11,
    HCI_DELETE_STORED_LINK_KEY_COMMAND                               = 0x12,
    // The HCI_Delete_Stored_Link_Key command provides the ability to remove one or more of the link keys stored in the Controller.
    HCI_WRITE_LOCAL_NAME_COMMAND                                     = 0x13,
    HCI_READ_LOCAL_NAME_COMMAND                                      = 0x14,
    HCI_READ_CONNECTION_ACCEPT_TIMEOUT_COMMAND                       = 0x15,
    HCI_WRITE_CONNECTION_ACCEPT_TIMEOUT_COMMAND                      = 0x16,
    HCI_READ_PAGE_TIMEOUT_COMMAND                                    = 0x17,
    HCI_WRITE_PAGE_TIMEOUT_COMMAND                                   = 0x18,
    HCI_READ_SCAN_ENABLE_COMMAND                                     = 0x19,
    HCI_WRITE_SCAN_ENABLE_COMMAND                                    = 0x1a,
    HCI_READ_PAGE_SCAN_ACTIVITY_COMMAND                              = 0x1b,
    HCI_WRITE_PAGE_SCAN_ACTIVITY_COMMAND                             = 0x1c,
    HCI_READ_INQUIRY_SCAN_ACTIVITY_COMMAND                           = 0x1d,
    HCI_WRITE_INQUIRY_SCAN_ACTIVITY_COMMAND                          = 0x1e,
    HCI_READ_AUTHENTICATION_ENABLE_COMMAND                           = 0x1f,
    HCI_WRITE_AUTHENTICATION_ENABLE_COMMAND                          = 0x20,
    HCI_READ_CLASS_OF_DEVICE_COMMAND                                 = 0x23,
    HCI_WRITE_CLASS_OF_DEVICE_COMMAND                                = 0x24,
    HCI_READ_VOICE_SETTING_COMMAND                                   = 0x25,
    HCI_WRITE_VOICE_SETTING_COMMAND                                  = 0x26,
    HCI_READ_AUTOMATIC_FLUSH_TIMEOUT_COMMAND                         = 0x27,
    HCI_WRITE_AUTOMATIC_FLUSH_TIMEOUT_COMMAND                        = 0x28,
    HCI_READ_NUM_BROADCAST_RETRANSMISSIONS_COMMAND                   = 0x29,
    HCI_WRITE_NUM_BROADCAST_RETRANSMISSIONS_COMMAND                  = 0x2a,
    HCI_READ_HOLD_MODE_ACTIVITY_COMMAND                              = 0x2b,
    HCI_WRITE_HOLD_MODE_ACTIVITY_COMMAND                             = 0x2c,
    HCI_READ_TRANSMIT_POWER_LEVEL_COMMAND                            = 0x2d,
    HCI_READ_SYNCHRONOUS_FLOW_CONTROL_ENABLE_COMMAND                 = 0x2e,
    HCI_WRITE_SYNCHRONOUS_FLOW_CONTROL_ENABLE_COMMAND                = 0x2f,
    HCI_SET_CONTROLLER_TO_HOST_FLOW_CONTROL_COMMAND                  = 0x31,
    HCI_HOST_BUFFER_SIZE_COMMAND                                     = 0x33,
    // The HCI_Host_Buffer_Size command is used by the Host to notify the Controller about its buffer sizes for ACL and synchronous data. 
    // The Controller will fragment the data to be transmitted from the Controller to the Host, so that data contained in HCI Data packets will not exceed these sizes.
    HCI_HOST_NUMBER_OF_COMPLETED_PACKETS_COMMAND                     = 0x35,
    // The HCI_Host_Number_Of_Completed_Packets command is used by the Host to indicate to the Controller when the Host is ready to receive more HCI packets for any Connection_Handle.
    HCI_READ_LINK_SUPERVISION_TIMEOUT_COMMAND                        = 0x36,
    HCI_WRITE_LINK_SUPERVISION_TIMEOUT_COMMAND                       = 0x37,
    HCI_READ_NUMBER_OF_SUPPORTED_IAC_COMMAND                         = 0x38,
    HCI_READ_CURRENT_IAC_LAP_COMMAND                                 = 0x39,
    HCI_WRITE_CURRENT_IAC_LAP_COMMAND                                = 0x3a,
    HCI_SET_AFH_HOST_CHANNEL_CLASSIFICATION_COMMAND                  = 0x3f,
    HCI_READ_INQUIRY_SCAN_TYPE_COMMAND                               = 0x42,
    HCI_WRITE_INQUIRY_SCAN_TYPE_COMMAND                              = 0x43,
    HCI_READ_INQUIRY_MODE_COMMAND                                    = 0x44,
    HCI_WRITE_INQUIRY_MODE_COMMAND                                   = 0x45,
    HCI_READ_PAGE_SCAN_TYPE_COMMAND                                  = 0x46,
    HCI_WRITE_PAGE_SCAN_TYPE_COMMAND                                 = 0x47,
    HCI_READ_AFH_CHANNEL_ASSESSMENT_MODE_COMMAND                     = 0x48,
    HCI_WRITE_AFH_CHANNEL_ASSESSMENT_MODE_COMMAND                    = 0x49,
    HCI_READ_EXTENDED_INQUIRY_RESPONSE_COMMAND                       = 0x51,
    HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_COMMAND                      = 0x52,
    HCI_REFRESH_ENCRYPTION_KEY_COMMAND                               = 0x53,
    HCI_READ_SIMPLE_PAIRING_MODE_COMMAND                             = 0x55,
    HCI_WRITE_SIMPLE_PAIRING_MODE_COMMAND                            = 0x56,
    HCI_READ_LOCAL_OOB_DATA_COMMAND                                  = 0x57,
    HCI_READ_INQUIRY_RESPONSE_TRANSMIT_POWER_LEVEL_COMMAND           = 0x58,
	HCI_WRITE_INQUIRY_TRANSMIT_POWER_LEVEL_COMMAND                   = 0x59,
	HCI_SEND_KEYPRESS_NOTIFICATION_COMMAND                           = 0x60,
	HCI_READ_DEFAULT_ERRONEOUS_DATA_REPORTING_COMMAND                = 0x5a,
	HCI_WRITE_DEFAULT_ERRONEOUS_DATA_REPORTING_COMMAND               = 0x5b,
	HCI_ENHANCED_FLUSH_COMMAND                                       = 0x5f,
    // The HCI_Enhanced_Flush command is used to discard specific packets currently pending for transmission in the Controller for the specified Handle. This command takes a parameter specifying the type of packets to be flushed.
    HCI_SET_EVENT_MASK_PAGE2_COMMAND                                 = 0x63,
    HCI_READ_FLOW_CONTROL_MODE_COMMAND                               = 0x66,
    HCI_WRITE_FLOW_CONTROL_MODE_COMMAND                              = 0x67,
    HCI_READ_ENHANCED_TRANSMIT_POWER_LEVEL_COMMAND                   = 0x68,
    HCI_READ_LE_HOST_SUPPORT_COMMAND                                 = 0x6c,
    HCI_WRITE_LE_HOST_SUPPORT_COMMAND                                = 0x6d,
    HCI_SET_MWS_CHANNEL_PARAMETERS_COMMAND                           = 0x6e,
    HCI_SET_EXTERNAL_FRAME_CONFIGURATION_COMMAND                     = 0x6f,
    HCI_SET_MWS_SIGNALING_COMMAND                                    = 0x70,
    HCI_SET_MWS_TRANSPORT_LAYER_COMMAND                              = 0x71,
    HCI_SET_MWS_SCAN_FREQUENCY_TABLE_COMMAND                         = 0x72,
    HCI_SET_MWS_PATTERN_CONFIGURATION_COMMAND                        = 0x73,
    HCI_SET_RESERVED_LT_ADDR_COMMAND                                 = 0x74,
    HCI_DELETE_RESERVED_LT_ADDR_COMMAND                              = 0x75,
    // The HCI_Delete_Reserved_LT_ADDR command requests that the BR/EDR Controller cancel the reservation of a specific LT_ADDR reserved for the purposes of Connectionless Peripheral Broadcast.
    HCI_SET_CONNECTIONLESS_PERIPHERAL_BROADCAST_DATA_COMMAND         = 0x76,
    HCI_READ_SYNCHRONIZATION_TRAIN_PARAMETERS_COMMAND                = 0x77,
    HCI_WRITE_SYNCHRONIZATION_TRAIN_PARAMETERS_COMMAND               = 0x78,
    HCI_READ_SECURE_CONNECTIONS_HOST_SUPPORT_COMMAND                 = 0x79,
    HCI_WRITE_SECURE_CONNECTIONS_HOST_SUPPORT_COMMAND                = 0x7A,
    HCI_READ_AUTHENTICATED_PAYLOAD_TIMEOUT_COMMAND                   = 0x7B,
    HCI_WRITE_AUTHENTICATED_PAYLOAD_TIMEOUT_COMMAND                  = 0x7C,
    HCI_READ_LOCAL_OOB_EXTENDED_DATA_COMMAND                         = 0x7D,
    HCI_READ_EXTENDED_PAGE_TIMEOUT_COMMAND                           = 0x7E,
    HCI_WRITE_EXTENDED_PAGE_TIMEOUT_COMMAND                          = 0x7F,
    HCI_READ_EXTENDED_INQUIRY_LENGTH_COMMAND                         = 0x80,
    HCI_WRITE_EXTENDED_INQUIRY_LENGTH_COMMAND                        = 0x81,
    HCI_SET_ECOSYSTEM_BASE_INTERVAL_COMMAND                          = 0x82,
    HCI_CONFIGURE_DATA_PATH_COMMAND                                  = 0x83,
    // The HCI_Configure_Data_Path command is used by a Host to configure a data path to enable codec operation in the Controller.
    HCI_SET_MIN_ENCRYPTION_KEY_SIZE_COMMAND                          = 0x84,
}hci_command_controller_baseband_e;

typedef enum
{
	HCI_READ_LOCAL_VERSION_INFORMATION_COMMAND                       = 0x01,
    HCI_READ_LOCAL_SUPPORTED_COMMANDS_COMMAND                        = 0x02,
    HCI_READ_LOCAL_SUPPORTED_FEATURES_COMMAND                        = 0x03,
    HCI_READ_LOCAL_EXTENDED_FEATURES_COMMAND                         = 0x04,
    HCI_READ_BUFFER_SIZE_COMMAND                                     = 0x05,
    HCI_READ_BDADDR_COMMAND                                          = 0x09,
    HCI_READ_DATA_BLOCK_SIZE_COMMAND                                 = 0x0a,
    HCI_READ_LOCAL_SUPPORTED_CODECS_COMMAND                          = 0x0b,
	HCI_READ_LOCAL_SUPPORTED_CODECS_COMMAND_V2                       = 0x0d,
    HCI_READ_LOCAL_SIMPLE_PAIRING_OPTIONS_COMMAND                    = 0x0c,
    HCI_READ_LOCAL_SUPPORTED_CODEC_CAPABILITIES_COMMAND              = 0x0e,
    HCI_READ_LOCAL_SUPPORTED_CONTROLLER_DELAY_COMMAND                = 0x0f,
}hci_command_informational_parameters_e;


typedef enum {
    HCI_READ_FAILED_CONTACT_COUNTER_COMMAND              = 0x01,
    HCI_RESET_FAILED_CONTACT_COUNTER_COMMAND             = 0x02,
    HCI_READ_LINK_QUALITY_COMMAND                        = 0x03,
    HCI_READ_RSSI_COMMAND                                = 0x05,
    HCI_READ_AFH_CHANNEL_MAP_COMMAND                     = 0x06,
    HCI_READ_CLOCK_COMMAND                               = 0x07,
    HCI_READ_ENCRYPTION_KE_SIZE_COMMAND                  = 0x08,
    HCI_GET_MWS_TRANSPORT_LAYER_CONFIGURATION_COMMAND    = 0x0C,
    // The HCI_Get_MWS_Transport_Layer_Configuration command reads the supported baud rates from the Controller.
    HCI_SET_TRIGGERED_CLOCK_CAPTURE_COMMAND              = 0x0D,
} hci_command_status_parameters_e;

typedef enum{
	HCI_READ_LOOPBACK_MODE_COMMAND                       = 0x01,
	HCI_WRITE_LOOPBACK_MODE_COMMAND                      = 0x02,
	HCI_ENABLE_IMPLEMENTATION_UNDER_TEST_MODE_COMMAND    = 0x03,
    // The HCI_Enable_Implementation_Under_Test_Mode command will allow the local Controller to enter test mode via LMP test commands. 
    // The Host issues this command when it wants the local device to be the IUT for the Testing scenarios as described in the Bluetooth Test Mode document.
	HCI_WRITE_SIMPLE_PAIRING_DEBUG_MODE_COMMAND          = 0x04,
	HCI_WRITE_SECURE_CONNECTIONS_TEST_MODE_COMMAND       = 0x0a,
}hci_command_testing_e;


typedef enum 
{
    HCI_LE_SET_EVENT_MASK_COMMAND                                       = 0x01,
    // The HCI_LE_Set_Event_Mask command is used to control which events are generated by the HCI for the Host.
    HCI_LE_READ_BUFFER_SIZE_COMMAND                                     = 0x02,
	HCI_LE_READ_BUFFER_SIZE_COMMAND_V2                                  = 0x60,
    // The HCI_LE_Read_Buffer_Size command returns the size of the HCI buffers. These buffers are used by the LE Controller to buffer data that is to be transmitted.
    HCI_LE_READ_LOCAL_SUPPORTED_FEATURES_PAGE_0_COMMAND                  = 0x03,
    // The HCI_LE_Read_Local_Supported_Features_Page_0 command will read page 0 of the features supported by the local LE Controller.
    HCI_LE_SET_RANDOM_ADDRESS_COMMAND                                   = 0x05,
    // The HCI_LE_Set_Random_Address command will set the Random Device Address that may be used in a packet sent on the advertising physical channel.
    HCI_LE_SET_ADVERTISING_PARAMETERS_COMMAND                           = 0x06,
    // The HCI_LE_Set_Advertising_Parameters command will set the parameters used for advertising.
    HCI_LE_READ_ADVERTISING_PHYSICAL_CHANNEL_TX_POWER_COMMAND           = 0x07,
    // The HCI_LE_Read_Advertising_Physical_Channel_Tx_Power command will read the transmit power level that will be used for advertising.
    HCI_LE_SET_ADVERTISING_DATA_COMMAND                                 = 0x08,
    // The HCI_LE_Set_Advertising_Data command will set the data transmitted when advertising.
    HCI_LE_SET_SCAN_RESPONSE_DATA_COMMAND                               = 0x09,
    // The HCI_LE_Set_Scan_Response_Data command will set the data transmitted in a scan response.
    HCI_LE_SET_ADVERTISING_ENABLE_COMMAND                               = 0x0a,
    // The HCI_LE_Set_Advertising_Enable command will enable or disable advertising.
    HCI_LE_SET_SCAN_PARAMETERS_COMMAND                                  = 0x0b,
    // The HCI_LE_Set_Scan_Parameters command will set the parameters used for scanning.
    HCI_LE_SET_SCAN_ENABLE_COMMAND                                      = 0x0c,
    // The HCI_LE_Set_Scan_Enable command will enable or disable scanning.
    HCI_LE_CREATE_CONNECTION_COMMAND                                    = 0x0d,
    // The HCI_LE_Create_Connection command is used to create a new connection.
    HCI_LE_CREATE_CONNECTION_CANCEL_COMMAND                             = 0x0e,
    // The HCI_LE_Create_Connection_Cancel command is used to cancel an ongoing HCI_LE_Create_Connection command.
    HCI_LE_READ_FILTER_ACCEPT_LIST_SIZE_COMMAND                         = 0x0f,
    // The HCI_LE_Read_Filter_Accept_List_Size command will read the maximum number of Filter Accept List entries that this Controller supports.
    HCI_LE_CLEAR_FILTER_ACCEPT_LIST_COMMAND                             = 0x10,
    // The HCI_LE_Clear_Filter_Accept_List command will clear the Filter Accept List.
    HCI_LE_ADD_DEVICE_TO_FILTER_ACCEPT_LIST_COMMAND                     = 0x11,
    // The HCI_LE_Add_Device_To_Filter_Accept_List command will add a device to the Filter Accept List.
    HCI_LE_REMOVE_DEVICE_FROM_FILTER_ACCEPT_LIST_COMMAND                = 0x12,
    // The HCI_LE_Remove_Device_From_Filter_Accept_List command will remove a single device from the Filter Accept List.
    HCI_LE_CONNECTION_UPDATE_COMMAND                                    = 0x13,
    // The HCI_LE_Connection_Update command will be used to change the connection parameters of an existing connection.
    HCI_LE_SET_HOST_CHANNEL_CLASSIFICATION_COMMAND                      = 0x14,
    // The HCI_LE_Set_Host_Channel_Classification command allows the Host to specify a channel classification based on its "local information".
    HCI_LE_READ_CHANNEL_MAP_COMMAND                                     = 0x15,
    // The HCI_LE_Read_Channel_Map command will read the current state of the channel map for a connection.
    HCI_LE_READ_REMOTE_FEATURES_PAGE_0_COMMAND                          = 0x16,
    // The HCI_LE_Read_Remote_Features_Page_0 command is used to read page 0 of the features used on a connection and the features supported by a remote LE device.
    HCI_LE_ENCRYPT_COMMAND                                              = 0x17,
    // The HCI_LE_Encrypt command will encrypt a block of unencrypted data against a key and generate a block of encrypted data.
    HCI_LE_RAND_COMMAND                                                 = 0x18,
    // The HCI_LE_Rand command will generate a random number.
    HCI_LE_ENABLE_ENCRYPTION_COMMAND                                    = 0x19,
    // The HCI_LE_Enable_Encryption command is used to enable link level encryption.
    HCI_LE_LONG_TERM_KEY_REQUEST_REPLY_COMMAND                          = 0x1a,
    // The HCI_LE_Long_Term_Key_Request_Reply command is used to reply to an HCI_LE_Long_Term_Key_Request event and includes the Long Term Key stored in the Host for that connection.
    HCI_LE_LONG_TERM_KEY_REQUEST_NEGATIVE_REPLY_COMMAND                 = 0x1b,
    // The HCI_LE_Long_Term_Key_Request_Negative_Reply command is used to reply to an HCI_LE_Long_Term_Key_Request event and indicates that the Host does not have a Long Term Key for that connection.
    HCI_LE_READ_SUPPORTED_STATES_COMMAND                                = 0x1c,
    // The HCI_LE_Read_Supported_States command will read the current supported state and role combinations for the local LE Controllers.
    HCI_LE_RECEIVER_TEST_COMMAND                                        = 0x1d,
	HCI_LE_RECEIVER_TEST_COMMAND_V2                                     = 0x33,
	HCI_LE_RECEIVER_TEST_COMMAND_V3                                     = 0x4f,
    // The HCI_LE_Receiver_Test command will run the LE receiver test.
    HCI_LE_TRANSMITTER_TEST_COMMAND                                     = 0x1e,
	HCI_LE_TRANSMITTER_TEST_COMMAND_V2                                  = 0x34,
	HCI_LE_TRANSMITTER_TEST_COMMAND_V3                                  = 0x50,
	HCI_LE_TRANSMITTER_TEST_COMMAND_V4                                  = 0x7B,
    // The HCI_LE_Transmitter_Test command will run the LE transmitter test.
    HCI_LE_TEST_END_COMMAND                                             = 0x1f,
    // The HCI_LE_Test_End command will end the current the receiver or transmitter test.
    HCI_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_REPLY_COMMAND            = 0x20,
    // The HCI_LE_Remote_Connection_Parameter_Request_Reply command is used to accept the remote device’s request to change the connection parameters of the LE connection.
    HCI_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_NEGATIVE_REPLY_COMMAND   = 0x21,
    // The HCI_LE_Remote_Connection_Parameter_Request_Negative_Reply command is used to reject the remote device’s request to change the connection parameters of the LE connection.
    HCI_LE_SET_DATA_LENGTH_COMMAND                                      = 0x22,
    // The HCI_LE_Set_Data_Length command is used to suggest maximum packet sizes to the Controller.
    HCI_LE_READ_SUGGESTED_DEFAULT_DATA_LENGTH_COMMAND                   = 0x23,
    // The HCI_LE_Read_Suggested_Default_Data_Length command allows the Host to read the initial MaxTxOctets and MaxTxTime values for new connections it suggested to the Controller.
    HCI_LE_WRITE_SUGGESTED_DEFAULT_DATA_LENGTH_COMMAND                  = 0x24,
    // The HCI_LE_Write_Suggested_Default_Data_Length command allows the Host to suggest initial MaxTxOctets and MaxTxTime values for new connections.
    HCI_LE_READ_LOCAL_P_256_PUBLIC_KEY_COMMAND                          = 0x25,
    // The HCI_LE_Read_Local_P-256_Public_Key command is used to return the local P-256 public key from the Controller.
    HCI_LE_GENERATE_DHKEY_CMMMMAND                                      = 0x26,
	HCI_LE_GENERATE_DHKEY_CMMMMAND_V2                                   = 0x5e,
    // The HCI_LE_Generate_DHKey command is used to initiate generation of a Diffie-Hellman key in the Controller for use over the LE transport.
    HCI_LE_ADD_DEVICE_TO_RESOLVING_LIST_COMMAND                         = 0x27,
    // The HCI_LE_Add_Device_To_Resolving_List command is used to add one device to the resolving list used to resolve Resolvable Private Addresses in the Controller.
    HCI_LE_REMOVE_DEVICE_FROM_RESOLVING_LIST_COMMAND                    = 0x28,
    // The HCI_LE_Remove_Device_From_Resolving_List command is used to remove one device from the resolving list used to resolve Resolvable Private Addresses in the Controller.
    HCI_LE_CLEAR_RESOLVING_LIST_COMMAND                                 = 0x29,
    // The HCI_LE_Clear_Resolving_List command is used to remove all devices from the resolving list used to resolve Resolvable Private Addresses in the Controller.
    HCI_LE_READ_RESOLVING_LIST_SIZE_COMMAND                             = 0x2a,
    // The HCI_LE_Read_Resolving_List_Size command is used to read the total number of entries in the resolving list that can be stored in the Controller.
    HCI_LE_READ_PEER_RESOLVABLE_ADDRESS_COMMAND                         = 0x2b,
    // The HCI_LE_Read_Peer_Resolvable_Address command is used to get the current peer Resolvable Private Address being used for the corresponding peer Public and Random (static) Identity Address.
    HCI_LE_READ_LOCAL_RESOLVABLE_ADDRESS_COMMAND                        = 0x2c,
    // The HCI_LE_Read_Local_Resolvable_Address command is used to get the current local Resolvable Private Address being used for the corresponding peer Identity Address.
    HCI_LE_SET_ADDRESS_RESOLUTION_ENABLE_COMMAND                        = 0x2d,
    // The HCI_LE_Set_Address_Resolution_Enable command is used to enable resolution of Resolvable Private Addresses in the Controller.
    HCI_LE_SET_RESOLVABLE_PRIVATE_ADDRESS_TIMEOUT_COMMAND               = 0x2e,
    // The HCI_LE_Set_Resolvable_Private_Address_Timeout sets the length of time the Controller uses a random private address before a new random private address is generated and starts being used.
    HCI_LE_READ_MAXIMUM_DATA_LENGTH_COMMAND                             = 0x2f,
    // The HCI_LE_Read_Maximum_Data_Length command allows the Host to read the Controller’s supportedMaxTxOctets, supportedMaxTxTime, supportedMaxRxOctets, and supportedMaxRxTime parameters.
    HCI_LE_READ_PHY_COMMAND                                             = 0x30,
    // The HCI_LE_Read_PHY command will read the current PHY.
    HCI_LE_SET_DEFAULT_PHY_COMMAND                                      = 0x31,
    // The HCI_LE_Set_Default_PHY command is used to configure preferred PHYs for new connections for the local device.
    HCI_LE_SET_PHY_COMMAND                                              = 0x32,
    // The HCI_LE_Set_PHY command is used to request a change of the PHY for a Connection_Handle.
    HCI_LE_SET_ADVERTISING_SET_RANDOM_ADDRESS_COMMAND                   = 0x35,
    // The HCI_LE_Set_Advertising_Set_Random_Address command will set the random address used in advertising.
    HCI_LE_SET_EXTENDED_ADVERTISING_PARAMETERS_COMMAND                  = 0x36,
	HCI_LE_SET_EXTENDED_ADVERTISING_PARAMETERS_COMMAND_V2               = 0x7f,
    // The HCI_LE_Set_Extended_Advertising_Parameters command will set the parameters used for advertising.
    HCI_LE_SET_EXTENDED_ADVERTISING_DATA_COMMAND                        = 0x37,
    // The HCI_LE_Set_Extended_Advertising_Data command will set the advertising data transmitted when advertising.
    HCI_LE_SET_EXTENDED_SCAN_RESPONSE_DATA_COMMAND                      = 0x38,
    // The HCI_LE_Set_Extended_Scan_Response_Data command will set the data transmitted in a scan response.
    HCI_LE_SET_EXTENDED_ADVERTISING_ENABLE_COMMAND                      = 0x39,
    // The HCI_LE_Set_Extended_Advertising_Enable command will enable or disable advertising.
    HCI_LE_READ_MAXIMUM_ADVERTISING_DATA_LENGTH_COMMAND                 = 0x3a,
    // The HCI_LE_Read_Maximum_Advertising_Data_Length command will read the maximum length of advertising data that the advertising Controller supports in a given advertising set.
    HCI_LE_READ_NUMBER_OF_SUPPORTED_ADVERTISING_SETS_COMMAND            = 0x3b,
    // The HCI_LE_Read_Number_of_Supported_Advertising_Sets command will read the maximum number of advertising sets supported by the advertising Controller at the same time.
    HCI_LE_REMOVE_ADVERTISING_SET_COMMAND                               = 0x3c,
    // The HCI_LE_Remove_Advertising_Set command will remove an advertising set from the Controller.
    HCI_LE_CLEAR_ADVERTISING_SETS_COMMAND                               = 0x3d,
    // The HCI_LE_Clear_Advertising_Sets command will remove all existing advertising sets from the Controller.
    HCI_LE_SET_PERIODIC_ADVERTISING_PARAMETERS_COMMAND                  = 0x3e,
	HCI_LE_SET_PERIODIC_ADVERTISING_PARAMETERS_COMMAND_V2               = 0x86,
    // The HCI_LE_Set_Periodic_Advertising_Parameters command will set the parameters used for periodic advertising.
    HCI_LE_SET_PERIODIC_ADVERTISING_DATA_COMMAND                        = 0x3f,
    // The HCI_LE_Set_Periodic_Advertising_Data command will set the periodic advertising data transmitted when advertising.
    HCI_LE_SET_PERIODIC_ADVERTISING_ENABLE_COMMAND                      = 0x40,
    // The HCI_LE_Set_Periodic_Advertising_Enable command will enable or disable periodic advertising.
    HCI_LE_SET_EXTENDED_SCAN_PARAMETERS_COMMAND                         = 0x41,
    // The HCI_LE_Set_Extended_Scan_Parameters command will set the parameters used for scanning on the primary advertising physical channel.
    HCI_LE_SET_EXTENDED_SCAN_ENABLE_COMMAND                             = 0x42,
    // The HCI_LE_Set_Extended_Scan_Enable command will enable or disable scanning on the primary advertising physical channels.
    HCI_LE_EXTENDED_CREATE_CONNECTION_COMMAND                           = 0x43,
	HCI_LE_EXTENDED_CREATE_CONNECTION_COMMAND_V2                        = 0x85,
    // The HCI_LE_Extended_Create_Connection command is used to create a new connection supporting different initiating PHYs and to initiate a connection with a synchronized device.
    HCI_LE_PERIODIC_ADVERTISING_CREATE_SYNC_COMMAND                     = 0x44,
    // The HCI_LE_Periodic_Advertising_Create_Sync command is used to start receiving periodic advertising packets from an advertiser.
    HCI_LE_PERIODIC_ADVERTISING_CREATE_SYNC_CANCEL_COMMAND              = 0x45,
    // The HCI_LE_Periodic_Advertising_Create_Sync_Cancel command is used to cancel a pending HCI_LE_Periodic_Advertising_Create_Sync command.
    HCI_LE_PERIODIC_ADVERTISING_TERMINATE_SYNC_COMMAND                  = 0x46,
    // The HCI_LE_Periodic_Advertising_Terminate_Sync command is used to end receiving of a periodic advertising train.
    HCI_LE_ADD_DEVICE_TO_PERIODIC_ADVERTISER_LIST_COMMAND               = 0x47,
    // The HCI_LE_Add_Device_To_Periodic_Advertiser_List command will add a device to the Periodic Advertiser List.
    HCI_LE_REMOVE_DEVICE_FROM_PERIODIC_ADVERTISER_LIST_COMMAND          = 0x48,
    // The HCI_LE_Remove_Device_From_Periodic_Advertiser_List command will remove a single device from the Periodic Advertiser List.
    HCI_LE_CLEAR_PERIODIC_ADVERTISER_LIST_COMMAND                       = 0x49,
    // The HCI_LE_Clear_Periodic_Advertiser_List command will clear the Periodic Advertiser List.
    HCI_LE_READ_PERIODIC_ADVERTISER_LIST_SIZE_COMMAND                   = 0x4a,
    // The HCI_LE_Read_Periodic_Advertiser_List_Size command will read the maximum number of Periodic Advertiser List entries that the Controller supports.
    HCI_LE_READ_TRANSMIT_POWER_COMMAND                                  = 0x4b,
    // The HCI_LE_Read_Transmit_Power command will read the minimum and maximum transmit powers supported by the Controller.
    HCI_LE_READ_RF_PATH_COMPENSATION_COMMAND                            = 0x4c,
    // The HCI_LE_Read_RF_Path_Compensation command is used to read the RF Path Compensation Value.
    HCI_LE_WRITE_RF_PATH_COMPENSATION_COMMAND                           = 0x4d,
    // The HCI_LE_Write_RF_Path_Compensation command is used to indicate the RF path gain or loss from the RF transceiver output to the antenna output contributed by intermediate components.
    HCI_LE_SET_PRIVACY_MODE_COMMAND                                     = 0x4e,
    // The HCI_LE_Set_Privacy_Mode command is used to allow the Host to specify the privacy mode for an entry on the resolving list.
    HCI_LE_SET_CONNECTIONLESS_CTE_TRANSMIT_PARAMETERS_COMMAND           = 0x51,
    // The HCI_LE_Set_Connectionless_CTE_Transmit_Parameters command will set the antenna-switching pattern and switching and sampling slot durations for the transmission of Constant Tone Extensions.
    HCI_LE_SET_CONNECTIONLESS_CTE_TRANSMIT_ENABLE_COMMAND               = 0x52,
    // The HCI_LE_Set_Connectionless_CTE_Transmit_Enable command will request the Controller to enable or disable sending packets containing a Constant Tone Extension.
    HCI_LE_SET_CONNECTIONLESS_IQ_SAMPLING_ENABLE_COMMAND                = 0x53,
    // The HCI_LE_Set_Connectionless_IQ_Sampling_Enable command will request the Controller to enable or disable taking IQ samples from the Constant Tone Extension of advertising packet
    HCI_LE_SET_CONNECTION_CTE_RECEIVE_PARAMETERS_COMMAND                = 0x54,
    // The HCI_LE_Set_Connection_CTE_Receive_Parameters command will set the antenna-switching pattern, switching and sampling slot durations for receiving the Constant Tone Extension on a connection.
    HCI_LE_SET_CONNECTION_CTE_TRANSMIT_PARAMETERS_COMMAND               = 0x55,
	HCI_LE_CONNECTION_CTE_REQUEST_ENABLE_COMMAND                        = 0x56,
    // The HCI_LE_Connection_CTE_Request_Enable command will request the Controller to start or stop sending of LL_CTE_REQ PDUs on a connection.
    HCI_LE_CONNECTION_CTE_RESPONSE_ENABLE_COMMAND                       = 0x57,
    // The HCI_LE_Connection_CTE_Response_Enable command will command the Controller to respond to LL_CTE_REQ PDUs with LL_CTE_RSP PDUs.
    HCI_LE_READ_ANTENNA_INFORMATION_COMMAND                             = 0x58,
    // The HCI_LE_Read_Antenna_Information command allows the Host to read the switching rates, the sampling rates, the number of antennae, and the maximum length of the Constant Tone Extension supported by the Controller.
    HCI_LE_SET_PERIODIC_ADVERTISING_RECEIVE_ENABLE_COMMAND              = 0x59,
    // The HCI_LE_Set_Periodic_Advertising_Receive_Enable command will enable or disable periodic advertising reports once synchronized.
    HCI_LE_PERIODIC_ADVERTISING_SYNC_TRANSFER_COMMAND                   = 0x5a,
    // The HCI_LE_Periodic_Advertising_Sync_Transfer command is used to send periodic advertising synchronization information to a connected Controller.
    HCI_LE_PERIODIC_ADVERTISING_SET_INFO_TRANSFER_COMMAND               = 0x5b,
    // The HCI_LE_Periodic_Advertising_Set_Info_Transfer command is used to send periodic advertising synchronization information, describing periodic advertising events that the Controller is transmitting, to a connected Controller.
    HCI_LE_SET_PERIODIC_ADVERTISING_SYNC_TRANSFER_PARAMETERS_COMMAND    = 0x5c,
    // The HCI_LE_Set_Periodic_Advertising_Sync_Transfer_Parameters command is used to allow the Host to specify the behavior of the Controller when periodic advertising synchronization information is received from a connected Controller.
    HCI_LE_SET_DEFAULT_PERIODIC_ADVERTISING_SYNC_TRANSFER_PARAMETERS_COMMAND = 0x5d,
    // The HCI_LE_Set_Default_Periodic_Advertising_Sync_Transfer_Parameters command is used to specify the default behavior of the Controller when periodic advertising synchronization information is received from a connected Controller.
    HCI_LE_MODIFY_SLEEP_CLOCK_ACCURACY_COMMAND                          = 0x5f,
    // The HCI_LE_Modify_Sleep_Clock_Accuracy command requests the Controller changes its sleep clock accuracy for testing purposes.
    HCI_LE_READ_ISO_TX_SYNC_COMMAND                                     = 0x61,
    // The HCI_LE_Read_ISO_TX_Sync command is used to read the Time_Stamp and Time_Offset of a transmitted SDU.
    HCI_LE_SET_CIG_PARAMETERS_COMMAND                                   = 0x62,
    // The HCI_LE_Set_CIG_Parameters command is used by a Central’s Host to set the parameters of one or more Connected Isochronous Streams (CISes) that are associated with a CIG in the Controller.
    HCI_LE_SET_CIG_PARAMETERS_TEST_COMMAND                              = 0x63,
    // The HCI_LE_Set_CIG_Parameters_Test command is used by a Central’s Host to set the parameters of one or more CISes that are associated with a CIG in the Controller for testing purposes.
    HCI_LE_CREATE_CIS_COMMAND                                           = 0x64,
    // The HCI_LE_Create_CIS command is used by the Central’s Host to create one or more CISes.
    HCI_LE_REMOVE_CIG_COMMAND                                           = 0x65,
    // The HCI_LE_Remove_CIG command is used by the Central’s Host to remove a CIG from the Controller.
    HCI_LE_ACCEPT_CIS_REQUEST_COMMAND                                   = 0x66,
    // The HCI_LE_Accept_CIS_Request command is used by the Peripheral’s Host to inform the Controller to accept the request for creating the CIS.W
    HCI_LE_REJECT_CIS_REQUEST_COMMAND                                   = 0x67,
    // The HCI_LE_Reject_CIS_Request command is used by the Peripheral’s Host to inform the Controller to reject the request for creating the CIS.
    HCI_LE_CREATE_BIG_COMMAND                                           = 0x68,
    // The HCI_LE_Create_BIG command creates one or more BISes of a BIG.
    HCI_LE_CREATE_BIG_TEST_COMMAND                                      = 0x69,
    // The HCI_LE_Create_BIG_Test command is used to create one or more BISes of a BIG for testing purposes.
    HCI_LE_TERMINATE_BIG_COMMAND                                        = 0x6a,
    // The HCI_LE_Terminate_BIG command terminates the transmission of all BISes of a BIG or cancels the process of creating a BIG.
    HCI_LE_BIG_CREATE_SYNC_COMMAND                                      = 0x6b,
    // The HCI_LE_BIG_Create_Sync synchronizes and receives PDUs from one or more BISes.
    HCI_LE_BIG_TERMINATE_SYNC_COMMAND                                   = 0x6c,
    // The HCI_LE_BIG_Terminate_Sync command stops or cancels synchronizing with a BIG.
    HCI_LE_REQUEST_PEER_SCA_COMMAND                                     = 0x6d,
    // The HCI_LE_Request_Peer_SCA command requests the Sleep Clock Accuracy of the peer device.
    HCI_LE_SETUP_ISO_DATA_PATH_COMMAND                                  = 0x6e,
    // The HCI_LE_Setup_ISO_Data_Path command identifies and creates the isochronous data path between the Host and the Controller and optionally configures the codec in the Controller.
    HCI_LE_REMOVE_ISO_DATA_PATH_COMMAND                                 = 0x6f,
    // The HCI_LE_Remove_ISO_Data_Path command removes an isochronous data path between the Host and the Controller.
    HCI_LE_ISO_TRANSMIT_TEST_COMMAND                                    = 0x70,
    // The HCI_LE_ISO_Transmit_Test command configures an established CIS or BIS to transmit test payloads that are generated by the Controller.
    HCI_LE_ISO_RECEIVE_TEST_COMMAND                                     = 0x71,
    // The HCI_LE_ISO_Receive_Test command configures a Link Layer to receive test payloads from an established CIS or a synchronized BIS.
    HCI_LE_ISO_READ_TEST_COUNTERS_COMMAND                               = 0x72,
    // The HCI_LE_ISO_Read_Test_Counters command reads the test counters in the Controller which is configured in ISO Receive Test mode.
    HCI_LE_ISO_TEST_END_COMMAND                                         = 0x73,
    // The HCI_LE_ISO_Test_End command terminates the ISO Transmit and/or Receive Test mode.
    HCI_LE_SET_HOST_FEATURE_COMMAND                                     = 0x74,
	HCI_LE_SET_HOST_FEATURE_COMMAND_V2                                  = 0x97,
    // The HCI_LE_Set_Host_Feature command is used to set or clear a bit controlled by the Host in the Link Layer FeatureSet stored in the Controller.
    HCI_LE_READ_ISO_LINK_QUALITY_COMMAND                                = 0x75,
    // The HCI_LE_Read_ISO_Link_Quality command returns the value of various counters related to link quality on an isochronous stream.
    HCI_LE_ENHANCED_READ_TRANSMIT_POWER_LEVEL_COMMAND                   = 0x76,
    // The HCI_LE_Enhanced_Read_Transmit_Power_Level command is used to read the current and maximum transmit power levels used by the local Controller on a specified PHY on an ACL connection.
    HCI_LE_READ_REMOTE_TRANSMIT_POWER_LEVEL_COMMAND                     = 0x77,
    // The HCI_LE_Read_Remote_Transmit_Power_Level command is used to read the transmit power level used by the remote Controller on a specified PHY on an ACL connection.
    HCI_LE_SET_PATH_LOSS_REPORTING_PARAMETERS_COMMAND                   = 0x78,
    // The HCI_LE_Set_Path_Loss_Reporting_Parameters command is used to set the path loss threshold and related parameters used to trigger reports for an ACL connection.
    HCI_LE_SET_PATH_LOSS_REPORTING_ENABLE_COMMAND                       = 0x79,
    // The HCI_LE_Set_Path_Loss_Reporting_Enable command is used to enable or disable path loss reporting events for an ACL connection.
    HCI_LE_SET_TRANSMIT_POWER_REPORTING_ENABLE_COMMAND                  = 0x7a,
    // The HCI_LE_Set_Transmit_Power_Reporting_Enable command is used to enable or disable reporting to the local Host of transmit power level changes on an ACL connection.
    HCI_LE_SET_DATA_RELATED_ADDRESS_CHANGES_COMMAND                     = 0x7c,
    // The HCI_LE_Set_Data_Related_Address_Changes command specifies circumstances when the Controller shall refresh any Resolvable Private Address used by an advertising set, whether or not the address timeout period has been reached.
    HCI_LE_SET_DEFAULT_SUBRATE_COMMAND                                  = 0x7d,
    // The HCI_LE_Set_Default_Subrate command sets the range of the min and max subrates and other subrate parameters on a Central that may be requested by a Peripheral.
    HCI_LE_SUBRATE_REQUEST_COMMAND                                      = 0x7e,
    // The HCI_LE_Subrate_Request command modifies an existing ACL connection by applying a subrate factor.
    HCI_LE_SET_PERIODIC_ADVERTISING_SUBEVENT_DATA_COMMAND               = 0x82,
    // The HCI_LE_Set_Periodic_Advertising_Subevent_Data command is used to send subevent data for one or more subevents.
    HCI_LE_SET_PERIODIC_ADVERTISING_RESPONSE_DATA_COMMAND               = 0x83,
    // The HCI_LE_Set_Periodic_Advertising_Response_Data command is used to set the data for a response slot.
    HCI_LE_SET_PERIODIC_SYNC_SUBEVENT_COMMAND                           = 0x84,
    // The HCI_LE_Set_Periodic_Sync_Subevent command is used to configure the subset of subevents a device will synchronize with.
    HCI_LE_READ_ALL_LOCAL_SUPPORTED_FEATURES_COMMAND                    = 0x87,
    // The HCI_LE_Read_All_Local_Supported_Features command will read all the features supported by the local LE Controller.
    HCI_LE_READ_ALL_REMOTE_FEATURES_COMMAND                             = 0x88,
    // The HCI_LE_Read_All_Remote_Features command is used to read all the features used on a connection and the features supported by a remote LE device.
    HCI_LE_CS_READ_LOCAL_SUPPORTED_CAPABILITIES_COMMAND                 = 0x89,
    // The HCI_LE_CS_Read_Local_Supported_Capabilities command is used by a Host to query CS capabilities.
    HCI_LE_CS_READ_REMOTE_SUPPORTED_CAPABILITIES_COMMAND                = 0x8a,
    // The HCI_LE_CS_Read_Remote_Supported_Capabilities command is used by a Host to query the CS capabilities of a remote device.
    HCI_LE_CS_WRITE_CACHED_REMOTE_SUPPORTED_CAPABILITIES_COMMAND        = 0x8b,
    // The HCI_LE_CS_Write_Cached_Remote_Supported_Capabilities command is used by a Host to write a cached copy of the remote CS capabilities in a local Controller.
    HCI_LE_CS_SECURITY_ENABLE_COMMAND                                   = 0x8c,
    // The HCI_LE_CS_Security_Enable command is used by a Host to start the CS Security procedure on the specified connection.
    HCI_LE_CS_SET_DEFAULT_SETTINGS_COMMAND                              = 0x8d,
    // The HCI_LE_CS_Set_Default_Settings command is used by a Host to set default CS settings in the local Controller.
    HCI_LE_CS_READ_REMOTE_FAE_TABLE_COMMAND                             = 0x8e,
    // The HCI_LE_CS_Read_Remote_FAE_Table command is used by a Host to read the per-channel mode-0 Frequency Actuation Error table of the remote Controller.
    HCI_LE_CS_WRITE_CACHED_REMOTE_FAE_TABLE_COMMAND                     = 0x8f,
    // The HCI_LE_CS_Write_Cached_Remote_FAE_Table command is used by a Host to write the per-channel mode-0 Frequency Actuation Error table of the remote device in the local Controller.
    HCI_LE_CS_CREATE_CONFIG_COMMAND                                     = 0x90,
    // The HCI_LE_CS_Create_Config command is used by a Host to create a CS configuration.
    HCI_LE_CS_REMOVE_CONFIG_COMMAND                                     = 0x91,
    // The HCI_LE_CS_Remove_Config command is used by a Host to remove an active CS configuration.
    HCI_LE_CS_SET_CHANNEL_CLASSIFICATION_COMMAND                        = 0x92,
    // The HCI_LE_CS_Set_Channel_Classification command is used by the Host to specify the channel classification to be used for CS procedures.
    HCI_LE_CS_SET_PROCEDURE_PARAMETERS_COMMAND                          = 0x93,
    // The HCI_LE_CS_Set_Procedure_Parameters command is used by the Host to specify the parameters to be used for scheduling CS procedures.
    HCI_LE_CS_PROCEDURE_ENABLE_COMMAND                                  = 0x94,
    // The HCI_LE_CS_Procedure_Enable command is used by a Host to enable the Controller to initiate the CS start procedure.
    HCI_LE_CS_TEST_COMMAND                                              = 0x95,
    // The HCI_LE_CS_Test command runs the CS test.
    HCI_LE_CS_TEST_END_COMMAND                                          = 0x96,
    // The HCI_LE_CS_Test_End command stops the CS test.
    HCI_LE_SET_DECISION_DATA_COMMAND                                    = 0x80,
    // The HCI_LE_Set_Decision_Data command will set the decision data transmitted when advertising.
    HCI_LE_SET_DECISION_INSTRUCTIONS_COMMAND                            = 0x81,
    // The HCI_LE_Set_Decision_Instructions command will set the decision instructions used when scanning for advertisements that include decision data.
    HCI_LE_ADD_DEVICE_TO_MONITORED_ADVERTISERS_LIST_COMMAND             = 0x98,
    // The HCI_LE_Add_Device_To_Monitored_Advertisers_List command will add a device to the Monitored Advertisers List.
    HCI_LE_REMOVE_DEVICE_FROM_MONITORED_ADVERTISERS_LIST_COMMAND        = 0x99,
    // The HCI_LE_Remove_Device_From_Monitored_Advertisers_List command will remove a device from the Monitored Advertisers list.
    HCI_LE_CLEAR_MONITORED_ADVERTISERS_LIST_COMMAND                     = 0x9a,
    // The HCI_LE_Clear_Monitored_Advertisers_List command will clear the Monitored Advertisers List.
    HCI_LE_ENABLE_MONITORING_ADVERTISERS_COMMAND                        = 0x9c,
    // The HCI_LE_Enable_Monitoring_Advertisers command will enable monitoring advertisers that are in the Monitored Advertisers List.
    HCI_LE_READ_MONITORED_ADVERTISERS_LIST_SIZE_COMMAND                 = 0x9b,
    // The HCI_LE_Read_Monitored_Advertisers_List_Size command will read the number of Monitored Advertisers List entries that the Controller supports.
    HCI_LE_FRAME_SPACE_UPDATE_COMMAND                                   = 0x9d,
    // The HCI_LE_Frame_Space_Update command is used to initiate the Frame Space Update procedure.
}hci_command_le_controller_e;

typedef enum
{
    HCI_INQUIRY_COMPLETE_EVENT                          = 0x01,
    // The HCI_Inquiry_Complete event indicates that the Inquiry is finished.
    HCI_INQUIRY_RESULT_EVENT                            = 0x02,
    // The HCI_Inquiry_Result event indicates that a BR/EDR Controller or multiple BR/EDR Controllers have responded so far during the current Inquiry process.
    HCI_CONNECTION_COMPLETE_EVENT                       = 0x03,
    // The HCI_Connection_Complete event indicates to both of the Hosts forming the connection that a new BR/EDR connection has been established.
    HCI_CONNECTION_REQUEST_EVENT                        = 0x04,
    // The HCI_Connection_Request event is used to indicate that a new incoming BR/EDR connection is trying to be established.
    HCI_DISCONNECTION_COMPLETE_EVENT                    = 0x05,
    // The HCI_Disconnection_Complete event occurs when a connection has been terminated.
    HCI_AUTHENTICATION_COMPLETE_EVENT                   = 0x06,
    // The HCI_Authentication_Complete event occurs when authentication has been completed for the specified connection.
    HCI_REMOTE_NAME_REQUEST_COMPLETE_EVENT              = 0x07,
    HCI_ENCRYPTION_CHANGE_EVENT                         = 0x08,
    HCI_ENCRYPTION_CHANGE_EVENT_V2                      = 0x59,
    // The HCI_Encryption_Change event is used to indicate that the change in encryption has been completed for the specified Connection_Handle.
    HCI_CHANGE_CONNECTION_LINK_KEY_COMPLETE_EVENT       = 0x09,
    // The HCI_Change_Connection_Link_Key_Complete event is used to indicate that the change in the Link Key for the Connection_Handle specified by the Connection_Handle parameter had been completed.
    HCI_LINK_KEY_TYPE_CHANGED_EVENT                     = 0x0a,
    HCI_READ_REMOTE_SUPPORTED_FEATURES_COMPLETE_EVENT   = 0x0b,
    HCI_READ_REMOTE_VERSION_INFORMATION_COMPLETE_EVENT  = 0x0c,
    HCI_QOS_SETUP_COMPLETE_EVENT                        = 0x0d,
    HCI_COMMAND_COMPLETE_EVENT                          = 0x0e,
    // The HCI_Command_Complete event is used by the Controller to pass the return status of a command and the other parameters for each HCI command.
    HCI_COMMAND_STATUS_EVENT                            = 0x0f,
    // The HCI_Command_Status event is used to indicate that the command described by the Command_Opcode parameter has been received and the Controller is currently performing the task for this command.
    HCI_HARDWARE_ERROR_EVENT                            = 0x10,
    // The HCI_Hardware_Error event is used to indicate some type of hardware failure for the Controller.
    HCI_FLUSH_OCCURRED_EVENT                            = 0x11,
    // The HCI_Flush_Occurred event is used to indicate that, for the specified Handle, the data to be transmitted has been discarded.
    HCI_ROLE_CHANGE_EVENT                               = 0x12,
    HCI_NUMBER_OF_COMPLETED_PACKETS_EVENT               = 0x13,
    HCI_MODE_CHANGE_EVENT                               = 0x14,
    HCI_RETURN_LINK_KEYS_EVENT                          = 0x15,
    HCI_PIN_CODE_REQUEST_EVENT                          = 0x16,
    HCI_LINK_KEY_REQUEST_EVENT                          = 0x17,
    HCI_LINK_KEY_NOTIFICATION_EVENT                     = 0x18,
    HCI_LOOPBACK_COMMAND_EVENT                          = 0x19,
    HCI_DATA_BUFFER_OVERFLOW_EVENT                      = 0x1a,
    // The HCI_Data_Buffer_Overflow event is used to indicate that the Controller's data buffers have overflowed, because the Host has sent more packets than allowed.
    HCI_MAX_SLOTS_CHANGE_EVENT                          = 0x1b,
    HCI_READ_CLOCK_OFFSET_COMPLETE_EVENT                = 0x1c,
    HCI_CONNECTION_PACKET_TYPE_CHANGED_EVENT            = 0x1d,
    // The HCI_Connection_Packet_Type_Changed event is used to indicate the completion of the process of the Link Manager changing the packet type mask used for the specified Connection_Handle.
    HCI_QOS_VIOLATION_EVENT                             = 0x1e,
    HCI_PAGE_SCAN_REPETITION_MODE_CHANGE_EVENT          = 0x20,
    HCI_FLOW_SPECIFICATION_COMPLETE_EVENT               = 0x21,
    // The HCI_Flow_Specification_Complete event is used to inform the Host about the Quality of Service for the ACL connection the Controller is able to support.
    HCI_INQUIRY_RESULT_WITH_RSSI_EVENT                  = 0x22,
     // The HCI_Inquiry_Result_with_RSSI event indicates that a BR/EDR Controller or multiple BR/EDR Controllers have responded so far during the current Inquiry process.
    HCI_READ_REMOTE_EXTENDED_FEATURES_COMPLETE_EVENT    = 0x23,
    HCI_SYNCHRONOUS_CONNECTION_COMPLETE_EVENT           = 0x2c,
    HCI_SYNCHRONOUS_CONNECTION_CHANGED_EVENT            = 0x2d,
    HCI_SNIFF_SUBRATING_EVENT                           = 0x2e,
    HCI_EXTENDED_INQUIRY_RESULT_EVENT                   = 0x2f,
    // The HCI_Extended_Inquiry_Result event indicates that a BR/EDR Controller has responded with an extended inquiry response during the current Inquiry process.
    HCI_ENCRYPTION_KEY_REFRESH_COMPLETE_EVENT           = 0x30,
    // The HCI_Encryption_Key_Refresh_Complete event is used to indicate to the Host that the encryption key was refreshed on the given Connection_Handle any time encryption is paused and then resumed.
    HCI_IO_CAPABILITY_REQUEST_EVENT                     = 0x31,
    // The HCI_IO_Capability_Request event is used to indicate that the IO capabilities of the Host are required for a Secure Simple Pairing process.
    HCI_IO_CAPABILITY_RESPONSE_EVENT                    = 0x32,
    // The HCI_IO_Capability_Response event is used to indicate to the Host that IO capabilities from a remote device specified by BD_ADDR have been received during a Secure Simple Pairing process.
    HCI_USER_CONFIRMATION_REQUEST_EVENT                 = 0x33,
    HCI_USER_PASSKEY_REQUEST_EVENT                      = 0x34,
    HCI_REMOTE_OOB_DATA_REQUEST_EVENT                   = 0x35,
    HCI_SIMPLE_PAIRING_COMPLETE_EVENT                   = 0x36,
    HCI_LINK_SUPERVISION_TIMEOUT_CHANGED_EVENT          = 0x38,
    HCI_ENHANCED_FLUSH_COMPLETE_EVENT                   = 0x39,
    // The HCI_Enhanced_Flush_Complete event is used to indicate that an Enhanced Flush is complete.
    HCI_USER_PASSKEY_NOTIFICATION_EVENT                 = 0x3b,
    HCI_KEYPRESS_NOTIFICATION_EVENT                     = 0x3c,
    // The HCI_Keypress_Notification event is sent to the Host after a passkey notification has been received by the Link Manager on the given BD_ADDR.
    HCI_REMOTE_HOST_SUPPORTED_FEATURES_EVENT            = 0x3d,
    HCI_LE_META_EVENT                                   = 0x3e,
    HCI_NUMBER_OF_COMPLETED_DATA_BLOCKS_EVENT           = 0x48,
    HCI_TRIGGERED_CLOCK_CAPTURE_EVENT                   = 0x4e,
    HCI_SYNCHRONIZATION_TRAIN_COMPLETE_EVENT            = 0x4f,
    HCI_SYNCHRONIZATION_TRAIN_RECEIVED_EVENT            = 0x50,
    HCI_CONNECTIONLESS_PERIPHERAL_BROADCAST_RECEIVE_EVENT = 0x51,
    // The HCI_Connectionless_Peripheral_Broadcast_Receive event provides the Host with the data received from a Connectionless Peripheral Broadcast packet.
    HCI_CONNECTIONLESS_PERIPHERAL_BROADCAST_TIMEOUT_EVENT = 0x52,
    // On the Connectionless Peripheral Broadcast Receiver,the HCI_Connectionless_Peripheral_Broadcast_Timeout event indicates to the Host that 
    // the BR/EDR Controller has lost synchronization with the Connectionless Peripheral Broadcast Transmitter. On the Connectionless Peripheral Broadcast Transmitter, it indicates that the BR/EDR Controller has been unable to transmit a Connectionless Peripheral Broadcast packet for the timeout interval specified in the HCI_Set_Connectionless_Peripheral_Broadcast command.
    HCI_TRUNCATED_PAGE_COMPLETE_EVENT                   = 0x53,
    HCI_PERIPHERAL_PAGE_RESPONSE_TIMEOUT_EVENT          = 0x54,
    HCI_CONNECTIONLESS_PERIPHERAL_BROADCAST_CHANNEL_MAP_CHANGE_EVENT = 0x55,
    // The HCI_Connectionless_Peripheral_Broadcast_Channel_Map_Change event indicates to the Host that the BR/EDR Controller has moved to a new AFH channel map for the PBD logical link.
    HCI_INQUIRY_RESPONSE_NOTIFICATION_EVENT             = 0x56,
    // The HCI_Inquiry_Response_Notification event indicates to the Host that the BR/EDR Controller responded to an inquiry message.
    HCI_AUTHENTICATED_PAYLOAD_TIMEOUT_EXPIRED_EVENT     = 0x57,
    // The HCI_Authenticated_Payload_Timeout_Expired event is used to indicate that a packet containing a valid MIC on the Handle was not received within the authenticatedPayloadTO.
    HCI_SAM_STATUS_CHANGE_EVENT                         = 0x58
}hci_eventCode_e;

typedef enum
{
    HCI_LE_CONNECTION_COMPLETE_EVENT                      = 0x01,
    // The HCI_LE_Connection_Complete event indicates to the Host that a new connection has been created.
    HCI_LE_ADVERTISING_REPORT_EVENT                       = 0x02,
    // The HCI_LE_Advertising_Report event indicates that an advertising or scan response packet has been received.
    HCI_LE_CONNECTION_UPDATE_COMPLETE_EVENT               = 0x03,
    // The HCI_LE_Connection_Update_Complete event indicates the completion of the process to change the connection parameters.
    HCI_LE_READ_REMOTE_FEATURES_PAGE_0_COMPLETE_EVENT     = 0x04,
    // The HCI_LE_Read_Remote_Features_Page_0_Complete event indicates the completion of the process to read page 0 of the features used on a connection and the features supported by a remote LE device.
    HCI_LE_LONG_TERM_KEY_REQUEST_EVENT                    = 0x05,
    // The HCI_LE_Long_Term_Key_Request event indicates that a Long Term Key is required for a connection.
    HCI_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_EVENT      = 0x06,
    // The HCI_LE_Remote_Connection_Parameter_Request event is used to indicate to the Host that the remote device is requesting a change in the connection parameters.
    HCI_LE_DATA_LENGTH_CHANGE_EVENT                       = 0x07,
    // The HCI_LE_Data_Length_Change event is used to indicate a change in the maximum packet sizes by the Link Layer.
    HCI_LE_READ_LOCAL_P_256_PUBLIC_KEY_COMPLETE_EVENT     = 0x08,
    // The HCI_LE_Read_Local_P-256_Public_Key_Complete event is generated when local P-256 key generation is complete.
    HCI_LE_GENERATE_DHKEY_COMPLETE_EVENT                  = 0x09,
    // The HCI_LE_Generate_DHKey_Complete event indicates that LE Diffie-Hellman key generation has been completed by the Controller.
    HCI_LE_ENHANCED_CONNECTION_COMPLETE_EVENT             = 0x0a,
    HCI_LE_ENHANCED_CONNECTION_COMPLETE_EVENT_V2          = 0x29,
    // The HCI_LE_Enhanced_Connection_Complete event indicates to the Host that a new connection has been created. This event contains the additional parameters of the local and peer resolvable private addresses.
    HCI_LE_DIRECTED_ADVERTISING_REPORT_EVENT              = 0x0b,
    // The HCI_LE_Directed_Advertising_Report event indicates that directed advertisements have been received where the advertiser is using 
    // a resolvable private address for the TargetA field in the ADV_DIRECT_IND PDU and the scanning filter policy is set to send this event to the Host.
    HCI_LE_PHY_UPDATE_COMPLETE_EVENT                      = 0x0c,
    // The HCI_LE_PHY_Update_Complete event is used to inform the Host of the current PHY.
    HCI_LE_EXTENDED_ADVERTISING_REPORT_EVENT              = 0x0d,
    // The HCI_LE_Extended_Advertising_Report event indicates that an advertising packet has been received.
    HCI_LE_PERIODIC_ADVERTISING_SYNC_ESTABLISHED_EVENT    = 0x0e,
    HCI_LE_PERIODIC_ADVERTISING_SYNC_ESTABLISHED_EVENT_V2 = 0x24,
    // The HCI_LE_Periodic_Advertising_Sync_Established event indicates that the Controller has started receiving periodic advertising packets from an advertiser.
    HCI_LE_PERIODIC_ADVERTISING_REPORT_EVENT              = 0x0f,
    HCI_LE_PERIODIC_ADVERTISING_REPORT_EVENT_V2           = 0x25,
    // The HCI_LE_Periodic_Advertising_Report event indicates that a periodic advertising packet has been received.
    HCI_LE_PERIODIC_ADVERTISING_SYNC_LOST_EVENT           = 0x10,
    // The HCI_LE_Periodic_Advertising_Sync_Lost event indicates the Controller has ended receiving a periodic advertising train.
    HCI_LE_SCAN_TIMEOUT_EVENT                             = 0x11,
    // The HCI_LE_Scan_Timeout event indicates that scanning has finished.
    HCI_LE_ADVERTISING_SET_TERMINATED_EVENT               = 0x12,
    // The HCI_LE_Advertising_Set_Terminated event indicates that advertising in a given advertising set has stopped.
    HCI_LE_SCAN_REQUEST_RECEIVED_EVENT                    = 0x13,
    // The HCI_LE_Scan_Request_Received event indicates that a scan request has been received.
    HCI_LE_CHANNEL_SELECTION_ALGORITHM_EVENT              = 0x14,
    // The HCI_LE_Channel_Selection_Algorithm event indicates the channel selection algorithm used on a connection.
    HCI_LE_CONNECTIONLESS_IQ_REPORT_EVENT                 = 0x15,
    // The HCI_LE_Connectionless_IQ_Report event reports IQ information from the Constant Tone Extension of a received advertising packet.
    HCI_LE_CONNECTION_IQ_REPORT_EVENT                     = 0x16,
    // The HCI_LE_Connection_IQ_Report event is used to report IQ samples from the Constant Tone Extension field of a received packet containing an LL_CTE_RSP PDU.
    HCI_LE_CTE_REQUEST_FAILED_EVENT                       = 0x17,
    // The HCI_LE_CTE_Request_Failed event indicates a problem with a request generated by an HCI_LE_Connection_CTE_Request_Enable command for a peer device to send Constant Tone Extensions.
    HCI_LE_PERIODIC_ADVERTISING_SYNC_TRANSFER_RECEIVED_EVENT    = 0x18,
    HCI_LE_PERIODIC_ADVERTISING_SYNC_TRANSFER_RECEIVED_EVENT_V2 = 0x26,
    // The HCI_LE_PLE_Periodic_Advertising_Sync_Transfer_Received event reports reception of periodic advertising synchronization information from a connected Controller.
    HCI_LE_CIS_ESTABLISHED_EVENT                          = 0x19,
    HCI_LE_CIS_ESTABLISHED_EVENT_V2                       = 0x2a,
    // The HCI_LE_CIS_Established event indicates that the Controller established a CIS.
    HCI_LE_CIS_REQUEST_EVENT                              = 0x1a,
    // The HCI_LE_CIS_Request event indicates that the Peripheral’s Controller received a request from the Central to create a CIS.
    HCI_LE_CREATE_BIG_COMPLETE_EVENT                      = 0x1b,
    // The HCI_LE_Create_BIG_Complete event indicates that the Controller completed an attempt to create the BISes that were requested by the Host.
    HCI_LE_TERMINATE_BIG_COMPLETE_EVENT                   = 0x1c,
    // The HCI_LE_Terminate_BIG_Complete event indicates that the transmission of all the BISes in the BIG have been terminated.
    HCI_LE_BIG_SYNC_ESTABLISHED_EVENT                     = 0x1d,
    // The HCI_LE_BIG_Sync_Established event indicates that the Controller has completed an attempt to synchronize with the requested BISes.
    HCI_LE_BIG_SYNC_LOST_EVENT                            = 0x1e,
    // The HCI_LE_BIG_Sync_Lost event indicates that the Controller stopped synchronizing with a BIG.
    HCI_LE_REQUEST_PEER_SCA_COMPLETE_EVENT                = 0x1f,
    // The HCI_LE_Request_Peer_SCA_Complete event indicates that the Controller completed the attempt to read the Sleep Clock Accuracy (SCA) of the peer device.
    HCI_LE_PATH_LOSS_THRESHOLD_EVENT                      = 0x20,
    // The HCI_LE_Path_Loss_Threshold event is used to report a path loss threshold crossing on an ACL connection.
    HCI_LE_TRANSMIT_POWER_REPORTING_EVENT                 = 0x21,
    HCI_LE_BIGINFO_ADVERTISING_REPORT_EVENT               = 0x22,
    // The HCI_LE_BIGInfo_Advertising_Report event indicates that the Controller has received an Advertising PDU that contained a BIGInfo field.
    HCI_LE_SUBRATE_CHANGE_EVENT                           = 0x23,
    // The HCI_LE_Subrate_Change event indicates that a new subrate factor has been applied to an existing ACL connection.
    HCI_LE_PERIODIC_ADVERTISING_SUBEVENT_DATA_REQUEST_EVENT = 0x27,
    // The HCI_LE_Periodic_Advertising_Subevent_Data_Request event is used to request subevent data from a Host.
    HCI_LE_PERIODIC_ADVERTISING_RESPONSE_REPORT_EVENT     = 0x28,
    // The HCI_LE_Periodic_Advertising_Response_Report event is used to report response data to a Host.
    HCI_LE_READ_ALL_REMOTE_FEATURES_COMPLETE_EVENT        = 0x2b,
    // The HCI_LE_Read_All_Remote_Features_Complete event indicates the completion of the process to read all the features used on a connection and the features supported by a remote LE device.
    HCI_LE_CS_READ_REMOTE_SUPPORTED_CAPABILITIES_COMPLETE_EVENT = 0x2c,
    // The HCI_LE_CS_Read_Remote_Supported_Capabilities_Complete event is used to report the CS capabilities of a remote device.
    HCI_LE_CS_READ_REMOTE_FAE_TABLE_COMPLETE_EVENT        = 0x2d,
    // The HCI_LE_CS_Read_Remote_FAE_Table_Complete event is used to report the completion of a locally initiated Channel Sounding mode-0 FAE Table Request procedure.
    HCI_LE_CS_SECURITY_ENABLE_COMPLETE_EVENT              = 0x2e,
    // The HCI_LE_CS_Security_Enable_Complete event is used to report the completion of a CS Security Start procedure initiated by either a local or remote Controller.
    HCI_LE_CS_CONFIG_COMPLETE_EVENT                       = 0x2f,
    // The HCI_LE_CS_Config_Complete event is used to report the status of a CS configuration initiated by either a local or remote Controller.
    HCI_LE_CS_PROCEDURE_ENABLE_COMPLETE_EVENT             = 0x30,
    // The HCI_LE_CS_Procedure_Enable_Complete event is used to report the status after enabling a new CS procedure or disabling an ongoing CS procedure.
    HCI_LE_CS_SUBEVENT_RESULT_EVENT                       = 0x31,
    // The HCI_LE_CS_Subevent_Result event is used to report the results of a CS subevent in the initiator or reflector.
    HCI_LE_CS_SUBEVENT_RESULT_CONTINUE_EVENT              = 0x32,
    // The HCI_LE_CS_Subevent_Result_Continue event is used to report any remaining results of a CS subevent in the initiator or reflector.
    HCI_LE_CS_TEST_END_COMPLETE_EVENT                     = 0x33,
    // The HCI_LE_CS_Test_End_Complete event is used to report the termination of a CS test.
    HCI_LE_MONITORED_ADVERTISERS_REPORT_EVENT             = 0x34,
    // The HCI_LE_Monitored_Advertisers_Report event is used by a Controller to send reports on advertising devices being monitored.
    HCI_LE_FRAME_SPACE_UPDATE_COMPLETE_EVENT              = 0x35
    // The HCI_LE_Frame_Space_Update_Complete event is used to inform the Host of changes to the frame space values.
}hci_le_subeventCode_e;

void hci_command_packet_process(_u8* data);

#endif/*HCI_COMMAND_H_ */