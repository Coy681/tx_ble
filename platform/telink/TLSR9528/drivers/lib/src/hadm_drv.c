/********************************************************************************************************
 * @file    hadm_drv.c
 *
 * @brief   This is the source file for B92
 *
 * @author  Driver Group
 * @date    2023
 *
 * @par     Copyright (c) 2023, Telink Semiconductor (Shanghai) Co., Ltd.
 *          All rights reserved.
 *
 *          The information contained herein is confidential property of Telink
 *          Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *          of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *          Co., Ltd. and the licensee or the terms described here-in. This heading
 *          MUST NOT be removed from this file.
 *
 *          Licensee shall not delete, modify or alter (or permit any third party to delete, modify, or
 *          alter) any information contained herein in whole or in part except as expressly authorized
 *          by Telink semiconductor (shanghai) Co., Ltd. Otherwise, licensee shall be solely responsible
 *          for any claim to the extent arising out of or relating to such deletion(s), modification(s)
 *          or alteration(s).
 *
 *          Licensees are granted free, non-transferable use of the information in this
 *          file under Mutual Non-Disclosure Agreement. NO WARRANTY of ANY KIND is provided.
 *
 *******************************************************************************************************/
#include "lib/include/rf.h"
#include "lib/include/hadm_drv.h"
#include "compiler.h"
#include "dma.h"

static unsigned int g_iq_group_num;

/****************************************************************************************************************************************
 *                                         RF : HADM related functions                          		 			  					*
 ****************************************************************************************************************************************/

/**
 * @brief		This function is mainly used to initialize some parameter settings of the HADM IQ sample.
 * @param[in]	samp_num	- Number of groups to sample IQ data.
 * @param[in]	interval	- The interval time between each IQ sampling is (interval + 1)*0.125us.
 * @param[in]	start_point	- Set the starting point of the sample.If it is rx_en mode, sampling starts
 * 							  at 0.25us+start_point*0.125us after settle. If it is in sync mode, sampling
 * 							  starts at (start_point + 1) * 0.125us after sync.
 * @param[in]	suppmode    - The length of each I or Q data.
 * @param[in]	sample_mode - IQ sampling starts after syncing packets or after the rx_en is pulled up.
 * @return		none.
 */
void rf_hadm_iq_sample_init(unsigned short samp_num,unsigned char interval,unsigned char start_point,rf_iq_data_mode_e suppmode,rf_hadm_iq_sample_mode_e sample_mode)
{
	rf_hadm_iq_sample_number(samp_num);
	rf_hadm_sample_interval_time(interval);
	rf_hadm_iq_start_point(start_point);
	rf_aoa_aod_iq_data_mode(suppmode);
	rf_hadm_iq_sample_mode(sample_mode);
	rf_iq_sample_enable();

}

/**
 * @brief		This function is mainly used to set the sample interval.
 * @param[in]	ant_interval- Set the interval for IQ sample, (interval + 1)*0.125us.
 * @return		none.
 * @note		The max sample rate is 4Mhz.
 */
void rf_hadm_sample_interval_time(unsigned char interval)
{
	reg_rf_mode_ctrl0 = ((reg_rf_mode_ctrl0 & (~FLD_RF_IQ_SAMP_INTERVAL)) | (interval<<4));
}

/**
 * @brief		This function is mainly used to initialize the parameters related to HADM antennas.
 * @param[in]	clk_mode	- Set whether the antenna-related clock is always on or only when switching antennas.
 * @param[in]	ant_interval- Set the interval for antenna switching, (interval + 1)*0.125us.
 * @param[in]	ant_rxoffset- Adjust the switching start point of the rx-side antenna,(ant_rxoffset + 1)*0.125us.
 * @param[in]	ant_txoffset- Adjust the switching start point of the tx-side antenna,(ant_rxoffset + 1)*0.125us.
 * @return		none.
 */
void rf_hadm_ant_init(rf_hadm_ant_clk_mode_e clk_mode,unsigned char ant_interval,unsigned char ant_rxoffset,unsigned char ant_txoffset)
{
	rf_hadm_ant_clk_mode(clk_mode);
	rf_set_hadm_ant_interval(ant_interval);
	rf_set_hadm_rx_ant_offset(ant_rxoffset);
	rf_set_hadm_tx_ant_offset(ant_txoffset);
}

