#include"common/txCommon.h"
#include"ll_feature.h"
 #ifndef LL_H_
 #define LL_H_

/***********************Bluetooth LE Controller code**************************/
typedef enum
{
    SUCCESS                                           = 0x00,
    UNKNOWN_HCI_COMMAND                               = 0x01,
    UNKNOWN_CONNECTION_IDENTIFIER                     = 0x02,
    HARDWARE_FAILURE                                  = 0x03,
    PAGE_TIMEOUT                                      = 0x04,
    AUTHENTICATION_FAILURE                            = 0x05,
    PIN_OR_KEY_MISSING                                = 0x06,
    MEMORY_CAPACITY_EXCEEDED                          = 0x07,
    CONNECTION_TIMEOUT                                = 0x08,
    CONNECTION_LIMIT_EXCEEDED                         = 0x09,
    SYNCHRONOUS_CONNECTION_LIMIT_TO_A_DEVICE_EXCEEDED = 0x0A,
    CONNECTION_ALREADY_EXISTS                         = 0x0B,
    COMMAND_DISALLOWED                                = 0x0C,
    CONNECTION_REJECTED_DUE_TO_LIMITED_RESOURCES      = 0x0D,
    CONNECTION_REJECTED_DUE_TO_SECURITY_REASONS       = 0x0E,
    CONNECTION_REJECTED_DUE_TO_UNACCEPTABLE_BD_ADDR   = 0x0F,
    CONNECTION_ACCEPT_TIMEOUT_EXCEEDED                = 0x10,
    UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE            = 0x11,
    IVALID_HCI_COMMAND_PARAMETERS                     = 0x12,
    REMOTE_USER_TERMINATED_CONNNECTION                = 0x13,
    REMOTE_DEVICE_TERMINATED_DUE_TO_LOW_RESOURCES     = 0x14,
    REMOTE_DEVICE_TERMINATED_DUE_TO_POWER_OFF         = 0x15,
    CONNECTION_TERMINATED_BY_LOCAL_HOST               = 0x16,
    REPEATED_ATTEMPTS                                 = 0x17,
    PAIRING_NOT_ALLOWED                               = 0x18,
    UNKNOWN_LMP_PDU                                   = 0x19,
    UNSUPPORTED_REMOTE_FEATURE                        = 0x1A,
    SCO_OFFSET_REJECTED                               = 0x1B,
    SCO_INTERVAL_REJECTED                             = 0x1C,
    SCO_AIR_MODE_REJECTED                             = 0x1D,
    INVALID_LL_PARAMETERS                             = 0x1E,
    UNSPECIFIED_ERROR                                 = 0x1F,
    UNSUPPORTED_LL_PARAMETER_VALUE                    = 0x20,
    ROLE_CHANGE_NOT_ALLOWED                           = 0x21,
    LL_RESPONSE_TIMEOUT                               = 0x22,
    LL_PROCEDURE_COLLISION                            = 0x23,
    LMP_PDU_NOT_ALLOWED                               = 0x24,
    ENCRYPTION_MODE_NOT_ACCEPTABLE                    = 0x25,
    LINK_KEY_CANNOT_BE_CHANGED                        = 0x26,
    REQUESTED_QOS_NOT_SUPPORTED                       = 0x27,
    INSTANT_PASSED                                    = 0x28,
    PAIRING_WITH_UNIT_KEY_NOT_SUPPORTED               = 0x29,
    DIFFERENT_TRANSACTION_COLLISION                   = 0x2A,
    RESERVED_FOR_FUTURE_USE_0                         = 0x2B,
    QOS_UNACCEPTABLE_PARAMETER                        = 0x2C,
    QOS_REJECTED                                      = 0x2D,
    CHANNEL_CLASSIFICATION_NOT_SUPPORTED              = 0x2E,
    INSUFFICIENT_SECURITY                             = 0x2F,
    PARAMETER_OUT_OF_MANDATORY_RANGE                  = 0x30,
    RESERVED_FOR_FUTURE_USE_1                         = 0x31,
    ROLE_SWITCH_PENDING                               = 0x32,
    RESERVED_FOR_FUTURE_USE_2                         = 0x33,
    RESERVED_SLOT_VIOLATION                           = 0x34,
    ROLE_SWITCH_FAILED                                = 0x35,
    EXTENDED_INQUIRY_RESPONSE_TOO_LARGE               = 0x36,
    SECURE_SIMPLE_PAIRING_NOT_SUPPORTED_BY_HOST       = 0x37,
    HOST_BUSY_PAIRING                                 = 0x38,
    CONNECTION_REJECTED_DUE_TO_NO_SUITABLE_CHANNEL    = 0x39,
    CONTROLLER_BUSY                                   = 0x3A,
    UNACCEPTABLE_CONNECTION_PARAMETERS                = 0x3B,
    ADVERTISING_TIMEOUT                               = 0x3C,
    CONNECTION_TERMINATED_DUE_TO_MIC_FAILURE          = 0x3D,
    CONNECTION_FAILED_TO_ESTABLISHED                  = 0x3E,
    PREVIOUSLY_USED                                   = 0x3F,
    COARSE_CLOCK_ADJUSTMENT_REJECTED_TRY_TO_ADJIUT    = 0x40,
    TYPE0_SUBMAP_NOT_DEFINED                          = 0x41,
    UNKNOWN_ADVERTISING_IDENTIFIER                    = 0x42,
    LIMIT_REACHED                                     = 0x43,
    OPERATION_CANCELLED_BY_HOST                       = 0x44,
    PACKET_TOO_LONG                                   = 0x45,
    TOO_LATE                                          = 0x46,
    TOO_EARLY                                         = 0x47,
    INSUFFICIENT_CHANNELS                             = 0x48,
}controller_error_code_e;


