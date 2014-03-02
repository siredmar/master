/***********************************************************************
* $Id:: lpc313x_pwm.h 1412 2008-12-04 05:55:22Z stefanovicz           $
*
* Project: LPC313X Pulse Width Modulation controller definitions
*
* Description:
*     This file contains the structure definitions and manifest
*     constants for the LPC313X chip family component:
*         Pulse Width Modulation Controller
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

#ifndef LPC313X_PWM_H
#define LPC313X_PWM_H

#include "lpc_types.h"
#include "lpc313x_chip.h"

#ifdef __cplusplus
extern "C"
{
#endif

/***********************************************************************
* Pulse Width Modulation controller Register Structures
**********************************************************************/

/* Pulse Width Modulation Module Register Structure */
typedef volatile struct
{
  volatile UNS_32 tmr;   /* PWM Timer   */
  volatile UNS_32 cntl;  /* PWM Control */
} PWM_REGS_T;

/***********************************************************************
* PWM Control Register (cntl) (0x004) Read/Write
**********************************************************************/
#define PWM_PDM_BIT     _BIT(7)
#define PWM_LOOP_BIT    _BIT(6)
#define PWM_HI_BIT      _BIT(4)
#define PWM_CLK_BIT(n)  _SBF(0, ((n) & 0x3))

/**********************************************************************
* Macro to access PWM registers
**********************************************************************/
#define PWM         ((PWM_REGS_T*)PWM_BASE)

#ifdef __cplusplus
}
#endif

#endif /* LPC313X_PWM_H */
