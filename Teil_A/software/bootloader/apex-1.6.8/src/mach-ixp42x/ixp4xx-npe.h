/* ixp4xx-npe.h

   written by Marc Singer
   20 Dec 2006

   Copyright (C) 2006 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__IXP4XX_NPE_H__)
#    define   __IXP4XX_NPE_H__

/* ----- Includes */

#include <asm/reg.h>

/* ----- Types */

/* ----- Globals */

/* ----- Prototypes */

#define NPEA_PHYS	IXP4XX_NPEA_PHYS
#define NPEB_PHYS	IXP4XX_NPEB_PHYS
#define NPEC_PHYS	IXP4XX_NPEC_PHYS

#define NPEA_NAME	"NPE-A"
#define NPEA_DATA_SIZE	(0x800)
#define NPEA_INST_SIZE	(0x1000)

#define NPEB_NAME	"NPE-B"
#define NPEB_DATA_SIZE	(0x800)
#define NPEB_INST_SIZE	(0x1000)

#define NPEC_NAME	"NPE-C"
#define NPEC_DATA_SIZE	(0x800)
#define NPEC_INST_SIZE	(0x1000)

#define MAC_TX_CNTRL1       0x00  // 0x000
#define MAC_TX_CNTRL2       0x01  // 0x004
#define MAC_RX_CNTRL1       0x04  // 0x010
#define MAC_RX_CNTRL2       0x05  // 0x014
#define MAC_RANDOM_SEED     0x08  // 0x020
#define MAC_THRESH_P_EMPTY  0x0c  // 0x030
#define MAC_THRESH_P_FULL   0x0e  // 0x038
#define MAC_BUF_SIZE_TX     0x10  // 0x040
#define MAC_TX_DEFER        0x14  // 0x050
#define MAC_RX_DEFER        0x15  // 0x054
#define MAC_TX_TWO_DEFER_1  0x18  // 0x060
#define MAC_TX_TWO_DEFER_2  0x19  // 0x064
#define MAC_SLOT_TIME       0x1c  // 0x070
#define MAC_MDIO_CMD        0x20  // 0x080 4 registers 0x20 - 0x23
#define MAC_MDIO_STS        0x24  // 0x090 4 registers 0x24 - 0x27
#define MAC_ADDR_MASK       0x28  // 0x0A0 6 registers 0x28 - 0x2d
#define MAC_ADDR            0x30  // 0x0C0 6 registers 0x30 - 0x35
#define MAC_INT_CLK_THRESH  0x38  // 0x0E0 1 register
#define MAC_UNI_ADDR        0x3c  // 0x0F0 6 registers 0x3c - 0x41
#define MAC_CORE_CNTRL      0x7f  // 0x1fC

#define MAC_REG_LIST { \
	MAC_TX_CNTRL1, MAC_TX_CNTRL2, \
	MAC_RX_CNTRL1, MAC_RX_CNTRL2, \
	MAC_RANDOM_SEED, MAC_THRESH_P_EMPTY, MAC_THRESH_P_FULL, \
	MAC_BUF_SIZE_TX, MAC_TX_DEFER, MAC_RX_DEFER, \
	MAC_TX_TWO_DEFER_1, MAC_TX_TWO_DEFER_2, MAC_SLOT_TIME, \
	MAC_ADDR_MASK +0, MAC_ADDR_MASK +1, MAC_ADDR_MASK +2, \
	MAC_ADDR_MASK +3, MAC_ADDR_MASK +4, MAC_ADDR_MASK +5, \
	MAC_ADDR +0, MAC_ADDR +1, MAC_ADDR +2, \
	MAC_ADDR +3, MAC_ADDR +4, MAC_ADDR +5, \
	MAC_INT_CLK_THRESH, \
	MAC_UNI_ADDR +0, MAC_UNI_ADDR +1, MAC_UNI_ADDR +2, \
	MAC_UNI_ADDR +3, MAC_UNI_ADDR +4, MAC_UNI_ADDR +5, \
	MAC_CORE_CNTRL, \
}

/* TX Control Register 1*/

#define TX_CNTRL1_TX_EN         (1<<0)
#define TX_CNTRL1_DUPLEX        (1<<1)
#define TX_CNTRL1_RETRY         (1<<2)
#define TX_CNTRL1_PAD_EN        (1<<3)
#define TX_CNTRL1_FCS_EN        (1<<4)
#define TX_CNTRL1_2DEFER        (1<<5)
#define TX_CNTRL1_RMII          (1<<6)

