/*
 * rf.h
 *
 *  Created on: 2024年12月4日
 *      Author: 12407
 *
 *  RF hardware abstraction for BLE link layer: packet timing constants,
 *  radio configuration, timed tx/rx and irq callback registration.
 *  All timing macros are in us. All phy-dependent parameters use the
 *  phy index: 0=1M, 1=2M, 2=CODED_S2, 3=CODED_S8 (same as HAL_RF_MODE_x).
 */

#ifndef HAL_RF_H_
#define HAL_RF_H_
#include"tx_common.h"

/* PHY index used by all phy-parameterized APIs (same values as HAL_RF_MODE_x) */
#define HAL_RF_PHY_1M                           0
#define HAL_RF_PHY_2M                           1
#define HAL_RF_PHY_CODED_S2                     2
#define HAL_RF_PHY_CODED_S8                     3

//octet time for different phy, unit: us
#define RF_PACKET_OCTET_TIME_1M                8
#define RF_PACKET_OCTET_TIME_2M                4
#define RF_PACKET_OCTET_TIME_CODED_S2          16
#define RF_PACKET_OCTET_TIME_CODED_S8          64

//preamble len and time define, unit: us
#define RF_PACKET_PREAMBLE_LEN_1M              1
#define RF_PACKET_PREAMBLE_LEN_2M              2
#define RF_PACKET_PREAMBLE_LEN_CODED_S2        10
#define RF_PACKET_PREAMBLE_LEN_CODED_S8        10
#define RF_PACKET_PREAMBLE_TIME_1M             8
#define RF_PACKET_PREAMBLE_TIME_2M             8
#define RF_PACKET_PREAMBLE_TIME_CODED_S2       80
#define RF_PACKET_PREAMBLE_TIME_CODED_S8       80

//access code len and time define, unit: us
#define RF_PACKET_ACCESS_CODE_LEN              4
#define RF_PACKET_ACCESS_CODE_TIME_1M          32
#define RF_PACKET_ACCESS_CODE_TIME_2M          16
#define RF_PACKET_ACCESS_CODE_TIME_CODED_S2    256
#define RF_PACKET_ACCESS_CODE_TIME_CODED_S8    256

//crc time len and time define, unit: us
#define RF_PACKET_CRC_LEN                      3
#define RF_PACKET_CRC_TIME_1M                  24
#define RF_PACKET_CRC_TIME_2M                  12
#define RF_PACKET_CRC_TIME_CODED_S2            48
#define RF_PACKET_CRC_TIME_CODED_S8            192

//coded ci time define, unit: us
#define RF_PACKET_CI_TIME_CODED_S2             16
#define RF_PACKET_CI_TIME_CODED_S8             16

//term1 time define, unit: us
#define RF_PACKET_TERM1_TIME_CODED_S2          24
#define RF_PACKET_TERM1_TIME_CODED_S8          24

//term2 time define, unit: us
#define RF_PACKET_TERM2_TIME_CODED_S2          6
#define RF_PACKET_TERM2_TIME_CODED_S8          24

/*******************************config rf *********************************/

/**
 * @brief     This function is used to set the 4-byte BLE access code.
 * @param[in] accessCode - access code value, LSB transmitted first.
 * @return    none
 */
void hal_rf_set_access_code(_u32 accessCode);

/**
 * @brief     This function is used to set the 3-byte BLE CRC initial value.
 * @param[in] crc - crc init value, low 24 bits used.
 * @return    none
 */
void hal_rf_set_crc_value(_u32 crc);

/**
 * @brief     This function is used to set the BLE RF channel index (0~39).
 * @param[in] chn - BLE channel index, 0~39 (data chn 0~36, adv chn 37~39).
 * @return    none
 */
void hal_rf_set_channel_index(_u8 chn);

/**
 * @brief  RF transmit power levels, abstract values, no hardware encoding.
 *         Implementation maps them to chip trim values.
 */
enum
{
    HAL_RF_POWER_P9dBm = 0,
    HAL_RF_POWER_P8dBm,
    HAL_RF_POWER_P7dBm,
    HAL_RF_POWER_P6dBm,
    HAL_RF_POWER_P5dBm,
    HAL_RF_POWER_P4dBm,
    HAL_RF_POWER_P3dBm,
    HAL_RF_POWER_P2dBm,
    HAL_RF_POWER_P1dBm,
    HAL_RF_POWER_0dBm ,
    HAL_RF_POWER_N1dBm,
    HAL_RF_POWER_N2dBm,
    HAL_RF_POWER_N3dBm,
    HAL_RF_POWER_N4dBm,
    HAL_RF_POWER_N5dBm,
    HAL_RF_POWER_N6dBm,
    HAL_RF_POWER_N7dBm,
    HAL_RF_POWER_N8dBm,
};

/**
 * @brief     This function is used to set rf transmit power.
 * @param[in] power - power level, HAL_RF_POWER_xxx. Out-of-range index is
 *            undefined behavior, callers must use the enum.
 * @return    none
 */