/**
 * @brief		This function is mainly used to set the antenna switching interval.
 * @param[in]	ant_interval- Set the interval for antenna switching, (interval + 1)*0.125us.
 * @return		none.
 */
void rf_set_hadm_ant_interval(unsigned char ant_interval)
{
	write_reg8(0x170035,ant_interval);
	write_reg8(0x170036,(read_reg8(0x170036)&(~BIT(0)))|(ant_interval>>8));
}

/**
 * @brief		This function is mainly used to set the starting position of the antenna switching at the rx-side.
 * @param[in]	ant_rxoffset- Adjust the switching start point of the rx-side antenna,(ant_rxoffset + 1)*0.125us.
 * @return		none.
 */
void rf_set_hadm_rx_ant_offset(unsigned char ant_rxoffset)
{
	write_reg8(0x17003a,ant_rxoffset);
	write_reg8(0x170036,(read_reg8(0x170036)&(~BIT(2)))|((ant_rxoffset>>8)<<2));
	write_reg8(0x170007,read_reg8(0x170007)|BIT(2));//rx_ant_switch
}

/**
 * @brief		This function is mainly used to set the starting position of the antenna switching at the tx-side.
 * @param[in]	ant_txoffset- Adjust the switching start point of the rx-side antenna,(ant_txoffset + 1)*0.125us.
 * @return		none.
 */
void rf_set_hadm_tx_ant_offset(unsigned char ant_txoffset)
{
	write_reg8(0x170039,ant_txoffset);
	write_reg8(0x170036,(read_reg8(0x170036)&(~BIT(1)))|((ant_txoffset>>8)<<1));
	write_reg8(0x170007,(read_reg8(0x170007)&0xfc)|0x02);//tx_ant_switch
}

/**
 * @brief		This function is mainly used to set the clock working mode of the antenna.
 * @para[in]	clk_mode	- Open all the time or only when switching antennas.
 * @return		none.
 */
void rf_hadm_ant_clk_mode(rf_hadm_ant_clk_mode_e clk_mode)
{
//		write_reg8(0x17002b,read_reg8(0x17002b)&(~BIT(0)));
	reg_rf_rxclk_auto = ((reg_rf_rxclk_auto&0xfe) | clk_mode);
}

/**
 * @brief		This function is mainly used to set the way IQ sampling starts.
 * @para[in]	sample_mode	- IQ sampling starts after syncing packets or after the rx_en is pulled up.
 * @return		none.
 */
void rf_hadm_iq_sample_mode(rf_hadm_iq_sample_mode_e sample_mode)
{
	if(sample_mode == RF_HADM_IQ_SAMPLE_SYNC_MODE)
	{
		reg_rf_rxlatf |= FLD_RF_R_IQ_SAMP_MODE;
	}
	else
	{
		reg_rf_rxlatf &= (~FLD_RF_R_IQ_SAMP_MODE);
	}
}

/**
 * @brief		This function is mainly used to set the starting position of IQ sampling.
 * @para[in]	start_point  - Set the starting point of the sample.If it is rx_en mode, sampling starts
 * 							  at 0.25us+start_point*0.125us after settle. If it is in sync mode, sampling
 * 							  starts at (start_point + 1) * 0.125us after sync.
 * @return		none.
 */
void rf_hadm_iq_start_point(unsigned char start_point)
{
	reg_rf_iq_samp_start = start_point;
}

/**
 * @brief		This function is mainly used to set the number of IQ samples in groups.
 * @para[in]	samp_num    - Number of groups to sample IQ data.
 * @return		none.
 */
void rf_hadm_iq_sample_number(unsigned short samp_num)
{
	reg_rf_iq_samp_num = samp_num;
	g_iq_group_num = samp_num;
}

/**
 * @brief		Mainly used to set thresholds when sync data packets.
 * @para[in]	thres_value  - The value of thresholds.
 * @return		none.
 */
void rf_set_ble_sync_threshold(unsigned char thres_value)
{
	reg_rf_modem_sync_thres_ble = thres_value;
}

