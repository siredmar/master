/* lh7a40x.h
     $Id$

   written by Marc Singer
   10 Nov 2004

   Copyright (C) 2004 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__LH7A40X_H__)
#    define   __LH7A40X_H__

/* ----- Includes */

#include <asm/reg.h>

/* ----- Prototypes */

/* ----- Macros */

	/* Physical register base addresses */

#define AC97_PHYS	(0x80000000)	/* AC97 Controller */
#define MMC_PHYS	(0x80000100)	/* Multimedia Card Controller */
#define USB_PHYS	(0x80000200)	/* USB Client */
#define SCI_PHYS	(0x80000300)	/* Secure Card Interface */
#define CSC_PHYS	(0x80000400)	/* Clock/State Controller  */
#define INTC_PHYS	(0x80000500)	/* Interrupt Controller */
#define UART1_PHYS	(0x80000600)	/* UART1 Controller */
#define SIR_PHYS	(0x80000600)	/* IR Controller, same are UART1 */
#define UART2_PHYS	(0x80000700)	/* UART2 Controller */
#define UART3_PHYS	(0x80000800)	/* UART3 Controller */
#define DCDC_PHYS	(0x80000900)	/* DC to DC Controller */
#define ACI_PHYS	(0x80000a00)	/* Audio Codec Interface */
#define SSP_PHYS	(0x80000b00)	/* Synchronous ... */
#define TIMER_PHYS	(0x80000c00)	/* Timer Controller */
#define RTC_PHYS	(0x80000d00)	/* Real-time Clock */
#define GPIO_PHYS	(0x80000e00)	/* General Purpose IO */
#define BMI_PHYS	(0x80000f00)	/* Battery Monitor Interface */
#define PWM_PHYS	(0x80001100)	/* Pulse Width Modulator */
#define WDT_PHYS	(0x80001400)	/* Watchdog Timer */
#define SMC_PHYS	(0x80002000)	/* Static Memory Controller */
#define SDRC_PHYS	(0x80002400)	/* SDRAM Controller */
#define DMAC_PHYS	(0x80002800)	/* DMA Controller */
#define CLCDC_PHYS	(0x80003000)	/* Color LCD Controller */

#define UART		(UART2_PHYS)

	/* Physical registers of the LH7A404 */

#define VIC1_PHYS	(0x80008000)	/* Vectored Interrupt Controller 1 */
#define USBH_PHYS	(0x80009000)	/* USB OHCI host controller */
#define VIC2_PHYS	(0x8000a000)	/* Vectored Interrupt Controller 2 */


#define CSC_PWRSR	__REG (CSC_PHYS + 0x00)	/* Power reset */
#define CSC_PWRCNT	__REG (CSC_PHYS + 0x04)	/* Power control */
#define CSC_CLKSET	__REG (CSC_PHYS + 0x20)	/* Clock speed control */

#define CSC_PWRSR_CHIPMAN_SHIFT	(24)
#define CSC_PWRSR_CHIPMAN_MASK	(0xff)
#define CSC_PWRSR_CHIPID_SHIFT	(16)
#define CSC_PWRSR_CHIPID_MASK	(0xff)

  /* CLOCKMODE describes how the system should setup the clocking
     mode.  Simply put, if FCLK and HCLK are the same, the CLOCKMODE
     is FastBus.  If FCLK is an integer multiple of HCLK, the
     CLOCKMODE is synchronous.  If FCLK is not an integer multiple of
     HCLK, the CLOCKMODE is asynchronous.

     Set the CLKCOMODE macro to a letter of either 'f', 'a', or 's'.
  */

      /* CONFIG_FREQ_FCLK_HCLK_PCLK */

#if defined (CONFIG_FREQ_200_100_50)
# define CSC_CLKSET_V		(0x0004ee39)
# define HCLK			(99993600)
# define CLOCKMODE		's'
#endif

#if defined (CONFIG_FREQ_200_66_33)
# define CSC_CLKSET_V		(0x0004ee3a)
# define HCLK			(66662400)
# define CLOCKMODE		's'
#endif

#if defined (CONFIG_FREQ_240_120_60)
/*  PS   PCLKDIV  MAINDIV2  MAINDIV1  PREDIV  HCLKDIV      GCLK		*/
/*   0      0       0x12      0xb       0xe      1      239.616MHz	*/
# define CSC_CLKSET_V		((0x12<<11)|(0xb<<7)|(0xe<<2)|(1<<0))
# define HCLK			(119808000)
# define CLOCKMODE		's'
#endif

