/***********************************************************************
 * $Id:: usb_test.c 7601 2011-06-16 20:27:09Z usb10131                 $
 *
 * Project: USB display/camera/FPGA streaming Test
 *
 * Description:
 *     A simple USB display/camera/FPGA streaming test.
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
#include "lpc313x_sysreg.h"
#include "lpc313x_usbd_driver.h"
#include "lpc31xx_vmem_driver.h"
#include "mscuser.h"
#include "usbcore.h"
#include "usbhw.h"

//#define USB_DEST_ADDR     EXT_SRAM0_0_BASE
//#define USB_DEST_ADDR     EXT_SRAM0_0_BASE
//#define USB_DEST_ADDR     EA31_LCD_DATA_ADR
//#define USB_DEST_ADDR     0x11032000


#define EP2_OUT_ADR 0x02
#define EP2_IN_ADR  0x82

extern DQH_T* ep_QH;
extern DTD_T* ep_TD;
extern UNS_32 DevStatusFS2HS;

extern void USB_EndPoint0(UNS_32 event);

extern void uart_string_write(char *pbuff, UNS_32 len);
extern INT_32 g_uart_id;

/* debug string buffer. */
static UNS_8 buff[512];

DTD_T ALIGNED(CACHE_DLINESIZE) virt_ep2_out_TD[8];
DTD_T ALIGNED(CACHE_DLINESIZE) virt_ep2_in_TD[8];
DTD_T ALIGNED(CACHE_DLINESIZE) virt_ep1_out_TD[8];
DTD_T ALIGNED(CACHE_DLINESIZE) virt_ep1_in_TD[8];

int ep2_out_count = 0;
int ep2_in_count = 0;
int ep1_out_count = 0;
int ep1_in_count = 0;

/***********************************************************************
 *
 * Function: USB_Prog_EP_out_DTD
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
void USB_Prog_EP_out_DTD(UNS_32 dest_addr, UNS_32 ep_idx)
{
  int i;
  UNS_32 dtd_phys;
  DTD_T* pDTD = (ep_idx == 4)?&virt_ep2_out_TD[0]: &virt_ep1_out_TD[0];


  /* Zero out the device transfer descriptors */
  memset((void*)pDTD, 0, 8 * sizeof(DTD_T));

  /* Get physical address for TD */
  dtd_phys = lpc31xx_va_to_pa((void *)pDTD);

  /* set each TD to transfer 20KB */
  for (i = 0; i < 8; i++)
  {
    pDTD[i].next_dTD = (UNS_32)(dtd_phys + ((1 + i) * sizeof(DTD_T)));
    pDTD[i].total_bytes = (0x5000 << 16) | TD_IOC | 0x80;
    pDTD[i].buffer0 = dest_addr;
    pDTD[i].buffer1 = (dest_addr + 0x1000);
    pDTD[i].buffer2 = (dest_addr + 0x2000);
    pDTD[i].buffer3 = (dest_addr + 0x3000);
    pDTD[i].buffer4 = (dest_addr + 0x4000);
    pDTD[i].reserved = 0;
  }

  
  pDTD[7].next_dTD = 0x01; /* The next DTD pointer is INVALID */


  ep_QH[ep_idx].next_dTD = dtd_phys;
  ep_QH[ep_idx].total_bytes &= (~0xC0);

  /* prime the endpoint for read */
  USB_OTG->endptprime |= _BIT((ep_idx >> 1)) ;
}
/***********************************************************************
 *
 * Function: USB_Prog_EP_in_DTD
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
void USB_Prog_EP_in_DTD(UNS_32 dest_addr, UNS_32 ep_idx)
{
  int i;
  UNS_32 dtd_phys;
  DTD_T* pDTD = (ep_idx == 5)?&virt_ep2_in_TD[0]: &virt_ep1_in_TD[0];

  /* Zero out the device transfer descriptors */
  memset((void*)pDTD, 0, 8 * sizeof(DTD_T));

  /* Get physical address for TD */
  dtd_phys = lpc31xx_va_to_pa((void *)pDTD);

  /* set each TD to transfer 20KB */
  for (i = 0; i < 8; i++)
  {
    pDTD[i].next_dTD = (UNS_32)(dtd_phys + ((1 + i) * sizeof(DTD_T)));
    pDTD[i].total_bytes = (0x5000 << 16) | TD_IOC | 0x80;
    pDTD[i].buffer0 = dest_addr;
    pDTD[i].buffer1 = (dest_addr + 0x1000);
    pDTD[i].buffer2 = (dest_addr + 0x2000);
    pDTD[i].buffer3 = (dest_addr + 0x3000);
    pDTD[i].buffer4 = (dest_addr + 0x4000);
    pDTD[i].reserved = 0;
  }

  pDTD[7].next_dTD = 0x01; /* The next DTD pointer is INVALID */

  ep_QH[ep_idx].next_dTD = dtd_phys;
  ep_QH[ep_idx].total_bytes &= (~0xC0);

  /* prime the endpoint for read */
  USB_OTG->endptprime |= _BIT((0x10 + (ep_idx >> 1)));
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
  USB_Prog_EP_out_DTD(EXT_SRAM0_0_BASE, 4);
  USB_Prog_EP_in_DTD(EXT_SRAM0_0_BASE, 5);
  
  USB_Prog_EP_out_DTD(EXT_SRAM0_0_BASE, 2);
  USB_Prog_EP_in_DTD(EXT_SRAM0_0_BASE, 3);
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

  switch (event)
  {
    case USB_EVT_OUT_NAK:
      /* should be here for the first time only. */
      break;
    case USB_EVT_OUT:
      ep1_out_count++;
      /* enqueue next TD to transfer to external SRAM */;
      if (ep1_out_count == 8)
      {
        ep1_out_count = 0;
        USB_Prog_EP_out_DTD(EXT_SRAM0_0_BASE, 2);
      }
      break;
    case USB_EVT_IN:
      /* enqueue next TD to transfer to external SRAM */
      ep1_in_count++;
      if (ep1_in_count == 8)
      {
        ep1_in_count = 0;
        USB_Prog_EP_in_DTD(EXT_SRAM0_0_BASE, 3);
      }
      break;
    case USB_EVT_IN_NAK:
      /* should be here for the first time only. */
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
      ep2_out_count++;
      /* enqueue next TD to transfer to external SRAM */;
      if (ep2_out_count == 8)
      {
        ep2_out_count = 0;
        USB_Prog_EP_out_DTD(EXT_SRAM0_0_BASE, 4);
      }
      break;
    case USB_EVT_IN:
      /* enqueue next TD to transfer to external SRAM */
      ep2_in_count++;
      if (ep2_in_count == 8)
      {
        ep2_in_count = 0;
        USB_Prog_EP_in_DTD(EXT_SRAM0_0_BASE, 5);
      }
      break;
    case USB_EVT_IN_NAK:
      /* should be here for the first time only. */
      break;
  }
}



