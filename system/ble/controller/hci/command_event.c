#include"data.h"
#include"command_event.h"
#include"hci.h"
#include"../info.h"
#include"../ll/ll_config.h"

/************************************hci feature adapt layer****************************************/

/************* mandory supported hci cmd process **************/
#define RESET_PROCESS                                                        reset_process//v1_1
#define READ_BD_ADDR_PROCESS                                                 read_bd_addr_process//v1_1
#define READ_LOCAL_SUPPORTED_FEATURES_PROCESS                                read_local_supported_features_process//v1_1
#define READ_LOCAL_VERSION_INFORMATION_PROCESS                               read_local_version_information_process//v1_1
#define SET_EVENT_MASK_PROCESS                                               set_event_mask_process//v1_1
#define READ_LOCAL_SUPPORTED_COMMANDS_PROCESS                                read_local_supported_commands_process//v1_2

#define LE_READ_FILTER_ACCEPT_LIST_SIZE_PROCESS                              le_read_filter_accept_list_size_process//v4_0
#define LE_ADD_DEVICE_TO_FILTER_ACCEPT_LIST_PROCESS                          le_add_device_to_filter_accept_list_process//v4_0
#define LE_REMOVE_DEVICE_FROM_FILTER_ACCEPT_LIST_PROCESS                     le_remove_device_from_filter_accept_list_process//v4_0
#define LE_CLEAR_FILTER_ACCEPT_LIST_PROCESS                                  le_clear_filter_accept_list_process//v4_0
#define LE_READ_LOCAL_SUPPORTED_FEATURES_PAGE_0_PROCESS                      le_read_local_supported_features_page_0_process//v4_0
#define LE_READ_SUPPORTED_STATES_PROCESS                                     le_read_supported_states_process//v4_0
#define LE_SET_EVENT_MASK_PROCESS                                            le_set_event_mask_process//v4_0
#define LE_TEST_END_PROCESS                                                  le_test_end_process//v4_0

/************* optional supported hci cmd process **************/
#define READ_LOCAL_SUPPORTED_CODECS_V2_PROCESS                               NULL
#define SET_EXTERNAL_FRAME_CONFIGURATION_PROCESS                             NULL
#define SET_MWS_CHANNEL_PARAMETERS_PROCESS                                   NULL
#define SET_MWS_SCAN_FREQUENCY_TABLE_PROCESS                                 NULL
#define SET_MWS_SIGNALING_PROCESS                                            NULL
#define LE_READ_LOCAL_P_256_PUBLIC_KEY_PROCESS                               NULL
#define LE_GENERATE_DHKEY_V2_PROCESS                                         NULL
#define SET_ECOSYSTEM_BASE_INTERVAL_PROCESS                                  NULL
// C.145: Mandatory if any event in event mask page 2 is supported, otherwise optional
#define SET_EVENT_MASK_PAGE_2_PROCESS                                        NULL

#if(SET_MWS_SIGNALING_PROCESS)
// C.109: Mandatory if the Set MWS Signaling command is supported, otherwise excluded
#define SET_MWS_TRANSPORT_LAYER_PROCESS                                      NULL
#define GET_MWS_TRANSPORT_LAYER_CONFIGURATION_PROCESS                        NULL
#else
#define SET_MWS_TRANSPORT_LAYER_PROCESS                                      NULL
#define GET_MWS_TRANSPORT_LAYER_CONFIGURATION_PROCESS                        NULL
#endif


/************************ state combination *****************************/
#if(BLE_SUPPORT_CONN_STATE)
// C.3: Mandatory if the LE Controller supports Connection State; 
#define DISCONNECT_PROCESS                                                   NULL
#define READ_REMOTE_VERSION_INFORMATION_PROCESS                              NULL
#define READ_RSSI_PROCESS                                                    NULL
#define READ_TRANSMIT_POWER_LEVEL_PROCESS                                    NULL
#define LE_READ_CHANNEL_MAP_PROCESS                                          NULL
#define LE_READ_REMOTE_FEATURES_PAGE_0_PROCESS                               NULL
#define LE_READ_BUFFER_SIZE_PROCESS                                          NULL
// C.96: Optional if the LE Controller supports Connection State, otherwise excluded
#define SET_CONTROLLER_TO_HOST_FLOW_CONTROL_PROCESS                          NULL

#if(SET_CONTROLLER_TO_HOST_FLOW_CONTROL_PROCESS)
// C.107: Mandatory if the Set Controller To Host Flow Control command is supported, otherwise excluded
#define HOST_BUFFER_SIZE_PROCESS                                             NULL
#define HOST_NUMBER_OF_COMPLETED_PACKETS_PROCESS                             NULL
#else
#define HOST_BUFFER_SIZE_PROCESS                                             NULL
#define HOST_NUMBER_OF_COMPLETED_PACKETS_PROCESS                             NULL
#endif

#else
#define DISCONNECT_PROCESS                                                   NULL
#define READ_REMOTE_VERSION_INFORMATION_PROCESS                              NULL
#define READ_RSSI_PROCESS                                                    NULL
#define READ_TRANSMIT_POWER_LEVEL_PROCESS                                    NULL
#define LE_READ_CHANNEL_MAP_PROCESS                                          NULL
#define LE_READ_REMOTE_FEATURES_PAGE_0_PROCESS                               NULL
#define LE_READ_BUFFER_SIZE_PROCESS                                          NULL
#define SET_CONTROLLER_TO_HOST_FLOW_CONTROL_PROCESS                          NULL
#define HOST_BUFFER_SIZE_PROCESS                                             NULL
#define HOST_NUMBER_OF_COMPLETED_PACKETS_PROCESS                             NULL
#endif

#if(BLE_SUPPORT_BROADCAST)
#else
#endif

#if(BLE_SUPPORT_OBSERVER)
#else
#endif

#if(BLE_SUPPORT_CENTRAL)
#else
#endif

#if(BLE_SUPPORT_PERIPHERAL)
#else
#endif

/************************ feature ***************************************/

#if(LL_SUPPORT_LE_ENCRYPTION)

#else
#endif

#if(LL_SUPPORT_CONNECTION_PARAMETERS_REQUEST)
#else
#endif

#if(LL_SUPPORT_EXTENDED_REJECT)
#else
#endif

#if(LL_SUPPORT_PERIPHERAL_INITIATED_FEATURES_EXCHANGE)
#else
#endif

#if(LL_SUPPORT_LE_PING)
#else
#endif

#if(LL_SUPPORT_LE_DATA_PACKET_LENGTH_EXTENSION)
#else
#endif

#if(LL_SUPPORT_LL_PRIVACY)
#else
#endif

#if(LL_SUPPORT_EXTENDED_SCANNING_FILTER_POLICIES)
#else
#endif

#if(LL_SUPPORT_LE_2M_PHY)
#else
#endif

#if(LL_SUPPORT_STABLE_MODULATION_INDEX_TX)
#else
#endif

#if(LL_SUPPORT_STABLE_MODULATION_INDEX_RX)
#else
#endif

#if(LL_SUPPORT_LE_CODED_PHY)
#else
#endif

#if(LL_SUPPORT_LE_EXTENDED_ADVERTISING)
#else
#endif

#if(LL_SUPPORT_LE_PERIODIC_ADVERTISING)
#else
#endif

#if(LL_SUPPORT_CHANNEL_SELECTION_ALGORITHM_2)
#else
#endif

#if(LL_SUPPORT_LE_POWER_CLASS_1)
#else
#endif

#if(LL_SUPPORT_MINIMUM_NUMBER_OF_USED_CHANNELS_PROCEDURE)
#else
#endif

#if(LL_SUPPORT_CONNECTION_CTE_REQUEST)
#else
#endif

#if(LL_SUPPORT_CONNECTION_CTE_RESPONSE)
#else
#endif

#if(LL_SUPPORT_CONNECTIONLESS_CTE_TRANSMITTER)
#else
#endif

#if(LL_SUPPORT_CONNECTIONLESS_CTE_RECEIVER)
#else
#endif

#if(LL_SUPPORT_ANTENNA_SWITCHING_DURING_CTE_TRANSMISSION)
#else
#endif

#if(LL_SUPPORT_ANTENNA_SWITCHING_DURING_CTE_RECEPTION)
#else
#endif

#if(LL_SUPPORT_RECEIVING_CONSTANT_TONE_EXTENSIONS)
#else
#endif

#if(LL_SUPPORT_PERIODIC_ADVERTISING_SYNC_TRANSFER_SENDER)
#else
#endif

#if(LL_SUPPORT_PERIODIC_ADVERTISING_SYNC_TRANSFER_RECIPIENT)
#else
#endif

#if(LL_SUPPORT_SLEEP_CLOCK_ACCURACY_UPDATES)
#else
#endif

#if(LL_SUPPORT_REMOTE_PUBLIC_KEY_VALIDATION)
#else
#endif

#if(LL_SUPPORT_CONNECTED_ISOCHRONOUS_STREAM_CENTRAL)
#else
#endif

#if(LL_SUPPORT_CONNECTED_ISOCHRONOUS_STREAM_PERIPHERAL)
// C.40: Mandatory if LE Feature (Connected Isochronous Stream - Peripheral) is supported, otherwise excluded
#define READ_CONNECTION_ACCEPT_TIMEOUT_PROCESS                               NULL
#define WRITE_CONNECTION_ACCEPT_TIMEOUT_PROCESS                              NULL
#else
#define READ_CONNECTION_ACCEPT_TIMEOUT_PROCESS                               NULL
#define WRITE_CONNECTION_ACCEPT_TIMEOUT_PROCESS                              NULL
#endif

#if(LL_SUPPORT_ISOCHRONOUS_BROADCASTER)
#else
#endif

#if(LL_SUPPORT_SYNCHRONIZED_RECEIVER)
#else
#endif

#if(LL_SUPPORT_CONNECTED_ISOCHRONOUS_STREAM_HOST_SUPPORT)
#else
#endif

#if(LL_SUPPORT_LE_POWER_CONTROL_REQUEST)
#else
#endif

#if(LL_SUPPORT_LE_POWER_CONTROL_REQUEST1)
#else
#endif

#if(LL_SUPPORT_LE_PATH_LOSS_MONITORING)
#else
#endif

#if(LL_SUPPORT_PERIODIC_ADVERTISING_ADI_SUPPORT)
#else
#endif

#if(LL_SUPPORT_CONNECTION_SUBRATING)
#else
#endif

#if(LL_SUPPORT_CONNECTION_SUBRATING_HOST_SUPPORT)
#else
#endif

#if(LL_SUPPORT_CHANNEL_CLASSIFICATION)
// C.58: Mandatory if LE Feature (Channel Classification) is supported
#define WRITE_AFH_CHANNEL_ASSESSMENT_MODE_PROCESS                            NULL
#define READ_AFH_CHANNEL_ASSESSMENT_MODE_PROCESS                             NULL
#else
#define WRITE_AFH_CHANNEL_ASSESSMENT_MODE_PROCESS                            NULL
#define READ_AFH_CHANNEL_ASSESSMENT_MODE_PROCESS                             NULL
#endif

#if(LL_SUPPORT_ADVERTISING_CODING_SELECTION)
#else
#endif

#if(LL_SUPPORT_ADVERTISING_CODING_SELECTION_HOST_SUPPORT)
#else
#endif

#if(LL_SUPPORT_DECISION_BASED_ADVERTISING_FILTERING)
#else
#endif

#if(LL_SUPPORT_PERIODIC_ADVERTISING_WITH_RESPONSES_ADVERTISER)
#else
#endif

#if(LL_SUPPORT_PERIODIC_ADVERTISING_WITH_RESPONSES_SCANNER)
#else
#endif

#if(LL_SUPPORT_UNSEGMENTED_FRAMED_MODE)
#else
#endif

#if(LL_SUPPORT_CHANNEL_SOUNDING)
#else
#endif

#if(LL_SUPPORT_CHANNEL_SOUNDING_HOST_SUPPORT)
#else
#endif

#if(LL_SUPPORT_CHANNEL_SOUNDING_TONE_QUALITY_INDICATION)
#else
#endif

#if(LL_SUPPORT_LE_EXTENDED_FEATURE_SET)
#else
#endif

#if(LL_SUPPORT_MONITORING_ADVERTISERS)
#else
#endif

#if(LL_SUPPORT_FRAME_SPACE_UPDATE)
#else
#endif




#define INQUIRY_PROCESS                                                      NULL
#define INQUIRY_CANCEL_PROCESS                                               NULL
#define PERIODIC_INQUIRY_MODE_PROCESS                                        NULL
#define EXIT_PERIODIC_INQUIRY_MODE_PROCESS                                   NULL
#define CREATE_CONNECTION_PROCESS                                            NULL

#define CREATE_CONNECTION_CANCEL_PROCESS                                     NULL
#define ACCEPT_CONNECTION_REQUEST_PROCESS                                    NULL
#define REJECT_CONNECTION_REQUEST_PROCESS                                    NULL
#define LINK_KEY_REQUEST_REPLY_PROCESS                                       NULL
#define LINK_KEY_REQUEST_NEGATIVE_REPLY_PROCESS                              NULL
#define PIN_CODE_REQUEST_REPLY_PROCESS                                       NULL
#define PIN_CODE_REQUEST_NEGATIVE_REPLY_PROCESS                              NULL
#define CHANGE_CONNECTION_PACKET_TYPE_PROCESS                                NULL
#define AUTHENTICATION_REQUESTED_PROCESS                                     NULL
#define SET_CONNECTION_ENCRYPTION_PROCESS                                    NULL
#define CHANGE_CONNECTION_LINK_KEY_PROCESS                                   NULL
#define LINK_KEY_SELECTION_PROCESS                                           NULL
#define REMOTE_NAME_REQUEST_PROCESS                                          NULL
#define REMOTE_NAME_REQUEST_CANCEL_PROCESS                                   NULL
#define READ_REMOTE_SUPPORTED_FEATURES_PROCESS                               NULL
#define READ_REMOTE_EXTENDED_FEATURES_PROCESS                                NULL

#define READ_CLOCK_OFFSET_PROCESS                                            NULL
#define READ_LMP_HANDLE_PROCESS                                              NULL
#define SETUP_SYNCHRONOUS_CONNECTION_PROCESS                                 NULL
#define ACCEPT_SYNCHRONOUS_CONNECTION_REQUEST_PROCESS                        NULL
#define REJECT_SYNCHRONOUS_CONNECTION_REQUEST_PROCESS                        NULL
#define IO_CAPABILITY_REQUEST_REPLY_PROCESS                                  NULL
#define USER_CONFIRMATION_REQUEST_REPLY_PROCESS                              NULL
#define USER_CONFIRMATION_REQUEST_NEGATIVE_REPLY_PROCESS                     NULL
#define USER_PASSKEY_REQUEST_REPLY_PROCESS                                   NULL
#define USER_PASSKEY_REQUEST_NEGATIVE_REPLY_PROCESS                          NULL
#define REMOTE_OOB_DATA_REQUEST_REPLY_PROCESS                                NULL
#define REMOTE_OOB_DATA_REQUEST_NEGATIVE_REPLY_PROCESS                       NULL
#define IO_CAPABILITY_REQUEST_NEGATIVE_REPLY_PROCESS                         NULL
#define ENHANCED_SETUP_SYNCHRONOUS_CONNECTION_PROCESS                        NULL
#define ENHANCED_ACCEPT_SYNCHRONOUS_CONNECTION_REQUEST_PROCESS               NULL
#define TRUNCATED_PAGE_PROCESS                                               NULL
#define TRUNCATED_PAGE_CANCEL_PROCESS                                        NULL
#define SET_CONNECTIONLESS_PERIPHERAL_BROADCAST_PROCESS                      NULL
#define SET_CONNECTIONLESS_PERIPHERAL_BROADCAST_RECEIVE_PROCESS              NULL
#define START_SYNCHRONIZATION_TRAIN_PROCESS                                  NULL
#define RECEIVE_SYNCHRONIZATION_TRAIN_PROCESS                                NULL
#define REMOTE_OOB_EXTENDED_DATA_REQUEST_REPLY_PROCESS                       NULL


#define HOLD_MODE_PROCESS                                                    NULL
#define SNIFF_MODE_PROCESS                                                   NULL
#define EXIT_SNIFF_MODE_PROCESS                                              NULL
#define QOS_SETUP_PROCESS                                                    NULL
#define ROLE_DISCOVERY_PROCESS                                               NULL
#define SWITCH_ROLE_PROCESS                                                  NULL
#define READ_LINK_POLICY_SETTINGS_PROCESS                                    NULL
#define WRITE_LINK_POLICY_SETTINGS_PROCESS                                   NULL
#define READ_DEFAULT_LINK_POLICY_SETTINGS_PROCESS                            NULL
#define WRITE_DEFAULT_LINK_POLICY_SETTINGS_PROCESS                           NULL
#define FLOW_SPECIFICATION_PROCESS                                           NULL
#define SNIFF_SUBRATING_PROCESS                                              NULL



#define SET_EVENT_FILTER_PROCESS                                             NULL
#define FLUSH_PROCESS                                                        NULL
#define READ_PIN_TYPE_PROCESS                                                NULL
#define WRITE_PIN_TYPE_PROCESS                                               NULL
#define READ_STORED_LINK_KEY_PROCESS                                         NULL
#define WRITE_STORED_LINK_KEY_PROCESS                                        NULL
#define DELETE_STORED_LINK_KEY_PROCESS                                       NULL
#define WRITE_LOCAL_NAME_PROCESS                                             NULL
#define READ_LOCAL_NAME_PROCESS                                              NULL


#define READ_PAGE_TIMEOUT_PROCESS                                            NULL
#define WRITE_PAGE_TIMEOUT_PROCESS                                           NULL
#define READ_SCAN_ENABLE_PROCESS                                             NULL
#define WRITE_SCAN_ENABLE_PROCESS                                            NULL
#define READ_PAGE_SCAN_ACTIVITY_PROCESS                                      NULL
#define WRITE_PAGE_SCAN_ACTIVITY_PROCESS                                     NULL
#define READ_INQUIRY_SCAN_ACTIVITY_PROCESS                                   NULL
#define WRITE_INQUIRY_SCAN_ACTIVITY_PROCESS                                  NULL
#define READ_AUTHENTICATION_ENABLE_PROCESS                                   NULL
#define WRITE_AUTHENTICATION_ENABLE_PROCESS                                  NULL
#define READ_CLASS_OF_DEVICE_PROCESS                                         NULL
#define WRITE_CLASS_OF_DEVICE_PROCESS                                        NULL
#define READ_VOICE_SETTING_PROCESS                                           NULL
#define WRITE_VOICE_SETTING_PROCESS                                          NULL
#define READ_AUTOMATIC_FLUSH_TIMEOUT_PROCESS                                 NULL
#define WRITE_AUTOMATIC_FLUSH_TIMEOUT_PROCESS                                NULL
#define READ_NUM_BROADCAST_RETRANSMISSIONS_PROCESS                           NULL
#define WRITE_NUM_BROADCAST_RETRANSMISSIONS_PROCESS                          NULL
#define READ_HOLD_MODE_ACTIVITY_PROCESS                                      NULL
#define WRITE_HOLD_MODE_ACTIVITY_PROCESS                                     NULL

