/**
 * devices are identified using a device address and an address type
 * address type - public address
 *                random address - static  address
 *                               - private address - resolvable device
 *                                                 - non-resolvable device.
 *                random address sub-type is indicated by the two most significant bits in random address
 *                0b00 -- non-resolvable private address
 *                0b01 -- resolvable private address
 *                0b10 -- reserved for future use
 *                0b11 -- static address
 */

/**
 * random address -- 0b11 -- static address
 * -------------------------------------------------------------
 * |                      random part                  | 1 | 1 |
 * -------------------------------------------------------------
 * <----------------------------48bit-------------------------->
 * Key Point
 * (1)at least one bit of the random part shall be 0
 * (2)at least one bit of the random part shall be 1
 * (3)device may initialize its static address after each power cycle,device shall not change its static address until next power cycle.
 * 
 */
/**
 * random address --0b00 --non-resolvable private address
 * -------------------------------------------------------------
 * |                      random part                  | 0 | 0 |
 * -------------------------------------------------------------
 * <----------------------------48bit-------------------------->
 *  * Key Point
 * (1)at least one bit of the random part shall be 0
 * (2)at least one bit of the random part shall be 1
 * (3)the address should not equal to public address.
 */
 
  
/**
 * random address --0b01 -- resolvable private address
 * -------------------------------------------------------------
 * |            hash             |random part of prand  | 0 | 1 |
 * -------------------------------------------------------------
 * <--------hash(24bit)--------->|<--------prand(24bit)--------->
 * Key Point
 * (1)at least one bit of the prand part shall be 0
 * (2)at least one bit of the prand part shall be 1
 * (3)device must have local IRK or peer IRK.
 * (4)prand is a 24bit random number.
 * (5)hash = ah(IRK,prand)
 * (6)if a device have more than one stored IRQ,the device should repet the procedure to determine if the device is associated device.
 */

