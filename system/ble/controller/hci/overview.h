#include"common/txCommon.h"
#include"command_event.h"
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
    int                name;
    bt_version_e       version;
    const _u8*         BR_EDR;
    const _u8*         LE;
}bt_ov_cmd_evts_t;

bt_ov_cmd_evts_t bt_commands_events_overview[] = 
{
    /**HCI name**/                                                            /**version**/  /**BR/EDR**/        /**LE**/
    /****************************************************BT VERSION 1.1*************************************************************/
    {HCI_RESET_COMMAND,                                                           v1_1,          "M",                "M"},
    {HCI_COMMAND_COMPLETE_EVENT,                                                  v1_1,          "M",                "M"},
    {HCI_COMMAND_STATUS_EVENT,                                                    v1_1,          "M",                "M"},
    {HCI_READ_BD_ADDR_COMMAND,                                                    v1_1,          "M",                "M"},
    {HCI_READ_LOCAL_SUPPORTED_FEATURES_COMMAND,                                   v1_1,          "M",                "M"},
    {HCI_READ_LOCAL_VERSION_INFORMATION_COMMAND,                                  v1_1,          "M",                "M"},
    {HCI_SET_EVENT_MASK_COMMAND,                                                  v1_1,          "M",                "M"},
    {HCI_DISCONNECT_COMMAND,                                                      v1_1,          "M",                "C.3"}, 
    {HCI_DISCONNECTION_COMPLETE_EVENT,                                            v1_1,          "M",                "C.3"}, 
    {HCI_NUMBER_OF_COMPLETED_PACKETS_EVENT,                                       v1_1,          "M",                "C.3"}, 
    {HCI_READ_REMOTE_VERSION_INFORMATION_COMMAND,                                 v1_1,          "O",                "C.3"}, 
    {HCI_READ_RSSI_COMMAND,                                                       v1_1,          "O",                "C.3"}, 
    // C.3: Mandatory if the LE Controller supports Connection State, otherwise excluded
    {HCI_READ_REMOTE_VERSION_INFORMATION_COMPLETE_EVENT,                          v1_1,          "C.105",            "C.3"}, 
    // C.105: Mandatory if the Read Remote Version Information command is supported; 
    {HCI_READ_TRANSMIT_POWER_LEVEL_COMMAND,                                       v1_1,          "C.152",            "C.3"}, 
    // C.152: Mandatory if Power Control is supported; C.3: Mandatory if the LE Controller supports Connection State
    {HCI_DATA_BUFFER_OVERFLOW_EVENT,                                              v1_1,          "O",                "O"},
    {HCI_HARDWARE_ERROR_EVENT,                                                    v1_1,          "O",                "O"},
    {HCI_HOST_BUFFER_SIZE_COMMAND,                                                v1_1,          "C.107",            "C.107"}, 
    {HCI_HOST_NUMBER_OF_COMPLETED_PACKETS_COMMAND,                                v1_1,          "C.107",            "C.107"}, 
    // C.107: Mandatory if the Set Controller To Host Flow Control command is supported, otherwise excluded
    {HCI_READ_CONNECTION_ACCEPT_TIMEOUT_COMMAND,                                  v1_1,          "M",                "C.40"}, 
    {HCI_WRITE_CONNECTION_ACCEPT_TIMEOUT_COMMAND,                                 v1_1,          "M",                "C.40"}, 
    // C.40: Mandatory if LE Feature (Connected Isochronous Stream - Peripheral) is supported, otherwise excluded
    {HCI_SET_CONTROLLER_TO_HOST_FLOW_CONTROL_COMMAND,                             v1_1,          "O",                "C.96"}, 
    // C.96: Optional if the LE Controller supports Connection State, otherwise excluded
    {HCI_ENCRYPTION_CHANGE_EVENT,                                                 v1_1,          "[v1] M [v2] C.158", "[v1] C.4 [v2] C.56"}, 
    // C.158:Mandatory if the Set Min Encryption Key Size command is supported,otherwise optional.
    // C.4: Mandatory if LE Feature (LE Encryption) is supported;
    // C.56: Mandatory if LE Feature (LL Extended Feature Set) is supported
    {HCI_ACCEPT_CONNECTION_REQUEST_COMMAND,                                       v1_1,          "M",                "E"},
    {HCI_AUTHENTICATION_COMPLETE_EVENT,                                           v1_1,          "C.101",            "E"}, 
    // C.101: Mandatory if the Authentication Requested command is supported, otherwise excluded
    {HCI_AUTHENTICATION_REQUESTED_COMMAND,                                        v1_1,          "O",                "E"},
    {HCI_CHANGE_CONNECTION_LINK_KEY_COMMAND,                                      v1_1,          "O",                "E"},
    {HCI_CHANGE_CONNECTION_LINK_KEY_COMPLETE_EVENT,                               v1_1,          "C.102",            "E"}, 
    // C.102: Mandatory if the Change Connection Link Key command is supported, otherwise excluded
    {HCI_CHANGE_CONNECTION_PACKET_TYPE_COMMAND,                                   v1_1,          "C.133",            "E"}, 
    // C.133: Mandatory if HV2, HV3, or multi-slot or EDR ACL packets are supported, otherwise excluded
    {HCI_CONNECTION_COMPLETE_EVENT,                                               v1_1,          "M",                "E"},
    {HCI_CONNECTION_PACKET_TYPE_CHANGED_EVENT,                                    v1_1,          "C.133",            "E"},
     // C.133: Mandatory if HV2, HV3, or multi-slot or EDR ACL packets are supported, otherwise excluded
    {HCI_CONNECTION_REQUEST_EVENT,                                                v1_1,          "M",                "E"},
    {HCI_CREATE_CONNECTION_COMMAND,                                               v1_1,          "M",                "E"},
    {HCI_DELETE_STORED_LINK_KEY_COMMAND,                                          v1_1,          "C.121",            "E"}, 
    // C.121: Mandatory if the Write Stored Link Key command is supported, otherwise excluded
    {HCI_ENABLE_IMPLEMENTATION_UNDER_TEST_MODE_COMMAND,                           v1_1,          "C.123",            "E"}, 
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
    {HCI_READ_NUMBER_OF_SUPPORTED_IAC_COMMAND,                                    v1_1,          "C.125",            "E"}, 
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
    {HCI_SNIFF_MODE_COMMAND,                                                      v1_1,          "C.214",            "E"}, 
    // C.214: Mandatory if Sniff mode is supported, otherwise excluded
    {HCI_WRITE_AUTHENTICATION_ENABLE_COMMAND,                                     v1_1,          "O",                "E"},
    {HCI_WRITE_AUTOMATIC_FLUSH_TIMEOUT_COMMAND,                                   v1_1,          "M",                "E"},
    {HCI_WRITE_CLASS_OF_DEVICE_COMMAND,                                           v1_1,          "M",                "E"},
    {HCI_WRITE_CURRENT_IAC_LAP_COMMAND,                                           v1_1,          "C.125",            "E"}, 
    // C.125: Mandatory if Inquiry Scan is supported, otherwise excluded
    {HCI_SWITCH_ROLE_COMMAND,                                                     v1_1,          "C.212",            "E"}, 
    // C.212: Mandatory if Role Switch is supported, otherwise excluded
    {HCI_WRITE_HOLD_MODE_ACTIVITY_COMMAND,                                        v1_1,          "C.213",            "E"}, 
    // C.213: Mandatory if Hold mode is supported, otherwise excluded
    {HCI_WRITE_INQUIRY_SCAN_ACTIVITY_COMMAND,                                     v1_1,          "C.125",            "E"}, 
    // C.125: Mandatory if Inquiry Scan is supported, otherwise excluded
    {HCI_WRITE_LINK_POLICY_SETTINGS_COMMAND,                                      v1_1,          "C.141",            "E"}, 
    // C.141: Mandatory if Role Switch, Hold mode, or Sniff mode is supported, otherwise excluded
    {HCI_WRITE_LINK_SUPERVISION_TIMEOUT_COMMAND,                                  v1_1,          "O",                "E"},
    {HCI_WRITE_LOCAL_NAME_COMMAND,                                                v1_1,          "M",                "E"},
    {HCI_WRITE_LOOPBACK_MODE_COMMAND,                                             v1_1,          "C.123",            "E"}, 
    // C.123: Mandatory if BR/EDR test mode is supported, otherwise excluded
    {HCI_WRITE_NUM_BROADCAST_RETRANSMISSIONS_COMMAND,                             v1_1,          "O",                "E"},
    {HCI_WRITE_PAGE_SCAN_ACTIVITY_COMMAND,                                        v1_1,          "M",                "E"},
    {HCI_WRITE_PAGE_TIMEOUT_COMMAND,                                              v1_1,          "M",                "E"},
    {HCI_WRITE_PIN_TYPE_COMMAND,                                                  v1_1,          "O",                "E"},
    {HCI_WRITE_SCAN_ENABLE_COMMAND,                                               v1_1,          "M",                "E"},
    {HCI_WRITE_STORED_LINK_KEY_COMMAND,                                           v1_1,          "O",                "E"},
    {HCI_WRITE_SYNCHRONOUS_FLOW_CONTROL_ENABLE_COMMAND,                           v1_1,          "C.135",            "E"}, 
    // C.135: Optional if SCO or eSCO is supported, otherwise excluded
    {HCI_WRITE_VOICE_SETTING_COMMAND,                                             v1_1,          "C.134",            "E"}, 
    // C.134: Mandatory if SCO or eSCO is supported, otherwise excluded


    /****************************************************BT VERSION 1.2*************************************************************/
    {HCI_READ_LOCAL_SUPPORTED_COMMANDS_COMMAND,                                   v1_2,          "M",                "M"},
    {HCI_WRITE_AFH_CHANNEL_ASSESSMENT_MODE_COMMAND,                               v1_2,          "C.140",            "C.58"}, 
    {HCI_READ_AFH_CHANNEL_ASSESSMENT_MODE_COMMAND,                                v1_2,          "C.140",            "C.58"}, 
    // C.140: Mandatory if the Controller supports AFH classification in either role or is an AFH capable Central; 
    // C.58: Mandatory if LE Feature (Channel Classification) is supported
    {HCI_READ_CLOCK_COMMAND,                                                      v1_2,          "O",                "E"},
    {HCI_ACCEPT_SYNCHRONOUS_CONNECTION_REQUEST_COMMAND,                           v1_2,         "C.134",             "E"}, 
    // C.134: Mandatory if SCO or eSCO is supported, otherwise excluded  
    {HCI_CREATE_CONNECTION_CANCEL_COMMAND,                                        v1_2,          "M",                "E"},
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
    {HCI_SETUP_SYNCHRONOUS_CONNECTION_COMMAND,                                    v1_2,          "C.134",            "E"}, 
    // C.134: Mandatory if SCO or eSCO is supported, otherwise excluded
    {HCI_SYNCHRONOUS_CONNECTION_CHANGED_EVENT,                                    v1_2,          "C.134",            "E"}, 
    // C.134: Mandatory if SCO or eSCO is supported, otherwise excluded
    {HCI_SYNCHRONOUS_CONNECTION_COMPLETE_EVENT,                                   v1_2,          "C.134",            "E"}, 
    // C.134: Mandatory if SCO or eSCO is supported, otherwise excluded
    {HCI_WRITE_DEFAULT_LINK_POLICY_SETTINGS_COMMAND,                              v1_2,          "C.141",            "E"}, 
    // C.141: Mandatory if Role Switch, Hold mode, or Sniff mode is supported, otherwise excluded
    {HCI_WRITE_INQUIRY_MODE_COMMAND,                                              v1_2,          "C.146",            "E"}, 
    // C.146: Mandatory if the Extended Inquiry Result event or the IO Capability Request event is supported, otherwise optional if Inquiry is supported
    {HCI_WRITE_INQUIRY_SCAN_TYPE_COMMAND,                                         v1_2,          "C.125",            "E"}, 
    // C.125: Mandatory if Inquiry Scan is supported, otherwise excluded
    {HCI_WRITE_PAGE_SCAN_TYPE_COMMAND,                                            v1_2,          "C.154",            "E"}, 
    // C.154: Mandatory if Interlaced Page Scan is supported, otherwise optional

        /****************************************************BT VERSION 2.1*************************************************************/
    {HCI_ENCRYPTION_KEY_REFRESH_COMPLETE_EVENT,                                   v2_1_EDR,      "M",                "C.4"}, 
    // C.4: Mandatory if LE Feature (LE Encryption) is supported
    {HCI_ENHANCED_FLUSH_COMMAND,                                                  v2_1_EDR,      "M",                "E"},
    {HCI_ENHANCED_FLUSH_COMPLETE_EVENT,                                           v2_1_EDR,      "M",                "E"},
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
    {HCI_SIMPLE_PAIRING_COMPLETE_EVENT,                                           v2_1_EDR,      "M",                "E"},
    {HCI_SNIFF_SUBRATING_COMMAND,                                                 v2_1_EDR,      "C.221221",         "E"}, 
    // C.221: Mandatory if Sniff subrating is supported, otherwise excluded
    {HCI_SNIFF_SUBRATING_EVENT,                                                   v2_1_EDR,      "C.221",            "E"}, 
    // C.221: Mandatory if Sniff subrating is supported, otherwise excluded
    {HCI_USER_CONFIRMATION_REQUEST_EVENT,                                         v2_1_EDR,      "M",                "E"},
    {HCI_USER_CONFIRMATION_REQUEST_NEGATIVE_REPLY_COMMAND,                        v2_1_EDR,      "M",                "E"},
    {HCI_USER_CONFIRMATION_REQUEST_REPLY_COMMAND,                                 v2_1_EDR,      "M",                "E"},
    {HCI_USER_PASSKEY_NOTIFICATION_EVENT,                                         v2_1_EDR,      "M",                "E"},
    {HCI_USER_PASSKEY_REQUEST_EVENT,                                              v2_1_EDR,      "M",                "E"},
    {HCI_USER_PASSKEY_REQUEST_NEGATIVE_REPLY_COMMAND,                             v2_1_EDR,      "M",                "E"},
    {HCI_USER_PASSKEY_REQUEST_REPLY_COMMAND,                                      v2_1_EDR,      "M",                "E"},
    {HCI_WRITE_DEFAULT_ERRONEOUS_DATA_REPORTING_COMMAND,                          v2_1_EDR,      "C.206",            "E"}, 
    // C.206: Mandatory if Erroneous Synchronous Data Reporting is supported, otherwise excluded
    {HCI_WRITE_EXTENDED_INQUIRY_RESPONSE_COMMAND,                                 v2_1_EDR,      "C.205",            "E"}, 
    // C.205: Mandatory if Extended Inquiry Response is supported, otherwise excluded
    {HCI_WRITE_INQUIRY_TRANSMIT_POWER_LEVEL_COMMAND,                              v2_1_EDR,      "C.127",            "E"}, 
    // C.127: Mandatory if Inquiry is supported, otherwise excluded
    {HCI_WRITE_SIMPLE_PAIRING_DEBUG_MODE_COMMAND,                                 v2_1_EDR,      "M",                "E"},
    {HCI_WRITE_SIMPLE_PAIRING_MODE_COMMAND,                                       v2_1_EDR,      "M",                "E"},

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

    {HCI_WRITE_FLOW_CONTROL_MODE_COMMAND,                                         v3_0_HS,       "C.124",            "E"}, 
    // C.124: Mandatory if Data block based flow control is supported, otherwise excluded


    /****************************************************BT VERSION CSA2*************************************************************/
    {HCI_READ_LOCAL_SUPPORTED_CODECS_COMMAND,                                     CSA2,          "[v1] C.157 [v2] O","[v1] E [v2] O"}, 
    // C.157: Mandatory if the Read Local Supported Codecs command [v2] is supported, otherwise optional
    {HCI_ENHANCED_ACCEPT_SYNCHRONOUS_CONNECTION_REQUEST_COMMAND,                  CSA2,          "C.135",            "E"}, 
    // C.135: Optional if SCO or eSCO is supported, otherwise excluded
    {HCI_ENHANCED_SETUP_SYNCHRONOUS_CONNECTION_COMMAND,                           CSA2,          "C.135",            "E"}, 
    // C.135: Optional if SCO or eSCO is supported, otherwise excluded

    /****************************************************BT VERSION CSA3*************************************************************/
    {HCI_SET_MWS_TRANSPORT_LAYER_COMMAND,                                         CSA3,          "C.109",            "C.109"},
    {HCI_GET_MWS_TRANSPORT_LAYER_CONFIGURATION_COMMAND,                           CSA3,          "C.109",            "C.109"}, 
    // C.109: Mandatory if the Set MWS Signaling command is supported, otherwise excluded
    {HCI_SET_EXTERNAL_FRAME_CONFIGURATION_COMMAND,                                CSA3,          "C.108",            "O"}, 
    // C.108: Mandatory if the Set MWS_PATTERN Configuration command is supported, otherwise optional
    {HCI_SET_MWS_CHANNEL_PARAMETERS_COMMAND,                                      CSA3,          "O",                "O"},
    {HCI_SET_MWS_SCAN_FREQUENCY_TABLE_COMMAND,                                    CSA3,          "O",                "O"},
    {HCI_SET_MWS_SIGNALING_COMMAND,                                               CSA3,          "O",                "O"},
    {HCI_SET_MWS_PATTERN_CONFIGURATION_COMMAND,                                   CSA3,          "C.136",            "E"}, 
    // C.136: Optional if Slot Availability Mask is supported, otherwise excluded

    /****************************************************BT VERSION CSA4*************************************************************/
    {HCI_CONNECTIONLESS_PERIPHERAL_BROADCAST_CHANNEL_MAP_CHANGE_EVENT,            CSA4,          "C.201",            "E"}, 
    // C.201: Mandatory if Connectionless Peripheral Broadcast - Transmitter is supported, otherwise excluded
    {HCI_CONNECTIONLESS_PERIPHERAL_BROADCAST_RECEIVE_EVENT,                       CSA4,          "C.202",            "E"}, 
    // C.202: Mandatory if Connectionless Peripheral Broadcast - Receiver is supported, otherwise excluded
    {HCI_CONNECTIONLESS_PERIPHERAL_BROADCAST_TIMEOUT_EVENT,                       CSA4,          "C.202",            "E"},
    // C.202: Mandatory if Connectionless Peripheral Broadcast - Receiver is supported, otherwise excluded
    {HCI_DELETE_RESERVED_LT_ADDR_COMMAND,                                         CSA4,          "C.201",            "E"}, 
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
    {HCI_SET_RESERVED_LT_ADDR_COMMAND,                                            CSA4,          "C.201",            "E"}, 
    // C.201: Mandatory if Connectionless Peripheral Broadcast - Transmitter is supported, otherwise excluded
    {HCI_SET_TRIGGERED_CLOCK_CAPTURE_COMMAND,                                     CSA4,          "O",                "E"},
    {HCI_START_SYNCHRONIZATION_TRAIN_COMMAND,                                     CSA4,          "C.201",            "E"}, 
    // C.201: Mandatory if Connectionless Peripheral Broadcast - Transmitter is supported, otherwise excluded
    {HCI_SYNCHRONIZATION_TRAIN_COMPLETE_EVENT,                                    CSA4,          "C.201",            "E"}, 
    // C.201: Mandatory if Connectionless Peripheral Broadcast - Transmitter is supported, otherwise excluded
    {HCI_SYNCHRONIZATION_TRAIN_RECEIVED_EVENT,                                    CSA4,          "C.202",            "E"}, 
    // C.202: Mandatory if Connectionless Peripheral Broadcast - Receiver is supported, otherwise excluded
    {HCI_TRIGGERED_CLOCK_CAPTURE_EVENT,                                           CSA4,          "C.110",            "E"}, 
    // C.110: Mandatory if the Set Triggered Clock Capture command is supported, otherwise excluded
    {HCI_TRUNCATED_PAGE_CANCEL_COMMAND,                                           CSA4,          "C.129",            "E"}, 
    // C.129: Mandatory if Truncated page state is supported, otherwise excluded
    {HCI_TRUNCATED_PAGE_COMMAND,                                                  CSA4,          "C.129",            "E"}, 
    // C.129: Mandatory if Truncated page state is supported, otherwise excluded
    {HCI_TRUNCATED_PAGE_COMPLETE_EVENT,                                           CSA4,          "C.129",            "E"}, 
    // C.129: Mandatory if Truncated page state is supported, otherwise excluded
    {HCI_WRITE_SYNCHRONIZATION_TRAIN_PARAMETERS_COMMAND,                          CSA4,          "C.201",            "E"}, 
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
    // C.3: Mandatory if the LE Controller supports Connection State; 
    // C.55: Optional if LE Feature (Connected Isochronous Stream - Central), or LE Feature (Connected Isochronous Stream - Peripheral), or LE Feature (Synchronized Receiver role) is supported
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
    {HCI_WRITE_LE_HOST_SUPPORT_COMMAND,                                           v4_0,          "C.153",            "E"}, 
    // C.153: Mandatory if LE supported in the Controller, otherwise optional

    /****************************************************BT VERSION 4.1*************************************************************/
    {HCI_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_EVENT,                            v4_1,          "E",                "C.6"}, 
    {HCI_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_NEGATIVE_REPLY_COMMAND,           v4_1,          "E",                "C.6"}, 
    {HCI_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_REPLY_COMMAND,                    v4_1,          "E",                "C.6"}, 
    // C.6: Mandatory if LE Feature (Connection Parameters Request procedure) is supported, otherwise excluded
    {HCI_AUTHENTICATED_PAYLOAD_TIMEOUT_EXPIRED_EVENT,                             v4_1,          "C.155",            "C.155"}, 
    {HCI_READ_AUTHENTICATED_PAYLOAD_TIMEOUT_COMMAND,                              v4_1,          "C.155",            "C.155"}, 
    // C.155: Mandatory if the Write Authenticated Payload Timeout command is supported, otherwise excluded
    {HCI_WRITE_AUTHENTICATED_PAYLOAD_TIMEOUT_COMMAND,                             v4_1,          "C.151",            "C.7"}, 
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
    {HCI_WRITE_SECURE_CONNECTIONS_HOST_SUPPORT_COMMAND,                           v4_1,          "C.218",            "E"}, 
    // C.218: Mandatory if Secure Connections (Controller) is supported, otherwise excluded
    {HCI_WRITE_EXTENDED_INQUIRY_LENGTH_COMMAND,                                   v4_1,          "C.128",            "E"}, 
    // C.128: Optional if Inquiry is supported, otherwise excluded
    {HCI_WRITE_EXTENDED_PAGE_TIMEOUT_COMMAND,                                     v4_1,          "O",                "E"},
    {HCI_WRITE_SECURE_CONNECTIONS_TEST_MODE_COMMAND,                              v4_1,          "C.138",            "E"}, 
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
    {HCI_LE_TRANSMIT_POWER_REPORTING_EVENT,                                       v5_2,          "E",                "C.51"},
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
    {HCI_CONFIGURE_DATA_PATH_COMMAND,                                             v5_2,          "C.156",            "C.156"}, 
    // C.156: Mandatory if the Read Local Supported Codecs command [v2] is supported, otherwise excluded
    {HCI_SET_ECOSYSTEM_BASE_INTERVAL_COMMAND,                                     v5_2,          "O",                "O"},

    /****************************************************BT VERSION 5.3*************************************************************/
    {HCI_LE_SET_DATA_RELATED_ADDRESS_CHANGES_COMMAND,                             v5_3,          "E",                "C.10"}, 
    // C.10: Optional if LE Feature (LL Privacy) is supported, otherwise excluded
    {HCI_LE_SET_DEFAULT_SUBRATE_COMMAND,                                          v5_3,          "E",                "C.57"}, 
    {HCI_LE_SUBRATE_CHANGE_EVENT,                                                 v5_3,          "E",                "C.57"}, 
    {HCI_LE_SUBRATE_REQUEST_COMMAND,                                              v5_3,          "E",                "C.57"}, 
    // C.57: Mandatory if LE Feature (Connection Subrating) is supported, otherwise excluded    
    {HCI_SET_MIN_ENCRYPTION_KEY_SIZE_COMMAND,                                     v5_3,          "O",                "E"},


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


