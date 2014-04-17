/* ixp42x.h

   written by Marc Singer
   14 Jan 2005

   Copyright (C) 2005 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__IXP42X_H__)
#    define   __IXP42X_H__

/* ----- Includes */

#include <asm/reg.h>

/* ----- Types */

/* ----- Globals */

/* ----- Prototypes */

#define COPROCESSOR_WAIT\
 ({ unsigned long v; \
    __asm volatile ("mrc p15, 0, %0, c2, c0, 0\n\t" \
		    "mov %0, %0\n\t" \
		    "sub pc, pc, #4" : "=r" (v)); })

	/* Registers */

#define SDR_PHYS	(0xcc000000)
#define SDR_CONFIG	__REG(SDR_PHYS + 0x00)
#define SDR_REFRESH	__REG(SDR_PHYS + 0x04)	/* Refresh register */
#define SDR_IR		__REG(SDR_PHYS + 0x08)	/* Instruction register */

#define SDR_IR_MODE_CAS2	0x0
#define SDR_IR_MODE_CAS3	0x1
#define SDR_IR_PRECHARGE_ALL	0x2
#define SDR_IR_NOP		0x3
#define SDR_IR_AUTO_REFRESH	0x4
#define SDR_IR_BURST_TERMINATE	0x5
#define SDR_IR_NORMAL		0x6 /* *** undocumented? *** */

#define SDR_CONFIG_64MIB	(1<<5) /* 64 Mib chip enable */
#define SDR_CONFIG_RAS3		(1<<4) /* Enable RAS3 (always set) */
#define SDR_CONFIG_CAS3		(1<<3) /* Enable CAS3, CAS2 default */
#define SDR_CONFIG_2x8Mx16       0
#define SDR_CONFIG_4x8Mx16       1
#define SDR_CONFIG_2x16Mx16      2
#define SDR_CONFIG_4x16Mx16      3
#define SDR_CONFIG_2x32Mx16      4
#define SDR_CONFIG_4x32Mx16      5

#define EXP_PHYS	(0xc4000000)
#define EXP_TIMING_CS0	__REG(EXP_PHYS + 0x00)
#define EXP_TIMING_CS1	__REG(EXP_PHYS + 0x04)
#define EXP_TIMING_CS2	__REG(EXP_PHYS + 0x08)
#define EXP_TIMING_CS3	__REG(EXP_PHYS + 0x0c)
#define EXP_TIMING_CS4	__REG(EXP_PHYS + 0x10)
#define EXP_TIMING_CS5	__REG(EXP_PHYS + 0x14)
#define EXP_TIMING_CS6	__REG(EXP_PHYS + 0x18)
#define EXP_TIMING_CS7	__REG(EXP_PHYS + 0x1c)
#define EXP_CNFG0	__REG(EXP_PHYS + 0x20)
#define EXP_CNFG1	__REG(EXP_PHYS + 0x24)

#define EXP_CS_EN	       (1<<31)	/* Enable chip select */
#define EXP_BYTE_EN	       (1<<0)	/* Eight-bit device */
#define EXP_WR_EN	       (1<<1)	/* Enable writes */
#define EXP_SPLIT_EN	       (1<<3)	/* Enable split transfers */
#define EXP_MUX_EN	       (1<<4)	/* Multiplexed A/D */
#define EXP_HRDY_POL	       (1<<5)	/* HRDY polarity */
#define EXP_BYTE_RD16	       (1<<6)	/* Byte access to 16 bit device */
#define EXP_CNFG_SHIFT	       (10)	/* Device addressing range */
#define EXP_CNFG_MASK	       (0xf)
#define EXP_CYC_TYPE_SHIFT     (14)	/* Cycle type */
#define EXP_CYC_TYPE_MASK      (0x3)
#define EXP_T1_SHIFT	       (28)	/* Address timing */
#define EXP_T1_MASK	       (0x3)
#define EXP_T2_SHIFT	       (26)	/* Setup/CS timing */
#define EXP_T2_MASK	       (0x3)
#define EXP_T3_SHIFT	       (22)	/* Strobe timing */
#define EXP_T3_MASK	       (0xf)
#define EXP_T4_SHIFT	       (20)	/* Hold timing */
#define EXP_T4_MASK	       (0x3)
#define EXP_T5_SHIFT	       (20)	/* Recovery timing */
#define EXP_T5_MASK	       (0xf)

#define EXP_CNFG0_MEM_MAP      (1<<31) /* Boot mode mapping of EXP_CS0 */