/**
 * @brief		This function is mainly used to enable the IQ sampling function.
 * @return		none.
 */
void rf_iq_sample_enable()
{
	reg_rf_mode_ctrl0 |= FLD_RF_IQ_SAMP_EN;
}

/**
 * @brief		This function is mainly used to disable the IQ sampling function.
 * @return		none.
 */
void rf_iq_sample_disable()
{
	reg_rf_mode_ctrl0 &= (~FLD_RF_IQ_SAMP_EN);
}

/**
 * @brief		This function is mainly used to obtain the sync flag bit from the packet, which is
 * 				used to identify whether the packet is data received after passing synchronisation.
 * @param[in]	p			- The packet address.
 * @param[in]	sample_num	- The number of sample points that the packet contains.
 * @param[in]	data_len	- The data length of the sample point in the packet.
 * @return		Returns the Sync flag information in the packet.
 */
unsigned char rf_hadm_sync_flag(unsigned char *p,unsigned short sample_num,rf_iq_data_mode_e data_len)
{
	unsigned char x = 0;
	x = ((data_len >> 8) & 0xff);
	return ((p[x*sample_num+9]&BIT(3))>>3);
}

/**
 * @brief		This function is mainly used to obtain the packet quality indicator from the packet, which is
 * 				used to identify whether the packet is data received after passing synchronisation.
 * @param[in]	p			- The packet address.
 * @param[in]	sample_num	- The number of sample points that the packet contains.
 * @param[in]	data_len	- The data length of the sample point in the packet.
 * @return		Returns the packet quality information in the packet.
 */
unsigned char rf_hadm_get_packet_quality_indicator(unsigned char *p,unsigned short sample_num,rf_iq_data_mode_e data_len)
{
	unsigned char x = 0; unsigned char quality_indicator = 0;
	x = ((data_len >> 8) & 0xff);
	quality_indicator = 32 - (p[x*sample_num+16]);
	return  (quality_indicator > 2 ? 2 : quality_indicator);
}

/**
 * @brief		This function is mainly used to get the timestamp information from the packet that is
 * 				synchronised to the packet.
 * @param[in]	p			- The packet address.
 * @param[in]	sample_num	- The number of sample points that the packet contains.
 * @param[in]	data_len	- The data length of the sample point in the packet.
 * @return		Returns the Sync timestamp information in the packet.
 */
unsigned int rf_hadm_get_pkt_rx_sync_timestamp(unsigned char *p,unsigned short sample_num,rf_iq_data_mode_e data_len)
{
	unsigned char x = 0;
	x = ((data_len >> 8) & 0xff);
	return (p[x*sample_num+7]<<24 | p[x*sample_num+6]<<16 | p[x*sample_num+5]<<8 | p[x*sample_num+4]);
}

/**
 * @brief		This function is mainly used to obtain the timestamp information of the tx_pos from the packet.
 * @param[in]	p			- The packet address.
 * @param[in]	sample_num	- The number of sample points that the packet contains.
 * @param[in]	data_len	- The data length of the sample point in the packet.
 * @return		Returns the timestamp information in the packet.
 */
unsigned int rf_hadm_get_pkt_tx_pos_timestamp(unsigned char *p,unsigned short sample_num,rf_iq_data_mode_e data_len)
{
	unsigned char x = 0;
	x = ((data_len >> 8) & 0xff);
	return (p[x*sample_num+31]<<24 | p[x*sample_num+30]<<16 | p[x*sample_num+29]<<8 | p[x*sample_num+28]);
}

/**
 * @brief		This function is mainly used to obtain the timestamp information of the tx_neg from the packet.
 * @param[in]	p			- The packet address.
 * @param[in]	sample_num	- The number of sample points that the packet contains.
 * @param[in]	data_len	- The data length of the sample point in the packet.
 * @return		Returns the timestamp information in the packet.
 */
unsigned int rf_hadm_get_pkt_tx_neg_timestamp(unsigned char *p,unsigned short sample_num,rf_iq_data_mode_e data_len)
{
	unsigned char x = 0;
	x = ((data_len >> 8) & 0xff);
	return (p[x*sample_num+35]<<24 | p[x*sample_num+34]<<16 | p[x*sample_num+33]<<8 | p[x*sample_num+32]);
}

