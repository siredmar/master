/***********************************************************************
 * $Id:: main.c 6163 2011-01-12 06:40:20Z ing02124                     $
 *
 * Project: USB Mass Storage example using SDRAM Memory
 *
 * Description:
 *     A simple USB Mass Storage example using SDRAM Memory.
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
#include "ea3152_board.h"
#include "lpc313x_timer_driver.h"
#include "lpc313x_intc_driver.h"
#include "lpc313x_ioconf_driver.h"
#include "lpc313x_uart_driver.h"
#include "lpc313x_usbotg.h"
#include "lpc313x_usbd_driver.h"
#include "lpc31xx_vmem_driver.h"
#include "mscuser.h"
#include "usbcore.h"
#include "usbhw.h"


extern DQH_T *ep_QH;
extern DTD_T *ep_TD;
extern UNS_32 DevStatusFS2HS;

extern void USB_EndPoint0(UNS_32 event);

//UNS_8*  Memory = (UNS_8*)(EXT_SDRAM_BASE + 0x4000);  /* MSC RAM */
UNS_8*  pMemory = (UNS_8*)(EXT_SDRAM_BASE + 0x2000000);  /* MSC RAM */
UNS_8*  Memory;  /* MSC RAM */

static INT_32 g_uart_id;
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

  if (g_uart_id != 0)
  {
    if (len == 0)
      len = str_size(pbuff);

    while (len > 0)
    {
      written = uart_write(g_uart_id, pbuff, len);
      len -= written;
      pbuff += written;
    }
  }
}