#define READ_SYNCHRONOUS_FLOW_CONTROL_ENABLE_PROCESS                         NULL
#define WRITE_SYNCHRONOUS_FLOW_CONTROL_ENABLE_PROCESS                        NULL



#define READ_LINK_SUPERVISION_TIMEOUT_PROCESS                                NULL
#define WRITE_LINK_SUPERVISION_TIMEOUT_PROCESS                               NULL
#define READ_NUMBER_OF_SUPPORTED_IAC_PROCESS                                 NULL
#define READ_CURRENT_IAC_LAP_PROCESS                                         NULL
#define WRITE_CURRENT_IAC_LAP_PROCESS                                        NULL
#define SET_AFH_HOST_CHANNEL_CLASSIFICATION_PROCESS                          NULL
#define READ_INQUIRY_SCAN_TYPE_PROCESS                                       NULL
#define WRITE_INQUIRY_SCAN_TYPE_PROCESS                                      NULL
#define READ_INQUIRY_MODE_PROCESS                                            NULL
#define WRITE_INQUIRY_MODE_PROCESS                                           NULL
#define READ_PAGE_SCAN_TYPE_PROCESS                                          NULL
#define WRITE_PAGE_SCAN_TYPE_PROCESS                                         NULL


#define READ_EXTENDED_INQUIRY_RESPONSE_PROCESS                               NULL
#define WRITE_EXTENDED_INQUIRY_RESPONSE_PROCESS                              NULL
#define REFRESH_ENCRYPTION_KEY_PROCESS                                       NULL
#define READ_SIMPLE_PAIRING_MODE_PROCESS                                     NULL
#define WRITE_SIMPLE_PAIRING_MODE_PROCESS                                    NULL
#define READ_LOCAL_OOB_DATA_PROCESS                                          NULL
#define READ_INQUIRY_RESPONSE_TRANSMIT_POWER_LEVEL_PROCESS                   NULL
#define WRITE_INQUIRY_TRANSMIT_POWER_LEVEL_PROCESS                           NULL
#define SEND_KEYPRESS_NOTIFICATION_PROCESS                                   NULL
#define READ_DEFAULT_ERRONEOUS_DATA_REPORTING_PROCESS                        NULL
#define WRITE_DEFAULT_ERRONEOUS_DATA_REPORTING_PROCESS                       NULL
#define ENHANCED_FLUSH_PROCESS                                               NULL

#define READ_FLOW_CONTROL_MODE_PROCESS                                       NULL
#define WRITE_FLOW_CONTROL_MODE_PROCESS                                      NULL
#define READ_ENHANCED_TRANSMIT_POWER_LEVEL_PROCESS                           NULL
#define READ_LE_HOST_SUPPORT_PROCESS                                         NULL
#define WRITE_LE_HOST_SUPPORT_PROCESS                                        NULL




#define SET_MWS_PATTERN_CONFIGURATION_PROCESS                                NULL
#define SET_RESERVED_LT_ADDR_PROCESS                                         NULL
#define DELETE_RESERVED_LT_ADDR_PROCESS                                      NULL
#define SET_CONNECTIONLESS_PERIPHERAL_BROADCAST_DATA_PROCESS                 NULL
#define READ_SYNCHRONIZATION_TRAIN_PARAMETERS_PROCESS                        NULL
#define WRITE_SYNCHRONIZATION_TRAIN_PARAMETERS_PROCESS                       NULL
#define READ_SECURE_CONNECTIONS_HOST_SUPPORT_PROCESS                         NULL
#define WRITE_SECURE_CONNECTIONS_HOST_SUPPORT_PROCESS                        NULL
#define READ_AUTHENTICATED_PAYLOAD_TIMEOUT_PROCESS                           NULL
#define WRITE_AUTHENTICATED_PAYLOAD_TIMEOUT_PROCESS                          NULL
#define READ_LOCAL_OOB_EXTENDED_DATA_PROCESS                                 NULL
#define READ_EXTENDED_PAGE_TIMEOUT_PROCESS                                   NULL
#define WRITE_EXTENDED_PAGE_TIMEOUT_PROCESS                                  NULL
#define READ_EXTENDED_INQUIRY_LENGTH_PROCESS                                 NULL
#define WRITE_EXTENDED_INQUIRY_LENGTH_PROCESS                                NULL

#define CONFIGURE_DATA_PATH_PROCESS                                          NULL
#define SET_MIN_ENCRYPTION_KEY_SIZE_PROCESS                                  NULL





#define READ_LOCAL_EXTENDED_FEATURES_PROCESS                                 NULL
#define READ_BUFFER_SIZE_PROCESS                                             NULL

#define READ_DATA_BLOCK_SIZE_PROCESS                                         NULL
#define READ_LOCAL_SUPPORTED_CODECS_PROCESS                                  NULL
#define READ_LOCAL_SIMPLE_PAIRING_OPTIONS_PROCESS                            NULL

#define READ_LOCAL_SUPPORTED_CODEC_CAPABILITIES_PROCESS                      NULL
#define READ_LOCAL_SUPPORTED_CONTROLLER_DELAY_PROCESS                        NULL


#define READ_FAILED_CONTACT_COUNTER_PROCESS                                  NULL
#define RESET_FAILED_CONTACT_COUNTER_PROCESS                                 NULL
#define READ_LINK_QUALITY_PROCESS                                            NULL

#define READ_AFH_CHANNEL_MAP_PROCESS                                         NULL
#define READ_CLOCK_PROCESS                                                   NULL
#define READ_ENCRYPTION_KEY_SIZE_PROCESS                                     NULL

#define SET_TRIGGERED_CLOCK_CAPTURE_PROCESS                                  NULL

#define READ_LOOPBACK_MODE_PROCESS                                           NULL
#define WRITE_LOOPBACK_MODE_PROCESS                                          NULL
#define ENABLE_IMPLEMENTATION_UNDER_TEST_MODE_PROCESS                        NULL
#define WRITE_SIMPLE_PAIRING_DEBUG_MODE_PROCESS                              NULL
#define WRITE_SECURE_CONNECTIONS_TEST_MODE_PROCESS                           NULL




#define LE_READ_BUFFER_SIZE_V2_PROCESS                                       NULL

#define LE_SET_RANDOM_ADDRESS_PROCESS                                        NULL
#define LE_SET_ADVERTISING_PARAMETERS_PROCESS                                NULL
#define LE_READ_ADVERTISING_PHYSICAL_CHANNEL_TX_POWER_PROCESS                NULL
#define LE_SET_ADVERTISING_DATA_PROCESS                                      NULL
#define LE_SET_SCAN_RESPONSE_DATA_PROCESS                                    NULL
#define LE_SET_ADVERTISING_ENABLE_PROCESS                                    NULL
#define LE_SET_SCAN_PARAMETERS_PROCESS                                       NULL
#define LE_SET_SCAN_ENABLE_PROCESS                                           NULL
#define LE_CREATE_CONNECTION_PROCESS                                         NULL
#define LE_CREATE_CONNECTION_CANCEL_PROCESS                                  NULL




#define LE_CONNECTION_UPDATE_PROCESS                                         NULL
#define LE_SET_HOST_CHANNEL_CLASSIFICATION_PROCESS                           NULL


#define LE_ENCRYPT_PROCESS                                                   NULL
#define LE_RAND_PROCESS                                                      NULL
#define LE_ENABLE_ENCRYPTION_PROCESS                                         NULL
#define LE_LONG_TERM_KEY_REQUEST_REPLY_PROCESS                               NULL
#define LE_LONG_TERM_KEY_REQUEST_NEGATIVE_REPLY_PROCESS                      NULL

#define LE_RECEIVER_TEST_PROCESS                                             NULL
#define LE_RECEIVER_TEST_V2_PROCESS                                          NULL
#define LE_RECEIVER_TEST_V3_PROCESS                                          NULL
#define LE_TRANSMITTER_TEST_PROCESS                                          NULL
#define LE_TRANSMITTER_TEST_V2_PROCESS                                       NULL
#define LE_TRANSMITTER_TEST_V3_PROCESS                                       NULL
#define LE_TRANSMITTER_TEST_V4_PROCESS                                       NULL

#define LE_REMOTE_CONNECTION_PARAMETER_REQUEST_REPLY_PROCESS                 NULL
#define LE_REMOTE_CONNECTION_PARAMETER_REQUEST_NEGATIVE_REPLY_PROCESS        NULL
#define LE_SET_DATA_LENGTH_PROCESS                                           NULL
#define LE_READ_SUGGESTED_DEFAULT_DATA_LENGTH_PROCESS                        NULL
#define LE_WRITE_SUGGESTED_DEFAULT_DATA_LENGTH_PROCESS                       NULL

#define LE_GENERATE_DHKEY_PROCESS                                            NULL

#define LE_ADD_DEVICE_TO_RESOLVING_LIST_PROCESS                              NULL
#define LE_REMOVE_DEVICE_FROM_RESOLVING_LIST_PROCESS                         NULL
#define LE_CLEAR_RESOLVING_LIST_PROCESS                                      NULL
#define LE_READ_RESOLVING_LIST_SIZE_PROCESS                                  NULL
#define LE_READ_PEER_RESOLVABLE_ADDRESS_PROCESS                              NULL
#define LE_READ_LOCAL_RESOLVABLE_ADDRESS_PROCESS                             NULL
#define LE_SET_ADDRESS_RESOLUTION_ENABLE_PROCESS                             NULL
#define LE_SET_RESOLVABLE_PRIVATE_ADDRESS_TIMEOUT_PROCESS                    NULL
#define LE_READ_MAXIMUM_DATA_LENGTH_PROCESS                                  NULL
#define LE_READ_PHY_PROCESS                                                  NULL
#define LE_SET_DEFAULT_PHY_PROCESS                                           NULL
#define LE_SET_PHY_PROCESS                                                   NULL
#define LE_SET_ADVERTISING_SET_RANDOM_ADDRESS_PROCESS                        NULL
#define LE_SET_EXTENDED_ADVERTISING_PARAMETERS_PROCESS                       NULL
#define LE_SET_EXTENDED_ADVERTISING_PARAMETERS_V2_PROCESS                    NULL
#define LE_SET_EXTENDED_ADVERTISING_DATA_PROCESS                             NULL
#define LE_SET_EXTENDED_SCAN_RESPONSE_DATA_PROCESS                           NULL
#define LE_SET_EXTENDED_ADVERTISING_ENABLE_PROCESS                           NULL
#define LE_READ_MAXIMUM_ADVERTISING_DATA_LENGTH_PROCESS                      NULL
#define LE_READ_NUMBER_OF_SUPPORTED_ADVERTISING_SETS_PROCESS                 NULL
#define LE_REMOVE_ADVERTISING_SET_PROCESS                                    NULL
#define LE_CLEAR_ADVERTISING_SETS_PROCESS                                    NULL
#define LE_SET_PERIODIC_ADVERTISING_PARAMETERS_PROCESS                       NULL
#define LE_SET_PERIODIC_ADVERTISING_PARAMETERS_V2_PROCESS                    NULL
#define LE_SET_PERIODIC_ADVERTISING_DATA_PROCESS                             NULL
#define LE_SET_PERIODIC_ADVERTISING_ENABLE_PROCESS                           NULL
#define LE_SET_EXTENDED_SCAN_PARAMETERS_PROCESS                              NULL
#define LE_SET_EXTENDED_SCAN_ENABLE_PROCESS                                  NULL
#define LE_EXTENDED_CREATE_CONNECTION_PROCESS                                NULL
#define LE_EXTENDED_CREATE_CONNECTION_V2_PROCESS                             NULL
#define LE_PERIODIC_ADVERTISING_CREATE_SYNC_PROCESS                          NULL
#define LE_PERIODIC_ADVERTISING_CREATE_SYNC_CANCEL_PROCESS                   NULL
#define LE_PERIODIC_ADVERTISING_TERMINATE_SYNC_PROCESS                       NULL
#define LE_ADD_DEVICE_TO_PERIODIC_ADVERTISER_LIST_PROCESS                    NULL
#define LE_REMOVE_DEVICE_FROM_PERIODIC_ADVERTISER_LIST_PROCESS               NULL
#define LE_CLEAR_PERIODIC_ADVERTISER_LIST_PROCESS                            NULL
#define LE_READ_PERIODIC_ADVERTISER_LIST_SIZE_PROCESS                        NULL
#define LE_READ_TRANSMIT_POWER_PROCESS                                       NULL
#define LE_READ_RF_PATH_COMPENSATION_PROCESS                                 NULL
#define LE_WRITE_RF_PATH_COMPENSATION_PROCESS                                NULL
#define LE_SET_PRIVACY_MODE_PROCESS                                          NULL
#define LE_SET_CONNECTIONLESS_CTE_TRANSMIT_PARAMETERS_PROCESS                NULL
#define LE_SET_CONNECTIONLESS_CTE_TRANSMIT_ENABLE_PROCESS                    NULL
#define LE_SET_CONNECTIONLESS_IQ_SAMPLING_ENABLE_PROCESS                     NULL
#define LE_SET_CONNECTION_CTE_RECEIVE_PARAMETERS_PROCESS                     NULL
#define LE_SET_CONNECTION_CTE_TRANSMIT_PARAMETERS_PROCESS                    NULL
#define LE_CONNECTION_CTE_REQUEST_ENABLE_PROCESS                             NULL
#define LE_CONNECTION_CTE_RESPONSE_ENABLE_PROCESS                            NULL
#define LE_READ_ANTENNA_INFORMATION_PROCESS                                  NULL
#define LE_SET_PERIODIC_ADVERTISING_RECEIVE_ENABLE_PROCESS                   NULL
#define LE_PERIODIC_ADVERTISING_SYNC_TRANSFER_PROCESS                        NULL
#define LE_PERIODIC_ADVERTISING_SET_INFO_TRANSFER_PROCESS                    NULL
#define LE_SET_PERIODIC_ADVERTISING_SYNC_TRANSFER_PARAMETERS_PROCESS         NULL
#define LE_SET_DEFAULT_PERIODIC_ADVERTISING_SYNC_TRANSFER_PARAMETERS_PROCESS NULL
#define LE_MODIFY_SLEEP_CLOCK_ACCURACY_PROCESS                               NULL
#define LE_READ_ISO_TX_SYNC_PROCESS                                          NULL
#define LE_SET_CIG_PARAMETERS_PROCESS                                        NULL
#define LE_SET_CIG_PARAMETERS_TEST_PROCESS                                   NULL
#define LE_CREATE_CIS_PROCESS                                                NULL
#define LE_REMOVE_CIG_PROCESS                                                NULL
#define LE_ACCEPT_CIS_REQUEST_PROCESS                                        NULL
#define LE_REJECT_CIS_REQUEST_PROCESS                                        NULL
#define LE_CREATE_BIG_PROCESS                                                NULL
#define LE_CREATE_BIG_TEST_PROCESS                                           NULL
#define LE_TERMINATE_BIG_PROCESS                                             NULL
#define LE_BIG_CREATE_SYNC_PROCESS                                           NULL
#define LE_BIG_TERMINATE_SYNC_PROCESS                                        NULL
#define LE_REQUEST_PEER_SCA_PROCESS                                          NULL
#define LE_SETUP_ISO_DATA_PATH_PROCESS                                       NULL
#define LE_REMOVE_ISO_DATA_PATH_PROCESS                                      NULL
#define LE_ISO_TRANSMIT_TEST_PROCESS                                         NULL
#define LE_ISO_RECEIVE_TEST_PROCESS                                          NULL
#define LE_ISO_READ_TEST_COUNTERS_PROCESS                                    NULL
#define LE_ISO_TEST_END_PROCESS                                              NULL
#define LE_SET_HOST_FEATURE_PROCESS                                          NULL
#define LE_SET_HOST_FEATURE_V2_PROCESS                                       NULL
#define LE_READ_ISO_LINK_QUALITY_PROCESS                                     NULL
#define LE_ENHANCED_READ_TRANSMIT_POWER_LEVEL_PROCESS                        NULL
#define LE_READ_REMOTE_TRANSMIT_POWER_LEVEL_PROCESS                          NULL
#define LE_SET_PATH_LOSS_REPORTING_PARAMETERS_PROCESS                        NULL
#define LE_SET_PATH_LOSS_REPORTING_ENABLE_PROCESS                            NULL
#define LE_SET_TRANSMIT_POWER_REPORTING_ENABLE_PROCESS                       NULL
#define LE_SET_DATA_RELATED_ADDRESS_CHANGES_PROCESS                          NULL
#define LE_SET_DEFAULT_SUBRATE_PROCESS                                       NULL
#define LE_SUBRATE_REQUEST_PROCESS                                           NULL
#define LE_SET_PERIODIC_ADVERTISING_SUBEVENT_DATA_PROCESS                    NULL
#define LE_SET_PERIODIC_ADVERTISING_RESPONSE_DATA_PROCESS                    NULL
#define LE_SET_PERIODIC_SYNC_SUBEVENT_PROCESS                                NULL
#define LE_READ_ALL_LOCAL_SUPPORTED_FEATURES_PROCESS                         NULL
#define LE_READ_ALL_REMOTE_FEATURES_PROCESS                                  NULL
#define LE_CS_READ_LOCAL_SUPPORTED_CAPABILITIES_PROCESS                      NULL
#define LE_CS_READ_REMOTE_SUPPORTED_CAPABILITIES_PROCESS                     NULL
#define LE_CS_WRITE_CACHED_REMOTE_SUPPORTED_CAPABILITIES_PROCESS             NULL
#define LE_CS_SECURITY_ENABLE_PROCESS                                        NULL
#define LE_CS_SET_DEFAULT_SETTINGS_PROCESS                                   NULL
#define LE_CS_READ_REMOTE_FAE_TABLE_PROCESS                                  NULL
#define LE_CS_WRITE_CACHED_REMOTE_FAE_TABLE_PROCESS                          NULL
#define LE_CS_CREATE_CONFIG_PROCESS                                          NULL
#define LE_CS_REMOVE_CONFIG_PROCESS                                          NULL
#define LE_CS_SET_CHANNEL_CLASSIFICATION_PROCESS                             NULL
#define LE_CS_SET_PROCEDURE_PARAMETERS_PROCESS                               NULL
#define LE_CS_PROCEDURE_ENABLE_PROCESS                                       NULL
#define LE_CS_TEST_PROCESS                                                   NULL
#define LE_CS_TEST_END_PROCESS                                               NULL
#define LE_SET_DECISION_DATA_PROCESS                                         NULL
#define LE_SET_DECISION_INSTRUCTIONS_PROCESS                                 NULL
#define LE_ADD_DEVICE_TO_MONITORED_ADVERTISERS_LIST_PROCESS                  NULL
#define LE_REMOVE_DEVICE_FROM_MONITORED_ADVERTISERS_LIST_PROCESS             NULL
#define LE_CLEAR_MONITORED_ADVERTISERS_LIST_PROCESS                          NULL
#define LE_ENABLE_MONITORING_ADVERTISERS_PROCESS                             NULL
#define LE_READ_MONITORED_ADVERTISERS_LIST_SIZE_PROCESS                      NULL
#define LE_FRAME_SPACE_UPDATE_PROCESS                                        NULL
/******************************************hci event**********************************************/
static _u8  hci_event_buffer[256];
static _u16 hciCommandOpcode; 
#define HCI_NUMBER_OF_COMMAND_PACKETS               1


