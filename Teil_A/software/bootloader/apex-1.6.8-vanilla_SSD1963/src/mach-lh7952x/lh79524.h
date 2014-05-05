/* lh79524.h

   written by Marc Singer
   31 Oct 2004

   Copyright (C) 2004 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__LH79524_H__)
#    define   __LH79524_H__

/* ----- Includes */

#include <asm/reg.h>

/* ----- Types */

/* ----- Globals */

/* ----- Prototypes */

	/* Registers */

#define ADC_PHYS	(0xfffc3000)
#define ADC_HW		__REG(ADC_PHYS + 0x00)	/* High Word (RO) */
#define ADC_LW		__REG(ADC_PHYS + 0x04)	/* Low Word (RO) */
#define ADC_RR		__REG(ADC_PHYS + 0x08)	/* Results (RO) */
#define ADC_IM		__REG(ADC_PHYS + 0x0c)	/* Interrupt Masking */
#define ADC_PC		__REG(ADC_PHYS + 0x10)	/* Power Configuration */
#define ADC_GC		__REG(ADC_PHYS + 0x14)	/* General Configuration */
#define ADC_GS		__REG(ADC_PHYS + 0x18)	/* General Status */
#define ADC_IS		__REG(ADC_PHYS + 0x1c)	/* Interrupt Status */
#define ADC_FS		__REG(ADC_PHYS + 0x20)	/* FIFO Status */
#define ADC_LWC_BASE	__REG(ADC_PHYS + 0x64)	/* Low Word Control (0-15) */
#define ADC_HWC_BASE_PHYS	(ADC_PHYS + 0x24)
#define ADC_HWC_BASE	__REG(ADC_PHYS + 0x24)	/* High Word Control (0-15) */
#define ADC_LWC_BASE_PHYS	(ADC_PHYS + 0x64)
#define ADC_IHWCTRL	__REG(ADC_PHYS + 0xa4)	/* Idle High Word Control */
#define ADC_ILWCTRL	__REG(ADC_PHYS + 0xa8)	/* Idle Low word Control */
#define ADC_MIS		__REG(ADC_PHYS + 0xac)	/* Masked Interrupt Status  */
#define ADC_IC		__REG(ADC_PHYS + 0xb0)	/* Interrupt clear */

#define BOOT_PHYS	(0xfffe6000)

#define BOOT_PBC	__REG(BOOT_PHYS + 0x00)
#define BOOT_CS1OV	__REG(BOOT_PHYS + 0x04)
#define BOOT_EPM	__REG(BOOT_PHYS + 0x08)

#define RCPC_PHYS	(0xfffe2000)

#define RCPC_CTRL	__REG(RCPC_PHYS + 0x00)
#define RCPC_CHIPID	__REG(RCPC_PHYS + 0x04)
#define RCPC_REMAP	__REG(RCPC_PHYS + 0x08)
#define RCPC_SOFTRESET	__REG(RCPC_PHYS + 0x0c)
#define RCPC_SYSCLKPRE	__REG(RCPC_PHYS + 0x18)
#define RCPC_CPUCLKPRE	__REG(RCPC_PHYS + 0x1c)
#define RCPC_CPUCLKPRE	__REG(RCPC_PHYS + 0x1c)
#define RCPC_PCLKCTRL0	__REG(RCPC_PHYS + 0x24)
#define RCPC_PCLKCTRL1	__REG(RCPC_PHYS + 0x28)
#define RCPC_AHBCLKCTRL	__REG(RCPC_PHYS + 0x2c)
#define RCPC_PCLKSEL0	__REG(RCPC_PHYS + 0x30)
#define RCPC_PCLKSEL1	__REG(RCPC_PHYS + 0x34)
#define RCPC_LCDPRE	__REG(RCPC_PHYS + 0x40)
#define RCPC_SSPPRE	__REG(RCPC_PHYS + 0x44)
#define RCPC_ADCPRE	__REG(RCPC_PHYS + 0x48)
#define RCPC_INTCONFIG	__REG(RCPC_PHYS + 0x80)
#define RCPC_INTCLR	__REG(RCPC_PHYS + 0x84)
#define RCPC_CORECONFIG	__REG(RCPC_PHYS + 0x88)
#define RCPC_SYSPLLCNTL	__REG(RCPC_PHYS + 0xc0)

