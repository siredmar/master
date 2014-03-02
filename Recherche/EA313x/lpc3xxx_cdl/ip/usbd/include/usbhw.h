/*----------------------------------------------------------------------------
 * U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 * Name:    usbhw.h
 * Purpose: USB Hardware Function prototypes
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
 *----------------------------------------------------------------------------*/
#include "lpc_usb.h"

#ifndef __USBHW_H__
#define __USBHW_H__

/* USB Hardware Functions */
extern void USB_EndPoint0 (uint32_t event);
extern void USB_Connect (BOOL_32 con);
extern void  USB_WakeUpCfg(uint32_t  cfg);
extern void  USB_SetAddress(uint32_t adr);
extern void  USB_Configure(uint32_t  cfg);
extern void  USB_ConfigEP(USB_ENDPOINT_DESCRIPTOR *pEPD);
extern void  USB_DirCtrlEP(uint32_t dir);
extern void  USB_EnableEP(uint32_t EPNum);
extern void  USB_DisableEP(uint32_t EPNum);
extern void  USB_ResetEP(uint32_t EPNum);
extern void  USB_SetStallEP(uint32_t EPNum);
extern void  USB_ClrStallEP(uint32_t EPNum);
extern BOOL_32 USB_SetTestMode(uint8_t mode);
extern uint32_t USB_ReadEP(uint32_t EPNum, uint8_t *pData);
extern uint32_t USB_ReadReqEP(uint32_t EPNum, uint8_t *pData, uint32_t len);
extern uint32_t USB_ReadSetupPkt(uint32_t, uint32_t *);
extern uint32_t USB_WriteEP(uint32_t EPNum, uint8_t *pData, uint32_t cnt);


#endif  /* __USBHW_H__ */
