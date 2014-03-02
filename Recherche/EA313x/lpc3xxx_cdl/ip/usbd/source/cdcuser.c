/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 *      Name:    cdcuser.c
 *      Purpose: USB Communication Device Class User module
 *      Version: V1.10
 *----------------------------------------------------------------------------
*      This software is supplied "AS IS" without any warranties, express,
 *      implied or statutory, including but not limited to the implied
 *      warranties of fitness for purpose, satisfactory quality and
 *      noninfringement. Keil extends you a royalty-free right to reproduce
 *      and distribute executable files created using this software for use
 *      on NXP Semiconductors LPC microcontroller devices only. Nothing else
 *      gives you the right to use this software.
 *
 * Copyright (c) 2009 Keil - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#include "lpc_types.h"
#include "lpc_usb.h"
#include "usbhw.h"
#include "usbcfg.h"
#include "usbcore.h"
#include "cdc.h"
#include "cdcuser.h"
#include "serial.h"

ALIGNED(32) unsigned char BulkBufIn  [USB_CDC_BUFSIZE];            // Buffer to store USB IN  packet
ALIGNED(32) unsigned char BulkBufOut [USB_CDC_BUFSIZE];            // Buffer to store USB OUT packet
unsigned char NotificationBuf [10];

CDC_LINE_CODING CDC_LineCoding  = {115200, 0, 0, 8};
unsigned short  CDC_SerialState = 0x0000;
unsigned short  CDC_DepInEmpty  = 1;                   // Data IN EP is empty

/*----------------------------------------------------------------------------
  We need a buffer for incomming data on USB port because USB receives
  much faster than  UART transmits
 *---------------------------------------------------------------------------*/
/* Buffer masks */
#define CDC_BUF_SIZE               (64 * 4)             // Output buffer in bytes (power 2)
// large enough for file transfer
#define CDC_BUF_MASK               (CDC_BUF_SIZE-1ul)

/* Buffer read / write macros */
#define CDC_BUF_RESET(cdcBuf)      (cdcBuf.rdIdx = cdcBuf.wrIdx = 0)
#define CDC_BUF_WR(cdcBuf, dataIn) (cdcBuf.data[CDC_BUF_MASK & cdcBuf.wrIdx++] = (dataIn))
#define CDC_BUF_RD(cdcBuf)         (cdcBuf.data[CDC_BUF_MASK & cdcBuf.rdIdx++])
#define CDC_BUF_EMPTY(cdcBuf)      (cdcBuf.rdIdx == cdcBuf.wrIdx)
#define CDC_BUF_FULL(cdcBuf)       (cdcBuf.rdIdx == cdcBuf.wrIdx+1)
#define CDC_BUF_COUNT(cdcBuf)      (CDC_BUF_MASK & (cdcBuf.wrIdx - cdcBuf.rdIdx))

// CDC output buffer
typedef struct __CDC_BUF_T
{
  unsigned char data[CDC_BUF_SIZE];
  volatile unsigned int wrIdx;
  volatile unsigned int rdIdx;
  volatile unsigned int full;
  volatile unsigned int empty;
} CDC_BUF_T;

CDC_BUF_T  CDC_OutBuf;                                 // buffer for all CDC Out data

/*----------------------------------------------------------------------------
  read data from CDC_OutBuf
 *---------------------------------------------------------------------------*/
int CDC_RdOutBuf(char *buffer, const int *length)
{
  int bytesToRead = *length, bytesRead = 0;
  int rdIdx = CDC_OutBuf.rdIdx, wrIdx = CDC_OutBuf.wrIdx;
  int full = CDC_OutBuf.full, empty = CDC_OutBuf.empty;

  if ((empty) || (bytesToRead == 0))
    return bytesRead;

  while (bytesToRead--)
  {
    *buffer++ = CDC_OutBuf.data[rdIdx++];
    full = 0;
    if (rdIdx > CDC_BUF_MASK)
      rdIdx = 0;
    bytesRead++;

    if (rdIdx == wrIdx)
    {
      empty = 1;
      break;
    }
  }

  CDC_OutBuf.empty = empty;
  CDC_OutBuf.rdIdx = rdIdx;
  CDC_OutBuf.full = full;

  return (bytesRead);
}

