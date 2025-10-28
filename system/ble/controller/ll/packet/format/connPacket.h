/*
 * connPacket.h
 *
 *  Created on: 2025年7月5日
 *      Author: Admin
 */

#ifndef BLE_CONTROLLER_LL_PACKET_FORMAT_CONNPACKET_H_
#define BLE_CONTROLLER_LL_PACKET_FORMAT_CONNPACKET_H_

#include"../../ll_feature.h"
#include"common/txCommon.h"

typedef enum{
    LL_CTRL_PDU_CONNECTION_UPDATE_IND  = 0x00,
    LL_CTRL_PDU_CHANNEL_MAP_IND        = 0x01,
    LL_CTRL_PDU_TERMINATE_IND          = 0x02,
    LL_CTRL_PDU_ENC_REQ                = 0x03,
    LL_CTRL_PDU_ENC_RSP                = 0x04,
    LL_CTRL_PDU_START_ENC_REQ          = 0x05,
    LL_CTRL_PDU_START_ENC_RSP          = 0x06,
    LL_CTRL_PDU_UNKNOWN_RSP            = 0x07,
    LL_CTRL_PDU_FEATURE_REQ            = 0x08,
    LL_CTRL_PDU_FEATURE_RSP            = 0x09,
    LL_CTRL_PDU_PAUSE_ENC_REQ          = 0x0A,
    LL_CTRL_PDU_PAUSE_ENC_RSP          = 0x0B,
    LL_CTRL_PDU_VERSION_IND            = 0x0C,
    LL_CTRL_PDU_REJECT_IND             = 0x0D,
    LL_CTRL_PDU_PERIPHERAL_FEATURE_REQ = 0x0E,
    LL_CTRL_PDU_CONNECTION_PARAM_REQ   = 0x0F,
    LL_CTRL_PDU_CONNECTION_PARAM_RSP   = 0x10,
    LL_CTRL_PDU_REJECT_EXT_IND         = 0x11,
    LL_CTRL_PDU_PING_REQ               = 0x12,
    LL_CTRL_PDU_PING_RSP               = 0x13,
    LL_CTRL_PDU_LENGTH_REQ             = 0x14,
    LL_CTRL_PDU_LENGTH_RSP             = 0x15,
    LL_CTRL_PDU_PHY_REQ                = 0x16,
    LL_CTRL_PDU_PHY_RSP                = 0x17,
    LL_CTRL_PDU_PHY_UPDATE_IND         = 0x18,
    LL_CTRL_PDU_MIN_USED_CHANNELS_IND  = 0x19,
    LL_CTRL_PDU_CTE_REQ                = 0x1A,
    LL_CTRL_PDU_CTE_RSP                = 0x1B,
    LL_CTRL_PDU_PERIODIC_SYNC_IND      = 0x1C,
    LL_CTRL_PDU_CLOCK_ACCURACY_REQ     = 0x1D,
    LL_CTRL_PDU_CLOCK_ACCURACY_RSP     = 0x1E,
    LL_CTRL_PDU_CIS_REQ                = 0x1F,
    LL_CTRL_PDU_CIS_RSP                = 0x20,
    LL_CTRL_PDU_CIS_IND                = 0x21,
    LL_CTRL_PDU_CIS_TERMINATE_IND      = 0x22,
    LL_CTRL_PDU_POWER_CONTROL_REQ      = 0x23,
    LL_CTRL_PDU_POWER_CONTROL_RSP      = 0x24,
    LL_CTRL_PDU_POWER_CHANGE_IND       = 0x25,
    LL_CTRL_PDU_SUBRATE_REQ            = 0x26,
    LL_CTRL_PDU_SUBRATE_IND            = 0x27,
    LL_CTRL_PDU_CHANNEL_REPORTING_IND  = 0x28,
    LL_CTRL_PDU_CHANNEL_STATUS_IND     = 0x29,
    LL_CTRL_PDU_PERIODIC_SYNC_WR_IND   = 0x2A,
    LL_CTRL_PDU_FEATURE_EXT_REQ        = 0x2B,
    LL_CTRL_PDU_FEATURE_EXT_RSP        = 0x2C,
    LL_CTRL_PDU_CS_SEC_RSP             = 0x2D,
    LL_CTRL_PDU_CS_CAPABILITIES_REQ    = 0x2E,
    LL_CTRL_PDU_CS_CAPABILITIES_RSP    = 0x2F,
    LL_CTRL_PDU_CS_CONFIG_REQ          = 0x30,
    LL_CTRL_PDU_CS_CONFIG_RSP          = 0x31,
    LL_CTRL_PDU_CS_REQ                 = 0x32,
    LL_CTRL_PDU_CS_RSP                 = 0x33,
    LL_CTRL_PDU_CS_IND                 = 0x34,
    LL_CTRL_PDU_CS_TERMINATE_REQ       = 0x35,
    LL_CTRL_PDU_CS_FAE_REQ             = 0x36,
    LL_CTRL_PDU_CS_FAE_RSP             = 0x37,
    LL_CTRL_PDU_CS_CHANNEL_MAP_IND     = 0x38,
    LL_CTRL_PDU_CS_SEC_REQ             = 0x39,
    LL_CTRL_PDU_CS_TERMINATE_RSP       = 0x3A,
    LL_CTRL_PDU_FRAME_SPACE_REQ        = 0x3B,
    LL_CTRL_PDU_FRAME_SPACE_RSP        = 0x3C,
}ll_ctrlPdu_opcode_e;

