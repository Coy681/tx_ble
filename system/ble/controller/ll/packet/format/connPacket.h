/*
 * connPacket.h
 *
 *  Created on: 2025年7月5日
 *      Author: Admin
 */

#ifndef BLE_CONTROLLER_LL_PACKET_FORMAT_CONNPACKET_H_
#define BLE_CONTROLLER_LL_PACKET_FORMAT_CONNPACKET_H_

typedef struct _PACKED{
    _u32 AA;

    _u32 crcInit:24;
    _u32 winSize:8;

    _u16 winOffset;
    _u16 interval;

    _u16 latency;
    _u16 timeout;

    _u8 chM[5];
    _u8 hop:5;
    _u8 sca:3;
}init_connectInd_llData_t;



#endif /* BLE_CONTROLLER_LL_PACKET_FORMAT_CONNPACKET_H_ */