/***********************************************************************
 *
 * Function: USB_Reset_Event
 *
 * Purpose: USB Reset Event Callback
 *
 * Processing:
 *     Called automatically on USB Reset Event.
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
void USB_Reset_Event(void)
{
  USB_ResetCore();
}

#if USB_CONFIGURE_EVENT
/***********************************************************************
 *
 * Function: USB_Configure_Event
 *
 * Purpose: USB Configure Event Callback
 *
 * Processing:
 *     Called automatically on USB configure Event.
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
void USB_Configure_Event(void)
{

}
#endif

/***********************************************************************
 *
 * Function: USB_EndPoint1
 *
 * Purpose: USB Endpoint 1 Event Callback
 *
 * Processing:
 *     Called automatically on USB Endpoint 1 Event
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
void USB_EndPoint1(UNS_32 event)
{

  switch (event)
  {
    case USB_EVT_OUT_NAK:
      MSC_BulkOutNak();
      break;
    case USB_EVT_OUT:
      MSC_BulkOut();
      break;
    case USB_EVT_IN:
      MSC_BulkIn();
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
  char buff[512];
  UNS_8 ch;
  volatile UNS_32* start_adr, *end_adr;
  LPC_USBDRV_INIT_T usb_cb;

  /* Disable interrupts in ARM core */
  disable_irq_fiq();

  /* Set virtual address of MMU table */
  cp15_set_vmmu_addr((void *)ISROM_MMU_TTB_BASE);

  /* Initialize interrupt system */
  int_initialize(0xFFFFFFFF);

  /******************************************************************/
  /* Setup UART for 115.2K, 8 data bits, no parity, 1 stop bit */
  g_uart_id = uart_open((void *) UART_BASE, 0);
  uart_string_write("\r\nLPC313x USB MEM example>", 0);
  uart_string_write("\r\nBlocks:", 0);
  str_makehex((UNS_8*)buff, (UNS_32)MSC_BlockCount, 8);
  uart_string_write(buff, 10);
  uart_string_write("\r\nBlock Len:", 0);
  str_makehex((UNS_8*)buff, (UNS_32)MSC_BlockSize, 8);
  uart_string_write(buff, 10);
  uart_string_write("\r\n>", 0);

  /* Install timer interrupts handlers as a IRQ interrupts */
  int_install_irq_handler(IRQ_USB, (PFV) USB_ISR);

  /* initilize call back structures */
  memset((void*)&usb_cb, 0, sizeof(LPC_USBDRV_INIT_T));
  usb_cb.USB_Reset_Event = USB_Reset_Event;
  usb_cb.USB_P_EP[0] = USB_EndPoint0;
  usb_cb.USB_P_EP[1] = USB_EndPoint1;
  usb_cb.USB_virt_to_phys = lpc31xx_va_to_pa;
  usb_cb.USB_cache_flush = lpc31xx_flush_dcache_range;
  usb_cb.USB_cache_invalidate = lpc31xx_invalidate_dcache_range;
  usb_cb.ep0_maxp = USB_MAX_PACKET0;

  /* USB Initialization */
  USB_Init(&usb_cb);

  /* Enable USB interrupts in the interrupt controller */
  int_enable(IRQ_USB);
  /* Enable IRQ interrupts in the ARM core */
  enable_irq();

  //  memcpy(Memory, DiskImage, MSC_ImageSize);
  Memory = lpc31xx_pa_to_va((UNS_32)pMemory);
  memset((void*)Memory, 0xff, MSC_MemorySize);

  /* wait */
  timer_wait_ms(NULL, 10);

  /* USB Connect */
  USB_Connect(TRUE);

  uart_string_write("\r\nType following commands for test states:\r\n", 0);
  uart_string_write("j - Set in J state\r\n", 0);
  uart_string_write("k - Set in K state\r\n", 0);
  uart_string_write("s - Set in SE0 state\r\n", 0);
  uart_string_write("p - Send test packet\r\n", 0);
  uart_string_write("c - Clear test states\r\n", 0);
  uart_string_write("f - force full-speed\r\n", 0);
  uart_string_write("h - Clear full-speed force\r\n", 0);
  uart_string_write("r - Dump registers\r\n", 0);
  uart_string_write("q - Dump EP queues\r\n", 0);
  uart_string_write("t - Dump transfer descriptors\r\n", 0);
  uart_string_write("\r\n>", 0);

  ch = 0;
  while (ch != 27)
  {
    start_adr = end_adr = 0;

    if (uart_read(g_uart_id, &ch, 1) > 0)
    {
      uart_string_write((char*)&ch, 1);
      uart_string_write("\r\n", 0);
      switch (ch)
      {
        case 'c':
        case 'C':
          /* clear test state*/
          USB_OTG->portsc1 &= ~(0x0F << 16);
          break;
        case 'j':
        case 'J':
          /* J state */
          USB_OTG->portsc1 |= (0x01 << 16);
          break;

        case 'k':
        case 'K':
          /* K state */
          USB_OTG->portsc1 |= (0x02 << 16);
          break;

        case 's':
        case 'S':
          /* s state */
          USB_OTG->portsc1 |= (0x03 << 16);
          break;

        case 'p':
        case 'P':
          /* test packet */
          USB_OTG->portsc1 |= (0x04 << 16);
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
        case 'q':
        case 'Q':
          /* dump usb queue */
          start_adr = (volatile UNS_32*)ep_QH;
          end_adr = (volatile UNS_32*) & ep_QH[EP_NUM_MAX];
          break;
        case 't':
        case 'T':
          /* dump usb queue */
          start_adr = (volatile UNS_32*) ep_TD;
          end_adr = (volatile UNS_32*) & ep_TD[EP_NUM_MAX];
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
          uart_string_write(buff, 11);

          str_makehex((UNS_8*)&buff[0], *start_adr++, 8);
          buff[10] = ' ';
          str_makehex((UNS_8*)&buff[11], *start_adr++, 8);
          buff[21] = ' ';
          str_makehex((UNS_8*)&buff[22], *start_adr++, 8);
          buff[32] = ' ';
          str_makehex((UNS_8*)&buff[33], *start_adr++, 8);
          buff[43] = '\r';
          buff[44] = '\n';
          uart_string_write(buff, 45);
        }
      }
      uart_string_write("\r\n>", 0);
    }
  }


  /* Disable timer interrupts in the interrupt controller */
  int_disable(IRQ_USB);

  /* Disable interrupts in ARM core */
  disable_irq_fiq();

  return 1;
}
