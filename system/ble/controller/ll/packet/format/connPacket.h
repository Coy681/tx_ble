/*
 * connPacket.h
 *
 *  Created on: 2025年7月5日
 *      Author: Admin
 */

#ifndef BLE_CONTROLLER_LL_PACKET_FORMAT_CONNPACKET_H_
#define BLE_CONTROLLER_LL_PACKET_FORMAT_CONNPACKET_H_

#include"../../ll_feature.h"

/**
 * Core Spec,Volume 6,parr- Low Energy Controller, LL Control PDU define
 */

//2.4.2.1 LL_CONNECTION_UPDATE_IND
typedef struct _PACKED{
    _u8  winSize;
    _u16 winOffset;
    _u16 interval;
    _u16 latency;
    _u16 timeout;
    _u16 instant;
}ll_connection_update_ind;

// 2.4.2.2 LL_CHANNEL_MAP_IND
typedef struct _PACKED{
    _u8  chM[5];
    _u16 instant;
}ll_channel_map_ind;

// 2.4.2.3 LL_TERMINATE_IND
typedef struct _PACKED{
    _u8  errorCode;
}ll_terminate_ind;

// 2.4.2.4 LL_ENC_REQ
typedef struct _PACKED{
    _u8  rand[8];
    _u16 ediv;
    _u8  skd_c[8];
    _u8  iv_c[4];
}ll_enc_req;

// 2.4.2.5 LL_ENC_RSP
typedef struct _PACKED{
    _u8  skd_p[8];
    _u8  iv_p[4];
}ll_enc_rsp;

// 2.4.2.6 LL_START_ENC_REQ
typedef struct _PACKED{
}ll_start_enc_req;

// 2.4.2.7 LL_START_ENC_RSP
typedef struct _PACKED{
}ll_start_enc_rsp;

// 2.4.2.8 LL_UNKNOWN_RSP
typedef struct _PACKED{
    _u8  unknownType;
}ll_unknown_rsp;

// 2.4.2.9 LL_FEATURE_REQ
typedef struct _PACKED{
    _u8  featureSet[8];
}ll_feature_req;

// 2.4.2.10 LL_FEATURE_RSP
typedef struct _PACKED{
    _u8  featureSet[8];
}ll_feature_rsp;

// 2.4.2.11 LL_PAUSE_ENC_REQ
typedef struct _PACKED{
}ll_pause_enc_req;

// 2.4.2.12 LL_PAUSE_ENC_RSP
typedef struct _PACKED{
}ll_pause_enc_rsp;

// 2.4.2.13 LL_VERSION_IND
typedef struct _PACKED{
    _u8  version;
    _u16 company_identifier;
    _u16 subversion;
}ll_version_ind;

// 2.4.2.14 LL_REJECT_IND
typedef struct _PACKED{
    _u8  errorCode;
}ll_reject_ind;

// 2.4.2.15 LL_PERIPHERAL_FEATURE_REQ
typedef struct _PACKED{
    _u8  featureSet[8];
}ll_peripheral_feature_req;

// 2.4.2.16 LL_CONNECTION_PARAM_REQ
typedef struct _PACKED{
    _u16 interval_min;
    _u16 interval_max;
    _u16 latency;
    _u16 timeout;
    _u8  preferredPeriodicity;
    _u16 referenceConnEventCount;
    _u16 offset0;
    _u16 offset1;
    _u16 offset2;
    _u16 offset3;
    _u16 offset4;
    _u16 offset5;
}ll_connection_param_req;

// 2.4.2.17 LL_CONNECTION_PARAM_RSP
typedef struct _PACKED{
    _u16 interval_min;
    _u16 interval_max;
    _u16 latency;
    _u16 timeout;
    _u8  preferredPeriodicity;
    _u16 referenceConnEventCount;
    _u16 offset0;
    _u16 offset1;
    _u16 offset2;
    _u16 offset3;
    _u16 offset4;
    _u16 offset5;
}ll_connection_param_rsp;

// 2.4.2.18 LL_REJECT_EXT_IND
typedef struct _PACKED{
    _u8  rejectOpcode;
    _u8  errorCode;
}ll_reject_ext_ind;

// 2.4.2.19 LL_PING_REQ
typedef struct _PACKED{
}ll_ping_req;

// 2.4.2.20 LL_PING_RSP
typedef struct _PACKED{
}ll_ping_rsp;

// 2.4.2.21 LL_LENGTH_REQ
typedef struct _PACKED{
    _u16 maxRxOctets;
    _u16 maxRxTime;
    _u16 maxTxOctets;
    _u16 maxTxTime;
}ll_length_req;