/***********************************************************************
 *
 * Function: usb_test_start
 *
 * Purpose: USB test function
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
int usb_test_start(void)
{
  UNS_8 ch;
  INT_32 dev_pwm;
  volatile int i;
  volatile UNS_32* start_adr, *end_adr;
  UNS_32 tmp;
  LPC_USBDRV_INIT_T usb_cb;


  /* enable back light */
  dev_pwm = pwm_open((void *)PWM, 0);
  pwm_ioctl(dev_pwm, PWM_ENABLE, 1);
  pwm_ioctl(dev_pwm, PWM_DUTY_R, 0);

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

  /* USB Connect */
  USB_Connect(TRUE);

  /* set USB high priority */
  SYS_REGS->ahb0_extprio = _BIT(3);
  SYS_REGS->ebi_ip2024_1 = 0;
  //SYS_REGS->abc_cfg = (0x5 << 9);

  ch = 0;
  while (ch != 27)
  {
    start_adr = end_adr = 0;

    if (uart_read(g_uart_id, &ch, 1) > 0)
    {
      uart_string_write("\r\n", 0);
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
          tmp = (ch - '0');
          SYS_REGS->abc_cfg = (tmp << 9);
          break;
        case 'c':
          str_makedec((UNS_8*)buff, (UNS_32)ep2_out_count);
          uart_string_write((char *)buff, 4);
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
          uart_string_write((char *)buff, 11);

          str_makehex((UNS_8*)&buff[0], *start_adr++, 8);
          buff[10] = ' ';
          str_makehex((UNS_8*)&buff[11], *start_adr++, 8);
          buff[21] = ' ';
          str_makehex((UNS_8*)&buff[22], *start_adr++, 8);
          buff[32] = ' ';
          str_makehex((UNS_8*)&buff[33], *start_adr++, 8);
          buff[43] = '\r';
          buff[44] = '\n';
          uart_string_write((char *)buff, 45);
        }
      }
      uart_string_write("\r\n>", 0);
    }
  }


  /* Disable timer interrupts in the interrupt controller */
  int_disable(IRQ_USB);

  return 1;
}