#define EMC_PHYS	(0xffff1000)

#define EMC_CONTROL	__REG(EMC_PHYS + 0x00)
#define EMC_STATUS	__REG(EMC_PHYS + 0x04)
#define EMC_CONFIG	__REG(EMC_PHYS + 0x08)

#define EMC_DYNMCTRL	__REG(EMC_PHYS + 0x20)
#define EMC_DYNMREF	__REG(EMC_PHYS + 0x24)
#define EMC_READCONFIG	__REG(EMC_PHYS + 0x28)	/* Undocumented */
#define EMC_PRECHARGE	__REG(EMC_PHYS + 0x30)	/* rp */
#define EMC_DYNM2PRE	__REG(EMC_PHYS + 0x34)	/* ras */
#define EMC_REFEXIT	__REG(EMC_PHYS + 0x38)	/* rex */
#define EMC_DOACTIVE	__REG(EMC_PHYS + 0x3c)	/* apr */
#define EMC_DIACTIVE	__REG(EMC_PHYS + 0x40)	/* dal */
#define EMC_DWRT	__REG(EMC_PHYS + 0x44)	/* wr */

#define EMC_DYNACTCMD	__REG(EMC_PHYS + 0x48)	/* rc */
#define EMC_DYNAUTO	__REG(EMC_PHYS + 0x4c)	/* rfc */
#define EMC_DYNREFEXIT	__REG(EMC_PHYS + 0x50)	/* xsr */
#define EMC_DYNACTIVEAB	__REG(EMC_PHYS + 0x54)	/* rrd */
#define EMC_DYNAMICTMRD	__REG(EMC_PHYS + 0x58)	/* mrd */

#define EMC_WAIT	__REG(EMC_PHYS + 0x80)

#define EMC_DYNCFG0	__REG(EMC_PHYS + 0x100)
#define EMC_DYNRASCAS0	__REG(EMC_PHYS + 0x104)
#define EMC_DYNCFG1	__REG(EMC_PHYS + 0x120)
#define EMC_DYNRASCAS1	__REG(EMC_PHYS + 0x124)

#define EMC_SCONFIG0	__REG(EMC_PHYS + 0x200)
#define EMC_SWAITWEN0	__REG(EMC_PHYS + 0x204)
#define EMC_SWAITOEN0	__REG(EMC_PHYS + 0x208)
#define EMC_SWAITRD0	__REG(EMC_PHYS + 0x20c)
#define EMC_SWAITPAGE0	__REG(EMC_PHYS + 0x210)
#define EMC_SWAITWR0	__REG(EMC_PHYS + 0x214)
#define EMC_STURN0	__REG(EMC_PHYS + 0x218)

#define EMC_SCONFIG1	__REG(EMC_PHYS + 0x220)
#define EMC_SWAITWEN1	__REG(EMC_PHYS + 0x224)
#define EMC_SWAITOEN1	__REG(EMC_PHYS + 0x228)
#define EMC_SWAITRD1	__REG(EMC_PHYS + 0x22c)
#define EMC_SWAITPAGE1	__REG(EMC_PHYS + 0x230)
#define EMC_SWAITWR1	__REG(EMC_PHYS + 0x234)
#define EMC_STURN1	__REG(EMC_PHYS + 0x238)

#define EMC_SCONFIG2	__REG(EMC_PHYS + 0x240)
#define EMC_SWAITWEN2	__REG(EMC_PHYS + 0x244)
#define EMC_SWAITOEN2	__REG(EMC_PHYS + 0x248)
#define EMC_SWAITRD2	__REG(EMC_PHYS + 0x24c)
#define EMC_SWAITPAGE2	__REG(EMC_PHYS + 0x250)
#define EMC_SWAITWR2	__REG(EMC_PHYS + 0x254)
#define EMC_STURN2	__REG(EMC_PHYS + 0x258)

