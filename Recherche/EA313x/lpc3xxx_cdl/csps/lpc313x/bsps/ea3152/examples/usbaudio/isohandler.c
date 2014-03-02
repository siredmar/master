/***********************************************************************
 * $Id:: isohandler.c 6628 2011-02-28 12:01:20Z ing02124                     $
 *
 * Project: USB ISO handling
 *
 * Description:
 *     Handles USB ISO endpoints.
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
#include <string.h>
#include <stdio.h>

#include "lpc313x_usbotg.h"
#include "lpc31xx_vmem_driver.h"
#include "lpc313x_usbd_driver.h"

/* Local dir includes */
#include "usbcfg.h"
#include "isohandler.h"
#include "pcmhandler.h"

#ifdef DEBUG
#include "dbg.h"
#endif

/* Static globals */
static UNS_32 iso_ep_list[] = USB_ISO_EP_LIST;
static struct iso_desc DMA_BUFFER abuff[USB_ISO_EP_CNT][USB_ISO_DESC_CNT];
static struct iso_ep_marker ep_td_index[EP_NUM_MAX];
extern DQH_T* const ep_QH;

#define USB_ISO_RXTD_TOTAL_SZ  (((USB_ISO_BUF_SZ & 0x7FFF) << 16) | 0x80 | TD_IOC)
/***********************************************************************
 *
 * Function: iso_ep_prepTD
 *
 * Purpose: Prepare transfer descritptors for iso in/out xfers
 *
 * Processing:
 *     See function.
 *
 * Parameters: EPnum - Physical endpoint number
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: Since in endpoint is primed prior to getting data from
 *        actual device it will send silence for 3ms.
 *
 **********************************************************************/
static void iso_ep_prepTD(int EPnum)
{
  int i;
  DQH_T *qh = &ep_QH[EPnum];
  UNS_32 mult = (USB_ISO_EP_PKT_SZ + 1024) / 1024;
  DTD_T *prev = &GET_LAST_BUF(EPnum)->dTD;

  /* Prepare all the descriptors */
  for (i = 0; i < USB_ISO_DESC_CNT; i ++)
  {
    struct iso_desc * abf = GET_TD_BUF(EPnum, i);
    DTD_T *td = &abf->dTD;
    uint8_t *buffer = abf->buffer;

    prev->next_dTD = lpc31xx_va_to_pa((void *) td);
    td->buffer0 = lpc31xx_va_to_pa((void *) buffer);
    td->buffer1 = (td->buffer0 + 0x1000) & 0xFFFUL;
    td->buffer2 = td->buffer3 = td->buffer4 = 0;
    td->total_bytes = (EPnum & 1) ?
                      (mult << 10) | USB_ISO_RXTD_TOTAL_SZ : USB_ISO_TXTD_TOTAL_SZ;
    usb_iso_flush_desc(td);
    prev = td;
  }

  /* Initialize the Queue head */
  qh->next_dTD = lpc31xx_va_to_pa((void *) & GET_FIRST_BUF(EPnum)->dTD);
  qh->cap = (mult << 30) | QH_MAXP(USB_ISO_BUF_SZ / mult) | QH_ZLT;
  qh->total_bytes &= ~0xC0;
  usb_iso_flush_desc(qh);
}

/***********************************************************************
 *
 * Function: iso_ep_enable
 *
 * Purpose: Enables a given endpoint
 *
 * Processing:
 *     See function.
 *
 * Parameters: logical_ep - Logical endpoint number
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: The logical endpoint number given as a parameter for this
 *        function must have 7th bit set to indicate input endpoint.
 *
 **********************************************************************/