/*----------------------------------------------------------------------------
  write data to CDC_OutBuf
 *---------------------------------------------------------------------------*/
int CDC_WrOutBuf(const char *buffer, int *length)
{
  int bytesToWrite = *length, bytesWritten = 0;
  int rdIdx = CDC_OutBuf.rdIdx, wrIdx = CDC_OutBuf.wrIdx;
  int full = CDC_OutBuf.full, empty = CDC_OutBuf.empty;

  if ((full) || (bytesToWrite == 0))
    return bytesWritten;

  /* Find free bytes available in Serial buffer */
  while (bytesToWrite--)
  {
    CDC_OutBuf.data[wrIdx++] = *buffer++;
    empty = 0;
    if (wrIdx > CDC_BUF_MASK)
      wrIdx = 0;
    bytesWritten++;

    if (wrIdx == rdIdx)
    {
      full = 1;
      break;
    }
  }

  CDC_OutBuf.empty = empty;
  CDC_OutBuf.wrIdx = wrIdx;
  CDC_OutBuf.full = full;

  return (bytesWritten);
}

/*----------------------------------------------------------------------------
  check if character(s) are available at CDC_OutBuf
 *---------------------------------------------------------------------------*/
int CDC_OutBufAvailChar(int *availChar)
{
  int wrIdx, rdIdx;

  /* Find bytes available in Serial buffer */
  if (CDC_OutBuf.empty)
  {
    *availChar = 0;
  }
  else
  {
    wrIdx = CDC_OutBuf.wrIdx;
    rdIdx = CDC_OutBuf.rdIdx;
    if (wrIdx >= rdIdx)
    {
      /* Buffer not wrapped */
      *availChar = wrIdx - rdIdx;
    }
    else
    {
      /* Buffer wrapped */
      *availChar = (CDC_BUF_SIZE - rdIdx) + wrIdx;
    }
  }

  return (0);
}
/* end Buffer handling */


/*----------------------------------------------------------------------------
  CDC Initialisation
  Initializes the data structures and serial port
  Parameters:   None
  Return Value: None
 *---------------------------------------------------------------------------*/
void CDC_Init(void)
{

  ser_OpenPort();
  ser_InitPort(CDC_LineCoding.dwDTERate,
               CDC_LineCoding.bDataBits,
               CDC_LineCoding.bParityType,
               CDC_LineCoding.bCharFormat);

  CDC_DepInEmpty  = 1;
  CDC_SerialState = CDC_GetSerialState();

  CDC_BUF_RESET(CDC_OutBuf);
  CDC_OutBuf.empty = 1;
  CDC_OutBuf.full = 0;
}


/*----------------------------------------------------------------------------
  CDC SendEncapsulatedCommand Request Callback
  Called automatically on CDC SEND_ENCAPSULATED_COMMAND Request
  Parameters:   None                          (global SetupPacket and EP0Buf)
  Return Value: TRUE - Success, FALSE - Error
 *---------------------------------------------------------------------------*/
uint32_t CDC_SendEncapsulatedCommand(void)
{

  return (TRUE);
}


/*----------------------------------------------------------------------------
  CDC GetEncapsulatedResponse Request Callback
  Called automatically on CDC Get_ENCAPSULATED_RESPONSE Request
  Parameters:   None                          (global SetupPacket and EP0Buf)
  Return Value: TRUE - Success, FALSE - Error
 *---------------------------------------------------------------------------*/
uint32_t CDC_GetEncapsulatedResponse(void)
{

  /* ... add code to handle request */
  return (TRUE);
}


/*----------------------------------------------------------------------------
  CDC SetCommFeature Request Callback
  Called automatically on CDC Set_COMM_FATURE Request
  Parameters:   FeatureSelector
  Return Value: TRUE - Success, FALSE - Error
 *---------------------------------------------------------------------------*/