static void* hci_event(_s32 eventCode,_u8 len,bt_hci_event_t** event)
{
    *event = (bt_hci_event_t*)hci_event_buffer;
    (*event)->eventCode = eventCode;
    (*event)->length    = len;
    return (*event)->parameter;
}


typedef struct _PACKED
{
    _u8  status;
    _u8  numOfCmd;
    _u16 opcode;
}hci_event_command_status_t;
static bt_hci_event_t* hci_command_status_event(_u16 opcode,controller_error_code_e status)
{
    bt_hci_event_t* event = (bt_hci_event_t*)hci_event_buffer;
    hci_event_command_status_t *param = (hci_event_command_status_t*)event->parameter;
    event->eventCode = HCI_COMMAND_STATUS_EVENT;
    event->length = sizeof(hci_event_command_status_t);
    param->numOfCmd = HCI_NUMBER_OF_COMMAND_PACKETS;
    param->opcode = opcode;
    param->status = status;
    return event;
}

typedef struct _PACKED
{
    _u8  numOfCmd;
    _u16 opcode;
    _u8  data[0];
}hci_event_command_complete_t;

static void* hci_command_complete_event(_u16 opcode,_u8 len,bt_hci_event_t **event)
{
    hci_event_command_complete_t * command =  (hci_event_command_complete_t*)hci_event((_s32)HCI_COMMAND_COMPLETE_EVENT,len+3,event);
    command->numOfCmd = HCI_NUMBER_OF_COMMAND_PACKETS;
    command->opcode   = opcode;
    txMemsetByte(command->data,0,len);
    return command->data;
}

/******************************************hci  command**********************************************/
typedef controller_error_code_e(*hci_command_f)(_u8* data,_u8 length,bt_hci_event_t** event);

typedef struct 
{
	_u32                       ocf;
	hci_command_f              process;
}hci_command_t;

#define HCI_COMMAND_LIST_LENGTH(hci_command_list)      (sizeof(hci_command_list)/sizeof(hci_command_list[0]))
#define HCI_COMMAND_LENGTH(hci_command_array)          (sizeof(hci_command_array)/sizeof(hci_command_array[0]))

typedef struct
{
    hci_command_e        command;
	hci_command_t const *pArray;
	_u32                 conut;
}hci_command_array_t;

/****************************************hci link control command***********************************/

/****************************************hci link policy command***********************************/

/****************************************hci controller baseband command***************************/
struct reset_retParam_t
{
    _u8 status;
};
controller_error_code_e reset_process(_u8* data,_u8 length,bt_hci_event_t** event)
{
    controller_error_code_e status = ll_reset();
    struct reset_retParam_t* retParam = (struct reset_retParam_t*)hci_command_complete_event(hciCommandOpcode,sizeof(struct reset_retParam_t),event);
    retParam->status = (_u8)status;
    return status;
}


struct _PACKED set_event_mask_retParam_t
{
	_u8 status;
};
controller_error_code_e set_event_mask_process(_u8* data,_u8 length,bt_hci_event_t** event)
{
	_u64 eventMask = ((_u64)data[0])|((_u64)data[1]<<8)|((_u64)data[2]<<16)|((_u64)data[3]<<24)|(((_u64)data[4]<<32))|((_u64)data[5]<<40)|((_u64)data[6]<<48)|((_u64)data[7]<<56);
	controller_error_code_e status = ll_set_event_mask(eventMask);
	struct set_event_mask_retParam_t* retParam = \
    (struct set_event_mask_retParam_t*)hci_command_complete_event(hciCommandOpcode,sizeof(struct set_event_mask_retParam_t),event);
	retParam->status = status;
	return status;
}

/****************************************hci status parameters command*********************************/

/****************************************hci testing command*******************************************/

/****************************************hci le controller command*************************************/
struct _PACKED read_filter_accept_list_size_retParam_t
{
    _u8 status;
    _u8 size;
};
controller_error_code_e le_read_filter_accept_list_size_process(_u8* data,_u8 length,bt_hci_event_t** event)
{
    struct read_filter_accept_list_size_retParam_t* retParam = \
    (struct read_filter_accept_list_size_retParam_t*)hci_command_complete_event(hciCommandOpcode,sizeof(struct read_filter_accept_list_size_retParam_t),event);
    retParam->status = (_u8)SUCCESS;
    retParam->size   = BLE_FILTER_ACCEPT_LIST_SIZE;
    return SUCCESS;
}


struct _PACKED le_add_device_to_filter_accept_list_retParam_t
{
    _u8 status;
};
struct _PACKED le_add_device_to_filter_accept_list_param_t
{
    _u8 addrType;
    _u8 addr[6];
};
controller_error_code_e le_add_device_to_filter_accept_list_process(_u8* data,_u8 length,bt_hci_event_t** event)
{
    struct le_add_device_to_filter_accept_list_param_t* param  = (struct le_add_device_to_filter_accept_list_param_t*)data;
    int status = ll_add_device_to_filter_accept_list(param->addrType,param->addr);
    struct le_add_device_to_filter_accept_list_retParam_t* retParam = \
    (struct le_add_device_to_filter_accept_list_retParam_t*)hci_command_complete_event(hciCommandOpcode,sizeof(struct le_add_device_to_filter_accept_list_retParam_t),event);
    retParam->status = SUCCESS;
    return status;
}

struct _PACKED le_remove_device_from_filter_accept_list_retParam_t
{
    _u8 status;
};
struct _PACKED le_remove_device_from_filter_accept_list_param_t
{
    _u8 addrType;
    _u8 addr[6];
};
controller_error_code_e le_remove_device_from_filter_accept_list_process(_u8* data,_u8 length,bt_hci_event_t** event)
{
    struct le_remove_device_from_filter_accept_list_param_t* param  = (struct le_remove_device_from_filter_accept_list_param_t*)data;
    int status = ll_remove_device_from_filter_accept_list(param->addrType,param->addr);
    struct le_remove_device_from_filter_accept_list_retParam_t* retParam = \
    (struct le_remove_device_from_filter_accept_list_retParam_t*)hci_command_complete_event(hciCommandOpcode,sizeof(struct le_remove_device_from_filter_accept_list_retParam_t),event);
    retParam->status = status;
    return status;
}

struct _PACKED le_clear_filter_accept_list_retParam_t
{
    _u8 status;
};
controller_error_code_e le_clear_filter_accept_list_process(_u8* data,_u8 length,bt_hci_event_t** event)
{
    int status = ll_clear_filter_accept_list();
    struct le_clear_filter_accept_list_retParam_t* param = \
    (struct le_clear_filter_accept_list_retParam_t*)hci_command_complete_event(hciCommandOpcode,sizeof(struct le_clear_filter_accept_list_retParam_t),event);
    param->status = status;
    return status;
}


struct _PACKED le_read_local_supported_features_page_0_retParam_t
{
	_u8  status;
    _u8  feature[8];
};
controller_error_code_e le_read_local_supported_features_page_0_process(_u8* data,_u8 length,bt_hci_event_t** event)
{
	struct le_read_local_supported_features_page_0_retParam_t *retParam = \
    (struct le_read_local_supported_features_page_0_retParam_t*)hci_command_complete_event(hciCommandOpcode,sizeof(struct le_read_local_supported_features_page_0_retParam_t),event);
	retParam->status  = (_u8)SUCCESS;
	ll_get_feature(retParam->feature,8);
	return SUCCESS;
}

struct _PACKED le_read_supported_states_retParam_t
{
	_u8  status;
    _u64 states;
};
controller_error_code_e le_read_supported_states_process(_u8* data,_u8 length,bt_hci_event_t** event)
{
    struct le_read_supported_states_retParam_t* retParam = \
    (struct le_read_supported_states_retParam_t*)hci_command_complete_event(hciCommandOpcode,sizeof(struct le_read_supported_states_retParam_t),event);
    retParam->status = (_u8)SUCCESS;

    #if(BLE_SUPPORT_BROADCAST)
    retParam->states|=(BIT64(0)  | BIT64(1)  | BIT64(8)  | BIT64(9)  | BIT64(12) | \
		BIT64(13) | BIT64(16) | BIT64(17) | BIT64(18) | BIT64(19) | \
		BIT64(20) | BIT64(21));
    #endif

    #if(BLE_SUPPORT_OBSERVER)
    retParam->states|=(BIT64(4)  | BIT64(5)  | BIT64(8)  | BIT64(9)  | BIT64(10) | \
		BIT64(11) | BIT64(12) | BIT64(13) | BIT64(14) | BIT64(15) | \
		BIT64(22) | BIT64(23) | BIT64(24) | BIT64(25) | BIT64(26) | \
		BIT64(27) | BIT64(30) | BIT64(31));
    #endif

    #if(BLE_SUPPORT_CENTRAL)
    retParam->states|=(BIT64(6)  | BIT64(16) | BIT64(17) | BIT64(18) | BIT64(19) | \
		BIT64(22) | BIT64(23) | BIT64(24) | BIT64(25) | BIT64(28) | \
		BIT64(32) | BIT64(33) | BIT64(34) | BIT64(35) | BIT64(36) | \
		BIT64(37) | BIT64(41));
    #endif

    #if(BLE_SUPPORT_PERIPHERAL)
    retParam->states|=(BIT64(2)  | BIT64(3)  | BIT64(7)  | BIT64(10) | BIT64(11) | \
		BIT64(14) | BIT64(15) | BIT64(20) | BIT64(21) | BIT64(26) | \
		BIT64(27) | BIT64(29) | BIT64(30) | BIT64(31) | BIT64(32) | \
		BIT64(33) | BIT64(34) | BIT64(35) | BIT64(36) | BIT64(37) | \
		BIT64(38) | BIT64(39) | BIT64(40) | BIT64(41));
    #endif

    return SUCCESS;
}

controller_error_code_e hci_le_set_advertising_parameters(_u8* data,_u8 length,bt_hci_event_t** event)
{
    
}
controller_error_code_e hci_le_set_advertising_data(_u8* data,_u8 length,bt_hci_event_t** event)
{
    
}
controller_error_code_e hci_le_set_scan_response_data(_u8* data,_u8 length,bt_hci_event_t** event)
{
    
}
controller_error_code_e hci_le_set_advertising_enable(_u8* data,_u8 length,bt_hci_event_t** event)
{
    
}


struct _PACKED le_set_event_mask_retParam_t
{
	_u8  status;
};
controller_error_code_e le_set_event_mask_process(_u8* data,_u8 length,bt_hci_event_t** event)
{
	_u64 eventMask = ((_u64)data[0])|((_u64)data[1]<<8)|((_u64)data[2]<<16)|((_u64)data[3]<<24)|(((_u64)data[4]<<32))|((_u64)data[5]<<40)|((_u64)data[6]<<48)|((_u64)data[7]<<56);
	controller_error_code_e status = ll_set_le_event_mask(eventMask);
	struct le_set_event_mask_retParam_t* retParam =\
	(struct le_set_event_mask_retParam_t*)hci_command_complete_event(hciCommandOpcode,sizeof(struct le_set_event_mask_retParam_t),event);
	retParam->status = status;
	return status;
}

struct _PACKED le_test_end_retParam_t
{
    _u8  status;
    _u16 numPackets;
};
controller_error_code_e le_test_end_process(_u8* data,_u8 length,bt_hci_event_t** event)
{
    struct le_test_end_retParam_t* param = \
    (struct le_test_end_retParam_t*)hci_command_complete_event(hciCommandOpcode,sizeof(struct le_test_end_retParam_t),event);
    param->status = (_u8)SUCCESS;
    param->numPackets = 0;
    return SUCCESS;
}

//special,should put it last
/****************************************hci informational parameters command**************************/
struct _PACKED read_br_addr_retParam_t
{
	_u8 status;
    _u8 addr[6];
};
controller_error_code_e read_bd_addr_process(_u8* data,_u8 length,bt_hci_event_t** event)
{
	ll_sm_t* ll = ll_get_current_state_machine();
	struct read_br_addr_retParam_t* retParam = \
	(struct read_br_addr_retParam_t*)hci_command_complete_event(hciCommandOpcode,sizeof(struct read_br_addr_retParam_t),event);
	retParam->status = (_u8)SUCCESS;
	txMemcpy(retParam->addr,ll_get_device_address(),6);
	return SUCCESS;
}

struct _PACKED read_local_supported_features_retParam_t
{
    _u8 status;
    _u8 lmpFeatures[8];
};
controller_error_code_e read_local_supported_features_process(_u8* data,_u8 length,bt_hci_event_t** event)
{
    struct read_local_supported_features_retParam_t* retParam = \
	(struct read_local_supported_features_retParam_t*)hci_command_complete_event(hciCommandOpcode,sizeof(struct read_local_supported_features_retParam_t),event);
    retParam->status = (_u8)SUCCESS;
    //LE Supported
    retParam->lmpFeatures[4] = (BIT(5)|BIT(6));
    return SUCCESS;
}

struct _PACKED read_local_version_information_retParam_t
{
    _u8  status;
    _u8  hciVersion;
    _u16 hciSubVesion;
    _u8  lmpVersion;
    _u16 commanyIdentifier;
    _u16 lmpSubVersion;
};
controller_error_code_e read_local_version_information_process(_u8* data,_u8 length,bt_hci_event_t** event)
{
    struct read_local_version_information_retParam_t* retParam = \
    (struct read_local_version_information_retParam_t*)hci_command_complete_event(hciCommandOpcode,sizeof(struct read_local_version_information_retParam_t),event);
    retParam->status            = (_u8)SUCCESS;
    retParam->hciVersion        = BT_VERSION;
    retParam->hciSubVesion      = HCI_SUB_VERSION;
    retParam->lmpVersion        = BT_VERSION;
    retParam->commanyIdentifier = COMPANY_IDENTIFIER;
    retParam->lmpSubVersion     = LMP_SUB_VERSION;
    return SUCCESS;
}