#if defined (CONFIG_FREQ_100_100_50)
# define CSC_CLKSET_V		(0x0004eab8)
# define HCLK			(99993600)
# define CLKMODE		'f'
#endif

#if defined (CONFIG_FREQ_150_75_37)
# define CSC_CLKSET_V		(0x00048eb1)
# define HCLK			(75044600)
# define CLKMODE		's'
#endif

#if defined (CONFIG_FREQ_250_125_63)
/*  PS   PCLKDIV  MAINDIV2  MAINDIV1  PREDIV  HCLKDIV      GCLK		*/
/*   0      0       0x15       0xb     0x11      1      249.8991MHz	*/
# define CSC_CLKSET_V		((0x15<<11)|(0xb<<7)|(0x11<<2)|(1<<0))
# define HCLK			(124949557)
# define CLOCKMODE		's'
#endif

#if defined (CONFIG_FREQ_266_133_66)
/*  PS   PCLKDIV  MAINDIV2  MAINDIV1  PREDIV  HCLKDIV      GCLK		*/
/*   0      0       0xa       0x1      0x0      1      265.4218 MHz	*/
# define CSC_CLKSET_V		((0xb<<11)|(0xb<<7)|(0x8<<2)|(1<<0))
# define HCLK			(132710400)
# define CLOCKMODE		's'
#endif


#define CSC_PWRCNT_USBH_EN	(1<<28)	/* USB Host power enable */
#define CSC_PWRCNT_DMAC_M2M1_EN	(1<<27)
#define CSC_PWRCNT_DMAC_M2M0_EN	(1<<26)
#define CSC_PWRCNT_DMAC_M2P8_EN	(1<<25)
#define CSC_PWRCNT_DMAC_M2P9_EN	(1<<24)
#define CSC_PWRCNT_DMAC_M2P6_EN	(1<<23)
#define CSC_PWRCNT_DMAC_M2P7_EN	(1<<22)
#define CSC_PWRCNT_DMAC_M2P4_EN	(1<<21)
#define CSC_PWRCNT_DMAC_M2P5_EN	(1<<20)
#define CSC_PWRCNT_DMAC_M2P2_EN	(1<<19)
#define CSC_PWRCNT_DMAC_M2P3_EN	(1<<18)
#define CSC_PWRCNT_DMAC_M2P0_EN	(1<<17)
#define CSC_PWRCNT_DMAC_M2P1_EN	(1<<16)

#define SMC_BCR0		__REG (SMC_PHYS + 0x00)
#define SMC_BCR1		__REG (SMC_PHYS + 0x04)
#define SMC_BCR2		__REG (SMC_PHYS + 0x08)
#define SMC_BCR3		__REG (SMC_PHYS + 0x0c)
#define SMC_BCR6		__REG (SMC_PHYS + 0x18)
#define SMC_BCR7		__REG (SMC_PHYS + 0x1c)
#define SMC_PCMCIACON		__REG (SMC_PHYS + 0x40)

#define SDRC_GBLCNFG		__REG (SDRC_PHYS + 0x04)
#define SDRC_RFSHTMR		__REG (SDRC_PHYS + 0x08)
#define SDRC_BOOTSTAT		__REG (SDRC_PHYS + 0x0c)
#define SDRC_SDCSC0		__REG (SDRC_PHYS + 0x10)
#define SDRC_SDCSC1		__REG (SDRC_PHYS + 0x14)
#define SDRC_SDCSC2		__REG (SDRC_PHYS + 0x18)
#define SDRC_SDCSC3		__REG (SDRC_PHYS + 0x1c)

#define SDRAM_BANK0_PHYS	0xc0000000
#define SDRAM_BANK1_PHYS	0xd0000000
#define SDRAM_BANK2_PHYS	0xe0000000

#define TIMER1_PHYS		(TIMER_PHYS + 0x00)
#define TIMER2_PHYS		(TIMER_PHYS + 0x20)
#define TIMER3_PHYS		(TIMER_PHYS + 0x80)

#define TIMER_LOAD		(0x00)
#define TIMER_VALUE		(0x04)
#define TIMER_CONTROL		(0x08)