uint32_t CDC_SetCommFeature(unsigned short wFeatureSelector)
{

  /* ... add code to handle request */
  return (TRUE);
}


/*----------------------------------------------------------------------------
  CDC GetCommFeature Request Callback
  Called automatically on CDC Get_COMM_FATURE Request
  Parameters:   FeatureSelector
  Return Value: TRUE - Success, FALSE - Error
 *---------------------------------------------------------------------------*/
uint32_t CDC_GetCommFeature(unsigned short wFeatureSelector)
{

  /* ... add code to handle request */
  return (TRUE);
}


/*----------------------------------------------------------------------------
  CDC ClearCommFeature Request Callback
  Called automatically on CDC CLEAR_COMM_FATURE Request
  Parameters:   FeatureSelector
  Return Value: TRUE - Success, FALSE - Error
 *---------------------------------------------------------------------------*/
uint32_t CDC_ClearCommFeature(unsigned short wFeatureSelector)
{

  /* ... add code to handle request */
  return (TRUE);
}

/*----------------------------------------------------------------------------
  CDC SetLineCoding Request Callback
  Called automatically on CDC SET_LINE_CODING Request
  Parameters:   none                    (global SetupPacket and EP0Buf)
  Return Value: TRUE - Success, FALSE - Error
 *---------------------------------------------------------------------------*/
uint32_t CDC_SetLineCoding(void)
{

  CDC_LineCoding.dwDTERate   = (EP0Buf[0] <<  0)
                               | (EP0Buf[1] <<  8)
                               | (EP0Buf[2] << 16)
                               | (EP0Buf[3] << 24);
  CDC_LineCoding.bCharFormat =  EP0Buf[4];
  CDC_LineCoding.bParityType =  EP0Buf[5];
  CDC_LineCoding.bDataBits   =  EP0Buf[6];

  ser_ClosePort();
  ser_OpenPort();
  ser_InitPort(CDC_LineCoding.dwDTERate,
               CDC_LineCoding.bDataBits,
               CDC_LineCoding.bParityType,
               CDC_LineCoding.bCharFormat);
  return (TRUE);
}


/*----------------------------------------------------------------------------
  CDC GetLineCoding Request Callback
  Called automatically on CDC GET_LINE_CODING Request
  Parameters:   None                         (global SetupPacket and EP0Buf)
  Return Value: TRUE - Success, FALSE - Error
 *---------------------------------------------------------------------------*/
uint32_t CDC_GetLineCoding(void)
{

  EP0Buf[0] = (CDC_LineCoding.dwDTERate >>  0) & 0xFF;
  EP0Buf[1] = (CDC_LineCoding.dwDTERate >>  8) & 0xFF;
  EP0Buf[2] = (CDC_LineCoding.dwDTERate >> 16) & 0xFF;
  EP0Buf[3] = (CDC_LineCoding.dwDTERate >> 24) & 0xFF;
  EP0Buf[4] =  CDC_LineCoding.bCharFormat;
  EP0Buf[5] =  CDC_LineCoding.bParityType;
  EP0Buf[6] =  CDC_LineCoding.bDataBits;

  return (TRUE);
}


/*----------------------------------------------------------------------------
  CDC SetControlLineState Request Callback
  Called automatically on CDC SET_CONTROL_LINE_STATE Request
  Parameters:   ControlSignalBitmap
  Return Value: TRUE - Success, FALSE - Error
 *---------------------------------------------------------------------------*/
uint32_t CDC_SetControlLineState(unsigned short wControlSignalBitmap)
{

  /* ... add code to handle request */
  return (TRUE);
}


/*----------------------------------------------------------------------------
  CDC SendBreak Request Callback
  Called automatically on CDC Set_COMM_FATURE Request
  Parameters:   0xFFFF  start of Break
                0x0000  stop  of Break
                0x####  Duration of Break
  Return Value: TRUE - Success, FALSE - Error
 *---------------------------------------------------------------------------*/