// 2.4.2.21 LL_LENGTH_RSP
typedef struct _PACKED{
    _u16 maxRxOctets;
    _u16 maxRxTime;
    _u16 maxTxOctets;
    _u16 maxTxTime;
}ll_length_rsp;

// 2.4.2.22 LL_PHY_REQ
typedef struct _PACKED{
    _u8  tx_phys;
    _u8  rx_phys;
}ll_phy_req;

// 2.4.2.22 LL_PHY_RSP
typedef struct _PACKED{
    _u8  tx_phys;
    _u8  rx_phys;
}ll_phy_rsp;

// 2.4.2.23 LL_PHY_UPDATE_IND
typedef struct _PACKED{
    _u8  phy_c_to_p;
    _u8  phy_p_to_c;
    _u16 instant;
}ll_phy_update_ind;

// 2.4.2.24 LL_MIN_USED_CHANNELS_IND
typedef struct _PACKED{
    _u8  phys;
    _u8  minUsedChannels;
}ll_min_used_channels_ind;

// 2.4.2.25 LL_CTE_REQ
typedef struct _PACKED{
    _u8  minCTELenReq : 5;
    _u8  rfu : 1;
    _u8  cteTypeReq : 2;
}ll_cte_req;

// 2.4.2.26 LL_CTE_RSP
typedef struct _PACKED{
}ll_cte_rsp;

// 2.4.2.27 LL_PERIODIC_SYNC_IND
typedef struct _PACKED{
    _u16 id;
    _u8  syncInfo[18];
    _u16 connEventCount;
    _u16 lastPaEventCounter;
    _u8  sid : 4;
    _u8  aType : 1;
    _u8  sca : 3;
    _u8  phy;
    _u8  advA[6];
    _u16 syncConnEventCount;
}ll_periodic_sync_ind;

// 2.4.2.28 LL_CLOCK_ACCURACY_REQ
typedef struct _PACKED{
    _u8  sca;
}ll_clock_accuracy_req;

// 2.4.2.28 LL_CLOCK_ACCURACY_RSP
typedef struct _PACKED{
    _u8  sca;
}ll_clock_accuracy_rsp;

// 2.4.2.29 LL_CIS_REQ
typedef struct _PACKED{
    _u8  cig_id;
    _u8  cis_id;
    _u8  phy_c_to_p;
    _u8  phy_p_to_c;
    _u16 max_sdu_c_to_p : 12;
    _u16 rfu:2;
    _u8  framingMode : 1;
    _u8  framed : 1;
    _u16 max_sdu_p_to_c : 12;
    _u8  rfu1 : 4;
    _u32 sdu_Interval_C_To_P : 20;
    _u32 rfu2 : 4;
    _u32 sduIntervalPToC : 20;
    _u32 rfu3 : 4;
    _u16 max_pdu_c_to_p;
    _u16 max_pdu_p_to_c;
    _u8  nse;
    _u32 sub_interval;
    _u8  bn_c_to_p;
    _u8  bn_p_to_c;
    _u8  ft_c_to_p;
    _u8  ft_p_to_c;
    _u16 iso_interval;
    _u32 cisOffsetMin;
    _u32 cisOffsetMax;
    _u16 connEventCount;
}ll_cis_req;

// 2.4.2.30 LL_CIS_RSP
typedef struct _PACKED{
    _u32 cisOffsetMin;
    _u32 cisOffsetMax;
    _u16 connEventCount;
}ll_cis_rsp;

// 2.4.2.31 LL_CIS_IND
typedef struct _PACKED{
    _u32 aa;
    _u32 cisOffset;
    _u32 cig_sync_delay;
    _u32 cis_sync_delay;
    _u16 connEventCount;
}ll_cis_ind;

// 2.4.2.32 LL_CIS_TERMINATE_IND
typedef struct _PACKED{
    _u8  cig_id;
    _u8  cis_id;
    _u8  errorCode;
}ll_cis_terminate_ind;

// 2.4.2.33 LL_POWER_CONTROL_REQ
typedef struct _PACKED{
    _u8  phy;
    int8_t delta;
    int8_t txPower;
}ll_power_control_req;

// 2.4.2.34 LL_POWER_CONTROL_RSP
typedef struct _PACKED{
    _u8  min : 1;
    _u8  max : 1;
    _u8  rfu : 6;
    int8_t delta;
    int8_t txPower;
    _u8  apr;
}ll_power_control_rsp;

// 2.4.2.35 LL_POWER_CHANGE_IND
typedef struct _PACKED{
    _u8  phy;
    _u8  min : 1;
    _u8  max : 1;
    _u8  rfu : 6;
    int8_t delta;
    int8_t txPower;
}ll_power_change_ind;