#define CLCDC_TIMING0		__REG (CLCDC_PHYS + 0x00)
#define CLCDC_TIMING1		__REG (CLCDC_PHYS + 0x04)
#define CLCDC_TIMING2		__REG (CLCDC_PHYS + 0x08)
#define CLCDC_TIMING3		__REG (CLCDC_PHYS + 0x0c)
#define CLCDC_UPBASE		__REG (CLCDC_PHYS + 0x10)
#define CLCDC_LPBASE		__REG (CLCDC_PHYS + 0x14)
#define CLCDC_INTREN		__REG (CLCDC_PHYS + 0x18)
#define CLCDC_CTRL		__REG (CLCDC_PHYS + 0x1c)
#define CLCDC_STATUS		__REG (CLCDC_PHYS + 0x20)
#define CLCDC_INTERRUPT		__REG (CLCDC_PHYS + 0x24)
#define CLCDC_UPCURR		__REG (CLCDC_PHYS + 0x28)
#define CLCDC_LPCURR		__REG (CLCDC_PHYS + 0x2c)
#define CLCDC_PALETTE		__REG (CLCDC_PHYS + 0x200)

#define GPIO_PINMUX		__REG (GPIO_PHYS + 0x2c)
#define GPIO_PADD		__REG (GPIO_PHYS + 0x10)
#define GPIO_PAD		__REG (GPIO_PHYS + 0x00)
#define GPIO_PCD		__REG (GPIO_PHYS + 0x08)
#define GPIO_PCDD		__REG (GPIO_PHYS + 0x18)
#define GPIO_PED		__REG (GPIO_PHYS + 0x20)
#define GPIO_PEDD		__REG (GPIO_PHYS + 0x24)
#define GPIO_PFD		__REG (GPIO_PHYS + 0x30)
#define GPIO_PFDD		__REG (GPIO_PHYS + 0x34)
#define GPIO_PGD		__REG (GPIO_PHYS + 0x38)
#define GPIO_PGDD		__REG (GPIO_PHYS + 0x3c)
#define GPIO_PHD		__REG (GPIO_PHYS + 0x40)
#define GPIO_PHDD		__REG (GPIO_PHYS + 0x44)

#define DMAC_GCA		__REG(DMAC_PHYS + 0x2b80)
#define DMAC_GIR		__REG(DMAC_PHYS + 0x2bc0)

#define DMAC_GIR_MMI1		(1<<11)
#define DMAC_GIR_MMI0		(1<<10)
#define DMAC_GIR_MPI8		(1<<9)
#define DMAC_GIR_MPI9		(1<<8)
#define DMAC_GIR_MPI6		(1<<7)
#define DMAC_GIR_MPI7		(1<<6)
#define DMAC_GIR_MPI4		(1<<5)
#define DMAC_GIR_MPI5		(1<<4)
#define DMAC_GIR_MPI2		(1<<3)
#define DMAC_GIR_MPI3		(1<<2)
#define DMAC_GIR_MPI0		(1<<1)
#define DMAC_GIR_MPI1		(1<<0)

#define DMAC_M2P0		0x0000
#define DMAC_M2P1		0x0040
#define DMAC_M2P2		0x0080
#define DMAC_M2P3		0x00c0
#define DMAC_M2P4		0x0240
#define DMAC_M2P5		0x0200
#define DMAC_M2P6		0x02c0
#define DMAC_M2P7		0x0280
#define DMAC_M2P8		0x0340
#define DMAC_M2P9		0x0300
#define DMAC_M2M0		0x0100
#define DMAC_M2M1		0x0140

#define DMAC_P_PCONTROL(c)	__REG(DMAC_PHYS + (c) + 0x00)
#define DMAC_P_PINTERRUPT(c)	__REG(DMAC_PHYS + (c) + 0x04)
#define DMAC_P_PPALLOC(c)	__REG(DMAC_PHYS + (c) + 0x08)
#define DMAC_P_PSTATUS(c)	__REG(DMAC_PHYS + (c) + 0x0c)
#define DMAC_P_REMAIN(c)	__REG(DMAC_PHYS + (c) + 0x14)
#define DMAC_P_MAXCNT0(c)	__REG(DMAC_PHYS + (c) + 0x20)
#define DMAC_P_BASE0(c)		__REG(DMAC_PHYS + (c) + 0x24)
#define DMAC_P_CURRENT0(c)	__REG(DMAC_PHYS + (c) + 0x28)
#define DMAC_P_MAXCNT1(c)	__REG(DMAC_PHYS + (c) + 0x30)
#define DMAC_P_BASE1(c)		__REG(DMAC_PHYS + (c) + 0x34)
#define DMAC_P_CURRENT1(c)	__REG(DMAC_PHYS + (c) + 0x38)

