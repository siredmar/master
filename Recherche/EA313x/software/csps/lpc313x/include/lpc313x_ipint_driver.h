/***********************************************************************
* $Id:: lpc313x_ipint_driver.h 1519 2008-12-17 23:16:59Z pdurgesh     $
*
* Project: LPC313X IPINT controller definitions
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

#ifndef LPC313X_IPINT_DRIVER_H
#define LPC313X_IPINT_DRIVER_H

#include "lpc313x_ipint.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum IpInt_DevType
{
  MASTER = 0,
  SLAVE
} IpInt_DevType_t;

typedef enum IpInt_DevPinType
{
  OPEN_DRAIN 	= 0x00000100,
  PUSH_PULL	= 0x00000600
} IpInt_DevPinType_t;

typedef enum IpInt_DCKFreq
{
  DCK_512 	= 0x00000000,
  DCK_768		= 0x00000008,
  DCK_1536	= 0x00000010,
  DCK_2048	= 0x00000018
} IpInt_DCKFreq_t;

typedef enum IpInt_FSCType
{
  FSC_FR 		= 0x00000000,
  FSC_FF		= 0x00000040,
  FSC_LF		= 0x00000080,
  FSC_LONG	= 0x000000C0
} IpInt_FSCType_t;

enum
{
  IPINT_OK,
  IPINT_ERR_BAD_PARAMETER,
  IPINT_ERR_NOT_INITIALIZED,
  IPINT_ERR_INIT_FAILED,
  IPINT_ERR_RESOURCE_OWNED
};
typedef struct
{
  IpInt_DevType_t 	type;
  IpInt_DevPinType_t 	pin;
  IpInt_FSCType_t 	fsc;
  IpInt_DCKFreq_t 	dck;
  UNS_32 				en_slots_used;
  UNS_32				dir_slots_used;
  UNS_32 				loop_mode;
} IPINT_COMPOSITE_T;
/*------------------------------------------------------------------------------
Exported functions:
------------------------------------------------------------------------------*/

INT_32 ipint_open(void *ipbase, INT_32 arg);

UNS_32 ipint_close(INT_32 devid);

UNS_32 ipint_init(IpInt_DevType_t type, IpInt_DevPinType_t pin, IpInt_FSCType_t fsc,
                  IpInt_DCKFreq_t dck, UNS_32 en_slots_used, UNS_32 dir_slots_used, UNS_32 loop_mode);

UNS_32 ipint_deinit(void);

UNS_32 ipint_hpout(UNS_8 num, UNS_16 data);

UNS_32 ipint_hpin(UNS_8 num, UNS_16 *data);

UNS_32 ipint_start(void);

UNS_32 ipint_stop(void);

#ifdef __cplusplus
}
#endif

#endif /* LPC313X_IPINT_H */
