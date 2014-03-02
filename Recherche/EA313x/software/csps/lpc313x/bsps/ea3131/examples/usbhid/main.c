/***********************************************************************
 * $Id:: main.c 2382 2009-10-27 22:12:38Z pdurgesh                     $
 *
 * Project: Timer driver example
 *
 * Description:
 *     A simple timer driver example.
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
#include "lpc313x_usbotg.h"
#include "lpc313x_usbd_driver.h"
#include "mscuser.h"
#include "usbcore.h"
#include "usbhw.h"


extern DQH_T* const ep_QH;
extern DTD_T* const ep_TD;
extern UNS_32 DevStatusFS2HS;

extern void USB_EndPoint0 (UNS_32 event);


UNS_8 InReport;                                /* HID Input Report    */
                                            /*   Bit0   : Buttons  */
                                            /*   Bit1..7: Reserved */

UNS_8 OutReport;                               /* HID Out Report      */
                                            /*   Bit0..7: LEDs     */



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
static void uart_string_write(INT_32 devid, char *pbuff, UNS_32 len)
{
  UNS_32 written;

  if (devid != 0)
  {
    if ( len == 0)
      len = str_size(pbuff);

    while (len > 0)
    {
      written = uart_write(devid, pbuff, len);
      len -= written;
      pbuff += written;
    }
  }
}


/*------------------------------------------------------------------------------
  Get HID Input Report -> InReport
 *------------------------------------------------------------------------------*/
void GetInReport (void) {

}


/*------------------------------------------------------------------------------
  Set HID Output Report <- OutReport
 *------------------------------------------------------------------------------*/
void SetOutReport (void) {
}

/*
 *  USB Reset Event Callback
 *   Called automatically on USB Reset Event
 */
void USB_Reset_Event(void)
{
  USB_ResetCore();
}
/*
 *  USB Set Configuration Event Callback
 *   Called automatically on USB Set Configuration Request
 */

#if USB_CONFIGURE_EVENT
void USB_Configure_Event (void) {

  if (USB_Configuration) {                   /* Check if USB is configured */
    GetInReport();
    USB_WriteEP(0x81, &InReport, sizeof(InReport));
  }
}
#endif

/*
 *  USB Endpoint 1 Event Callback
 *   Called automatically on USB Endpoint 1 Event
 *    Parameter:       event
 */

void USB_EndPoint1 (UNS_32 event) {

  switch (event) {
    case USB_EVT_IN:
      GetInReport();
      USB_WriteEP(0x81, &InReport, sizeof(InReport));
      break;
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
  INT_32 uartdev;
  char buff[512];
  UNS_8 ch, i;
  volatile UNS_32* start_adr, *end_adr;
  UNS_8 old_OutReport = 0;
  LPC_USBDRV_INIT_T usb_cb;

  /* Disable interrupts in ARM core */
  disable_irq_fiq();

  /* Setup miscellaneous board functions */
  ea3131_board_init();

  /* Set virtual address of MMU table */
  cp15_set_vmmu_addr((void *)ISROM_MMU_TTB_BASE);

  /* Initialize interrupt system */
  int_initialize(0xFFFFFFFF);

  /******************************************************************/
  /* Setup UART for 115.2K, 8 data bits, no parity, 1 stop bit */
  uartdev = uart_open((void *) UART_BASE, 0);
  uart_string_write(uartdev, "\r\nLPC313x USB HID example>", 0);


  /* Install timer interrupts handlers as a IRQ interrupts */
  int_install_irq_handler(IRQ_USB, (PFV) USB_ISR);

  /* initilize call back structures */
  memset((void*)&usb_cb, 0, sizeof(LPC_USBDRV_INIT_T));
  usb_cb.USB_Reset_Event = USB_Reset_Event;
  usb_cb.USB_P_EP[0] = USB_EndPoint0;
  usb_cb.USB_P_EP[1] = USB_EndPoint1;
  usb_cb.ep0_maxp = USB_MAX_PACKET0;

  /* USB Initialization */
  USB_Init(&usb_cb);                               

  /* Enable USB interrupts in the interrupt controller */
  int_enable(IRQ_USB);
  /* Enable IRQ interrupts in the ARM core */
  enable_irq();

  /* USB Connect */
  USB_Connect(TRUE);                        

  ch = 0;
  while (ch != 27)
  {
    start_adr = end_adr = 0;
    if (uart_read(uartdev, &ch, 1) > 0)
    {
      uart_string_write(uartdev, "\r\n", 0);
      switch (ch)
      {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
        InReport |= _BIT((ch - '0'));
        break;
      case '8':
        InReport = 0;
        break;

      case 'f':
      case 'F':
        /* force full-speed mode */
          USB_OTG->portsc1 |= _BIT(24);
          DevStatusFS2HS = FALSE;
        break;
      case 'h':
      case 'H':
        /* remove force full-speed mode */
        USB_OTG->portsc1 &= ~_BIT(24);
        break;

      case 'r':
      case 'R':
        /* dump usb regs */
        start_adr = (volatile UNS_32*)(USBOTG_BASE + 0x100);
        end_adr = (volatile UNS_32*)(USBOTG_BASE + 0x1D0);
        break;
      case 'l':
      case 'L':
        /* dump usb queue */
        start_adr = (volatile UNS_32*)ep_QH;
        end_adr = (volatile UNS_32*)&ep_TD[EP_NUM_MAX];
        break;
      default:
        break;
      }
      if (start_adr != 0)
      {
        while (start_adr < end_adr)
        {
          str_makehex((UNS_8*)buff, (UNS_32)start_adr, 8);
          buff[10] = '\t';
          uart_string_write(uartdev, buff, 11);

          str_makehex((UNS_8*)buff, *start_adr++, 8);
          buff[10] = ' ';
          str_makehex((UNS_8*)&buff[11], *start_adr++, 8);
          buff[21] = ' ';
          str_makehex((UNS_8*)&buff[22], *start_adr++, 8);
          buff[32] = ' ';
          str_makehex((UNS_8*)&buff[33], *start_adr++, 8);
          buff[43] = '\r';
          buff[44] = '\n';
          uart_string_write(uartdev, buff, 45);
        }
      }
      uart_string_write(uartdev, "\r\n>", 0);
    }
    if (OutReport != old_OutReport)
    {
      old_OutReport = OutReport;
      uart_string_write(uartdev, "\r\n outreport:", 0);
      for (i = 0; i < 8; i++)
      {
        if(OutReport & _BIT(i))
        {
          buff[0] = '0' + i;
        }
        else
        {
          buff[0] = '-';
        }
        uart_string_write(uartdev, buff, 1);

      }
      uart_string_write(uartdev, "\r\n", 0);
    }
  }


  /* Disable timer interrupts in the interrupt controller */
  int_disable(IRQ_USB);

  /* Disable interrupts in ARM core */
  disable_irq_fiq();

  return 1;
}
