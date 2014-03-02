/*----------------------------------------------------------------------------
 * U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 * Name:    usbcfg.h
 * Purpose: USB Custom Configuration
 * Version: V1.20
 *----------------------------------------------------------------------------
 * This software is supplied "AS IS" without any warranties, express,
 * implied or statutory, including but not limited to the implied
 * warranties of fitness for purpose, satisfactory quality and
 * noninfringement. Keil extends you a royalty-free right to reproduce
 * and distribute executable files created using this software for use
 * on NXP ARM microcontroller devices only. Nothing else gives
 * you the right to use this software.
 *
 * Copyright (c) 2005-2009 Keil Software.
 * Adaption to LPCxxxx, Copyright (c) 2009 NXP.
 *----------------------------------------------------------------------------
 * History:
 *          V1.20 Added vendor specific support
 *          V1.00 Initial Version
 *----------------------------------------------------------------------------*/

#ifndef __USBCFG_H__
#define __USBCFG_H__


//*** <<< Use Configuration Wizard in Context Menu >>> ***


/*
// <h> USB Configuration
//   <o0> USB Power
//        <i> Default Power Setting
//        <0=> Bus-powered
//        <1=> Self-powered
//   <o1> Max Number of Interfaces <1-256>
//   <o2> Max Number of Endpoints  <1-32>
//   <o3> Max Endpoint 0 Packet Size
//        <8=> 8 Bytes <16=> 16 Bytes <32=> 32 Bytes <64=> 64 Bytes
//   <o4> USB device Speed
//        <i> Default Power Setting
//        <0=> High/full Speed
//        <1=> Full Speed only
// </h>
*/

#define USB_POWER           1
#define USB_IF_NUM          1
#define USB_EP_NUM          8
#define USB_MAX_PACKET0     64
#define USB_FULL_SPEED_ONLY 1


/*
// <h> USB Event Handlers
//   <h> Device Events
//     <o0.0> Power Event
//     <o1.0> Reset Event
//     <o2.0> Suspend Event
//     <o3.0> Resume Event
//     <o4.0> Remote Wakeup Event
//     <o5.0> Start of Frame Event
//     <o6.0> Error Event
//   </h>
//   <h> Endpoint Events
//     <o7.0>  Endpoint 0 Event
//     <o7.1>  Endpoint 1 Event
//     <o7.2>  Endpoint 2 Event
//     <o7.3>  Endpoint 3 Event
//     <o7.4>  Endpoint 4 Event
//   </h>
//   <h> USB Core Events
//     <o8.0>  Set Configuration Event
//     <o9.0>  Set Interface Event
//     <o10.0> Set/Clear Feature Event
//   </h>
// </h>
*/

#define USB_POWER_EVENT     0
#define USB_RESET_EVENT     1
#define USB_SUSPEND_EVENT   1
#define USB_RESUME_EVENT    1
#define USB_WAKEUP_EVENT    0
#define USB_SOF_EVENT       0
#define USB_ERROR_EVENT     0
#define USB_EP_EVENT        0x0005
#define USB_CONFIGURE_EVENT 0
#define USB_INTERFACE_EVENT 1
#define USB_FEATURE_EVENT   0


/*
// <e0> USB Class Support
//   <i> enables USB Class specific Requests
//   <e1> Human Interface Device (HID)
//     <o2> Interface Number <0-255>
//   </e>
//   <e3> Mass Storage
//     <o4> Interface Number <0-255>
//   </e>
//   <e5> Audio Device
//     <o6> Control Interface Number <0-255>
//     <o7> Streaming Interface 1 Number <0-255>
//     <o8> Streaming Interface 2 Number <0-255>
//   </e>
//   <e9> Communication Device
//     <o10> Control Interface Number <0-255>
//     <o11> Bulk Interface Number <0-255>
//     <o12> Max Communication Device Buffer Size
//        <8=> 8 Bytes <16=> 16 Bytes <32=> 32 Bytes <64=> 64 Bytes
//   </e>
// </e>
*/

#define USB_CLASS           1
#define USB_HID             0
#define USB_HID_IF_NUM      0
#define USB_MSC             0
#define USB_MSC_IF_NUM      0
#define USB_AUDIO           1
#define USB_ADC_CIF_NUM     0
#define USB_ADC_SIF1_NUM    1
#define USB_ADC_SIF2_NUM    2
#define USB_CDC  			      0
#define USB_CDC_CIF_NUM     0
#define USB_CDC_DIF_NUM     1
#define USB_CDC_BUFSIZE     64

/* USB Audio Device Class end points */
#define USB_ADC_OUT_EP       0x01
#define USB_ADC_IN_EP        0x82

/* ISOCH specific defines */
/** NOTE: BUF_SZ and PKT_SZ should be multiple of 32 */
#define USB_ISO_EP_LIST      {USB_ADC_OUT_EP, USB_ADC_IN_EP}
#define USB_ISO_EP_PKT_SZ    192 /* Max. Pkt. Size of ISO EPs */
#define USB_ISO_BUF_SZ       192 /* Total Buffer Size */
#define USB_ISO_DESC_CNT     3   /* No. of ISO descriptors to use */

/*
// <e0> USB Vendor Support
//   <i> enables USB Vendor specific Requests
// </e>
*/
#define USB_VENDOR          0

#endif  /* __USBCFG_H__ */
