/*----------------------------------------------------------------------------
 * U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 * Name:    hiduser.h
 * Purpose: HID Custom User Definitions
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
 * Copyright (c) 2008 Keil - An ARM Company. All rights reserved.
 * Adaption to LPCxxxx, Copyright (c) 2009 NXP.
 *----------------------------------------------------------------------------*/

#ifndef __HIDUSER_H__
#define __HIDUSER_H__

#include "usbdesc.h"

/* HID Number of Reports */
#define HID_REPORT_NUM      1

/* HID In/Out Endpoint Address */
#define HID_EP_IN       EP_ADDR_IN1
#define HID_EP_OUT      EP_ADDR_OUT1

/* HID Global Variables */
extern uint8_t   HID_Protocol;
extern uint8_t   HID_IdleTime[HID_REPORT_NUM];

/* HID Requests Callback Functions */
extern BOOL_32 HID_GetReport   (void);
extern BOOL_32 HID_SetReport   (void);
extern BOOL_32 HID_GetIdle     (void);
extern BOOL_32 HID_SetIdle     (void);
extern BOOL_32 HID_GetProtocol (void);
extern BOOL_32 HID_SetProtocol (void);


#endif  /* __HIDUSER_H__ */
