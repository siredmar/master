/***********************************************************************
 * $Id:: main.c 6157 2011-01-12 05:33:34Z ing02124                     $
 *
 * Project: USB Mass Storage example using SD/MMC card
 *
 * Description:
 *     A simple USB Mass Storage Example using SD/MMC card as storage.
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
#include "lpc313x_mci_driver.h"
#include "lpc31xx_vmem_driver.h"
#include "msc.h"
#include "mscuser.h"
#include "usbcore.h"
#include "usbhw.h"
#include "integer.h"
#include "diskio.h"

extern void USB_EndPoint0(UNS_32 event);
extern DRESULT disk_ioctl(
    BYTE drv,	BYTE ctrl, void *buf);
extern DSTATUS disk_initialize(UNS_8 drv);

static INT_32 g_uart_id;
char buff[512];

/* SD/MMC card info */
DWORD SD_BlockSize = 0;
DWORD SD_BlockCount = 0;
DWORD SD_DeviceSize = 0;

/* Dummy function for FATFS */
DWORD get_fattime(void)
{
  DWORD tmr = 0;
  return tmr;
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
  LPC_USBDRV_INIT_T usb_cb;
  DSTATUS status;

  /* Disable interrupts in ARM core */
  disable_irq_fiq();

  /* Setup SD/MMC card detection */
  mci_init();

  /* Set virtual address of MMU table */
  cp15_set_vmmu_addr((void *)ISROM_MMU_TTB_BASE);

  /* Initialize interrupt system */
  int_initialize(0xFFFFFFFF);

  /******************************************************************/
  /* Setup UART for 115.2K, 8 data bits, no parity, 1 stop bit */
  g_uart_id = uart_open((void *) UART_BASE, 0);

  uart_string_write("LPC313x USB Mass Stoarge using SD/MMC example: \r\n", 0);

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

  /* Enable IRQ interrupts in the ARM core */
  enable_irq();

  /* Initliase SD controller */
  if ((status = disk_initialize(0)) & STA_NODISK)
  {
    uart_string_write("Insert SD/MMC card \r\n", 0);
    /* Could not initialize disk */
    return -1;
  }

  /* Get Sector Size */
  status = disk_ioctl(0, GET_SECTOR_SIZE, &SD_BlockSize);
  if (status != RES_OK)
  {
    uart_string_write("Can not read SD/MMC Block Size \r\n", 0);
    /* Could not initialize disk */
    return -1;
  }

  /* Get Sector Count */
  status = disk_ioctl(0, GET_SECTOR_COUNT, &SD_BlockCount);
  if (status != RES_OK)
  {
    uart_string_write("Can not read SD/MMC Block Count \r\n", 0);
    /* Could not initialize disk */
    return -1;
  }

  /* Print SD Card Info */
  uart_string_write("----------------------------- \r\n", 0);
  uart_string_write("SD/MMC Card Information: \r\n", 0);
  SD_DeviceSize = SD_BlockCount / (1024 * 1024);
  SD_DeviceSize = SD_DeviceSize * SD_BlockSize;
  sprintf(buff, "Block size: %lu Bytes \r\n", SD_BlockSize);
  uart_string_write(buff, 0);
  sprintf(buff, "Number of Blocks: %lu \r\n", SD_BlockCount);
  uart_string_write(buff, 0);
  sprintf(buff, "Device Size: %lu MB \r\n", SD_DeviceSize);
  uart_string_write(buff, 0);
  uart_string_write("----------------------------- \r\n", 0);

  /* Check if MSC Block Size & SD Block size match */
  if (SD_BlockSize != MSC_BlockSize)
  {
    uart_string_write("Block Sizes Don't MATCH! \r\n", 0);
    sprintf(buff, "SD Block Size: %lu MSC Block Size : %d \r\n",
            SD_BlockSize, MSC_BlockSize);
    uart_string_write(buff, 0);
    return -1;
  }

  /* wait */
  timer_wait_ms(NULL, 10);

  /* USB Connect */
  USB_Connect(TRUE);

  /* Call USB ISR in loop */
  while (1)
  {
    USB_ISR();
  }
}