/***********************Bluetooth LE LL config **************************/
void ll_init_state_machine(_u8 number);

controller_error_code_e ll_reset(void);
controller_error_code_e ll_set_event_mask(_u64 eventMask);
controller_error_code_e ll_set_le_event_mask(_u64 eventMask);
void ll_get_feature(_u8* feature,_u8 len);

controller_error_code_e ll_add_device_to_filter_accept_list(_u8 addrType,_u8* addr);
controller_error_code_e ll_remove_device_from_filter_accept_list(_u8 addrType,_u8* addr);
controller_error_code_e ll_clear_filter_accept_list(void);

/***********************Bluetooth LE Advertising **************************/
typedef enum
{
    LL_ADV_IND                     = 0x00,//connectable and scannable undirected advertising,default.
    LL_ADV_DIRECT_IND_HIGH_DUTY    = 0x01,//connectable high duty directed advertising.
    LL_ADV_SCAN_IND                = 0x02,//scannable undirected advertising.
    LL_ADV_NONCONN_IND             = 0x03,//none connectable undirected advertising.
    LL_ADV_DIRECT_IND_LOW_DUTY     = 0x04,//connectable low duty directed advertising.
}ll_advertising_type_e;

typedef enum
{
    LL_PUBLIC_DEVICE_ADDRESS              = 0x00,
    LL_RANDOM_DEVICE_ADDRESS              = 0x01,
    LL_RESOLVABLE_PRIVATE_PUBLIC_ADDRESS  = 0x02,
    LL_RESOLVABLE_PRIVATE_RANDOM_ADDRESS  = 0x03,
}ll_own_address_type_e;

typedef enum
{
    LL_PUBLIC_DEVICE_OR_IDENTITY_ADDRESS  = 0x00,
    LL_RANDOM_DEVICE_OR_IDENTITY_ADDRESS  = 0x01,
}ll_peer_address_type_e;

typedef enum
{
    LL_FILTER_LIST_NOT_USE                        = 0x00,
    LL_FILTER_SCAN_REQUEST                        = 0x01,
    LL_FILTER_CONNECTION_REQUEST                  = 0x02,
    LL_FILTER_SCAN_REQUEST_AND_CONNECTION_REQUEST = 0x03,
}ll_advertising_filter_policy_e;

typedef enum
{
    LL_ADVERTISING_ENABLE  = 1,
    LL_ADVERTISING_DISABLE = 0,
}ll_advertising_enable_e;

typedef enum
{
    LL_ADV_CHN_37 = BIT(0),
    LL_ADV_CHN_38 = BIT(1),
    LL_ADV_CHN_39 = BIT(2),
}ll_advertising_chn_e;

#if(LL_SUPPORT_LE_EXTENDED_ADVERTISING!=1)
controller_error_code_e ll_set_advertising_parameters(_u16 interval,\
                                                     ll_advertising_type_e type,\
                                                     ll_own_address_type_e ownAddressType,\
                                                     ll_peer_address_type_e peerAddressType,\
                                                     _u8* peerAddress,_u8 channelMap,\
                                                     ll_advertising_filter_policy_e policy);                
                                                    
controller_error_code_e ll_set_advertising_data(_u8* data,_u8 length);

controller_error_code_e ll_set_scan_response_data(_u8* data,_u8 length);

controller_error_code_e ll_set_advertising_enable(_u8 enable);


/***********************Bluetooth LE Extended Advertising **************************/
#else
typedef enum
{
    LL_ADV_EVENT_PROPERTY_CONNECTED                 = BIT(0),
    LL_ADV_EVENT_PROPERTY_SCANNABLE                 = BIT(1),
    LL_ADV_EVENT_PROPERTY_DIRECTED                  = BIT(2),
    LL_ADV_EVENT_PROPERTY_HIGH_DUTY_CONNECTED       = BIT(3),
    LL_ADV_EVENT_PROPERTY_LEGACY_PDU                = BIT(4),
    LL_ADV_EVENT_PROPERTY_ANONYMOUS_ADV             = BIT(5),
    LL_ADV_EVENT_PROPERTY_INCLUDE_TX_POWER          = BIT(6),
    LL_ADV_EVENT_PROPERTY_DECISION_PDU              = BIT(7),
    LL_ADV_EVENT_PROPERTY_INCLUDE_ADVA_IN_DECISION  = BIT(8),
    LL_ADV_EVENT_PROPERTY_INCLUDE_ADI_IN_DECISION   = BIT(9),
}ll_advertising_event_property_e;