uint32_t CDC_SendBreak(unsigned short wDurationOfBreak)
{

  /* ... add code to handle request */
  return (TRUE);
}


/*----------------------------------------------------------------------------
  CDC_BulkIn call on DataIn Request
  Parameters:   none
  Return Value: none
 *---------------------------------------------------------------------------*/
void CDC_BulkIn(void)
{
  int numBytesRead, numBytesAvail;

  ser_AvailChar(&numBytesAvail);

  numBytesRead = ser_Read((char *) & BulkBufIn[0], &numBytesAvail);

  // send over USB
  if (numBytesRead > 0)
  {
    USB_WriteEP(CDC_DEP_IN, &BulkBufIn[0], numBytesRead);
  }
  else
  {
    CDC_DepInEmpty = 1;
  }
}

void CDC_BulkOutNak(void)
{
  USB_ReadReqEP(CDC_DEP_OUT, BulkBufOut, USB_CDC_BUFSIZE);
}


/*----------------------------------------------------------------------------
  CDC_BulkOut call on DataOut Request
  Parameters:   none
  Return Value: none
 *---------------------------------------------------------------------------*/
void CDC_BulkOut(void)
{
  int numBytesRead, numBytesWrite;
  int numBytesWritten;

  // get data from USB into intermediate buffer
  numBytesRead = USB_ReadEP(CDC_DEP_OUT, &BulkBufOut[0]);

  // store data in a buffer to transmit it over serial interface
  numBytesWrite = numBytesRead;
  if (numBytesWrite > 0)
  {
    numBytesWritten = CDC_WrOutBuf((char *) & BulkBufOut[0], &numBytesWrite);
  }
}


/*----------------------------------------------------------------------------
  Get the SERIAL_STATE as defined in usbcdc11.pdf, 6.3.5, Table 69.
  Parameters:   none
  Return Value: SerialState as defined in usbcdc11.pdf
 *---------------------------------------------------------------------------*/
unsigned short CDC_GetSerialState(void)
{
  unsigned short temp;

  CDC_SerialState = 0;
  ser_LineState(&temp);

  if (temp & 0x8000)  CDC_SerialState |= CDC_SERIAL_STATE_RX_CARRIER;
  if (temp & 0x2000)  CDC_SerialState |= CDC_SERIAL_STATE_TX_CARRIER;
  if (temp & 0x0010)  CDC_SerialState |= CDC_SERIAL_STATE_BREAK;
  if (temp & 0x4000)  CDC_SerialState |= CDC_SERIAL_STATE_RING;
  if (temp & 0x0008)  CDC_SerialState |= CDC_SERIAL_STATE_FRAMING;
  if (temp & 0x0004)  CDC_SerialState |= CDC_SERIAL_STATE_PARITY;
  if (temp & 0x0002)  CDC_SerialState |= CDC_SERIAL_STATE_OVERRUN;

  return (CDC_SerialState);
}


/*----------------------------------------------------------------------------
  Send the SERIAL_STATE notification as defined in usbcdc11.pdf, 6.3.5.
 *---------------------------------------------------------------------------*/
void CDC_NotificationIn(void)
{

  NotificationBuf[0] = 0xA1;                           // bmRequestType
  NotificationBuf[1] = CDC_NOTIFICATION_SERIAL_STATE;  // bNotification (SERIAL_STATE)
  NotificationBuf[2] = 0x00;                           // wValue
  NotificationBuf[3] = 0x00;
  NotificationBuf[4] = 0x00;                           // wIndex (Interface #, LSB first)
  NotificationBuf[5] = 0x00;
  NotificationBuf[6] = 0x02;                           // wLength (Data length = 2 bytes, LSB first)
  NotificationBuf[7] = 0x00;
  NotificationBuf[8] = (CDC_SerialState >>  0) & 0xFF; // UART State Bitmap (16bits, LSB first)
  NotificationBuf[9] = (CDC_SerialState >>  8) & 0xFF;

  USB_WriteEP(CDC_CEP_IN, &NotificationBuf[0], 10);    // send notification
}
