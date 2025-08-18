#include"common/txCommon.h"
typedef enum {
    HCI_ACCEPT_CONNECTION_REQUEST_COMMAND, // The HCI_Accept_Connection_Request command is used to accept a new incoming BR/EDR connection request.
    HCI_ACCEPT_SYNCHRONOUS_CONNECTION_REQUEST_COMMAND, // The HCI_Accept_Synchronous_Connection_Request command is used to accept an incoming request for a synchronous connection and to inform the local Link Manager about the acceptable parameter values for the synchronous connection.
    HCI_AUTHENTICATED_PAYLOAD_TIMEOUT_EXPIRED_EVENT, // The HCI_Authenticated_Payload_Timeout_Expired event is used to indicate that a packet containing a valid MIC on the Handle was not received within the authenticatedPayloadTO.
    HCI_AUTHENTICATION_COMPLETE_EVENT, // The HCI_Authentication_Complete event occurs when authentication has been completed for the specified connection.
    HCI_AUTHENTICATION_REQUESTED_COMMAND, // The HCI_Authentication_Requested command is used to establish authentication between the two devices associated with the specified Connection_Handle.
    HCI_CHANGE_CONNECTION_LINK_KEY_COMMAND, // The HCI_Change_Connection_Link_Key command is used to force both devices of a connection associated to the Connection_Handle, to generate a new link key.
    HCI_CHANGE_CONNECTION_LINK_KEY_COMPLETE_EVENT, // The HCI_Change_Connection_Link_Key_Complete event is used to indicate that the change in the Link Key for the Connection_Handle specified by the Connection_Handle parameter had been completed.
    HCI_CHANGE_CONNECTION_PACKET_TYPE_COMMAND, // The HCI_Change_Connection_Packet_Type command is used to change which packet types can be used for a connection that is currently established.
    HCI_COMMAND_COMPLETE_EVENT, // The HCI_Command_Complete event is used by the Controller to pass the return status of a command and the other parameters for each HCI command.
    HCI_COMMAND_STATUS_EVENT, // The HCI_Command_Status event is used to indicate that the command described by the Command_Opcode parameter has been received and the Controller is currently performing the task for this command.
    HCI_CONFIGURE_DATA_PATH_COMMAND, // The HCI_Configure_Data_Path command is used by a Host to configure a data path to enable codec operation in the Controller.
    HCI_CONNECTION_COMPLETE_EVENT, // The HCI_Connection_Complete event indicates to both of the Hosts forming the connection that a new BR/EDR connection has been established.
    HCI_CONNECTION_PACKET_TYPE_CHANGED_EVENT, // The HCI_Connection_Packet_Type_Changed event is used to indicate the completion of the process of the Link Manager changing the packet type mask used for the specified Connection_Handle.
    HCI_CONNECTION_REQUEST_EVENT, // The HCI_Connection_Request event is used to indicate that a new incoming BR/EDR connection is trying to be established.
    HCI_CONNECTIONLESS_PERIPHERAL_BROADCAST_CHANNEL_MAP_CHANGE_EVENT, // The HCI_Connectionless_Peripheral_Broadcast_Channel_Map_Change event indicates to the Host that the BR/EDR Controller has moved to a new AFH channel map for the PBD logical link.
    HCI_CONNECTIONLESS_PERIPHERAL_BROADCAST_RECEIVE_EVENT, // The HCI_Connectionless_Peripheral_Broadcast_Receive event provides the Host with the data received from a Connectionless Peripheral Broadcast packet.
    HCI_CONNECTIONLESS_PERIPHERAL_BROADCAST_TIMEOUT_EVENT, // On the Connectionless Peripheral Broadcast Receiver, the HCI_Connectionless_Peripheral_Broadcast_Timeout event indicates to the Host that the BR/EDR Controller has lost synchronization with the Connectionless Peripheral Broadcast Transmitter. On the Connectionless Peripheral Broadcast Transmitter, it indicates that the BR/EDR Controller has been unable to transmit a Connectionless Peripheral Broadcast packet for the timeout interval specified in the HCI_Set_Connectionless_Peripheral_Broadcast command.
    HCI_CREATE_CONNECTION_CANCEL_COMMAND, // The HCI_Create_Connection_Cancel command is used to cancel an ongoing Create Connection.
    HCI_CREATE_CONNECTION_COMMAND, // The HCI_Create_Connection command will cause the BR/EDR Link Manager to create an ACL connection to the BR/EDR Controller with the BD_ADDR specified by the parameters.
    HCI_DATA_BUFFER_OVERFLOW_EVENT, // The HCI_Data_Buffer_Overflow event is used to indicate that the Controller's data buffers have overflowed, because the Host has sent more packets than allowed.
    HCI_DELETE_RESERVED_LT_ADDR_COMMAND, // The HCI_Delete_Reserved_LT_ADDR command requests that the BR/EDR Controller cancel the reservation of a specific LT_ADDR reserved for the purposes of Connectionless Peripheral Broadcast.
    HCI_DELETE_STORED_LINK_KEY_COMMAND, // The HCI_Delete_Stored_Link_Key command provides the ability to remove one or more of the link keys stored in the Controller.
    HCI_DISCONNECT_COMMAND, // The HCI_Disconnect command is used to terminate an existing BR/EDR or LE connection.
    HCI_DISCONNECTION_COMPLETE_EVENT, // The HCI_Disconnection_Complete event occurs when a connection has been terminated.
    HCI_ENABLE_IMPLEMENTATION_UNDER_TEST_MODE_COMMAND, // The HCI_Enable_Implementation_Under_Test_Mode command will allow the local Controller to enter test mode via LMP test commands. The Host issues this command when it wants the local device to be the IUT for the Testing scenarios as described in the Bluetooth Test Mode document.
    HCI_ENCRYPTION_CHANGE_EVENT, // The HCI_Encryption_Change event is used to indicate that the change in encryption has been completed for the specified Connection_Handle.
    HCI_ENCRYPTION_KEY_REFRESH_COMPLETE_EVENT, // The HCI_Encryption_Key_Refresh_Complete event is used to indicate to the Host that the encryption key was refreshed on the given Connection_Handle any time encryption is paused and then resumed.
    HCI_ENHANCED_ACCEPT_SYNCHRONOUS_CONNECTION_REQUEST_COMMAND, // The HCI_Enhanced_Accept_Synchronous_Connection_Request command is used to accept an incoming request for a synchronous connection and to inform the local Link Manager about the acceptable parameter values for the synchronous connection.
    HCI_ENHANCED_FLUSH_COMMAND, // The HCI_Enhanced_Flush command is used to discard specific packets currently pending for transmission in the Controller for the specified Handle. This command takes a parameter specifying the type of packets to be flushed.
    HCI_ENHANCED_FLUSH_COMPLETE_EVENT, // The HCI_Enhanced_Flush_Complete event is used to indicate that an Enhanced Flush is complete.
    HCI_ENHANCED_SETUP_SYNCHRONOUS_CONNECTION_COMMAND, // The HCI_Enhanced_Setup_Synchronous_Connection command adds a new or modifies an existing synchronous logical transport (SCO or eSCO) on a physical link depending on the Connection_Handle parameter specified.
    HCI_EXIT_PERIODIC_INQUIRY_MODE_COMMAND, // The HCI_Exit_Periodic_Inquiry_Mode command is used to end the Periodic Inquiry mode when the local device is in Periodic Inquiry Mode.
    HCI_EXIT_SNIFF_MODE_COMMAND, // The HCI_Exit_Sniff_Mode command is used to end Sniff mode for a Connection_Handle which is currently in Sniff mode.
    HCI_EXTENDED_INQUIRY_RESULT_EVENT, // The HCI_Extended_Inquiry_Result event indicates that a BR/EDR Controller has responded with an extended inquiry response during the current Inquiry process.
    HCI_FLOW_SPECIFICATION_COMMAND, // The HCI_Flow_Specification command is used to specify the flow parameters for the traffic carried over the ACL connection identified by the Connection_Handle.
    HCI_FLOW_SPECIFICATION_COMPLETE_EVENT, // The HCI_Flow_Specification_Complete event is used to inform the Host about the Quality of Service for the ACL connection the Controller is able to support.
    HCI_FLUSH_COMMAND, // The HCI_Flush command is used to discard all data that is currently pending for transmission in the Controller for the specified Connection_Handle.
    HCI_FLUSH_OCCURRED_EVENT, // The HCI_Flush_Occurred event is used to indicate that, for the specified Handle, the data to be transmitted has been discarded.
    HCI_GET_MWS_TRANSPORT_LAYER_CONFIGURATION_COMMAND, // The HCI_Get_MWS_Transport_Layer_Configuration command reads the supported baud rates from the Controller.
    HCI_HARDWARE_ERROR_EVENT, // The HCI_Hardware_Error event is used to indicate some type of hardware failure for the Controller.
    HCI_HOLD_MODE_COMMAND, // The HCI_Hold_Mode command is used to initiate Hold mode.
    HCI_HOST_BUFFER_SIZE_COMMAND, // The HCI_Host_Buffer_Size command is used by the Host to notify the Controller about its buffer sizes for ACL and synchronous data. The Controller will fragment the data to be transmitted from the Controller to the Host, so that data contained in HCI Data packets will not exceed these sizes.
    HCI_HOST_NUMBER_OF_COMPLETED_PACKETS_COMMAND, // The HCI_Host_Number_Of_Completed_Packets command is used by the Host to indicate to the Controller when the Host is ready to receive more HCI packets for any Connection_Handle.
    HCI_INQUIRY_CANCEL_COMMAND, // The HCI_Inquiry_Cancel command will cause the BR/EDR Controller to stop the current Inquiry if the BR/EDR Controller is in Inquiry Mode.
    HCI_INQUIRY_COMMAND, // The HCI_Inquiry command will cause the BR/EDR Controller to enter Inquiry Mode. Inquiry Mode is used to discovery other nearby BR/EDR Controllers.
    HCI_INQUIRY_COMPLETE_EVENT, // The HCI_Inquiry_Complete event indicates that the Inquiry is finished.
    HCI_INQUIRY_RESPONSE_NOTIFICATION_EVENT, // The HCI_Inquiry_Response_Notification event indicates to the Host that the BR/EDR Controller responded to an inquiry message.
    HCI_INQUIRY_RESULT_EVENT, // The HCI_Inquiry_Result event indicates that a BR/EDR Controller or multiple BR/EDR Controllers have responded so far during the current Inquiry process.
    HCI_INQUIRY_RESULT_WITH_RSSI_EVENT, // The HCI_Inquiry_Result_with_RSSI event indicates that a BR/EDR Controller or multiple BR/EDR Controllers have responded so far during the current Inquiry process.
    HCI_IO_CAPABILITY_REQUEST_EVENT, // The HCI_IO_Capability_Request event is used to indicate that the IO capabilities of the Host are required for a Secure Simple Pairing process.
    HCI_IO_CAPABILITY_REQUEST_NEGATIVE_REPLY_COMMAND, // The HCI_IO_Capability_Request_Negative_Reply command is used to reject a pairing attempt after an HCI_IO_Capability_Request event has been received by the Host.
    HCI_IO_CAPABILITY_REQUEST_REPLY_COMMAND, // The HCI_IO_Capability_Request_Reply command is used to reply to an HCI_IO_Capability_Request event from the Controller, and specifies the current IO capabilities of the Host.
    HCI_IO_CAPABILITY_RESPONSE_EVENT, // The HCI_IO_Capability_Response event is used to indicate to the Host that IO capabilities from a remote device specified by BD_ADDR have been received during a Secure Simple Pairing process.
    HCI_KEYPRESS_NOTIFICATION_EVENT, // The HCI_Keypress_Notification event is sent to the Host after a passkey notification has been received by the Link Manager on the given BD_ADDR.
    HCI_LE_ACCEPT_CIS_REQUEST_COMMAND, // The HCI_LE_Accept_CIS_Request command is used by the Peripheral’s Host to inform the Controller to accept the request for creating the CIS.
    HCI_LE_ADD_DEVICE_TO_FILTER_ACCEPT_LIST_COMMAND, // The HCI_LE_Add_Device_To_Filter_Accept_List command will add a device to the Filter Accept List.
    HCI_LE_ADD_DEVICE_TO_MONITORED_ADVERTISERS_LIST_COMMAND, // The HCI_LE_Add_Device_To_Monitored_Advertisers_List command will add a device to the Monitored Advertisers List.
    HCI_LE_ADD_DEVICE_TO_PERIODIC_ADVERTISER_LIST_COMMAND, // The HCI_LE_Add_Device_To_Periodic_Advertiser_List command will add a device to the Periodic Advertiser List.
    HCI_LE_ADD_DEVICE_TO_RESOLVING_LIST_COMMAND, // The HCI_LE_Add_Device_To_Resolving_List command is used to add one device to the resolving list used to resolve Resolvable Private Addresses in the Controller.
    HCI_LE_ADVERTISING_REPORT_EVENT, // The HCI_LE_Advertising_Report event indicates that an advertising or scan response packet has been received.
    HCI_LE_ADVERTISING_SET_TERMINATED_EVENT, // The HCI_LE_Advertising_Set_Terminated event indicates that advertising in a given advertising set has stopped.
    HCI_LE_BIG_CREATE_SYNC_COMMAND, // The HCI_LE_BIG_Create_Sync synchronizes and receives PDUs from one or more BISes.
    HCI_LE_BIG_SYNC_ESTABLISHED_EVENT, // The HCI_LE_BIG_Sync_Established event indicates that the Controller has completed an attempt to synchronize with the requested BISes.
    HCI_LE_BIG_SYNC_LOST_EVENT, // The HCI_LE_BIG_Sync_Lost event indicates that the Controller stopped synchronizing with a BIG.
    HCI_LE_BIG_TERMINATE_SYNC_COMMAND, // The HCI_LE_BIG_Terminate_Sync command stops or cancels synchronizing with a BIG.
    HCI_LE_BIGINFO_ADVERTISING_REPORT_EVENT, // The HCI_LE_BIGInfo_Advertising_Report event indicates that the Controller has received an Advertising PDU that contained a BIGInfo field.
    HCI_LE_CHANNEL_SELECTION_ALGORITHM_EVENT, // The HCI_LE_Channel_Selection_Algorithm event indicates the channel selection algorithm used on a connection.
    HCI_LE_CIS_ESTABLISHED_EVENT, // The HCI_LE_CIS_Established event indicates that the Controller established a CIS.
    HCI_LE_CIS_REQUEST_EVENT, // The HCI_LE_CIS_Request event indicates that the Peripheral’s Controller received a request from the Central to create a CIS.
    HCI_LE_CLEAR_ADVERTISING_SETS_COMMAND, // The HCI_LE_Clear_Advertising_Sets command will remove all existing advertising sets from the Controller.
    HCI_LE_CLEAR_FILTER_ACCEPT_LIST_COMMAND, // The HCI_LE_Clear_Filter_Accept_List command will clear the Filter Accept List.
    HCI_LE_CLEAR_MONITORED_ADVERTISERS_LIST_COMMAND, // The HCI_LE_Clear_Monitored_Advertisers_List command will clear the Monitored Advertisers List.
    HCI_LE_CLEAR_PERIODIC_ADVERTISER_LIST_COMMAND, // The HCI_LE_Clear_Periodic_Advertiser_List command will clear the Periodic Advertiser List.
    HCI_LE_CLEAR_RESOLVING_LIST_COMMAND, // The HCI_LE_Clear_Resolving_List command is used to remove all devices from the resolving list used to resolve Resolvable Private Addresses in the Controller.
    HCI_LE_CONNECTION_COMPLETE_EVENT, // The HCI_LE_Connection_Complete event indicates to the Host that a new connection has been created.
    HCI_LE_CONNECTION_CTE_REQUEST_ENABLE_COMMAND, // The HCI_LE_Connection_CTE_Request_Enable command will request the Controller to start or stop sending of LL_CTE_REQ PDUs on a connection.
    HCI_LE_CONNECTION_CTE_RESPONSE_ENABLE_COMMAND, // The HCI_LE_Connection_CTE_Response_Enable command will command the Controller to respond to LL_CTE_REQ PDUs with LL_CTE_RSP PDUs.
    HCI_LE_CONNECTION_IQ_REPORT_EVENT, // The HCI_LE_Connection_IQ_Report event is used to report IQ samples from the Constant Tone Extension field of a received packet containing an LL_CTE_RSP PDU.
    HCI_LE_CONNECTION_UPDATE_COMMAND, // The HCI_LE_Connection_Update command will be used to change the connection parameters of an existing connection.
    HCI_LE_CONNECTION_UPDATE_COMPLETE_EVENT, // The HCI_LE_Connection_Update_Complete event indicates the completion of the process to change the connection parameters.
    HCI_LE_CONNECTIONLESS_IQ_REPORT_EVENT, // The HCI_LE_Connectionless_IQ_Report event reports IQ information from the Constant Tone Extension of a received advertising packet.
    HCI_LE_CREATE_BIG_COMMAND, // The HCI_LE_Create_BIG command creates one or more BISes of a BIG.
    HCI_LE_CREATE_BIG_COMPLETE_EVENT, // The HCI_LE_Create_BIG_Complete event indicates that the Controller completed an attempt to create the BISes that were requested by the Host.
    HCI_LE_CREATE_BIG_TEST_COMMAND, // The HCI_LE_Create_BIG_Test command is used to create one or more BISes of a BIG for testing purposes.
    HCI_LE_CREATE_CIS_COMMAND, // The HCI_LE_Create_CIS command is used by the Central’s Host to create one or more CISes.
    HCI_LE_CREATE_CONNECTION_CANCEL_COMMAND, // The HCI_LE_Create_Connection_Cancel command is used to cancel an ongoing HCI_LE_Create_Connection command.
    HCI_LE_CREATE_CONNECTION_COMMAND, // The HCI_LE_Create_Connection command is used to create a new connection.
    HCI_LE_CS_CONFIG_COMPLETE_EVENT, // The HCI_LE_CS_Config_Complete event is used to report the status of a CS configuration initiated by either a local or remote Controller.
    HCI_LE_CS_CREATE_CONFIG_COMMAND, // The HCI_LE_CS_Create_Config command is used by a Host to create a CS configuration.
    HCI_LE_CS_PROCEDURE_ENABLE_COMMAND, // The HCI_LE_CS_Procedure_Enable command is used by a Host to enable the Controller to initiate the CS start procedure.
    HCI_LE_CS_PROCEDURE_ENABLE_COMPLETE_EVENT, // The HCI_LE_CS_Procedure_Enable_Complete event is used to report the status after enabling a new CS procedure or disabling an ongoing CS procedure.
    HCI_LE_CS_READ_LOCAL_SUPPORTED_CAPABILITIES_COMMAND, // The HCI_LE_CS_Read_Local_Supported_Capabilities command is used by a Host to query CS capabilities.
    HCI_LE_CS_READ_REMOTE_FAE_TABLE_COMMAND, // The HCI_LE_CS_Read_Remote_FAE_Table command is used by a Host to read the per-channel mode-0 Frequency Actuation Error table of the remote Controller.
    HCI_LE_CS_READ_REMOTE_FAE_TABLE_COMPLETE_EVENT, // The HCI_LE_CS_Read_Remote_FAE_Table_Complete event is used to report the completion of a locally initiated Channel Sounding mode-0 FAE Table Request procedure.
    HCI_LE_CS_READ_REMOTE_SUPPORTED_CAPABILITIES_COMMAND, // The HCI_LE_CS_Read_Remote_Supported_Capabilities command is used by a Host to query the CS capabilities of a remote device.
    HCI_LE_CS_READ_REMOTE_SUPPORTED_CAPABILITIES_COMPLETE_EVENT, // The HCI_LE_CS_Read_Remote_Supported_Capabilities_Complete event is used to report the CS capabilities of a remote device.
    HCI_LE_CS_REMOVE_CONFIG_COMMAND, // The HCI_LE_CS_Remove_Config command is used by a Host to remove an active CS configuration.
    HCI_LE_CS_SECURITY_ENABLE_COMMAND, // The HCI_LE_CS_Security_Enable command is used by a Host to start the CS Security procedure on the specified connection.
    HCI_LE_CS_SECURITY_ENABLE_COMPLETE_EVENT, // The HCI_LE_CS_Security_Enable_Complete event is used to report the completion of a CS Security Start procedure initiated by either a local or remote Controller.
    HCI_LE_CS_SET_CHANNEL_CLASSIFICATION_COMMAND, // The HCI_LE_CS_Set_Channel_Classification command is used by the Host to specify the channel classification to be used for CS procedures.
    HCI_LE_CS_SET_DEFAULT_SETTINGS_COMMAND, // The HCI_LE_CS_Set_Default_Settings command is used by a Host to set default CS settings in the local Controller.
    HCI_LE_CS_SET_PROCEDURE_PARAMETERS_COMMAND, // The HCI_LE_CS_Set_Procedure_Parameters command is used by the Host to specify the parameters to be used for scheduling CS procedures.
    HCI_LE_CS_SUBEVENT_RESULT_CONTINUE_EVENT, // The HCI_LE_CS_Subevent_Result_Continue event is used to report any remaining results of a CS subevent in the initiator or reflector.
    HCI_LE_CS_SUBEVENT_RESULT_EVENT, // The HCI_LE_CS_Subevent_Result event is used to report the results of a CS subevent in the initiator or reflector.
    HCI_LE_CS_TEST_COMMAND, // The HCI_LE_CS_Test command runs the CS test.
    HCI_LE_CS_TEST_END_COMMAND, // The HCI_LE_CS_Test_End command stops the CS test.
    HCI_LE_CS_TEST_END_COMPLETE_EVENT, // The HCI_LE_CS_Test_End_Complete event is used to report the termination of a CS test.
    HCI_LE_CS_WRITE_CACHED_REMOTE_FAE_TABLE_COMMAND, // The HCI_LE_CS_Write_Cached_Remote_FAE_Table command is used by a Host to write the per-channel mode-0 Frequency Actuation Error table of the remote device in the local Controller.
    HCI_LE_CS_WRITE_CACHED_REMOTE_SUPPORTED_CAPABILITIES_COMMAND, // The HCI_LE_CS_Write_Cached_Remote_Supported_Capabilities command is used by a Host to write a cached copy of the remote CS capabilities in a local Controller.
    HCI_LE_CTE_REQUEST_FAILED_EVENT, // The HCI_LE_CTE_Request_Failed event indicates a problem with a request generated by an HCI_LE_Connection_CTE_Request_Enable command for a peer device to send Constant Tone Extensions.
    HCI_LE_DATA_LENGTH_CHANGE_EVENT, // The HCI_LE_Data_Length_Change event is used to indicate a change in the maximum packet sizes by the Link Layer.
    HCI_LE_DIRECTED_ADVERTISING_REPORT_EVENT, // The HCI_LE_Directed_Advertising_Report event indicates that directed advertisements have been received where the advertiser is using a resolvable private address for the TargetA field in the ADV_DIRECT_IND PDU and the scanning filter policy is set to send this event to the Host.
    HCI_LE_ENABLE_ENCRYPTION_COMMAND, // The HCI_LE_Enable_Encryption command is used to enable link level encryption.
    HCI_LE_ENABLE_MONITORING_ADVERTISERS_COMMAND, // The HCI_LE_Enable_Monitoring_Advertisers command will enable monitoring advertisers that are in the Monitored Advertisers List.
    HCI_LE_ENCRYPT_COMMAND, // The HCI_LE_Encrypt command will encrypt a block of unencrypted data against a key and generate a block of encrypted data.
    HCI_LE_ENHANCED_CONNECTION_COMPLETE_EVENT, // The HCI_LE_Enhanced_Connection_Complete event indicates to the Host that a new connection has been created. This event contains the additional parameters of the local and peer resolvable private addresses.
    HCI_LE_ENHANCED_READ_TRANSMIT_POWER_LEVEL_COMMAND, // The HCI_LE_Enhanced_Read_Transmit_Power_Level command is used to read the current and maximum transmit power levels used by the local Controller on a specified PHY on an ACL connection.
    HCI_LE_EXTENDED_ADVERTISING_REPORT_EVENT, // The HCI_LE_Extended_Advertising_Report event indicates that an advertising packet has been received.
    HCI_LE_EXTENDED_CREATE_CONNECTION_COMMAND, // The HCI_LE_Extended_Create_Connection command is used to create a new connection supporting different initiating PHYs and to initiate a connection with a synchronized device.
    HCI_LE_FRAME_SPACE_UPDATE_COMMAND, // The HCI_LE_Frame_Space_Update command is used to initiate the Frame Space Update procedure.
    HCI_LE_FRAME_SPACE_UPDATE_COMPLETE_EVENT, // The HCI_LE_Frame_Space_Update_Complete event is used to inform the Host of changes to the frame space values.
    HCI_LE_GENERATE_DHKEY_CMMMMAND, // The HCI_LE_Generate_DHKey command is used to initiate generation of a Diffie-Hellman key in the Controller for use over the LE transport.
    HCI_LE_GENERATE_DHKEY_COMPLETE_EVENT, // The HCI_LE_Generate_DHKey_Complete event indicates that LE Diffie-Hellman key generation has been completed by the Controller.
    HCI_LE_ISO_READ_TEST_COUNTERS_COMMAND, // The HCI_LE_ISO_Read_Test_Counters command reads the test counters in the Controller which is configured in ISO Receive Test mode.
    HCI_LE_ISO_RECEIVE_TEST_COMMAND, // The HCI_LE_ISO_Receive_Test command configures a Link Layer to receive test payloads from an established CIS or a synchronized BIS.
    HCI_LE_ISO_TEST_END_COMMAND, // The HCI_LE_ISO_Test_End command terminates the ISO Transmit and/or Receive Test mode.
    HCI_LE_ISO_TRANSMIT_TEST_COMMAND, // The HCI_LE_ISO_Transmit_Test command configures an established CIS or BIS to transmit test payloads that are generated by the Controller.
    HCI_LE_LONG_TERM_KEY_REQUEST_EVENT, // The HCI_LE_Long_Term_Key_Request event indicates that a Long Term Key is required for a connection.
    HCI_LE_LONG_TERM_KEY_REQUEST_NEGATIVE_REPLY_COMMAND, // The HCI_LE_Long_Term_Key_Request_Negative_Reply command is used to reply to an HCI_LE_Long_Term_Key_Request event and indicates that the Host does not have a Long Term Key for that connection.
    HCI_LE_LONG_TERM_KEY_REQUEST_REPLY_COMMAND, // The HCI_LE_Long_Term_Key_Request_Reply command is used to reply to an HCI_LE_Long_Term_Key_Request event and includes the Long Term Key stored in the Host for that connection.
    HCI_LE_MODIFY_SLEEP_CLOCK_ACCURACY_COMMAND, // The HCI_LE_Modify_Sleep_Clock_Accuracy command requests the Controller changes its sleep clock accuracy for testing purposes.
    HCI_LE_MONITORED_ADVERTISERS_REPORT_EVENT, // The HCI_LE_Monitored_Advertisers_Report event is used by a Controller to send reports on advertising devices being monitored.
    HCI_LE_PATH_LOSS_THRESHOLD_EVENT, // The HCI_LE_Path_Loss_Threshold event is used to report a path loss threshold crossing on an ACL connection.
    HCI_LE_PERIODIC_ADVERTISING_CREATE_SYNC_CANCEL_COMMAND, // The HCI_LE_Periodic_Advertising_Create_Sync_Cancel command is used to cancel a pending HCI_LE_Periodic_Advertising_Create_Sync command.
    HCI_LE_PERIODIC_ADVERTISING_CREATE_SYNC_COMMAND, // The HCI_LE_Periodic_Advertising_Create_Sync command is used to start receiving periodic advertising packets from an advertiser.
    HCI_LE_PERIODIC_ADVERTISING_REPORT_EVENT, // The HCI_LE_Periodic_Advertising_Report event indicates that a periodic advertising packet has been received.
    HCI_LE_PERIODIC_ADVERTISING_SET_INFO_TRANSFER_COMMAND, // The HCI_LE_Periodic_Advertising_Set_Info_Transfer command is used to send periodic advertising synchronization information, describing periodic advertising events that the Controller is transmitting, to a connected Controller.
    HCI_LE_PERIODIC_ADVERTISING_RESPONSE_REPORT_EVENT, // The HCI_LE_Periodic_Advertising_Response_Report event is used to report response data to a Host.
    HCI_LE_PERIODIC_ADVERTISING_SUBEVENT_DATA_REQUEST_EVENT, // The HCI_LE_Periodic_Advertising_Subevent_Data_Request event is used to request subevent data from a Host.
    HCI_LE_PERIODIC_ADVERTISING_SYNC_ESTABLISHED_EVENT, // The HCI_LE_Periodic_Advertising_Sync_Established event indicates that the Controller has started receiving periodic advertising packets from an advertiser.
    HCI_LE_PERIODIC_ADVERTISING_SYNC_LOST_EVENT, // The HCI_LE_Periodic_Advertising_Sync_Lost event indicates the Controller has ended receiving a periodic advertising train.
    HCI_LE_PERIODIC_ADVERTISING_SYNC_TRANSFER_COMMAND, // The HCI_LE_Periodic_Advertising_Sync_Transfer command is used to send periodic advertising synchronization information to a connected Controller.
    HCI_LE_PERIODIC_ADVERTISING_SYNC_TRANSFER_RECEIVED_EVENT, // The HCI_LE_PLE_Periodic_Advertising_Sync_Transfer_Received event reports reception of periodic advertising synchronization information from a connected Controller.
    HCI_LE_PERIODIC_ADVERTISING_TERMINATE_SYNC_COMMAND, // The HCI_LE_Periodic_Advertising_Terminate_Sync command is used to end receiving of a periodic advertising train.
    HCI_LE_PHY_UPDATE_COMPLETE_EVENT, // The HCI_LE_PHY_Update_Complete event is used to inform the Host of the current PHY.
    HCI_LE_RAND_COMMAND, // The HCI_LE_Rand command will generate a random number.
    HCI_LE_READ_ADVERTISING_PHYSICAL_CHANNEL_TX_POWER_COMMAND, // The HCI_LE_Read_Advertising_Physical_Channel_Tx_Power command will read the transmit power level that will be used for advertising.
    HCI_LE_READ_ALL_LOCAL_SUPPORTED_FEATURES_COMMAND, // The HCI_LE_Read_All_Local_Supported_Features command will read all the features supported by the local LE Controller.
    HCI_LE_READ_ALL_REMOTE_FEATURES_COMMAND, // The HCI_LE_Read_All_Remote_Features command is used to read all the features used on a connection and the features supported by a remote LE device.
    HCI_LE_READ_ALL_REMOTE_FEATURES_COMPLETE_EVENT, // The HCI_LE_Read_All_Remote_Features_Complete event indicates the completion of the process to read all the features used on a connection and the features supported by a remote LE device.
    HCI_LE_READ_ANTENNA_INFORMATION_COMMAND, // The HCI_LE_Read_Antenna_Information command allows the Host to read the switching rates, the sampling rates, the number of antennae, and the maximum length of the Constant Tone Extension supported by the Controller.
    HCI_LE_READ_BUFFER_SIZE_COMMAND, // The HCI_LE_Read_Buffer_Size command returns the size of the HCI buffers. These buffers are used by the LE Controller to buffer data that is to be transmitted.
    HCI_LE_READ_CHANNEL_MAP_COMMAND, // The HCI_LE_Read_Channel_Map command will read the current state of the channel map for a connection.
    HCI_LE_READ_FILTER_ACCEPT_LIST_SIZE_COMMAND, // The HCI_LE_Read_Filter_Accept_List_Size command will read the maximum number of Filter Accept List entries that this Controller supports.
    HCI_LE_READ_ISO_LINK_QUALITY_COMMAND, // The HCI_LE_Read_ISO_Link_Quality command returns the value of various counters related to link quality on an isochronous stream.
    HCI_LE_READ_ISO_TX_SYNC_COMMAND, // The HCI_LE_Read_ISO_TX_Sync command is used to read the Time_Stamp and Time_Offset of a transmitted SDU.
    HCI_LE_READ_LOCAL_P_256_PUBLIC_KEY_COMMAND, // The HCI_LE_Read_Local_P-256_Public_Key command is used to return the local P-256 public key from the Controller.
    HCI_LE_READ_LOCAL_P_256_PUBLIC_KEY_COMPLETE_EVENT, // The HCI_LE_Read_Local_P-256_Public_Key_Complete event is generated when local P-256 key generation is complete.
    HCI_LE_READ_LOCAL_RESOLVABLE_ADDRESS_COMMAND, // The HCI_LE_Read_Local_Resolvable_Address command is used to get the current local Resolvable Private Address being used for the corresponding peer Identity Address.
    HCI_LE_READ_LOCAL_SUPPORTED_FEATURES_PAGE_0_COMMAND, // The HCI_LE_Read_Local_Supported_Features_Page_0 command will read page 0 of the features supported by the local LE Controller.
    HCI_LE_READ_MAXIMUM_ADVERTISING_DATA_LENGTH_COMMAND, // The HCI_LE_Read_Maximum_Advertising_Data_Length command will read the maximum length of advertising data that the advertising Controller supports in a given advertising set.
    HCI_LE_READ_MAXIMUM_DATA_LENGTH_COMMAND, // The HCI_LE_Read_Maximum_Data_Length command allows the Host to read the Controller’s supportedMaxTxOctets, supportedMaxTxTime, supportedMaxRxOctets, and supportedMaxRxTime parameters.
    HCI_LE_READ_MONITORED_ADVERTISERS_LIST_SIZE_COMMAND, // The HCI_LE_Read_Monitored_Advertisers_List_Size command will read the number of Monitored Advertisers List entries that the Controller supports.
    HCI_LE_READ_NUMBER_OF_SUPPORTED_ADVERTISING_SETS_COMMAND, // The HCI_LE_Read_Number_of_Supported_Advertising_Sets command will read the maximum number of advertising sets supported by the advertising Controller at the same time.
    HCI_LE_READ_PEER_RESOLVABLE_ADDRESS_COMMAND, // The HCI_LE_Read_Peer_Resolvable_Address command is used to get the current peer Resolvable Private Address being used for the corresponding peer Public and Random (static) Identity Address.
    HCI_LE_READ_PERIODIC_ADVERTISER_LIST_SIZE_COMMAND, // The HCI_LE_Read_Periodic_Advertiser_List_Size command will read the maximum number of Periodic Advertiser List entries that the Controller supports.
    HCI_LE_READ_PHY_COMMAND, // The HCI_LE_Read_PHY command will read the current PHY.
    HCI_LE_READ_REMOTE_FEATURES_PAGE_0_COMMAND, // The HCI_LE_Read_Remote_Features_Page_0 command is used to read page 0 of the features used on a connection and the features supported by a remote LE device.
    HCI_LE_READ_REMOTE_FEATURES_PAGE_0_COMPLETE_EVENT, // The HCI_LE_Read_Remote_Features_Page_0_Complete event indicates the completion of the process to read page 0 of the features used on a connection and the features supported by a remote LE device.
    HCI_LE_READ_REMOTE_TRANSMIT_POWER_LEVEL_COMMAND, // The HCI_LE_Read_Remote_Transmit_Power_Level command is used to read the transmit power level used by the remote Controller on a specified PHY on an ACL connection.
    HCI_LE_READ_RESOLVING_LIST_SIZE_COMMAND, // The HCI_LE_Read_Resolving_List_Size command is used to read the total number of entries in the resolving list that can be stored in the Controller.
    HCI_LE_READ_RF_PATH_COMPENSATION_COMMAND, // The HCI_LE_Read_RF_Path_Compensation command is used to read the RF Path Compensation Value.
    HCI_LE_READ_SUGGESTED_DEFAULT_DATA_LENGTH_COMMAND, // The HCI_LE_Read_Suggested_Default_Data_Length command allows the Host to read the initial MaxTxOctets and MaxTxTime values for new connections it suggested to the Controller.
    HCI_LE_READ_SUPPORTED_STATES_COMMAND, // The HCI_LE_Read_Supported_States command will read the current supported state and role combinations for the local LE Controllers.
    HCI_LE_READ_TRANSMIT_POWER_COMMAND, // The HCI_LE_Read_Transmit_Power command will read the minimum and maximum transmit powers supported by the Controller.
    HCI_LE_RECEIVER_TEST_COMMAND, // The HCI_LE_Receiver_Test command will run the LE receiver test.
    HCI_LE_REJECT_CIS_REQUEST_COMMAND, // The HCI_LE_Reject_CIS_Request command is used by the Peripheral’s Host to inform the Controller to reject the request for creating the CIS.
    HCI_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_EVENT, // The HCI_LE_Remote_Connection_Parameter_Request event is used to indicate to the Host that the remote device is requesting a change in the connection parameters.
    HCI_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_NEGATIVE_REPLY_COMMAND, // The HCI_LE_Remote_Connection_Parameter_Request_Negative_Reply command is used to reject the remote device’s request to change the connection parameters of the LE connection.
    HCI_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_REPLY_COMMAND, // The HCI_LE_Remote_Connection_Parameter_Request_Reply command is used to accept the remote device’s request to change the connection parameters of the LE connection.
    HCI_LE_REMOVE_ADVERTISING_SET_COMMAND, // The HCI_LE_Remove_Advertising_Set command will remove an advertising set from the Controller.
    HCI_LE_REMOVE_CIG_COMMAND, // The HCI_LE_Remove_CIG command is used by the Central’s Host to remove a CIG from the Controller.
    HCI_LE_REMOVE_DEVICE_FROM_FILTER_ACCEPT_LIST_COMMAND, // The HCI_LE_Remove_Device_From_Filter_Accept_List command will remove a single device from the Filter Accept List.
    HCI_LE_REMOVE_DEVICE_FROM_MONITORED_ADVERTISERS_LIST_COMMAND, // The HCI_LE_Remove_Device_From_Monitored_Advertisers_List command will remove a device from the Monitored Advertisers list.
    HCI_LE_REMOVE_DEVICE_FROM_PERIODIC_ADVERTISER_LIST_COMMAND, // The HCI_LE_Remove_Device_From_Periodic_Advertiser_List command will remove a single device from the Periodic Advertiser List.
    HCI_LE_REMOVE_DEVICE_FROM_RESOLVING_LIST_COMMAND, // The HCI_LE_Remove_Device_From_Resolving_List command is used to remove one device from the resolving list used to resolve Resolvable Private Addresses in the Controller.
    HCI_LE_REMOVE_ISO_DATA_PATH_COMMAND, // The HCI_LE_Remove_ISO_Data_Path command removes an isochronous data path between the Host and the Controller.
    HCI_LE_REQUEST_PEER_SCA_COMMAND, // The HCI_LE_Request_Peer_SCA command requests the Sleep Clock Accuracy of the peer device.
    HCI_LE_REQUEST_PEER_SCA_COMPLETE_EVENT, // The HCI_LE_Request_Peer_SCA_Complete event indicates that the Controller completed the attempt to read the Sleep Clock Accuracy (SCA) of the peer device.
    HCI_LE_SCAN_REQUEST_RECEIVED_EVENT, // The HCI_LE_Scan_Request_Received event indicates that a scan request has been received.
    HCI_LE_SCAN_TIMEOUT_EVENT, // The HCI_LE_Scan_Timeout event indicates that scanning has finished.
    HCI_LE_SET_ADDRESS_RESOLUTION_ENABLE_COMMAND, // The HCI_LE_Set_Address_Resolution_Enable command is used to enable resolution of Resolvable Private Addresses in the Controller.
    HCI_LE_SET_ADVERTISING_DATA_COMMAND, // The HCI_LE_Set_Advertising_Data command will set the data transmitted when advertising.
    HCI_LE_SET_ADVERTISING_ENABLE_COMMAND, // The HCI_LE_Set_Advertising_Enable command will enable or disable advertising.
    HCI_LE_SET_ADVERTISING_PARAMETERS_COMMAND, // The HCI_LE_Set_Advertising_Parameters command will set the parameters used for advertising.
    HCI_LE_SET_ADVERTISING_SET_RANDOM_ADDRESS_COMMAND, // The HCI_LE_Set_Advertising_Set_Random_Address command will set the random address used in advertising.
    HCI_LE_SET_CIG_PARAMETERS_COMMAND, // The HCI_LE_Set_CIG_Parameters command is used by a Central’s Host to set the parameters of one or more Connected Isochronous Streams (CISes) that are associated with a CIG in the Controller.
    HCI_LE_SET_CIG_PARAMETERS_TEST_COMMAND, // The HCI_LE_Set_CIG_Parameters_Test command is used by a Central’s Host to set the parameters of one or more CISes that are associated with a CIG in the Controller for testing purposes.
    HCI_LE_SET_CONNECTION_CTE_RECEIVE_PARAMETERS_COMMAND, // The HCI_LE_Set_Connection_CTE_Receive_Parameters command will set the antenna-switching pattern, switching and sampling slot durations for receiving the Constant Tone Extension on a connection.
    HCI_LE_SET_CONNECTION_CTE_TRANSMIT_PARAMETERS_COMMAND, // The HCI_LE_Set_Connection_CTE_Transmit_Parameters command will set the antenna-switching pattern, switching and sampling slot durations for transmitting the Constant Tone Extension on a connection.
    HCI_LE_SET_CONNECTIONLESS_CTE_TRANSMIT_ENABLE_COMMAND, // The HCI_LE_Set_Connectionless_CTE_Transmit_Enable command will request the Controller to enable or disable sending packets containing a Constant Tone Extension.
    HCI_LE_SET_CONNECTIONLESS_CTE_TRANSMIT_PARAMETERS_COMMAND, // The HCI_LE_Set_Connectionless_CTE_Transmit_Parameters command will set the antenna-switching pattern and switching and sampling slot durations for the transmission of Constant Tone Extensions.
    HCI_LE_SET_CONNECTIONLESS_IQ_SAMPLING_ENABLE_COMMAND, // The HCI_LE_Set_Connectionless_IQ_Sampling_Enable command will request the Controller to enable or disable taking IQ samples from the Constant Tone Extension of advertising packet
    HCI_LE_SET_DATA_LENGTH_COMMAND, // The HCI_LE_Set_Data_Length command is used to suggest maximum packet sizes to the Controller.
    HCI_LE_SET_DATA_RELATED_ADDRESS_CHANGES_COMMAND, // The HCI_LE_Set_Data_Related_Address_Changes command specifies circumstances when the Controller shall refresh any Resolvable Private Address used by an advertising set, whether or not the address timeout period has been reached.
    HCI_LE_SET_DECISION_DATA_COMMAND, // The HCI_LE_Set_Decision_Data command will set the decision data transmitted when advertising.
    HCI_LE_SET_DECISION_INSTRUCTIONS_COMMAND, // The HCI_LE_Set_Decision_Instructions command will set the decision instructions used when scanning for advertisements that include decision data.
    HCI_LE_SET_DEFAULT_PERIODIC_ADVERTISING_SYNC_TRANSFER_PARAMETERS_COMMAND, // The HCI_LE_Set_Default_Periodic_Advertising_Sync_Transfer_Parameters command is used to specify the default behavior of the Controller when periodic advertising synchronization information is received from a connected Controller.
    HCI_LE_SET_DEFAULT_PHY_COMMAND, // The HCI_LE_Set_Default_PHY command is used to configure preferred PHYs for new connections for the local device.
    HCI_LE_SET_DEFAULT_SUBRATE_COMMAND, // The HCI_LE_Set_Default_Subrate command sets the range of the min and max subrates and other subrate parameters on a Central that may be requested by a Peripheral.
    HCI_LE_SET_EVENT_MASK_COMMAND, // The HCI_LE_Set_Event_Mask command is used to control which events are generated by the HCI for the Host.
    HCI_LE_SET_EXTENDED_ADVERTISING_DATA_COMMAND, // The HCI_LE_Set_Extended_Advertising_Data command will set the advertising data transmitted when advertising.
    HCI_LE_SET_EXTENDED_ADVERTISING_ENABLE_COMMAND, // The HCI_LE_Set_Extended_Advertising_Enable command will enable or disable advertising.
    HCI_LE_SET_EXTENDED_ADVERTISING_PARAMETERS_COMMAND, // The HCI_LE_Set_Extended_Advertising_Parameters command will set the parameters used for advertising.
    HCI_LE_SET_EXTENDED_SCAN_ENABLE_COMMAND, // The HCI_LE_Set_Extended_Scan_Enable command will enable or disable scanning on the primary advertising physical channels.
    HCI_LE_SET_EXTENDED_SCAN_PARAMETERS_COMMAND, // The HCI_LE_Set_Extended_Scan_Parameters command will set the parameters used for scanning on the primary advertising physical channel.
    HCI_LE_SET_EXTENDED_SCAN_RESPONSE_DATA_COMMAND, // The HCI_LE_Set_Extended_Scan_Response_Data command will set the data transmitted in a scan response.
    HCI_LE_SET_HOST_CHANNEL_CLASSIFICATION_COMMAND, // The HCI_LE_Set_Host_Channel_Classification command allows the Host to specify a channel classification based on its "local information".
    HCI_LE_SET_HOST_FEATURE_COMMAND, // The HCI_LE_Set_Host_Feature command is used to set or clear a bit controlled by the Host in the Link Layer FeatureSet stored in the Controller.
    HCI_LE_SET_PATH_LOSS_REPORTING_ENABLE_COMMAND, // The HCI_LE_Set_Path_Loss_Reporting_Enable command is used to enable or disable path loss reporting events for an ACL connection.
    HCI_LE_SET_PATH_LOSS_REPORTING_PARAMETERS_COMMAND, // The HCI_LE_Set_Path_Loss_Reporting_Parameters command is used to set the path loss threshold and related parameters used to trigger reports for an ACL connection.
    HCI_LE_SET_PERIODIC_ADVERTISING_DATA_COMMAND, // The HCI_LE_Set_Periodic_Advertising_Data command will set the periodic advertising data transmitted when advertising.
    HCI_LE_SET_PERIODIC_ADVERTISING_ENABLE_COMMAND, // The HCI_LE_Set_Periodic_Advertising_Enable command will enable or disable periodic advertising.
    HCI_LE_SET_PERIODIC_ADVERTISING_PARAMETERS_COMMAND, // The HCI_LE_Set_Periodic_Advertising_Parameters command will set the parameters used for periodic advertising.
    HCI_LE_SET_PERIODIC_ADVERTISING_RECEIVE_ENABLE_COMMAND, // The HCI_LE_Set_Periodic_Advertising_Receive_Enable command will enable or disable periodic advertising reports once synchronized.
    HCI_LE_SET_PERIODIC_ADVERTISING_RESPONSE_DATA_COMMAND, // The HCI_LE_Set_Periodic_Advertising_Response_Data command is used to set the data for a response slot.
    HCI_LE_SET_PERIODIC_ADVERTISING_SUBEVENT_DATA_COMMAND, // The HCI_LE_Set_Periodic_Advertising_Subevent_Data command is used to send subevent data for one or more subevents.
    HCI_LE_SET_PERIODIC_SYNC_SUBEVENT_COMMAND, // The HCI_LE_Set_Periodic_Sync_Subevent command is used to configure the subset of subevents a device will synchronize with.
    HCI_LE_SET_PERIODIC_ADVERTISING_SYNC_TRANSFER_PARAMETERS_COMMAND, // The HCI_LE_Set_Periodic_Advertising_Sync_Transfer_Parameters command is used to allow the Host to specify the behavior of the Controller when periodic advertising synchronization information is received from a connected Controller.
    HCI_LE_SET_PHY_COMMAND, // The HCI_LE_Set_PHY command is used to request a change of the PHY for a Connection_Handle.
    HCI_LE_SET_PRIVACY_MODE_COMMAND, // The HCI_LE_Set_Privacy_Mode command is used to allow the Host to specify the privacy mode for an entry on the resolving list.
    HCI_LE_SET_RANDOM_ADDRESS_COMMAND, // The HCI_LE_Set_Random_Address command will set the Random Device Address that may be used in a packet sent on the advertising physical channel.
    HCI_LE_SET_RESOLVABLE_PRIVATE_ADDRESS_TIMEOUT_COMMAND, // The HCI_LE_Set_Resolvable_Private_Address_Timeout sets the length of time the Controller uses a random private address before a new random private address is generated and starts being used.
    HCI_LE_SET_SCAN_ENABLE_COMMAND, // The HCI_LE_Set_Scan_Enable command will enable or disable scanning.
    HCI_LE_SET_SCAN_PARAMETERS_COMMAND, // The HCI_LE_Set_Scan_Parameters command will set the parameters used for scanning.
    HCI_LE_SET_SCAN_RESPONSE_DATA_COMMAND, // The HCI_LE_Set_Scan_Response_Data command will set the data transmitted in a scan response.
    HCI_LE_SET_TRANSMIT_POWER_REPORTING_ENABLE_COMMAND, // The HCI_LE_Set_Transmit_Power_Reporting_Enable command is used to enable or disable reporting to the local Host of transmit power level changes on an ACL connection.
    HCI_LE_SETUP_ISO_DATA_PATH_COMMAND, // The HCI_LE_Setup_ISO_Data_Path command identifies and creates the isochronous data path between the Host and the Controller and optionally configures the codec in the Controller.
    HCI_LE_SUBRATE_CHANGE_EVENT, // The HCI_LE_Subrate_Change event indicates that a new subrate factor has been applied to an existing ACL connection.
    HCI_LE_SUBRATE_REQUEST_COMMAND, // The HCI_LE_Subrate_Request command modifies an existing ACL connection by applying a subrate factor.
    HCI_LE_TERMINATE_BIG_COMMAND, // The HCI_LE_Terminate_BIG command terminates the transmission of all BISes of a BIG or cancels the process of creating a BIG.
    HCI_LE_TERMINATE_BIG_COMPLETE_EVENT, // The HCI_LE_Terminate_BIG_Complete event indicates that the transmission of all the BISes in the BIG have been terminated.
    HCI_LE_TEST_END_COMMAND, // The HCI_LE_Test_End command will end the current the receiver or transmitter test.
    HCI_LE_TRANSMITTER_TEST_COMMAND, // The HCI_LE_Transmitter_Test command will run the LE transmitter test.
    HCI_LE_WRITE_RF_PATH_COMPENSATION_COMMAND, // The HCI_LE_Write_RF_Path_Compensation command is used to indicate the RF path gain or loss from the RF transceiver output to the antenna output contributed by intermediate components.
    HCI_LE_WRITE_SUGGESTED_DEFAULT_DATA_LENGTH_COMMAND, // The HCI_LE_Write_Suggested_Default_Data_Length command allows the Host to suggest initial MaxTxOctets and MaxTxTime values for new connections.
    HCI_LINK_KEY_NOTIFICATION_EVENT, // The HCI_Link_Key_Notification event is used to indicate to the Host that a new Link Key has been created for the connection with the BR/EDR Controller specified in BD_ADDR.
    HCI_LINK_KEY_REQUEST_EVENT, // The HCI_Link_Key_Request event is used to indicate that a Link Key is required for the connection with the device specified in BD_ADDR.
    HCI_LINK_KEY_REQUEST_NEGATIVE_REPLY_COMMAND, // The HCI_Link_Key_Request_Negative_Reply command is used to reply to an HCI_Link_Key_Request event from the BR/EDR Controller if the Host does not have a stored Link Key for the connection with the other BR/EDR Controller specified by BD_ADDR.
    HCI_LINK_KEY_REQUEST_REPLY_COMMAND, // The HCI_Link_Key_Request_Reply command is used to reply to an HCI_Link_Key_Request event from the BR/EDR Controller, and specifies the Link Key stored on the Host to be used as the link key for the connection with the other BR/EDR Controller specified by BD_ADDR.
    HCI_LINK_KEY_SELECTION_COMMAND, // The HCI_Link_Key_Selection command is used to force both BR/EDR Controllers of a connection associated to the Connection_Handle to use the temporary link key of the Central or the regular link keys.
    HCI_LINK_KEY_TYPE_CHANGED_EVENT, // The HCI_Link_Key_Type_Changed event is used to indicate that the change in the temporary Link Key or in the semi-permanent link keys on the Bluetooth Central side has been completed.
    HCI_LINK_SUPERVISION_TIMEOUT_CHANGED_EVENT, // The HCI_Link_Supervision_Timeout_Changed event indicates that the remote device changed the Link Supervision Timeout.
    HCI_LOOPBACK_COMMAND_EVENT, // The HCI_Loopback_Command event is used to loop back all commands that the Host sends to the BR/EDR Controller with some exceptions.
    HCI_MAX_SLOTS_CHANGE_EVENT, // The HCI_Max_Slots_Change event is used to indicate a change in the max slots by the LM.
    HCI_MODE_CHANGE_EVENT, // The HCI_Mode_Change event is used to indicate that the current mode has changed.
    HCI_NUMBER_OF_COMPLETED_DATA_BLOCKS_EVENT, // The HCI_Number_Of_Completed_Data_Blocks event is used by the Controller to indicate to the Host how many HCI ACL Data packets have been completed and how many data block buffers have been freed for each Handle since the previous HCI_Number_Of_Completed_Data_Blocks event was sent.
    HCI_NUMBER_OF_COMPLETED_PACKETS_EVENT, // The HCI_Number_Of_Completed_Packets event is used by the Controller to indicate to the Host how many HCI Data packets have been completed for each Connection_Handle since the previous HCI_Number_Of_Completed_Packets event was sent.
    HCI_PAGE_SCAN_REPETITION_MODE_CHANGE_EVENT, // The HCI_Page_Scan_Repetition_Mode_Change event indicates that the connected remote BR/EDR Controller with the specified Connection_Handle has successfully changed the Page Scan Repetition Mode (SR).
    HCI_PERIODIC_INQUIRY_MODE_COMMAND, // The HCI_Periodic_Inquiry_Mode command is used to configure the BR/EDR Controller to perform an automatic Inquiry based on a specified period range.
    HCI_PERIPHERAL_PAGE_RESPONSE_TIMEOUT_EVENT, // The HCI_Peripheral_Page_Response_Timeout event indicates to the Host that the pagerespTO has been exceeded on the BR/EDR Controller after the Controller responded to an ID packet.
    HCI_PIN_CODE_REQUEST_EVENT, // The HCI_PIN_Code_Request event is used to indicate that a PIN code is required to create a new link key for a connection.
    HCI_PIN_CODE_REQUEST_NEGATIVE_REPLY_COMMAND, // The HCI_PIN_Code_Request_Negative_Reply command is used to reply to an HCI_PIN_Code_Request event from the Controller when the Host cannot specify a PIN code to use for a connection.
    HCI_PIN_CODE_REQUEST_REPLY_COMMAND, // The HCI_PIN_Code_Request_Reply command is used to reply to an HCI_PIN_Code_Request event from the Controller and specifies the PIN code to use for a connection.
    HCI_QOS_SETUP_COMMAND, // The HCI_QoS_Setup command is used to specify Quality of Service parameters for a Connection_Handle.
    HCI_QOS_SETUP_COMPLETE_EVENT, // The HCI_QoS_Setup_Complete event is used to indicate that QoS is set up.
    HCI_QOS_VIOLATION_EVENT, // The HCI_QoS_Violation event is used to indicate the Controller's Link Manager is unable to provide the current QoS requirement for the Handle.
    HCI_READ_AFH_CHANNEL_ASSESSMENT_MODE_COMMAND, // The HCI_Read_AFH_Channel_Assessment_Mode command will read the value for the AFH Channel Classification Mode parameter. This value is used to enable or disable the Controller's channel assessment scheme.
    HCI_READ_AFH_CHANNEL_MAP_COMMAND, // The HCI_Read_AFH_Channel_Map command will read the current state of the channel map for a connection.
    HCI_READ_AUTHENTICATED_PAYLOAD_TIMEOUT_COMMAND, // The HCI_Read_Authenticated_Payload_Timeout command is used to read the Authenticated Payload Timeout parameter, which is used to set the maximum time between packets being received from the remote device without a valid MIC.
    HCI_READ_AUTHENTICATION_ENABLE_COMMAND, // The HCI_Read_Authentication_Enable command will read the value for the Authentication Enable parameter, which controls whether the Bluetooth device will require authentication for each connection with other Bluetooth devices.
    HCI_READ_AUTOMATIC_FLUSH_TIMEOUT_COMMAND, // The HCI_Read_Automatic_Flush_Timeout command will read the value for the Flush Timeout configuration parameter for the specified Connection_Handle. The Flush Timeout parameter is only used for ACL connections.
    HCI_READ_BD_ADDR_COMMAND, // The HCI_Read_BD_ADDR command will read the value for the BD_ADDR parameter.
    HCI_READ_BUFFER_SIZE_COMMAND, // The HCI_Read_Buffer_Size command returns the size of the HCI buffers. These buffers are used by the Controller to buffer data that is to be transmitted.
    HCI_READ_CLASS_OF_DEVICE_COMMAND, // The HCI_Read_Class_of_Device command will read the value for the Class of Device configuration parameter, which is used to indicate its capabilities to other devices.
    HCI_READ_CLOCK_COMMAND, // The HCI_Read_Clock command will read an estimate of a piconet or the local Bluetooth Clock.
    HCI_READ_CLOCK_OFFSET_COMMAND, // The HCI_Read_Clock_Offset command allows the Host to read the clock offset of remote BR/EDR Controllers.
    HCI_READ_CLOCK_OFFSET_COMPLETE_EVENT, // The HCI_Read_Clock_Offset_Complete event is used to indicate the completion of the process of the LM obtaining the Clock offset information.
    HCI_READ_CONNECTION_ACCEPT_TIMEOUT_COMMAND, // The HCI_Read_Connection_Accept_Timeout command will read the value for the Connection Accept Timeout configuration parameter, which allows the Controller to automatically deny a connection request after a specified period has occurred, and to refuse a new connection.
    HCI_READ_CURRENT_IAC_LAP_COMMAND, // The HCI_Read_Current_IAC_LAP command will read the LAP(s) used to create the Inquiry Access Codes (IAC) that the local BR/EDR Controller is simultaneously scanning for during Inquiry Scans.
    HCI_READ_DATA_BLOCK_SIZE_COMMAND, // The HCI_Read_Data_Block_Size command returns the maximum size of the HCI buffers. These buffers are used by the Controller to buffer data that is to be transmitted.
    HCI_READ_DEFAULT_ERRONEOUS_DATA_REPORTING_COMMAND, // The HCI_Read_Default_Erroneous_Data_Reporting command will read the value for the Erroneous Data Reporting configuration parameter, which controls whether the BR/EDR Controller will provide data for every (e)SCO interval, with the Packet_Status_Flag in HCI Synchronous Data packets set according to HCI Synchronous Data packets.
    HCI_READ_DEFAULT_LINK_POLICY_SETTINGS_COMMAND, // The HCI_Read_Default_Link_Policy_Settings command will read the Default Link Policy configuration parameter for all new connections.
    HCI_READ_ENCRYPTION_KEY_SIZE_COMMAND, // The HCI_Read_Encryption_Key_Size command is used to read the encryption key size on a given Connection_Handle.
    HCI_READ_ENHANCED_TRANSMIT_POWER_LEVEL_COMMAND, // The HCI_Read_Enhanced_Transmit_Power_Level command will read the values for the GFSK, π/4-DQPSK and 8DPSK Transmit Power Level parameters for the specified Connection_Handle.
    HCI_READ_EXTENDED_INQUIRY_LENGTH_COMMAND, // The HCI_Read_Extended_Inquiry_Length command is used to read the Extended Inquiry Length parameter from the Controller.
    HCI_READ_EXTENDED_INQUIRY_RESPONSE_COMMAND, // The HCI_Read_Extended_Inquiry_Response command will read the data that the BR/EDR Controller sends in the extended inquiry response packet during inquiry response.
    HCI_READ_EXTENDED_PAGE_TIMEOUT_COMMAND, // The HCI_Read_Extended_Page_Timeout command is used to read the Extended Page Timeout parameter from the Controller.
    HCI_READ_FAILED_CONTACT_COUNTER_COMMAND, // The HCI_Read_Failed_Contact_Counter command will read the value for the Failed Contact Counter configuration parameter for a particular connection to another device.
    HCI_READ_FLOW_CONTROL_MODE_COMMAND, // The HCI_Read_Flow_Control_Mode command returns the value of the Flow_Control_Mode configuration parameter supported by this Controller.
    HCI_READ_HOLD_MODE_ACTIVITY_COMMAND, // The HCI_Read_Hold_Mode_Activity command is used to read which activities should be suspended when the BR/EDR Controller is in Hold mode.
    HCI_READ_INQUIRY_MODE_COMMAND, // The HCI_Read_Inquiry_Mode command is used to read the Inquiry Mode configuration parameter of the local BR/EDR Controller.
    HCI_READ_INQUIRY_RESPONSE_TRANSMIT_POWER_LEVEL_COMMAND, // The HCI_Read_Inquiry_Response_Transmit_Power_Level command will read the inquiry response Transmit Power level used to transmit the FHS and EIR data packets. This can be used directly in the Tx Power Level EIR data type.
    HCI_READ_INQUIRY_SCAN_ACTIVITY_COMMAND, // The HCI_Read_Inquiry_Scan_Activity command will read the value for Inquiry Scan Interval and Inquiry Scan Window configuration parameters. Inquiry Scan Interval defines the amount of time between consecutive inquiry scans. Inquiry Scan Window defines the amount of time for the duration of the inquiry scan.
    HCI_READ_INQUIRY_SCAN_TYPE_COMMAND, // The HCI_Read_Inquiry_Scan_Type command is used to read the Inquiry Scan Type configuration parameter of the local BR/EDR Controller. The Inquiry Scan Type configuration parameter can set the inquiry scan to either normal or interlaced scan.
    HCI_READ_LE_HOST_SUPPORT_COMMAND, // The HCI_Read_LE_Host_Support command reads the LE Supported Host setting from the BR/EDR Controller.
    HCI_READ_LINK_POLICY_SETTINGS_COMMAND, // The HCI_Read_Link_Policy_Settings command will read the Link Policy configuration parameter for the specified Connection_Handle. The Link Policy settings allow the Host to specify which Link Modes the Link Manager can use for the specified Connection_Handle.
    HCI_READ_LINK_QUALITY_COMMAND, // The HCI_Read_Link_Quality command will read the value for the Link Quality for the specified Connection_Handle.
    HCI_READ_LINK_SUPERVISION_TIMEOUT_COMMAND, // The HCI_Read_Link_Supervision_Timeout command will read the value for the Link Supervision Timeout configuration parameter for the device. This parameter is used by the Controller to determine link loss.
    HCI_READ_LMP_HANDLE_COMMAND, // The HCI_Read_LMP_Handle command will read the current LMP Handle associated with the Connection_Handle.
    HCI_READ_LOCAL_EXTENDED_FEATURES_COMMAND, // The HCI_Read_Local_Extended_Features command requests a list of the supported extended features for the local device.
    HCI_READ_LOCAL_NAME_COMMAND, // The HCI_Read_Local_Name command provides the ability to read the stored user-friendly name for the BR/EDR Controller.
    HCI_READ_LOCAL_OOB_DATA_COMMAND, // The HCI_Read_Local_OOB_Data command is used to obtain a Secure Simple Pairing Hash C and Randomizer R which are intended to be transferred to a remote device using an OOB mechanism.
    HCI_READ_LOCAL_OOB_EXTENDED_DATA_COMMAND, // The HCI_Read_Local_OOB_Extended_Data command is used to obtain a Secure Simple Pairing Hash C and Randomizer R associated with both P-192 and P-256 public keys, which are intended to be transferred to a remote device using an OOB mechanism.
    HCI_READ_LOCAL_SIMPLE_PAIRING_OPTIONS_COMMAND, // The HCI_Read_Local_Simple_Pairing_Options command is used to read the Secure Simple Pairing options and the maximum encryption key size supported.
    HCI_READ_LOCAL_SUPPORTED_CODEC_CAPABILITIES_COMMAND, // The HCI_Read_Local_Supported_Codec_Capabilities command is used by a Host to query codec capabilities.
    HCI_READ_LOCAL_SUPPORTED_CODECS_COMMAND, // The HCI_Read_Local_Supported_Codecs command is used by a Host to query a Controller's supported codecs.
    HCI_READ_LOCAL_SUPPORTED_COMMANDS_COMMAND, // The HCI_Read_Local_Supported_Commands command requests a list of the supported HCI commands for the local device.
    HCI_READ_LOCAL_SUPPORTED_CONTROLLER_DELAY_COMMAND, // The HCI_Read_Local_Supported_Controller_Delay command is used by a Host to query a range of supported Controller delays for a given codec configuration.
    HCI_READ_LOCAL_SUPPORTED_FEATURES_COMMAND, // The HCI_Read_Local_Supported_Features command requests a list of the supported features for the local device.
    HCI_READ_LOCAL_VERSION_INFORMATION_COMMAND, // The HCI_Read_Local_Version_Information command will read the version information for the local Controller.
    HCI_READ_LOOPBACK_MODE_COMMAND, // The HCI_Read_Loopback_Mode command will read the value for the setting of the BR/EDR Controller's Loopback Mode. The setting of the Loopback Mode will determine the path of information.
    HCI_READ_NUM_BROADCAST_RETRANSMISSIONS_COMMAND, // The HCI_Read_Num_Broadcast_Retransmissions command will read the parameter value for the Number of Broadcast Retransmissions for the BR/EDR Controller.
    Read_Number_Of_Supported_IAC_command, // The HCI_Read_Number_Of_Supported_IAC command will read the value for the number of Inquiry Access Codes (IAC) that the local BR/EDR Controller can simultaneously listen for during an Inquiry Scan.
    HCI_READ_PAGE_SCAN_ACTIVITY_COMMAND, // The HCI_Read_Page_Scan_Activity command will read the values for the Page Scan Interval and Page Scan Window configuration parameters. Page Scan Interval defines the amount of time between consecutive page scans. Page Scan Window defines the duration of the page scan.
    HCI_READ_PAGE_SCAN_TYPE_COMMAND, // The HCI_Read_Page_Scan_Type command is used to read the page scan type of the local BR/EDR Controller. The Page Scan Type configuration parameter can set the page scan to either normal or interlaced scan.
    HCI_READ_PAGE_TIMEOUT_COMMAND, // The HCI_Read_Page_Timeout command will read the value for the Page Reply Timeout configuration parameter, which determines the time the BR/EDR Controller will wait for the remote device to respond to a connection request before the local device returns a connection failure.
    HCI_READ_PIN_TYPE_COMMAND, // The HCI_Read_PIN_Type command is used for the Host to read the value that is specified to indicate whether the Host supports variable PINs or only fixed PINs.
    HCI_READ_REMOTE_EXTENDED_FEATURES_COMMAND, // The HCI_Read_Remote_Extended_Features command requests a list of the supported extended features of a remote device.
    HCI_READ_REMOTE_EXTENDED_FEATURES_COMPLETE_EVENT, // The HCI_Read_Remote_Extended_Features_Complete event is used to indicate the completion of the process of the Link Manager obtaining the supported Extended features of the remote BR/EDR Controller specified by the Connection_Handle parameter.
    HCI_READ_REMOTE_SUPPORTED_FEATURES_COMMAND, // The HCI_Read_Remote_Supported_Features command requests a list of the supported features of a remote device.
    HCI_READ_REMOTE_SUPPORTED_FEATURES_COMPLETE_EVENT, // The HCI_Read_Remote_Supported_Features_Complete event is used to indicate the completion of the process of the Link Manager obtaining the supported features of the remote BR/EDR Controller specified by the Connection_Handle parameter.
    HCI_READ_REMOTE_VERSION_INFORMATION_COMMAND, // The HCI_Read_Remote_Version_Information command will read the values for the version information for the remote device associated with the Connection_Handle.
    HCI_READ_REMOTE_VERSION_INFORMATION_COMPLETE_EVENT, // The HCI_Read_Remote_Version_Information_Complete event is used to indicate the completion of the process of the Link Manager obtaining the version information of the remote device associated with the Connection_Handle parameter.
    HCI_READ_RSSI_COMMAND, // The HCI_Read_RSSI command will read the value for the Received Signal Strength Indication (RSSI) for a Connection_Handle to another Controller.
    HCI_READ_SCAN_ENABLE_COMMAND, // The HCI_Read_Scan_Enable command will read the value for the Scan Enable configuration parameter, which controls whether or not the BR/EDR Controller will periodically scan for page attempts and/or inquiry requests from other BR/EDR Controllers.
    HCI_READ_SECURE_CONNECTIONS_HOST_SUPPORT_COMMAND, // The HCI_Read_Secure_Connections_Host_Support command is used to read the Secure Connections Host Supports parameter from the Controller.
    HCI_READ_SIMPLE_PAIRING_MODE_COMMAND, // The HCI_Read_Simple_Pairing_Mode command reads the Secure Simple Pairing mode setting in the BR/EDR Controller.
    HCI_READ_STORED_LINK_KEY_COMMAND, // The HCI_Read_Stored_Link_Key command provides the ability to read whether one or more link keys are stored in the Controller.
    HCI_READ_SYNCHRONIZATION_TRAIN_PARAMETERS_COMMAND, // The HCI_Read_Synchronization_Train_Parameters command returns the currently configured values for the Synchronization Train functionality in the BR/EDR Controller.
    HCI_READ_SYNCHRONOUS_FLOW_CONTROL_ENABLE_COMMAND, // The HCI_Read_Synchronous_Flow_Control_Enable command provides the ability to read the Synchronous Flow Control Enable setting. By using this setting, the Host can decide if the Controller will send HCI_Number_Of_Completed_Packets events for synchronous Connection_Handles.
    HCI_READ_TRANSMIT_POWER_LEVEL_COMMAND, // The HCI_Read_Transmit_Power_Level command will read the values for the Transmit Power Level parameter for the specified Connection_Handle.
    HCI_READ_VOICE_SETTING_COMMAND, // The HCI_Read_Voice_Setting command will read the values for the Voice Setting configuration parameter, which controls all the various settings for the voice connections.
    HCI_RECEIVE_SYNCHRONIZATION_TRAIN_COMMAND, // The HCI_Receive_Synchronization_Train command requests synchronization with the specified Connectionless Peripheral Broadcast transmitter.
    HCI_REFRESH_ENCRYPTION_KEY_COMMAND, // The HCI_Refresh_Encryption_Key command is used by the Host to cause the Controller to refresh the encryption key by pausing and resuming encryption.
    HCI_REJECT_CONNECTION_REQUEST_COMMAND, // The HCI_Reject_Connection_Request command is used to decline a new incoming BR/EDR connection request.
    HCI_REJECT_SYNCHRONOUS_CONNECTION_REQUEST_COMMAND, // The HCI_Reject_Synchronous_Connection_Request command is used to decline an incoming request for a synchronous link.
    HCI_REMOTE_HOST_SUPPORTED_FEATURES_NOTIFICATION_EVENT, // The HCI_Remote_Host_Supported_Features_Notification event is used to return the LMP extended features page containing the Host features.
    HCI_REMOTE_NAME_REQUEST_CANCEL_COMMAND, // The HCI_Remote_Name_Request_Cancel command is used to cancel an ongoing Remote Name Request.
    HCI_REMOTE_NAME_REQUEST_COMMAND, // The HCI_Remote_Name_Request command is used to obtain the user-friendly name of another BR/EDR Controller.
    HCI_REMOTE_NAME_REQUEST_COMPLETE_EVENT, // The HCI_Remote_Name_Request_Complete event is used to indicate a remote name request has been completed.
    HCI_REMOTE_OOB_DATA_REQUEST_EVENT, // The HCI_Remote_OOB_Data_Request event is used to indicate that the Secure Simple Pairing Hash C and Randomizer R is required for the Secure Simple Pairing process involving the device identified by BD_ADDR.
    HCI_REMOTE_OOB_DATA_REQUEST_NEGATIVE_REPLY_COMMAND, // The HCI_Remote_OOB_Data_Request_Negative_Reply command is used to reply to an HCI_Remote_OOB_Data_Request event that the Host does not have the C and R.
    HCI_REMOTE_OOB_DATA_REQUEST_REPLY_COMMAND, // The HCI_Remote_OOB_Data_Request_Reply command is used to reply to an HCI_Remote_OOB_Data_Request event with the C and R values received via an OOB transfer from a remote BR/EDR Controller identified by BD_ADDR.
    HCI_REMOTE_OOB_EXTENDED_DATA_REQUEST_REPLY_COMMAND, // The HCI_Remote_OOB_Extended_Data_Request_Reply command is used to reply to an HCI_Remote_OOB_Data_Request event with the C and R values received via an OOB transfer from a remote BR/EDR Controller identified by the BD_ADDR.
    HCI_RESET_COMMAND, // For a BR/EDR Controller, the HCI_Reset command resets HCI, the Link Manager, and the Bluetooth radio. For an LE Controller, the HCI_Reset command resets HCI, the Link Layer, and LE PHY.
    HCI_RESET_FAILED_CONTACT_COUNTER_COMMAND, // The HCI_Reset_Failed_Contact_Counter command will reset the value for the Failed Contact Counter configuration parameter for a particular connection to another device.
    HCI_RETURN_LINK_KEYS_EVENT, // The HCI_Return_Link_Keys event is used to return stored link keys after an HCI_Read_Stored_Link_Key command is used.
    HCI_ROLE_CHANGE_EVENT, // The HCI_Role_Change event is used to indicate that the current BR/EDR Controller role related to the particular connection has been changed.
    HCI_ROLE_DISCOVERY_COMMAND, // The HCI_Role_Discovery command is used for a BR/EDR Controller to determine which role the device is performing for a particular Connection_Handle.
    HCI_SAM_STATUS_CHANGE_EVENT, // The HCI_SAM_Status_Change event is used to indicate that either the local or remote SAM slot map on a particular connection has been changed.
    HCI_SEND_KEYPRESS_NOTIFICATION_COMMAND, // The HCI_Send_Keypress_Notification command is used during the Passkey Entry protocol by a device with KeyboardOnly IO capabilities. It is used by a Host to inform the remote device when keys have been entered or erased.
    HCI_SET_AFH_HOST_CHANNEL_CLASSIFICATION_COMMAND, // The HCI_Set_AFH_Host_Channel_Classification command allows the Host to specify a channel classification based on its "local information".
    HCI_SET_CONNECTION_ENCRYPTION_COMMAND, // The HCI_Set_Connection_Encryption command is used to enable and disable the link level encryption.
    HCI_SET_CONNECTIONLESS_PERIPHERAL_BROADCAST_COMMAND, // The HCI_Set_Connectionless_Peripheral_Broadcast command controls Connectionless Peripheral Broadcast functionality (for transmission) in the BR/EDR Controller including enabling and disabling the broadcast.
    HCI_SET_CONNECTIONLESS_PERIPHERAL_BROADCAST_DATA_COMMAND, // The HCI_Set_Connectionless_Peripheral_Broadcast_Data command is used by the Host to set Connectionless Peripheral Broadcast data in the BR/EDR Controller.
    HCI_SET_CONNECTIONLESS_PERIPHERAL_BROADCAST_RECEIVE_COMMAND, // The HCI_Set_Connectionless_Peripheral_Broadcast_Receive command enables and disables Connectionless Peripheral Broadcast reception in the BR/EDR Controller.
    HCI_SET_CONTROLLER_TO_HOST_FLOW_CONTROL_COMMAND, // The HCI_Set_Controller_To_Host_Flow_Control command is used by the Host to turn flow control on or off in the direction from the Controller to the Host.
    HCI_SET_ECOSYSTEM_BASE_INTERVAL_COMMAND, // The HCI_Set_Ecosystem_Base_Interval command indicates to the Controller the base interval of the ecosystem.
    HCI_SET_EVENT_FILTER_COMMAND, // The HCI_Set_Event_Filter command is used by the Host to specify different event filters. The Host may issue this command multiple times to request various conditions for the same type of event filter and for different types of event filters.
    HCI_SET_EVENT_MASK_COMMAND, // The HCI_Set_Event_Mask command is used to control which events are generated by the HCI for the Host.
    HCI_SET_EVENT_MASK_PAGE_2_COMMAND, // The HCI_Set_Event_Mask_Page 2 command is used to control which events are generated by the HCI for the Host.
    Set_External_Frame_Configuration_command, // The HCI_Set_External_Frame_Configuration command enables an external device to describe a frame structure to the Controller.
    Set_Min_Encryption_Key_Size_command, // The HCI_Set_Min_Encryption_Key_Size command is used to modify the minimum encryption key size that may be negotiated by the Controller.
    Set_MWS_Channel_Parameters_command, // The HCI_Set_MWS_Channel_Parameters command enables an MWS device to inform the Controller about the MWS channel configuration.
    Set_MWS_Scan_Frequency_Table_command, // The HCI_Set_MWS_Scan_Frequency_Table command specifies the frequencies represented by the frequency index supplied by the MWS_SCAN_FREQUENCY signal.
    Set_MWS_Signaling_command, // The HCI_Set_MWS_Signaling command enables an MWS device to inform the Controller about the timing parameters for the MWS coexistence interface.
    Set_MWS_Transport_Layer_command, // The HCI_Set_MWS_Transport_Layer command selects the MWS coexistence signaling transport layer in the Controller.
    Set_MWS_PATTERN_Configuration_command, // The HCI_Set_MWS_PATTERN_Configuration command specifies the configuration of the pattern indicated over the MWS Coexistence Transport Layer.
    Set_Reserved_LT_ADDR_command, // The HCI_Set_Reserved_LT_ADDR command requests that the BR/EDR Controller reserve a specific LT_ADDR for the purposes of Connectionless Peripheral Broadcast.
    Set_Triggered_Clock_Capture_command, // The HCI_Set_Triggered_Clock_Capture command is used to configure the Controller to return events containing an estimate of a piconet or the local Bluetooth clock.
    Setup_Synchronous_Connection_command, // The HCI_Setup_Synchronous_Connection command adds a new or modifies an existing synchronous logical transport (SCO or eSCO) on a physical link depending on the Connection_Handle parameter specified.
    Simple_Pairing_Complete_event, // The HCI_Simple_Pairing_Complete event is used to indicate that the Secure Simple Pairing process has completed.
    Sniff_Mode_command, // The HCI_Sniff_Mode command is used to alter the behavior of the LM and have the LM place the local or remote device into Sniff mode.
    Sniff_Subrating_command, // The HCI_Sniff_Subrating command is used to configure the sniff subrating parameters in the local device.
    Sniff_Subrating_event, // The HCI_Sniff_Subrating event is used to inform the Host of the local and remote transmit and receive latencies.
    Start_Synchronization_Train_command, // The HCI_Start_Synchronization_Train command enables the Synchronization Train on the BR/EDR Controller using the currently configured Synchronization Train parameters.
    Switch_Role_command, // The HCI_Switch_Role command is used to switch Central and Peripheral roles of the devices on either side of a connection.
    Synchronization_Train_Complete_event, // The HCI_Synchronization_Train_Complete event indicates that the Synchronization Train has completed.
    Synchronization_Train_Received_event, // The HCI_Synchronization_Train_Received event provides the status of Synchronization Train packets received from the device with the given BD_ADDR.
    Synchronous_Connection_Changed_event, // The HCI_Synchronous_Connection_Changed event indicates to the Host that an existing synchronous connection has been reconfigured.
    Synchronous_Connection_Complete_event, // The HCI_Synchronous_Connection_Complete event indicates to both the Hosts that a new synchronous connection has been established.
    Triggered_Clock_Capture_event, // The HCI_Triggered_Clock_Capture event reports the Bluetooth clock when an external trigger occurred.
    Truncated_Page_Cancel_command, // The HCI_Truncated_Page_Cancel command is used to cancel an ongoing Truncated Page.
    Truncated_Page_command, // The HCI_Truncated_Page command will cause the BR/EDR Controller to page the BR/EDR Controller with the BD_ADDR specified by the parameters and abort the page sequence after receiving the ID response packet.
    Truncated_Page_Complete_event, // The HCI_Truncated_Page_Complete event indicates to the Host that a Truncated Page has completed.
    User_Confirmation_Request_event, // The HCI_User_Confirmation_Request event is used to indicate that user confirmation of a numeric value is required.
    User_Confirmation_Request_Negative_Reply_command, // The HCI_User_Confirmation_Request_Negative_Reply command is used to reply to an HCI_User_Confirmation_Request event and indicates that the user selected "no". This command will terminate Secure Simple Pairing.
    User_Confirmation_Request_Reply_command, // The HCI_User_Confirmation_Request_Reply command is used to reply to an HCI_User_Confirmation_Request event and indicates that the user selected "yes". It is also used when the Host has no input and no output capabilities.
    User_Passkey_Notification_event, // The HCI_User_Passkey_Notification event is used to provide a passkey for the Host to display to the user as required as part of a Secure Simple Pairing process.
    User_Passkey_Request_event, // The HCI_User_Passkey_Request event is used to indicate that a passkey is required as part of a Secure Simple Pairing process.
    User_Passkey_Request_Negative_Reply_command, // The HCI_User_Passkey_Request_Negative_Reply command is used to reply to an HCI_User_Passkey_Request event and indicates the Host could not provide a passkey. This command will terminate Secure Simple Pairing.
    User_Passkey_Request_Reply_command, // The HCI_User_Passkey_Request_Reply command is used to reply to an HCI_User_Passkey_Request event and specifies the Numeric_Value (passkey) entered by the user to be used in the Secure Simple Pairing process.
    Write_AFH_Channel_Assessment_Mode_command, // The HCI_Write_AFH_Channel_Assessment_Mode command will write the value for the Channel Classification Mode configuration parameter. This value is used to enable or disable the Controller's channel assessment scheme.
    Write_Authenticated_Payload_Timeout_command, // The HCI_Write_Authenticated_Payload_Timeout command is used to write the Authenticated Payload Timeout parameter, which is used to set the maximum time between packets being received from the remote device without a valid MIC.
    Write_Authentication_Enable_command, // The HCI_Write_Authentication_Enable command will write the value for the Authentication Enable parameter, which controls whether the Bluetooth device will require authentication for each connection with other Bluetooth devices.
    Write_Automatic_Flush_Timeout_command, // The HCI_Write_Automatic_Flush_Timeout command will write the value for the Flush Timeout configuration parameter for the specified Connection_Handle. The Flush Timeout parameter is only used for ACL connections.
    Write_Class_of_Device_command, // The HCI_Write_Class_of_Device command will write the value for the Class_of_Device configuration parameter, which is used to indicate its capabilities to other devices.
    Write_Connection_Accept_Timeout_command, // The HCI_Write_Connection_Accept_Timeout command will write the value for the Connection Accept Timeout configuration parameter, which allows the Controller to automatically deny a connection request after a specified period has occurred, and to refuse a new connection.
    Write_Current_IAC_LAP_command, // The HCI_Write_Current_IAC_LAP command will write the LAP(s) used to create the Inquiry Access Codes (IAC) that the local BR/EDR Controller is simultaneously scanning for during Inquiry Scans.
    Write_Default_Erroneous_Data_Reporting_command, // The HCI_Write_Default_Erroneous_Data_Reporting command will write the value for the Erroneous Data Reporting configuration parameter, which controls whether the Bluetooth Controller will provide data for every (e)SCO interval, with the Packet_Status_Flag in HCI Synchronous Data packets set according to HCI Synchronous Data packets.
    Write_Default_Link_Policy_Settings_command, // The HCI_Write_Default_Link_Policy_Settings command will write the Default Link Policy configuration parameter for all new connections.
    Write_Extended_Inquiry_Length_command, // The HCI_Write_Extended_Inquiry_Length command is used to write the Extended Inquiry Length parameter to the Controller.
    Write_Extended_Inquiry_Response_command, // The HCI_Write_Extended_Inquiry_Response command will write the data that the BR/EDR Controller sends in the extended inquiry response packet during inquiry response.
    Write_Extended_Page_Timeout_command, // The HCI_Write_Extended_Page_Timeout command is used to write the Extended Page Timeout parameter to the Controller.
    Write_Flow_Control_Mode_command, // The HCI_Write_Flow_Control_Mode command sets the value of the Flow_Control_Mode configuration parameter for this Controller.
    Write_Hold_Mode_Activity_command, // The HCI_Write_Hold_Mode_Activity command is used to write which activities should be suspended when the BR/EDR Controller is in Hold mode.
    Write_Inquiry_Mode_command, // The HCI_Write_Inquiry_Mode command is used to write the Inquiry Mode configuration parameter of the local BR/EDR Controller.
    Write_Inquiry_Scan_Activity_command, // The HCI_Write_Inquiry_Scan_Activity command will write the value for Inquiry Scan Interval and Inquiry Scan Window configuration parameters. Inquiry Scan Interval defines the amount of time between consecutive inquiry scans. Inquiry Scan Window defines the amount of time for the duration of the inquiry scan.
    Write_Inquiry_Scan_Type_command, // The HCI_Write_Inquiry_Scan_Type command is used to write the Inquiry Scan Type configuration parameter of the local BR/EDR Controller. The Inquiry Scan Type configuration parameter can set the inquiry scan to either normal or interlaced scan.
    Write_Inquiry_Transmit_Power_Level_command, // The HCI_Write_Inquiry_Transmit_Power_Level command is used to write the transmit power level used to transmit the inquiry (ID) data packets.
    Write_LE_Host_Support_command, // The HCI_Write_LE_Host_Support command writes the LE Supported Host setting to the BR/EDR Controller.
    Write_Link_Policy_Settings_command, // The HCI_Write_Link_Policy_Settings command will write the Link Policy configuration parameter for the specified Connection_Handle. The Link Policy settings allow the Host to specify which Link Modes the Link Manager can use for the specified Connection_Handle.
    Write_Link_Supervision_Timeout_command, // The HCI_Write_Link_Supervision_Timeout command will write the value for the Link Supervision Timeout configuration parameter for the device. This parameter is used by the Controller to determine link loss.
    Write_Local_Name_command, // The HCI_Write_Local_Name command provides the ability to modify the user-friendly name for the BR/EDR Controller.
    Write_Loopback_Mode_command, // The HCI_Write_Loopback_Mode command will write the value for the setting of the BR/EDR Controllers Loopback Mode. The setting of the Loopback Mode will determine the path of information.
    Write_Num_Broadcast_Retransmissions_command, // The HCI_Write_Num_Broadcast_Retransmissions command will write the parameter value for the Number of Broadcast Retransmissions for the BR/EDR Controller.
    Write_Page_Scan_Activity_command, // The HCI_Write_Page_Scan_Activity command will write the value for Page Scan Interval and Page Scan Window configuration parameters. Page Scan Interval defines the amount of time between consecutive page scans. Page Scan Window defines the duration of the page scan.
    Write_Page_Scan_Type_command, // The HCI_Write_Page_Scan_Type command is used to write the page scan type of the local BR/EDR Controller. The Page Scan Type configuration parameter can set the page scan to either normal or interlaced scan.
    Write_Page_Timeout_command, // The HCI_Write_Page_Timeout command will write the value for the Page Reply Timeout configuration parameter, which allows the BR/EDR Controller to define the amount of time a connection request will wait for the remote device to respond before the local device returns a connection failure.
    Write_PIN_Type_command, // The HCI_Write_PIN_Type command is used for the Host to specify whether the Host supports variable PIN or only fixed PINs.
    Write_Scan_Enable_command, // The HCI_Write_Scan_Enable command will write the value for the Scan Enable configuration parameter, which controls whether or not the BR/EDR Controller will periodically scan for page attempts and/or inquiry requests from other BR/EDR Controllers.
    Write_Secure_Connections_Host_Support_command, // The HCI_Write_Secure_Connections_Host_Support command is used to write the Secure Connections Host Supports parameter to the Controller.
    Write_Secure_Connections_Test_Mode_command, // The HCI_Write_Secure_Connections_Test_Mode command is used to put the Controller in a test mode where DM1 packets are not allowed to be used for ACL-U traffic and/or the contents of eSCO payloads can be looped back.
    Write_Simple_Pairing_Debug_Mode_command, // The HCI_Write_Simple_Pairing_Debug_Mode command configures the BR/EDR Controller to use a predefined Diffie Hellman private key for Secure Simple Pairing to enable debug equipment to monitor the encrypted connection.
    Write_Simple_Pairing_Mode_command, // The HCI_Write_Simple_Pairing_Mode command writes the Secure Simple Pairing mode setting in the BR/EDR Controller.
    Write_Stored_Link_Key_command, // The HCI_Write_Stored_Link_Key command provides the ability to write one or more link keys to be stored in the Controller.
    Write_Synchronization_Train_Parameters_command, // The HCI_Write_Synchronization_Train_Parameters command configures the Synchronization Train functionality in the BR/EDR Controller.
    Write_Synchronous_Flow_Control_Enable_command, // The HCI_Write_Synchronous_Flow_Control_Enable command provides the ability to write the Synchronous Flow Control Enable setting. By using this setting, the Host can decide if the Controller will send HCI_Number_Of_Completed_Packets events for synchronous Connection_Handles.
    Write_Voice_Setting_command // The HCI_Write_Voice_Setting command will write the values for the Voice Setting configuration parameter, which controls all the various settings for the voice connections.
}bt_command_event_e;

