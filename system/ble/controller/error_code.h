/*
 * error_code.h
 *
 *  Created on: 2025年4月11日
 *      Author: Admin
 */

#ifndef BLE_CONTROLLER_ERROR_CODE_H_
#define BLE_CONTROLLER_ERROR_CODE_H_

typedef enum{
    SUCCESS                                    = 0x00,
    UNKNOWN_HCI_COMMAND                        = 0x01,
    UNKNOWN_CONNECTION_IDENTIFIER              = 0x02,
    HARDWARE_FAILURE                           = 0x03,
    PAGE_TIMEOUT                               = 0x04,
    AUTHENTICATION_FAILURE                     = 0x05,
    PIN_OR_KEY_MISSING                         = 0x06,
    MEMORY_CAPACITY_EXCEEDED                   = 0x07,
    CONNECTION_TIMEOUT                         = 0x08,
    CONNECTION_LIMIT_EXCEEDED                  = 0x09,
    SYNC_CONNECTION_LIMIT_EXCEEDED             = 0x0A,
    CONNECTION_ALREADY_EXISTS                  = 0x0B,
    COMMAND_DISALLOWED                         = 0x0C,
    CONNECTION_REJECTED_LIMITED_RESOURCES      = 0x0D,
    CONNECTION_REJECTED_SECURITY_REASONS       = 0x0E,
    CONNECTION_REJECTED_UNACCEPTABLE_BD_ADDR   = 0x0F,
    CONNECTION_ACCEPT_TIMEOUT_EXCEEDED         = 0x10,
    UNSUPPORTED_FEATURE_PARAM_VALUE            = 0x11,
    IVALID_HCI_COMMAND_PARAMETERS              = 0x12,
    REMOTE_USER_TERMINATED_CONN = 0x13,
    REMOTE_DEV_TERMINATED_LOW_RESOURCES        = 0x14,
    REMOTE_DEV_TERMINATED_POWER_OFF            = 0x15,
    CONNECTION_TERMINATED_BY_LOCAL_HOST        = 0x16,
    REPEATED_ATTEMPTS                          = 0x17,
    PAIRING_NOT_ALLOWED                        = 0x18,
    UNKNOWN_LMP_PDU                            = 0x19,
    UNSUPPORTED_REMOTE_FEATURE                 = 0x1A,
    SCO_OFFSET_REJECTED                        = 0x1B,
    SCO_INTERVAL_REJECTED                      = 0x1C,
    SCO_AIR_MODE_REJECTED                      = 0x1D,
    INVALID_LMP_LL_PARAMETERS                  = 0x1E,
    UNSPECIFIED_ERROR                          = 0x1F,
    UNSUPPORTED_LMP_LL_PARAM_VALUE             = 0x20,
    ROLE_CHANGE_NOT_ALLOWED                    = 0x21,
    LMP_LL_RESPONSE_TIMEOUT                    = 0x22,
    LMP_LL_TRANSACTION_COLLISION               = 0x23,
    LMP_PDU_NOT_ALLOWED                        = 0x24,
    ENCRYPTION_MODE_NOT_ACCEPTABLE             = 0x25,
    LINK_KEY_CANNOT_CHANGE                     = 0x26,
    REQUESTED_QOS_NOT_SUPPORTED                = 0x27,
    INSTANT_PASSED                             = 0x28,
    PAIRING_WITH_UNIT_KEY_NOT_SUPPORTED        = 0x29,
    DIFFERENT_TRANSACTION_COLLISION            = 0x2A,
    RESERVED_FUTURE_USE_2B                     = 0x2B,
    QOS_UNACCEPTABLE_PARAMETER                 = 2C,
    QOS_REJECTED                               = 0x2D,
    CHANNEL_CLASSIFICATION_NOT_SUPPORTED       = 0x2E,
    INSUFFICIENT_SECURITY                      = 0x2F,
    PARAMETER_OUT_OF_MANDATORY_RANGE           = 0x30,
    ROLE_SWITCH_FAILED                         = 0x35,
    EXTENDED_INQUIRY_RESPONSE_TOO_LARGE        = 0x36,
    SECURE_SIMPLE_PAIRING_NOT_SUPPORTED        = 0x37,
    HOST_BUSY_PAIRING                          = 0x38,
    CONNECTION_REJECTED_NO_SUITABLE_CHANNEL    = 0x39,
    CONTROLLER_BUSY                            = 0x3A,
    UNACCEPTABLE_CONNECTION_PARAMETERS         = 0x3B,
    ADVERTISING_TIMEOUT                        = 0C,
    CONNECTION_TERMINATED_BY_COF_FAILURE       = 0x3D,
    CONNECTION_ESTABLISHMENT_FAILED            = 0x3E,
    COARSE_CLOCK_ADJUSTMENT_REJECTED           = 0x40,
    TYPE_SUBMAP_NOT_DEFINED                    = 0x41,
    LIMIT_REACHED                              = 0x43,
    OPERATION_CANCELLED_BY_HOST                = 0x44,
    PACKET_TOO_LONG                            = 0x46,
    TOO_LATE                                   = 0x47
}controller_error_code_e;

#endif /* BLE_CONTROLLER_ERROR_CODE_H_ */
