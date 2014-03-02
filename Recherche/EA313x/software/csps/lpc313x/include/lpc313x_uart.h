/***********************************************************************
* $Id:: lpc313x_uart.h 1180 2008-09-11 19:32:40Z pdurgesh             $
*
* Project: LPC313X UART definitions
*
* Description:
*     This file contains the structure definitions and manifest
*     constants for the LPC313X chip family component:
*         Universal Asynchronous Receiver Transmitter (UART)
*
***********************************************************************
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
* NXP Semiconductors assumes no responsibility or liability for the
* use of the software, conveys no license or title under any patent,
* copyright, or mask work right to the product. NXP Semiconductors
* reserves the right to make changes in the software without
* notification. NXP Semiconductors also make no representation or
* warranty that such application will be suitable for the specified
* use without further testing or modification.
**********************************************************************/

#ifndef LPC313X_UART_H
#define LPC313X_UART_H

#include "lpc_types.h"
#include "lpc313x_chip.h"

#ifdef __cplusplus
extern "C"
{
#endif

/***********************************************************************
* Universal Asynchronous Receiver Transmitter (UART) Register Structures
**********************************************************************/

/* UART Module Register Structure */
typedef volatile struct
{
  /* LSB*/
  volatile UNS_32 dll_fifo;     /* UART data FIFO/holding/dll reg */
  volatile UNS_32 dlm_ie;       /* UART high divisor/int enable reg */
  volatile UNS_32 iir_fcr;      /* UART int pending/FIFO contrl reg */
  volatile UNS_32 lcr;         /* Line Control Register */
  volatile UNS_32 mcr;         /* Modem Control Register */
  volatile const UNS_32 lsr;   /* Line Status Register*/
  volatile const UNS_32 msr;   /* Modem Status Register*/
  volatile UNS_32 scr;         /* Scratch Pad Register */
  volatile UNS_32 acr;         /* Auto-baud Control Register */
  volatile UNS_32 icr;         /* Irda Control Register */
  volatile UNS_32 fdr;         /* Fractional Divider Register */
  volatile const UNS_32 unused;/* Unused registers */
  volatile UNS_32 pop;         /* NHP Pop Register */
  volatile UNS_32 mode;        /* NHP Mode Selection Register */
  volatile const UNS_32 unused1[999];/* Unused registers */
  volatile const UNS_32 cfg;   /* Configuration Register */
  volatile UNS_32 intce;       /* Interrupt Clear Enable Register */
  volatile UNS_32 intse;       /* Interrupt Set Enable Register */
  volatile const UNS_32 ints;  /* Interrupt Status Register */
  volatile const UNS_32 inte;  /* Interrupt Enable Register */
  volatile UNS_32 intcs;       /* Interrupt Clear Status Register */
  volatile UNS_32 intss;       /* Interrupt Set Status Register */
  volatile const UNS_32 unused2[3]; /* Unused registers */
  volatile const UNS_32 mid;   /* Module Identification Register */
  /* MSB*/
} UART_REGS_T;

/**********************************************************************
* dll and dlm register definitions
**********************************************************************/
/* Macro for loading most and least significant halfs of divisors */
#define UART_LOAD_DLL(div)          ((div) & 0xFF)
#define UART_LOAD_DLM(div)          (((div) >> 8) & 0xFF)

/**********************************************************************
* ie register definitions
**********************************************************************/
/* Bit for enabling the Auto-baud Time-Out interrupt */
#define UART_IE_ABTO               _BIT(9)
/* Bit for enabling the End of Auto-Baud interrupt */
#define UART_IE_ABEO               _BIT(8)
/* Bit for enabling the cts_ansignal transition interrupt */
#define UART_IE_CTS                _BIT(7)
/* Bit for enabling the Modem Status interrupt */
#define UART_IE_MS                 _BIT(3)
/* Bit for enabling the RX line status interrupt(s) */
#define UART_IE_RXLINE_STS         _BIT(2)
/* Bit for enabling the transmit holding register empty interrupt */
#define UART_IE_THRE               _BIT(1)
/* Bit for enabling the receive data available (RDA) interrupt */
#define UART_IE_RDA                _BIT(0)

/**********************************************************************
* iir register definitions
**********************************************************************/
/* Bit for the Auto-baud Time-Out interrupt status*/
#define UART_IR_INT_ABTO           _BIT(9)
/* Bit for the End of Auto-Baud interrupt status*/
#define UART_IR_INT_ABEO           _BIT(8)
/* Bit for masking interrupt pending status */
#define UART_IIR_INT_PENDING       _BIT(0)
/* Mask for getting interrupt source */
#define UART_IIR_INTSRC_MASK       0xE
/* Interrupt sources */
#define UART_IIR_INTSRC_MS         0x0
#define UART_IIR_INTSRC_THRE       0x2
#define UART_IIR_INTSRC_RDA        0x4
#define UART_IIR_INTSRC_RXLINE     0x6
#define UART_IIR_INTSRC_CTI        0xC /* Character timeout */


/**********************************************************************
* fcr register definitions
**********************************************************************/
/* Receive FIFO trigger level selections */
#define UART_FCR_RXFIFO_TL1        _SBF(6, 0)
#define UART_FCR_RXFIFO_TL16       _SBF(6, 1)
#define UART_FCR_RXFIFO_TL24       _SBF(6, 2)
#define UART_FCR_RXFIFO_TL28       _SBF(6, 3)
/* DMA mode select bit  */
#define UART_FCR_DMA_MODE          _BIT(3)
/* Clear TX FIFO bit */
#define UART_FCR_TXFIFO_FLUSH      _BIT(2)
/* Clear RX FIFO bit */
#define UART_FCR_RXFIFO_FLUSH      _BIT(1)
/* Enable FIFO bit  */
#define UART_FCR_FIFO_EN           _BIT(0)

/**********************************************************************
* lcr register definitions
**********************************************************************/
/* Bit for enabling divisor latch and IER register */
#define UART_LCR_DIVLATCH_EN       _BIT(7)
/* Bit for enabling break transmission (forces TX low) */
#define UART_LCR_BREAK_EN          _BIT(6)
/* Parity selection */
#define UART_LCR_PARITY_ODD        0x0
#define UART_LCR_PARITY_EVEN       _BIT(4)
#define UART_LCR_PARITY_FORCE1     _BIT(5)
#define UART_LCR_PARITY_FORCE0     (_BIT(5) | _BIT(4))
/* Parity selection mask */
#define UART_LCR_PARITY_MASK       (_BIT(5) | _BIT(4))
/* Parity enable bit */
#define UART_LCR_PARITY_ENABLE     _BIT(3)
/* Stop bit selection */
#define UART_LCR_STOP1BIT          0x0
#define UART_LCR_STOP2BITS         _BIT(2)
/* Word length selections */
#define UART_LCR_WLEN_5BITS        _SBF(0, 0)
#define UART_LCR_WLEN_6BITS        _SBF(0, 1)
#define UART_LCR_WLEN_7BITS        _SBF(0, 2)
#define UART_LCR_WLEN_8BITS        _SBF(0, 3)
/* Word length mask */
#define UART_LCR_WLEN_MASK         (_BIT(1) | _BIT(0))

/**********************************************************************
* mcr register definitions
**********************************************************************/
/* Bit for Auto-cts flow control enable */
#define UART_MCR_AUTO_CTS         _BIT(7)
/* Bit for Auto-rts flow control enable */
#define UART_MCR_AUTO_RTS         _BIT(6)
/* Bit for Loop-back mode enable */
#define UART_MCR_LOOP             _BIT(4)
/* Bit for Inverse control for the out2_n output */
#define UART_MCR_OUT2             _BIT(3)
/* Bit for Inverse control for the out1_n output */
#define UART_MCR_OUT1             _BIT(2)
/* Bit for inverse control for the rts_n output */
#define UART_MCR_RTS              _BIT(1)
/* Bit for Inverse control for the Data Terminal Ready output */
#define UART_MCR_DTS              _BIT(0)

/**********************************************************************
* lsr register definitions
**********************************************************************/
/* FIFO RX error status */
#define UART_LSR_FIFORX_ERR        _BIT(7)
/* transmitter empty status */
#define UART_LSR_TEMT              _BIT(6)
/* transmitter Holding Register empty status */
#define UART_LSR_THRE              _BIT(5)
/* break interrupt status */
#define UART_LSR_BI                _BIT(4)
/* framing error status */
#define UART_LSR_FR                _BIT(3)
/* parity error status */
#define UART_LSR_PE                _BIT(2)
/* RX FIFO overrun error status */
#define UART_LSR_OE                _BIT(1)
/* RX FIFO empty status */
#define UART_LSR_RDR               _BIT(0)

/**********************************************************************
* FDR register definitions
**********************************************************************/
/* Macro for loading most and least significant halfs of divisors */
#define UART_FDR_MUL_GET(div)          (((div) >> 4) & 0xF)
#define UART_FDR_DIVADD_GET(div)       ((div) & 0xF)
#define UART_FDR_MUL_SET(div)          _SBF(4, ((div) & 0xF))
#define UART_FDR_DIVADD_SET(div)       ((div) & 0xF)

/**********************************************************************
* Macro to access UART registers
**********************************************************************/
#define UART    ((UART_REGS_T*)UART_BASE)

#ifdef __cplusplus
}
#endif

#endif /* LPC313X_UART_H */
