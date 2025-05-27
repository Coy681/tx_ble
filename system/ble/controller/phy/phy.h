/**************************packet format for the LE Uncoded PHYs**************************** */



/** packet format for the LE Uncoded PHYs
 * -----------------------------------------------------------------------------------------
 * |  preamble  |  access address    |      PDU     |    CRC    |  Constant Tone Extension |
 * | 1/2 octets |     4 octets       | 2-258 octets |  3 octets |          16-160us        |
 * -----------------------------------------------------------------------------------------
 * 
 * Key Points
 * (1)preamble is 1 octet when transmitting or receiving on the LE 1M PHY,2 octets on LE 2M PHY.
 * (2)preamble,access address,PDU,CRC is necessary in each packet,CTE is optional field.
 */

/** premble
 * Key Points
 * (1)The first bit of preamble shall be the same as the LSB of the Access address.
 */

/** access address
 * Key Points
 * (1)the access address for all advertising physical channel packets shall be 0x8E89BED6
 * (2)not repeted with existing logic train.
 * (3)more than one bit differ from existing advertising physical channel.
 * (3)not have all for octets equal.
 * (4)have no more than 6 bits zeros or ones.  
 * (5)have no more than 24 transitions.
 * (6)have a minimum of two transitions in the most significant six bits.
 * for the access address of link layer in isochronous broadcasting state,it shall generate a new Seed Access Address for each BIG
 * 
 * the SAA shall meet the requirements
 * 1> SAA19  = SAA15
 * 2> SAA22  = SAA16 != SAA15
 * 3> SAA25  = 0
 * 4> SAA23  = 1;
 * 5> for the BIGs in a device,the SAA15-SAA0 shall differ in as least two bits.
 * 
 * for each BIS,the Access Address shall be equal to the SAA bit-wise XOR with a diversifer word(DW)
 * for which  
 *           D  = ((35*n)+42)mod 128      n is BIS number,n will be 0 for BIG Control logic link
 *           DW = 0bD0 D0 D0 D0_D0 D0 D1 D6_D1 0 D5 D4_0 D3 D2 0_00000000_00000000 
 *           for example, if n = 1,D=77=0b01001101,DW = 0b1111_1101_0000_0110_0000_0000_0000_0000 = 0xFD060000
 * 
 * on an implementation that also support the LE Coded PHY,the access address shall also meet the following requirements.
 * (1)have at least three ones in the least significant 16 bits.
 * (2)have no more than eleven transitions in the least significant 16 bits.
 * 
 */

 /**************************packet format for the LE coded PHYs**************************** */

/** packet format for the LE Uncoded PHYs
 * 
 *            |<-------S=8 coding---------- |<--------S=2 or S=8 coding---------|          
 * |   80bit  |     32bit      |2bit|  3bit |   N*8 bit   |  24 bit   |  3 bit  |
 * |   80us   |     256us      |16us|  24us |  N*8*S us   |  24*S us  |  3*S us |
 * ------------------------------------------------------------------------------
 * | preamble | access address | CI | TERM1 | PDU,N Bytes |    CRC    |  TERM2  |
 * ------------------------------------------------------------------------------
 *            |<-------FEC Block 1--------->|<------------FEC Block 2---------->|
 * 
 * Key Points
 * (1)preamble is not coded,
 * (2)CI determines which coding scheme is used for FEC block2
 * (3)the entire packet is tranmist with 1Msym/s.
 * (4)PDU at most 257 octets
 */

/** preamble of coded PHY
 * preamble is 80 symbols in length,and 10 repetitions of "00111100"
 */

/** preamble of coded PHY
 * specified in uncoded PHY
 */

/** Coding Indicator(CI) of coded PHY
 *  >0b00 - FEC Block 2 using S=8
 *  >0b01 - FEC Block 2 using S=2
 */

/**TERM1 and TERM2
 * FEC used
 */

#include"common/txCommon.h"
#include"platform/platform.h"

 #ifndef LL_PHY_H_
 #define LL_PHY_H_

#define PHY_DEFAULT_TIFS_TIME     150


typedef enum
{
    PHY_MODE_1M       = HAL_RF_MODE_1M,
    PHY_MODE_2M       = HAL_RF_MODE_2M,
    PHY_MODE_CODED_S2 = HAL_RF_MODE_CODED_S2,
    PHY_MODE_CODED_S8 = HAL_RF_MODE_CODED_S8,
}phy_mode_e;

typedef enum
{
    PHY_DIR_TX        = 0x01,
    PHY_DIR_RX        = 0x02,
}phy_dir_e;

typedef enum
{
    PHY_IRQ_TX         = HAL_RF_IRQ_TX,
    PHY_IRQ_RX         = HAL_RF_IRQ_RX,
    PHY_IRQ_RX_TIMEOUT = HAL_RF_IRQ_RX_TIMEOUT,
}phy_irq_e;

typedef struct _PACKED
{
    _u8  phy:3;//phy_mode_e
    _u8  powerIndex:5;//
    _u8  chnIdx:6;
    _u8  dir:2;//phy_dir_e
    _u8  rxMaxOctets;
    _u8  rsvd1;

    _u8* txAddress;
    _u8* rxAddress;
    _u32 crcInit;
    _u32 accessCode;
    _u32 timestamp;//first bit of header
    _u32 rxTimeout;
    void (*start)(void);
    void (*stop)(void);
    _u32 (*hw_get_rx_air_ts)(void);//timestamp means first bit time of preamble                     
    _u32 (*hw_get_trigger_from_ts)(_u32);//get tx or rx trigger time form timsetamp
    _u32 (*hw_if_rx_packet_valid)(void);
    void (*hw_irq_cb)(_u8);//phy_irq_e
}phy_obj_t;

typedef void(*phy_irq_cb_f)(_u8 type);

void phy_obj_init(phy_obj_t* phy);

void phy_obj_cast(phy_obj_t* phy);

#endif//LL_PHY_H_