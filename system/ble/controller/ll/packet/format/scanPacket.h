/*
 * scanPacket.h
 *
 *  Created on: 2025年7月5日
 *      Author: Admin
 */

#ifndef BLE_CONTROLLER_LL_PACKET_FORMAT_SCANPACKET_H_
#define BLE_CONTROLLER_LL_PACKET_FORMAT_SCANPACKET_H_

typedef struct _PACKED{
	_u8 scanA[6];
	_u8 advA[6];
}scan_type_scan_req_t,scan_type_aux_scan_req_t;

#endif /* BLE_CONTROLLER_LL_PACKET_FORMAT_SCANPACKET_H_ */
