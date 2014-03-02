/***********************************************************************
 * $Id:: main.c 2414 2009-10-28 23:57:28Z pdurgesh                     $
 *
 * Project: USB display streaming example
 *
 * Description:
 *     A simple USB display streaming example.
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
#include "mscuser.h"
#include "usbcore.h"
#include "usbhw.h"

extern DQH_T *ep_QH;
extern DTD_T *ep_TD;
extern UNS_32 DevStatusFS2HS;

extern void USB_EndPoint0(UNS_32 event);

/* EP1 loopback buffer. */
ALIGNED(CACHE_DLINESIZE) UNS_8  BulkBuf[512]; /* Bulk In/Out Buffer */
DTD_T ALIGNED(CACHE_DLINESIZE) ep2_TD[8];

int ep2_count = 0;

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
 * Function: USB_Prog_EP2_DTD
 *
 * Purpose: Program video frame buffer transfer descriptor.
 *
 * Processing:
 *
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
void USB_Prog_EP2_DTD(void)
{

  int i;
  UNS_32 dtd_phys;
  UNS_32 Edpt = 4; /*index for EP2 OUT */

  /* Zero out the device transfer descriptors */
  memset((void*)&ep2_TD[ 0 ], 0, 8 * sizeof(DTD_T));

  /* Get physical address for TD */
  dtd_phys = lpc31xx_va_to_pa((void *) & ep2_TD[0]);

  /* set each TD to transfer 20KB */
  for (i = 0; i < 8; i++)
  {
    //ep2_TD[i].next_dTD = (UNS_32) & ep2_TD[i + 1];
    ep2_TD[i].next_dTD = (UNS_32)(dtd_phys + ((1 + i) * sizeof(DTD_T)));
    ep2_TD[i].total_bytes = (0x5000 << 16) | TD_IOC | 0x80;
    ep2_TD[i].buffer0 = EA31_LCD_DATA_ADR;
    ep2_TD[i].buffer1 = (EA31_LCD_DATA_ADR + 0x1000);
    ep2_TD[i].buffer2 = (EA31_LCD_DATA_ADR + 0x2000);
    ep2_TD[i].buffer3 = (EA31_LCD_DATA_ADR + 0x3000);
    ep2_TD[i].buffer4 = (EA31_LCD_DATA_ADR + 0x4000);
    ep2_TD[i].reserved = 0;
  }

  /* Since frame buffer is 150K modify last TD to transfer 10K */
  ep2_TD[7].next_dTD = 0x01; /* The next DTD pointer is INVALID */
  ep2_TD[7].total_bytes = (0x2800 << 16) | TD_IOC | 0x80;

  //ep_QH[Edpt].next_dTD = (UNS_32)(&ep2_TD[ 0 ]);
  ep_QH[Edpt].next_dTD = lpc31xx_va_to_pa((void *)(&ep2_TD[0]));
  ep_QH[Edpt].total_bytes &= (~0xC0);

  /* prime the endpoint for read */
  USB_OTG->endptprime |= _BIT(2) ;
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

/***********************************************************************
 *
 * Function: USB_Configure_Event
 *
 * Purpose: USB Configure Event Callback
 *
 * Processing:
 *     Called automatically after active configuration is configured.
 * Now overwrite EP2_out enpoint queue.
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
  USB_Prog_EP2_DTD();
}
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

  UNS_32 len = 0;
  switch (event)
  {
    case USB_EVT_OUT_NAK:
      /* enqueue TD to receive data in loopback buffer */
      USB_ReadReqEP(0x01, BulkBuf, MSC_HS_MAX_PACKET);
      break;
    case USB_EVT_OUT:
      /* receive completed. */
      len = USB_ReadEP(0x01, BulkBuf);
      /* do something with the recived buffer.*/
      //razer_process(BulkBuf, len);
      /* for time being loop it back to host */
      if (len)
      {
        if ((len == 4) && (BulkBuf[0] == 'S') && (BulkBuf[1] == 'Y')
            && (BulkBuf[2] == 'N') && (BulkBuf[3] == 'C'))
        {
          ep2_count = 0;
          ea3131_lcd_set_pos(0, 0);
          USB_Prog_EP2_DTD();
        }
        else
        {
          USB_WriteEP(0x81, BulkBuf, len);
        }
      }
      break;
    case USB_EVT_IN:
      /* transmit complete do any other house keeping here. */
      break;
  }
}
/***********************************************************************
 *
 * Function: USB_EndPoint2
 *
 * Purpose: USB Endpoint 2 Event Callback
 *
 * Processing:
 *     Called automatically on USB Endpoint 2 Event
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
void USB_EndPoint2(UNS_32 event)
{

  switch (event)
  {
    case USB_EVT_OUT_NAK:
      /* should be here for the first time only. */
      break;
    case USB_EVT_OUT:
      ep2_count++;
      /* enqueue next TD to transfer to external SRAM */;
      if (ep2_count == 8)
      {
        ep2_count = 0;
        ea3131_lcd_set_pos(0, 0);
        USB_Prog_EP2_DTD();
      }
      break;
    case USB_EVT_IN:
      /* shouldn't be here */
      break;
  }
}