/**
 * Core Spec,Volume 6,parr- Low Energy Controller, LL Control PDU define
 */

//2.4.2.1 LL_CTRL_PDU_CONNECTION_UPDATE_IND
typedef struct _PACKED{
    _u8  winSize;
    _u16 winOffset;
    _u16 interval;
    _u16 latency;
    _u16 timeout;
    _u16 instant;
}ll_ctrlPdu_connection_update_ind_t;

// 2.4.2.2 LL_CTRL_PDU_CHANNEL_MAP_IND
typedef struct _PACKED{
    _u8  chM[5];
    _u16 instant;
}ll_ctrlPdu_channel_map_ind_t;

// 2.4.2.3 LL_CTRL_PDU_TERMINATE_IND
typedef struct _PACKED{
    _u8  errorCode;
}ll_ctrlPdu_terminate_ind_t;

// 2.4.2.4 LL_CTRL_PDU_ENC_REQ
typedef struct _PACKED{
    _u8  rand[8];
    _u16 ediv;
    _u8  skd_c[8];
    _u8  iv_c[4];
}ll_ctrlPdu_enc_req_t;

// 2.4.2.5 LL_CTRL_PDU_ENC_RSP
typedef struct _PACKED{
    _u8  skd_p[8];
    _u8  iv_p[4];
}ll_ctrlPdu_enc_rsp_t;

// 2.4.2.6 LL_CTRL_PDU_START_ENC_REQ
typedef struct _PACKED{
}ll_ctrlPdu_start_enc_req_t;

// 2.4.2.7 LL_CTRL_PDU_START_ENC_RSP
typedef struct _PACKED{
}ll_start_enc_rsp_t;

// 2.4.2.8 LL_CTRL_PDU_UNKNOWN_RSP
typedef struct _PACKED{
    _u8  unknownType;
}ll_ctrlPdu_unknown_rsp_t;

// 2.4.2.9 LL_CTRL_PDU_FEATURE_REQ
typedef struct _PACKED{
    _u8  featureSet[8];
}ll_ctrlPdu_feature_req_t;

// 2.4.2.10 LL_CTRL_PDU_FEATURE_RSP
typedef struct _PACKED{
    _u8  featureSet[8];
}ll_ctrlPdu_feature_rsp_t;

// 2.4.2.11 LL_CTRL_PDU_PAUSE_ENC_REQ
typedef struct _PACKED{
}ll_ctrlPdu_pause_enc_req_t;