struct _PACKED read_local_supported_commands_retParam_t
{
    _u8 status;
    _u8 commands[64];
};
controller_error_code_e read_local_supported_commands_process(_u8* data,_u8 length,bt_hci_event_t** event)
{
    struct read_local_supported_commands_retParam_t* retParam = \
    (struct read_local_supported_commands_retParam_t*)hci_command_complete_event(hciCommandOpcode,sizeof(struct read_local_supported_commands_retParam_t),event);
    retParam->status = (_u8)SUCCESS;
    // Octet 0
    retParam->commands[0] |= (INQUIRY_PROCESS ? BIT(0) : 0);
    retParam->commands[0] |= (INQUIRY_CANCEL_PROCESS ? BIT(1) : 0);
    retParam->commands[0] |= (PERIODIC_INQUIRY_MODE_PROCESS ? BIT(2) : 0);
    retParam->commands[0] |= (EXIT_PERIODIC_INQUIRY_MODE_PROCESS ? BIT(3) : 0);
    retParam->commands[0] |= (CREATE_CONNECTION_PROCESS ? BIT(4) : 0);
    retParam->commands[0] |= (DISCONNECT_PROCESS ? BIT(5) : 0);
    // Octet 0 Bit 6: Previously used (跳过)
    retParam->commands[0] |= (CREATE_CONNECTION_CANCEL_PROCESS ? BIT(7) : 0);

    // Octet 1
    retParam->commands[1] |= (ACCEPT_CONNECTION_REQUEST_PROCESS ? BIT(0) : 0);
    retParam->commands[1] |= (REJECT_CONNECTION_REQUEST_PROCESS ? BIT(1) : 0);
    retParam->commands[1] |= (LINK_KEY_REQUEST_REPLY_PROCESS ? BIT(2) : 0);
    retParam->commands[1] |= (LINK_KEY_REQUEST_NEGATIVE_REPLY_PROCESS ? BIT(3) : 0);
    retParam->commands[1] |= (PIN_CODE_REQUEST_REPLY_PROCESS ? BIT(4) : 0);
    retParam->commands[1] |= (PIN_CODE_REQUEST_NEGATIVE_REPLY_PROCESS ? BIT(5) : 0);
    retParam->commands[1] |= (CHANGE_CONNECTION_PACKET_TYPE_PROCESS ? BIT(6) : 0);
    retParam->commands[1] |= (AUTHENTICATION_REQUESTED_PROCESS ? BIT(7) : 0);

    // Octet 2
    retParam->commands[2] |= (SET_CONNECTION_ENCRYPTION_PROCESS ? BIT(0) : 0);
    retParam->commands[2] |= (CHANGE_CONNECTION_LINK_KEY_PROCESS ? BIT(1) : 0);
    retParam->commands[2] |= (LINK_KEY_SELECTION_PROCESS ? BIT(2) : 0);
    retParam->commands[2] |= (REMOTE_NAME_REQUEST_PROCESS ? BIT(3) : 0);
    retParam->commands[2] |= (REMOTE_NAME_REQUEST_CANCEL_PROCESS ? BIT(4) : 0);
    retParam->commands[2] |= (READ_REMOTE_SUPPORTED_FEATURES_PROCESS ? BIT(5) : 0);
    retParam->commands[2] |= (READ_REMOTE_EXTENDED_FEATURES_PROCESS ? BIT(6) : 0);
    retParam->commands[2] |= (READ_REMOTE_VERSION_INFORMATION_PROCESS ? BIT(7) : 0);

    // Octet 3
    retParam->commands[3] |= (READ_CLOCK_OFFSET_PROCESS ? BIT(0) : 0);
    retParam->commands[3] |= (READ_LMP_HANDLE_PROCESS ? BIT(1) : 0);
    // Octet 3 Bits 2-4: Reserved for future use (跳过)
    // Octet 3 Bits 5-7: Reserved for future use (跳过)

    // Octet 4
    // Octet 4 Bit 0: Reserved for future use (跳过)
    retParam->commands[4] |= (HOLD_MODE_PROCESS ? BIT(1) : 0);
    retParam->commands[4] |= (SNIFF_MODE_PROCESS ? BIT(2) : 0);
    retParam->commands[4] |= (EXIT_SNIFF_MODE_PROCESS ? BIT(3) : 0);
    // Octet 4 Bits 4-5: Previously used (跳过)
    retParam->commands[4] |= (QOS_SETUP_PROCESS ? BIT(6) : 0);
    retParam->commands[4] |= (ROLE_DISCOVERY_PROCESS ? BIT(7) : 0);

    // Octet 5
    retParam->commands[5] |= (SWITCH_ROLE_PROCESS ? BIT(0) : 0);
    retParam->commands[5] |= (READ_LINK_POLICY_SETTINGS_PROCESS ? BIT(1) : 0);
    retParam->commands[5] |= (WRITE_LINK_POLICY_SETTINGS_PROCESS ? BIT(2) : 0);
    retParam->commands[5] |= (READ_DEFAULT_LINK_POLICY_SETTINGS_PROCESS ? BIT(3) : 0);
    retParam->commands[5] |= (WRITE_DEFAULT_LINK_POLICY_SETTINGS_PROCESS ? BIT(4) : 0);
    retParam->commands[5] |= (FLOW_SPECIFICATION_PROCESS ? BIT(5) : 0);
    retParam->commands[5] |= (SET_EVENT_MASK_PROCESS ? BIT(6) : 0);
    retParam->commands[5] |= (RESET_PROCESS ? BIT(7) : 0);

    // Octet 6
    retParam->commands[6] |= (SET_EVENT_FILTER_PROCESS ? BIT(0) : 0);
    retParam->commands[6] |= (FLUSH_PROCESS ? BIT(1) : 0);
    retParam->commands[6] |= (READ_PIN_TYPE_PROCESS ? BIT(2) : 0);
    retParam->commands[6] |= (WRITE_PIN_TYPE_PROCESS ? BIT(3) : 0);
    // Octet 6 Bit 4: Previously used (跳过)
    retParam->commands[6] |= (READ_STORED_LINK_KEY_PROCESS ? BIT(5) : 0);
    retParam->commands[6] |= (WRITE_STORED_LINK_KEY_PROCESS ? BIT(6) : 0);
    retParam->commands[6] |= (DELETE_STORED_LINK_KEY_PROCESS ? BIT(7) : 0);

    // Octet 7
    retParam->commands[7] |= (WRITE_LOCAL_NAME_PROCESS ? BIT(0) : 0);
    retParam->commands[7] |= (READ_LOCAL_NAME_PROCESS ? BIT(1) : 0);
    retParam->commands[7] |= (READ_CONNECTION_ACCEPT_TIMEOUT_PROCESS ? BIT(2) : 0);
    retParam->commands[7] |= (WRITE_CONNECTION_ACCEPT_TIMEOUT_PROCESS ? BIT(3) : 0);
    retParam->commands[7] |= (READ_PAGE_TIMEOUT_PROCESS ? BIT(4) : 0);
    retParam->commands[7] |= (WRITE_PAGE_TIMEOUT_PROCESS ? BIT(5) : 0);
    retParam->commands[7] |= (READ_SCAN_ENABLE_PROCESS ? BIT(6) : 0);
    retParam->commands[7] |= (WRITE_SCAN_ENABLE_PROCESS ? BIT(7) : 0);

    // Octet 8
    retParam->commands[8] |= (READ_PAGE_SCAN_ACTIVITY_PROCESS ? BIT(0) : 0);
    retParam->commands[8] |= (WRITE_PAGE_SCAN_ACTIVITY_PROCESS ? BIT(1) : 0);
    retParam->commands[8] |= (READ_INQUIRY_SCAN_ACTIVITY_PROCESS ? BIT(2) : 0);
    retParam->commands[8] |= (WRITE_INQUIRY_SCAN_ACTIVITY_PROCESS ? BIT(3) : 0);
    retParam->commands[8] |= (READ_AUTHENTICATION_ENABLE_PROCESS ? BIT(4) : 0);
    retParam->commands[8] |= (WRITE_AUTHENTICATION_ENABLE_PROCESS ? BIT(5) : 0);
    // Octet 8 Bits 6-7: Previously used (跳过)

    // Octet 9
    retParam->commands[9] |= (READ_CLASS_OF_DEVICE_PROCESS ? BIT(0) : 0);
    retParam->commands[9] |= (WRITE_CLASS_OF_DEVICE_PROCESS ? BIT(1) : 0);
    retParam->commands[9] |= (READ_VOICE_SETTING_PROCESS ? BIT(2) : 0);
    retParam->commands[9] |= (WRITE_VOICE_SETTING_PROCESS ? BIT(3) : 0);
    retParam->commands[9] |= (READ_AUTOMATIC_FLUSH_TIMEOUT_PROCESS ? BIT(4) : 0);
    retParam->commands[9] |= (WRITE_AUTOMATIC_FLUSH_TIMEOUT_PROCESS ? BIT(5) : 0);
    retParam->commands[9] |= (READ_NUM_BROADCAST_RETRANSMISSIONS_PROCESS ? BIT(6) : 0);
    retParam->commands[9] |= (WRITE_NUM_BROADCAST_RETRANSMISSIONS_PROCESS ? BIT(7) : 0);

    // Octet 10
    retParam->commands[10] |= (READ_HOLD_MODE_ACTIVITY_PROCESS ? BIT(0) : 0);
    retParam->commands[10] |= (WRITE_HOLD_MODE_ACTIVITY_PROCESS ? BIT(1) : 0);
    retParam->commands[10] |= (READ_TRANSMIT_POWER_LEVEL_PROCESS ? BIT(2) : 0);
    retParam->commands[10] |= (READ_SYNCHRONOUS_FLOW_CONTROL_ENABLE_PROCESS ? BIT(3) : 0);
    retParam->commands[10] |= (WRITE_SYNCHRONOUS_FLOW_CONTROL_ENABLE_PROCESS ? BIT(4) : 0);
    retParam->commands[10] |= (SET_CONTROLLER_TO_HOST_FLOW_CONTROL_PROCESS ? BIT(5) : 0);
    retParam->commands[10] |= (HOST_BUFFER_SIZE_PROCESS ? BIT(6) : 0);
    retParam->commands[10] |= (HOST_NUMBER_OF_COMPLETED_PACKETS_PROCESS ? BIT(7) : 0);

    // Octet 11
    retParam->commands[11] |= (READ_LINK_SUPERVISION_TIMEOUT_PROCESS ? BIT(0) : 0);
    retParam->commands[11] |= (WRITE_LINK_SUPERVISION_TIMEOUT_PROCESS ? BIT(1) : 0);
    retParam->commands[11] |= (READ_NUMBER_OF_SUPPORTED_IAC_PROCESS ? BIT(2) : 0);
    retParam->commands[11] |= (READ_CURRENT_IAC_LAP_PROCESS ? BIT(3) : 0);
    retParam->commands[11] |= (WRITE_CURRENT_IAC_LAP_PROCESS ? BIT(4) : 0);
    // Octet 11 Bits 5-6: Previously used (跳过)
    // Octet 11 Bit 7: Previously used (跳过)

    // Octet 12
    // Octet 12 Bit 0: Previously used (跳过)
    retParam->commands[12] |= (SET_AFH_HOST_CHANNEL_CLASSIFICATION_PROCESS ? BIT(1) : 0);
    retParam->commands[12] |= (LE_CS_READ_REMOTE_FAE_TABLE_PROCESS ? BIT(2) : 0);
    retParam->commands[12] |= (LE_CS_WRITE_CACHED_REMOTE_FAE_TABLE_PROCESS ? BIT(3) : 0);
    retParam->commands[12] |= (READ_INQUIRY_SCAN_TYPE_PROCESS ? BIT(4) : 0);
    retParam->commands[12] |= (WRITE_INQUIRY_SCAN_TYPE_PROCESS ? BIT(5) : 0);
    retParam->commands[12] |= (READ_INQUIRY_MODE_PROCESS ? BIT(6) : 0);
    retParam->commands[12] |= (WRITE_INQUIRY_MODE_PROCESS ? BIT(7) : 0);

    // Octet 13
    retParam->commands[13] |= (READ_PAGE_SCAN_TYPE_PROCESS ? BIT(0) : 0);
    retParam->commands[13] |= (WRITE_PAGE_SCAN_TYPE_PROCESS ? BIT(1) : 0);
    retParam->commands[13] |= (READ_AFH_CHANNEL_ASSESSMENT_MODE_PROCESS ? BIT(2) : 0);
    retParam->commands[13] |= (WRITE_AFH_CHANNEL_ASSESSMENT_MODE_PROCESS ? BIT(3) : 0);
    // Octet 13 Bits 4-7: Reserved for future use (跳过)

    // Octet 14
    // Octet 14 Bits 0-2: Reserved for future use (跳过)
    retParam->commands[14] |= (READ_LOCAL_VERSION_INFORMATION_PROCESS ? BIT(3) : 0);
    // Octet 14 Bit 4: Reserved for future use (跳过)
    retParam->commands[14] |= (READ_LOCAL_SUPPORTED_FEATURES_PROCESS ? BIT(5) : 0);
    retParam->commands[14] |= (READ_LOCAL_EXTENDED_FEATURES_PROCESS ? BIT(6) : 0);
    retParam->commands[14] |= (READ_BUFFER_SIZE_PROCESS ? BIT(7) : 0);

    // Octet 15
    // Octet 15 Bit 0: Previously used (跳过)
    retParam->commands[15] |= (READ_BD_ADDR_PROCESS ? BIT(1) : 0);
    retParam->commands[15] |= (READ_FAILED_CONTACT_COUNTER_PROCESS ? BIT(2) : 0);
    retParam->commands[15] |= (RESET_FAILED_CONTACT_COUNTER_PROCESS ? BIT(3) : 0);
    retParam->commands[15] |= (READ_LINK_QUALITY_PROCESS ? BIT(4) : 0);
    retParam->commands[15] |= (READ_RSSI_PROCESS ? BIT(5) : 0);
    retParam->commands[15] |= (READ_AFH_CHANNEL_MAP_PROCESS ? BIT(6) : 0);
    retParam->commands[15] |= (READ_CLOCK_PROCESS ? BIT(7) : 0);

    // Octet 16
    retParam->commands[16] |= (READ_LOOPBACK_MODE_PROCESS ? BIT(0) : 0);
    retParam->commands[16] |= (WRITE_LOOPBACK_MODE_PROCESS ? BIT(1) : 0);
    retParam->commands[16] |= (ENABLE_IMPLEMENTATION_UNDER_TEST_MODE_PROCESS ? BIT(2) : 0);
    retParam->commands[16] |= (SETUP_SYNCHRONOUS_CONNECTION_PROCESS ? BIT(3) : 0);
    retParam->commands[16] |= (ACCEPT_SYNCHRONOUS_CONNECTION_REQUEST_PROCESS ? BIT(4) : 0);
    retParam->commands[16] |= (REJECT_SYNCHRONOUS_CONNECTION_REQUEST_PROCESS ? BIT(5) : 0);
    retParam->commands[16] |= (LE_CS_CREATE_CONFIG_PROCESS ? BIT(6) : 0);
    retParam->commands[16] |= (LE_CS_REMOVE_CONFIG_PROCESS ? BIT(7) : 0);

    // Octet 17
    retParam->commands[17] |= (READ_EXTENDED_INQUIRY_RESPONSE_PROCESS ? BIT(0) : 0);
    retParam->commands[17] |= (WRITE_EXTENDED_INQUIRY_RESPONSE_PROCESS ? BIT(1) : 0);
    retParam->commands[17] |= (REFRESH_ENCRYPTION_KEY_PROCESS ? BIT(2) : 0);
    // Octet 17 Bit 3: Reserved for future use (跳过)
    retParam->commands[17] |= (SNIFF_SUBRATING_PROCESS ? BIT(4) : 0);
    retParam->commands[17] |= (READ_SIMPLE_PAIRING_MODE_PROCESS ? BIT(5) : 0);
    retParam->commands[17] |= (WRITE_SIMPLE_PAIRING_MODE_PROCESS ? BIT(6) : 0);
    retParam->commands[17] |= (READ_LOCAL_OOB_DATA_PROCESS ? BIT(7) : 0);
  
    // Octet 18
    retParam->commands[18] |= (READ_INQUIRY_RESPONSE_TRANSMIT_POWER_LEVEL_PROCESS ? BIT(0) : 0);
    retParam->commands[18] |= (WRITE_INQUIRY_TRANSMIT_POWER_LEVEL_PROCESS ? BIT(1) : 0);
    retParam->commands[18] |= (READ_DEFAULT_ERRONEOUS_DATA_REPORTING_PROCESS ? BIT(2) : 0);
    retParam->commands[18] |= (WRITE_DEFAULT_ERRONEOUS_DATA_REPORTING_PROCESS ? BIT(3) : 0);
    retParam->commands[18] |= (IO_CAPABILITY_REQUEST_REPLY_PROCESS ? BIT(7) : 0);
    
    // Octet 19
    retParam->commands[19] |= (USER_CONFIRMATION_REQUEST_REPLY_PROCESS ? BIT(0) : 0);
    retParam->commands[19] |= (USER_CONFIRMATION_REQUEST_NEGATIVE_REPLY_PROCESS ? BIT(1) : 0);
    retParam->commands[19] |= (USER_PASSKEY_REQUEST_REPLY_PROCESS ? BIT(2) : 0);
    retParam->commands[19] |= (USER_PASSKEY_REQUEST_NEGATIVE_REPLY_PROCESS ? BIT(3) : 0);
    retParam->commands[19] |= (REMOTE_OOB_DATA_REQUEST_REPLY_PROCESS ? BIT(4) : 0);
    retParam->commands[19] |= (WRITE_SIMPLE_PAIRING_DEBUG_MODE_PROCESS ? BIT(5) : 0);
    retParam->commands[19] |= (ENHANCED_FLUSH_PROCESS ? BIT(6) : 0);
    retParam->commands[19] |= (REMOTE_OOB_DATA_REQUEST_NEGATIVE_REPLY_PROCESS ? BIT(7) : 0);
    
    // Octet 20
    retParam->commands[20] |= (SEND_KEYPRESS_NOTIFICATION_PROCESS ? BIT(2) : 0);
    retParam->commands[20] |= (IO_CAPABILITY_REQUEST_NEGATIVE_REPLY_PROCESS ? BIT(3) : 0);
    retParam->commands[20] |= (READ_ENCRYPTION_KEY_SIZE_PROCESS ? BIT(4) : 0);
    retParam->commands[20] |= (LE_CS_READ_LOCAL_SUPPORTED_CAPABILITIES_PROCESS ? BIT(5) : 0);
    retParam->commands[20] |= (LE_CS_READ_REMOTE_SUPPORTED_CAPABILITIES_PROCESS ? BIT(6) : 0);
    retParam->commands[20] |= (LE_CS_WRITE_CACHED_REMOTE_SUPPORTED_CAPABILITIES_PROCESS ? BIT(7) : 0);
    
    // Octet 22
    retParam->commands[22] |= (SET_EVENT_MASK_PAGE_2_PROCESS ? BIT(2) : 0);
    
    // Octet 23
    retParam->commands[23] |= (READ_FLOW_CONTROL_MODE_PROCESS ? BIT(0) : 0);
    retParam->commands[23] |= (WRITE_FLOW_CONTROL_MODE_PROCESS ? BIT(1) : 0);
    retParam->commands[23] |= (READ_DATA_BLOCK_SIZE_PROCESS ? BIT(2) : 0);
    retParam->commands[23] |= (LE_CS_TEST_PROCESS ? BIT(3) : 0);
    retParam->commands[23] |= (LE_CS_TEST_END_PROCESS ? BIT(4) : 0);
    
    // Octet 24
    retParam->commands[24] |= (READ_ENHANCED_TRANSMIT_POWER_LEVEL_PROCESS ? BIT(0) : 0);
    retParam->commands[24] |= (LE_CS_SECURITY_ENABLE_PROCESS ? BIT(1) : 0);
    retParam->commands[24] |= (READ_LE_HOST_SUPPORT_PROCESS ? BIT(5) : 0);
    retParam->commands[24] |= (WRITE_LE_HOST_SUPPORT_PROCESS ? BIT(6) : 0);
    retParam->commands[24] |= (LE_CS_SET_DEFAULT_SETTINGS_PROCESS ? BIT(7) : 0);
    
    // Octet 25
    retParam->commands[25] |= (LE_SET_EVENT_MASK_PROCESS ? BIT(0) : 0);
    retParam->commands[25] |= (LE_READ_BUFFER_SIZE_PROCESS ? BIT(1) : 0);
    retParam->commands[25] |= (LE_READ_LOCAL_SUPPORTED_FEATURES_PAGE_0_PROCESS ? BIT(2) : 0);
    retParam->commands[25] |= (LE_SET_RANDOM_ADDRESS_PROCESS ? BIT(4) : 0);
    retParam->commands[25] |= (LE_SET_ADVERTISING_PARAMETERS_PROCESS ? BIT(5) : 0);
    retParam->commands[25] |= (LE_READ_ADVERTISING_PHYSICAL_CHANNEL_TX_POWER_PROCESS ? BIT(6) : 0);
    retParam->commands[25] |= (LE_SET_ADVERTISING_DATA_PROCESS ? BIT(7) : 0);
    
    // Octet 26
    retParam->commands[26] |= (LE_SET_SCAN_RESPONSE_DATA_PROCESS ? BIT(0) : 0);
    retParam->commands[26] |= (LE_SET_ADVERTISING_ENABLE_PROCESS ? BIT(1) : 0);
    retParam->commands[26] |= (LE_SET_SCAN_PARAMETERS_PROCESS ? BIT(2) : 0);
    retParam->commands[26] |= (LE_SET_SCAN_ENABLE_PROCESS ? BIT(3) : 0);
    retParam->commands[26] |= (LE_CREATE_CONNECTION_PROCESS ? BIT(4) : 0);
    retParam->commands[26] |= (LE_CREATE_CONNECTION_CANCEL_PROCESS ? BIT(5) : 0);
    retParam->commands[26] |= (LE_READ_FILTER_ACCEPT_LIST_SIZE_PROCESS ? BIT(6) : 0);
    retParam->commands[26] |= (LE_CLEAR_FILTER_ACCEPT_LIST_PROCESS ? BIT(7) : 0);
    
    // Octet 27
    retParam->commands[27] |= (LE_ADD_DEVICE_TO_FILTER_ACCEPT_LIST_PROCESS ? BIT(0) : 0);
    retParam->commands[27] |= (LE_REMOVE_DEVICE_FROM_FILTER_ACCEPT_LIST_PROCESS ? BIT(1) : 0);
    retParam->commands[27] |= (LE_CONNECTION_UPDATE_PROCESS ? BIT(2) : 0);
    retParam->commands[27] |= (LE_SET_HOST_CHANNEL_CLASSIFICATION_PROCESS ? BIT(3) : 0);
    retParam->commands[27] |= (LE_READ_CHANNEL_MAP_PROCESS ? BIT(4) : 0);
    retParam->commands[27] |= (LE_READ_REMOTE_FEATURES_PAGE_0_PROCESS ? BIT(5) : 0);
    retParam->commands[27] |= (LE_ENCRYPT_PROCESS ? BIT(6) : 0);
    retParam->commands[27] |= (LE_RAND_PROCESS ? BIT(7) : 0);
    
    // Octet 28
    retParam->commands[28] |= (LE_ENABLE_ENCRYPTION_PROCESS ? BIT(0) : 0);
    retParam->commands[28] |= (LE_LONG_TERM_KEY_REQUEST_REPLY_PROCESS ? BIT(1) : 0);
    retParam->commands[28] |= (LE_LONG_TERM_KEY_REQUEST_NEGATIVE_REPLY_PROCESS ? BIT(2) : 0);
    retParam->commands[28] |= (LE_READ_SUPPORTED_STATES_PROCESS ? BIT(3) : 0);
    retParam->commands[28] |= (LE_RECEIVER_TEST_PROCESS ? BIT(4) : 0);
    retParam->commands[28] |= (LE_TRANSMITTER_TEST_PROCESS ? BIT(5) : 0);
    retParam->commands[28] |= (LE_TEST_END_PROCESS ? BIT(6) : 0);
    retParam->commands[28] |= (LE_ENABLE_MONITORING_ADVERTISERS_PROCESS ? BIT(7) : 0);
    
    // Octet 29
    retParam->commands[29] |= (LE_CS_SET_CHANNEL_CLASSIFICATION_PROCESS ? BIT(0) : 0);
    retParam->commands[29] |= (LE_CS_SET_PROCEDURE_PARAMETERS_PROCESS ? BIT(1) : 0);
    retParam->commands[29] |= (LE_CS_PROCEDURE_ENABLE_PROCESS ? BIT(2) : 0);
    retParam->commands[29] |= (ENHANCED_SETUP_SYNCHRONOUS_CONNECTION_PROCESS ? BIT(3) : 0);
    retParam->commands[29] |= (ENHANCED_ACCEPT_SYNCHRONOUS_CONNECTION_REQUEST_PROCESS ? BIT(4) : 0);
    retParam->commands[29] |= (READ_LOCAL_SUPPORTED_CODECS_PROCESS ? BIT(5) : 0);
    retParam->commands[29] |= (SET_MWS_CHANNEL_PARAMETERS_PROCESS ? BIT(6) : 0);
    retParam->commands[29] |= (SET_EXTERNAL_FRAME_CONFIGURATION_PROCESS ? BIT(7) : 0);
    
    // Octet 30
    retParam->commands[30] |= (SET_MWS_SIGNALING_PROCESS ? BIT(0) : 0);
    retParam->commands[30] |= (SET_MWS_TRANSPORT_LAYER_PROCESS ? BIT(1) : 0);
    retParam->commands[30] |= (SET_MWS_SCAN_FREQUENCY_TABLE_PROCESS ? BIT(2) : 0);
    retParam->commands[30] |= (GET_MWS_TRANSPORT_LAYER_CONFIGURATION_PROCESS ? BIT(3) : 0);
    retParam->commands[30] |= (SET_MWS_PATTERN_CONFIGURATION_PROCESS ? BIT(4) : 0);
    retParam->commands[30] |= (SET_TRIGGERED_CLOCK_CAPTURE_PROCESS ? BIT(5) : 0);
    retParam->commands[30] |= (TRUNCATED_PAGE_PROCESS ? BIT(6) : 0);
    retParam->commands[30] |= (TRUNCATED_PAGE_CANCEL_PROCESS ? BIT(7) : 0);
    
    // Octet 31
    retParam->commands[31] |= (SET_CONNECTIONLESS_PERIPHERAL_BROADCAST_PROCESS ? BIT(0) : 0);
    retParam->commands[31] |= (SET_CONNECTIONLESS_PERIPHERAL_BROADCAST_RECEIVE_PROCESS ? BIT(1) : 0);
    retParam->commands[31] |= (START_SYNCHRONIZATION_TRAIN_PROCESS ? BIT(2) : 0);
    retParam->commands[31] |= (RECEIVE_SYNCHRONIZATION_TRAIN_PROCESS ? BIT(3) : 0);
    retParam->commands[31] |= (SET_RESERVED_LT_ADDR_PROCESS ? BIT(4) : 0);
    retParam->commands[31] |= (DELETE_RESERVED_LT_ADDR_PROCESS ? BIT(5) : 0);
    retParam->commands[31] |= (SET_CONNECTIONLESS_PERIPHERAL_BROADCAST_DATA_PROCESS ? BIT(6) : 0);
    retParam->commands[31] |= (READ_SYNCHRONIZATION_TRAIN_PARAMETERS_PROCESS ? BIT(7) : 0);
    
    // Octet 32
    retParam->commands[32] |= (WRITE_SYNCHRONIZATION_TRAIN_PARAMETERS_PROCESS ? BIT(0) : 0);
    retParam->commands[32] |= (REMOTE_OOB_EXTENDED_DATA_REQUEST_REPLY_PROCESS ? BIT(1) : 0);
    retParam->commands[32] |= (READ_SECURE_CONNECTIONS_HOST_SUPPORT_PROCESS ? BIT(2) : 0);
    retParam->commands[32] |= (WRITE_SECURE_CONNECTIONS_HOST_SUPPORT_PROCESS ? BIT(3) : 0);
    retParam->commands[32] |= (READ_AUTHENTICATED_PAYLOAD_TIMEOUT_PROCESS ? BIT(4) : 0);
    retParam->commands[32] |= (WRITE_AUTHENTICATED_PAYLOAD_TIMEOUT_PROCESS ? BIT(5) : 0);
    retParam->commands[32] |= (READ_LOCAL_OOB_EXTENDED_DATA_PROCESS ? BIT(6) : 0);
    retParam->commands[32] |= (WRITE_SECURE_CONNECTIONS_TEST_MODE_PROCESS ? BIT(7) : 0);
    
    // Octet 33
    retParam->commands[33] |= (READ_EXTENDED_PAGE_TIMEOUT_PROCESS ? BIT(0) : 0);
    retParam->commands[33] |= (WRITE_EXTENDED_PAGE_TIMEOUT_PROCESS ? BIT(1) : 0);
    retParam->commands[33] |= (READ_EXTENDED_INQUIRY_LENGTH_PROCESS ? BIT(2) : 0);
    retParam->commands[33] |= (WRITE_EXTENDED_INQUIRY_LENGTH_PROCESS ? BIT(3) : 0);
    retParam->commands[33] |= (LE_REMOTE_CONNECTION_PARAMETER_REQUEST_REPLY_PROCESS ? BIT(4) : 0);
    retParam->commands[33] |= (LE_REMOTE_CONNECTION_PARAMETER_REQUEST_NEGATIVE_REPLY_PROCESS ? BIT(5) : 0);
    retParam->commands[33] |= (LE_SET_DATA_LENGTH_PROCESS ? BIT(6) : 0);
    retParam->commands[33] |= (LE_READ_SUGGESTED_DEFAULT_DATA_LENGTH_PROCESS ? BIT(7) : 0);
    
    // Octet 34
    retParam->commands[34] |= (LE_WRITE_SUGGESTED_DEFAULT_DATA_LENGTH_PROCESS ? BIT(0) : 0);
    retParam->commands[34] |= (LE_READ_LOCAL_P_256_PUBLIC_KEY_PROCESS ? BIT(1) : 0);
    retParam->commands[34] |= (LE_GENERATE_DHKEY_PROCESS ? BIT(2) : 0);
    retParam->commands[34] |= (LE_ADD_DEVICE_TO_RESOLVING_LIST_PROCESS ? BIT(3) : 0);
    retParam->commands[34] |= (LE_REMOVE_DEVICE_FROM_RESOLVING_LIST_PROCESS ? BIT(4) : 0);
    retParam->commands[34] |= (LE_CLEAR_RESOLVING_LIST_PROCESS ? BIT(5) : 0);
    retParam->commands[34] |= (LE_READ_RESOLVING_LIST_SIZE_PROCESS ? BIT(6) : 0);
    retParam->commands[34] |= (LE_READ_PEER_RESOLVABLE_ADDRESS_PROCESS ? BIT(7) : 0);
    
    // Octet 35
    retParam->commands[35] |= (LE_READ_LOCAL_RESOLVABLE_ADDRESS_PROCESS ? BIT(0) : 0);
    retParam->commands[35] |= (LE_SET_ADDRESS_RESOLUTION_ENABLE_PROCESS ? BIT(1) : 0);
    retParam->commands[35] |= (LE_SET_RESOLVABLE_PRIVATE_ADDRESS_TIMEOUT_PROCESS ? BIT(2) : 0);
    retParam->commands[35] |= (LE_READ_MAXIMUM_DATA_LENGTH_PROCESS ? BIT(3) : 0);
    retParam->commands[35] |= (LE_READ_PHY_PROCESS ? BIT(4) : 0);
    retParam->commands[35] |= (LE_SET_DEFAULT_PHY_PROCESS ? BIT(5) : 0);
    retParam->commands[35] |= (LE_SET_PHY_PROCESS ? BIT(6) : 0);
    retParam->commands[35] |= (LE_RECEIVER_TEST_PROCESS ? BIT(7) : 0);

    // Octet 36
    retParam->commands[36] |= (LE_TRANSMITTER_TEST_V2_PROCESS ? BIT(0) : 0);
    retParam->commands[36] |= (LE_SET_ADVERTISING_SET_RANDOM_ADDRESS_PROCESS ? BIT(1) : 0);
    retParam->commands[36] |= (LE_SET_EXTENDED_ADVERTISING_PARAMETERS_PROCESS ? BIT(2) : 0);
    retParam->commands[36] |= (LE_SET_EXTENDED_ADVERTISING_DATA_PROCESS ? BIT(3) : 0);
    retParam->commands[36] |= (LE_SET_EXTENDED_SCAN_RESPONSE_DATA_PROCESS ? BIT(4) : 0);
    retParam->commands[36] |= (LE_SET_EXTENDED_ADVERTISING_ENABLE_PROCESS ? BIT(5) : 0);
    retParam->commands[36] |= (LE_READ_MAXIMUM_ADVERTISING_DATA_LENGTH_PROCESS ? BIT(6) : 0);
    retParam->commands[36] |= (LE_READ_NUMBER_OF_SUPPORTED_ADVERTISING_SETS_PROCESS ? BIT(7) : 0);
    
    // Octet 37
    retParam->commands[37] |= (LE_REMOVE_ADVERTISING_SET_PROCESS ? BIT(0) : 0);
    retParam->commands[37] |= (LE_CLEAR_ADVERTISING_SETS_PROCESS ? BIT(1) : 0);
    retParam->commands[37] |= (LE_SET_PERIODIC_ADVERTISING_PARAMETERS_PROCESS ? BIT(2) : 0);
    retParam->commands[37] |= (LE_SET_PERIODIC_ADVERTISING_DATA_PROCESS ? BIT(3) : 0);
    retParam->commands[37] |= (LE_SET_PERIODIC_ADVERTISING_ENABLE_PROCESS ? BIT(4) : 0);
    retParam->commands[37] |= (LE_SET_EXTENDED_SCAN_PARAMETERS_PROCESS ? BIT(5) : 0);
    retParam->commands[37] |= (LE_SET_EXTENDED_SCAN_ENABLE_PROCESS ? BIT(6) : 0);
    retParam->commands[37] |= (LE_EXTENDED_CREATE_CONNECTION_PROCESS ? BIT(7) : 0);
    
    // Octet 38
    retParam->commands[38] |= (LE_PERIODIC_ADVERTISING_CREATE_SYNC_PROCESS ? BIT(0) : 0);
    retParam->commands[38] |= (LE_PERIODIC_ADVERTISING_CREATE_SYNC_CANCEL_PROCESS ? BIT(1) : 0);
    retParam->commands[38] |= (LE_PERIODIC_ADVERTISING_TERMINATE_SYNC_PROCESS ? BIT(2) : 0);
    retParam->commands[38] |= (LE_ADD_DEVICE_TO_PERIODIC_ADVERTISER_LIST_PROCESS ? BIT(3) : 0);
    retParam->commands[38] |= (LE_REMOVE_DEVICE_FROM_PERIODIC_ADVERTISER_LIST_PROCESS ? BIT(4) : 0);
    retParam->commands[38] |= (LE_CLEAR_PERIODIC_ADVERTISER_LIST_PROCESS ? BIT(5) : 0);
    retParam->commands[38] |= (LE_READ_PERIODIC_ADVERTISER_LIST_SIZE_PROCESS ? BIT(6) : 0);
    retParam->commands[38] |= (LE_READ_TRANSMIT_POWER_PROCESS ? BIT(7) : 0);
    
    // Octet 39
    retParam->commands[39] |= (LE_READ_RF_PATH_COMPENSATION_PROCESS ? BIT(0) : 0);
    retParam->commands[39] |= (LE_WRITE_RF_PATH_COMPENSATION_PROCESS ? BIT(1) : 0);
    retParam->commands[39] |= (LE_SET_PRIVACY_MODE_PROCESS ? BIT(2) : 0);
    retParam->commands[39] |= (LE_RECEIVER_TEST_V3_PROCESS ? BIT(3) : 0);
    retParam->commands[39] |= (LE_TRANSMITTER_TEST_V3_PROCESS ? BIT(4) : 0);
    retParam->commands[39] |= (LE_SET_CONNECTIONLESS_CTE_TRANSMIT_PARAMETERS_PROCESS ? BIT(5) : 0);
    retParam->commands[39] |= (LE_SET_CONNECTIONLESS_CTE_TRANSMIT_ENABLE_PROCESS ? BIT(6) : 0);
    retParam->commands[39] |= (LE_SET_CONNECTIONLESS_IQ_SAMPLING_ENABLE_PROCESS ? BIT(7) : 0);
    
    // Octet 40
    retParam->commands[40] |= (LE_SET_CONNECTION_CTE_RECEIVE_PARAMETERS_PROCESS ? BIT(0) : 0);
    retParam->commands[40] |= (LE_SET_CONNECTION_CTE_TRANSMIT_PARAMETERS_PROCESS ? BIT(1) : 0);
    retParam->commands[40] |= (LE_CONNECTION_CTE_REQUEST_ENABLE_PROCESS ? BIT(2) : 0);
    
    // Octet 41
    retParam->commands[41] |= (LE_SET_PERIODIC_ADVERTISING_RECEIVE_ENABLE_PROCESS ? BIT(5) : 0);
    retParam->commands[41] |= (LE_PERIODIC_ADVERTISING_SYNC_TRANSFER_PROCESS ? BIT(6) : 0);
    retParam->commands[41] |= (LE_PERIODIC_ADVERTISING_SET_INFO_TRANSFER_PROCESS ? BIT(7) : 0);
    retParam->commands[41] |= (LE_SET_PERIODIC_ADVERTISING_SYNC_TRANSFER_PARAMETERS_PROCESS ? BIT(0) : 0);
    retParam->commands[41] |= (LE_SET_DEFAULT_PERIODIC_ADVERTISING_SYNC_TRANSFER_PARAMETERS_PROCESS ? BIT(1) : 0);
    
    // Octet 42
    retParam->commands[42] |= (LE_GENERATE_DHKEY_V2_PROCESS ? BIT(2) : 0);
    retParam->commands[42] |= (READ_LOCAL_SIMPLE_PAIRING_OPTIONS_PROCESS ? BIT(3) : 0);
    retParam->commands[42] |= (LE_MODIFY_SLEEP_CLOCK_ACCURACY_PROCESS ? BIT(4) : 0);
    retParam->commands[42] |= (LE_READ_BUFFER_SIZE_V2_PROCESS ? BIT(5) : 0);
    retParam->commands[42] |= (LE_READ_ISO_TX_SYNC_PROCESS ? BIT(6) : 0);
    retParam->commands[42] |= (LE_SET_CIG_PARAMETERS_PROCESS ? BIT(7) : 0);
    
    // Octet 43
    retParam->commands[43] |= (LE_SET_CIG_PARAMETERS_TEST_PROCESS ? BIT(0) : 0);
    retParam->commands[43] |= (LE_CREATE_CIS_PROCESS ? BIT(1) : 0);
    retParam->commands[43] |= (LE_REMOVE_CIG_PROCESS ? BIT(2) : 0);
    retParam->commands[43] |= (LE_ACCEPT_CIS_REQUEST_PROCESS ? BIT(3) : 0);
    retParam->commands[43] |= (LE_REJECT_CIS_REQUEST_PROCESS ? BIT(4) : 0);
    retParam->commands[43] |= (LE_CREATE_BIG_PROCESS ? BIT(5) : 0);
    retParam->commands[43] |= (LE_CREATE_BIG_TEST_PROCESS ? BIT(6) : 0);
    retParam->commands[43] |= (LE_TERMINATE_BIG_PROCESS ? BIT(7) : 0);
    
    // Octet 44
    retParam->commands[44] |= (LE_BIG_CREATE_SYNC_PROCESS ? BIT(0) : 0);
    retParam->commands[44] |= (LE_BIG_TERMINATE_SYNC_PROCESS ? BIT(1) : 0);
    retParam->commands[44] |= (LE_REQUEST_PEER_SCA_PROCESS ? BIT(2) : 0);
    retParam->commands[44] |= (LE_SETUP_ISO_DATA_PATH_PROCESS ? BIT(3) : 0);
    retParam->commands[44] |= (LE_REMOVE_ISO_DATA_PATH_PROCESS ? BIT(4) : 0);
    retParam->commands[44] |= (LE_ISO_TRANSMIT_TEST_PROCESS ? BIT(5) : 0);
    retParam->commands[44] |= (LE_ISO_RECEIVE_TEST_PROCESS ? BIT(6) : 0);
    retParam->commands[44] |= (LE_ISO_READ_TEST_COUNTERS_PROCESS ? BIT(7) : 0);
    
    // Octet 45
    retParam->commands[45] |= (LE_ISO_TEST_END_PROCESS ? BIT(0) : 0);
    retParam->commands[45] |= (LE_SET_HOST_FEATURE_PROCESS ? BIT(1) : 0);
    retParam->commands[45] |= (LE_READ_ISO_LINK_QUALITY_PROCESS ? BIT(2) : 0);
    retParam->commands[45] |= (LE_ENHANCED_READ_TRANSMIT_POWER_LEVEL_PROCESS ? BIT(3) : 0);
    retParam->commands[45] |= (LE_READ_REMOTE_TRANSMIT_POWER_LEVEL_PROCESS ? BIT(4) : 0);
    retParam->commands[45] |= (LE_SET_PATH_LOSS_REPORTING_PARAMETERS_PROCESS ? BIT(5) : 0);
    retParam->commands[45] |= (LE_SET_PATH_LOSS_REPORTING_ENABLE_PROCESS ? BIT(6) : 0);
    retParam->commands[45] |= (LE_SET_TRANSMIT_POWER_REPORTING_ENABLE_PROCESS ? BIT(7) : 0);
    
    // Octet 46
    retParam->commands[46] |= (LE_TRANSMITTER_TEST_V4_PROCESS ? BIT(0) : 0);
    retParam->commands[46] |= (SET_ECOSYSTEM_BASE_INTERVAL_PROCESS ? BIT(1) : 0);
    retParam->commands[46] |= (READ_LOCAL_SUPPORTED_CODECS_V2_PROCESS ? BIT(2) : 0);
    retParam->commands[46] |= (READ_LOCAL_SUPPORTED_CODEC_CAPABILITIES_PROCESS ? BIT(3) : 0);
    retParam->commands[46] |= (READ_LOCAL_SUPPORTED_CONTROLLER_DELAY_PROCESS ? BIT(4) : 0);
    retParam->commands[46] |= (CONFIGURE_DATA_PATH_PROCESS ? BIT(5) : 0);
    retParam->commands[46] |= (LE_SET_DATA_RELATED_ADDRESS_CHANGES_PROCESS ? BIT(6) : 0);
    retParam->commands[46] |= (SET_MIN_ENCRYPTION_KEY_SIZE_PROCESS ? BIT(7) : 0);
    
    // Octet 47
    retParam->commands[47] |= (LE_SET_DEFAULT_SUBRATE_PROCESS ? BIT(0) : 0);
    retParam->commands[47] |= (LE_SUBRATE_REQUEST_PROCESS ? BIT(1) : 0);
    retParam->commands[47] |= (LE_SET_EXTENDED_ADVERTISING_PARAMETERS_V2_PROCESS ? BIT(2) : 0);
    retParam->commands[47] |= (LE_SET_DECISION_DATA_PROCESS ? BIT(3) : 0);
    retParam->commands[47] |= (LE_SET_DECISION_INSTRUCTIONS_PROCESS ? BIT(4) : 0);
    retParam->commands[47] |= (LE_SET_PERIODIC_ADVERTISING_SUBEVENT_DATA_PROCESS ? BIT(5) : 0);
    retParam->commands[47] |= (LE_SET_PERIODIC_ADVERTISING_RESPONSE_DATA_PROCESS ? BIT(6) : 0);
    retParam->commands[47] |= (LE_SET_PERIODIC_SYNC_SUBEVENT_PROCESS ? BIT(7) : 0);
    
    // Octet 48
    retParam->commands[48] |= (LE_EXTENDED_CREATE_CONNECTION_V2_PROCESS ? BIT(0) : 0);
    retParam->commands[48] |= (LE_SET_PERIODIC_ADVERTISING_PARAMETERS_V2_PROCESS ? BIT(1) : 0);
    retParam->commands[48] |= (LE_READ_ALL_LOCAL_SUPPORTED_FEATURES_PROCESS ? BIT(2) : 0);
    retParam->commands[48] |= (LE_READ_ALL_REMOTE_FEATURES_PROCESS ? BIT(3) : 0);
    retParam->commands[48] |= (LE_SET_HOST_FEATURE_V2_PROCESS ? BIT(4) : 0);
    retParam->commands[48] |= (LE_ADD_DEVICE_TO_MONITORED_ADVERTISERS_LIST_PROCESS ? BIT(5) : 0);
    retParam->commands[48] |= (LE_REMOVE_DEVICE_FROM_MONITORED_ADVERTISERS_LIST_PROCESS ? BIT(6) : 0);
    retParam->commands[48] |= (LE_CLEAR_MONITORED_ADVERTISERS_LIST_PROCESS ? BIT(7) : 0);

    return SUCCESS;
}


