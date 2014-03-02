/***********************************************************************
 * $Id:: main.c 6288 2011-01-28 10:23:45Z ing02124                     $
 *
 * Project: USB CDC Serial example
 *
 * Description:
 *     A simple USB CDC serial example.
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
#include <stdio.h>
#include <string.h>

#include "lpc_types.h"
#include "lpc_string.h"
#include "lpc_irq_fiq.h"
#include "lpc_arm922t_cp15_driver.h"
#include "ea3131_board.h"
#include "lpc313x_timer_driver.h"
#include "lpc313x_intc_driver.h"
#include "lpc313x_ioconf_driver.h"
#include "lpc313x_uart_driver.h"
#include "lpc313x_pwm_driver.h"
#include "lpc313x_usbotg.h"
#include "lpc313x_usbd_driver.h"
#include "lpc31xx_vmem_driver.h"
#include "cdcuser.h"
#include "serial.h"
#include "usbcore.h"
#include "usbhw.h"

extern void USB_EndPoint0(UNS_32 event);

/***********************************************************************
 *
 * Function: usb_uart_write
 *
 * Purpose: UART string output helper function
 *
 * Processing:
 *     See function.
 *
 * Parameters: @buffer: Pointer to data buffer
 *             @buffer: Number of bytes to be transmitted
 *
 * Outputs: None
 *
 * Returns: Actual No. of bytes transmitted
 *
 * Notes: None
 *
 ***********************************************************************/
static INT_32 usb_uart_write(
  void *buffer,
  INT_32 n_bytes)
{
  INT_32 bwrite = 0;
  UART_REGS_T *pregs = UART;
  UNS_8 *buff8 = (UNS_8 *) buffer;

  /* Only add data if the current FIFO level can be determined */
  if ((pregs->lsr & UART_LSR_TEMT) != 0)
  {
    /* Assuming TX fifo is 32 byte deep. */
    if (n_bytes > 32)
    {
      n_bytes = 32;
    }
    while (n_bytes > 0)
    {
      pregs->dll_fifo = (UNS_32) * buff8;
      buff8++;
      n_bytes--;
      bwrite++;
    }
  }

  return bwrite;
}

/***********************************************************************
 *
 * Function: uart_string_write
 *
 * Purpose: UART string output helper function
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 ***********************************************************************/
void uart_string_write(char *pbuff, UNS_32 len)
{
  UNS_32 written;

  if (len == 0)
    len = str_size(pbuff);

  while (len > 0)
  {
    written = usb_uart_write(pbuff, len);
    len -= written;
    pbuff += written;
  }
}

#if USB_POWER_EVENT
/***********************************************************************
 *
 * Function: USB_Power_Event
 *
 * Purpose: USB Power Event Callback
 *
 * Processing:
 *           Called automatically on USB Power Event
 *
 * Parameters:
 *            @power: On(TRUE)/Off(FALSE)
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 ***********************************************************************/
void USB_Power_Event(uint32_t  power)
{
}
#endif