#define EMC_SCONFIG3	__REG(EMC_PHYS + 0x260)
#define EMC_SWAITWEN3	__REG(EMC_PHYS + 0x264)
#define EMC_SWAITOEN3	__REG(EMC_PHYS + 0x268)
#define EMC_SWAITRD3	__REG(EMC_PHYS + 0x26c)
#define EMC_SWAITPAGE3	__REG(EMC_PHYS + 0x270)
#define EMC_SWAITWR3	__REG(EMC_PHYS + 0x274)
#define EMC_STURN3	__REG(EMC_PHYS + 0x278)

#define IOCON_PHYS	(0xfffe5000)

#define IOCON_MUXCTL1	__REG(IOCON_PHYS + 0x00)
#define IOCON_RESCTL1	__REG(IOCON_PHYS + 0x04)
#define IOCON_MUXCTL5	__REG(IOCON_PHYS + 0x20)
#define IOCON_RESCTL5	__REG(IOCON_PHYS + 0x24)
#define IOCON_MUXCTL6	__REG(IOCON_PHYS + 0x28)
#define IOCON_MUXCTL7	__REG(IOCON_PHYS + 0x30)
#define IOCON_RESCTL7	__REG(IOCON_PHYS + 0x34)
#define IOCON_MUXCTL10	__REG(IOCON_PHYS + 0x48)
#define IOCON_MUXCTL11	__REG(IOCON_PHYS + 0x50)
#define IOCON_MUXCTL12	__REG(IOCON_PHYS + 0x58)
#define IOCON_MUXCTL14	__REG(IOCON_PHYS + 0x68)
#define IOCON_MUXCTL19	__REG(IOCON_PHYS + 0x90)
#define IOCON_RESCTL19	__REG(IOCON_PHYS + 0x94)
#define IOCON_MUXCTL20	__REG(IOCON_PHYS + 0x98)
#define IOCON_RESCTL20	__REG(IOCON_PHYS + 0x9c)
#define IOCON_MUXCTL21	__REG(IOCON_PHYS + 0xa0)
#define IOCON_RESCTL21	__REG(IOCON_PHYS + 0xa4)
#define IOCON_MUXCTL22	__REG(IOCON_PHYS + 0xa8)
#define IOCON_RESCTL22	__REG(IOCON_PHYS + 0xac)
#define IOCON_MUXCTL23	__REG(IOCON_PHYS + 0xb0)
#define IOCON_RESCTL23	__REG(IOCON_PHYS + 0xb4)
#define IOCON_MUXCTL24	__REG(IOCON_PHYS + 0xb8)
#define IOCON_RESCTL24	__REG(IOCON_PHYS + 0xbc)
#define IOCON_MUXCTL25	__REG(IOCON_PHYS + 0xc0)

#define DMA_PHYS	(0xfffe1000)
#define DMA0_PHYS	(0xfffe1000)
#define DMA1_PHYS	(0xfffe1040)
#define DMA2_PHYS	(0xfffe1080)
#define DMA3_PHYS	(0xfffe10c0)

#define DMA0_SOURCELO	__REG(DMA0_PHYS + 0x00)
#define DMA0_SOURCEHI	__REG(DMA0_PHYS + 0x04)
#define DMA0_DESTLO	__REG(DMA0_PHYS + 0x08)
#define DMA0_DESTHI	__REG(DMA0_PHYS + 0x0c)
#define DMA0_MAX	__REG(DMA0_PHYS + 0x10)
#define DMA0_CTRL	__REG(DMA0_PHYS + 0x14)
#define DMA0_CURSLO	__REG(DMA0_PHYS + 0x18)
#define DMA0_CURSHI	__REG(DMA0_PHYS + 0x1c)
#define DMA0_CURDLO	__REG(DMA0_PHYS + 0x20)
#define DMA0_CURDHI	__REG(DMA0_PHYS + 0x24)
#define DMA0_TCNT	__REG(DMA0_PHYS + 0x28)