static void iso_ep_enable(int logical_ep)
{
  const int phy_ep = EP_ADR_LOG_PHY(logical_ep);
  const int lep = logical_ep & 0x3;
  const int input = (logical_ep & 0x80) != 0;

  /* Clear out EndPt type */
  USB_OTG->endptctrl[lep] &=
    ~(input ? EPCTRL_TX_TYPE(3) : EPCTRL_RX_TYPE(3));
  ep_QH[phy_ep].cap = (0x400 << QH_MAX_PKT_LEN_POS) | QH_ZLT;
  USB_OTG->endptctrl[lep] |= input ? EPCTRL_TX_ISO : EPCTRL_RX_ISO;
  iso_ep_prepTD(phy_ep);
  USB_OTG->endptprime = _BIT(USB_EP_BITPOS(logical_ep));
  USB_OTG->endptctrl[lep] |= input ? EPCTRL_TXR : EPCTRL_RXR;
  USB_OTG->endptctrl[lep] |= input ? EPCTRL_TXE : EPCTRL_RXE;
}

/***********************************************************************
 *
 * Function: iso_ep_disable
 *
 * Purpose: Disables a given endpoint
 *
 * Processing:
 *     See function.
 *
 * Parameters: logical_ep - Logical endpoint number
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: The logical endpoint number given as a parameter for this
 *        function must have 7th bit set to indicate input endpoint.
 *
 **********************************************************************/
static void iso_ep_disable(int logical_ep)
{
  const int input = (logical_ep & 0x80) != 0;
  const int bit_pos = _BIT(USB_EP_BITPOS(logical_ep));
  const int lep = logical_ep & 0x3;

  if (!(USB_OTG->endptstatus & bit_pos))
    return;
  do
  {
    USB_OTG->endptflush |= bit_pos;
    while (USB_OTG->endptflush & bit_pos)
      ;
  }
  while (USB_OTG->endptstatus & bit_pos);
  USB_OTG->endptctrl[lep] &= input ? ~EPCTRL_TXE : ~EPCTRL_RXE;
}

/***********************************************************************
 *
 * Function: iso_ep_setup
 *
 * Purpose: Sets up an given endpoint (by priming or depriming it)
 *
 * Processing:
 *     See function.
 *
 * Parameters: input - 0 when input endpoints has to be setup
 *                     !0 when output endpoints to be setup
 *             disable - 0 when endpoints has to be enabled
 *                     !0 when endpoints has to be disabled
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
void iso_ep_setup(int input, int disable)
{
  int i;
  for (i = 0; i < USB_ISO_EP_CNT; i++)
  {
    int cond = !(iso_ep_list[i] & 0x80) ^ input;
    if (cond)
      (!disable ? iso_ep_enable : iso_ep_disable)(iso_ep_list[i]);
  }
}

/***********************************************************************
 *
 * Function: iso_ep_event_in
 *
 * Purpose: iso IN event handler
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
 * Notes: This function imports "pcm_buffer_record" from the pcmhandler.
 *
 **********************************************************************/
static void iso_ep_event_in(void)
{
  const int phy_ep = EP_ADR_LOG_PHY(USB_ADC_IN_EP);
  UNS_32 mult = (USB_ISO_EP_PKT_SZ + 1024) / 1024;
  do
  {
    struct iso_desc *abf = GET_CURR_BUF(phy_ep);
    DTD_T *td = &abf->dTD;
    int sz;

    usb_iso_inval_desc(td);
    if (td->total_bytes & 0x80)
    {
      break;
    }
    sz = USB_ISO_BUF_SZ - ((td->total_bytes >> 16) & 0x7FFF);
    /* Call PCM Record routine to get the recorded data */
    pcm_buffer_record(abf->buffer, sz);
    usb_iso_flush_buffer(abf->buffer);
    td->buffer0 = lpc31xx_va_to_pa(abf->buffer);
    td->buffer1 = (td->buffer0 + 0x1000) & ~0xFFFUL;
    td->total_bytes = (mult << 10) | USB_ISO_RXTD_TOTAL_SZ;
    usb_iso_flush_desc(td);
    SET_NEXT_BUF(phy_ep);
  }
  while (1);
}