#if USB_RESET_EVENT
/***********************************************************************
 *
 * Function: USB_Reset_Event
 *
 * Purpose: USB Reset Event Callback
 *
 * Processing:
 *           Called automatically on USB Reset Event
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
void USB_Reset_Event(void)
{
  USB_ResetCore();
}
#endif

#if USB_SUSPEND_EVENT
/***********************************************************************
 *
 * Function: USB_Suspend_Event
 *
 * Purpose: USB Suspend Event Callback
 *
 * Processing:
 *           Called automatically on USB Suspend Event
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
void USB_Suspend_Event(void)
{
}
#endif


#if USB_RESUME_EVENT
/***********************************************************************
 *
 * Function: USB_Resume_Event
 *
 * Purpose: USB Suspend Resume Callback
 *
 * Processing:
 *           Called automatically on USB Resume Event
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
void USB_Resume_Event(void)
{
}
#endif

#if USB_WAKEUP_EVENT
/***********************************************************************
 *
 * Function: USB_Wakeup_Event
 *
 * Purpose: USB Wakeup Callback
 *
 * Processing:
 *           Called automatically on USB Wakeup Event
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
void USB_WakeUp_Event(void)
{
}
#endif

#if USB_SOF_EVENT
/***********************************************************************
 *
 * Function: USB_SOF_Event
 *
 * Purpose: USB Start of Frame Event Callback
 *
 * Processing:
 *           Called automatically on USB SOF Event
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
void USB_SOF_Event(void)
{

}
#endif

#if USB_ERROR_EVENT
/***********************************************************************
 *
 * Function: USB_Error_Event
 *
 * Purpose: USB Error Event Callback
 *
 * Processing:
 *           Called automatically on USB Error Event
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
void USB_Error_Event(uint32_t error)
{
}
#endif

#if USB_CONFIGURE_EVENT
/***********************************************************************
 *
 * Function: USB_Configure_Event
 *
 * Purpose: USB Set Configuration Event Callback
 *
 * Processing:
 *           Called automatically on USB Set Configuration Event
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
void USB_Configure_Event(void)
{
  if (USB_Configuration)                    /* Check if USB is configured */
  {
    /* add your code here */
  }
}
#endif

#if USB_INTERFACE_EVENT
/***********************************************************************
 *
 * Function: USB_Interface_Event
 *
 * Purpose: USB Set Interface Event Callback
 *
 * Processing:
 *           Called automatically on USB Set Interface Event
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
void USB_Interface_Event(void)
{
}
#endif

#if USB_FEATURE_EVENT
/***********************************************************************
 *
 * Function: USB_Feature_Event
 *
 * Purpose: USB Set/Clear Feature Event Callback
 *
 * Processing:
 *           Called automatically on USB Set/Feature Event
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
void USB_Feature_Event(void)
{

}
#endif

/***********************************************************************
 *
 * Function: USB_EndPoint1
 *
 * Purpose: USB EP1 Event Callback
 *
 * Processing:
 *           Called automatically on USB End Point1 Event
 *
 * Parameters:
 *           @event: EP1 Event
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 ***********************************************************************/
void USB_EndPoint1(uint32_t event)
{
  uint16_t temp;
  static uint16_t serialState;

  switch (event)
  {
    case USB_EVT_IN:
      temp = CDC_GetSerialState();
      if (serialState != temp)
      {
        serialState = temp;
        CDC_NotificationIn();            /* send SERIAL_STATE notification */
      }
      break;
  }
}

/***********************************************************************
 *
 * Function: USB_EndPoint2
 *
 * Purpose: USB EP2 Event Callback
 *
 * Processing:
 *           Called automatically on USB End Point2 Event
 *
 * Parameters:
 *           @event: EP1 Event
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 ***********************************************************************/
void USB_EndPoint2(uint32_t event)
{
  event = event;
}

/***********************************************************************
 *
 * Function: USB_EndPoint3
 *
 * Purpose: USB EP3 Event Callback
 *
 * Processing:
 *           Called automatically on USB End Point3 Event
 *
 * Parameters:
 *           @event: EP3 Event
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 ***********************************************************************/
void USB_EndPoint3(uint32_t event)
{
  switch (event)
  {
    case USB_EVT_OUT_NAK:
      CDC_BulkOutNak();
    case USB_EVT_OUT:
      CDC_BulkOut();                 /* data received from Host */
      break;
    case USB_EVT_IN:
      CDC_BulkIn();                  /* data expected from Host */
      break;
  }
}

