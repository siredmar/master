/***********************************************************************
* $Id:: lpc313x_wdt.h 1180 2008-09-11 19:32:40Z pdurgesh              $
*
* Project: LPC313X Watch Dog Timer (WDOG) definitions
*
* Description:
*     This file contains the structure definitions and manifest
*     constants for the LPC313X chip family component:
*         Watch Dog Timer (WDOG)
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

#ifndef LPC313X_WDT_H
#define LPC313X_WDT_H

#include "lpc_types.h"
#include "lpc313x_chip.h"

#ifdef __cplusplus
extern "C"
{
#endif

/***********************************************************************
* Watch Dog Timer (WDOG) Register Structures
**********************************************************************/

/* Watch Dog Timer (WDOG) Module Register Structure */
typedef volatile struct
{
  volatile UNS_32 ir;
  volatile UNS_32 tcr;
  volatile UNS_32 tc;
  volatile UNS_32 pr;
  volatile UNS_32 pc;
  volatile UNS_32 mcr;
  volatile UNS_32 mr0;
  volatile UNS_32 mr1;
  volatile UNS_32 mr2;
  volatile UNS_32 mr3;
  volatile UNS_32 ccr;
  volatile UNS_32 cr0;
  volatile UNS_32 cr1;
  volatile UNS_32 cr2;
  volatile UNS_32 cr3;
  volatile UNS_32 emr;
} WDT_REGS_T;

/***********************************************************************
 * WDT interrupt register definitions
 **********************************************************************/
#define WDT_IR_MR1        _BIT(1)
#define WDT_IR_MR0        _BIT(0)

/***********************************************************************
 * WDT TCR register definitions
 **********************************************************************/
#define WDT_TCR_CNT_RESET _BIT(1)
#define WDT_TCR_CNT_EN    _BIT(0)

/***********************************************************************
 * WDT MCR register definitions
 **********************************************************************/
#define WDT_MCR_STOP_MR1  _BIT(5)
#define WDT_MCR_RESET_MR1 _BIT(4)
#define WDT_MCR_INT_MR1   _BIT(3)
#define WDT_MCR_STOP_MR0  _BIT(2)
#define WDT_MCR_RESET_MR0 _BIT(1)
#define WDT_MCR_INT_MR0   _BIT(0)

/***********************************************************************
 * WDT EMR register definitions
 **********************************************************************/
#define WDT_EMR_CTRL0(n)  _SBF(4,((n) &0x3))
#define WDT_EMR_CTRL1(n)  _SBF(6,((n) &0x3))
#define WDT_EMR_M1        _BIT(1)
#define WDT_EMR_M0        _BIT(0)

/**********************************************************************
* Macro to access WDT registers
**********************************************************************/
#define WDT   ((WDT_REGS_T*)WDOG_BASE)

#ifdef __cplusplus
}
#endif

#endif /* LPC313X_WDT_H */
