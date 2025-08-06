/*
 * adv_packet.h
 *
 *  Created on: 2025年7月5日
 *      Author: Admin
 */

#ifndef BLE_CONTROLLER_LL_PACKET_ADV_PACKET_H_
#define BLE_CONTROLLER_LL_PACKET_ADV_PACKET_H_

/******************adv pdus*********************/
typedef struct _PACKED{
	_u8 advA[6];
	_u8 advData[2];//0-31 byte
}adv_type_ind_t;

typedef struct _PACKED{
	_u8 advA[6];
	_u8 targetA[6];
}adv_type_direct_ind_t;

typedef struct _PACKED{
	_u8 advA[6];
	_u8 advData[2];//0-31 byte
}adv_type_nonConn_ind_t;

typedef struct _PACKED{
	_u8 advA[6];
	_u8 scanRsp[2];//0-31 byte
}adv_type_scan_rsp_t;

/******************adv extended*********************/
typedef enum
{
	ADV_EXTENDED_HEADER_FLAG_ADV_A     = BIT(0),
	ADV_EXTENDED_HEADER_FLAG_TARGET_A  = BIT(1),
	ADV_EXTENDED_HEADER_FLAG_CTE_INFO  = BIT(2),
	ADV_EXTENDED_HEADER_FLAG_ADI       = BIT(3),
	ADV_EXTENDED_HEADER_FLAG_AUX_PTR   = BIT(4),
	ADV_EXTENDED_HEADER_FLAG_SYNC_INFO = BIT(5),
	ADV_EXTENDED_HEADER_FLAG_TX_POWER  = BIT(6),
	ADV_EXTENDED_HEADER_FLAG_RSVD      = BIT(7),
}adv_extended_header_flag_e;

typedef enum
{
	ADV_EXTENDED_MODE_NON_CONNECTABLE_NON_SCANNABLE = 0,
	ADV_EXTENDED_MODE_CONNECTABLE_NON_SCANNABLE     = 1,
	ADV_EXTENDED_MODE_NON_CONNECTABLE_SCANNABLE     = 2,
	ADV_EXTENDED_MODE_RSVD                          = 3,
}adv_extended_mode_e;

typedef struct _PACKED{
	_u8 len:6;
	_u8 advMode:2;
	_u8 flags;
	_u8 param[0];
}adv_extended_header_t;

typedef struct _PACKED{
	_u8 advA[6];
}adv_extended_header_subfield_advA_t;

typedef struct _PACKED{
	_u8 targetA[6];
}adv_extended_header_subfield_targetA_t;

typedef struct _PACKED{
	_u8 info;
}adv_extended_header_subfield_cteInfo_t;

typedef struct _PACKED{
	_u16 did:12;
	_u16 sid:4;
}adv_extended_header_subfield_adi_t;

typedef enum
{
	ADV_EXTENDED_AUX_PTR_OFFSET_UNIT_30US  = 0,
	ADV_EXTENDED_AUX_PTR_OFFSET_UNIT_300US = 1,
}adv_extended_header_auxPtr_offsetUnit_e;

typedef enum
{
	ADV_EXTENDED_AUX_PTR_CA_0_50PPM        = 1,
	ADV_EXTENDED_AUX_PTR_CA_51_500PPM      = 0,
}adv_extended_header_auxPtr_offsetUnit_e;

typedef struct _PACKED{
	_u8  chnInx:6;
	_u8  ca:1;//clock accuracy,0:51-500ppm,1:0-50ppm
	_u8  offsetUnits:1;//0:30us,1:300us
	_u16 auxOffset:13;
	_u16 auxPhy:3;
}adv_extended_header_subfield_auxPtr_t;

typedef struct _PACKED{
	_u16 offsetBase:13;
	_u16 offsetUnits:1;
	_u16 offsetAdjust:1;
	_u16 rsvd:1;

	_u16 interval;
	_u32 chML;
	_u8  chMH:5;
	_u8  sca:3;
	_u32 AA;
	_u8  crcInit[3];
	_u16 eventCounter;
}adv_extended_header_subfield_syncInfo_t;

typedef struct _PACKED{
	_u8 power;
}adv_extended_header_subfield_Tx_Power_t;

/******************scan pdus********************/
typedef struct _PACKED{
	_u8 advA[6];
	_u8 advData[2];//0-31 byte
}adv_type_scan_ind_t;



#endif /* BLE_CONTROLLER_LL_PACKET_ADV_PACKET_H_ */