// 2.4.2.12 LL_CTRL_PDU_PAUSE_ENC_RSP
typedef struct _PACKED{
}ll_ctrlPdu_pause_enc_rsp_t;

// 2.4.2.13 LL_CTRL_PDU_VERSION_IND
typedef struct _PACKED{
    _u8  version;
    _u16 company_identifier;
    _u16 subversion;
}ll_ctrlPdu_version_ind_t;

// 2.4.2.14 LL_CTRL_PDU_REJECT_IND
typedef struct _PACKED{
    _u8  errorCode;
}ll_ctrlPdu_reject_ind_t;

// 2.4.2.15 LL_CTRL_PDU_PERIPHERAL_FEATURE_REQ
typedef struct _PACKED{
    _u8  featureSet[8];
}ll_ctrlPdu_peripheral_feature_req_t;

// 2.4.2.16 LL_CTRL_PDU_CONNECTION_PARAM_REQ
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
}ll_ctrlPdu_connection_param_req_t;

// 2.4.2.17 LL_CTRL_PDU_CONNECTION_PARAM_RSP
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
}ll_ctrlPdu_connection_param_rsp_t;

// 2.4.2.18 LL_CTRL_PDU_REJECT_EXT_IND
typedef struct _PACKED{
    _u8  rejectOpcode;
    _u8  errorCode;
}ll_ctrlPdu_reject_ext_ind_t;

// 2.4.2.19 LL_CTRL_PDU_PING_REQ
typedef struct _PACKED{
}ll_ctrlPdu_ping_req_t;

// 2.4.2.20 LL_CTRL_PDU_PING_RSP
typedef struct _PACKED{
}ll_ctrlPdu_ping_rsp_t;

// 2.4.2.21 LL_CTRL_PDU_LENGTH_REQ
typedef struct _PACKED{
    _u16 maxRxOctets;
    _u16 maxRxTime;
    _u16 maxTxOctets;
    _u16 maxTxTime;
}ll_ctrlPdu_length_req_t;

// 2.4.2.21 LL_CTRL_PDU_LENGTH_RSP
typedef struct _PACKED{
    _u16 maxRxOctets;
    _u16 maxRxTime;
    _u16 maxTxOctets;
    _u16 maxTxTime;
}ll_ctrlPdu_length_rsp_t;

// 2.4.2.22 LL_CTRL_PDU_PHY_REQ
typedef struct _PACKED{
    _u8  tx_phys;
    _u8  rx_phys;
}ll_ctrlPdu_phy_req_t;

// 2.4.2.22 LL_CTRL_PDU_PHY_RSP
typedef struct _PACKED{
    _u8  tx_phys;
    _u8  rx_phys;
}ll_ctrlPdu_phy_rsp_t;

// 2.4.2.23 LL_CTRL_PDU_PHY_UPDATE_IND
typedef struct _PACKED{
    _u8  phy_c_to_p;
    _u8  phy_p_to_c;
    _u16 instant;
}ll_ctrlPdu_phy_update_ind_t;

// 2.4.2.24 LL_CTRL_PDU_MIN_USED_CHANNELS_IND
typedef struct _PACKED{
    _u8  phys;
    _u8  minUsedChannels;
}ll_ctrlPdu_min_used_channels_ind_t;

// 2.4.2.25 LL_CTRL_PDU_CTE_REQ
typedef struct _PACKED{
    _u8  minCTELenReq : 5;
    _u8  rfu : 1;
    _u8  cteTypeReq : 2;
}ll_ctrlPdu_cte_req_t;

// 2.4.2.26 LL_CTRL_PDU_CTE_RSP
typedef struct _PACKED{
}ll_ctrlPdu_cte_rsp_t;

// 2.4.2.27 LL_CTRL_PDU_PERIODIC_SYNC_IND
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
}ll_ctrlPdu_periodic_sync_ind_t;

// 2.4.2.28 LL_CTRL_PDU_CLOCK_ACCURACY_REQ
typedef struct _PACKED{
    _u8  sca;
}ll_ctrlPdu_clock_accuracy_req_t;