// 2.4.2.36 LL_SUBRATE_REQ
typedef struct _PACKED{
    _u16 subrateFactorMin;
    _u16 subrateFactorMax;
    _u16 max_latency;
    _u16 continuationNumber;
    _u16 timeout;
}ll_subrate_req;

// 2.4.2.37 LL_SUBRATE_IND
typedef struct _PACKED{
    _u16 subrateFactor;
    _u16 subrateBaseEvent;
    _u16 latency;
    _u16 continuationNumber;
    _u16 timeout;
}ll_subrate_ind;

// 2.4.2.38 LL_CHANNEL_REPORTING_IND
typedef struct _PACKED{
    _u8  enable;
    _u8  minSpacing;
    _u8  maxDelay;
}ll_channel_reporting_ind;

// 2.4.2.39 LL_CHANNEL_STATUS_IND
typedef struct _PACKED{
    _u16 channelClassification[37];
}ll_channel_status_ind;

// 2.4.2.40 LL_PERIODIC_SYNC_WR_IND
typedef struct _PACKED{
    ll_periodic_sync_ind periodicSyncInd;
    _u32 rspAa;
    _u8  numSubevents;
    _u8  subeventInterval;
    _u8  responseSlotDelay;
    _u8  responseSlotSpacing;
}ll_periodic_sync_wr_ind;

// 2.4.2.41 LL_FEATURE_EXT_REQ
typedef struct _PACKED{
    _u8  maxPage;
    _u8  pageNumber;
    _u8  featurePage[24];
}ll_feature_ext_req;

// 2.4.2.41 LL_FEATURE_EXT_RSP
typedef struct _PACKED{
    _u8  maxPage;
    _u8  pageNumber;
    _u8  featurePage[24];
}ll_feature_ext_rsp;

// 2.4.2.42 LL_CS_SEC_REQ
typedef struct _PACKED{
    _u8  cs_iv_c[8];
    _u8  cs_in_c[4];
    _u8  cs_pv_c[8];
}ll_cs_sec_req;

// 2.4.2.43 LL_CS_SEC_RSP
typedef struct _PACKED{
    _u8  cs_iv_p[8];
    _u8  cs_in_p[4];
    _u8  cs_pv_p[8];
}ll_cs_sec_rsp;

// 2.4.2.44 LL_CS_CAPABILITIES_REQ
typedef struct _PACKED{
    _u8  modeTypes;
    _u8  rttCapability;
    _u16 rttAaOnlyN;
    _u16 rttSoundingN;
    _u16 rttRandomSequenceN;
    _u16 nadmSoundingCapability;
    _u16 nadmRandomSequenceCapability;
    _u8  csSyncPhyCapability;
    _u8  numAnt : 4;
    _u8  maxAntPath : 4;
    _u8  role : 1;
    _u8  noFae : 1;
    _u8  channelSelection3c : 1;
    _u8  rfu1 : 5;
    _u8  soundingPctEstimate : 1;
    _u8  numConfigs;
    _u16 maxProceduresSupported;
    _u8  tSw;
    _u16 tIp1Capability;
    _u16 tIp2Capability;
    _u16 tFcsCapability;
    _u16 tPmCapability;
    _u8  txSnrCapability : 7;
    _u8  rfu2 : 1;
}ll_cs_capabilities_req;

// 2.4.2.44 LL_CS_CAPABILITIES_RSP
typedef struct _PACKED{
    _u8  modeTypes;
    _u8  rttCapability;
    _u16 rttAaOnlyN;
    _u16 rttSoundingN;
    _u16 rttRandomSequenceN;
    _u16 nadmSoundingCapability;
    _u16 nadmRandomSequenceCapability;
    _u8  csSyncPhyCapability;
    _u8  numAnt : 4;
    _u8  maxAntPath : 4;
    _u8  role : 1;
    _u8  noFae : 1;
    _u8  channelSelection3c : 1;
    _u8  rfu1 : 5;
    _u8  soundingPctEstimate : 1;
    _u8  numConfigs;
    _u16 maxProceduresSupported;
    _u8  tSw;
    _u16 tIp1Capability;
    _u16 tIp2Capability;
    _u16 tFcsCapability;
    _u16 tPmCapability;
    _u8  txSnrCapability : 7;
    _u8  rfu2 : 1;
}ll_cs_capabilities_rsp;

// 2.4.2.45 LL_CS_CONFIG_REQ
typedef struct _PACKED{
    _u8  configId : 6;
    _u8  action : 2;
    _u8  chM[10];
    _u8  chMRepetition;
    _u8  mainMode;
    _u8  subMode;
    _u8  mainModeMinSteps;
    _u8  mainModeMaxSteps;
    _u8  mainModeRepetition;
    _u8  mode0Steps;
    _u8  csSyncPhy;
    _u8  rttType;
    _u8  role : 2;
    _u8  chSel : 4;
    _u8  rfu1 : 2;
    _u8  ch3cShape : 4;
    _u8  rfu2 : 4;
    _u8  ch3cJump;
    _u8  tIp1;
    _u8  tIp2;
    _u8  tFcs;
    _u8  tPm;
}ll_cs_config_req;