/*************************************** HCI COMMAND LIST ********************************************/
static const hci_command_t  hci_command_link_control_list[] =
{
    {HCI_INQUIRY_COMMAND, INQUIRY_PROCESS},
    {HCI_INQUIRY_CANCEL_COMMAND, INQUIRY_CANCEL_PROCESS},
    {HCI_PERIODIC_INQUIRY_MODE_COMMAND, PERIODIC_INQUIRY_MODE_PROCESS},
    {HCI_EXIT_PERIODIC_INQUIRY_MODE_COMMAND, EXIT_PERIODIC_INQUIRY_MODE_PROCESS},
    {HCI_CREATE_CONNECTION_COMMAND, CREATE_CONNECTION_PROCESS},
    {HCI_DISCONNECT_COMMAND, DISCONNECT_PROCESS},
    {HCI_CREATE_CONNECTION_CANCEL_COMMAND, CREATE_CONNECTION_CANCEL_PROCESS},
    {HCI_ACCEPT_CONNECTION_REQUEST_COMMAND, ACCEPT_CONNECTION_REQUEST_PROCESS},
    {HCI_REJECT_CONNECTION_REQUEST_COMMAND, REJECT_CONNECTION_REQUEST_PROCESS},
    {HCI_LINK_KEY_REQUEST_REPLY_COMMAND, LINK_KEY_REQUEST_REPLY_PROCESS},
    {HCI_LINK_KEY_REQUEST_NEGATIVE_REPLY_COMMAND, LINK_KEY_REQUEST_NEGATIVE_REPLY_PROCESS},
    {HCI_PIN_CODE_REQUEST_REPLY_COMMAND, PIN_CODE_REQUEST_REPLY_PROCESS},
    {HCI_PIN_CODE_REQUEST_NEGATIVE_REPLY_COMMAND, PIN_CODE_REQUEST_NEGATIVE_REPLY_PROCESS},
    {HCI_CHANGE_CONNECTION_PACKET_TYPE_COMMAND, CHANGE_CONNECTION_PACKET_TYPE_PROCESS},
    {HCI_AUTHENTICATION_REQUESTED_COMMAND, AUTHENTICATION_REQUESTED_PROCESS},
    {HCI_SET_CONNECTION_ENCRYPTION_COMMAND, SET_CONNECTION_ENCRYPTION_PROCESS},
    {HCI_CHANGE_CONNECTION_LINK_KEY_COMMAND, CHANGE_CONNECTION_LINK_KEY_PROCESS},
    {HCI_LINK_KEY_SELECTION_COMMAND, LINK_KEY_SELECTION_PROCESS},
    {HCI_REMOTE_NAME_REQUEST_COMMAND, REMOTE_NAME_REQUEST_PROCESS},
    {HCI_REMOTE_NAME_REQUEST_CANCEL_COMMAND, REMOTE_NAME_REQUEST_CANCEL_PROCESS},
    {HCI_READ_REMOTE_SUPPORTED_FEATURES_COMMAND, READ_REMOTE_SUPPORTED_FEATURES_PROCESS},
    {HCI_READ_REMOTE_EXTENDED_FEATURES_COMMAND, READ_REMOTE_EXTENDED_FEATURES_PROCESS},
    {HCI_READ_REMOTE_VERSION_INFORMATION_COMMAND, READ_REMOTE_VERSION_INFORMATION_PROCESS},
    {HCI_READ_CLOCK_OFFSET_COMMAND, READ_CLOCK_OFFSET_PROCESS},
    {HCI_READ_LMP_HANDLE_COMMAND, READ_LMP_HANDLE_PROCESS},
    {HCI_SETUP_SYNCHRONOUS_CONNECTION_COMMAND, SETUP_SYNCHRONOUS_CONNECTION_PROCESS},
    {HCI_ACCEPT_SYNCHRONOUS_CONNECTION_REQUEST_COMMAND, ACCEPT_SYNCHRONOUS_CONNECTION_REQUEST_PROCESS},
    {HCI_REJECT_SYNCHRONOUS_CONNECTION_REQUEST_COMMAND, REJECT_SYNCHRONOUS_CONNECTION_REQUEST_PROCESS},
    {HCI_IO_CAPABILITY_REQUEST_REPLY_COMMAND, IO_CAPABILITY_REQUEST_REPLY_PROCESS},
    {HCI_USER_CONFIRMATION_REQUEST_REPLY_COMMAND, USER_CONFIRMATION_REQUEST_REPLY_PROCESS},
    {HCI_USER_CONFIRMATION_REQUEST_NEGATIVE_REPLY_COMMAND, USER_CONFIRMATION_REQUEST_NEGATIVE_REPLY_PROCESS},
    {HCI_USER_PASSKEY_REQUEST_REPLY_COMMAND, USER_PASSKEY_REQUEST_REPLY_PROCESS},
    {HCI_USER_PASSKEY_REQUEST_NEGATIVE_REPLY_COMMAND, USER_PASSKEY_REQUEST_NEGATIVE_REPLY_PROCESS},
    {HCI_REMOTE_OOB_DATA_REQUEST_REPLY_COMMAND, REMOTE_OOB_DATA_REQUEST_REPLY_PROCESS},
    {HCI_REMOTE_OOB_DATA_REQUEST_NEGATIVE_REPLY_COMMAND, REMOTE_OOB_DATA_REQUEST_NEGATIVE_REPLY_PROCESS},
    {HCI_IO_CAPABILITY_REQUEST_NEGATIVE_REPLY_COMMAND, IO_CAPABILITY_REQUEST_NEGATIVE_REPLY_PROCESS},
    {HCI_ENHANCED_SETUP_SYNCHRONOUS_CONNECTION_COMMAND, ENHANCED_SETUP_SYNCHRONOUS_CONNECTION_PROCESS},
    {HCI_ENHANCED_ACCEPT_SYNCHRONOUS_CONNECTION_REQUEST_COMMAND, ENHANCED_ACCEPT_SYNCHRONOUS_CONNECTION_REQUEST_PROCESS},
    {HCI_TRUNCATED_PAGE_COMMAND, TRUNCATED_PAGE_PROCESS},
    {HCI_TRUNCATED_PAGE_CANCEL_COMMAND, TRUNCATED_PAGE_CANCEL_PROCESS},
    {HCI_SET_CONNECTIONLESS_PERIPHERAL_BROADCAST_COMMAND, SET_CONNECTIONLESS_PERIPHERAL_BROADCAST_PROCESS},
    {HCI_SET_CONNECTIONLESS_PERIPHERAL_BROADCAST_RECEIVE_COMMAND, SET_CONNECTIONLESS_PERIPHERAL_BROADCAST_RECEIVE_PROCESS},
    {HCI_START_SYNCHRONIZATION_TRAIN_COMMAND, START_SYNCHRONIZATION_TRAIN_PROCESS},
    {HCI_RECEIVE_SYNCHRONIZATION_TRAIN_COMMAND, RECEIVE_SYNCHRONIZATION_TRAIN_PROCESS},
    {HCI_REMOTE_OOB_EXTENDED_DATA_REQUEST_REPLY_COMMAND, REMOTE_OOB_EXTENDED_DATA_REQUEST_REPLY_PROCESS}
};