/**
 * @brief		This function is mainly used to obtain the timestamp information of the iq_start from the packet.
 * @param[in]	p			- The packet address.
 * @param[in]	sample_num	- The number of sample points that the packet contains.
 * @param[in]	data_len	- The data length of the sample point in the packet.
 * @return		Returns the timestamp information in the packet.
 */
unsigned int rf_hadm_get_pkt_iq_start_timestamp(unsigned char *p,unsigned short sample_num,rf_iq_data_mode_e data_len)
{
	unsigned char x = 0;
	x = ((data_len >> 8) & 0xff);
	return (p[x*sample_num+15]<<24 | p[x*sample_num+14]<<16 | p[x*sample_num+13]<<8 | p[x*sample_num+12]);
}

/**
 * @brief		This function is mainly used to obtain the rssi information from the packet.
 * @param[in]	p			- The packet address.
 * @param[in]	sample_num	- The number of sample points that the packet contains.
 * @param[in]	data_len	- The data length of the sample point in the packet.
 * @return		Returns the rssi information in the packet.
 */
signed char rf_hadm_get_pkt_rssi_value(unsigned char *p,unsigned short sample_num,rf_iq_data_mode_e data_len)
{
	unsigned char x = 0;
	x = ((data_len >> 8) & 0xff);
	return (p[x*sample_num+10]-110);
}

/**
 * @brief   	This function serves to set RF's channel.The step of this function is in KHz.
 *				The frequency set by this function is (chn+2400) MHz+chn_k KHz.
 * @param[in]   chn_m - RF channel. The unit of this parameter is MHz, and its set frequency
 * 					 	point is (2400+chn)MHz.
 * @param[in]   chn_k - The unit of this parameter is KHz, which means to shift chn_k KHz to
 * 						the right on the basis of chn.Its value ranges from 0 to 999.
 * @param[in]	trx_mode - Defines the frequency point setting of tx mode or rx mode.
 * @return  	none.
 */
void rf_set_channel_k_step(signed char chn_m,unsigned int chn_k,rf_trx_chn_e trx_mode)//general
{
    unsigned int rf_chn_k =0;
    unsigned int ctrim_k;
    unsigned int temp_k;
    long chnl_freq_k;

    rf_set_chn(chn_m-trx_mode);

    rf_chn_k = (((chn_m+2400-trx_mode)*1000)+chn_k)*100;
    ctrim_k = rf_chn_k/48;
    temp_k = ((rf_chn_k/100000+24)/48);
    temp_k *= 100000;
    if(ctrim_k >= temp_k)
    {
    	chnl_freq_k = ctrim_k - temp_k;
    	chnl_freq_k = chnl_freq_k*2621/1000;
    }
    else
    {
    	chnl_freq_k = temp_k - ctrim_k;
    	chnl_freq_k = chnl_freq_k*2621/1000;
    	chnl_freq_k = 0x40000 - chnl_freq_k;
    }
    write_reg8(0x170649,(chnl_freq_k & 0x3fc00)>>10);  //DSM_FRAC higher 8 bits
    write_reg8(0x170648, (chnl_freq_k & 0x3f8)>>2);   //DSM_FRAC next 7 bits
    write_reg8(0x170641, ((read_reg8(0x170641)&0xfc) | (chnl_freq_k & 0x06)>>1 ));  //DSM_FRAC next 2 bits
    write_reg8(0x170640, (read_reg8(0x170640)&0x7f) | ((chnl_freq_k & 0x01)<<7));  //DSM_FRAC last bit

    write_reg8(0x170648, (read_reg8(0x170648) | 0x01));  //enable DSM_FRAC_OW manual mode
}

/**
 * @brief		This function is mainly used for frezee agc.
 * @return		none.
 * @note		It should be noted that this function should be called after receiving the package.
 */