// 2.4.2.46 LL_CS_CONFIG_RSP
typedef struct _PACKED{
    _u8  configId : 6;
    _u8  status : 2;
}ll_cs_config_rsp;

// 2.4.2.47 LL_CS_REQ
typedef struct _PACKED{
    _u8  configId : 6;
    _u8  rfu1 : 2;
    _u16 connEventCount;
    _u32 offsetMin;
    _u32 offsetMax;
    _u16 maxProcedureLen;
    _u16 eventInterval;
    _u8  subeventsPerEvent;
    _u16 subeventInterval;
    _u32 subeventLen;
    _u16 procedureInterval;
    _u16 procedureCount;
    _u8  aci;
    _u8  preferredPeerAnt;
    _u8  phy;
    int8_t pwrDelta;
    _u8  txSnrI : 4;
    _u8  txSnrR : 4;
}ll_cs_req;

// 2.4.2.48 LL_CS_RSP
typedef struct _PACKED{
    _u8  configId : 6;
    _u8  rfu1 : 2;
    _u16 connEventCount;
    _u32 offsetMin;
    _u32 offsetMax;
    _u16 eventInterval;
    _u8  subeventsPerEvent;
    _u16 subeventInterval;
    _u32 subeventLen;
    _u16 procedureInterval;
    _u16 procedureCount;
    _u8  aci;
    _u8  preferredPeerAnt;
    _u8  phy;
    int8_t pwrDelta;
    _u8  txSnrI : 4;
    _u8  txSnrR : 4;
}ll_cs_rsp;

// 2.4.2.49 LL_CS_IND
typedef struct _PACKED{
    _u8  configId : 6;
    _u8  rfu1 : 2;
    _u16 connEventCount;
    _u32 offset;
    _u16 eventInterval;
    _u8  subeventsPerEvent;
    _u16 subeventInterval;
    _u32 subeventLen;
    _u8  aci;
    _u8  phy;
    int8_t pwrDelta;
    _u8  rfu2;
}ll_cs_ind;

// 2.4.2.50 LL_CS_TERMINATE_REQ
typedef struct _PACKED{
    _u8  configId : 6;
    _u8  rfu1 : 2;
    _u16 procCount;
    _u8  errorCode;
}ll_cs_terminate_req;

// 2.4.2.50 LL_CS_TERMINATE_RSP
typedef struct _PACKED{
    _u8  configId : 6;
    _u8  rfu1 : 2;
    _u16 procCount;
    _u8  errorCode;
}ll_cs_terminate_rsp;

// 2.4.2.51 LL_CS_FAE_REQ
typedef struct _PACKED{
}ll_cs_fae_req;

// 2.4.2.52 LL_CS_FAE_RSP
typedef struct _PACKED{
    int8_t chFae[79];
}ll_cs_fae_rsp;

// 2.4.2.53 LL_CS_CHANNEL_MAP_IND
typedef struct _PACKED{
    _u8  chM[10];
    _u16 instant;
}ll_cs_channel_map_ind;

// 2.4.2.54 LL_FRAME_SPACE_REQ
typedef struct _PACKED{
    _u16 fsMin;
    _u16 fsMax;
    _u8  phys;
    _u16 spacingTypes;
}ll_frame_space_req;

// 2.4.2.55 LL_FRAME_SPACE_RSP
typedef struct _PACKED{
    _u16 fs;
    _u8  phys;
    _u16 spacingTypes;
}ll_frame_space_rsp;

#if(LL_SUPPORT_CHANNEL_SOUNDING)
#define LL_CONTROL_PDU_MAX_LENGTH      73
#elif(LL_SUPPORT_PERIODIC_ADVERTISING_WITH_RESPONSES_SCANNER||LL_SUPPORT_PERIODIC_ADVERTISING_WITH_RESPONSES_ADVERTISER)
#define LL_CONTROL_PDU_MAX_LENGTH      45
#elif(LL_SUPPORT_CONNECTED_ISOCHRONOUS_STREAM_CENTRAL||BLE_SUPPORT_CONNECTED_ISOCHRONOUS_STREAM_PERIPHERAL)
#define LL_CONTROL_PDU_MAX_LENGTH      37
#else
#define LL_CONTROL_PDU_MAX_LENGTH      27
#endif


#endif /* BLE_CONTROLLER_LL_PACKET_FORMAT_CONNPACKET_H_ */