// 2.4.2.28 LL_CTRL_PDU_CLOCK_ACCURACY_RSP
typedef struct _PACKED{
    _u8  sca;
}ll_ctrlPdu_clock_accuracy_rsp_t;

// 2.4.2.29 LL_CTRL_PDU_CIS_REQ
typedef struct _PACKED{
    _u8  cig_id;
    _u8  cis_id;
    _u8  phy_c_to_p;
    _u8  phy_p_to_c;
    _u16 max_sdu_c_to_p : 12;
    _u16 rfu:2;
    _u16 framing_mode : 1;
    _u16 framed : 1;
    _u16 max_sdu_p_to_c : 12;
    _u16 rfu1 : 4;
    _u8  sdu_interval_c_to_p[3];
    _u8  sdu_interval_p_to_c[3];
    _u16 max_pdu_c_to_p;
    _u16 max_pdu_p_to_c;
    _u8  nse;
    _u8  sub_interval[3];
    _u8  bn_c_to_p:4;
    _u8  bn_p_to_c:4;
    _u8  ft_c_to_p;
    _u8  ft_p_to_c;
    _u16 iso_interval;
    _u8  cisOffsetMin[3];
    _u8  cisOffsetMax[3];
    _u16 connEventCount;
}ll_ctrlPdu_cis_req_t;

// 2.4.2.30 LL_CTRL_PDU_CIS_RSP
typedef struct _PACKED{
    _u8  cisOffsetMin[3];
    _u8  cisOffsetMax[3];
    _u16 connEventCount;
}ll_ctrlPdu_cis_rsp_t;

// 2.4.2.31 LL_CTRL_PDU_CIS_IND
typedef struct _PACKED{
    _u32 aa;
    _u8  cis_offset[3];
    _u8  cig_sync_delay[3];
    _u8  cis_sync_delay[3];
    _u16 connEventCount;
}ll_ctrlPdu_cis_ind_t;

// 2.4.2.32 LL_CTRL_PDU_CIS_TERMINATE_IND
typedef struct _PACKED{
    _u8  cig_id;
    _u8  cis_id;
    _u8  errorCode;
}ll_ctrlPdu_cis_terminate_ind_t;

// 2.4.2.33 LL_CTRL_PDU_POWER_CONTROL_REQ
typedef struct _PACKED{
    _u8  phy;
    _u8  delta;
    _u8  txPower;
}ll_ctrlPdu_power_control_req_t;

// 2.4.2.34 LL_CTRL_PDU_POWER_CONTROL_RSP
typedef struct _PACKED{
    _u8  min : 1;
    _u8  max : 1;
    _u8  rfu : 6;
    _u8  delta;
    _u8  txPower;
    _u8  apr;
}ll_ctrlPdu_power_control_rsp_t;

// 2.4.2.35 LL_CTRL_PDU_POWER_CHANGE_IND
typedef struct _PACKED{
    _u8  phy;
    _u8  min : 1;
    _u8  max : 1;
    _u8  rfu : 6;
    _u8  delta;
    _u8  txPower;
}ll_ctrlPdu_power_change_ind_t;

// 2.4.2.36 LL_CTRL_PDU_SUBRATE_REQ
typedef struct _PACKED{
    _u16 subrateFactorMin;
    _u16 subrateFactorMax;
    _u16 max_latency;
    _u16 continuationNumber;
    _u16 timeout;
}ll_ctrlPdu_subrate_req_t;

// 2.4.2.37 LL_CTRL_PDU_SUBRATE_IND
typedef struct _PACKED{
    _u16 subrateFactor;
    _u16 subrateBaseEvent;
    _u16 latency;
    _u16 continuationNumber;
    _u16 timeout;
}ll_ctrlPdu_subrate_ind_t;

// 2.4.2.38 LL_CTRL_PDU_CHANNEL_REPORTING_IND
typedef struct _PACKED{
    _u8  enable;
    _u8  min_spacing;
    _u8  max_delay;
}ll_ctrlPdu_channel_reporting_ind_t;