/* TX Control Register 2 */
#define TX_CNTRL2_RETRIES_MASK  0xf

/* RX Control Register 1 */
#define RX_CNTRL1_RX_EN         (1<<0)
#define RX_CNTRL1_PADSTRIP_EN   (1<<1)
#define RX_CNTRL1_CRC_EN        (1<<2)
#define RX_CNTRL1_PAUSE_EN      (1<<3)
#define RX_CNTRL1_LOOP_EN       (1<<4)
#define RX_CNTRL1_ADDR_FLTR_EN  (1<<5)
#define RX_CNTRL1_RX_RUNT_EN    (1<<6)
#define RX_CNTRL1_BCAST_DIS     (1<<7)

/* RX Control Register 2 */
#define RX_CNTRL2_DEFER_EN      (1<<0)

/* Core Control Register */
#define CORE_RESET              (1<<0)
#define CORE_RX_FIFO_FLUSH      (1<<1)
#define CORE_TX_FIFO_FLUSH      (1<<2)
#define CORE_SEND_JAM           (1<<3)
#define CORE_MDC_EN             (1<<4)

/* Definitions for MII access routines*/

#define MII_REG_SHL    16
#define MII_ADDR_SHL   21

#define MII_GO                  (1<<31)
#define MII_WRITE               (1<<26)
#define MII_READ_FAIL           (1<<31)

#define MII_TIMEOUT_10TH_SECS        5
#define MII_10TH_SEC_IN_MILLIS     100

#define MAC_TX_CNTRL1_DEFAULT\
	(TX_CNTRL1_TX_EN | TX_CNTRL1_RETRY | TX_CNTRL1_FCS_EN\
	| TX_CNTRL1_2DEFER | TX_CNTRL1_PAD_EN)

#define MAC_TX_MAX_RETRIES_DEFAULT (0x0f)

#define MAC_RX_CNTRL1_DEFAULT\
	(RX_CNTRL1_PADSTRIP_EN | RX_CNTRL1_CRC_EN | RX_CNTRL1_RX_EN)

#define MAC_RX_CNTRL2_DEFAULT       (0x0)
#define MAC_TX_CNTRL2_DEFAULT       TX_CNTRL2_RETRIES_MASK

/* Thresholds determined by NPE firmware FS */
#define MAC_THRESH_P_EMPTY_DEFAULT  (0x12)
#define MAC_THRESH_P_FULL_DEFAULT   (0x30)

/* Number of bytes that must be in the tx fifo before
 * transmission commences */
#define MAC_BUF_SIZE_TX_DEFAULT     (0x8)

/* One-part deferral values */
#define MAC_TX_DEFER_DEFAULT        (0x15)
#define MAC_RX_DEFER_DEFAULT        (0x16)

/* Two-part deferral values... */
#define MAC_TX_TWO_DEFER_1_DEFAULT  (0x08)
#define MAC_TX_TWO_DEFER_2_DEFAULT  (0x07)

/* This value applies to MII */
#define MAC_SLOT_TIME_DEFAULT       (0x80)

/* This value applies to RMII */
#define MAC_SLOT_TIME_RMII_DEFAULT  (0xFF)

#define MAC_ADDR_MASK_DEFAULT       (0xFF)

#define MAC_INT_CLK_THRESH_DEFAULT  (0x1)
/* The following is a value chosen at random */
#define RANDOM_SEED_DEFAULT         (0x8)

/* By default we must configure the MAC to generate the MDC clock*/
#define CORE_DEFAULT                (CORE_MDC_EN)

/* End of Intel provided register information */


/* Execution Address  */
#define IX_NPEDL_REG_OFFSET_EXAD             0x00
/* Execution Data */
#define IX_NPEDL_REG_OFFSET_EXDATA           0x04
/* Execution Control */
#define IX_NPEDL_REG_OFFSET_EXCTL            0x08
/* Execution Count */
#define IX_NPEDL_REG_OFFSET_EXCT             0x0C
/* Action Point 0 */
#define IX_NPEDL_REG_OFFSET_AP0              0x10
/* Action Point 1 */
#define IX_NPEDL_REG_OFFSET_AP1              0x14
/* Action Point 2 */
#define IX_NPEDL_REG_OFFSET_AP2              0x18
/* Action Point 3 */
#define IX_NPEDL_REG_OFFSET_AP3              0x1C
/* Watchpoint FIFO */
#define IX_NPEDL_REG_OFFSET_WFIFO            0x20
/* Watch Count */
#define IX_NPEDL_REG_OFFSET_WC               0x24
/* Profile Count */
#define IX_NPEDL_REG_OFFSET_PROFCT           0x28