#define DMA1_SOURCELO	__REG(DMA1_PHYS + 0x00)
#define DMA1_SOURCEHI	__REG(DMA1_PHYS + 0x04)
#define DMA1_DESTLO	__REG(DMA1_PHYS + 0x08)
#define DMA1_DESTHI	__REG(DMA1_PHYS + 0x0c)
#define DMA1_MAX	__REG(DMA1_PHYS + 0x10)
#define DMA1_CTRL	__REG(DMA1_PHYS + 0x14)
#define DMA1_CURSLO	__REG(DMA1_PHYS + 0x18)
#define DMA1_CURSHI	__REG(DMA1_PHYS + 0x1c)
#define DMA1_CURDLO	__REG(DMA1_PHYS + 0x20)
#define DMA1_CURDHI	__REG(DMA1_PHYS + 0x24)
#define DMA1_TCNT	__REG(DMA1_PHYS + 0x28)

#define DMA_MASK	__REG(DMA_PHYS + 0xf0)
#define DMA_CLR		__REG(DMA_PHYS + 0xf4)
#define DMA_STATUS	__REG(DMA_PHYS + 0xf8)

#define UART_PHYS	(0xfffc0000)

#define UART0_PHYS	(UART_PHYS + 0x0000)
#define UART1_PHYS	(UART_PHYS + 0x1000)
#define UART2_PHYS	(UART_PHYS + 0x2000)
#define UART		(UART0_PHYS)

#define TIMER_PHYS	(0xfffc4000)

#define TIMER0_PHYS	(TIMER_PHYS + 0x00)
#define TIMER1_PHYS	(TIMER_PHYS + 0x30)
#define TIMER2_PHYS	(TIMER_PHYS + 0x50)

#define IRQ_TIMER0	4
#define IRQ_TIMER1	5
#define IRQ_TIMER2	6

#define TIMER1_CTRL	__REG(TIMER1_PHYS + 0x00)
#define TIMER1_INTEN	__REG(TIMER1_PHYS + 0x04)
#define TIMER1_STATUS	__REG(TIMER1_PHYS + 0x08)
#define TIMER1_CNT	__REG(TIMER1_PHYS + 0x0c)
#define TIMER1_CMP0	__REG(TIMER1_PHYS + 0x10)
#define TIMER1_CMP1	__REG(TIMER1_PHYS + 0x14)
#define TIMER1_CAPA	__REG(TIMER1_PHYS + 0x18)
#define TIMER1_CAPB	__REG(TIMER1_PHYS + 0x1c)

#define TIMER2_CTRL	__REG(TIMER2_PHYS + 0x00)
#define TIMER2_INTEN	__REG(TIMER2_PHYS + 0x04)
#define TIMER2_STATUS	__REG(TIMER2_PHYS + 0x08)
#define TIMER2_CNT	__REG(TIMER2_PHYS + 0x0c)
#define TIMER2_CMP0	__REG(TIMER2_PHYS + 0x10)
#define TIMER2_CMP1	__REG(TIMER2_PHYS + 0x14)
#define TIMER2_CAPA	__REG(TIMER2_PHYS + 0x18)
#define TIMER2_CAPB	__REG(TIMER2_PHYS + 0x1c)

