/********************advertising channel pdu*******************
 *                 ## Advertising PDU ##
 *  ---------------------------------------------------
 *  | Header |                Payload                 | 
 *  | 16 bit |              1-255 octets              |
 *  ---------------------------------------------------
 * 
 *                 ## Advertising PDU Header##
 *  ---------------------------------------------------
 *  | PDU type | RFU | ChSel | TxAdd | RxAdd | Length |
 *  |   4bit   | 1bit| 1bit  | 1bit  | 1bit  |  8bit  |
 *  ---------------------------------------------------
 */


typedef enum
{
    BLE_ADV_CHANNEL_PDU_TYPE_ADV_IND              = 0,//0b0000,Primary   advertising,transmited or received on LE 1M PHY
    BLE_ADV_CHANNEL_PDU_TYPE_ADV_DIRECT_IND       = 1,//0b0001,Primary   advertising,transmited or received on LE 1M PHY
    BLE_ADV_CHANNEL_PDU_TYPE_ADV_NON_CONN_IND     = 2,//0b0010,Primary   advertising,transmited or received on LE 1M PHY

    BLE_ADV_CHANNEL_PDU_TYPE_SCAN_REQ             = 3,//0b0011,Primary   advertising,transmited or received on LE 1M PHY
    BLE_ADV_CHANNEL_PDU_TYPE_AUX_SCAN_REQ         = 3,//0b0011,Secondary advertising,transmited or received on LE 1M PHY,LE 2M PHY,LE CODED PHY

    BLE_ADV_CHANNEL_PDU_TYPE_SCAN_RSP             = 4,//0b0100,Primary   advertising,transmited or received on LE 1M PHY

    BLE_ADV_CHANNEL_PDU_TYPE_CONNECT_IND          = 5,//0b0101,Primary   advertising,transmited or received on LE 1M PHY
    BLE_ADV_CHANNEL_PDU_TYPE_AUX_CONNECT_REQ      = 5,//0b0101,Secondary advertising,transmited or received on LE 1M PHY,LE 2M PHY,LE CODED PHY

    BLE_ADV_CHANNEL_PDU_TYPE_SCAN_IND             = 6,//0b0110,Primary   advertising,transmited or received on LE 1M PHY

    BLE_ADV_CHANNEL_PDU_TYPE_ADV_EXT_IND          = 7,//0b0111,Primary   advertising,transmited or received on LE 1M PHY,LE CODED PHY
    BLE_ADV_CHANNEL_PDU_TYPE_AUX_ADV_IND          = 7,//0b0111,Secondary advertising,transmited or received on LE 1M PHY,LE 2M PHY,LE CODED PHY
    BLE_ADV_CHANNEL_PDU_TYPE_AUX_SCAN_RSP         = 7,//0b0111,Secondary advertising,transmited or received on LE 1M PHY,LE 2M PHY,LE CODED PHY
    BLE_ADV_CHANNEL_PDU_TYPE_AUX_SYNC_IND         = 7,//0b0111,Periodic,             transmited or received on LE 1M PHY,LE 2M PHY,LE CODED PHY
    BLE_ADV_CHANNEL_PDU_TYPE_AUX_CHAIN_IND        = 7,//0b0111,Secondary advertising,transmited or received on LE 1M PHY,LE 2M PHY,LE CODED PHY
    BLE_ADV_CHANNEL_PDU_TYPE_AUX_SYNC_SUBEVENT_IND= 7,//0b0111,Periodic,             transmited or received on LE 1M PHY,LE 2M PHY,LE CODED PHY
    BLE_ADV_CHANNEL_PDU_TYPE_AUX_SYNC_SUBEVENT_RSP= 7,//0b0111,Periodic,             transmited or received on LE 1M PHY,LE 2M PHY,LE CODED PHY

    BLE_ADV_CHANNEL_PDU_TYPE_AUX_CONNECT_RSP      = 8,//0b1000,Secondary advertising,transmited or received on LE 1M PHY,LE 2M PHY,LE CODED PHY
    BLE_ADV_CHANNEL_PDU_TYPE_ADV_DECISION_IND     = 9,//0b1001,Primary   advertising,transmited or received on LE 1M PHY,LE CODED PHY
}ble_adv_channel_pdu_type_e;