/***********************************************************************
 *
 * Function: iso_ep_event_out
 *
 * Purpose: iso OUT event handler
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
 * Notes: This function imports "pcm_buffer_playback" from the pcmhandler.
 *
 **********************************************************************/
static void iso_ep_event_out(void)
{
  const int phy_ep = EP_ADR_LOG_PHY(USB_ADC_OUT_EP);
  do
  {
    struct iso_desc *abf = GET_CURR_BUF(phy_ep);
    DTD_T *td = &abf->dTD;
    int sz;

    usb_iso_inval_desc(td);
    if (td->total_bytes & 0x80) break;
    sz = USB_ISO_BUF_SZ - ((td->total_bytes >> 16) & 0x7FFF);
    usb_iso_inval_buffer(abf->buffer);
    /* Call PCM play routine to play the buffer */
    pcm_buffer_playback(abf->buffer, sz);
    td->buffer0 = lpc31xx_va_to_pa(abf->buffer);
    td->buffer1 = (td->buffer0 + 0x1000) & ~0xFFFUL;
    td->total_bytes = USB_ISO_TXTD_TOTAL_SZ;
    usb_iso_flush_desc(td);
    SET_NEXT_BUF(phy_ep);
  }
  while (1);
}

/***********************************************************************
 *
 * Function: iso_ep_event
 *
 * Purpose: iso IN/OUT event handler
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
 **********************************************************************/
void iso_ep_event(UNS_32 event)
{
  if (event == USB_EVT_OUT)
    iso_ep_event_out();
  else if (event == USB_EVT_IN)
    iso_ep_event_in();
  else
    /* Must never come here */
    return ;
}

/***********************************************************************
 *
 * Function: usb_iso_reset
 *
 * Purpose: Reset index of ISO endpoints
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
 * Notes: Must be called everytime when USB RESET event is received.
 *
 **********************************************************************/
void usb_iso_reset(void)
{
  int i;
  memset(ep_td_index, 0, sizeof(ep_td_index));
  for (i = 0; i < USB_ISO_EP_CNT; i++)
  {
    ep_td_index[EP_ADR_LOG_PHY(iso_ep_list[i])].index = i;
  }
}

#ifdef DEBUG
/***********************************************************************
 *
 * Function: iso_ep_dump_queue
 *
 * Purpose: Debug routine to dump the contents of TD and QH
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
 * Notes: Function available only in debug builds.
 *
 **********************************************************************/
void iso_ep_dump_queue(int epno)
{
  DQH_T *qh = &ep_QH[epno];
  uint32_t *uptr;
  int i, j;

  sprintf(buftmp, "Reg_VAL = 0x%08x\r\n", (unsigned int) USB_OTG->asynclistaddr__endpointlistaddr);
  dbgbuf_puts(buftmp);
  sprintf(buftmp, "Adr QH = 0x%08x\r\n", (unsigned int) ep_QH);
  dbgbuf_puts(buftmp);
  sprintf(buftmp, "Dumping QH[%02d] @0x%08x\r\n", epno, (unsigned int) qh);
  dbgbuf_puts(buftmp);
  for (i = 0, uptr = (uint32_t *)qh; i < sizeof(*ep_QH); i += 4, uptr++)
  {
    sprintf(buftmp, "[%02d] = 0x%08x\r\n", i, (unsigned int)*uptr);
    dbgbuf_puts(buftmp);
  }
  for (j = 0; j < USB_ISO_DESC_CNT; j ++)
  {
    DTD_T * td = &GET_TD_BUF(epno, j)->dTD;
    sprintf(buftmp, "Dumping TD[%02d] @0x%08x\r\n", j, (unsigned int)td);
    dbgbuf_puts(buftmp);
    for (i = 0, uptr = (uint32_t *)td; i < sizeof(DTD_T); i += 4, uptr++)
    {
      sprintf(buftmp, "[%02d] = 0x%08x\r\n", i, (unsigned int)*uptr);
      dbgbuf_puts(buftmp);
    }
  }
}
#endif

