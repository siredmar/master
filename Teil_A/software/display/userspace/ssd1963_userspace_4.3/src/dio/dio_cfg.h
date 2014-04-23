/*
 * dio_cfg.h
 *
 *  Created on: Nov 10, 2012
 *      Author: armin
 */

#ifndef DIO_CFG_H_
#define DIO_CFG_H_

/* LPC3131 UM chapter 15 */
#define IOCONFIG    0x13003000
#define FBLO_GPIO   0x1C0  /* GPIO functional block */

/* offset in GPIO functional block  */
#define PINS        0x00
#define MODE0       0x10
#define MODE0_SET   0x14
#define MODE0_RESET 0x18
#define MODE1       0x20
#define MODE1_SET   0x24
#define MODE1_RESET 0x28


#define DIO_MAX_NUMBER_OF_PORT_GROUPS        3
#define DIO_MAX_NUMBER_OF_PORTS_PER_GROUP    16



#endif /* DIO_CFG_H_ */
