/*
 * adv_packet.h
 *
 *  Created on: 2025年7月5日
 *      Author: Admin
 */

#ifndef BLE_CONTROLLER_LL_PACKET_ADV_PACKET_H_
#define BLE_CONTROLLER_LL_PACKET_ADV_PACKET_H_

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
	_u8 advData[2];//0-31 byte
}adv_type_scan_ind_t;

typedef struct _PACKED{
	_u8 advA[6];
	_u8 scanRsp[2];//0-31 byte
}adv_type_scan_rsp_t;

#endif /* BLE_CONTROLLER_LL_PACKET_ADV_PACKET_H_ */