#define TIMER_CTRL_CCL		(1<<0)
#define TIMER_CTRL_CS		(1<<1)
#define TIMER_CTRL_TC		(1<<13)	/* Clear counter when CMP1 reached */
#define TIMER_CTRL_CASCADE	(1<<4)
#define TIMER_CTRL_SCALE_MASK	(3<<2)
#define TIMER_CTRL_SCALE_2	(0<<2)
#define TIMER_CTRL_SCALE_4	(1<<2)
#define TIMER_CTRL_SCALE_8	(2<<2)
#define TIMER_CTRL_SCALE_16	(3<<2)
#define TIMER_CTRL_SCALE_32	(4<<2)
#define TIMER_CTRL_SCALE_64	(5<<2)
#define TIMER_CTRL_SCALE_128	(6<<2)
#define TIMER_INTEN_OVF		(1<<0) /* overflow */
#define TIMER_INTEN_CMP0	(1<<1) /* compare 0 */
#define TIMER_INTEN_CMP1	(1<<2) /* compare 1 */

#define RTC_PHYS	(0xfffe0000)
#define RTC_DR		__REG (RTC_PHYS + 0x00)
#define RTC_LR		__REG (RTC_PHYS + 0x08)
#define RTC_CR		__REG (RTC_PHYS + 0x0c)

#define GPIO_AB_PHYS	__REG(0xfffdf000)
#define GPIO_CD_PHYS	__REG(0xfffde000)
#define GPIO_EF_PHYS    __REG(0xfffdd000)
#define GPIO_GH_PHYS	__REG(0xfffdc000)
#define GPIO_IJ_PHYS	__REG(0xfffdb000)
#define GPIO_KL_PHYS	__REG(0xfffda000)
#define GPIO_MN_PHYS	__REG(0xfffd9000)

#define SSP_PHYS	(0xfffc6000)
#define I2S_PHYS	(0xfffc8000)

#define VIC_PHYS	0xfffff000
#define VIC_IRQSTATUS	__REG(VIC_PHYS + 0x00)
#define VIC_FIQSTATUS	__REG(VIC_PHYS + 0x04)
#define VIC_RAWINTSR	__REG(VIC_PHYS + 0x08)
#define VIC_INTSELECT	__REG(VIC_PHYS + 0x0c)
#define VIC_INTENABLE	__REG(VIC_PHYS + 0x10)
#define VIC_INTENCLEAR	__REG(VIC_PHYS + 0x14)
#define VIC_SOFTINT	__REG(VIC_PHYS + 0x18)
#define VIC_SOFTINT_CLEAR __REG(VIC_PHYS + 0x1c)
#define VIC_VECTADDR	__REG(VIC_PHYS + 0x30)
#define VIC_DEFVECTADDR	__REG(VIC_PHYS + 0x34)
#define VIC_VECTADDR0	__REG(VIC_PHYS + 0x100)
#define VIC_VECTCTRL0	__REG(VIC_PHYS + 0x200)
#define VIC_ITOP	__REG(VIC_PHYS + 0x30c)

#define ALI_PHYS	(0xfffe4000) /* Advanced LCD Interface */
#define ALI_SETUP	__REG(ALI_PHYS + 0x00)
#define ALI_CTRL	__REG(ALI_PHYS + 0x04)
#define ALI_TIMING1	__REG(ALI_PHYS + 0x08)
#define ALI_TIMING2	__REG(ALI_PHYS + 0x0c)

#define CLCDC_PHYS	(0xffff4000)
#define CLCDC_TIMING0	__REG(CLCDC_PHYS + 0x00)
#define CLCDC_TIMING1	__REG(CLCDC_PHYS + 0x04)
#define CLCDC_TIMING2	__REG(CLCDC_PHYS + 0x08)
#define CLCDC_UPBASE	__REG(CLCDC_PHYS + 0x10)
#define CLCDC_LPBASE	__REG(CLCDC_PHYS + 0x14)
#define CLCDC_INTREN	__REG(CLCDC_PHYS + 0x18)
#define CLCDC_CTRL	__REG(CLCDC_PHYS + 0x1c)
#define CLCDC_STATUS	__REG(CLCDC_PHYS + 0x20)
#define CLCDC_INTERRUPT	__REG(CLCDC_PHYS + 0x24)
#define CLCDC_UPCURR	__REG(CLCDC_PHYS + 0x28)
#define CLCDC_LPCURR	__REG(CLCDC_PHYS + 0x2c)
#define CLCDC_PALETTE	__REG(CLCDC_PHYS + 0x200)