// 2.4.2.39 LL_CTRL_PDU_CHANNEL_STATUS_IND
typedef struct _PACKED{
    _u8 channel_classification[10];
}ll_ctrlPdu_channel_status_ind_t;

// 2.4.2.40 LL_CTRL_PDU_PERIODIC_SYNC_WR_IND
typedef struct _PACKED{
    ll_ctrlPdu_periodic_sync_ind_t periodicSyncInd;
    _u32 rspAa;
    _u8  numSubevents;
    _u8  subeventInterval;
    _u8  responseSlotDelay;
    _u8  responseSlotSpacing;
}ll_ctrlPdu_periodic_sync_wr_ind_t;

// 2.4.2.41 LL_CTRL_PDU_FEATURE_EXT_REQ
typedef struct _PACKED{
    _u8  maxPage;
    _u8  pageNumber;
    _u8  featurePage[24];
}ll_ctrlPdu_feature_ext_req_t;

// 2.4.2.41 LL_CTRL_PDU_FEATURE_EXT_RSP
typedef struct _PACKED{
    _u8  maxPage;
    _u8  pageNumber;
    _u8  featurePage[24];
}ll_ctrlPdu_feature_ext_rsp_t;

// 2.4.2.42 LL_CTRL_PDU_CS_SEC_REQ
typedef struct _PACKED{
    _u8  cs_iv_c[8];
    _u8  cs_in_c[4];
    _u8  cs_pv_c[8];
}ll_ctrlPdu_cs_sec_req_t;

// 2.4.2.43 LL_CTRL_PDU_CS_SEC_RSP
typedef struct _PACKED{
    _u8  cs_iv_p[8];
    _u8  cs_in_p[4];
    _u8  cs_pv_p[8];
}ll_ctrlPdu_cs_sec_rsp_t;

// 2.4.2.44 LL_CTRL_PDU_CS_CAPABILITIES_REQ
typedef struct _PACKED{
    _u8  mode_types;
    _u8  rtt_capability;
    _u8  rtt_aa_only_n;
    _u8  rtt_sounding_n;
    _u8  rtt_random_sequence_n;
    _u16 nadm_sounding_capability;
    _u16 nadm_random_sequence_capability;
    _u8  cs_sync_phy_capability;
    _u8  num_ant : 4;
    _u8  max_ant_path : 4;
    _u8  role : 2;
    _u8  rfu:1;
    _u8  no_fae : 1;
    _u8  channel_selection_3c : 1;
    _u8  sounding_pct_estimate : 1;
    _u8  rfu1 : 2;
    _u8  num_configs;
    _u16 max_procedures_supported;
    _u8  t_sw;
    _u16 t_ip1_capability;
    _u16 t_ip2_capability;
    _u16 t_fcs_capability;
    _u16 t_pm_capability;
    _u8  rfu2 : 1;
    _u8  tx_snr_capability : 7;
}ll_ctrlPdu_cs_capabilities_req_t;

// 2.4.2.44 LL_CTRL_PDU_CS_CAPABILITIES_RSP
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
}ll_ctrlPdu_cs_capabilities_rsp_t;

// 2.4.2.45 LL_CTRL_PDU_CS_CONFIG_REQ
typedef struct _PACKED{
    _u8  config_id : 6;
    _u8  action : 2;
    _u8  chM[10];
    _u8  chM_repetition;
    _u8  main_mode;
    _u8  sub_mode;
    _u8  main_mode_min_steps;
    _u8  main_mode_max_steps;
    _u8  main_mode_repetition;
    _u8  mode_0_steps;
    _u8  cs_sync_phy;
    _u8  rtt_type:4;
    _u8  role : 2;
    _u8  rfu:2;
    _u8  chSel : 4;
    _u8  ch3cShape : 4;
    _u8  ch3cJump;
    _u8  t_ip1;
    _u8  t_ip2;
    _u8  t_fcs;
    _u8  t_pm;
    _u8  rfu1;
}ll_ctrlPdu_cs_config_req_t;

