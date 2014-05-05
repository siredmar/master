/* sc16c652.h

   written by Marc Singer
   22 Feb 2007

   Copyright (C) 2007 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__SC16C652_H__)
#    define   __SC16C652_H__

/* ----- Includes */

#include <asm/reg.h>

/* ----- Types */

/* ----- Globals */

/* ----- Prototypes */


		/* External DUART SC16C652 */
#define SC_UARTA_PHYS		(0xb4010000) /* DUART port A */
#define SC_UARTB_PHYS		(0xb4010010) /* DUART port B */
#define SC_UART_PHYS		SC_UARTA_PHYS

#define SC_UART_HR		__REG8(SC_UART_PHYS + 0x00)
#define SC_UART_IER		__REG8(SC_UART_PHYS + 0x01)
#define SC_UART_ISR		__REG8(SC_UART_PHYS + 0x02)
#define SC_UART_FCR		__REG8(SC_UART_PHYS + 0x02)
#define SC_UART_LCR		__REG8(SC_UART_PHYS + 0x03)
#define SC_UART_MCR		__REG8(SC_UART_PHYS + 0x04)
#define SC_UART_LSR		__REG8(SC_UART_PHYS + 0x05)
#define SC_UART_MSR		__REG8(SC_UART_PHYS + 0x06)
#define SC_UART_SPR		__REG8(SC_UART_PHYS + 0x07)

#define SC_UART_DLL		__REG8(SC_UART_PHYS + 0x00)
#define SC_UART_DLM		__REG8(SC_UART_PHYS + 0x01)

#define SC_UART_EFR		__REG8(SC_UART_PHYS + 0x02)
#define SC_UART_XON1		__REG8(SC_UART_PHYS + 0x04)
#define SC_UART_XON2		__REG8(SC_UART_PHYS + 0x05)
#define SC_UART_XOFF1		__REG8(SC_UART_PHYS + 0x06)
#define SC_UART_XOFF2		__REG8(SC_UART_PHYS + 0x07)

#define SC_UART_FCR_FEN		(1<<0)
#define SC_UART_FCR_RX_RESET	(1<<1)
#define SC_UART_FCR_TX_RESET	(1<<2)
#define SC_UART_FCR_DMA		(1<<3)

#define SC_UART_LCR_WLEN_SH	(0) /* word length shift */
#define SC_UART_LCR_WLEN_MASK	(0x3 << 0)
#define SC_UART_LCR_WLEN_8	(0x3 << 0)
#define SC_UART_LCR_STOP	(1<<2)
#define SC_UART_LCR_PEN		(1<<3) /* parity enable */
#define SC_UART_LCR_P_EVEN	(1<<4)
#define SC_UART_LCR_P_SET	(1<<5)
#define SC_UART_LCR_BREAK	(1<<6)
#define SC_UART_LCR_DLE		(1<<7) /* divisor latch enable */

#define SC_UART_LSR_RDR		(1<<0) /* receive data ready */
#define SC_UART_LSR_OVR		(1<<1) /* overrun */
#define SC_UART_LSR_PE		(1<<2) /* parity error */
#define SC_UART_LSR_FE		(1<<3) /* framing error */
#define SC_UART_LSR_BI		(1<<4) /* break interrupt */
#define SC_UART_LSR_TXE		(1<<5) /* transmit fifo empty */
#define SC_UART_LSR_TXIDLE	(1<<6) /* transmitter idle */
#define SC_UART_LSR_FIFOE	(1<<7) /* fifo data error */

#define SC_UART_EFR_CTS		(1<<7) /* auto cts */
#define SC_UART_EFR_RTS		(1<<6) /* auto rts */
#define SC_UART_EFR_EFEN	(1<<4) /* enhanced function enable */

//#define SC_UART_CLK		((int) (1.8432*1000*1000))
#define SC_UART_CLK		((int) (14.7456*1000.0*1000.0))

# define UART_DIVISOR (SC_UART_CLK/(115200*16))
//  CPLD_CTRL1_SET = CPLD_CTRL1_XUART_RST;
//  usleep (50);
//  CPLD_CTRL1_CLR = CPLD_CTRL1_XUART_RST;
//  usleep (50);
  //  CPLD_CTRL1_CLR = CPLD_CTRL1_UARTC_EN;
# define INITIALIZE_CONSOLE_UART\
  ({ SC_UART_LCR = SC_UART_LCR_WLEN_8 | SC_UART_LCR_DLE;\
     SC_UART_DLL = UART_DIVISOR & 0xff;\
     SC_UART_DLM = ((UART_DIVISOR >> 8) & 0xff);\
     SC_UART_LCR = SC_UART_LCR_WLEN_8;\
     SC_UART_IER = 0;\
     SC_UART_FCR = SC_UART_FCR_FEN | SC_UART_FCR_RX_RESET\
       | SC_UART_FCR_TX_RESET;\
     SC_UART_LSR = 0; })


#endif  /* __SC16C652_H__ */