/* Messaging Status */
#define IX_NPEDL_REG_OFFSET_STAT	     0x2C
/* Messaging Control */
#define IX_NPEDL_REG_OFFSET_CTL	             0x30
/* Mailbox Status */
#define IX_NPEDL_REG_OFFSET_MBST	     0x34
/* messaging in/out FIFO */
#define IX_NPEDL_REG_OFFSET_FIFO	     0x38


#define IX_NPEDL_MASK_ECS_DBG_REG_2_IF       0x00100000
#define IX_NPEDL_MASK_ECS_DBG_REG_2_IE       0x00080000
#define IX_NPEDL_MASK_ECS_REG_0_ACTIVE       0x80000000

#define IX_NPEDL_EXCTL_CMD_NPE_STEP          0x01
#define IX_NPEDL_EXCTL_CMD_NPE_START         0x02
#define IX_NPEDL_EXCTL_CMD_NPE_STOP          0x03
#define IX_NPEDL_EXCTL_CMD_NPE_CLR_PIPE      0x04
#define IX_NPEDL_EXCTL_CMD_CLR_PROFILE_CNT   0x0C
#define IX_NPEDL_EXCTL_CMD_RD_INS_MEM        0x10
#define IX_NPEDL_EXCTL_CMD_WR_INS_MEM        0x11
#define IX_NPEDL_EXCTL_CMD_RD_DATA_MEM       0x12
#define IX_NPEDL_EXCTL_CMD_WR_DATA_MEM       0x13
#define IX_NPEDL_EXCTL_CMD_RD_ECS_REG        0x14
#define IX_NPEDL_EXCTL_CMD_WR_ECS_REG        0x15

#define IX_NPEDL_EXCTL_STATUS_RUN            0x80000000
#define IX_NPEDL_EXCTL_STATUS_STOP           0x40000000
#define IX_NPEDL_EXCTL_STATUS_CLEAR          0x20000000

#define IX_NPEDL_MASK_WFIFO_VALID            0x80000000
#define IX_NPEDL_MASK_STAT_OFNE              0x00010000
#define IX_NPEDL_MASK_STAT_IFNE              0x00080000

#define IX_NPEDL_ECS_DBG_CTXT_REG_0          0x0C
#define IX_NPEDL_ECS_PRI_1_CTXT_REG_0        0x04
#define IX_NPEDL_ECS_PRI_2_CTXT_REG_0        0x08

/* NPE control register bit definitions */
#define IX_NPEMH_NPE_CTL_OFE   (1 << 16) /**< OutFifoEnable */
#define IX_NPEMH_NPE_CTL_IFE   (1 << 17) /**< InFifoEnable */
#define IX_NPEMH_NPE_CTL_OFEWE (1 << 24) /**< OutFifoEnableWriteEnable */
#define IX_NPEMH_NPE_CTL_IFEWE (1 << 25) /**< InFifoEnableWriteEnable */

/* NPE status register bit definitions */
#define IX_NPEMH_NPE_STAT_OFNE  (1 << 16) /**< OutFifoNotEmpty */
#define IX_NPEMH_NPE_STAT_IFNF  (1 << 17) /**< InFifoNotFull */
#define IX_NPEMH_NPE_STAT_OFNF  (1 << 18) /**< OutFifoNotFull */
#define IX_NPEMH_NPE_STAT_IFNE  (1 << 19) /**< InFifoNotEmpty */
#define IX_NPEMH_NPE_STAT_MBINT (1 << 20) /**< Mailbox interrupt */
#define IX_NPEMH_NPE_STAT_IFINT (1 << 21) /**< InFifo interrupt */
#define IX_NPEMH_NPE_STAT_OFINT (1 << 22) /**< OutFifo interrupt */
#define IX_NPEMH_NPE_STAT_WFINT (1 << 23) /**< WatchFifo interrupt */


#endif  /* __IXP42X_NPE_H__ */
