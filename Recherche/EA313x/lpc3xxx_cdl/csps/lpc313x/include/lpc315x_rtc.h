/***********************************************************************
* $Id:: lpc315x_rtc.h 1368 2008-11-26 02:53:37Z tangdz                $
*
* Project: LPC315X Real time clock definitions
*
* Description:
*     This file contains the structure definitions and manifest
*     constants for the LPC315X chip family component:
*         Real time clock
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

#ifndef LPC315X_RTC_H
#define LPC315X_RTC_H

#include "lpc_types.h"
#include "lpc313x_chip.h"

#if defined(__cplusplus)
extern "C"
{
#endif

/**********************************************************************
* status register definitions
**********************************************************************/
#define RTC_ALARM			_BIT(0)
#define RTC_TIME_UNSET		_BIT(1)
#define RTC_AD_INT_EN		_BIT(8)
#define RTC_PLAY_INT_EN		_BIT(9)
#define RTC_RST_RTC			_BIT(12)
#define RTC_LS_ENA			_BIT(13)
#define RTC_TIME_PENDING	_BIT(16)


#ifdef __cplusplus
}
#endif

#endif /* LPC315X_RTC_H */