static const hci_command_t hci_command_link_policy_list[] =
{
    {HCI_HOLD_MODE_COMMAND, HOLD_MODE_PROCESS},
    {HCI_SNIFF_MODE_COMMAND, SNIFF_MODE_PROCESS},
    {HCI_EXIT_SNIFF_MODE_COMMAND, EXIT_SNIFF_MODE_PROCESS},
    {HCI_QOS_SETUP_COMMAND, QOS_SETUP_PROCESS},
    {HCI_ROLE_DISCOVERY_COMMAND, ROLE_DISCOVERY_PROCESS},
    {HCI_SWITCH_ROLE_COMMAND, SWITCH_ROLE_PROCESS},
    {HCI_READ_LINK_POLICY_SETTINGS_COMMAND, READ_LINK_POLICY_SETTINGS_PROCESS},
    {HCI_WRITE_LINK_POLICY_SETTINGS_COMMAND, WRITE_LINK_POLICY_SETTINGS_PROCESS},
    {HCI_READ_DEFAULT_LINK_POLICY_SETTINGS_COMMAND, READ_DEFAULT_LINK_POLICY_SETTINGS_PROCESS},
    {HCI_WRITE_DEFAULT_LINK_POLICY_SETTINGS_COMMAND, WRITE_DEFAULT_LINK_POLICY_SETTINGS_PROCESS},
    {HCI_FLOW_SPECIFICATION_COMMAND, FLOW_SPECIFICATION_PROCESS},
    {HCI_SNIFF_SUBRATING_COMMAND, SNIFF_SUBRATING_PROCESS}
};

static const hci_command_t hci_command_controller_baseband_list[] =
{
    {HCI_SET_EVENT_MASK_COMMAND, SET_EVENT_MASK_PROCESS},
    {HCI_RESET_COMMAND, RESET_PROCESS},
    {HCI_SET_EVENT_FILTER_COMMAND, SET_EVENT_FILTER_PROCESS},
    {HCI_FLUSH_COMMAND, FLUSH_PROCESS},
    {HCI_READ_PIN_TYPE_COMMAND, READ_PIN_TYPE_PROCESS},
    {HCI_WRITE_PIN_TYPE_COMMAND, WRITE_PIN_TYPE_PROCESS},
    {HCI_READ_STORED_LINK_KEY_COMMAND, READ_STORED_LINK_KEY_PROCESS},
    {HCI_WRITE_STORED_LINK_KEY_COMMAND, WRITE_STORED_LINK_KEY_PROCESS},
    {HCI_DELETE_STORED_LINK_KEY_COMMAND, DELETE_STORED_LINK_KEY_PROCESS},
    {HCI_WRITE_LOCAL_NAME_COMMAND, WRITE_LOCAL_NAME_PROCESS},
    {HCI_READ_LOCAL_NAME_COMMAND, READ_LOCAL_NAME_PROCESS},
    {HCI_READ_CONNECTION_ACCEPT_TIMEOUT_COMMAND, READ_CONNECTION_ACCEPT_TIMEOUT_PROCESS},
    {HCI_WRITE_CONNECTION_ACCEPT_TIMEOUT_COMMAND, WRITE_CONNECTION_ACCEPT_TIMEOUT_PROCESS},
    {HCI_READ_PAGE_TIMEOUT_COMMAND, READ_PAGE_TIMEOUT_PROCESS},
    {HCI_WRITE_PAGE_TIMEOUT_COMMAND, WRITE_PAGE_TIMEOUT_PROCESS},
    {HCI_READ_SCAN_ENABLE_COMMAND, READ_SCAN_ENABLE_PROCESS},
    {HCI_WRITE_SCAN_ENABLE_COMMAND, WRITE_SCAN_ENABLE_PROCESS},
    {HCI_READ_PAGE_SCAN_ACTIVITY_COMMAND, READ_PAGE_SCAN_ACTIVITY_PROCESS},
    {HCI_WRITE_PAGE_SCAN_ACTIVITY_COMMAND, WRITE_PAGE_SCAN_ACTIVITY_PROCESS},
    {HCI_READ_INQUIRY_SCAN_ACTIVITY_COMMAND, READ_INQUIRY_SCAN_ACTIVITY_PROCESS},
    {HCI_WRITE_INQUIRY_SCAN_ACTIVITY_COMMAND, WRITE_INQUIRY_SCAN_ACTIVITY_PROCESS},
    {HCI_READ_AUTHENTICATION_ENABLE_COMMAND, READ_AUTHENTICATION_ENABLE_PROCESS},
    {HCI_WRITE_AUTHENTICATION_ENABLE_COMMAND, WRITE_AUTHENTICATION_ENABLE_PROCESS},
    {HCI_READ_CLASS_OF_DEVICE_COMMAND, READ_CLASS_OF_DEVICE_PROCESS},
    {HCI_WRITE_CLASS_OF_DEVICE_COMMAND, WRITE_CLASS_OF_DEVICE_PROCESS},
    {HCI_READ_VOICE_SETTING_COMMAND, READ_VOICE_SETTING_PROCESS},
    {HCI_WRITE_VOICE_SETTING_COMMAND, WRITE_VOICE_SETTING_PROCESS},
    {HCI_READ_AUTOMATIC_FLUSH_TIMEOUT_COMMAND, READ_AUTOMATIC_FLUSH_TIMEOUT_PROCESS},
    {HCI_WRITE_AUTOMATIC_FLUSH_TIMEOUT_COMMAND, WRITE_AUTOMATIC_FLUSH_TIMEOUT_PROCESS},
    {HCI_READ_NUM_BROADCAST_RETRANSMISSIONS_COMMAND, READ_NUM_BROADCAST_RETRANSMISSIONS_PROCESS},
    {HCI_WRITE_NUM_BROADCAST_RETRANSMISSIONS_COMMAND, WRITE_NUM_BROADCAST_RETRANSMISSIONS_PROCESS},
    {HCI_READ_HOLD_MODE_ACTIVITY_COMMAND, READ_HOLD_MODE_ACTIVITY_PROCESS},
    {HCI_WRITE_HOLD_MODE_ACTIVITY_COMMAND, WRITE_HOLD_MODE_ACTIVITY_PROCESS},
    {HCI_READ_TRANSMIT_POWER_LEVEL_COMMAND, READ_TRANSMIT_POWER_LEVEL_PROCESS},
    {HCI_READ_SYNCHRONOUS_FLOW_CONTROL_ENABLE_COMMAND, READ_SYNCHRONOUS_FLOW_CONTROL_ENABLE_PROCESS},
    {HCI_WRITE_SYNCHRONOUS_FLOW_CONTROL_ENABLE_COMMAND, WRITE_SYNCHRONOUS_FLOW_CONTROL_ENABLE_PROCESS},
    {HCI_SET_CONTROLLER_TO_HOST_FLOW_CONTROL_COMMAND, SET_CONTROLLER_TO_HOST_FLOW_CONTROL_PROCESS},
    {HCI_HOST_BUFFER_SIZE_COMMAND, HOST_BUFFER_SIZE_PROCESS},
    {HCI_HOST_NUMBER_OF_COMPLETED_PACKETS_COMMAND, HOST_NUMBER_OF_COMPLETED_PACKETS_PROCESS},
    {HCI_READ_LINK_SUPERVISION_TIMEOUT_COMMAND, READ_LINK_SUPERVISION_TIMEOUT_PROCESS},
    {HCI_WRITE_LINK_SUPERVISION_TIMEOUT_COMMAND, WRITE_LINK_SUPERVISION_TIMEOUT_PROCESS},
    {HCI_READ_NUMBER_OF_SUPPORTED_IAC_COMMAND, READ_NUMBER_OF_SUPPORTED_IAC_PROCESS},
    {HCI_READ_CURRENT_IAC_LAP_COMMAND, READ_CURRENT_IAC_LAP_PROCESS},
    {HCI_WRITE_CURRENT_IAC_LAP_COMMAND, WRITE_CURRENT_IAC_LAP_PROCESS},
    {HCI_SET_AFH_HOST_CHANNEL_CLASSIFICATION_COMMAND, SET_AFH_HOST_CHANNEL_CLASSIFICATION_PROCESS},
    {HCI_READ_INQUIRY_SCAN_TYPE_COMMAND, READ_INQUIRY_SCAN_TYPE_PROCESS},
    {HCI_WRITE_INQUIRY_SCAN_TYPE_COMMAND, WRITE_INQUIRY_SCAN_TYPE_PROCESS},
    {HCI_READ_INQUIRY_MODE_COMMAND, READ_INQUIRY_MODE_PROCESS},
    {HCI_WRITE_INQUIRY_MODE_COMMAND, WRITE_INQUIRY_MODE_PROCESS},
    {HCI_READ_PAGE_SCAN_TYPE_COMMAND, READ_PAGE_SCAN_TYPE_PROCESS},
    {HCI_WRITE_PAGE_SCAN_TYPE_COMMAND, WRITE_PAGE_SCAN_TYPE_PROCESS},
    {HCI_READ_AFH_CHANNEL_ASSESSMENT_MODE_COMMAND, READ_AFH_CHANNEL_ASSESSMENT_MODE_PROCESS},
    {HCI_WRITE_AFH_CHANNEL_ASSESSMENT_MODE_COMMAND, WRITE_AFH_CHANNEL_ASSESSMENT_MODE_PROCESS},
    {HCI_READ_EXTENDED_INQUIRY_RESPONSE_COMMAND, READ_EXTENDED_INQUIRY_RESPONSE_PROCESS},
    {HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_COMMAND, WRITE_EXTENDED_INQUIRY_RESPONSE_PROCESS},
    {HCI_REFRESH_ENCRYPTION_KEY_COMMAND, REFRESH_ENCRYPTION_KEY_PROCESS},
    {HCI_READ_SIMPLE_PAIRING_MODE_COMMAND, READ_SIMPLE_PAIRING_MODE_PROCESS},
    {HCI_WRITE_SIMPLE_PAIRING_MODE_COMMAND, WRITE_SIMPLE_PAIRING_MODE_PROCESS},
    {HCI_READ_LOCAL_OOB_DATA_COMMAND, READ_LOCAL_OOB_DATA_PROCESS},
    {HCI_READ_INQUIRY_RESPONSE_TRANSMIT_POWER_LEVEL_COMMAND, READ_INQUIRY_RESPONSE_TRANSMIT_POWER_LEVEL_PROCESS},
    {HCI_WRITE_INQUIRY_TRANSMIT_POWER_LEVEL_COMMAND, WRITE_INQUIRY_TRANSMIT_POWER_LEVEL_PROCESS},
    {HCI_SEND_KEYPRESS_NOTIFICATION_COMMAND, SEND_KEYPRESS_NOTIFICATION_PROCESS},
    {HCI_READ_DEFAULT_ERRONEOUS_DATA_REPORTING_COMMAND, READ_DEFAULT_ERRONEOUS_DATA_REPORTING_PROCESS},
    {HCI_WRITE_DEFAULT_ERRONEOUS_DATA_REPORTING_COMMAND, WRITE_DEFAULT_ERRONEOUS_DATA_REPORTING_PROCESS},
    {HCI_ENHANCED_FLUSH_COMMAND, ENHANCED_FLUSH_PROCESS},
    {HCI_SET_EVENT_MASK_PAGE_2_COMMAND, SET_EVENT_MASK_PAGE_2_PROCESS},
    {HCI_READ_FLOW_CONTROL_MODE_COMMAND, READ_FLOW_CONTROL_MODE_PROCESS},
    {HCI_WRITE_FLOW_CONTROL_MODE_COMMAND, WRITE_FLOW_CONTROL_MODE_PROCESS},
    {HCI_READ_ENHANCED_TRANSMIT_POWER_LEVEL_COMMAND, READ_ENHANCED_TRANSMIT_POWER_LEVEL_PROCESS},
    {HCI_READ_LE_HOST_SUPPORT_COMMAND, READ_LE_HOST_SUPPORT_PROCESS},
    {HCI_WRITE_LE_HOST_SUPPORT_COMMAND, WRITE_LE_HOST_SUPPORT_PROCESS},
    {HCI_SET_MWS_CHANNEL_PARAMETERS_COMMAND, SET_MWS_CHANNEL_PARAMETERS_PROCESS},
    {HCI_SET_EXTERNAL_FRAME_CONFIGURATION_COMMAND, SET_EXTERNAL_FRAME_CONFIGURATION_PROCESS},
    {HCI_SET_MWS_SIGNALING_COMMAND, SET_MWS_SIGNALING_PROCESS},
    {HCI_SET_MWS_TRANSPORT_LAYER_COMMAND, SET_MWS_TRANSPORT_LAYER_PROCESS},
    {HCI_SET_MWS_SCAN_FREQUENCY_TABLE_COMMAND, SET_MWS_SCAN_FREQUENCY_TABLE_PROCESS},
    {HCI_SET_MWS_PATTERN_CONFIGURATION_COMMAND, SET_MWS_PATTERN_CONFIGURATION_PROCESS},
    {HCI_SET_RESERVED_LT_ADDR_COMMAND, SET_RESERVED_LT_ADDR_PROCESS},
    {HCI_DELETE_RESERVED_LT_ADDR_COMMAND, DELETE_RESERVED_LT_ADDR_PROCESS},
    {HCI_SET_CONNECTIONLESS_PERIPHERAL_BROADCAST_DATA_COMMAND, SET_CONNECTIONLESS_PERIPHERAL_BROADCAST_DATA_PROCESS},
    {HCI_READ_SYNCHRONIZATION_TRAIN_PARAMETERS_COMMAND, READ_SYNCHRONIZATION_TRAIN_PARAMETERS_PROCESS},
    {HCI_WRITE_SYNCHRONIZATION_TRAIN_PARAMETERS_COMMAND, WRITE_SYNCHRONIZATION_TRAIN_PARAMETERS_PROCESS},
    {HCI_READ_SECURE_CONNECTIONS_HOST_SUPPORT_COMMAND, READ_SECURE_CONNECTIONS_HOST_SUPPORT_PROCESS},
    {HCI_WRITE_SECURE_CONNECTIONS_HOST_SUPPORT_COMMAND, WRITE_SECURE_CONNECTIONS_HOST_SUPPORT_PROCESS},
    {HCI_READ_AUTHENTICATED_PAYLOAD_TIMEOUT_COMMAND, READ_AUTHENTICATED_PAYLOAD_TIMEOUT_PROCESS},
    {HCI_WRITE_AUTHENTICATED_PAYLOAD_TIMEOUT_COMMAND, WRITE_AUTHENTICATED_PAYLOAD_TIMEOUT_PROCESS},
    {HCI_READ_LOCAL_OOB_EXTENDED_DATA_COMMAND, READ_LOCAL_OOB_EXTENDED_DATA_PROCESS},
    {HCI_READ_EXTENDED_PAGE_TIMEOUT_COMMAND, READ_EXTENDED_PAGE_TIMEOUT_PROCESS},
    {HCI_WRITE_EXTENDED_PAGE_TIMEOUT_COMMAND, WRITE_EXTENDED_PAGE_TIMEOUT_PROCESS},
    {HCI_READ_EXTENDED_INQUIRY_LENGTH_COMMAND, READ_EXTENDED_INQUIRY_LENGTH_PROCESS},
    {HCI_WRITE_EXTENDED_INQUIRY_LENGTH_COMMAND, WRITE_EXTENDED_INQUIRY_LENGTH_PROCESS},
    {HCI_SET_ECOSYSTEM_BASE_INTERVAL_COMMAND, SET_ECOSYSTEM_BASE_INTERVAL_PROCESS},
    {HCI_CONFIGURE_DATA_PATH_COMMAND, CONFIGURE_DATA_PATH_PROCESS},
    {HCI_SET_MIN_ENCRYPTION_KEY_SIZE_COMMAND, SET_MIN_ENCRYPTION_KEY_SIZE_PROCESS}
};
static const hci_command_t hci_command_informational_parameters_list[] =
{
    {HCI_READ_LOCAL_VERSION_INFORMATION_COMMAND, READ_LOCAL_VERSION_INFORMATION_PROCESS},
    {HCI_READ_LOCAL_SUPPORTED_COMMANDS_COMMAND, READ_LOCAL_SUPPORTED_COMMANDS_PROCESS},
    {HCI_READ_LOCAL_SUPPORTED_FEATURES_COMMAND, READ_LOCAL_SUPPORTED_FEATURES_PROCESS},
    {HCI_READ_LOCAL_EXTENDED_FEATURES_COMMAND, READ_LOCAL_EXTENDED_FEATURES_PROCESS},
    {HCI_READ_BUFFER_SIZE_COMMAND, READ_BUFFER_SIZE_PROCESS},
    {HCI_READ_BD_ADDR_COMMAND, READ_BD_ADDR_PROCESS},  
    {HCI_READ_DATA_BLOCK_SIZE_COMMAND, READ_DATA_BLOCK_SIZE_PROCESS},
    {HCI_READ_LOCAL_SUPPORTED_CODECS_COMMAND, READ_LOCAL_SUPPORTED_CODECS_PROCESS},
    {HCI_READ_LOCAL_SIMPLE_PAIRING_OPTIONS_COMMAND, READ_LOCAL_SIMPLE_PAIRING_OPTIONS_PROCESS},
    {HCI_READ_LOCAL_SUPPORTED_CODECS_COMMAND_V2, READ_LOCAL_SUPPORTED_CODECS_V2_PROCESS},
    {HCI_READ_LOCAL_SUPPORTED_CODEC_CAPABILITIES_COMMAND, READ_LOCAL_SUPPORTED_CODEC_CAPABILITIES_PROCESS},
    {HCI_READ_LOCAL_SUPPORTED_CONTROLLER_DELAY_COMMAND, READ_LOCAL_SUPPORTED_CONTROLLER_DELAY_PROCESS}
};