void rf_agc_disable()
{
	char gain_lat, lna_hgain, lna_lgain, lna_attn, cbpf_gain;
	reg_rf_radio_txrx_dbg1_0 |= FLD_RF_AGC_DISABLE;
	gain_lat = (read_reg8(0x170059)>>4)&0x07;
	write_reg8(0x170640,(read_reg8(0x170640)&0xe3)|((gain_lat&0x07)<<2));

	if(gain_lat == 0)
	{
		lna_hgain = 0;
		lna_lgain = 1;
		lna_attn  = 3;
		cbpf_gain = 0;
	}
	else if(gain_lat == 1)
	{
		lna_hgain = 0;
		lna_lgain = 3;
		lna_attn  = 2;
		cbpf_gain = 1;
	}
	else if(gain_lat == 2)
	{
		lna_hgain = 0;
		lna_lgain = 3;
		lna_attn  = 0;
		cbpf_gain = 1;
	}
	else if(gain_lat == 3)
	{
		lna_hgain = 3;
		lna_lgain = 0;
		lna_attn  = 0;
		cbpf_gain = 1;
	}
	else if(gain_lat == 4)
	{
		lna_hgain = 0xf;
		lna_lgain = 0;
		lna_attn  = 0;
		cbpf_gain = 1;
	}
	else if(gain_lat == 5)
	{
		lna_hgain = 0x3f;
		lna_lgain = 0;
		lna_attn  = 0;
		cbpf_gain = 1;
	}
	else if(gain_lat == 6)
	{
		lna_hgain = 0;
		lna_lgain = 0;
		lna_attn  = 0;
		cbpf_gain = 1;
	}
	else
	{
		lna_hgain = 0;
		lna_lgain = 0;
		lna_attn  = 0;
		cbpf_gain = 0;
	}

	write_reg8(0x17077a,(read_reg8(0x17077a)&0x81)|(lna_hgain<<1));
	write_reg8(0x170778,read_reg8(0x170778)|0x02);

	write_reg8(0x17077b,(read_reg8(0x17077b)&0xfe)|(lna_lgain>>1));
	write_reg8(0x17077a,(read_reg8(0x17077a)&0x7f)|(lna_lgain<<7));
	write_reg8(0x170778,read_reg8(0x170778)|0x04);

	write_reg8(0x17077b,(read_reg8(0x17077b)&0xf9)|((lna_attn&0x03)<<1));
	write_reg8(0x170778,read_reg8(0x170778)|0x08);

	write_reg8(0x170782,(read_reg8(0x170782)&0xfd)|(cbpf_gain&0x01)<<1);
	write_reg8(0x170780,read_reg8(0x170780)|0x02);
}

/**
 * @brief		This function is mainly used for agc auto run.
 * @return		none.
 * @note		It needs to be called before sending and receiving packets after the tone interaction is complete.
 */
void rf_agc_enable(void)
{
	reg_rf_radio_txrx_dbg1_0 &= (~FLD_RF_AGC_DISABLE);
	write_reg8(0x170778,read_reg8(0x170778)&(~BIT(1)));
	write_reg8(0x170778,read_reg8(0x170778)&(~BIT(2)));
	write_reg8(0x170778,read_reg8(0x170778)&(~BIT(3)));
	write_reg8(0x170780,read_reg8(0x170780)&(~BIT(1)));
}

/**
 * @brief		This function is mainly used to set the sequence related to Fast Settle in HADM.
 * @return		none.
 */
