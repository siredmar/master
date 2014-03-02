/***********************************************************************
* $Id:: isohandler.h 6404 2011-02-07 02:24:41Z ing03005               $
*
* Project: USB audio device ISO handler functions
*
* Description:
*     This file contains the structure definitions and manifest
*     constants for the LPC313X chip family component:
*        ISO endpoint handling for USB Device configuration
*
* Notes:
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
#ifndef _ISOHANDLER_H
#define _ISOHANDLER_H

/* Calculates the size of an array */
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x)       (sizeof(x)/sizeof((x)[0]))
#endif

/* Number of endpoints available for ISO streaming */
#define USB_ISO_EP_CNT      ARRAY_SIZE(iso_ep_list)
#define EP_ADR_LOG_PHY(x)   (((x) & 0x0F) * 2 + (((x) & 0x80) != 0))
#define GET_TD_BUF(phy_epnum,idx)    (&abuff[ep_td_index[phy_epnum].index][idx])
#define GET_FIRST_BUF(phy_epnum)  GET_TD_BUF(phy_epnum, 0)
#define GET_LAST_BUF(phy_epnum) GET_TD_BUF(phy_epnum, USB_ISO_DESC_CNT-1)
#define GET_CURR_BUF(phy_epnum) GET_TD_BUF(phy_epnum, ep_td_index[phy_epnum].curr)
#define SET_NEXT_BUF(phy_epnum) (ep_td_index[phy_epnum].curr = ep_td_index[phy_epnum].curr + 1 >= USB_ISO_DESC_CNT ? 0 : ep_td_index[phy_epnum].curr + 1)
#define USB_ISO_TXTD_TOTAL_SZ  (((USB_ISO_BUF_SZ & 0x7FFF) << 16) | 0x80 | TD_IOC)

#define usb_iso_flush_desc(td)  lpc31xx_flush_dcache_range((void*)td, (void *)((UNS_32)(td) + sizeof(*(td))))
#define usb_iso_inval_desc(td)  lpc31xx_invalidate_dcache_range((void*)td, (void *)((UNS_32)(td) + sizeof(*(td))))
#define usb_iso_inval_buffer(buf) lpc31xx_invalidate_dcache_range((void*)buf, (void *)((UNS_32)(buf) + USB_ISO_BUF_SZ))
#define usb_iso_flush_buffer(buf) lpc31xx_flush_dcache_range((void*)buf, (void *)((UNS_32)(buf) + USB_ISO_BUF_SZ))
/* Internal book keeping structure */
struct iso_ep_marker
{
  int index;
  int curr;
};

/* ISO Descriptors */
struct iso_desc
{
  DTD_T dTD;
  uint8_t buffer[USB_ISO_BUF_SZ];
};
#ifdef __cplusplus
extern "C"
{
#endif
/* Enables/Disables all Input/Output buffers */
void iso_ep_setup(int input, int disable);

/* ISO endpoint event handler */
void iso_ep_event(UNS_32 event);

/* Initialize the ISO handler */
void usb_iso_reset(void);

#ifdef DEBUG
void iso_ep_dump_queue(int epno);
#else
#define iso_ep_dump_queue(x)
#endif

#ifdef __cplusplus
}
#endif
#endif /* _ISOHANDLER_H */
