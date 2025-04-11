
#include"common/txCommon.h"

/** HCI command packet format
 *  0        4        8        12        16        20        24        28         31
 * --------------------------------------------------------------------------------
 * |           OPCODE(16bit)              |        8bit      |         nbit       |
 * ---------------------------------------|------------------|---------------------
 * |       OCF(10bit)      |   OGF(6bit)  |       length     |      parameter     |
 * --------------------------------------------------------------------------------
 */

 typedef struct _PACKED
 {
	 union
	 {
		 _u16 opcode;
		 struct
		 {
			_u16 ocf:10; //opcode command field
			_u16 ogf:6;  //opcode group field,0x3e reserved for specification development,0x3f reserved for vendor-specific debug.
		 };
	 };
    _u8  length; //fix size in each specific command.
    _u8  data[1];
 }bt_hci_command_t;

 /** HCI acl data packet format
 *  0        4        8        12        16        20        24        28         31
 * --------------------------------------------------------------------------------
 * |           12bit            |2bit|2bit|                 16bit                 |
 * -----------------------------|----|----|----------------------------------------
 * |          HANDLE            | PB | BC |                 length                |       
 * --------------------------------------------------------------------------------
 *                                  data(nbit)                                    |
 * --------------------------------------------------------------------------------
 * 
 * 
 */

typedef enum
{
    BT_HCI_ACL_PACKET_BOUNDARY_FLAG_START_NON_FLUSHABLE = 0x00,//First non-automatically-flushable packet,used in le from host to controller. LLID = 0b00
    BT_HCI_ACL_PACKET_BOUNDARY_FLAG_CONTINUE_FRAGMENT   = 0x01,//continue fragment,used in le from host to controller and controller to host. LLID = 0b01
    BT_HCI_ACL_PACKET_BOUNDARY_FLAG_START_FLUSHABLE     = 0x02,//First automatically flushable packet,used in le form controller to host.
    BT_HCI_ACL_PACKET_BOUNDARY_FLAG_MAX                 = 0x03,
}bt_hci_acl_data_packet_boundary_flag_e;

typedef enum
{
    BT_HCI_ACL_BROADCAST_FLAG_POINT_TO_POINT      = 0x00,//used in le
    BT_HCI_ACL_BROADCAST_FLAG_BR_EDR_BROADCAST    = 0x01,//used in BR/EDR
    BT_HCI_ACL_BROADCAST_FLAG_MAX                 = 0x02,
}bt_hci_acl_data_broadcast_flag_e;


 typedef struct _PACKED
 {
    _u16 handle:12; //0x000->0xeff;
    _u16 pbFlag:2;  //packet boundary flag.
    _u16 bcFlag:2;  //broadcast flag.
    _u16 length;    
 }bt_hci_acl_data_t;

/** HCI synchronous data packet format
 *  0        4        8        12        16        20        24        28         31
 * --------------------------------------------------------------------------------
 * |           12bit            |2bit|2bit|       8bit        |       nbit        |
 * -----------------------------|----|----|----------------------------------------
 * |    connection_handle       | PS | RFU|      length       |       data        |
 * --------------------------------------------------------------------------------
 */

typedef enum 
{
    BT_HCI_SYNCHRONOUS_PACKET_STATUS_CORRECTLY_RECEIVED = 0x00,
    BT_HCI_SYNCHRONOUS_PACKET_STATUS_POSSIBLY_VALID     = 0x01,
    BT_HCI_SYNCHRONOUS_PACKET_STATUS_NO_DATA_RECEIVED   = 0x02,
    BT_HCI_SYNCHRONOUS_PACKET_STATUS_PARTIALLY_LOST     = 0x03,
}bt_hci_sychronous_packet_status_e;

 typedef struct _PACKED
 {
    _u16 handle:12;
    _u16 psFlag:2; //packet status,search in 'bt_hci_sychronous_packet_status_e'
    _u16 rfu:2;
    _u8  length;
    _u8  data[1];
 }bt_hci_synchronous_data_t;

/** HCI event packet format
 *  0        4        8        12        16        20        24        28         31
 * --------------------------------------------------------------------------------
 * |       8bit       |      8bit         |                  nbit                 | 
 * -------------------|-------------------|---------------------------------------|
 * |    event code    |      length       |                parameter              |
 * --------------------------------------------------------------------------------
 */

 typedef struct _PACKED
 {
    _u8 eventCode;
    _u8 length;
    _u8 parameter[2];
 }bt_hci_event_t;

/** HCI ISO data packet format
 *  0         4          8         12          16          20          24          28           31
 * ----------------------------------------------------------------------------------------------
 * |            12bit              |2bit| 1 | 1 |                  14bit                  |2bit ｜
 * --------------------------------|----|---|---|-----------------------------------------|-----|
 * |      connection_handle        | PB |TS |RFU|                 length                  | RFU |
 * ----------------------------------------------------------------------------------------------
 *                                    time_stamp(32bit)                                         |
 * ----------------------------------------------------------------------------------------------
 *      packet sequence number(16bit)           |       ISO SDU Length(12bit)      | RFU 2| PS 2| 
 * ----------------------------------------------------------------------------------------------
 *                                  ISO SDU Fragment(nbit)                                      | 
 * ----------------------------------------------------------------------------------------------
 */

typedef enum
{
    BT_HCI_ISO_PB_FLAG_FIRST_FRAGMENT_SDU        = 0x00,
    BT_HCI_ISO_PB_FLAG_INTERMEDIATE_FRAGMENT_SDU = 0x01,
    BT_HCI_ISO_PB_FLAG_COMPLETE_SDU              = 0x02,
    BT_HCI_ISO_PB_FLAG_LAST_FRAGMENT_SDU         = 0x03,
}bt_hci_iso_data_pb_flag_e;

typedef enum
{
    BT_HCI_ISO_TIME_STAMP_NOT_PRESENT  =  0x00,
    BT_HCI_ISO_TIME_STAMP_PRESENT      =  0x01,
}bt_hci_iso_data_ts_flag_e;

 typedef struct _PACKED
 {
    _u16 handle:12;
    _u16 PB:2;     //search in 'bt_hci_iso_data_pb_flag_e'
    _u16 TS:1;     //search in 'bt_hci_iso_data_ts_flag_e'
    _u16 RFU:1;
    _u16 length:14;
    _u16 RFU1:2;

    _u8 data[0];
 }bt_hci_iso_data_t;

 typedef struct _PACKED
 {
    _u32 timeStamp;

    _u16 sequenceNumber;
    _u16 isoDataLength:12;
    _u16 RFU:2;
    _u16 psFlag:2;

    _u8  data[0];
 }bt_iso_sub_data_with_ts_pb_t;

 typedef struct _PACKED
 {
    _u16 sequenceNumber;
    _u16 isoDataLength:12;
    _u16 RFU:2;
    _u16 psFlag:2;

    _u8  data[0];
 }bt_iso_sub_data_with_pb_t;




 void hci_acl_data_packet_process(_u8* data);

 void hci_iso_packet_process(_u8* data);