/***********************************************************************
 *
 * Function: lcd_write_cmd_reg
 *
 * Purpose: Write command and argument or data to a register
 *
 * Processing:
 *     Does nothing
 *
 * Parameters:
 *    [in] cmd_addr - command or register address
 *    [in] data - data to write
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
static void lcd_write_cmd_reg(UNS_16 cmd_addr, UNS_16 data)
{
  EA31_LCD_CMD = cmd_addr;
  EA31_LCD_DATA = data;
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
  INT_32 dev_pwm;
  volatile int i;
  volatile UNS_32* start_adr, *end_adr;
  LPC_USBDRV_INIT_T usb_cb;

  /* Disable interrupts in ARM core */
  disable_irq_fiq();

  /* Setup miscellaneous board functions */
  ea3131_board_init();

  /* Set virtual address of MMU table */
  cp15_set_vmmu_addr((void *)ISROM_MMU_TTB_BASE);

  /* Initialize interrupt system */
  int_initialize(0xFFFFFFFF);

  /* Init LCD panel */
  ea3131_lcd_init(EA_DISP_QVGA_LCD_V2_ID);

  /* Overwrite Vertiacl Scroll Control */
  lcd_write_cmd_reg(0x11, 0x6020);    //vertical scroll control

  /* enable back light */
  dev_pwm = pwm_open((void *)PWM, 0);
  pwm_ioctl(dev_pwm, PWM_ENABLE, 1);
  pwm_ioctl(dev_pwm, PWM_DUTY_R, 0);

  /******************************************************************/
  /* Setup UART for 115.2K, 8 data bits, no parity, 1 stop bit */
  g_uart_id = uart_open((void *) UART_BASE, 0);

  uart_string_write("\r\nLPC313x USB display example>", 0);
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
  usb_cb.USB_P_EP[2] = USB_EndPoint2;
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

  /* fill fram buffer with green color */
  ea3131_lcd_set_pos(0, 0);
  i = 320 * 240;
  while (i--)
  {
    //EA31_LCD_DATA = 0x7800;
    EA31_LCD_DATA = 0x03E0;
    //EA31_LCD_DATA = 0x001f;
  }

  /* wait */
  timer_wait_ms(NULL, 10);

  /* USB Connect */
  USB_Connect(TRUE);

  ch = 0;
  while (ch != 27)
  {
    start_adr = end_adr = 0;

    if (uart_read(g_uart_id, &ch, 1) > 0)
    {
      uart_string_write("\r\n", 0);
      switch (ch)
      {
        case 'c':
          str_makedec((UNS_8*)buff, (UNS_32)ep2_count);
          uart_string_write(buff, 4);
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
          start_adr = (volatile UNS_32*) & ep_QH[0];
          end_adr = (volatile UNS_32*) & ep_QH[EP_NUM_MAX];
          break;
        case 't':
        case 'T':
          /* dump usb queue */
          start_adr = (volatile UNS_32*) & ep_TD[0];
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