// 2.4.2.46 LL_CTRL_PDU_CS_CONFIG_RSP
typedef struct _PACKED{
    _u8  config_id : 6;
    _u8  rfu : 2;
}ll_ctrlPdu_cs_config_rsp_t;

// 2.4.2.47 LL_CTRL_PDU_CS_REQ
typedef struct _PACKED{
    _u8  config_id : 6;
    _u8  rfu : 2;
    _u16 connEventCount;
    _u8  offset_min[3];
    _u8  offset_max[3];
    _u16 max_procedure_len;
    _u16 event_interval;
    _u8  subevents_per_event;
    _u16 subevent_interval;
    _u8  subeventLen[3];
    _u16 procedure_interval;
    _u16 procedure_count;
    _u8  aci;
    _u8  preferred_peer_ant;
    _u8  phy;
    _u8  pwr_delta;
    _u8  tx_snr_i : 4;
    _u8  tx_snr_r : 4;
}ll_ctrlPdu_cs_req_t;

// 2.4.2.48 LL_CTRL_PDU_CS_RSP
typedef struct _PACKED{
    _u8  config_id : 6;
    _u8  rfu : 2;
    _u16 connEventCount;
    _u8  offset_min[3];
    _u8  offset_max[3];
    _u16 event_interval;
    _u8  subevents_per_Event;
    _u16 subevent_interval;
    _u8  subevent_len[3];
    _u8  aci;
    _u8  phy;
    _u8  pwr_delta;
    _u8  rfu1;
}ll_ctrlPdu_cs_rsp_t;

// 2.4.2.49 LL_CTRL_PDU_CS_IND
typedef struct _PACKED{
    _u8  config_id : 6;
    _u8  rfu : 2;
    _u16 connEventCount;
    _u8  offset[3];
    _u16 event_interval;
    _u8  subevents_per_event;
    _u16 subevent_interval;
    _u8  subevent_len[3];
    _u8  aci;
    _u8  phy;
    _u8  pwr_delta;
    _u8  rfu1;
}ll_ctrlPdu_cs_ind_t;

// 2.4.2.50 LL_CTRL_PDU_CS_TERMINATE_REQ
typedef struct _PACKED{
    _u8  config_id : 6;
    _u8  rfu : 2;
    _u16 procCount;
    _u8  errorCode;
}ll_ctrlPdu_cs_terminate_req_t;

// 2.4.2.50 LL_CTRL_PDU_CS_TERMINATE_RSP
typedef struct _PACKED{
    _u8  config_id : 6;
    _u8  rfu : 2;
    _u16 procCount;
    _u8  errorCode;
}ll_ctrlPdu_cs_terminate_rsp_t;

// 2.4.2.51 LL_CTRL_PDU_CS_FAE_REQ
typedef struct _PACKED{
}ll_ctrlPdu_cs_fae_req_t;

// 2.4.2.52 LL_CTRL_PDU_CS_FAE_RSP
typedef struct _PACKED{
    int8_t chFae[72];
}ll_ctrlPdu_cs_fae_rsp_t;

// 2.4.2.53 LL_CTRL_PDU_CS_CHANNEL_MAP_IND
typedef struct _PACKED{
    _u8  chM[10];
    _u16 instant;
}ll_ctrlPdu_cs_channel_map_ind_t;

// 2.4.2.54 LL_CTRL_PDU_FRAME_SPACE_REQ
typedef struct _PACKED{
    _u16 fs_min;
    _u16 fs_max;
    _u8  phys;
    _u16 spacing_types;
}ll_ctrlPdu_frame_space_req_t;

// 2.4.2.55 LL_CTRL_PDU_FRAME_SPACE_RSP
typedef struct _PACKED{
    _u16 fs;
    _u8  phys;
    _u16 spacing_types;
}ll_ctrlPdu_frame_space_rsp_t;

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