typedef enum
{
    LL_ADV_PHY_1M    = 0x01,
    LL_ADV_PHY_2M    = 0x02,
    LL_ADV_PHY_CODED = 0x03,
}ll_advertising_phy_e;

typedef enum
{
    LL_ADV_PHY_OPTIONS_NO_PREFERENCE = 0x00,
    LL_ADV_PHY_OPTIONS_PREFER_S2     = 0x01,
    LL_ADV_PHY_OPTIONS_PREFER_S8     = 0x02,
    LL_ADV_PHY_OPTIONS_REQUIRE_S2    = 0x03,
    LL_ADV_PHY_OPTIONS_REQUIRE_S8    = 0x04,
}ll_advertising_phy_options_e;

typedef enum
{
    LL_ADV_DATA_OPERATION_INTERMEDIATE_FRAGMENT= 0x00,
    LL_ADV_DATA_OPERATION_FIRST_FRAGMENT       = 0x01,
    LL_ADV_DATA_OPERATION_LAST_FRAGMENT        = 0x02,
    LL_ADV_DATA_OPERATION_COMPLETE             = 0x03,
    LL_ADV_DATA_OPERATION_UNCHANGED            = 0x04,//just update the advertising data DID
}ll_advertising_data_operation_e;

typedef enum
{
    LL_ADV_DATA_FEAGMENT_ALL_DATA              = 0x00,
    LL_ADV_DATA_NOT_OR_MINIMIZE_FRAGMENT       = 0x01,
}ll_advertising_data_fragment_perference_e;
 
typedef struct 
{
    _u8  advHandle;
    _u16 duration;//unit is 10ms,0x0000,no max duration,continue to advertising until host disable it
    _u8  maxEvents;//0x00:no max number,other2:max number of extended adv events number controller shall send
}ll_extended_adv_enable_subField_e;


typedef struct 
{
    _u8  advHandle;//0x00->0xEF,identify an advertising set
    _u16 advEventProperty;//search for "ll_advertising_event_property_e"
    _u8  filterPolicy;//search for "ll_advertising_filter_policy_e"

    _u8  txPower;//range is -127 to +20,unit is dBm,    0x7F:no preference
    _u8  advSid;//0x00-0x0F,SID subfield in the ADI field.
    _u8  ownAddrType;//search for "ll_own_address_type_e"
    _u8  peerAddrType;//search for "ll_peer_address_type_e"

    _u8  peerAddr[6];
    _u8  scanReqNotifyEnable;//0x00:disable,0x01:enable
    _u8  rsvd;

    _u32 primaryAdvInterval;//unit is 625us 
    _u8  primaryAdvChnMap;//bit field,search for "ll_advertising_chn_e"
    _u8  primaryAdvPhy;//LL_ADV_PHY_1M or LL_ADV_PHY_CODED,if legacy pdu,must 1M phy
    _u8  primaryAdvphyOptions;//search for "ll_advertising_phy_options_e"
    _u8  secondaryAdvMaxSkip;////0x00:AUX_ADV_IND shall be prior to the next advertising event.0x01-0xFF,maximum skip events
    _u8  secondaryAdvPhy;//search for "ll_advertising_phy_e"
    _u8  secondaryAdvphyOptions;//search for "ll_advertising_phy_options_e"
    _u16 rsvd1;
}ll_extended_adv_param_t;


controller_error_code_e ll_set_extended_advertising_parameters(ll_extended_adv_param_t* pParam);

controller_error_code_e ll_set_extended_advertising_data(_u8 advHandle,\
                                                         ll_advertising_data_operation_e operation,\
                                                         ll_advertising_data_fragment_perference_e fragPref,\
                                                        _u8 dataLen,\
                                                        _u8* data);

controller_error_code_e ll_set_extended_scan_response_data(_u8 advHandle,\
                                                            ll_advertising_data_operation_e operation,\
                                                            ll_advertising_data_fragment_perference_e fragPref,\
                                                            _u8 dataLen,\
                                                            _u8* data);

controller_error_code_e ll_set_extended_advertising_enable(_u8 enable,\
                                                           _u8 numSets,\
                                                           ll_extended_adv_enable_subField_e* pEnableSubFiled);

controller_error_code_e ll_set_adv_set_random_address(_u8 advHandle,_u8* address);

controller_error_code_e ll_read_maximum_advertising_data_length(_u16* length);

controller_error_code_e ll_read_number_of_supported_advertising_sets(_u8* number);

controller_error_code_e ll_remove_advertising_sets(_u8 advHandle);

controller_error_code_e ll_clear_advertising_sets(void);

#endif

/***********************Bluetooth LE Periodic Advertising **************************/
#if(LL_SUPPORT_LE_PERIODIC_ADVERTISING==1)
#endif

/***********************Bluetooth LE Periodic With Response Advertising **************************/
#if(LL_SUPPORT_PERIODIC_ADVERTISING_WITH_RESPONSES_ADVERTISER==1)
#endif

#endif
