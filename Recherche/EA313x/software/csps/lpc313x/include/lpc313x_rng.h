/***********************************************************************
* $Id:: lpc313x_rng.h 1911 2009-06-16 00:31:46Z pdurgesh              $
*
* Project: LPC313X Random Number Generator (RNG) definitions
*
* Description:
*     This file contains the structure definitions and manifest
*     constants for the LPC313X chip family component:
*         Random Number Generator (RNG)
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

#ifndef LPC313X_RNG_H
#define LPC313X_RNG_H

#include "lpc_types.h"
#include "lpc313x_chip.h"

#ifdef __cplusplus
extern "C"
{
#endif

/***********************************************************************
* Random Number Generator (RNG) Register Structures
**********************************************************************/

/* Random Number Generator (RNG) Module Register Structure */
typedef volatile struct
{
  volatile UNS_32  random_Number;   // Random number register       (read only )
  volatile UNS_32  clockStats;      // Statistics register          (read only )
  volatile UNS_32  clockStatsSelect;// Statistics selector register       (read/write)
  volatile UNS_32  _unused0[1018];  // Added to match the RNG memory map
  volatile UNS_32  powerDown;       // Power down mode register       (read/write)
  volatile UNS_32  _unused1;  // Added to match the RNG memory map
  volatile UNS_32  moduleId;        // IP Identifier register     (read only )
} RNG_REGS_T;

/* Macro pointing to SysRegs registers */
#define RNG_REGS  ((RNG_REGS_T  *)(RNG_BASE))

#ifdef __cplusplus
}
#endif

#endif /* LPC313X_RNG_H */
