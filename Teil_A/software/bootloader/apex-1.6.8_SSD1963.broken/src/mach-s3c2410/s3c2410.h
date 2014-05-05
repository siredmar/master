/* s3c2410.h

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   Register definitions for the s3c2410

*/

#if !defined (__S3C2410_H__)
#    define   __S3C2410_H__

/* ----- Includes */

#include <asm/reg.h>


#define SDRAM_BANK0_PHYS	0x30000000

#define S3C24X0_UART_CHANNELS	3
#define S3C24X0_SPI_CHANNELS	2

/* S3C2410 only supports 512 Byte HW ECC */
#define S3C2410_ECCSIZE		512
#define S3C2410_ECCBYTES	3

/* S3C2410 device base addresses */
#define S3C2410_MEMCTL_BASE		0x48000000
#define S3C2410_USB_HOST_BASE		0x49000000
#define S3C2410_INTERRUPT_BASE		0x4A000000
#define S3C2410_DMA_BASE		0x4B000000
#define S3C2410_CLOCK_POWER_BASE	0x4C000000
#define S3C2410_LCD_BASE		0x4D000000
#define S3C2410_NAND_BASE		0x4E000000
#define S3C2410_UART_BASE		0x50000000
#define S3C2410_TIMER_BASE		0x51000000
#define S3C2410_USB_DEVICE_BASE		0x52000140
#define S3C2410_WATCHDOG_BASE		0x53000000
#define S3C2410_I2C_BASE		0x54000000
#define S3C2410_I2S_BASE		0x55000000
#define S3C2410_GPIO_BASE		0x56000000
#define S3C2410_RTC_BASE		0x57000000
#define S3C2410_ADC_BASE		0x58000000
#define S3C2410_SPI_BASE		0x59000000
#define S3C2410_SDI_BASE		0x5A000000
#define S3C2410_GPIO_BASE_H		(S3C2410_GPIO_BASE +0x70)

#define WATCHDOG_CTRL			__REG( S3C2410_WATCHDOG_BASE + 0x00 )
#define INT_CTRL_BASE			__REG( S3C2410_INTERRUPT_BASE + 0x00 )
#define INT_CTRL_MASK			__REG( S3C2410_INTERRUPT_BASE + 0x08 )
#define INT_CTRL_SUBMASK		__REG( S3C2410_INTERRUPT_BASE + 0x1C )
#define CLK_CTRL_BASE			__REG( S3C2410_CLOCK_POWER_BASE + 0x00)
#define CLK_CTRL_DIVN			__REG( S3C2410_CLOCK_POWER_BASE + 0x14)
#define CLK_CTRL_MPLL			__REG( S3C2410_CLOCK_POWER_BASE + 0x04)
#define GPIO_CTRL_BASE_H_CON		__REG( S3C2410_GPIO_BASE_H + 0x00)
#define GPIO_CTRL_BASE_H_UP		__REG( S3C2410_GPIO_BASE_H + 0x08)


#define BWSCON				__REG(S3C2410_MEMCTL_BASE + 0x00)
#define BANKCON0			__REG(S3C2410_MEMCTL_BASE + 0x04)
#define BANKCON1			__REG(S3C2410_MEMCTL_BASE + 0x08)
#define BANKCON2			__REG(S3C2410_MEMCTL_BASE + 0x0C)
#define BANKCON3			__REG(S3C2410_MEMCTL_BASE + 0x10)
#define BANKCON4			__REG(S3C2410_MEMCTL_BASE + 0x14)
#define BANKCON5			__REG(S3C2410_MEMCTL_BASE + 0x18)
#define BANKCON6			__REG(S3C2410_MEMCTL_BASE + 0x1C)
#define BANKCON7			__REG(S3C2410_MEMCTL_BASE + 0x20)
#define REFRESH				__REG(S3C2410_MEMCTL_BASE + 0x24)
#define BANKSIZE			__REG(S3C2410_MEMCTL_BASE + 0x28)
#define MRSRB6				__REG(S3C2410_MEMCTL_BASE + 0x2C)
#define MRSRB7				__REG(S3C2410_MEMCTL_BASE + 0x30)


/* 200.00 MHz */
#define MDIV_200		0x5c
#define PDIV_200		0x4
#define SDIV_200		0x0

#define MDIV_203		0xA1
#define PDIV_203		0x03
#define SDIV_203		0x01


#define MPLLCON_USER	((MDIV_203 << 12) | (PDIV_203 << 4) | (SDIV_203))
#define UART_BAUD_RATE		115200

#define UART_BRD ((50700000 / (UART_BAUD_RATE * 16)) - 1)
#define S3C2410_UART0_BASE		(S3C2410_UART_BASE + 0x00 )
#define UART_BASE			__REG(S3C2410_UART0_BASE + 0x00)
#define UART0_CTRL_UFCON		__REG(S3C2410_UART0_BASE + 0x08)
#define UART0_CTRL_UMCON		__REG(S3C2410_UART0_BASE + 0x0C)
#define UART0_CTRL_ULCON		__REG(S3C2410_UART0_BASE + 0x00)
#define UART0_CTRL_UCON			__REG(S3C2410_UART0_BASE + 0x04)
#define UART0_CTRL_UBRDIV		__REG(S3C2410_UART0_BASE + 0x28)
#define UART0_CTRL_OUTRSTAT		__REG(S3C2410_UART0_BASE + 0x10)
#define UART0_CTRL_UTXHL		__REG(S3C2410_UART0_BASE + 0x20)


#define B6_MT		 	0x3	/* SDRAM */
#define B6_Trcd	 	 	0x1
#define B6_SCAN		 	0x1	/* 9bit */

#define B7_MT		 	0x3	/* SDRAM */
#define B7_Trcd		 	0x1	/* 3clk */
#define B7_SCAN		 	0x1	/* 9bit */

/* REFRESH parameter */
#define REFEN		 	0x1	/* Refresh enable */
#define TREFMD		 	0x0	/* CBR(CAS before RAS)/Auto refresh */
#define Trp		 	0x1	/* 2clk */
#define Trc		 	0x3	/* 7clk */
#define REFCNT		 	1113


#define BANKCON6_VAL	((B6_MT<<15)+(B6_Trcd<<2)+(B6_SCAN))
#define BANKCON7_VAL	((B7_MT<<15)+(B7_Trcd<<2)+(B7_SCAN))
#define REFRESH_VAL	((REFEN<<23)+(TREFMD<<22)+(Trp<<20)+(Trc<<18)+REFCNT)
#define BANKSIZE_VAL	0xB2
#define MRSRB6_VAL	0x30
#define MRSRB7_VAL	0x30

#define UTRSTAT_TX_EMPTY	(1 << 2)
#define UTRSTAT_RX_READY	(1 << 0)
#define UART_ERR_MASK		0xF



/* PWM Timer */
/* Registers */

#define TCFG0                   __REG(0x51000000 + 0x00)
#define TCFG1                   __REG(0x51000000 + 0x04)
#define TCON                    __REG(0x51000000 + 0x08)
#define TCNTB4                  __REG(0x51000000 + 0x3C)
#define TCNTO4                  __REG(0x51000000 + 0x40)


#endif				/* __S3C2410_H__ */
