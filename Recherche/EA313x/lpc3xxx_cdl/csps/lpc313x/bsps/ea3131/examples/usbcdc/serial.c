/***********************************************************************
 * $Id:: serial.c 6288 2011-01-28 10:23:45Z ing02124                     $
 *
 * Project: USB serial driver
 *
 * Description:
 *     A simple USB serial driver.
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

#include "lpc_types.h"
#include "lpc_usb.h"
#include "serial.h"
#include "cdc.h"
#include "lpc313x_uart_driver.h"
#include "lpc313x_intc_driver.h"
#include "lpc313x_cgu_driver.h"

/*----------------------------------------------------------------------------
  Defines for ring buffers
 *---------------------------------------------------------------------------*/
#define SER_BUF_SIZE               (128)               // serial buffer in bytes (power 2)
#define SER_BUF_MASK               (SER_BUF_SIZE-1ul)  // buffer size mask

/* Buffer read / write macros */
#define SER_BUF_RESET(serBuf)      (serBuf.rdIdx = serBuf.wrIdx = 0)
#define SER_BUF_WR(serBuf, dataIn) (serBuf.data[SER_BUF_MASK & serBuf.wrIdx++] = (dataIn))
#define SER_BUF_RD(serBuf)         (serBuf.data[SER_BUF_MASK & serBuf.rdIdx++])
#define SER_BUF_EMPTY(serBuf)      (serBuf.rdIdx == serBuf.wrIdx)
#define SER_BUF_FULL(serBuf)       (serBuf.rdIdx == serBuf.wrIdx+1)
#define SER_BUF_COUNT(serBuf)      (SER_BUF_MASK & (serBuf.wrIdx - serBuf.rdIdx))

// buffer type
typedef struct __SER_BUF_T
{
  unsigned char data[SER_BUF_SIZE];  // data array
  volatile unsigned int wrIdx;       // Write index
  volatile unsigned int rdIdx;       // Read index
  volatile unsigned int full;        // Buffer full flag
  volatile unsigned int empty;       // Buffer empty flag
} SER_BUF_T;

volatile unsigned short  ser_lineState;     // Line status word
SER_BUF_T                ser_out;           // Serial output data buffers
SER_BUF_T                ser_in;            // Serial input data buffer
INT_32                   uart_dev;          // UART instance

/***********************************************************************
 *
 * Function: ser_OpenPort
 *
 * Purpose: Open Serial port
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 ***********************************************************************/
void ser_OpenPort(void)
{
  /* Disable UART Interrupt in INTC */
  int_disable(IRQ_UART);

  /* Disable Clock output */
  cgu_clk_en_dis(CGU_SB_CLOCK_OUT_ID, 0);

  return;
}

/***********************************************************************
 *
 * Function: ser_ClosePort
 *
 * Purpose: Close the Serial port
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 ***********************************************************************/
void ser_ClosePort(void)
{
  /* Close UART instance */
  uart_close(uart_dev);

  /* Disable Clock output */
  cgu_clk_en_dis(CGU_SB_CLOCK_OUT_ID, 0);

  /* Disable UART Interrupt in INTC */
  int_disable(IRQ_UART);

  return;
}

/***********************************************************************
 *
 * Function: ser_TransData
 *
 * Purpose: Serial Data transmit callback function
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 ***********************************************************************/
void ser_TransData(void)
{
  int32_t towrite = 32;
  int rdIdx = ser_out.rdIdx, wrIdx = ser_out.wrIdx;
  volatile int full = ser_out.full, empty = ser_out.empty;
  UART_REGS_T *regptr = (UART_REGS_T *)UART;
  uint32_t lsr_status;

  /* Line Status word */
  lsr_status = regptr->lsr;

  /* If Tx FIFO empty & Data buffer not empty,
       Write into Tx FIFO */
  if (((lsr_status & UART_LSR_THRE) != 0) && (!empty))
  {
    /* Write 32-bytes into FIFO */
    while (towrite--)
    {
      regptr->dll_fifo = (UNS_32) ser_out.data[rdIdx];
      full = 0;
      rdIdx++;
      if (rdIdx > SER_BUF_MASK)
        rdIdx = 0;
      if (rdIdx == wrIdx)
      {
        empty = 1;
        break;
      }
    }
  }

  /* Update index, full & empty flags */
  ser_out.rdIdx = rdIdx;
  ser_out.full = full;
  if (empty)
  {
    /* Disable Tx interrupt, if no more data */
    regptr->dlm_ie &= ~UART_IE_THRE;
    ser_out.empty = 1;
  }

  /* Update LSR word */
  ser_lineState = lsr_status & ((UART_LSR_BI |
                                 UART_LSR_FR |
                                 UART_LSR_PE |
                                 UART_LSR_OE));
}

