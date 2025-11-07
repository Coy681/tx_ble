/*
 * ca.h
 *
 *  Created on: 2025年11月5日
 *      Author: Admin
 */

#ifndef LL_CA_H_
#define LL_CA_H_

#include"common/txCommon.h"
#include"../ll_feature.h"

typedef enum
{
	PPM_0_TO_20    = 20,
	PPM_21_TO_30   = 30,
	PPM_31_TO_50   = 50,
	PPM_51_TO_75   = 75,
	PPM_76_TO_100  = 100,
	PPM_101_TO_150 = 150,
	PPM_151_TO_250 = 250,
	PPM_251_TO_500 = 500,
}ll_conn_sca_e;

typedef enum
{
	LL_CA_TYPE_ACTIVE,
	LL_CA_TYPE_SLEEP,
}ll_ca_type_e;;

_u32 ll_ca_cal_window_winden(ll_ca_type_e type,_u32 peerPPM,_u32 localPPM,_u32 lastSync,_u32 windowEnd);

#endif /* LL_CA_H_ */