#define DMAC_PCONTROL_ENABLE	(1<<4)

#define DMAC_PORT_USB		0
#define DMAC_PORT_SDMMC		1
#define DMAC_PORT_AC97_1	2
#define DMAC_PORT_AC97_2	3
#define DMAC_PORT_AC97_3	4
#define DMAC_PORT_UART1		6
#define DMAC_PORT_UART2		7
#define DMAC_PORT_UART3		8

#define DMAC_PSTATUS_NEXTBUF	 (1<<6)
#define DMAC_PSTATUS_STALLRINT	 (1<<0)

#define DMAC_INT_CHE		 (1<<3)
#define DMAC_INT_NFB		 (1<<1)
#define DMAC_INT_STALL		 (1<<0)

#define MMC_CLKC		__REG(MMC_PHYS + 0x00)
#define MMC_STATUS		__REG(MMC_PHYS + 0x04)
#define MMC_RATE		__REG(MMC_PHYS + 0x08)
#define MMC_PREDIV		__REG(MMC_PHYS + 0x0c)
#define MMC_SPI			__REG(MMC_PHYS + 0x10) /* LH7A400 only */
#define MMC_CMDCON		__REG(MMC_PHYS + 0x14)
#define MMC_RES_TO		__REG(MMC_PHYS + 0x18)
#define MMC_READ_TO		__REG(MMC_PHYS + 0x1c)
#define MMC_BLK_LEN		__REG(MMC_PHYS + 0x20)
#define MMC_NOB			__REG(MMC_PHYS + 0x24)
#define MMC_INT_STATUS		__REG(MMC_PHYS + 0x28)
#define MMC_EOI			__REG(MMC_PHYS + 0x2c)
#define MMC_INT_MASK		__REG(MMC_PHYS + 0x34)
#define MMC_CMD			__REG(MMC_PHYS + 0x38)
#define MMC_ARGUMENT		__REG(MMC_PHYS + 0x3c)
#define MMC_RES_FIFO		__REG(MMC_PHYS + 0x40)
#define MMC_DATA_FIFO		__REG(MMC_PHYS + 0x48)
#define MMC_BUF_PART_FULL	__REG(MMC_PHYS + 0x4c) /* LH7A400 only */

#define PWM_TC2			__REG(PWM_PHYS + 0x00)
#define PWM_DC2			__REG(PWM_PHYS + 0x04)
#define PWM_EN2			__REG(PWM_PHYS + 0x08)
#define PWM_INV2		__REG(PWM_PHYS + 0x0c)
#define PWM_SYNC2		__REG(PWM_PHYS + 0x10)

#define PWM_TC3			__REG(PWM_PHYS + 0x20)
#define PWM_DC3			__REG(PWM_PHYS + 0x24)
#define PWM_EN3			__REG(PWM_PHYS + 0x28)
#define PWM_INV3		__REG(PWM_PHYS + 0x2c)

#define DCDC_DCDCCON		__REG(DCDC_PHYS + 0x00)
#define DCDC_DCDCFREQ		__REG(DCDC_PHYS + 0x08)

	/* Essential UART constants */

#define UART_DATA_FE		(1<<8)
#define UART_DATA_PE		(1<<9)
#define UART_DATA_DATAMASK	(0xff)

#define UART_STATUS_TXFE	(1<<7)
#define UART_STATUS_RXFF	(1<<6)
#define UART_STATUS_TXFF	(1<<5)
#define UART_STATUS_RXFE	(1<<4)
#define UART_STATUS_BUSY	(1<<3)
#define UART_STATUS_DCD		(1<<2)
#define UART_STATUS_DSR		(1<<1)
#define UART_STATUS_CTS		(1<<0)

#define UART_FCON_WLEN8		(3<<5)
#define UART_FCON_FEN		(1<<4)

#define UART_CON_SIRDIS		(1<<1)
#define UART_CON_ENABLE		(1<<0)

#endif  /* __LH7A40X_H__ */