#define EMAC_PHYS		0xfffc7000
#define EMAC_NETCTL		__REG(EMAC_PHYS + 0x00)
#define EMAC_NETCONFIG		__REG(EMAC_PHYS + 0x04)
#define EMAC_NETSTATUS		__REG(EMAC_PHYS + 0x08)
#define EMAC_TXSTATUS		__REG(EMAC_PHYS + 0x14)
#define EMAC_RXBQP		__REG(EMAC_PHYS + 0x18)
#define EMAC_TXBQP		__REG(EMAC_PHYS + 0x1c)
#define EMAC_RXSTATUS		__REG(EMAC_PHYS + 0x20)
#define EMAC_INTSTATUS		__REG(EMAC_PHYS + 0x24)
#define EMAC_ENABLE		__REG(EMAC_PHYS + 0x28)
#define EMAC_DISABLE		__REG(EMAC_PHYS + 0x2c)
#define EMAC_MASK		__REG(EMAC_PHYS + 0x30)
#define EMAC_PHYMAINT		__REG(EMAC_PHYS + 0x34)
#define EMAC_PAUSETIME		__REG(EMAC_PHYS + 0x38)
#define EMAC_TXPAUSEQUAN	__REG(EMAC_PHYS + 0xbc)
#define EMAC_PAUSERRX		__REG(EMAC_PHYS + 0x3c)
#define EMAC_FRMTXOK		__REG(EMAC_PHYS + 0x40)
#define EMAC_SINGLECOL		__REG(EMAC_PHYS + 0x44)
#define EMAC_MULTFRM		__REG(EMAC_PHYS + 0x48)
#define EMAC_FRMRXOK		__REG(EMAC_PHYS + 0x4c)
#define EMAC_FRCHK		__REG(EMAC_PHYS + 0x50)
#define EMAC_ALIGNERR		__REG(EMAC_PHYS + 0x54)
#define EMAC_DEFTXFRM		__REG(EMAC_PHYS + 0x58)
#define EMAC_LATECOL		__REG(EMAC_PHYS + 0x5c)
#define EMAC_EXCOL		__REG(EMAC_PHYS + 0x60)
#define EMAC_TXUNDER		__REG(EMAC_PHYS + 0x64)
#define EMAC_SENSERR		__REG(EMAC_PHYS + 0x68)
#define EMAC_RXRERR		__REG(EMAC_PHYS + 0x6c)
#define EMAC_RXOVERR		__REG(EMAC_PHYS + 0x70)
#define EMAC_RXSYMERR		__REG(EMAC_PHYS + 0x74)
#define EMAC_LENERR		__REG(EMAC_PHYS + 0x78)
#define EMAC_RXJAB		__REG(EMAC_PHYS + 0x7c)
#define EMAC_UNDERFRM		__REG(EMAC_PHYS + 0x80)
#define EMAC_SQERR		__REG(EMAC_PHYS + 0x84)
#define EMAC_RXLEN		__REG(EMAC_PHYS + 0x88)
#define EMAC_TXPAUSEFM		__REG(EMAC_PHYS + 0x8c)
#define EMAC_HASHBOT		__REG(EMAC_PHYS + 0x90)
#define EMAC_HASHTOP		__REG(EMAC_PHYS + 0x94)
#define EMAC_SPECAD1BOT		__REG(EMAC_PHYS + 0x98)
#define EMAC_SPECAD1TOP		__REG(EMAC_PHYS + 0x9c)
#define EMAC_SPECAD2BOT		__REG(EMAC_PHYS + 0xa0)
#define EMAC_SPECAD2TOP		__REG(EMAC_PHYS + 0xa4)
#define EMAC_SPECAD3BOT		__REG(EMAC_PHYS + 0xa8)
#define EMAC_SPECAD3TOP		__REG(EMAC_PHYS + 0xac)
#define EMAC_SPECAD4BOT		__REG(EMAC_PHYS + 0xb0)
#define EMAC_SPECAD4TOP		__REG(EMAC_PHYS + 0xb4)
#define EMAC_IDCHK		__REG(EMAC_PHYS + 0xb8)

 /* -- */

    /* Register values and bits */