static const hci_command_t hci_command_status_parameters_list[] =
{
    {HCI_READ_FAILED_CONTACT_COUNTER_COMMAND, READ_FAILED_CONTACT_COUNTER_PROCESS},  
    {HCI_RESET_FAILED_CONTACT_COUNTER_COMMAND, RESET_FAILED_CONTACT_COUNTER_PROCESS},
    {HCI_READ_LINK_QUALITY_COMMAND, READ_LINK_QUALITY_PROCESS},
    {HCI_READ_RSSI_COMMAND, READ_RSSI_PROCESS},
    {HCI_READ_AFH_CHANNEL_MAP_COMMAND, READ_AFH_CHANNEL_MAP_PROCESS},
    {HCI_READ_CLOCK_COMMAND, READ_CLOCK_PROCESS},
    {HCI_READ_ENCRYPTION_KEY_SIZE_COMMAND, READ_ENCRYPTION_KEY_SIZE_PROCESS},
    {HCI_GET_MWS_TRANSPORT_LAYER_CONFIGURATION_COMMAND, GET_MWS_TRANSPORT_LAYER_CONFIGURATION_PROCESS},
    {HCI_SET_TRIGGERED_CLOCK_CAPTURE_COMMAND, SET_TRIGGERED_CLOCK_CAPTURE_PROCESS}
};

static const hci_command_t hci_command_testing_list[] =
{
    {HCI_READ_LOOPBACK_MODE_COMMAND, READ_LOOPBACK_MODE_PROCESS},  // 按规则保留生成结果
    {HCI_WRITE_LOOPBACK_MODE_COMMAND, WRITE_LOOPBACK_MODE_PROCESS},
    {HCI_ENABLE_IMPLEMENTATION_UNDER_TEST_MODE_COMMAND, ENABLE_IMPLEMENTATION_UNDER_TEST_MODE_PROCESS},
    {HCI_WRITE_SIMPLE_PAIRING_DEBUG_MODE_COMMAND, WRITE_SIMPLE_PAIRING_DEBUG_MODE_PROCESS},
    {HCI_WRITE_SECURE_CONNECTIONS_TEST_MODE_COMMAND, WRITE_SECURE_CONNECTIONS_TEST_MODE_PROCESS}
};