typedef enum
{
    v1_1,
    v1_2,
    v4_0,
    v4_1,
    v4_2,
    v5_0,
    v5_1,
    v5_2,
    v5_3,
    v5_4,
    v6_0,
    CSA2,
    CSA3,
    CSA4,
    v2_1_EDR,
    v3_0_HS,
}bt_version_e;

typedef struct 
{
    bt_command_event_e name;
    bt_version_e       version;
    const _u8*         BR_EDR;
    const _u8*         LE;
}bt_ov_cmd_evts_t;

bt_ov_cmd_evts_t bt_commands_events_overview[] = 
{
    /**name**/                                                                /**version**/  /**BR/EDR**/        /**LE**/
    /****************************************************BT VERSION 1.1*************************************************************/
    {HCI_RESET_COMMAND,                                                           v1_1,          "M",                "M"},
    {HCI_COMMAND_COMPLETE_EVENT,                                              v1_1,          "M",                "M"},
    {HCI_COMMAND_STATUS_EVENT,                                                v1_1,          "M",                "M"},
    {HCI_READ_BD_ADDR_COMMAND,                                                    v1_1,          "M",                "M"},
    {HCI_READ_LOCAL_SUPPORTED_FEATURES_COMMAND,                                   v1_1,          "M",                "M"},
    {HCI_READ_LOCAL_VERSION_INFORMATION_COMMAND,                                  v1_1,          "M",                "M"},
    {HCI_READ_LOCAL_SUPPORTED_COMMANDS_COMMAND,                                   v1_2,          "M",                "M"},
    {HCI_SET_EVENT_MASK_COMMAND,                                                  v1_1,          "M",                "M"},
    {HCI_DISCONNECT_COMMAND,                                                  v1_1,          "M",                "C.3"}, 
    {HCI_DISCONNECTION_COMPLETE_EVENT,                                        v1_1,          "M",                "C.3"}, 
    {HCI_NUMBER_OF_COMPLETED_PACKETS_EVENT,                                       v1_1,          "M",                "C.3"}, 
    {HCI_READ_REMOTE_VERSION_INFORMATION_COMMAND,                                 v1_1,          "O",                "C.3"}, 
    {HCI_READ_RSSI_COMMAND,                                                       v1_1,          "O",                "C.3"}, 
    // C.3: Mandatory if the LE Controller supports Connection State, otherwise excluded
    {HCI_READ_REMOTE_VERSION_INFORMATION_COMPLETE_EVENT,                          v1_1,          "C.105",            "C.3"}, 
    // C.105: Mandatory if the Read Remote Version Information command is supported; 
    {HCI_READ_TRANSMIT_POWER_LEVEL_COMMAND,                                       v1_1,          "C.152",            "C.3"}, 
    // C.152: Mandatory if Power Control is supported; C.3: Mandatory if the LE Controller supports Connection State
    {HCI_DATA_BUFFER_OVERFLOW_EVENT,                                          v1_1,          "O",                "O"},
    {HCI_HARDWARE_ERROR_EVENT,                                                    v1_1,          "O",                "O"},
    {HCI_HOST_BUFFER_SIZE_COMMAND,                                                v1_1,          "C.107",            "C.107"}, 
    {HCI_HOST_NUMBER_OF_COMPLETED_PACKETS_COMMAND,                                v1_1,          "C.107",            "C.107"}, 
    // C.107: Mandatory if the Set Controller To Host Flow Control command is supported, otherwise excluded
    {HCI_READ_CONNECTION_ACCEPT_TIMEOUT_COMMAND,                                  v1_1,          "M",                "C.40"}, 
    {Write_Connection_Accept_Timeout_command,                                 v1_1,          "M",                "C.40"}, 
    // C.40: Mandatory if LE Feature (Connected Isochronous Stream - Peripheral) is supported, otherwise excluded
    {HCI_SET_CONTROLLER_TO_HOST_FLOW_CONTROL_COMMAND,                             v1_1,          "O",                "C.96"}, 
    // C.96: Optional if the LE Controller supports Connection State, otherwise excluded
    {HCI_ENCRYPTION_CHANGE_EVENT,                                             v1_1,          "[v1] M [v2] C.158", "[v1] C.4 [v2] C.56"}, 
    // C.158:Mandatory if the Set Min Encryption Key Size command is supported,otherwise optional.
    // C.4: Mandatory if LE Feature (LE Encryption) is supported;
    // C.56: Mandatory if LE Feature (LL Extended Feature Set) is supported
    {HCI_ACCEPT_CONNECTION_REQUEST_COMMAND,                                   v1_1,          "M",                "E"},
    {HCI_AUTHENTICATION_COMPLETE_EVENT,                                       v1_1,          "C.101",            "E"}, 
    // C.101: Mandatory if the Authentication Requested command is supported, otherwise excluded
    {HCI_AUTHENTICATION_REQUESTED_COMMAND,                                    v1_1,          "O",                "E"},
    {HCI_CHANGE_CONNECTION_LINK_KEY_COMMAND,                                  v1_1,          "O",                "E"},
    {HCI_CHANGE_CONNECTION_LINK_KEY_COMPLETE_EVENT,                           v1_1,          "C.102",            "E"}, 
    // C.102: Mandatory if the Change Connection Link Key command is supported, otherwise excluded
    {HCI_CHANGE_CONNECTION_PACKET_TYPE_COMMAND,                               v1_1,          "C.133",            "E"}, 
    // C.133: Mandatory if HV2, HV3, or multi-slot or EDR ACL packets are supported, otherwise excluded
    {HCI_CONNECTION_COMPLETE_EVENT,                                           v1_1,          "M",                "E"},
    {HCI_CONNECTION_PACKET_TYPE_CHANGED_EVENT,                                v1_1,          "C.133",            "E"},
     // C.133: Mandatory if HV2, HV3, or multi-slot or EDR ACL packets are supported, otherwise excluded
    {HCI_CONNECTION_REQUEST_EVENT,                                            v1_1,          "M",                "E"},
    {HCI_CREATE_CONNECTION_COMMAND,                                           v1_1,          "M",                "E"},
    {HCI_DELETE_STORED_LINK_KEY_COMMAND,                                      v1_1,          "C.121",            "E"}, 
    // C.121: Mandatory if the Write Stored Link Key command is supported, otherwise excluded
    {HCI_ENABLE_IMPLEMENTATION_UNDER_TEST_MODE_COMMAND,                       v1_1,          "C.123",            "E"}, 
    // C.123: Mandatory if BR/EDR test mode is supported, otherwise excluded
    {HCI_EXIT_PERIODIC_INQUIRY_MODE_COMMAND,                                      v1_1,          "C.103",            "E"},
    // C.103: Mandatory if the Periodic Inquiry Mode command is supported, otherwise excluded
    {HCI_EXIT_SNIFF_MODE_COMMAND,                                                 v1_1,          "C.214",            "E"}, 
    // C.214: Mandatory if Sniff mode is supported, otherwise excluded
    {HCI_FLUSH_COMMAND,                                                           v1_1,          "M",                "E"},
    {HCI_FLUSH_OCCURRED_EVENT,                                                    v1_1,          "M",                "E"},
    {HCI_HOLD_MODE_COMMAND,                                                       v1_1,          "C.213",            "E"}, 
    // C.213: Mandatory if Hold mode is supported, otherwise excluded
    {HCI_INQUIRY_CANCEL_COMMAND,                                                  v1_1,          "C.127",            "E"}, 
    // C.127: Mandatory if Inquiry is supported, otherwise excluded
    {HCI_INQUIRY_COMMAND,                                                         v1_1,          "C.127",            "E"}, 
    // C.127: Mandatory if Inquiry is supported, otherwise excluded
    {HCI_INQUIRY_COMPLETE_EVENT,                                                  v1_1,          "C.127",            "E"}, 
    // C.127: Mandatory if Inquiry is supported, otherwise excluded
    {HCI_INQUIRY_RESULT_EVENT,                                                    v1_1,          "C.127",            "E"}, 
    // C.127: Mandatory if Inquiry is supported, otherwise excluded
    {HCI_LINK_KEY_NOTIFICATION_EVENT,                                             v1_1,          "M",                "E"},
    {HCI_LINK_KEY_REQUEST_EVENT,                                                  v1_1,          "M",                "E"},
    {HCI_LINK_KEY_REQUEST_NEGATIVE_REPLY_COMMAND,                                 v1_1,          "M",                "E"},
    {HCI_LINK_KEY_REQUEST_REPLY_COMMAND,                                          v1_1,          "M",                "E"},
    {HCI_LINK_KEY_SELECTION_COMMAND,                                              v1_1,          "C.215",            "E"}, 
    // C.215: Mandatory if Broadcast Encryption is supported, otherwise excluded
    {HCI_LINK_KEY_TYPE_CHANGED_EVENT,                                             v1_1,          "C.215",            "E"}, 
    // C.215: Mandatory if Broadcast Encryption is supported, otherwise excluded
    {HCI_LOOPBACK_COMMAND_EVENT,                                                  v1_1,          "C.123",            "E"}, 
    // C.123: Mandatory if BR/EDR test mode is supported, otherwise excluded
    {HCI_MAX_SLOTS_CHANGE_EVENT,                                                  v1_1,          "C.132",            "E"}, 
    // C.132: Mandatory if multi-slot ACL packets are supported, otherwise excluded
    {HCI_MODE_CHANGE_EVENT,                                                       v1_1,          "C.144",            "E"}, 
    // C.144: Mandatory if Hold Mode or Sniff Mode is supported, otherwise excluded
    {HCI_PAGE_SCAN_REPETITION_MODE_CHANGE_EVENT,                                  v1_1,          "O",                "E"},
    {HCI_PERIODIC_INQUIRY_MODE_COMMAND,                                           v1_1,          "C.128",            "E"}, 
    // C.128: Optional if Inquiry is supported, otherwise excluded
    {HCI_READ_AUTHENTICATION_ENABLE_COMMAND,                                      v1_1,          "C.111",            "E"}, 
    // C.111: Mandatory if the Write Authentication Enable command is supported, otherwise excluded
    {HCI_READ_AUTOMATIC_FLUSH_TIMEOUT_COMMAND,                                    v1_1,          "M",                "E"},
    {HCI_READ_BUFFER_SIZE_COMMAND,                                                v1_1,          "M",                "E"},
    {HCI_READ_CLASS_OF_DEVICE_COMMAND,                                            v1_1,          "M",                "E"},
    {HCI_READ_CLOCK_OFFSET_COMMAND,                                               v1_1,          "O",                "E"},
    {HCI_READ_CLOCK_OFFSET_COMPLETE_EVENT,                                        v1_1,          "C.104",            "E"}, 
    // C.104: Mandatory if the Read Clock Offset command is supported, otherwise excluded
    {HCI_READ_CURRENT_IAC_LAP_COMMAND,                                            v1_1,          "C.125",            "E"}, 
    // C.125: Mandatory if Inquiry Scan is supported, otherwise excluded
    {HCI_PIN_CODE_REQUEST_EVENT,                                                  v1_1,          "M",                "E"},
    {HCI_PIN_CODE_REQUEST_NEGATIVE_REPLY_COMMAND,                                 v1_1,          "M",                "E"},
    {HCI_PIN_CODE_REQUEST_REPLY_COMMAND,                                          v1_1,          "M",                "E"},
    {HCI_QOS_SETUP_COMMAND,                                                       v1_1,          "M",                "E"},
    {HCI_QOS_SETUP_COMPLETE_EVENT,                                                v1_1,          "M",                "E"},
    {HCI_QOS_VIOLATION_EVENT,                                                     v1_1,          "M",                "E"},
    {HCI_READ_FAILED_CONTACT_COUNTER_COMMAND,                                     v1_1,          "M",                "E"},
    {HCI_READ_HOLD_MODE_ACTIVITY_COMMAND,                                         v1_1,          "C.213",            "E"}, 
    // C.213: Mandatory if Hold mode is supported, otherwise excluded
    {HCI_READ_INQUIRY_SCAN_ACTIVITY_COMMAND,                                      v1_1,          "C.125",            "E"}, 
    // C.125: Mandatory if Inquiry Scan is supported, otherwise excluded
    {HCI_READ_LINK_POLICY_SETTINGS_COMMAND,                                       v1_1,          "C.141",            "E"}, 
    // C.141: Mandatory if Role Switch, Hold mode, or Sniff mode is supported, otherwise excluded
    {HCI_READ_LINK_QUALITY_COMMAND,                                               v1_1,          "O",                "E"},
    {HCI_READ_LINK_SUPERVISION_TIMEOUT_COMMAND,                                   v1_1,          "C.117",            "E"}, 
    // C.117: Mandatory if the Write Link Supervision Timeout command is supported, otherwise excluded
    {HCI_READ_LOCAL_NAME_COMMAND,                                                 v1_1,          "M",                "E"},
    {HCI_READ_LOOPBACK_MODE_COMMAND,                                              v1_1,          "C.123",            "E"}, 
    // C.123: Mandatory if BR/EDR test mode is supported, otherwise excluded
    {HCI_READ_NUM_BROADCAST_RETRANSMISSIONS_COMMAND,                              v1_1,          "C.118",            "E"}, 
    // C.118: Mandatory if the Write Num Broadcast Retransmissions command is supported, otherwise excluded
    {Read_Number_Of_Supported_IAC_command,                                    v1_1,          "C.125",            "E"}, 
    // C.125: Mandatory if Inquiry Scan is supported, otherwise excluded
    {HCI_READ_PAGE_SCAN_ACTIVITY_COMMAND,                                         v1_1,          "M",                "E"},
    {HCI_READ_PAGE_TIMEOUT_COMMAND,                                               v1_1,          "M",                "E"},
    {HCI_READ_PIN_TYPE_COMMAND,                                                   v1_1,          "C.120",            "E"}, 
    // C.120: Mandatory if the Write PIN Type command is supported, otherwise excluded
    {HCI_READ_REMOTE_SUPPORTED_FEATURES_COMMAND,                                  v1_1,          "M",                "E"},
    {HCI_READ_REMOTE_SUPPORTED_FEATURES_COMPLETE_EVENT,                           v1_1,          "M",                "E"},
    {HCI_READ_SCAN_ENABLE_COMMAND,                                                v1_1,          "M",                "E"},
    {HCI_READ_STORED_LINK_KEY_COMMAND,                                            v1_1,          "C.121",            "E"}, 
    // C.121: Mandatory if the Write Stored Link Key command is supported, otherwise excluded
    {HCI_REJECT_CONNECTION_REQUEST_COMMAND,                                       v1_1,          "M",                "E"},
    {HCI_REMOTE_NAME_REQUEST_COMMAND,                                             v1_1,          "O",                "E"},
    {HCI_REMOTE_NAME_REQUEST_COMPLETE_EVENT,                                      v1_1,          "C.106",            "E"}, 
    // C.106: Mandatory if the Remote Name Request command is supported, otherwise excluded
    {HCI_RESET_FAILED_CONTACT_COUNTER_COMMAND,                                    v1_1,          "M",                "E"},
    {HCI_RETURN_LINK_KEYS_EVENT,                                                  v1_1,          "C.121",            "E"}, 
    // C.121: Mandatory if the Write Stored Link Key command is supported, otherwise excluded
    {HCI_ROLE_CHANGE_EVENT,                                                       v1_1,          "C.212",            "E"}, 
    // C.212: Mandatory if Role Switch is supported, otherwise excluded
    {HCI_ROLE_DISCOVERY_COMMAND,                                                  v1_1,          "O",                "E"},
    {HCI_READ_SYNCHRONOUS_FLOW_CONTROL_ENABLE_COMMAND,                            v1_1,          "C.122",            "E"}, 
    // C.122: Mandatory if the Write Synchronous Flow Control Enable command is supported, otherwise excluded
    {HCI_READ_VOICE_SETTING_COMMAND,                                              v1_1,          "C.134",            "E"}, 
    // C.134: Mandatory if SCO or eSCO is supported, otherwise excluded
    {HCI_SET_CONNECTION_ENCRYPTION_COMMAND,                                       v1_1,          "M",                "E"},
    {HCI_SET_EVENT_FILTER_COMMAND,                                                v1_1,          "C.148",            "E"}, 
    // C.148: Optional if any of the specified events is supported, otherwise excluded
    {Sniff_Mode_command,                                                      v1_1,          "C.214",            "E"}, 
    // C.214: Mandatory if Sniff mode is supported, otherwise excluded
    {Write_Authentication_Enable_command,                                     v1_1,          "O",                "E"},
    {Write_Automatic_Flush_Timeout_command,                                   v1_1,          "M",                "E"},
    {Write_Class_of_Device_command,                                           v1_1,          "M",                "E"},
    {Write_Current_IAC_LAP_command,                                           v1_1,          "C.125",            "E"}, 
    // C.125: Mandatory if Inquiry Scan is supported, otherwise excluded
    {Switch_Role_command,                                                     v1_1,          "C.212",            "E"}, 
    // C.212: Mandatory if Role Switch is supported, otherwise excluded
    {Write_Hold_Mode_Activity_command,                                        v1_1,          "C.213",            "E"}, 
    // C.213: Mandatory if Hold mode is supported, otherwise excluded
    {Write_Inquiry_Scan_Activity_command,                                     v1_1,          "C.125",            "E"}, 
    // C.125: Mandatory if Inquiry Scan is supported, otherwise excluded
    {Write_Link_Policy_Settings_command,                                      v1_1,          "C.141",            "E"}, 
    // C.141: Mandatory if Role Switch, Hold mode, or Sniff mode is supported, otherwise excluded
    {Write_Link_Supervision_Timeout_command,                                  v1_1,          "O",                "E"},
    {Write_Local_Name_command,                                                v1_1,          "M",                "E"},
    {Write_Loopback_Mode_command,                                             v1_1,          "C.123",            "E"}, 
    // C.123: Mandatory if BR/EDR test mode is supported, otherwise excluded
    {Write_Num_Broadcast_Retransmissions_command,                             v1_1,          "O",                "E"},
    {Write_Page_Scan_Activity_command,                                        v1_1,          "M",                "E"},
    {Write_Page_Timeout_command,                                              v1_1,          "M",                "E"},
    {Write_PIN_Type_command,                                                  v1_1,          "O",                "E"},
    {Write_Scan_Enable_command,                                               v1_1,          "M",                "E"},
    {Write_Stored_Link_Key_command,                                           v1_1,          "O",                "E"},
    {Write_Synchronous_Flow_Control_Enable_command,                           v1_1,          "C.135",            "E"}, 
    // C.135: Optional if SCO or eSCO is supported, otherwise excluded
    {Write_Voice_Setting_command,                                             v1_1,          "C.134",            "E"}, 
    // C.134: Mandatory if SCO or eSCO is supported, otherwise excluded


    /****************************************************BT VERSION 1.2*************************************************************/
    {Write_AFH_Channel_Assessment_Mode_command,                               v1_2,          "C.140",            "C.58"}, 
    {HCI_READ_AFH_CHANNEL_ASSESSMENT_MODE_COMMAND,                                v1_2,          "C.140",            "C.58"}, 
    // C.140: Mandatory if the Controller supports AFH classification in either role or is an AFH capable Central; 
    // C.58: Mandatory if LE Feature (Channel Classification) is supported
    {HCI_READ_CLOCK_COMMAND,                                                      v1_2,          "O",                "E"},
    {HCI_ACCEPT_SYNCHRONOUS_CONNECTION_REQUEST_COMMAND,                       v1_2,         "C.134",             "E"}, 
    // C.134: Mandatory if SCO or eSCO is supported, otherwise excluded  
    {HCI_CREATE_CONNECTION_CANCEL_COMMAND,                                    v1_2,          "M",                "E"},
    {HCI_FLOW_SPECIFICATION_COMMAND,                                              v1_2,          "M",                "E"},
    {HCI_FLOW_SPECIFICATION_COMPLETE_EVENT,                                       v1_2,          "M",                "E"},
    {HCI_INQUIRY_RESULT_WITH_RSSI_EVENT,                                          v1_2,          "C.128",            "E"}, 
    // C.128: Optional if Inquiry is supported, otherwise excluded
    {HCI_READ_AFH_CHANNEL_MAP_COMMAND,                                            v1_2,          "C.139",            "E"}, 
    // C.139: Mandatory if the Controller is AFH capable in either role, otherwise excluded
    {HCI_READ_DEFAULT_LINK_POLICY_SETTINGS_COMMAND,                               v1_2,          "C.141",            "E"}, 
    {HCI_READ_INQUIRY_MODE_COMMAND,                                               v1_2,          "C.115",            "E"}, 
    // C.115: Mandatory if the Write Inquiry Mode command is supported, otherwise excluded
    {HCI_READ_INQUIRY_SCAN_TYPE_COMMAND,                                          v1_2,          "C.125",            "E"}, 
    // C.125: Mandatory if Inquiry Scan is supported, otherwise excluded
    {HCI_READ_LMP_HANDLE_COMMAND,                                                 v1_2,          "C.134",            "E"}, 
    // C.134: Mandatory if SCO or eSCO is supported, otherwise excluded
    {HCI_READ_LOCAL_EXTENDED_FEATURES_COMMAND,                                    v1_2,          "C.220",            "E"}, 
    // C.220: Mandatory if LMP Extended Features mask is supported, otherwise excluded
    {HCI_READ_PAGE_SCAN_TYPE_COMMAND,                                             v1_2,          "C.119",            "E"}, 
    // C.119: Mandatory if the Write Page Scan Type command is supported, otherwise excluded
    {HCI_READ_REMOTE_EXTENDED_FEATURES_COMMAND,                                   v1_2,          "C.220",            "E"}, 
    // C.220: Mandatory if LMP Extended Features mask is supported, otherwise excluded
    {HCI_READ_REMOTE_EXTENDED_FEATURES_COMPLETE_EVENT,                            v1_2,          "C.220",            "E"}, 
    // C.220: Mandatory if LMP Extended Features mask is supported, otherwise excluded
    {HCI_REJECT_SYNCHRONOUS_CONNECTION_REQUEST_COMMAND,                           v1_2,          "C.134",            "E"}, 
    // C.134: Mandatory if SCO or eSCO is supported, otherwise excluded
    {HCI_REMOTE_NAME_REQUEST_CANCEL_COMMAND,                                      v1_2,          "C.106",            "E"}, 
    // C.106: Mandatory if the Remote Name Request command is supported, otherwise excluded
    {HCI_SET_AFH_HOST_CHANNEL_CLASSIFICATION_COMMAND,                             v1_2,          "C.140",            "E"}, 
    // C.140: Mandatory if the Controller supports AFH classification in either role or is an AFH capable Central, otherwise excluded
    {Setup_Synchronous_Connection_command,                                    v1_2,          "C.134",            "E"}, 
    // C.134: Mandatory if SCO or eSCO is supported, otherwise excluded
    {Synchronous_Connection_Changed_event,                                    v1_2,          "C.134",            "E"}, 
    // C.134: Mandatory if SCO or eSCO is supported, otherwise excluded
    {Synchronous_Connection_Complete_event,                                   v1_2,          "C.134",            "E"}, 
    // C.134: Mandatory if SCO or eSCO is supported, otherwise excluded
    {Write_Default_Link_Policy_Settings_command,                              v1_2,          "C.141",            "E"}, 
    // C.141: Mandatory if Role Switch, Hold mode, or Sniff mode is supported, otherwise excluded
    {Write_Inquiry_Mode_command,                                              v1_2,          "C.146",            "E"}, 
    // C.146: Mandatory if the Extended Inquiry Result event or the IO Capability Request event is supported, otherwise optional if Inquiry is supported
    {Write_Inquiry_Scan_Type_command,                                         v1_2,          "C.125",            "E"}, 
    // C.125: Mandatory if Inquiry Scan is supported, otherwise excluded
    {Write_Page_Scan_Type_command,                                            v1_2,          "C.154",            "E"}, 
    // C.154: Mandatory if Interlaced Page Scan is supported, otherwise optional

        /****************************************************BT VERSION 2.1*************************************************************/
    {HCI_ENCRYPTION_KEY_REFRESH_COMPLETE_EVENT,                               v2_1_EDR,      "M",                "C.4"}, 
    // C.4: Mandatory if LE Feature (LE Encryption) is supported
    {HCI_ENHANCED_FLUSH_COMMAND,                                              v2_1_EDR,      "M",                "E"},
    {HCI_ENHANCED_FLUSH_COMPLETE_EVENT,                                       v2_1_EDR,      "M",                "E"},
    {HCI_EXTENDED_INQUIRY_RESULT_EVENT,                                           v2_1_EDR,      "C.147",            "E"}, 
    // C.147: Optional if the Inquiry Result with RSSI event is supported, otherwise excluded
    {HCI_IO_CAPABILITY_REQUEST_EVENT,                                             v2_1_EDR,      "M",                "E"},
    {HCI_IO_CAPABILITY_REQUEST_NEGATIVE_REPLY_COMMAND,                            v2_1_EDR,      "M",                "E"},
    {HCI_IO_CAPABILITY_REQUEST_REPLY_COMMAND,                                     v2_1_EDR,      "M",                "E"},
    {HCI_IO_CAPABILITY_RESPONSE_EVENT,                                            v2_1_EDR,      "M",                "E"},
    {HCI_KEYPRESS_NOTIFICATION_EVENT,                                             v2_1_EDR,      "M",                "E"},
    {HCI_LINK_SUPERVISION_TIMEOUT_CHANGED_EVENT,                                  v2_1_EDR,      "M",                "E"},
    {HCI_READ_DEFAULT_ERRONEOUS_DATA_REPORTING_COMMAND,                           v2_1_EDR,      "C.112",            "E"}, 
    {HCI_READ_EXTENDED_INQUIRY_RESPONSE_COMMAND,                                  v2_1_EDR,      "C.205",            "E"}, 
    // C.205: Mandatory if Extended Inquiry Response is supported, otherwise excluded
    {HCI_READ_INQUIRY_RESPONSE_TRANSMIT_POWER_LEVEL_COMMAND,                      v2_1_EDR,      "C.125",            "E"}, 
    // C.125: Mandatory if Inquiry Scan is supported, otherwise excluded
    {HCI_READ_LOCAL_OOB_DATA_COMMAND,                                             v2_1_EDR,      "M",                "E"},
    {HCI_READ_SIMPLE_PAIRING_MODE_COMMAND,                                        v2_1_EDR,      "M",                "E"},
    {HCI_REFRESH_ENCRYPTION_KEY_COMMAND,                                          v2_1_EDR,      "M",                "E"},
    {HCI_REMOTE_HOST_SUPPORTED_FEATURES_NOTIFICATION_EVENT,                       v2_1_EDR,   "C.106",               "E"}, 
    // C.106: Mandatory if the Remote Name Request command is supported, otherwise excluded
    {HCI_REMOTE_OOB_DATA_REQUEST_EVENT,                                           v2_1_EDR,      "M",                "E"},
    {HCI_REMOTE_OOB_DATA_REQUEST_NEGATIVE_REPLY_COMMAND,                          v2_1_EDR,      "M",                "E"},
    {HCI_REMOTE_OOB_DATA_REQUEST_REPLY_COMMAND,                                   v2_1_EDR,      "M",                "E"},
    {HCI_SEND_KEYPRESS_NOTIFICATION_COMMAND,                                      v2_1_EDR,      "M",                "E"},
    {Simple_Pairing_Complete_event,                                           v2_1_EDR,      "M",                "E"},
    {Sniff_Subrating_command,                                                 v2_1_EDR,      "C.221221",         "E"}, 
    // C.221: Mandatory if Sniff subrating is supported, otherwise excluded
    {Sniff_Subrating_event,                                                   v2_1_EDR,      "C.221",            "E"}, 
    // C.221: Mandatory if Sniff subrating is supported, otherwise excluded
    {User_Confirmation_Request_event,                                         v2_1_EDR,      "M",                "E"},
    {User_Confirmation_Request_Negative_Reply_command,                        v2_1_EDR,      "M",                "E"},
    {User_Confirmation_Request_Reply_command,                                 v2_1_EDR,      "M",                "E"},
    {User_Passkey_Notification_event,                                         v2_1_EDR,      "M",                "E"},
    {User_Passkey_Request_event,                                              v2_1_EDR,      "M",                "E"},
    {User_Passkey_Request_Negative_Reply_command,                             v2_1_EDR,      "M",                "E"},
    {User_Passkey_Request_Reply_command,                                      v2_1_EDR,      "M",                "E"},
    {Write_Default_Erroneous_Data_Reporting_command,                          v2_1_EDR,      "C.206",            "E"}, 
    // C.206: Mandatory if Erroneous Synchronous Data Reporting is supported, otherwise excluded
    {Write_Extended_Inquiry_Response_command,                                 v2_1_EDR,      "C.205",            "E"}, 
    // C.205: Mandatory if Extended Inquiry Response is supported, otherwise excluded
    {Write_Inquiry_Transmit_Power_Level_command,                              v2_1_EDR,      "C.127",            "E"}, 
    // C.127: Mandatory if Inquiry is supported, otherwise excluded
    {Write_Simple_Pairing_Debug_Mode_command,                                 v2_1_EDR,      "M",                "E"},
    {Write_Simple_Pairing_Mode_command,                                       v2_1_EDR,      "M",                "E"},

    /****************************************************BT VERSION 3.0*************************************************************/
    {HCI_SET_EVENT_MASK_PAGE_2_COMMAND,                                           v3_0_HS,       "C.145",            "C.145"}, 
    // C.145: Mandatory if any event in event mask page 2 is supported, otherwise optional
    {HCI_NUMBER_OF_COMPLETED_DATA_BLOCKS_EVENT,                                   v3_0_HS,       "C.124",            "E"}, 
    // C.124: Mandatory if Data block based flow control is supported, otherwise excluded
    {HCI_READ_DATA_BLOCK_SIZE_COMMAND,                                            v3_0_HS,       "C.124",            "E"}, 
    // C.124: Mandatory if Data block based flow control is supported, otherwise excluded
    // C.112: Mandatory if the Write Default Erroneous Data Reporting command is supported, otherwise excluded
    // C.141: Mandatory if Role Switch, Hold mode, or Sniff mode is supported, otherwise excluded
    {HCI_READ_ENCRYPTION_KEY_SIZE_COMMAND,                                        v3_0_HS,       "M",                "E"},
    {HCI_READ_ENHANCED_TRANSMIT_POWER_LEVEL_COMMAND,                              v3_0_HS,       "C.217",            "E"}, 
    // C.217: Mandatory if BR/EDR Enhanced Power Control is supported, otherwise excluded
    {HCI_READ_FLOW_CONTROL_MODE_COMMAND,                                          v3_0_HS,       "C.124",            "E"}, 
    // C.124: Mandatory if Data block based flow control is supported, otherwise excluded

    {Write_Flow_Control_Mode_command,                                         v3_0_HS,       "C.124",            "E"}, 
    // C.124: Mandatory if Data block based flow control is supported, otherwise excluded


    /****************************************************BT VERSION CSA2*************************************************************/
    {HCI_READ_LOCAL_SUPPORTED_CODECS_COMMAND,                                     CSA2,          "[v1] C.157 [v2] O","[v1] E [v2] O"}, 
    // C.157: Mandatory if the Read Local Supported Codecs command [v2] is supported, otherwise optional
    {HCI_ENHANCED_ACCEPT_SYNCHRONOUS_CONNECTION_REQUEST_COMMAND,              CSA2,          "C.135",            "E"}, 
    // C.135: Optional if SCO or eSCO is supported, otherwise excluded
    {HCI_ENHANCED_SETUP_SYNCHRONOUS_CONNECTION_COMMAND,                           CSA2,          "C.135",            "E"}, 
    // C.135: Optional if SCO or eSCO is supported, otherwise excluded

    /****************************************************BT VERSION CSA3*************************************************************/
    {Set_MWS_Transport_Layer_command,                                         CSA3,          "C.109",            "C.109"},
    {HCI_GET_MWS_TRANSPORT_LAYER_CONFIGURATION_COMMAND,                           CSA3,          "C.109",            "C.109"}, 
    // C.109: Mandatory if the Set MWS Signaling command is supported, otherwise excluded
    {Set_External_Frame_Configuration_command,                                CSA3,          "C.108",            "O"}, 
    // C.108: Mandatory if the Set MWS_PATTERN Configuration command is supported, otherwise optional
    {Set_MWS_Channel_Parameters_command,                                      CSA3,          "O",                "O"},
    {Set_MWS_Scan_Frequency_Table_command,                                    CSA3,          "O",                "O"},
    {Set_MWS_Signaling_command,                                               CSA3,          "O",                "O"},
    {Set_MWS_PATTERN_Configuration_command,                                   CSA3,          "C.136",            "E"}, 
    // C.136: Optional if Slot Availability Mask is supported, otherwise excluded

    /****************************************************BT VERSION CSA4*************************************************************/
    {HCI_CONNECTIONLESS_PERIPHERAL_BROADCAST_CHANNEL_MAP_CHANGE_EVENT,        CSA4,          "C.201",            "E"}, 
    // C.201: Mandatory if Connectionless Peripheral Broadcast - Transmitter is supported, otherwise excluded
    {HCI_CONNECTIONLESS_PERIPHERAL_BROADCAST_RECEIVE_EVENT,                   CSA4,          "C.202",            "E"}, 
    // C.202: Mandatory if Connectionless Peripheral Broadcast - Receiver is supported, otherwise excluded
    {HCI_CONNECTIONLESS_PERIPHERAL_BROADCAST_TIMEOUT_EVENT,                   CSA4,          "C.202",            "E"},
    // C.202: Mandatory if Connectionless Peripheral Broadcast - Receiver is supported, otherwise excluded
    {HCI_DELETE_RESERVED_LT_ADDR_COMMAND,                                     CSA4,          "C.201",            "E"}, 
    // C.201: Mandatory if Connectionless Peripheral Broadcast - Transmitter is supported, otherwise excluded
    {HCI_INQUIRY_RESPONSE_NOTIFICATION_EVENT,                                     CSA4,          "C.126",            "E"}, 
    // C.126: Optional if Inquiry Scan is supported, otherwise excluded
    {HCI_PERIPHERAL_PAGE_RESPONSE_TIMEOUT_EVENT,                                  CSA4,          "O",                "E"},
    {HCI_READ_SYNCHRONIZATION_TRAIN_PARAMETERS_COMMAND,                           CSA4,          "C.201",            "E"}, 
    // C.201: Mandatory if Connectionless Peripheral Broadcast - Transmitter is supported, otherwise excluded
    {HCI_RECEIVE_SYNCHRONIZATION_TRAIN_COMMAND,                                   CSA4,          "C.202",            "E"}, 
    // C.202: Mandatory if Connectionless Peripheral Broadcast - Receiver is supported, otherwise excluded
    {HCI_SET_CONNECTIONLESS_PERIPHERAL_BROADCAST_COMMAND,                         CSA4,          "C.201",            "E"}, 
    // C.201: Mandatory if Connectionless Peripheral Broadcast - Transmitter is supported, otherwise excluded
    {HCI_SET_CONNECTIONLESS_PERIPHERAL_BROADCAST_DATA_COMMAND,                    CSA4,          "C.201",            "E"}, 
    // C.201: Mandatory if Connectionless Peripheral Broadcast - Transmitter is supported, otherwise excluded
    {HCI_SET_CONNECTIONLESS_PERIPHERAL_BROADCAST_RECEIVE_COMMAND,                 CSA4,          "C.202",            "E"}, 
    // C.202: Mandatory if Connectionless Peripheral Broadcast - Receiver is supported, otherwise excluded
    {Set_Reserved_LT_ADDR_command,                                            CSA4,          "C.201",            "E"}, 
    // C.201: Mandatory if Connectionless Peripheral Broadcast - Transmitter is supported, otherwise excluded
    {Set_Triggered_Clock_Capture_command,                                     CSA4,          "O",                "E"},
    {Start_Synchronization_Train_command,                                     CSA4,          "C.201",            "E"}, 
    // C.201: Mandatory if Connectionless Peripheral Broadcast - Transmitter is supported, otherwise excluded
    {Synchronization_Train_Complete_event,                                    CSA4,          "C.201",            "E"}, 
    // C.201: Mandatory if Connectionless Peripheral Broadcast - Transmitter is supported, otherwise excluded
    {Synchronization_Train_Received_event,                                    CSA4,          "C.202",            "E"}, 
    // C.202: Mandatory if Connectionless Peripheral Broadcast - Receiver is supported, otherwise excluded
    {Triggered_Clock_Capture_event,                                           CSA4,          "C.110",            "E"}, 
    // C.110: Mandatory if the Set Triggered Clock Capture command is supported, otherwise excluded
    {Truncated_Page_Cancel_command,                                           CSA4,          "C.129",            "E"}, 
    // C.129: Mandatory if Truncated page state is supported, otherwise excluded
    {Truncated_Page_command,                                                  CSA4,          "C.129",            "E"}, 
    // C.129: Mandatory if Truncated page state is supported, otherwise excluded
    {Truncated_Page_Complete_event,                                           CSA4,          "C.129",            "E"}, 
    // C.129: Mandatory if Truncated page state is supported, otherwise excluded
    {Write_Synchronization_Train_Parameters_command,                          CSA4,          "C.201",            "E"}, 
    // C.201: Mandatory if Connectionless Peripheral Broadcast - Transmitter is supported, otherwise excluded

    // {HCI_READ_LOCAL_SIMPLE_PAIRING_OPTIONS_COMMAND,                              Erratum_10734,  "O",                "E"},

    /****************************************************BT VERSION 4.0*************************************************************/
    {HCI_LE_ADD_DEVICE_TO_FILTER_ACCEPT_LIST_COMMAND,                             v4_0,          "E",                "M"},
    {HCI_LE_CLEAR_FILTER_ACCEPT_LIST_COMMAND,                                     v4_0,          "E",                "M"},
    {HCI_LE_READ_FILTER_ACCEPT_LIST_SIZE_COMMAND,                                 v4_0,          "E",                "M"},
    {HCI_LE_READ_LOCAL_SUPPORTED_FEATURES_PAGE_0_COMMAND,                         v4_0,          "E",                "M"},
    {HCI_LE_READ_SUPPORTED_STATES_COMMAND,                                        v4_0,          "E",                "M"},
    {HCI_LE_REMOVE_DEVICE_FROM_FILTER_ACCEPT_LIST_COMMAND,                        v4_0,          "E",                "M"},
    {HCI_LE_SET_EVENT_MASK_COMMAND,                                               v4_0,          "E",                "M"},
    {HCI_LE_TEST_END_COMMAND,                                                     v4_0,          "E",                "M"},
    {HCI_LE_CONNECTION_UPDATE_COMMAND,                                            v4_0,          "E",                "C.62"}, 
    // C.62: Mandatory if the LE Controller supports Central role or supports both Peripheral role and LE Feature (Connection Parameters Request Procedure), otherwise excluded
    {HCI_LE_SET_ADVERTISING_DATA_COMMAND,                                         v4_0,          "E",                "C.97"}, 
    {HCI_LE_SET_ADVERTISING_ENABLE_COMMAND,                                       v4_0,          "E",                "C.97"}, 
    {HCI_LE_SET_ADVERTISING_PARAMETERS_COMMAND,                                   v4_0,          "E",                "C.97"},
    {HCI_LE_READ_ADVERTISING_PHYSICAL_CHANNEL_TX_POWER_COMMAND,                   v4_0,          "E",                "C.97"}, 
    // C.97: Mandatory if Advertising State is supported, otherwise excluded
    {HCI_LE_SET_SCAN_RESPONSE_DATA_COMMAND,                                       v4_0,          "E",                "C.15"}, 
    // C.15: Mandatory if LE Controller supports transmitting scannable advertisements, otherwise excluded
    {HCI_LE_SET_SCAN_ENABLE_COMMAND,                                              v4_0,          "E",                "C.98"}, 
    {HCI_LE_SET_SCAN_PARAMETERS_COMMAND,                                          v4_0,          "E",                "C.98"}, 
    {HCI_LE_ADVERTISING_REPORT_EVENT,                                             v4_0,          "E",                "C.98"},
    // C.98: Mandatory if Scanning State is supported, otherwise excluded
    {HCI_LE_CONNECTION_COMPLETE_EVENT,                                            v4_0,          "E",                "C.3"},
    {HCI_LE_CONNECTION_UPDATE_COMPLETE_EVENT,                                     v4_0,          "E",                "C.3"},
    {HCI_LE_READ_CHANNEL_MAP_COMMAND,                                             v4_0,          "E",                "C.3"}, 
    {HCI_LE_READ_REMOTE_FEATURES_PAGE_0_COMMAND,                                  v4_0,          "E",                "C.3"}, 
    {HCI_LE_READ_REMOTE_FEATURES_PAGE_0_COMPLETE_EVENT,                           v4_0,          "E",                "C.3"}, 
    // C.3: Mandatory if the LE Controller supports Connection State, otherwise excluded
    {HCI_LE_CREATE_CONNECTION_CANCEL_COMMAND,                                     v4_0,          "E",                "C.94"}, 
    // C.94: Mandatory if the LE Create Connection or LE Extended Create Connection command is supported, otherwise excluded
    {HCI_LE_CREATE_CONNECTION_COMMAND,                                            v4_0,          "E",                "C.59"}, 
    // C.59: Mandatory if the LE Controller supports Central role, otherwise excluded
    {HCI_LE_ENABLE_ENCRYPTION_COMMAND,                                            v4_0,          "E",                "C.60"},
     // C.60: Mandatory if the LE Controller supports Central role and LE Feature (LE Encryption), otherwise excluded
    {HCI_LE_ENCRYPT_COMMAND,                                                      v4_0,          "E",                "C.4"},
    {HCI_LE_RAND_COMMAND,                                                         v4_0,          "E",                "C.4"}, 
     // C.4: Mandatory if LE Feature (LE Encryption) is supported, otherwise excluded
    {HCI_LE_LONG_TERM_KEY_REQUEST_NEGATIVE_REPLY_COMMAND,                         v4_0,          "E",                "C.61"}, 
    {HCI_LE_LONG_TERM_KEY_REQUEST_REPLY_COMMAND,                                  v4_0,          "E",                "C.61"}, 
    {HCI_LE_LONG_TERM_KEY_REQUEST_EVENT,                                          v4_0,          "E",                "C.61"}, 
    // C.61: Mandatory if the LE Controller supports Peripheral role and LE Feature (LE Encryption), otherwise excluded
    {HCI_LE_READ_BUFFER_SIZE_COMMAND,                                             v4_0,          "E",                "[v1] C.3 [v2] C.55"}, 
    // C.3: Mandatory if the LE Controller supports Connection State; C.55: Optional if LE Feature (Connected Isochronous Stream - Central), or LE Feature (Connected Isochronous Stream - Peripheral), or LE Feature (Synchronized Receiver role) is supported
    {HCI_LE_RECEIVER_TEST_COMMAND,                                                v4_0,          "E",                "[v1] C.2 [v2] C.13 [v3] C.30"}, 
    // C.2: Mandatory if the LE Controller supports receiving packets;
    // C.13: Mandatory if LE Feature (LE 2M PHY) or LE Feature (LE Coded PHY) or LE Feature (Stable Modulation Index - Receiver) is supported;
    // C.30: Mandatory if LE Feature (Connection CTE Request) or LE Feature (Connectionless CTE Receiver) is supported
    {HCI_LE_SET_HOST_CHANNEL_CLASSIFICATION_COMMAND,                              v4_0,          "E",                "C.36"}, 
    // C.36: Mandatory if the LE Controller supports Central role or supports both Peripheral role and LE Feature (Channel Classification), otherwise optional
    {HCI_LE_SET_RANDOM_ADDRESS_COMMAND,                                           v4_0,          "E",                "C.1"}, 
    // C.1: Mandatory if the LE Controller supports transmitting packets, otherwise excluded

    {HCI_LE_TRANSMITTER_TEST_COMMAND,                                             v4_0,          "E",                "[v1] C.1 [v2] C.12 [v3] C.29 [v4] C.53"}, 
    // C.1: Mandatory if the LE Controller supports transmitting packets; 
    // C.12: Mandatory if LE 2M PHY/Coded PHY/Stable Modulation Index (Transmitter) is supported;
    // C.29: Similar to C.12; C.53: Related to specific transmitter test features
    {HCI_READ_LE_HOST_SUPPORT_COMMAND,                                            v4_0,          "C.116",            "E"}, 
    // C.116: Mandatory if the Write LE Host Support command is supported, otherwise excluded
    {Write_LE_Host_Support_command,                                           v4_0,          "C.153",            "E"}, 
    // C.153: Mandatory if LE supported in the Controller, otherwise optional

    /****************************************************BT VERSION 4.1*************************************************************/
    {HCI_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_EVENT,                            v4_1,          "E",                "C.6"}, 
    {HCI_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_NEGATIVE_REPLY_COMMAND,           v4_1,          "E",                "C.6"}, 
    {HCI_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_REPLY_COMMAND,                    v4_1,          "E",                "C.6"}, 
    // C.6: Mandatory if LE Feature (Connection Parameters Request procedure) is supported, otherwise excluded
    {HCI_AUTHENTICATED_PAYLOAD_TIMEOUT_EXPIRED_EVENT,                             v4_1,          "C.155",            "C.155"}, 
    {HCI_READ_AUTHENTICATED_PAYLOAD_TIMEOUT_COMMAND,                              v4_1,          "C.155",            "C.155"}, 
    // C.155: Mandatory if the Write Authenticated Payload Timeout command is supported, otherwise excluded
    {Write_Authenticated_Payload_Timeout_command,                             v4_1,          "C.151",            "C.7"}, 
    // C.151: Mandatory if Secure Connections (Controller) and Ping are supported; 
    // C.7: Mandatory if LE Feature (LE Encryption) and LE Feature (LE Ping) are supported
    {HCI_READ_LOCAL_OOB_EXTENDED_DATA_COMMAND,                                    v4_1,          "C.142",            "E"}, 
    {HCI_REMOTE_OOB_EXTENDED_DATA_REQUEST_REPLY_COMMAND,                          v4_1,          "C.142",            "E"}, 
    // C.142: Mandatory if Secure Connections (Controller) or Secure Simple Pairing (Controller) is supported, otherwise excluded
    {HCI_READ_EXTENDED_INQUIRY_LENGTH_COMMAND,                                    v4_1,          "C.113",            "E"}, 
    // C.113: Mandatory if the Write Extended Inquiry Length command is supported, otherwise excluded
    {HCI_READ_EXTENDED_PAGE_TIMEOUT_COMMAND,                                      v4_1,          "C.114",            "E"}, 
    // C.114: Mandatory if the Write Extended Page Timeout command is supported, otherwise excluded
    {HCI_READ_SECURE_CONNECTIONS_HOST_SUPPORT_COMMAND,                            v4_1,          "C.218",            "E"}, 
    {Write_Secure_Connections_Host_Support_command,                           v4_1,          "C.218",            "E"}, 
    // C.218: Mandatory if Secure Connections (Controller) is supported, otherwise excluded
    {Write_Extended_Inquiry_Length_command,                                   v4_1,          "C.128",            "E"}, 
    // C.128: Optional if Inquiry is supported, otherwise excluded
    {Write_Extended_Page_Timeout_command,                                     v4_1,          "O",                "E"},
    {Write_Secure_Connections_Test_Mode_command,                              v4_1,          "C.138",            "E"}, 
    // C.138: Mandatory if Secure Connections (Controller) is supported, otherwise optional if eSCO is supported

    /****************************************************BT VERSION 4.2*************************************************************/
    {HCI_LE_READ_SUGGESTED_DEFAULT_DATA_LENGTH_COMMAND,                           v4_2,          "E",                "C.8"}, 
    {HCI_LE_SET_DATA_LENGTH_COMMAND,                                              v4_2,          "E",                "C.8"}, 
    {HCI_LE_WRITE_SUGGESTED_DEFAULT_DATA_LENGTH_COMMAND,                          v4_2,          "E",                "C.8"}, 
    {HCI_LE_DATA_LENGTH_CHANGE_EVENT,                                             v4_2,          "E",                "C.8"}, 
    {HCI_LE_READ_MAXIMUM_DATA_LENGTH_COMMAND,                                     v4_2,          "E",                "C.8"}, 
    // C.8: Mandatory if LE Feature (LE Data Packet Length Extension) is supported, otherwise optional
    {HCI_LE_ADD_DEVICE_TO_RESOLVING_LIST_COMMAND,                                 v4_2,          "E",                "C.9"}, 
    {HCI_LE_CLEAR_RESOLVING_LIST_COMMAND,                                         v4_2,          "E",                "C.9"}, 
    {HCI_LE_READ_RESOLVING_LIST_SIZE_COMMAND,                                     v4_2,          "E",                "C.9"}, 
    {HCI_LE_REMOVE_DEVICE_FROM_RESOLVING_LIST_COMMAND,                            v4_2,          "E",                "C.9"}, 
    {HCI_LE_SET_ADDRESS_RESOLUTION_ENABLE_COMMAND,                                v4_2,          "E",                "C.9"}, 
    {HCI_LE_SET_RESOLVABLE_PRIVATE_ADDRESS_TIMEOUT_COMMAND,                       v4_2,          "E",                "C.9"}, 
    // C.9: Mandatory if LE Feature (LL Privacy) is supported, otherwise excluded
    {HCI_LE_READ_LOCAL_RESOLVABLE_ADDRESS_COMMAND,                                v4_2,          "E",                "C.10"}, 
    {HCI_LE_READ_PEER_RESOLVABLE_ADDRESS_COMMAND,                                 v4_2,          "E",                "C.10"}, 
    // C.10: Optional if LE Feature (LL Privacy) is supported, otherwise excluded
    {HCI_LE_DIRECTED_ADVERTISING_REPORT_EVENT,                                    v4_2,          "E",                "C.63"}, 
    // C.63: Mandatory if the LE Controller supports Scanning state and LE Feature (LL Privacy), otherwise excluded
    {HCI_LE_GENERATE_DHKEY_COMPLETE_EVENT,                                        v4_2,          "E",                "O"},    
    {HCI_LE_READ_LOCAL_P_256_PUBLIC_KEY_COMMAND,                                  v4_2,          "E",                "O"},
    {HCI_LE_READ_LOCAL_P_256_PUBLIC_KEY_COMPLETE_EVENT,                           v4_2,          "E",                "O"},
    {HCI_LE_ENHANCED_CONNECTION_COMPLETE_EVENT,                                   v4_2,          "E",                "[v1] C.24 [v2] C.69"}, 
    // C.24: Mandatory if the LE Controller supports Connection State and either LE Feature (LL Privacy) or LE Feature (Extended Advertising) is supported;
    // C.69: Mandatory if the LE Controller supports LE Feature (LL Extended Feature Set)
    {HCI_LE_GENERATE_DHKEY_CMMMMAND,                                               v4_2,          "E",                "[v1] C.99 [v2] O"}, 
    // C.99: Mandatory if LE Generate DHKey command [v2] is supported, otherwise optional


    /****************************************************BT VERSION 5.0*************************************************************/
    {HCI_LE_PHY_UPDATE_COMPLETE_EVENT,                                            v5_0,          "E",                "C.11"}, 
    {HCI_LE_READ_PHY_COMMAND,                                                     v5_0,          "E",                "C.11"}, 
    {HCI_LE_SET_DEFAULT_PHY_COMMAND,                                              v5_0,          "E",                "C.11"}, 
    {HCI_LE_SET_PHY_COMMAND,                                                      v5_0,          "E",                "C.11"}, 
    // C.11: Mandatory if LE Feature (LE 2M PHY) or LE Feature (LE Coded PHY) is supported, otherwise optional
    {HCI_LE_PERIODIC_ADVERTISING_CREATE_SYNC_CANCEL_COMMAND,                      v5_0,          "E",                "C.16"}, 
    {HCI_LE_PERIODIC_ADVERTISING_CREATE_SYNC_COMMAND,                             v5_0,          "E",                "C.16"},
    {HCI_LE_PERIODIC_ADVERTISING_SYNC_ESTABLISHED_EVENT,                          v5_0,          "E",                "[v1] C.16 [v2] C.68"}, 
    // C.68: Mandatory if LE Feature (Periodic Advertising with Responses - Scanner) is supported
    // C.16: Mandatory if LE Feature (Periodic Advertising) is supported and the LE Controller supports both Scanning State and Synchronization State; 
    {HCI_LE_ADVERTISING_SET_TERMINATED_EVENT,                                     v5_0,          "E",                "C.17"}, 
    {HCI_LE_CLEAR_ADVERTISING_SETS_COMMAND,                                       v5_0,          "E",                "C.17"}, 
    {HCI_LE_SCAN_REQUEST_RECEIVED_EVENT,                                          v5_0,          "E",                "C.17"}, 
    {HCI_LE_REMOVE_ADVERTISING_SET_COMMAND,                                       v5_0,          "E",                "C.17"}, 
    {HCI_LE_SET_ADVERTISING_SET_RANDOM_ADDRESS_COMMAND,                           v5_0,          "E",                "C.17"}, 
    {HCI_LE_READ_MAXIMUM_ADVERTISING_DATA_LENGTH_COMMAND,                         v5_0,          "E",                "C.17"},
    {HCI_LE_READ_NUMBER_OF_SUPPORTED_ADVERTISING_SETS_COMMAND,                    v5_0,          "E",                "C.17"}, 
    {HCI_LE_SET_EXTENDED_ADVERTISING_DATA_COMMAND,                                v5_0,          "E",                "C.17"}, 
    {HCI_LE_SET_EXTENDED_SCAN_RESPONSE_DATA_COMMAND,                              v5_0,          "E",                "C.17"}, 
    {HCI_LE_SET_EXTENDED_ADVERTISING_ENABLE_COMMAND,                              v5_0,          "E",                "C.17"}, 
    // C.17: Mandatory if LE Feature (Extended Advertising) is supported and the LE Controller supports Advertising State, otherwise excluded
    {HCI_LE_SET_PERIODIC_ADVERTISING_DATA_COMMAND,                                v5_0,          "E",                "C.18"}, 
    {HCI_LE_SET_PERIODIC_ADVERTISING_ENABLE_COMMAND,                              v5_0,          "E",                "C.18"}, 
    {HCI_LE_SET_PERIODIC_ADVERTISING_PARAMETERS_COMMAND,                          v5_0,          "E",                "[v1] C.18 [v2] C.67"}, 
    // C.18: Mandatory if LE Feature (Periodic Advertising) is supported and the LE Controller supports Advertising State; 
    // C.67: Mandatory if LE Feature (Periodic Advertising with Responses - Advertiser) is supported
    {HCI_LE_SET_EXTENDED_SCAN_ENABLE_COMMAND,                                     v5_0,          "E",                "C.19"}, 
    {HCI_LE_SET_EXTENDED_SCAN_PARAMETERS_COMMAND,                                 v5_0,          "E",                "C.19"}, 
    {HCI_LE_EXTENDED_ADVERTISING_REPORT_EVENT,                                    v5_0,          "E",                "C.19"}, 
    {HCI_LE_SCAN_TIMEOUT_EVENT,                                                   v5_0,          "E",                "C.19"}, 
    // C.19: Mandatory if LE Feature (Extended Advertising) is supported and the LE Controller supports Scanning State, otherwise excluded
    {HCI_LE_ADD_DEVICE_TO_PERIODIC_ADVERTISER_LIST_COMMAND,                       v5_0,          "E",                "C.21"}, 
    {HCI_LE_PERIODIC_ADVERTISING_SYNC_LOST_EVENT,                                 v5_0,          "E",                "C.21"}, 
    {HCI_LE_PERIODIC_ADVERTISING_TERMINATE_SYNC_COMMAND,                          v5_0,          "E",                "C.21"}, 
    {HCI_LE_READ_PERIODIC_ADVERTISER_LIST_SIZE_COMMAND,                           v5_0,          "E",                "C.21"}, 
    {HCI_LE_REMOVE_DEVICE_FROM_PERIODIC_ADVERTISER_LIST_COMMAND,                  v5_0,          "E",                "C.21"}, 
    {HCI_LE_CLEAR_PERIODIC_ADVERTISER_LIST_COMMAND,                               v5_0,          "E",                "C.21"}, 
    // C.21: Mandatory if LE Feature (Periodic Advertising) is supported and the LE Controller supports Synchronization State, otherwise excluded
    {HCI_LE_PERIODIC_ADVERTISING_REPORT_EVENT,                                    v5_0,          "E",                "[v1] C.21 [v2] C.68"}, 
    // C.21: Mandatory if LE Feature (Periodic Advertising) is supported and the LE Controller supports Synchronization State; 
    // C.68: Mandatory if LE Feature (Periodic Advertising with Responses - Scanner) is supported
    {HCI_LE_CHANNEL_SELECTION_ALGORITHM_EVENT,                                    v5_0,          "E",                "C.23"}, 
    // C.23: Mandatory if LE Feature (LE Channel Selection Algorithm #2) is supported, otherwise excluded
    {HCI_LE_EXTENDED_CREATE_CONNECTION_COMMAND,                                   v5_0,          "E",                "[v1] C.20 [v2] C.67"}, 
    // C.20: Mandatory if LE Feature (Extended Advertising) is supported and the LE Controller supports Initiating State;
    // C.67: Mandatory if LE Feature (Periodic Advertising with Responses - Scanner) is supported
    {HCI_LE_READ_RF_PATH_COMPENSATION_COMMAND,                                    v5_0,          "E",                "C.22"}, 
    // C.22: Mandatory if the LE Controller supports sending Transmit Power in advertisements or if LE Feature (LE Power Control Request) is supported, otherwise optional
    {HCI_LE_READ_TRANSMIT_POWER_COMMAND,                                          v5_0,          "E",                "C.64"}, 
    // C.64: Optional if the Controller supports transmitting packets, otherwise excluded
    {HCI_LE_SET_EXTENDED_ADVERTISING_PARAMETERS_COMMAND,                          v5_0,          "E",                "[v1] C.65 [v2] C.66"}, 
    // C.65: Mandatory if LE Set Extended Advertising Parameters command [v2] is supported; 
    // C.66: Mandatory if LE Feature (Advertising Coding Selection) is supported
    {HCI_LE_SET_PRIVACY_MODE_COMMAND,                                             v5_0,          "E",                "C.9"}, 
    // C.9: Mandatory if LE Feature (LL Privacy) is supported, otherwise excluded
    {HCI_LE_WRITE_RF_PATH_COMPENSATION_COMMAND,                                   v5_0,          "E",                "C.22"}, 
    // C.22: Mandatory if the LE Controller supports sending Transmit Power in advertisements or LE Power Control Request, otherwise optional
    {HCI_SAM_STATUS_CHANGE_EVENT,                                                 v5_0,          "C.219",            "E"}, 
    // C.219: Mandatory if Slot Availability Mask is supported, otherwise excluded


    /****************************************************BT VERSION 5.1*************************************************************/
    {HCI_LE_CONNECTION_CTE_REQUEST_ENABLE_COMMAND,                                v5_1,          "E",                "C.25"},
    {HCI_LE_CONNECTION_IQ_REPORT_EVENT,                                           v5_1,          "E",                "C.25"}, 
    {HCI_LE_CTE_REQUEST_FAILED_EVENT,                                             v5_1,          "E",                "C.25"}, 
    {HCI_LE_SET_CONNECTION_CTE_RECEIVE_PARAMETERS_COMMAND,                        v5_1,          "E",                "C.25"}, 
    // C.25: Mandatory if LE Feature (Connection CTE Request) is supported, otherwise excluded
    {HCI_LE_CONNECTION_CTE_RESPONSE_ENABLE_COMMAND,                               v5_1,          "E",                "C.26"}, 
    {HCI_LE_SET_CONNECTION_CTE_TRANSMIT_PARAMETERS_COMMAND,                       v5_1,          "E",                "C.26"}, 
    // C.26: Mandatory if LE Feature (Connection CTE Response) is supported, otherwise excluded
    {HCI_LE_SET_CONNECTIONLESS_CTE_TRANSMIT_ENABLE_COMMAND,                       v5_1,          "E",                "C.27"}, 
    {HCI_LE_SET_CONNECTIONLESS_CTE_TRANSMIT_PARAMETERS_COMMAND,                   v5_1,          "E",                "C.27"}, 
    // C.27: Mandatory if LE Feature (Connectionless CTE Transmitter) is supported, otherwise excluded
    {HCI_LE_CONNECTIONLESS_IQ_REPORT_EVENT,                                       v5_1,          "E",                "C.28"},
    {HCI_LE_SET_CONNECTIONLESS_IQ_SAMPLING_ENABLE_COMMAND,                        v5_1,          "E",                "C.28"}, 
    // C.28: Mandatory if LE Feature (Connectionless CTE Receiver) is supported, otherwise excluded
    {HCI_LE_READ_ANTENNA_INFORMATION_COMMAND,                                     v5_1,          "E",                "C.31"},
    // C.31: Mandatory if LE Feature (Connection CTE Request) or LE Feature (Connection CTE Response) or LE Feature (Connectionless CTE Transmitter) or LE Feature (Connectionless CTE Receiver) is supported, otherwise excluded
    {HCI_LE_SET_PERIODIC_ADVERTISING_RECEIVE_ENABLE_COMMAND,                      v5_1,          "E",                "C.32"}, 
    // C.32: Mandatory if LE Feature (Periodic Advertising Sync Transfer – Recipient) is supported, otherwise optional if LE Feature (Periodic Advertising) is supported
    {HCI_LE_PERIODIC_ADVERTISING_SYNC_TRANSFER_COMMAND,                           v5_1,          "E",                "C.33"}, 
    // C.33: Mandatory if LE Feature (Periodic Advertising Sync Transfer – Sender) is supported and the LE Controller supports Scanning State, otherwise excluded
    {HCI_LE_PERIODIC_ADVERTISING_SET_INFO_TRANSFER_COMMAND,                       v5_1,          "E",                "C.34"}, 
    // C.34: Mandatory if LE Feature (Periodic Advertising Sync Transfer – Sender) is supported and the LE Controller supports Advertising State, otherwise excluded
    {HCI_LE_SET_DEFAULT_PERIODIC_ADVERTISING_SYNC_TRANSFER_PARAMETERS_COMMAND,    v5_1,          "E",                "C.35"}, 
    {HCI_LE_SET_PERIODIC_ADVERTISING_SYNC_TRANSFER_PARAMETERS_COMMAND,            v5_1,          "E",                "C.35"}, 
    {HCI_LE_PERIODIC_ADVERTISING_SYNC_TRANSFER_RECEIVED_EVENT,                    v5_1,          "E",                "[v1] C.35 [v2] C.68"}, 
    // C.35: Mandatory if LE Feature (Periodic Advertising Sync Transfer – Recipient) is supported, otherwise optional if LE Feature (Periodic Advertising) is supported
    // C.68: Mandatory if LE Feature (Periodic Advertising with Responses - Scanner) is supported
    {HCI_LE_MODIFY_SLEEP_CLOCK_ACCURACY_COMMAND,                                  v5_1,          "E",                "C.37"}, 
    // C.37: Mandatory if the LE Controller can change its sleep clock accuracy, otherwise excluded

    /****************************************************BT VERSION 5.2*************************************************************/
    {HCI_LE_CIS_ESTABLISHED_EVENT,                                                v5_2,          "E",                "[v1] C.38 [v2] C.159"}, 
    // C.38: Mandatory if LE Feature (Connected Isochronous Stream - Peripheral) is supported;
    // C.159: Optional if the LE CIS Established event [v1] is supported
    {HCI_LE_SET_CIG_PARAMETERS_COMMAND,                                           v5_2,          "E",                "C.39"}, 
    {HCI_LE_SET_CIG_PARAMETERS_TEST_COMMAND,                                      v5_2,          "E",                "C.39"}, 
    {HCI_LE_CREATE_CIS_COMMAND,                                                   v5_2,          "E",                "C.39"}, 
    {HCI_LE_REMOVE_CIG_COMMAND,                                                   v5_2,          "E",                "C.39"}, 
    // C.39: Mandatory if LE Feature (Connected Isochronous Stream - Central) is supported, otherwise excluded
    {HCI_LE_ACCEPT_CIS_REQUEST_COMMAND,                                           v5_2,          "E",                "C.40"},
    {HCI_LE_CIS_REQUEST_EVENT,                                                    v5_2,          "E",                "C.40"}, 
    {HCI_LE_REJECT_CIS_REQUEST_COMMAND,                                           v5_2,          "E",                "C.40"}, 
    // C.40: Mandatory if LE Feature (Connected Isochronous Stream - Peripheral) is supported, otherwise excluded
    {HCI_LE_TERMINATE_BIG_COMMAND,                                                v5_2,          "E",                "C.41"}, 
    {HCI_LE_TERMINATE_BIG_COMPLETE_EVENT,                                         v5_2,          "E",                "C.41"}, 
    {HCI_LE_CREATE_BIG_COMMAND,                                                   v5_2,          "E",                "C.41"}, 
    {HCI_LE_CREATE_BIG_COMPLETE_EVENT,                                            v5_2,          "E",                "C.41"},
    {HCI_LE_CREATE_BIG_TEST_COMMAND,                                              v5_2,          "E",                "C.41"}, 
    // C.41: Mandatory if LE Feature (Isochronous Broadcaster) is supported, otherwise excluded
    {HCI_LE_BIG_CREATE_SYNC_COMMAND,                                              v5_2,          "E",                "C.42"}, 
    {HCI_LE_BIG_SYNC_ESTABLISHED_EVENT,                                           v5_2,          "E",                "C.42"}, 
    {HCI_LE_BIG_SYNC_LOST_EVENT,                                                  v5_2,          "E",                "C.42"}, 
    {HCI_LE_BIG_TERMINATE_SYNC_COMMAND,                                           v5_2,          "E",                "C.42"},
    // C.42: Mandatory if LE Feature (Synchronized Receiver role) is supported, otherwise excluded
    {HCI_LE_REQUEST_PEER_SCA_COMMAND,                                             v5_2,          "E",                "C.44"}, 
    // C.44: Mandatory if LE Feature (Sleep Clock Accuracy Updates) and either LE Feature (Connected Isochronous Stream - Central) or LE Feature (Connected Isochronous Stream - Peripheral) are supported
    {HCI_LE_ISO_TRANSMIT_TEST_COMMAND,                                            v5_2,          "E",                "C.45"}, 
    {HCI_LE_READ_ISO_TX_SYNC_COMMAND,                                             v5_2,          "E",                "C.45"}, 
    // C.45: Mandatory if LE Feature (Connected Isochronous Stream - Central), or LE Feature (Connected Isochronous Stream - Peripheral), or LE Feature (Isochronous Broadcaster) is supported, otherwise excluded     
    {HCI_LE_ISO_READ_TEST_COUNTERS_COMMAND,                                       v5_2,          "E",                "C.46"}, 
    {HCI_LE_ISO_RECEIVE_TEST_COMMAND,                                             v5_2,          "E",                "C.46"}, 
    // C.46: Mandatory if LE Feature (Connected Isochronous Stream - Central), or LE Feature (Connected Isochronous Stream - Peripheral), or LE Feature (Synchronized Receiver role) is supported, otherwise excluded    
    {HCI_LE_REMOVE_ISO_DATA_PATH_COMMAND,                                         v5_2,          "E",                "C.47"}, 
    {HCI_LE_SETUP_ISO_DATA_PATH_COMMAND,                                          v5_2,          "E",                "C.47"}, 
    {HCI_LE_ISO_TEST_END_COMMAND,                                                 v5_2,          "E",                "C.47"}, 
    // C.47: Mandatory if LE Feature (Connected Isochronous Stream - Central), or LE Feature (Connected Isochronous Stream - Peripheral), or LE Feature (Isochronous Broadcaster), or LE Feature (Synchronized Receiver role) is supported, otherwise excluded
    {HCI_LE_SET_HOST_FEATURE_COMMAND,                                             v5_2,          "E",                "[v1] C.49 [v2] C.77"}, 
    // C.49: Mandatory if LE Feature (Connected Isochronous Stream - Central), or LE Feature (Connected Isochronous Stream - Peripheral), or LE Feature (Connection Subrating), or LE Feature (Advertising Coding Selection), or LE Feature (Channel Sounding) is supported;
    // C.77: Optional if the LE Set Host Feature command [v1] is supported    
    {HCI_LE_READ_ISO_LINK_QUALITY_COMMAND,                                        v5_2,          "E",                "C.50"}, 
    // C.50: Optional if LE Feature (Connected Isochronous Stream - Central), or LE Feature (Connected Isochronous Stream - Peripheral), or LE Feature (Synchronized Receiver role) is supported, otherwise excluded    
    {HCI_LE_READ_REMOTE_TRANSMIT_POWER_LEVEL_COMMAND,                             v5_2,          "E",                "C.51"}, 
    {HCI_LE_SET_TRANSMIT_POWER_REPORTING_ENABLE_COMMAND,                          v5_2,          "E",                "C.51"}, 
    {HCI_LE_ENHANCED_READ_TRANSMIT_POWER_LEVEL_COMMAND,                           v5_2,          "E",                "C.51"}, 
    // C.51: Mandatory if LE Feature (LE Power Control Request) is supported, otherwise excluded
    {HCI_LE_SET_PATH_LOSS_REPORTING_ENABLE_COMMAND,                               v5_2,          "E",                "C.52"}, 
    {HCI_LE_SET_PATH_LOSS_REPORTING_PARAMETERS_COMMAND,                           v5_2,          "E",                "C.52"}, 
    {HCI_LE_PATH_LOSS_THRESHOLD_EVENT,                                            v5_2,          "E",                "C.52"}, 
    // C.52: Mandatory if LE Feature (LE Path Loss Monitoring) is supported, otherwise excluded
    {HCI_LE_BIGINFO_ADVERTISING_REPORT_EVENT,                                     v5_2,          "E",                "C.54"}, 
    // C.54: Mandatory if LE Feature (Synchronized Receiver) is supported, otherwise optional
    {HCI_LE_REQUEST_PEER_SCA_COMPLETE_EVENT,                                      v5_2,          "E",                "C.95"}, 
    // C.95: Mandatory if the LE Request Peer SCA command is supported, otherwise excluded
    {HCI_READ_LOCAL_SUPPORTED_CODEC_CAPABILITIES_COMMAND,                         v5_2,          "C.156",            "C.156"}, 
    {HCI_READ_LOCAL_SUPPORTED_CONTROLLER_DELAY_COMMAND,                           v5_2,          "C.156",            "C.156"},  
    {HCI_CONFIGURE_DATA_PATH_COMMAND,                                         v5_2,          "C.156",            "C.156"}, 
    // C.156: Mandatory if the Read Local Supported Codecs command [v2] is supported, otherwise excluded
    {HCI_SET_ECOSYSTEM_BASE_INTERVAL_COMMAND,                                     v5_2,          "O",                "O"},

    /****************************************************BT VERSION 5.3*************************************************************/
    {HCI_LE_SET_DATA_RELATED_ADDRESS_CHANGES_COMMAND,                             v5_3,          "E",                "C.10"}, 
    // C.10: Optional if LE Feature (LL Privacy) is supported, otherwise excluded
    {HCI_LE_SET_DEFAULT_SUBRATE_COMMAND,                                          v5_3,          "E",                "C.57"}, 
    {HCI_LE_SUBRATE_CHANGE_EVENT,                                                 v5_3,          "E",                "C.57"}, 
    {HCI_LE_SUBRATE_REQUEST_COMMAND,                                              v5_3,          "E",                "C.57"}, 
    // C.57: Mandatory if LE Feature (Connection Subrating) is supported, otherwise excluded    
    {Set_Min_Encryption_Key_Size_command,                                     v5_3,          "O",                "E"},


    /****************************************************BT VERSION 5.4*************************************************************/
    {HCI_LE_PERIODIC_ADVERTISING_RESPONSE_REPORT_EVENT,                           v5_4,          "E",                "C.67"}, 
    {HCI_LE_PERIODIC_ADVERTISING_SUBEVENT_DATA_REQUEST_EVENT,                     v5_4,          "E",                "C.67"}, 
    {HCI_LE_SET_PERIODIC_ADVERTISING_SUBEVENT_DATA_COMMAND,                       v5_4,          "E",                "C.67"}, 
    // C.67: Mandatory if LE Feature (Periodic Advertising with Responses - Advertiser) is supported, otherwise excluded
    {HCI_LE_SET_PERIODIC_ADVERTISING_RESPONSE_DATA_COMMAND,                       v5_4,          "E",                "C.68"}, 
    {HCI_LE_SET_PERIODIC_SYNC_SUBEVENT_COMMAND,                                   v5_4,          "E",                "C.68"}, 
    // C.68: Mandatory if LE Feature (Periodic Advertising with Responses - Scanner) is supported, otherwise excluded

    /****************************************************BT VERSION 6.0*************************************************************/
    {HCI_LE_CS_CONFIG_COMPLETE_EVENT,                                             v6_0,          "E",                "C.75"},
    {HCI_LE_CS_CREATE_CONFIG_COMMAND,                                             v6_0,          "E",                "C.75"}, 
    {HCI_LE_CS_PROCEDURE_ENABLE_COMMAND,                                          v6_0,          "E",                "C.75"}, 
    {HCI_LE_CS_PROCEDURE_ENABLE_COMPLETE_EVENT,                                   v6_0,          "E",                "C.75"}, 
    {HCI_LE_CS_READ_LOCAL_SUPPORTED_CAPABILITIES_COMMAND,                         v6_0,          "E",                "C.75"}, 
    {HCI_LE_CS_READ_REMOTE_FAE_TABLE_COMMAND,                                     v6_0,          "E",                "C.75"}, 
    {HCI_LE_CS_READ_REMOTE_FAE_TABLE_COMPLETE_EVENT,                              v6_0,          "E",                "C.75"}, 
    {HCI_LE_CS_READ_REMOTE_SUPPORTED_CAPABILITIES_COMMAND,                        v6_0,          "E",                "C.75"}, 
    {HCI_LE_CS_READ_REMOTE_SUPPORTED_CAPABILITIES_COMPLETE_EVENT,                 v6_0,          "E",                "C.75"}, 
    {HCI_LE_CS_REMOVE_CONFIG_COMMAND,                                             v6_0,          "E",                "C.75"}, 
    {HCI_LE_CS_SECURITY_ENABLE_COMMAND,                                           v6_0,          "E",                "C.75"}, 
    {HCI_LE_CS_SECURITY_ENABLE_COMPLETE_EVENT,                                    v6_0,          "E",                "C.75"},
    {HCI_LE_CS_SET_CHANNEL_CLASSIFICATION_COMMAND,                                v6_0,          "E",                "C.75"}, 
    {HCI_LE_CS_SET_DEFAULT_SETTINGS_COMMAND,                                      v6_0,          "E",                "C.75"}, 
    {HCI_LE_CS_SET_PROCEDURE_PARAMETERS_COMMAND,                                  v6_0,          "E",                "C.75"}, 
    {HCI_LE_CS_SUBEVENT_RESULT_CONTINUE_EVENT,                                    v6_0,          "E",                "C.75"}, 
    {HCI_LE_CS_SUBEVENT_RESULT_EVENT,                                             v6_0,          "E",                "C.75"}, 
    {HCI_LE_CS_TEST_COMMAND,                                                      v6_0,          "E",                "C.75"}, 
    {HCI_LE_CS_TEST_END_COMMAND,                                                  v6_0,          "E",                "C.75"}, 
    {HCI_LE_CS_TEST_END_COMPLETE_EVENT,                                           v6_0,          "E",                "C.75"},
    {HCI_LE_CS_WRITE_CACHED_REMOTE_SUPPORTED_CAPABILITIES_COMMAND,                v6_0,          "E",                "C.75"}, 
    // C.75: Mandatory if LE Feature (Channel Sounding) is supported, otherwise excluded

    {HCI_LE_ADD_DEVICE_TO_MONITORED_ADVERTISERS_LIST_COMMAND,                     v6_0,          "E",                "C.78"}, 
    {HCI_LE_CLEAR_MONITORED_ADVERTISERS_LIST_COMMAND,                             v6_0,          "E",                "C.78"}, 
    {HCI_LE_READ_MONITORED_ADVERTISERS_LIST_SIZE_COMMAND,                         v6_0,          "E",                "C.78"}, 
    {HCI_LE_REMOVE_DEVICE_FROM_MONITORED_ADVERTISERS_LIST_COMMAND,                v6_0,          "E",                "C.78"}, 
    {HCI_LE_ENABLE_MONITORING_ADVERTISERS_COMMAND,                                v6_0,          "E",                "C.78"}, 
    {HCI_LE_MONITORED_ADVERTISERS_REPORT_EVENT,                                   v6_0,          "E",                "C.78"},
    // C.78: Mandatory if LE Feature (Monitoring Advertisers) is supported, otherwise excluded

    {HCI_LE_FRAME_SPACE_UPDATE_COMMAND,                                           v6_0,          "E",                "C.79"}, 
    {HCI_LE_FRAME_SPACE_UPDATE_COMPLETE_EVENT,                                    v6_0,          "E",                "C.79"}, 
    // C.79: Mandatory if LE Feature (Frame Space Update) is supported, otherwise excluded

    {HCI_LE_READ_ALL_LOCAL_SUPPORTED_FEATURES_COMMAND,                            v6_0,          "E",                "C.70"}, 
    // C.70: Mandatory if the LE Controller supports LE Feature (LL Extended Feature Set), otherwise optional
    {HCI_LE_READ_ALL_REMOTE_FEATURES_COMMAND,                                     v6_0,          "E",                "C.71"}, 
    // C.71: Mandatory if the LE Controller supports Connection State and LE Feature (LL Extended Feature Set), otherwise optional if the LE Controller supports Connection State, otherwise excluded
    {HCI_LE_READ_ALL_REMOTE_FEATURES_COMPLETE_EVENT,                              v6_0,          "E",                "C.72"}, 
    // C.72: Mandatory if the LE Controller supports the LE Read All Remote Features command, otherwise excluded
    {HCI_LE_SET_DECISION_DATA_COMMAND,                                            v6_0,          "E",                "C.73"}, 
    // C.73: Mandatory if LE Feature (Decision-Based Advertising Filtering) is supported and the LE Controller supports Advertising State, otherwise excluded
    {HCI_LE_SET_DECISION_INSTRUCTIONS_COMMAND,                                    v6_0,          "E",                "C.74"}, 
    // C.74: Mandatory if LE Feature (Decision-Based Advertising Filtering) is supported and the LE Controller supports Scanning State, otherwise excluded
    {HCI_LE_CS_WRITE_CACHED_REMOTE_FAE_TABLE_COMMAND,                             v6_0,          "E",                "C.76"},
     // C.76: Mandatory if LE Feature (Channel Sounding) and initiator role are supported, otherwise excluded
};      