#define RCPC_CTRL_UNLOCK	(1<<9)
#define RCPC_CORECONFIG_FASTBUS	(3)
#define RCPC_CORECONFIG_SYNC	(1)
#define RCPC_CORECONFIG_ASYNC	(0)
#define RCPC_PCLKCTRL0_U2	(1<<2)
#define RCPC_PCLKCTRL0_U1	(1<<1)
#define RCPC_PCLKCTRL0_U0	(1<<0)

#if defined (CONFIG_FREQ_51_51)
		// run the CPU at 50.8032 MHz, and the bus at 50.8032 MHz
# define RCPC_SYSPLLCNTL_V	(0x3049) /* 101.6064 MHz <= 11.2896 MHz * 9 */
# define RCPC_SYSCLKPRE_V	(1)		/* HCLK = PLL/2 */
# define RCPC_CPUCLKPRE_V	(1)		/* FCLK = PLL/2 */
# define RCPC_CORECONFIG_V	RCPC_CORECONFIG_ASYNC
# define HCLK			(50803200)	/* HCLK in Hz */

#endif

#if defined (CONFIG_FREQ_76_51)
		// run the CPU at 76.2048 MHz, and the bus at 50.8032 MHz
# define RCPC_SYSPLLCNTL_V	(0x305b) /* 304.8192 MHz <= 11.2896 MHz * 27 */
# define RCPC_SYSCLKPRE_V	(3)		/* HCLK = PLL/6 */
# define RCPC_CPUCLKPRE_V	(2)		/* FCLK = PLL/4 */
# define RCPC_CORECONFIG_V	RCPC_CORECONFIG_ASYNC
# define HCLK			(50803200)	/* HCLK in Hz */

#endif

#define RCPC_AHBCLKCTRL_V ((1<<4)|(1<<3)|(1<<2)) /* !LCD,!USB,!ETH,SDRAM,DMA */
#define RCPC_PCLKCTRL0_V  ((0<<9)|(1<<2)|(1<<1)|(1<<0)) /* RTC,!U2,!U1,!U0 */
#define RCPC_PCLKCTRL1_V  ((1<<3)|(1<<2)|(1<<1)|(1<<0)) /*!USB,!ADC,!SSP,!LCD*/
#define RCPC_PCLKSEL0_V	  ((3<<7))			/* RTC 32KHz */

#define SDRAM_BANK0_PHYS	0x20000000
#define SDRAM_BANK1_PHYS	0x30000000
#define SDRAM_BANK_SIZE		0x10000000

#define EMC_READCONFIG_CLKOUTDELAY (0<<0)	/* Undocumented */
#define EMC_READCONFIG_CMDDELAY	   (1<<0)	/* Undocumented */
#define EMC_READCONFIG_CMDDELAY_P1 (2<<0)	/* Undocumented */
#define EMC_READCONFIG_CMDDELAY_P2 (3<<0)	/* Undocumented */

#define IOCON_MUXCTL5_V		(0x0a00)	/* Enable UART0 */
#define IOCON_MUXCTL6_V		(0x000a)	/* Enable UART0 */
#define IOCON_MUXCTL7_V		(0x5555)	/* Enable A23-A19 */
#define IOCON_RESCTL7_V		(0x5555)	/* Pull-down */

#if defined (CONFIG_ARCH_LH79524)
#define IOCON_MUXCTL10_V	(0x5555)	/* Enable D25-D21, D15-D13 */
#define IOCON_MUXCTL11_V	(0x5555)	/* Enable D20-D17, D12-D9 */
#define IOCON_MUXCTL12_V	(0x5000)	/* Enable D16, D8 */
#define IOCON_MUXCTL14_V	(0x0000)	/* Normalize nCS0 */
#define IOCON_MUXCTL19_V	(0x0441)	/* Enable D31-D29 */
#define IOCON_MUXCTL20_V	(0x1110)	/* Enable D28-D26 */
#endif

