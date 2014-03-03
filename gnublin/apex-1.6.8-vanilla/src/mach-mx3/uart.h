/* uart.h

   written by Marc Singer
   28 Feb 2007

   Copyright (C) 2007 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__UART_H__)
#    define   __UART_H__

/* ----- Includes */

#include "mach/hardware.h"

/* ----- Types */

/* ----- Globals */

/* ----- Prototypes */

#if defined (CONFIG_MX31_UART1)
# define UART PHYS_UART1
//# define UARTB PHYS_UART2
//# define UARTB PHYS_UART3
#endif

#define UART_RXD		0x00
#define UART_TXD		0x40
#define UART_CR1		0x80
#define UART_CR2		0x84
#define UART_CR3		0x88
#define UART_CR4		0x8c
#define UART_FCR		0x90
#define UART_SR1		0x94
#define UART_SR2		0x98
#define UART_ESC		0x9c
#define UART_TIM		0xa0
#define UART_BIR		0xa4
#define UART_BMR		0xa8
#define UART_BCR		0xac
#define UART_ONEMS		0xb0
#define UART_TEST		0xb4

#define UART_RXD_ERR		(1<<14)
#define UART_RXD_OVRRUN		(1<<13)
#define UART_RXD_FRMERR		(1<<12)
#define UART_RXD_BRK		(1<<11)
#define UART_RXD_PRERR		(1<<10)

#define UART_CR1_EN		(1<<0)		/* UART Enable */
#define UART_CR2_IRTS		(1<<14)		/* Ignore RTS */
#define UART_CR2_CTSC		(1<<13)		/* Receiver controlled CTS */
#define UART_CR2_WS		(1<<5)		/* Word size 8 */
#define UART_CR2_TXEN		(1<<2)		/* Transmitter enable */
#define UART_CR2_RXEN		(1<<1)		/* Receiver enable */
#define UART_CR2_NSRST		(1<<0)		/* No software reset */
#define UART_CR3_RXDMUXSEL	(1<<2)		/* Enable MUX pins */
#define UART_CR4_LPBYP		(1<<4)		/* Low-power bypass */
#define UART_CR4_DREN		(1<<0)		/* Data ready intr. enable */
#define UART_CR4_CTSTL_SH	(10)

#define UART_FCR_RXTL_SH	(0)
#define UART_FCR_RFDIV_SH	(7)
#define UART_FCR_TXTL_SH	(10)
#define UART_FCR_DTE		(1<<6)		/* Select DTE mode */

#define UART_SR1_TRDY		(1<<13)		/* Transmit ready */
#define UART_SR1_RRDY		(1<<9)		/* Receiver ready */
#define UART_SR2_TXFE		(1<<14)		/* Transmit fifo empty */
#define UART_SR2_TXDC		(1<<3)		/* Transmission complete */
#define UART_SR2_RDR		(1<<0)		/* Receive Data Ready */

#define UART_BMR_115200		(5968 - 1)
#define UART_BIR_115200		(1000 - 0)

#if defined (CPLD_CTRL1_CLR)
# define INITIALIZE_CONSOLE_UART_MX31ADS \
     CPLD_CTRL1_CLR = 1<<4; CPLD_CTRL2_SET = 1<<2;
#else
# define INITIALIZE_CONSOLE_UART_MX31ADS
#endif

# define INITIALIZE_CONSOLE_UART\
  ({ MASK_AND_SET (__REG (0x43fac080), 0xffff, 0x1210); /* txd1/rxd1 */\
     __REG (UART + UART_CR1) = 0;\
     __REG (UART + UART_CR2) = UART_CR2_IRTS | UART_CR2_CTSC | UART_CR2_WS\
			     | UART_CR2_TXEN | UART_CR2_RXEN;\
     __REG (UART + UART_CR3) = UART_CR3_RXDMUXSEL;\
     __REG (UART + UART_CR4) = (32<<UART_CR4_CTSTL_SH)\
			     | UART_CR4_LPBYP | UART_CR4_DREN;\
     __REG (UART + UART_FCR) = (16<<UART_FCR_RXTL_SH)\
			     | ( 0<<UART_FCR_RFDIV_SH)\
			     | (16<<UART_FCR_TXTL_SH);\
     __REG (UART + UART_SR1) = ~0;\
     __REG (UART + UART_SR2) = ~0;\
     __REG (UART + UART_BIR) = UART_BIR_115200;\
     __REG (UART + UART_BMR) = UART_BMR_115200;\
     __REG (UART + UART_CR1) = UART_CR1_EN;\
     INITIALIZE_CONSOLE_UART_MX31ADS;\
   })

# define INITIALIZE_UARTB\
  ({ __REG (UARTB + UART_CR1) = 0;\
     __REG (UARTB + UART_CR2) = UART_CR2_IRTS | UART_CR2_CTSC | UART_CR2_WS\
			     | UART_CR2_TXEN | UART_CR2_RXEN;\
     __REG (UARTB + UART_CR3) = UART_CR3_RXDMUXSEL;\
     __REG (UARTB + UART_CR4) = (32<<UART_CR4_CTSTL_SH)\
			     | UART_CR4_LPBYP | UART_CR4_DREN;\
     __REG (UARTB + UART_FCR) = (16<<UART_FCR_RXTL_SH)\
			     | ( 0<<UART_FCR_RFDIV_SH)\
			     | (16<<UART_FCR_TXTL_SH);\
     __REG (UARTB + UART_SR1) = ~0;\
     __REG (UARTB + UART_SR2) = ~0;\
     __REG (UARTB + UART_BIR) = UART_BIR_115200;\
     __REG (UARTB + UART_BMR) = UART_BMR_115200;\
     __REG (UARTB + UART_CR1) = UART_CR1_EN;\
   })

#endif  /* __UART_H__ */