/***********************************************************************
 *
 * Function: ser_RcvData
 *
 * Purpose: Serial Data receive callback function
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 ***********************************************************************/
void ser_RcvData(void)
{
  int rdIdx = ser_in.rdIdx, wrIdx = ser_in.wrIdx;
  volatile int full = ser_in.full, empty = ser_in.empty;
  UART_REGS_T *regptr = (UART_REGS_T *)UART;
  uint32_t lsr_status;

  /* LSR word */
  lsr_status = regptr->lsr;

  /* Read till receive FIFO is empty */
  while ((!full) && ((lsr_status & UART_LSR_RDR) != 0))
  {
    ser_in.data[wrIdx] = (unsigned char) regptr->dll_fifo;
    empty = 0;
    wrIdx++;
    if (wrIdx > SER_BUF_MASK)
      wrIdx = 0;
    if (wrIdx == rdIdx)
    {
      full = 1;
      break;
    }
    lsr_status = regptr->lsr;
  }
  ser_in.wrIdx = wrIdx;
  ser_in.empty = empty;
  ser_in.full = full;

  /* Update LSR word */
  ser_lineState = lsr_status & ((UART_LSR_BI |
                                 UART_LSR_FR |
                                 UART_LSR_PE |
                                 UART_LSR_OE));
}

/***********************************************************************
 *
 * Function: ser_RcvLnStatus
 *
 * Purpose: Serial Line status callback function
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 ***********************************************************************/
void ser_RcvLnStatus(void)
{
  UART_REGS_T *regptr = (UART_REGS_T *)UART;
  uint32_t lsr_status;

  /* Line Status word */
  lsr_status = regptr->lsr;

  /* Update LSR word */
  ser_lineState = lsr_status & ((UART_LSR_BI |
                                 UART_LSR_FR |
                                 UART_LSR_PE |
                                 UART_LSR_OE));
}

/***********************************************************************
 *
 * Function: ser_InitPort
 *
 * Purpose: Initialize the serial port function
 *
 * Processing:
 *     See function.
 *
 * Parameters: @baudrate: Baud rate
 *             @databits: No. of databits
 *             @parity: Parity
 *             @stopbits: No. of stop bits
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 ***********************************************************************/
void ser_InitPort(unsigned long baudrate, unsigned int  databits,
                  unsigned int  parity,   unsigned int  stopbits)
{
  UART_CONTROL_T uart_cntl;
  UART_CBS_T uart_cbs;

  /* Enable Clock output */
  cgu_clk_en_dis(CGU_SB_CLOCK_OUT_ID, 1);

  /* Open UART instance with specified parameters */
  /* Baud rate */
  uart_cntl.baud_rate = baudrate;

  /* Databits */
  switch (databits)
  {
    case 5:                                            // 5 Data bits
    case 6:                                            // 6 Data bits
    case 7:                                            // 7 Data bits
    case 8:                                            // 8 Data bits
      uart_cntl.databits = databits;
      break;
    default:
      uart_cntl.databits = 8;
      break;
  }

  /* Parity bits */
  switch (parity)
  {
    case 1:                                            // Parity Odd
      uart_cntl.parity = UART_PAR_ODD;
      break;
    case 2:                                            // Parity Even
      uart_cntl.parity = UART_PAR_EVEN;
      break;
    case 3:                                            // Parity Mark
    case 4:                                            // Parity Space
    case 0:                                            // Parity None
    default:
      uart_cntl.parity = UART_PAR_NONE;
      break;
  }

  /* Stop bits */
  switch (stopbits)
  {
    case 1:                                            // 1.5 Stop bits
    case 2:                                            // 2   Stop bits
      uart_cntl.stopbits = 2;
      break;
    case 0:                                            // 1   Stop bit
    default:
      uart_cntl.stopbits = 1;
      break;
  }
  uart_dev = uart_open((void *) UART_BASE, (INT_32) & uart_cntl);
  if (!uart_dev)
  {
    /* Error in opening */
    return;
  }

  /* Reset Input & output buffers */
  SER_BUF_RESET(ser_out);                              // reset out buffer
  SER_BUF_RESET(ser_in);                               // reset in buffer
  ser_out.full = 0;
  ser_out.empty = 1;
  ser_in.full = 0;
  ser_in.empty = 1;

  /* Attach UART callback functions */
  uart_cbs.rxcb = ser_RcvData;
  uart_cbs.txcb = ser_TransData;
  uart_cbs.rxerrcb = ser_RcvLnStatus;
  uart_ioctl(uart_dev, UART_INSTALL_CBS, (INT_32)&uart_cbs);

  /* Enable the UART Interrupt */
  int_enable(IRQ_UART);

  return;
}