void rf_fast_settle_sequence_set(void)
{
	//seq_ldo_pll_pup_ow
	write_reg8(0x170762,read_reg8(0x170762)|BIT(3));
	write_reg8(0x170760,read_reg8(0x170760)|BIT(3));

	//seq_ldo_vco_pup_ow
	write_reg8(0x170762,read_reg8(0x170762)|BIT(4));
	write_reg8(0x170760,read_reg8(0x170760)|BIT(4));

	//seq_ldo_pll_fc_ow
	write_reg8(0x170763,read_reg8(0x170763)&(~BIT(3)));
	write_reg8(0x170761,read_reg8(0x170761)|BIT(3));

	//rf_seq_ldo_vco_fc_ow
	write_reg8(0x170763,read_reg8(0x170763)&(~BIT(4)));
	write_reg8(0x170761,read_reg8(0x170761)|BIT(4));

	//seq_pd_pup_ow
	write_reg8(0x17078a,read_reg8(0x17078a)|BIT(0));
	write_reg8(0x170788,read_reg8(0x170788)|BIT(0));

	//seq_pd_en_fcal_bias_ow
	write_reg8(0x17078a,read_reg8(0x17078a)&(~BIT(2)));
	write_reg8(0x170788,read_reg8(0x170788)|BIT(2));

	//seq_xo_en_clk_ref_ow
	write_reg8(0x170770,read_reg8(0x170770)|BIT(3));
	write_reg8(0x170770,read_reg8(0x170770)|BIT(1));

	//seq_vco_pup_ow
	write_reg8(0x17078e,read_reg8(0x17078e)|BIT(0));
	write_reg8(0x17078c,read_reg8(0x17078c)|BIT(0));

	//seq_lo_pup_vlo_fbk_ow
	write_reg8(0x17078e,read_reg8(0x17078e)|BIT(6));
	write_reg8(0x17078c,read_reg8(0x17078c)|BIT(6));

	//seq_fcal_pup_ow
	write_reg8(0x17078a,read_reg8(0x17078a)&(~BIT(3)));
	write_reg8(0x170788,read_reg8(0x170788)|BIT(3));

	//_seq_fcal_set_ow
	write_reg8(0x17078a,read_reg8(0x17078a)&(~BIT(4)));
	write_reg8(0x170788,read_reg8(0x170788)|BIT(4));

	//seq_fcal_run_ow
	write_reg8(0x17078a,read_reg8(0x17078a)&(~BIT(5)));
	write_reg8(0x170788,read_reg8(0x170788)|BIT(5));

	//seq_divn_pup_ow
	write_reg8(0x17078a,read_reg8(0x17078a)|BIT(6));
	write_reg8(0x170788,read_reg8(0x170788)|BIT(6));

	//seq_divn_openloop_ow
	write_reg8(0x17078a,read_reg8(0x17078a)&(~BIT(7)));
	write_reg8(0x170788,read_reg8(0x170788)|BIT(7));

	//ldo_rxtxhf_pup_ow
	write_reg8(0x170762,read_reg8(0x170762)|BIT(1));
	write_reg8(0x170760,read_reg8(0x170760)|BIT(1));

	//ldo_lv_pup_ow
	write_reg8(0x170762,read_reg8(0x170762)|BIT(0));
	write_reg8(0x170760,read_reg8(0x170760)|BIT(0));

	//bg_pup_ow
	write_reg8(0x170766,read_reg8(0x170766)|BIT(0));
	write_reg8(0x170764,read_reg8(0x170764)|BIT(0));

	//rf_mixer_pup_ow
	write_reg8(0x17077b,read_reg8(0x17077b)|BIT(3));
	write_reg8(0x170778,read_reg8(0x170778)|BIT(4));

	//dsm_run
	write_reg8(0x170682,read_reg8(0x170682)|BIT(0));
	write_reg8(0x170680,read_reg8(0x170680)|BIT(0));

	//rf_rx_dig_mixer_en_ow
	write_reg8(0x170688,read_reg8(0x170688)|BIT(1));
	write_reg8(0x170686,read_reg8(0x170686)|BIT(1));

	//rf_hpm_cal_disable
	write_reg8(0x170688,read_reg8(0x170688)&(~BIT(3)));
	write_reg8(0x170686,read_reg8(0x170686)|BIT(3));

	//rf_seq_lo_pup_vlo_txfsk_ow
	write_reg8(0x170792,read_reg8(0x170792)|BIT(6));
	write_reg8(0x170790,read_reg8(0x170790)|BIT(6));
	write_reg8(0x170792,read_reg8(0x170792)|BIT(7));
	write_reg8(0x170790,read_reg8(0x170790)|BIT(7));

	//seq_lo_pup_vlo_rx_ow
	write_reg8(0x17078e,read_reg8(0x17078e)|BIT(2));
	write_reg8(0x17078c,read_reg8(0x17078c)|BIT(2));
	write_reg8(0x17078e,read_reg8(0x17078e)|BIT(3));
	write_reg8(0x17078c,read_reg8(0x17078c)|BIT(3));
}