#define OST_PHYS	(0xc8005000)
#define OST_TS		__REG(OST_PHYS + 0x00) /* Time-stamp timer */
#define OST_TS_PHYS	(OST_PHYS + 0x00)
#define OST_WDOG	__REG(OST_PHYS + 0x14) /* Watchdog timer */
#define OST_WDOG_ENAB	__REG(OST_PHYS + 0x18) /* Watchdog enable */
#define OST_WDOG_KEY	__REG(OST_PHYS + 0x1c) /* Watchdog enable */

#define UART0_PHYS	(0xc8000000) /* High-speed UART */
#define UART1_PHYS	(0xc8001000) /* Console UART */
#define UART_PHYS	(UART0_PHYS) /* Primary */
#define UART_SEC_PHYS	(UART1_PHYS) /* Secondary */

#define GPIO_PHYS	(0xc8004000)
#define GPIO_OUTR	__REG(GPIO_PHYS + 0x00)
#define GPIO_OER	__REG(GPIO_PHYS + 0x04)
#define GPIO_INR	__REG(GPIO_PHYS + 0x08)
#define GPIO_ISR	__REG(GPIO_PHYS + 0x0c)
#define GPIO_IT1R	__REG(GPIO_PHYS + 0x10)
#define GPIO_IT2R	__REG(GPIO_PHYS + 0x14)
#define GPIO_CLKR	__REG(GPIO_PHYS + 0x18)

#define GPIO_CLKR_CLK0DC_SHIFT 0
#define GPIO_CLKR_CLK0TC_SHIFT 4
#define GPIO_CLKR_MUX14	(1<<8)
#define GPIO_CLKR_CLK1DC_SHIFT 16
#define GPIO_CLKR_CLK1TC_SHIFT 20
#define GPIO_CLKR_MUX15 (1<<24)

  /* GPIO allocations from the Intel dev board */
//#define GPIO_I_POWERFAIL	0
//#define GPIO_I_DSL		1
//#define GPIO_I_SLIC_A		2
//#define GPIO_I_SLIC_B		3
//#define GPIO_I_DSP		4
//#define GPIO_I_IDE		5
//#define GPIO_I_SPI_CS1		7
//#define GPIO_I_SPI_CS0		8
//#define GPIO_I_SPI_SCK		9
//#define GPIO_I_SPI_SDI		10
//#define GPIO_I_IORESET		12
//#define GPIO_I_SPI_SDO		13

#define GPIO_INT_TYPE_ACTIVELO 1
#define GPIO_INT_TYPE_ACTIVEHI 0
#define GPIO_INT_TYPE_RISING   2
#define GPIO_INT_TYPE_FALLING  3

#define GPIO_OUT_ENABLE(i)	(GPIO_OER   &= ~(1 << (i)))
#define GPIO_OUT_DISABLE(i)	(GPIO_OER   |=   1 << (i))
#define GPIO_OUT_SET(i)		(GPIO_OUTR |=   1 << (i))
#define GPIO_OUT_CLEAR(i)	(GPIO_OUTR &= ~(1 << (i)))

#define GPIO_INT_TYPE_MASK(i)\
  (((i) > 7) ? (GPIO_IT2R &= ~(7 << (((i) - 8)*3)))\
	     : (GPIO_IT1R &= ~(7 << ( (i)     *3))))
#define GPIO_INT_TYPE_SET(i,v)\
  (((i) > 7) ? (GPIO_IT2R |= (v) << (((i) - 8)*3))\
	     : (GPIO_IT1R |= (v) << ( (i)     *3)))

#define GPIO_INT_TYPE(i,v) \
	({ GPIO_INT_TYPE_MASK (i); GPIO_INT_TYPE_SET (i,v); })

#define IXP4XX_INTC_PHYS	(0xc8003000)
#define	IXP4XX_NPEA_PHYS	(0xc8006000)
#define IXP4XX_NPEB_PHYS	(0xc8007000)
#define IXP4XX_NPEC_PHYS	(0xc8008000)
#define IXP4XX_ETHB_PHYS	(0xc8009000)
#define IXP4XX_ETHC_PHYS	(0xc800a000)

#define PCI_PHYS	(0xc0000000)
#define PCI_NP_AD	__REG(PCI_PHYS + 0x00)
#define PCI_NP_CBE	__REG(PCI_PHYS + 0x04)
#define PCI_NP_WDATA	__REG(PCI_PHYS + 0x08)
#define PCI_NP_RDATA	__REG(PCI_PHYS + 0x0c)
#define PCI_CRP_AD_CBE	__REG(PCI_PHYS + 0x10)
#define PCI_CRP_WDATA	__REG(PCI_PHYS + 0x14)
#define PCI_CRP_RDATA	__REG(PCI_PHYS + 0x18)
#define PCI_CSR		__REG(PCI_PHYS + 0x1c)
#define PCI_ISR		__REG(PCI_PHYS + 0x20)
#define PCI_INTEN	__REG(PCI_PHYS + 0x24)
#define PCI_DMACTRL	__REG(PCI_PHYS + 0x28)
#define PCI_AHBMEMBASE	__REG(PCI_PHYS + 0x2c)
#define PCI_AHBIOBASE	__REG(PCI_PHYS + 0x30)
#define PCI_PCIMEMBASE	__REG(PCI_PHYS + 0x34)