/***********************************************************************
 *
 * Function: ser_Read
 *
 * Purpose: Read data from serial port function
 *
 * Processing:
 *     See function.
 *
 * Parameters: @buffer: Pointer to data buffer
 *             @length: Pointer to number of bytes to be read
 *
 * Outputs: None
 *
 * Returns: Actual No. of bytes read
 *
 * Notes: None
 *
 ***********************************************************************/
int ser_Read(char *buffer, const int *length)
{
  int bytesToRead = *length, bytesRead = 0;
  int rdIdx = ser_in.rdIdx;

  /* Return if no data to be read */
  if (bytesToRead == 0)
    return bytesRead;

  /* Read data from buffer */
  while (bytesToRead--)
  {
    /* If buffer is empty, wait */
    while (ser_in.empty);

    /* Read data, store in user buffer */
    *buffer++ = ser_in.data[rdIdx++];

    /* Update read index */
    if (rdIdx > SER_BUF_MASK)
      rdIdx = 0;

    /* Update No. of bytes read */
    bytesRead++;
  }

  /* Update empty, full flags */
  ser_in.rdIdx = rdIdx;
  ser_in.full = 0;
  if (ser_in.rdIdx == ser_in.wrIdx)
  {
    ser_in.empty = 1;
  }

  return (bytesRead);
}

/***********************************************************************
 *
 * Function: ser_Write
 *
 * Purpose: Write data to the from serial port function
 *
 * Processing:
 *     See function.
 *
 * Parameters: @buffer: Pointer to data buffer
 *             @length: Pointer to number of bytes to be written
 *
 * Outputs: None
 *
 * Returns: Actual No. of bytes written
 *
 * Notes: None
 *
 ***********************************************************************/
int ser_Write(const char *buffer, int *length)
{
  int bytesToWrite = *length, bytesWritten = 0;
  UART_REGS_T *regptr = (UART_REGS_T *)UART;
  int rdIdx, wrIdx;

  /* Return if no data to be written */
  if (bytesToWrite == 0)
    return bytesWritten;

  /* Wait till Output buffer if empty */
  while (!ser_out.empty);

  /* If number of bytes to be written > buffer size,
  	 just write buffer size */
  rdIdx = ser_out.rdIdx;
  wrIdx = ser_out.wrIdx;
  bytesToWrite = (bytesToWrite > SER_BUF_SIZE) ? SER_BUF_SIZE : bytesToWrite;

  /* Write data into output buffer */
  while (bytesToWrite--)
  {
    ser_out.data[wrIdx++] = *buffer++;
    if (wrIdx > SER_BUF_MASK)
      wrIdx = 0;
    bytesWritten++;
    if (wrIdx == rdIdx)
    {
      break;
    }
  }

  /* Update empty, full, write index */
  ser_out.empty = 0;
  ser_out.wrIdx = wrIdx;
  if (ser_out.wrIdx == ser_out.rdIdx)
    ser_out.full = 1;

  /* Write 1 character into Tx FIFO to start transfer */
  regptr->dll_fifo = (UNS_32) ser_out.data[rdIdx];
  rdIdx++;
  if (rdIdx > SER_BUF_MASK)
    rdIdx = 0;
  ser_out.rdIdx = rdIdx;
  if (ser_out.rdIdx == ser_out.wrIdx)
  {
    ser_out.full = 0;
    ser_out.empty = 1;
  }
  else
  {
    regptr->dlm_ie |= UART_IE_THRE;
  }

  return (bytesWritten);
}

/***********************************************************************
 *
 * Function: ser_AvailChar
 *
 * Purpose: Check if character(s) are available at the serial interface
 *
 * Processing:
 *     See function.
 *
 * Parameters: @availChar: Pointer to data buffer
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 ***********************************************************************/
void ser_AvailChar(int *availChar)
{
  int wrIdx, rdIdx;

  /* Find bytes available in Serial buffer */
  if (ser_in.empty)
  {
    *availChar = 0;
  }
  else
  {
    wrIdx = ser_in.wrIdx;
    rdIdx = ser_in.rdIdx;
    if (wrIdx >= rdIdx)
    {
      /* Buffer not wrapped */
      *availChar = wrIdx - rdIdx;
    }
    else
    {
      /* Buffer wrapped */
      *availChar = (SER_BUF_SIZE - rdIdx) + wrIdx;
    }
  }
}

/***********************************************************************
 *
 * Function: ser_LineState
 *
 * Purpose: Read the line state of the serial port
 *
 * Processing:
 *     See function.
 *
 * Parameters: @availChar: Pointer to data buffer
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 ***********************************************************************/
void ser_LineState(unsigned short *lineState)
{
  *lineState = ser_lineState;
  ser_lineState = 0;
}