#if defined (CONFIG_ARCH_LH79525)
#define IOCON_MUXCTL10_V	(0x0444)	/* Enable D15-D13 */
#define IOCON_MUXCTL11_V	(0x4441)	/* Enable D12-D9 */
#define IOCON_MUXCTL12_V	(0x1000)	/* Enable D8 */
#define IOCON_MUXCTL14_V	(0x0000)	/* Normalize nCS0 */
#define IOCON_MUXCTL19_V	(0x0000)
#define IOCON_MUXCTL20_V	(0x0000)
#endif

//#define NS_TO_HCLK(ns)	(((ns)*((HCLK + 999)/1000) + 999999)/1000000)
#define NS_TO_HCLK(ns)	(((ns)*((HCLK + 500)/1000) + 500000)/1000000)
//#define NS_TO_HCLK(ns)	(((ns)*((HCLK)/1000))/1000000)

#define RTC_CR_EN	(1)

#define EMAC_NETCTL_TXHALT	(1<<10)
#define EMAC_NETCTL_STARTTX	(1<<9)
#define EMAC_NETCTL_CLRSTAT	(1<<5)
#define EMAC_NETCTL_MANGEEN	(1<<4)
#define EMAC_NETCTL_TXEN	(1<<3)
#define EMAC_NETCTL_RXEN	(1<<2)

#define EMAC_NETCONFIG_IGNORE	(1<<19)
#define EMAC_NETCONFIG_ENFRM	(1<<18)
#define EMAC_NETCONFIG_DISCARDFCS (1<<17)
#define EMAC_NETCONFIG_LENGTHCHK (1<<16)
#define EMAC_NETCONFIG_RECBYTE	(1<<8) /* Large frames */
#define EMAC_NETCONFIG_CPYFRM	(1<<4) /* Promiscuous mode */
#define EMAC_NETCONFIG_FULLDUPLEX (1<<1) /* Force full-duplex */
#define EMAC_NETCONFIG_100MB	(1<<0) /* Force 100Mb */
#define EMAC_NETCONFIG_DIV32	(2<<10)

#define EMAC_TXSTATUS_TXUNDER	(1<<6)
#define EMAC_TXSTATUS_TXCOMPLETE (1<<5)
#define EMAC_TXSTATUS_BUFEX	(1<<4)
#define EMAC_TXSTATUS_TXGO	(1<<3)
#define EMAC_TXSTATUS_RETRYLIMIT (1<<2)
#define EMAC_TXSTATUS_COLLISION (1<<1)
#define EMAC_TXSTATUS_USEDBIT	(1<<0)

#define EMAC_RXSTATUS_RXCOVERRUN (1<<2)
#define EMAC_RXSTATUS_FRMREC	(1<<1)
#define EMAC_RXSTATUS_BUFNOTAVAIL (1<<0)

#define EMAC_INT_PAUSEZERO	(1<<13)
#define EMAC_INT_PAUSERRX	(1<<12)
#define EMAC_INT_NOTOK		(1<<11)
#define EMAC_INT_RECOVERRUN	(1<<10)
#define EMAC_INT_LINKCHG	(1<<9)
#define EMAC_INT_TXCOMPLETE	(1<<7)
#define EMAC_INT_TXBUFEXH	(1<<6)
#define EMAC_INT_RETRYLIM	(1<<5)
#define EMAC_INT_ETHTXBUFUR	(1<<4)
#define EMAC_INT_TXUSDBITRD	(1<<3)
#define EMAC_INT_RXUSDBITRD	(1<<2)
#define EMAC_INT_RXCOMP		(1<<1)
#define EMAC_INT_MNGFRMSENT	(1<<0)


#endif  /* __LH79524_H__ */
