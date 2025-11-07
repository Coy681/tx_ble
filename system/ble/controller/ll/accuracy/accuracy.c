/*
 * ca.c
 *
 *  Created on: 2025年11月5日
 *      Author: Admin
 */
#include"accuracy.h"
/**
 * jitter is instantaneous.
 */

/**
 * clock accuracy,is inherent
 */

/**
 * Active Clock Accuracy - clock accuracy when transmitting packets
 *               such as(1) adjacent packets in the same connection event
 *                      (2) packets in the same BIG or CIG events
 *                      (3) adv and scan process(with interaction)
 *                      (4) packets in the same pawr events
 *                      (5) cs steps within cs subevent
 *
 *
 * Sleep Clock Accuracy - average timing of all other activities.
 *
 */

/**
 * Active Clock Accuracy
 * (1)drift less than or equal to 50ppm
 * (2)instantaneous timing shall not deviate more than 2us from average timing.
 *
 * Sleep Clock Accuracy
 * (1)drift less than or equal to 500ppm
 * (2)instantaneous timing shall not deviate more than 16us form average timing
 */

/**
 * range delay
 * electromagnetic wave transmission speed is about 4ns per meter
 * when transmit packet,window widen should plus 4ns*distance
 */

/**
 * widow widen
 * transmitterAllowance = (txCA ÷ 1000000) × (receiveWindowEnd − timeOfLastSync) + Jµs
 * windowWidening = transmitterAllowance + receiverAllowance
 *
 * acl evcents: smaller than (connection/2 - T_MCES ),if reaches,acl lost
 * cis events : smaller than (iso_interval/2 - T_MSS_CIS),if reach,should terminate cis
 * bis events : smaller than (iso_interval/2 - T_MSS_150),if reach,should stop synchronization
 * if bis or cis events window widen reaches sub_interval,should terminate/stop sync.
 * cs procedure:smaller than (T_EVENT_INTERVAL/2 - T_MES),if reach,cs procedure should be aborted,and host shall be notified
 */

_RAM_CODE
_u32 ll_ca_cal_window_winden(ll_ca_type_e type,_u32 peerPPM,_u32 localPPM,_u32 lastSync,_u32 windowEnd)
{
	return ((peerPPM+localPPM)*(windowEnd - lastSync)/1000000 + (type == LL_CA_TYPE_ACTIVE?2:16));
}