void hal_rf_set_power(_u8 power);

/**
 * @brief     RF phy modes. Abstract values, no hardware encoding.
 */
enum
{
    HAL_RF_MODE_1M       = 0x00,
    HAL_RF_MODE_2M       = 0x01,
    HAL_RF_MODE_CODED_S2 = 0x02,
    HAL_RF_MODE_CODED_S8 = 0x03,
};

/**
 * @brief     These functions switch the active PHY mode. Must be called
 *            before hal_rf_tx/hal_rf_rx, affects all following packets.
 * @return    none
 */
void hal_rf_set_coded_phy_s2(void);

void hal_rf_set_coded_phy_s8(void);

void hal_rf_set_1M_phy(void);

void hal_rf_set_2M_phy(void);

/**
 * @brief     This function is used to set the rx timeout, counting from
 *            the moment rx starts waiting for sync word.
 * @param[in] time - timeout in us.
 * @return    none
 */
void hal_rf_set_rx_timeout(_u32 time);

/**
 * @brief     This function is used to set the maximum received packet
 *            payload length, hardware discards longer packets.
 * @param[in] len - max payload length in bytes (excluding CRC？).
 * @return    none
 */
void hal_rf_set_rx_max_len(_u8 len);

/*******************************get rf info *********************************/

/**
 * @brief     These functions return the hardware settle/prepare time before
 *            tx or rx can start in air, used by LL to schedule.
 * @param[in] phy - phy index: 0=1M, 1=2M, 2=CODED_S2, 3=CODED_S8.
 * @return    prepare time in us.
 */
_u32 hal_rf_get_rx_hw_prepare_time(_u8 phy);

_u32 hal_rf_get_tx_hw_prepare_time(_u8 phy);

/**
 * @brief     This function is used to convert the hardware rx timestamp
 *            register to the real air time when the access code started.
 * @param[in] phy - phy index: 0=1M, 1=2M, 2=CODED_S2, 3=CODED_S8.
 * @return    access code start time in us.
 */
_u32 hal_rf_get_rx_air_timestamp(_u8 phy);

/**
 * @brief     This function is used to check a received packet buffer: dma
 *            length field matches payload length and CRC check passed.
 * @param[in] packet - packet buffer, pointing to the dma length header.
 * @return    1 valid, 0 invalid (null, length mismatch, or CRC error).
 */
_u32 hal_rf_is_packet_valid(_u8* packet);

/**
 * @brief     These functions return the hardware packet buffer layout
 *            offsets so upper layer can locate the payload:
 *            TX: buffer = [4B dma header][payload...]
 *                extra_len = 4, header_offset = 4
 *            RX: buffer = [4B dma header][payload...][3B crc][4B ts][4B extra]
 *                extra_len = 15, header_offset = 4
 * @return    offset/extra length in bytes.
 */
_u32 hal_rf_get_hw_packet_tx_extra_len(void);

_u32 hal_rf_get_hw_tx_header_offset(void);

_u32 hal_rf_get_hw_packet_rx_extra_len(void);

_u32 hal_rf_get_hw_rx_header_offset(void);

/*******************************operate rf *********************************/

/**
 * @brief     This function is used to start a timed tx. Radio turns on
 *            before 'time' and transmits the packet in the dma buffer.
 * @param[in] address - tx dma buffer: [4B dma len header][payload...],
 *            dma len header is filled inside this function.
 * @param[in] time - absolute system time in us to start tx.
 * @return    none
 */
void hal_rf_tx(_u8* address,_u32 time);

/**
 * @brief     This function is used to start a timed rx. Radio turns on
 *            before 'time', DMA writes the packet into the buffer, and
 *            the buffer must be at least maxOctets + extra bytes
 *            (hal_rf_get_hw_packet_rx_extra_len).
 * @param[in] address - rx dma buffer, payload written from +4 offset.
 * @param[in] maxOctets - max bytes to receive into the buffer.
 * @param[in] time - absolute system time in us to start rx.
 * @return    none
 */
void hal_rf_rx(_u8* address,_u32 maxOctets,_u32 time);

/**
 * @brief     This function is used to abort current rf operation and
 *            reset the baseband and rf dma channels.
 * @return    none
 */
void hal_rf_stop(void);

/**
 * @brief     RF irq events reported through the hal_rf_init callback.
 */
enum
{
    HAL_RF_IRQ_TX,          /* tx done */
    HAL_RF_IRQ_RX,          /* packet received, buffer valid until callback returns */
    HAL_RF_IRQ_RX_TIMEOUT,  /* no packet within timeout */
};

/**
 * @brief     This function is used to init the rf module and register the
 *            irq callback. Sets BLE 1M mode, +4dBm, fast settle calibration,
 *            enables TX/RX/RX_TIMEOUT irqs.
 * @param[in] cb - callback invoked in rf irq context (ISR): cb(HAL_RF_IRQ_xxx).
 * @return    none
 */
void hal_rf_init(void(*cb)(_u8));

#endif /* HAL_RF_H_ */
