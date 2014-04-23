/* uart.h

   written by Marc Singer
   17 Sep 2008

   Copyright (C) 2008 Marc Singer

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__UART_H__)
#    define   __UART_H__

/* ----- Includes */

#include "mach/hardware.h"

/* ----- Macros */

#define UART_PHYS       UART0_PHYS		/* Console UART */

#define UART_DR		__REG(UART_PHYS + 0x00) /* Data Register */
#define UART_DLL	__REG(UART_PHYS + 0x00) /* Divisor Latch Low */
#define UART_DLH	__REG(UART_PHYS + 0x04) /* Divisor Latch High */
#define UART_IER	__REG(UART_PHYS + 0x04)	/* Interrupt enable */
#define UART_FCR	__REG(UART_PHYS + 0x08)	/* FIFO control */
#define UART_IIR	__REG(UART_PHYS + 0x08)	/* Interrupt identity control */
#define UART_LCR	__REG(UART_PHYS + 0x0c)	/* Line control */
#define UART_MCR	__REG(UART_PHYS + 0x10)	/* Modem control */
#define UART_LSR	__REG(UART_PHYS + 0x14)	/* Line status */

#define UART_IER_RX	(1<<0)
#define UART_IER_TXE	(1<<1)

#define UART_LCR_DLAB	(1<<7)
#define UART_LCR_WLS_8	(0x3<<0)
#define UART_LCR_STB_1	(0<<2)

#define UART_FCR_DMA	(1<<3)
#define UART_FCR_RSTTF	(1<<2)
#define UART_FCR_RSTRF	(1<<1)
#define UART_FCR_FIFOEN	(1<<0)

#define UART_MCR_RTS	(1<<1)

#define UART_LSR_TEMT	(1<<6)
#define UART_LSR_TXD	(1<<5) /* Transmit FIFO not full */
#define UART_LSR_FE	(1<<3)
#define UART_LSR_PE	(1<<2)
#define UART_LSR_OE	(1<<1)
#define UART_LSR_RXD	(1<<0) /* Receive bytes available */


#define INITIALIZE_CONSOLE_UART ({\
  static const int baudrate = 115200;\
  static const int tclk = TCLK_166;\
  static const int divisor = (tclk/baudrate)>>4;\
  UART_LCR = UART_LCR_WLS_8 | UART_LCR_STB_1 | UART_LCR_DLAB;\
  UART_DLL =  divisor       & 0xff;\
  UART_DLH = (divisor >> 8) & 0xff;\
  UART_LCR = UART_LCR_WLS_8 | UART_LCR_STB_1;\
  UART_FCR = UART_FCR_FIFOEN;\
  UART_MCR = UART_MCR_RTS; })


/* ----- Globals */

/* ----- Prototypes */



#endif  /* __UART_H__ */