/**
 * @brief		This function is mainly used to set the value of the dac_pup.
 * @param[in]	value	- The value of dac_pup.
 * @return		none.
 */
void rf_seq_dac_pup_ow(unsigned char value)
{
	write_reg8(0x17078e,(read_reg8(0x17078e)&0x7f) | (value&0x01)<<7);//tx:value = 1
	write_reg8(0x17078c,read_reg8(0x17078c)|BIT(7));
}

/**
 * @brief		This function is mainly used to set the value of the pa_pup.
 * @param[in]	value	- The value of pa_pup.
 * @return		none.
 */
void rf_seq_tx_pa_pup_ow(unsigned char value)
{
	write_reg8(0x17077c,(read_reg8(0x17077c)&0xfe)|(value&0x01));//tx value = 1
	write_reg8(0x170778,read_reg8(0x170778)|BIT(5));
}

/**
 * @brief		This function is mainly used to open the PA module.
 * @param[in]	pwr		- The slice value of power.
 * @return		none.
 */
void rf_pa_pwr_on(unsigned char pwr)
{
	write_reg8(0x17077c,(read_reg8(0x17077c)&0x81)|(pwr<<1));
	write_reg8(0x170778,read_reg8(0x170778)|BIT(6));
}

/**
 * @brief		This function is mainly used to set the preparation and enable of manual fcal.
 * @return		none.
 */
void rf_manual_fcal_setup(void)
{
//	rf_seq_pd_en_pd_drv_ow(0);
	write_reg8(0x17078a,read_reg8(0x17078a)|BIT(1));
	write_reg8(0x170788,read_reg8(0x170788)&(~BIT(1)));

	write_reg8(0x170738,read_reg8(0x170738)|BIT(2));

//	rf_seq_pd_en_fcal_bias_ow1();
	write_reg8(0x17078a,read_reg8(0x17078a)|BIT(2));
	write_reg8(0x170788,read_reg8(0x170788)|BIT(2));

//	rf_seq_fcal_pup_ow1();
	write_reg8(0x17078a,read_reg8(0x17078a)|BIT(3));
	write_reg8(0x170788,read_reg8(0x170788)|BIT(3));

//	rf_seq_fcal_set_disow();
	write_reg8(0x17078a,read_reg8(0x17078a)&(~BIT(4)));
	write_reg8(0x170788,read_reg8(0x170788)&(~BIT(4)));

//	rf_seq_fcal_run_disow();
	write_reg8(0x17078a,read_reg8(0x17078a)&(~BIT(5)));
	write_reg8(0x170788,read_reg8(0x170788)&(~BIT(5)));

	write_reg8(0x170683,read_reg8(0x170683)|BIT(3));
}

/**
 * @brief		This function is mainly used to set the relevant value after manual fcal.
 * @return		none.
 * @note		The function needs to be called after the rf_manual_fcal_setup call 22us.
 */
void rf_manual_fcal_done(void)
{
	write_reg8(0x170683,read_reg8(0x170683)&(~BIT(3)));
	write_reg8(0x170738,read_reg8(0x170738)&(~BIT(2)));

//	rf_seq_pd_en_fcal_bias_ow();
	write_reg8(0x17078a,read_reg8(0x17078a)&(~BIT(2)));
//	write_reg8(0x170788,read_reg8(0x170788)|BIT(2));

//	rf_seq_fcal_pup_ow();
	write_reg8(0x17078a,read_reg8(0x17078a)&(~BIT(3)));
//	write_reg8(0x170788,read_reg8(0x170788)|BIT(3));

//	rf_seq_fcal_set_ow();
//	write_reg8(0x17078a,read_reg8(0x17078a)&(~BIT(4)));
	write_reg8(0x170788,read_reg8(0x170788)|BIT(4));

//	rf_seq_fcal_run_ow();
//	write_reg8(0x17078a,read_reg8(0x17078a)&(~BIT(5)));
	write_reg8(0x170788,read_reg8(0x170788)|BIT(5));

//	rf_seq_pd_en_pd_drv_ow(1);
	write_reg8(0x170788,read_reg8(0x170788)|BIT(1));
}