static const hci_command_t hci_command_le_controller_list[] =
{
    {HCI_LE_SET_EVENT_MASK_COMMAND, LE_SET_EVENT_MASK_PROCESS},
    {HCI_LE_READ_BUFFER_SIZE_COMMAND, LE_READ_BUFFER_SIZE_PROCESS},
    {HCI_LE_READ_BUFFER_SIZE_COMMAND_V2, LE_READ_BUFFER_SIZE_V2_PROCESS},
    {HCI_LE_READ_LOCAL_SUPPORTED_FEATURES_PAGE_0_COMMAND, LE_READ_LOCAL_SUPPORTED_FEATURES_PAGE_0_PROCESS},
    {HCI_LE_SET_RANDOM_ADDRESS_COMMAND, LE_SET_RANDOM_ADDRESS_PROCESS},
    {HCI_LE_SET_ADVERTISING_PARAMETERS_COMMAND, LE_SET_ADVERTISING_PARAMETERS_PROCESS},
    {HCI_LE_READ_ADVERTISING_PHYSICAL_CHANNEL_TX_POWER_COMMAND, LE_READ_ADVERTISING_PHYSICAL_CHANNEL_TX_POWER_PROCESS},
    {HCI_LE_SET_ADVERTISING_DATA_COMMAND, LE_SET_ADVERTISING_DATA_PROCESS},
    {HCI_LE_SET_SCAN_RESPONSE_DATA_COMMAND, LE_SET_SCAN_RESPONSE_DATA_PROCESS},
    {HCI_LE_SET_ADVERTISING_ENABLE_COMMAND, LE_SET_ADVERTISING_ENABLE_PROCESS},
    {HCI_LE_SET_SCAN_PARAMETERS_COMMAND, LE_SET_SCAN_PARAMETERS_PROCESS},
    {HCI_LE_SET_SCAN_ENABLE_COMMAND, LE_SET_SCAN_ENABLE_PROCESS},
    {HCI_LE_CREATE_CONNECTION_COMMAND, LE_CREATE_CONNECTION_PROCESS},
    {HCI_LE_CREATE_CONNECTION_CANCEL_COMMAND, LE_CREATE_CONNECTION_CANCEL_PROCESS},
    {HCI_LE_READ_FILTER_ACCEPT_LIST_SIZE_COMMAND, LE_READ_FILTER_ACCEPT_LIST_SIZE_PROCESS},
    {HCI_LE_CLEAR_FILTER_ACCEPT_LIST_COMMAND, LE_CLEAR_FILTER_ACCEPT_LIST_PROCESS},
    {HCI_LE_ADD_DEVICE_TO_FILTER_ACCEPT_LIST_COMMAND, LE_ADD_DEVICE_TO_FILTER_ACCEPT_LIST_PROCESS},
    {HCI_LE_REMOVE_DEVICE_FROM_FILTER_ACCEPT_LIST_COMMAND, LE_REMOVE_DEVICE_FROM_FILTER_ACCEPT_LIST_PROCESS},
    {HCI_LE_CONNECTION_UPDATE_COMMAND, LE_CONNECTION_UPDATE_PROCESS},
    {HCI_LE_SET_HOST_CHANNEL_CLASSIFICATION_COMMAND, LE_SET_HOST_CHANNEL_CLASSIFICATION_PROCESS},
    {HCI_LE_READ_CHANNEL_MAP_COMMAND, LE_READ_CHANNEL_MAP_PROCESS},
    {HCI_LE_READ_REMOTE_FEATURES_PAGE_0_COMMAND, LE_READ_REMOTE_FEATURES_PAGE_0_PROCESS},
    {HCI_LE_ENCRYPT_COMMAND, LE_ENCRYPT_PROCESS},
    {HCI_LE_RAND_COMMAND, LE_RAND_PROCESS},
    {HCI_LE_ENABLE_ENCRYPTION_COMMAND, LE_ENABLE_ENCRYPTION_PROCESS},
    {HCI_LE_LONG_TERM_KEY_REQUEST_REPLY_COMMAND, LE_LONG_TERM_KEY_REQUEST_REPLY_PROCESS},
    {HCI_LE_LONG_TERM_KEY_REQUEST_NEGATIVE_REPLY_COMMAND, LE_LONG_TERM_KEY_REQUEST_NEGATIVE_REPLY_PROCESS},
    {HCI_LE_READ_SUPPORTED_STATES_COMMAND, LE_READ_SUPPORTED_STATES_PROCESS},
    {HCI_LE_RECEIVER_TEST_COMMAND, LE_RECEIVER_TEST_PROCESS},
    {HCI_LE_RECEIVER_TEST_COMMAND_V2, LE_RECEIVER_TEST_V2_PROCESS},
    {HCI_LE_RECEIVER_TEST_COMMAND_V3, LE_RECEIVER_TEST_V3_PROCESS},
    {HCI_LE_TRANSMITTER_TEST_COMMAND, LE_TRANSMITTER_TEST_PROCESS},
    {HCI_LE_TRANSMITTER_TEST_COMMAND_V2, LE_TRANSMITTER_TEST_V2_PROCESS},
    {HCI_LE_TRANSMITTER_TEST_COMMAND_V3, LE_TRANSMITTER_TEST_V3_PROCESS},
    {HCI_LE_TRANSMITTER_TEST_COMMAND_V4, LE_TRANSMITTER_TEST_V4_PROCESS},
    {HCI_LE_TEST_END_COMMAND, LE_TEST_END_PROCESS},
    {HCI_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_REPLY_COMMAND, LE_REMOTE_CONNECTION_PARAMETER_REQUEST_REPLY_PROCESS},
    {HCI_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_NEGATIVE_REPLY_COMMAND, LE_REMOTE_CONNECTION_PARAMETER_REQUEST_NEGATIVE_REPLY_PROCESS},
    {HCI_LE_SET_DATA_LENGTH_COMMAND, LE_SET_DATA_LENGTH_PROCESS},
    {HCI_LE_READ_SUGGESTED_DEFAULT_DATA_LENGTH_COMMAND, LE_READ_SUGGESTED_DEFAULT_DATA_LENGTH_PROCESS},
    {HCI_LE_WRITE_SUGGESTED_DEFAULT_DATA_LENGTH_COMMAND, LE_WRITE_SUGGESTED_DEFAULT_DATA_LENGTH_PROCESS},
    {HCI_LE_READ_LOCAL_P_256_PUBLIC_KEY_COMMAND, LE_READ_LOCAL_P_256_PUBLIC_KEY_PROCESS},
    {HCI_LE_GENERATE_DHKEY_CMMMMAND, LE_GENERATE_DHKEY_PROCESS},
    {HCI_LE_GENERATE_DHKEY_CMMMMAND_V2, LE_GENERATE_DHKEY_V2_PROCESS},
    {HCI_LE_ADD_DEVICE_TO_RESOLVING_LIST_COMMAND, LE_ADD_DEVICE_TO_RESOLVING_LIST_PROCESS},
    {HCI_LE_REMOVE_DEVICE_FROM_RESOLVING_LIST_COMMAND, LE_REMOVE_DEVICE_FROM_RESOLVING_LIST_PROCESS},
    {HCI_LE_CLEAR_RESOLVING_LIST_COMMAND, LE_CLEAR_RESOLVING_LIST_PROCESS},
    {HCI_LE_READ_RESOLVING_LIST_SIZE_COMMAND, LE_READ_RESOLVING_LIST_SIZE_PROCESS},
    {HCI_LE_READ_PEER_RESOLVABLE_ADDRESS_COMMAND, LE_READ_PEER_RESOLVABLE_ADDRESS_PROCESS},
    {HCI_LE_READ_LOCAL_RESOLVABLE_ADDRESS_COMMAND, LE_READ_LOCAL_RESOLVABLE_ADDRESS_PROCESS},
    {HCI_LE_SET_ADDRESS_RESOLUTION_ENABLE_COMMAND, LE_SET_ADDRESS_RESOLUTION_ENABLE_PROCESS},
    {HCI_LE_SET_RESOLVABLE_PRIVATE_ADDRESS_TIMEOUT_COMMAND, LE_SET_RESOLVABLE_PRIVATE_ADDRESS_TIMEOUT_PROCESS},
    {HCI_LE_READ_MAXIMUM_DATA_LENGTH_COMMAND, LE_READ_MAXIMUM_DATA_LENGTH_PROCESS},
    {HCI_LE_READ_PHY_COMMAND, LE_READ_PHY_PROCESS},
    {HCI_LE_SET_DEFAULT_PHY_COMMAND, LE_SET_DEFAULT_PHY_PROCESS},
    {HCI_LE_SET_PHY_COMMAND, LE_SET_PHY_PROCESS},
    {HCI_LE_SET_ADVERTISING_SET_RANDOM_ADDRESS_COMMAND, LE_SET_ADVERTISING_SET_RANDOM_ADDRESS_PROCESS},
    {HCI_LE_SET_EXTENDED_ADVERTISING_PARAMETERS_COMMAND, LE_SET_EXTENDED_ADVERTISING_PARAMETERS_PROCESS},
    {HCI_LE_SET_EXTENDED_ADVERTISING_PARAMETERS_COMMAND_V2, LE_SET_EXTENDED_ADVERTISING_PARAMETERS_V2_PROCESS},
    {HCI_LE_SET_EXTENDED_ADVERTISING_DATA_COMMAND, LE_SET_EXTENDED_ADVERTISING_DATA_PROCESS},
    {HCI_LE_SET_EXTENDED_SCAN_RESPONSE_DATA_COMMAND, LE_SET_EXTENDED_SCAN_RESPONSE_DATA_PROCESS},
    {HCI_LE_SET_EXTENDED_ADVERTISING_ENABLE_COMMAND, LE_SET_EXTENDED_ADVERTISING_ENABLE_PROCESS},
    {HCI_LE_READ_MAXIMUM_ADVERTISING_DATA_LENGTH_COMMAND, LE_READ_MAXIMUM_ADVERTISING_DATA_LENGTH_PROCESS},
    {HCI_LE_READ_NUMBER_OF_SUPPORTED_ADVERTISING_SETS_COMMAND, LE_READ_NUMBER_OF_SUPPORTED_ADVERTISING_SETS_PROCESS},
    {HCI_LE_REMOVE_ADVERTISING_SET_COMMAND, LE_REMOVE_ADVERTISING_SET_PROCESS},
    {HCI_LE_CLEAR_ADVERTISING_SETS_COMMAND, LE_CLEAR_ADVERTISING_SETS_PROCESS},
    {HCI_LE_SET_PERIODIC_ADVERTISING_PARAMETERS_COMMAND, LE_SET_PERIODIC_ADVERTISING_PARAMETERS_PROCESS},
    {HCI_LE_SET_PERIODIC_ADVERTISING_PARAMETERS_COMMAND_V2, LE_SET_PERIODIC_ADVERTISING_PARAMETERS_V2_PROCESS},
    {HCI_LE_SET_PERIODIC_ADVERTISING_DATA_COMMAND, LE_SET_PERIODIC_ADVERTISING_DATA_PROCESS},
    {HCI_LE_SET_PERIODIC_ADVERTISING_ENABLE_COMMAND, LE_SET_PERIODIC_ADVERTISING_ENABLE_PROCESS},
    {HCI_LE_SET_EXTENDED_SCAN_PARAMETERS_COMMAND, LE_SET_EXTENDED_SCAN_PARAMETERS_PROCESS},
    {HCI_LE_SET_EXTENDED_SCAN_ENABLE_COMMAND, LE_SET_EXTENDED_SCAN_ENABLE_PROCESS},
    {HCI_LE_EXTENDED_CREATE_CONNECTION_COMMAND, LE_EXTENDED_CREATE_CONNECTION_PROCESS},
    {HCI_LE_EXTENDED_CREATE_CONNECTION_COMMAND_V2, LE_EXTENDED_CREATE_CONNECTION_V2_PROCESS},
    {HCI_LE_PERIODIC_ADVERTISING_CREATE_SYNC_COMMAND, LE_PERIODIC_ADVERTISING_CREATE_SYNC_PROCESS},
    {HCI_LE_PERIODIC_ADVERTISING_CREATE_SYNC_CANCEL_COMMAND, LE_PERIODIC_ADVERTISING_CREATE_SYNC_CANCEL_PROCESS},
    {HCI_LE_PERIODIC_ADVERTISING_TERMINATE_SYNC_COMMAND, LE_PERIODIC_ADVERTISING_TERMINATE_SYNC_PROCESS},
    {HCI_LE_ADD_DEVICE_TO_PERIODIC_ADVERTISER_LIST_COMMAND, LE_ADD_DEVICE_TO_PERIODIC_ADVERTISER_LIST_PROCESS},
    {HCI_LE_REMOVE_DEVICE_FROM_PERIODIC_ADVERTISER_LIST_COMMAND, LE_REMOVE_DEVICE_FROM_PERIODIC_ADVERTISER_LIST_PROCESS},
    {HCI_LE_CLEAR_PERIODIC_ADVERTISER_LIST_COMMAND, LE_CLEAR_PERIODIC_ADVERTISER_LIST_PROCESS},
    {HCI_LE_READ_PERIODIC_ADVERTISER_LIST_SIZE_COMMAND, LE_READ_PERIODIC_ADVERTISER_LIST_SIZE_PROCESS},
    {HCI_LE_READ_TRANSMIT_POWER_COMMAND, LE_READ_TRANSMIT_POWER_PROCESS},
    {HCI_LE_READ_RF_PATH_COMPENSATION_COMMAND, LE_READ_RF_PATH_COMPENSATION_PROCESS},
    {HCI_LE_WRITE_RF_PATH_COMPENSATION_COMMAND, LE_WRITE_RF_PATH_COMPENSATION_PROCESS},
    {HCI_LE_SET_PRIVACY_MODE_COMMAND, LE_SET_PRIVACY_MODE_PROCESS},
    {HCI_LE_SET_CONNECTIONLESS_CTE_TRANSMIT_PARAMETERS_COMMAND, LE_SET_CONNECTIONLESS_CTE_TRANSMIT_PARAMETERS_PROCESS},
    {HCI_LE_SET_CONNECTIONLESS_CTE_TRANSMIT_ENABLE_COMMAND, LE_SET_CONNECTIONLESS_CTE_TRANSMIT_ENABLE_PROCESS},
    {HCI_LE_SET_CONNECTIONLESS_IQ_SAMPLING_ENABLE_COMMAND, LE_SET_CONNECTIONLESS_IQ_SAMPLING_ENABLE_PROCESS},
    {HCI_LE_SET_CONNECTION_CTE_RECEIVE_PARAMETERS_COMMAND, LE_SET_CONNECTION_CTE_RECEIVE_PARAMETERS_PROCESS},
    {HCI_LE_SET_CONNECTION_CTE_TRANSMIT_PARAMETERS_COMMAND, LE_SET_CONNECTION_CTE_TRANSMIT_PARAMETERS_PROCESS},
    {HCI_LE_CONNECTION_CTE_REQUEST_ENABLE_COMMAND, LE_CONNECTION_CTE_REQUEST_ENABLE_PROCESS},
    {HCI_LE_CONNECTION_CTE_RESPONSE_ENABLE_COMMAND, LE_CONNECTION_CTE_RESPONSE_ENABLE_PROCESS},
    {HCI_LE_READ_ANTENNA_INFORMATION_COMMAND, LE_READ_ANTENNA_INFORMATION_PROCESS},
    {HCI_LE_SET_PERIODIC_ADVERTISING_RECEIVE_ENABLE_COMMAND, LE_SET_PERIODIC_ADVERTISING_RECEIVE_ENABLE_PROCESS},
    {HCI_LE_PERIODIC_ADVERTISING_SYNC_TRANSFER_COMMAND, LE_PERIODIC_ADVERTISING_SYNC_TRANSFER_PROCESS},
    {HCI_LE_PERIODIC_ADVERTISING_SET_INFO_TRANSFER_COMMAND, LE_PERIODIC_ADVERTISING_SET_INFO_TRANSFER_PROCESS},
    {HCI_LE_SET_PERIODIC_ADVERTISING_SYNC_TRANSFER_PARAMETERS_COMMAND, LE_SET_PERIODIC_ADVERTISING_SYNC_TRANSFER_PARAMETERS_PROCESS},
    {HCI_LE_SET_DEFAULT_PERIODIC_ADVERTISING_SYNC_TRANSFER_PARAMETERS_COMMAND, LE_SET_DEFAULT_PERIODIC_ADVERTISING_SYNC_TRANSFER_PARAMETERS_PROCESS},
    {HCI_LE_MODIFY_SLEEP_CLOCK_ACCURACY_COMMAND, LE_MODIFY_SLEEP_CLOCK_ACCURACY_PROCESS},
    {HCI_LE_READ_ISO_TX_SYNC_COMMAND, LE_READ_ISO_TX_SYNC_PROCESS},
    {HCI_LE_SET_CIG_PARAMETERS_COMMAND, LE_SET_CIG_PARAMETERS_PROCESS},
    {HCI_LE_SET_CIG_PARAMETERS_TEST_COMMAND, LE_SET_CIG_PARAMETERS_TEST_PROCESS},
    {HCI_LE_CREATE_CIS_COMMAND, LE_CREATE_CIS_PROCESS},
    {HCI_LE_REMOVE_CIG_COMMAND, LE_REMOVE_CIG_PROCESS},
    {HCI_LE_ACCEPT_CIS_REQUEST_COMMAND, LE_ACCEPT_CIS_REQUEST_PROCESS},
    {HCI_LE_REJECT_CIS_REQUEST_COMMAND, LE_REJECT_CIS_REQUEST_PROCESS},
    {HCI_LE_CREATE_BIG_COMMAND, LE_CREATE_BIG_PROCESS},
    {HCI_LE_CREATE_BIG_TEST_COMMAND, LE_CREATE_BIG_TEST_PROCESS},
    {HCI_LE_TERMINATE_BIG_COMMAND, LE_TERMINATE_BIG_PROCESS},
    {HCI_LE_BIG_CREATE_SYNC_COMMAND, LE_BIG_CREATE_SYNC_PROCESS},
    {HCI_LE_BIG_TERMINATE_SYNC_COMMAND, LE_BIG_TERMINATE_SYNC_PROCESS},
    {HCI_LE_REQUEST_PEER_SCA_COMMAND, LE_REQUEST_PEER_SCA_PROCESS},
    {HCI_LE_SETUP_ISO_DATA_PATH_COMMAND, LE_SETUP_ISO_DATA_PATH_PROCESS},
    {HCI_LE_REMOVE_ISO_DATA_PATH_COMMAND, LE_REMOVE_ISO_DATA_PATH_PROCESS},
    {HCI_LE_ISO_TRANSMIT_TEST_COMMAND, LE_ISO_TRANSMIT_TEST_PROCESS},
    {HCI_LE_ISO_RECEIVE_TEST_COMMAND, LE_ISO_RECEIVE_TEST_PROCESS},
    {HCI_LE_ISO_READ_TEST_COUNTERS_COMMAND, LE_ISO_READ_TEST_COUNTERS_PROCESS},
    {HCI_LE_ISO_TEST_END_COMMAND, LE_ISO_TEST_END_PROCESS},
    {HCI_LE_SET_HOST_FEATURE_COMMAND, LE_SET_HOST_FEATURE_PROCESS},
    {HCI_LE_SET_HOST_FEATURE_COMMAND_V2, LE_SET_HOST_FEATURE_V2_PROCESS},
    {HCI_LE_READ_ISO_LINK_QUALITY_COMMAND, LE_READ_ISO_LINK_QUALITY_PROCESS},
    {HCI_LE_ENHANCED_READ_TRANSMIT_POWER_LEVEL_COMMAND, LE_ENHANCED_READ_TRANSMIT_POWER_LEVEL_PROCESS},
    {HCI_LE_READ_REMOTE_TRANSMIT_POWER_LEVEL_COMMAND, LE_READ_REMOTE_TRANSMIT_POWER_LEVEL_PROCESS},
    {HCI_LE_SET_PATH_LOSS_REPORTING_PARAMETERS_COMMAND, LE_SET_PATH_LOSS_REPORTING_PARAMETERS_PROCESS},
    {HCI_LE_SET_PATH_LOSS_REPORTING_ENABLE_COMMAND, LE_SET_PATH_LOSS_REPORTING_ENABLE_PROCESS},
    {HCI_LE_SET_TRANSMIT_POWER_REPORTING_ENABLE_COMMAND, LE_SET_TRANSMIT_POWER_REPORTING_ENABLE_PROCESS},
    {HCI_LE_SET_DATA_RELATED_ADDRESS_CHANGES_COMMAND, LE_SET_DATA_RELATED_ADDRESS_CHANGES_PROCESS},
    {HCI_LE_SET_DEFAULT_SUBRATE_COMMAND, LE_SET_DEFAULT_SUBRATE_PROCESS},
    {HCI_LE_SUBRATE_REQUEST_COMMAND, LE_SUBRATE_REQUEST_PROCESS},
    {HCI_LE_SET_PERIODIC_ADVERTISING_SUBEVENT_DATA_COMMAND, LE_SET_PERIODIC_ADVERTISING_SUBEVENT_DATA_PROCESS},
    {HCI_LE_SET_PERIODIC_ADVERTISING_RESPONSE_DATA_COMMAND, LE_SET_PERIODIC_ADVERTISING_RESPONSE_DATA_PROCESS},
    {HCI_LE_SET_PERIODIC_SYNC_SUBEVENT_COMMAND, LE_SET_PERIODIC_SYNC_SUBEVENT_PROCESS},
    {HCI_LE_READ_ALL_LOCAL_SUPPORTED_FEATURES_COMMAND, LE_READ_ALL_LOCAL_SUPPORTED_FEATURES_PROCESS},
    {HCI_LE_READ_ALL_REMOTE_FEATURES_COMMAND, LE_READ_ALL_REMOTE_FEATURES_PROCESS},
    {HCI_LE_CS_READ_LOCAL_SUPPORTED_CAPABILITIES_COMMAND, LE_CS_READ_LOCAL_SUPPORTED_CAPABILITIES_PROCESS},
    {HCI_LE_CS_READ_REMOTE_SUPPORTED_CAPABILITIES_COMMAND, LE_CS_READ_REMOTE_SUPPORTED_CAPABILITIES_PROCESS},
    {HCI_LE_CS_WRITE_CACHED_REMOTE_SUPPORTED_CAPABILITIES_COMMAND, LE_CS_WRITE_CACHED_REMOTE_SUPPORTED_CAPABILITIES_PROCESS},
    {HCI_LE_CS_SECURITY_ENABLE_COMMAND, LE_CS_SECURITY_ENABLE_PROCESS},
    {HCI_LE_CS_SET_DEFAULT_SETTINGS_COMMAND, LE_CS_SET_DEFAULT_SETTINGS_PROCESS},
    {HCI_LE_CS_READ_REMOTE_FAE_TABLE_COMMAND, LE_CS_READ_REMOTE_FAE_TABLE_PROCESS},
    {HCI_LE_CS_WRITE_CACHED_REMOTE_FAE_TABLE_COMMAND, LE_CS_WRITE_CACHED_REMOTE_FAE_TABLE_PROCESS},
    {HCI_LE_CS_CREATE_CONFIG_COMMAND, LE_CS_CREATE_CONFIG_PROCESS},
    {HCI_LE_CS_REMOVE_CONFIG_COMMAND, LE_CS_REMOVE_CONFIG_PROCESS},
    {HCI_LE_CS_SET_CHANNEL_CLASSIFICATION_COMMAND, LE_CS_SET_CHANNEL_CLASSIFICATION_PROCESS},
    {HCI_LE_CS_SET_PROCEDURE_PARAMETERS_COMMAND, LE_CS_SET_PROCEDURE_PARAMETERS_PROCESS},
    {HCI_LE_CS_PROCEDURE_ENABLE_COMMAND, LE_CS_PROCEDURE_ENABLE_PROCESS},
    {HCI_LE_CS_TEST_COMMAND, LE_CS_TEST_PROCESS},
    {HCI_LE_CS_TEST_END_COMMAND, LE_CS_TEST_END_PROCESS},
    {HCI_LE_SET_DECISION_DATA_COMMAND, LE_SET_DECISION_DATA_PROCESS},
    {HCI_LE_SET_DECISION_INSTRUCTIONS_COMMAND, LE_SET_DECISION_INSTRUCTIONS_PROCESS},
    {HCI_LE_ADD_DEVICE_TO_MONITORED_ADVERTISERS_LIST_COMMAND, LE_ADD_DEVICE_TO_MONITORED_ADVERTISERS_LIST_PROCESS},
    {HCI_LE_REMOVE_DEVICE_FROM_MONITORED_ADVERTISERS_LIST_COMMAND, LE_REMOVE_DEVICE_FROM_MONITORED_ADVERTISERS_LIST_PROCESS},
    {HCI_LE_CLEAR_MONITORED_ADVERTISERS_LIST_COMMAND, LE_CLEAR_MONITORED_ADVERTISERS_LIST_PROCESS},
    {HCI_LE_ENABLE_MONITORING_ADVERTISERS_COMMAND, LE_ENABLE_MONITORING_ADVERTISERS_PROCESS},
    {HCI_LE_READ_MONITORED_ADVERTISERS_LIST_SIZE_COMMAND, LE_READ_MONITORED_ADVERTISERS_LIST_SIZE_PROCESS},
    {HCI_LE_FRAME_SPACE_UPDATE_COMMAND, LE_FRAME_SPACE_UPDATE_PROCESS}
};

static const hci_command_t hci_command_vendor_specific_list[] =
{
    {0, NULL},
};

static const hci_command_array_t hci_cmd_handlers[] =
{
	{HCI_COMMAND_LINK_CONTROL,             hci_command_link_control_list,            HCI_COMMAND_LIST_LENGTH(hci_command_link_control_list)},
	{HCI_COMMAND_LINK_POLICY,              hci_command_link_policy_list,             HCI_COMMAND_LIST_LENGTH(hci_command_link_policy_list)},
	{HCI_COMMAND_CONTROLLER_BASEBAND,      hci_command_controller_baseband_list,     HCI_COMMAND_LIST_LENGTH(hci_command_controller_baseband_list)},
	{HCI_COMMAND_INFORMATIONAL_PARAMETERS, hci_command_informational_parameters_list,HCI_COMMAND_LIST_LENGTH(hci_command_informational_parameters_list)},
	{HCI_COMMAND_STATUS_PARAMETERS,        hci_command_status_parameters_list,       HCI_COMMAND_LIST_LENGTH(hci_command_status_parameters_list)},
	{HCI_COMMAND_TESTING,                  hci_command_testing_list,                 HCI_COMMAND_LIST_LENGTH(hci_command_testing_list)},
	{HCI_COMMAND_LE_CONTROLLER,            hci_command_le_controller_list,           HCI_COMMAND_LIST_LENGTH(hci_command_le_controller_list)},
    {HCI_COMMAND_VENDOR_SPECIFIC,          hci_command_vendor_specific_list,         HCI_COMMAND_LIST_LENGTH(hci_command_vendor_specific_list)},
};

volatile _u8 AAA_COMMAND_BUFFER[16];
volatile _u8 AAA_COMMAND_i;
volatile _u8 AAA_COMMAND_j;
void hci_command_packet_process(_u8* data)
{
    txMemcpy((_u8*)AAA_COMMAND_BUFFER,data,10);
	bt_hci_command_t* hciCommand = (bt_hci_command_t*)data;
    bt_hci_event_t* event = NULL;
    int status = UNKNOWN_HCI_COMMAND;

    for(_u8 i=0;i<HCI_COMMAND_LENGTH(hci_cmd_handlers);i++)
    {
        if(hci_cmd_handlers[i].command == hciCommand->ogf)
        {
            _u32 listLen = hci_cmd_handlers[i].conut;
            for(_u32 j=0 ;j<listLen;j++)
            {
                if(hci_cmd_handlers[i].pArray[j].ocf == hciCommand->ocf)
                {
                	hciCommandOpcode = hciCommand->opcode;
                	AAA_COMMAND_i = i;
                	AAA_COMMAND_j = j;
                	if(hci_cmd_handlers[i].pArray[j].process!=NULL)
                	{
                		status = hci_cmd_handlers[i].pArray[j].process(hciCommand->data,hciCommand->length,&event);
                	}
                }
            }
        }
    }
    if(status == UNKNOWN_HCI_COMMAND)
    {
        event = hci_command_status_event(hciCommand->opcode,UNKNOWN_HCI_COMMAND);
    }
    if(event!=NULL)
    {
        ble_hci_send_data(BLE_HCI_EVENT_PACKET,(_u8*)event,2+event->length);
    }
}