#define PCI_CSR_PRST	(1<<15)
#define PCI_CSR_IC	(1<<15)
#define PCI_CSR_ASE	(1<<8)
#define PCI_CSR_DBT	(1<<5)
#define PCI_CSR_ABE	(1<<4)
#define PCI_CSR_PDS	(1<<3)
#define PCI_CSR_ADS	(1<<2)
#define PCI_CSR_ARBEN	(1<<1)
#define PCI_CSR_HOST	(1<<0)

#define PCI_ISR_PSE	(1<<0)
#define PCI_ISR_PFE	(1<<1)
#define PCI_ISR_PPE	(1<<2)
#define PCI_ISR_AHBE	(1<<3)

#define PCI_CRP_AD_CBE_WRITE	(1<<16)
#define PCI_CRP_AD_CBE_READ	(0<<16)
#define PCI_CRP_AD_CBE_BE_SHIFT	20
#define PCI_CRP_AD_CBE_BE_MASK	(0xf<<20)

#define PCI_CFG_DIDVID			0x00
//#define PCI_CFG_VENDOR			0x00
//#define PCI_CFG_DEVICE			0x02
#define PCI_CFG_COMMAND			0x04
#define PCI_CFG_STATUS			0x06
#define PCI_CFG_CLASS_REV		0x08
#define PCI_CFG_CACHE_LINE_SIZE		0x0c
#define PCI_CFG_LATENCY_TIMER		0x0d
#define PCI_CFG_HEADER_TYPE		0x0e
#define PCI_CFG_BIST			0x0f
#define PCI_CFG_BAR_BASE		0x10
#define PCI_CFG_BAR_0			0x10
#define PCI_CFG_BAR_1			0x14
#define PCI_CFG_BAR_2			0x18
#define PCI_CFG_BAR_3			0x1c
#define PCI_CFG_BAR_4			0x20
#define PCI_CFG_BAR_5			0x24
#define PCI_CFG_CARDBUS_CIS		0x28
#define PCI_CFG_SUB_VENDOR		0x2c
#define PCI_CFG_SUB_ID			0x2e
#define PCI_CFG_ROM_ADDRESS		0x30
#define PCI_CFG_CAP_LIST		0x34
#define PCI_CFG_INT_LINE		0x3c
#define PCI_CFG_INT_PIN			0x3d
#define PCI_CFG_MIN_GNT			0x3e
#define PCI_CFG_MAX_LAT			0x3f

#define PCI_CFG_COMMAND_IO		(1<<0)
#define PCI_CFG_COMMAND_MEMORY		(1<<1)
#define PCI_CFG_COMMAND_MASTER		(1<<2)
#define PCI_CFG_COMMAND_SPECIAL		(1<<3)
#define PCI_CFG_COMMAND_INVALIDATE	(1<<4)
#define PCI_CFG_COMMAND_VGA_SNOOP	(1<<5)
#define PCI_CFG_COMMAND_PARITY		(1<<6)
#define PCI_CFG_COMMAND_WAIT		(1<<7)
#define PCI_CFG_COMMAND_SERR		(1<<8)
#define PCI_CFG_COMMAND_FAST_BACK	(1<<9)

#define PCI_CONFIG_WRITE32(o,v) {\
    PCI_CRP_AD_CBE = ((o) & ~3) | PCI_CRP_AD_CBE_WRITE;\
    PCI_CRP_WDATA = (v); }

#define PCI_CONFIG_WRITE16(o,v) {\
    PCI_CRP_AD_CBE = ((o) & ~3) | PCI_CRP_AD_CBE_WRITE\
      | ((((3 << (PCI_CRP_AD_CBE_BE_SHIFT + (o & 3)))\
	 ^ PCI_CRP_AD_CBE_BE_MASK) & PCI_CRP_AD_CBE_BE_MASK));\
    PCI_CRP_WDATA = (v) << (8 * ((o) & 3)); }

#define PCI_CONFIG_READ32(o) (\
    PCI_CRP_AD_CBE = ((o) & ~3) | PCI_CRP_AD_CBE_READ\
      | ((((3 << (PCI_CRP_AD_CBE_BE_SHIFT + (o & 3)))\
	 ^ PCI_CRP_AD_CBE_BE_MASK) & PCI_CRP_AD_CBE_BE_MASK)),\
    PCI_CRP_RDATA )

#endif  /* __IXP42X_H__ */
