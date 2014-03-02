/*
 * dm9000 Ethernet
 *
 * Cribbed from Linux kernel.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 * Please refer to the file debian/copyright for further details.
 *
 */

#ifndef _DM9000X_H_
#define _DM9000X_H_

#define DM9000_ID		0x90000A46

/* although the registers are 16 bit, they are 32-bit aligned.
 */

#define DM9000_NCR	0x00        /* Network control */
#define DM9000_NSR	0x01        /* Network status */
#define DM9000_TCR	0x02        /* TX control */
#define DM9000_TSR1	0x03        /* TX status I */
#define DM9000_TSR2	0x04        /* TX status II */
#define DM9000_RCR	0x05        /* RX control */
#define DM9000_RSR	0x06        /* RX status */
#define DM9000_ROCR	0x07        /* Receive overflow counter */
#define DM9000_BPTR	0x08        /* Back pressure threshold */
#define DM9000_FCTR	0x09        /* Flow control threshold */
#define DM9000_FCR	0x0A        /* RX flow control */
#define DM9000_EPCR	0x0B        /* EEPROM/PHY control */
#define DM9000_EPAR	0x0C        /* EEPROM/PHY address */
#define DM9000_EPDRL	0x0D        /* EEPROM/PHY low byte */
#define DM9000_EPDRH	0x0E        /* EEPROM/PHY high byte */
#define DM9000_WCR	0x0F        /* Wake up control */

#define DM9000_PAR	0x10        /* Physical address */
#define DM9000_MAR	0x16        /* Multicast address */

#define DM9000_GPCR	0x1e        /* General purpose control */
#define DM9000_GPR	0x1f        /* General purpose */
#define DM9000_TRPAL	0x22        /* TX SRAM read addr low */
#define DM9000_TRPAH	0x23        /* TX SRAM read addr high */
#define DM9000_RWPAL	0x24        /* RX SRAM write addr low */
#define DM9000_RWPAH	0x25        /* RX SRAM write adr high */

#define DM9000_VIDL	0x28        /* Vendor ID low */
#define DM9000_VIDH	0x29        /* Vendor ID high */
#define DM9000_PIDL	0x2A        /* Product ID low */
#define DM9000_PIDH	0x2B        /* Product ID high */

#define DM9000_CHIPR	0x2C        /* Chip revision */
#define DM9000_TXCR2	0x2D        /* TX control II */
#define DM9000_OCR	0x2E        /* Operation control */
#define DM9000_SMCR	0x2F        /* Special mode control */
#define DM9000_ETXCSR	0x30        /* Early transmit control/stat */
#define DM9000_TCSCR	0x31        /* Transmit checksum control */
#define DM9000_RCSCSR	0x32        /* Receive checksum ctrl/stat */
#define DM9000_MPAR	0x33        /* MII PHY address */
#define DM9000_LEDCR	0x34        /* LED pin control */
#define DM9000_BUSCR	0x38        /* Processor bus control */
#define DM9000_INTCR	0x39        /* INT pin control */
#define DM9000_SCCR	0x50        /* System clock turn on control */
#define DM9000_RSCCR	0x51        /* Resume system clock control */

#define DM9000_MRCMDX	0xF0        /* Memory data pre-fetch (!inc) */
#define DM9000_MRCMD	0xF2        /* Memory read w/inc */
#define DM9000_MRRL	0xF4        /* Memory read address low  */
#define DM9000_MRRH	0xF5        /* Memory read address high */
#define DM9000_MWCMDX	0xF6        /* Memory write w/o inc */
#define DM9000_MWCMD	0xF8        /* Memory write w/inc */
#define DM9000_MWRL	0xFA        /* Memory write address low */
#define DM9000_MWRH	0xFB        /* Memory write address high */
#define DM9000_TXPLL	0xFC        /* TX packet length low */
#define DM9000_TXPLH	0xFD        /* TX packet length high */
#define DM9000_ISR	0xFE        /* Interrupt status */
#define DM9000_IMR	0xFF        /* Interrupt mask */

#define NCR_EXT_PHY	(1<<7)
#define NCR_WAKEEN	(1<<6)
#define NCR_FCOL	(1<<4)
#define NCR_FDX		(1<<3)
#define NCR_LBK		(3<<1)
#define NCR_RST		(1<<0)

#define NSR_SPEED	(1<<7)
#define NSR_LINKST	(1<<6)
#define NSR_WAKEST	(1<<5)
#define NSR_TX2END	(1<<3)
#define NSR_TX1END	(1<<2)
#define NSR_RXOV	(1<<1)

#define TCR_TJDIS	(1<<6)
#define TCR_EXCECM	(1<<5)
#define TCR_PAD_DIS2	(1<<4)
#define TCR_CRC_DIS2	(1<<3)
#define TCR_PAD_DIS1	(1<<2)
#define TCR_CRC_DIS1	(1<<1)
#define TCR_TXREQ	(1<<0)

#define TSR_TJTO	(1<<7)
#define TSR_LC		(1<<6)
#define TSR_NC		(1<<5)
#define TSR_LCOL	(1<<4)
#define TSR_COL		(1<<3)
#define TSR_EC		(1<<2)

#define RCR_WTDIS	(1<<6)
#define RCR_DIS_LONG	(1<<5)
#define RCR_DIS_CRC	(1<<4)
#define RCR_ALL		(1<<3)
#define RCR_RUNT	(1<<2)
#define RCR_PRMSC	(1<<1)
#define RCR_RXEN	(1<<0)

#define RSR_RF		(1<<7)
#define RSR_MF		(1<<6)
#define RSR_LCS		(1<<5)
#define RSR_RWTO	(1<<4)
#define RSR_PLE		(1<<3)
#define RSR_AE		(1<<2)
#define RSR_CE		(1<<1)
#define RSR_FOE		(1<<0)

#define FCTR_HWOT(ot)	(( ot & 0xf ) << 4 )
#define FCTR_LWOT(ot)	( ot & 0xf )

#define IMR_PAR		(1<<7)
#define IMR_LINKCHGM	(1<<5)
#define IMR_UDRUNM	(1<<4)
#define IMR_ROOM	(1<<3)
#define IMR_ROM		(1<<2)
#define IMR_PTM		(1<<1)
#define IMR_PRM		(1<<0)

#define ISR_IOMODE	(1<<7)
#define ISR_LINKCHG	(1<<5)
#define ISR_UDRUN	(1<<4)
#define ISR_ROOS	(1<<3)
#define ISR_ROS		(1<<2)
#define ISR_PTS		(1<<1)
#define ISR_PRS		(1<<0)
#define ISR_CLR_STATUS  (ISR_ROOS | ISR_ROS | ISR_PTS | ISR_PRS)

#define EPCR_REEP	(1<<5)
#define EPCR_WEP	(1<<4)
#define EPCR_EPOS	(1<<3)  /* Select PHY when set */
#define EPCR_ERPRR	(1<<2)
#define EPCR_ERPRW	(1<<1)
#define EPCR_ERRE	(1<<0)

#define GPCR_GEP_CNTL   (1<<0)

#define DM9000_PKT_RDY	0x01	/* Packet ready to receive */
#define DM9000_PKT_MAX	1536	/* Received packet max size */

#endif /* _DM9000X_H_ */
