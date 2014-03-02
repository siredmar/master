/***********************************************************************
* $Id:: lpc313x_ipint.h 1529 2009-01-06 02:08:40Z pdurgesh            $
*
* Project: LPC313X PCM/IPINT controller definitions
*
* Description:
*     This file contains the structure definitions and manifest
*     constants for the LPC313X chip family component:
*         Pulse Code Modulation (PCM) & ISDN Oriented Modular (IOM)
*         Interface (IPINT)
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

#ifndef LPC313X_IPINT_H
#define LPC313X_IPINT_H

#include "lpc_types.h"
#include "lpc313x_chip.h"

#ifdef __cplusplus
extern "C"
{
#endif

/***********************************************************************
* PCM/IPINT controller Register Structures
**********************************************************************/

/* PCM/IPINT module registers */
typedef volatile struct
{
  volatile UNS_32 global;
  volatile UNS_32 cntl0;
  volatile UNS_32 cntl1;
  volatile UNS_32 hpout[6];
  volatile UNS_32 hpin[6];
  volatile UNS_32 cntl2;
} IPINT_REGS_T;

/* IPINT global register define */
#define 	IPINT_DMA_Rx_Ena			0x00000010
#define 	IPINT_DMA_Tx_Ena			0x00000008
#define 	IPINT_BusType_Normal		0x00000004
//#define 	IPINT_BusType_16Single		~(IPINT_BusType_Normal)
#define 	IPINT_Start					0x00000001
#define 	IPINT_Stop					~(IPINT_Start)

/* IPINT CNTL0 register define */
#define 	IPINT_Dev_Master			0x00004000
#define 	IPINT_Dev_Slave				~(IPINT_Dev_Master)

#define 	IPINT_DEV_LOOPBACK			0x00000800

#define TM_HW_IPINT_MAX_SLOT	(12)

/* slots_used definition */
#define		IPINT_SLOT_0			0x00000001
#define		IPINT_SLOT_1			0x00000002
#define		IPINT_SLOT_2			0x00000004
#define		IPINT_SLOT_3			0x00000008
#define		IPINT_SLOT_4			0x00000010
#define		IPINT_SLOT_5			0x00000020
#define		IPINT_SLOT_6			0x00000040
#define		IPINT_SLOT_7			0x00000080
#define		IPINT_SLOT_8			0x00000100
#define		IPINT_SLOT_9			0x00000200
#define		IPINT_SLOT_10			0x00000400
#define		IPINT_SLOT_11			0x00000800
#define		IPINT_SLOT_ALL			0x00000FFF

#ifdef __cplusplus
}
#endif

#endif /* LPC313X_IPINT_H */