/***********************************************************************
 *
 * Function: VCOM_Serial2Usb
 *
 * Purpose: Reads character from serial port buffer and
 *          writes to USB buffer
 *
 * Processing: None
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
void VCOM_Serial2Usb(void)
{
  static char serBuf [USB_CDC_BUFSIZE];
  int  numBytesRead, numAvailByte;

  ser_AvailChar(&numAvailByte);
  if (numAvailByte > 0)
  {
    if (CDC_DepInEmpty)
    {
      numBytesRead = ser_Read(&serBuf[0], &numAvailByte);

      CDC_DepInEmpty = 0;
      USB_WriteEP(CDC_DEP_IN, (unsigned char *)&serBuf[0], numBytesRead);
    }
  }

}

/***********************************************************************
 *
 * Function: VCOM_Usb2Serial
 *
 * Purpose: Reads character from USB buffer and writes to
 *          serial port buffer
 *
 * Processing: None
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
void VCOM_Usb2Serial(void)
{
  static char serBuf [USB_CDC_BUFSIZE];
  int  numBytesToRead, numBytesRead, numAvailByte;

  CDC_OutBufAvailChar(&numAvailByte);
  if (numAvailByte > 0)
  {
    numBytesToRead = numAvailByte > 64 ? 64 : numAvailByte;
    numBytesRead = CDC_RdOutBuf(&serBuf[0], &numBytesToRead);
    ser_Write(&serBuf[0], &numBytesRead);
  }

}

/***********************************************************************
 *
 * Function: VCOM_CheckSerialState
 *
 * Purpose: Checks the serial state and initiates notification
 *
 *
 * Processing: None
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
void VCOM_CheckSerialState(void)
{
  unsigned short temp;
  static unsigned short serialState;

  temp = CDC_GetSerialState();
  if (serialState != temp)
  {
    serialState = temp;
    CDC_NotificationIn();                  // send SERIAL_STATE notification
  }
}

/***********************************************************************
 *
 * Function: c_entry
 *
 * Purpose: Application entry point from the startup code
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Always returns 1
 *
 * Notes: None
 *
 **********************************************************************/
int c_entry(void)
{
  LPC_USBDRV_INIT_T usb_cb;

  /* Disable interrupts in ARM core */
  disable_irq_fiq();

  /* Setup miscellaneous board functions */
  ea3131_board_init();

  /* Set virtual address of MMU table */
  cp15_set_vmmu_addr((void *)ISROM_MMU_TTB_BASE);

  /* Initialize interrupt system */
  int_initialize(0xFFFFFFFF);

  /* initilize call back structures */
  memset((void*)&usb_cb, 0, sizeof(LPC_USBDRV_INIT_T));
  usb_cb.USB_Reset_Event = USB_Reset_Event;
  usb_cb.USB_P_EP[0] = USB_EndPoint0;
  usb_cb.USB_P_EP[1] = USB_EndPoint1;
  usb_cb.USB_P_EP[2] = USB_EndPoint2;
  usb_cb.USB_P_EP[3] = USB_EndPoint3;
  usb_cb.USB_virt_to_phys = lpc31xx_va_to_pa;
  usb_cb.USB_cache_flush = lpc31xx_flush_dcache_range;
  usb_cb.USB_cache_invalidate = lpc31xx_invalidate_dcache_range;
  usb_cb.ep0_maxp = USB_MAX_PACKET0;

  /* Init CDC */
  CDC_Init();
  uart_string_write("----------------------- \r\n", 0);
  uart_string_write("USB CDC Serial Example>> \r\n", 0);
  uart_string_write("----------------------- \r\n", 0);

  /* USB Initialization */
  USB_Init(&usb_cb);

  /* Enable IRQ interrupts in the ARM core */
  enable_irq();

  /* wait */
  timer_wait_ms(NULL, 10);

  /* USB Connect */
  USB_Connect(TRUE);

  /* Loop */
  while (1)
  {
    /* USB ISR */
    USB_ISR();

    /* Loopback from Serial to USB */
    VCOM_Serial2Usb();            // read serial port and initiate USB event

    /* Check for serial line status changes */
    VCOM_CheckSerialState();

    /* Loopback from USB to Serial */
    VCOM_Usb2Serial();
  }

  return 1;
}
