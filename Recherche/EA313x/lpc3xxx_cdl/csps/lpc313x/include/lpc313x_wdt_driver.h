/***********************************************************************
 * $Id:: lpc313x_wdt_driver.h 1180 2008-09-11 19:32:40Z pdurgesh       $
 *
 * Project: LPC313X WDT driver
 *
 * Description:
 *     This file contains driver support for the LPC313X WDT.
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
 *********************************************************************/

#ifndef LPC313X_WDT_DRIVER_H
#define LPC313X_WDT_DRIVER_H

#include "lpc313x_wdt.h"

#ifdef __cplusplus
extern "C"
{
#endif

/***********************************************************************
 * WDT device configuration commands (IOCTL commands and arguments)
 **********************************************************************/

/* WDT match options */
typedef enum
{
  WDT_MATCH_IDLE = 0,
  WDT_MATCH_EN_INT,
  WDT_MATCH_EN_RESET,
  WDT_MATCH_EN_INT_RESET,
  WDT_MATCH_EN_STOP,
  WDT_MATCH_EN_STOP_INT,
  WDT_MATCH_EN_STOP_RESET,
  WDT_MATCH_EN_STOP_RESET_INT
} WDT_MATCH_OPT;

/* WDT external match control options */
typedef enum
{
  WDT_EXT_MATCH_IDLE = 0,
  WDT_EXT_MATCH_HIGH = 2
} WDT_EXT_MATCH_OPT;

/* Structure for initial setup of the WDT */
typedef struct
{
  BOOL_32 initial_setup;
  UNS_32 resfrc2;
  UNS_32 resfrc1;
  UNS_32 m_res2;
  UNS_32 m_res1;
  WDT_EXT_MATCH_OPT ext_match_setup;
  WDT_MATCH_OPT match_setup;
  UNS_32 match0;
  BOOL_32 match0_update;
  UNS_32 counter;
  BOOL_32 counter_update;
  UNS_32 pulse;
  BOOL_32 pulse_update;
} WDT_SETUP_T;

/* WDT device commands (IOCTL commands) */
typedef enum
{
  WDT_TIMER_CONTROL,      /* WDT timer control */
  WDT_COUNTER_READ,     /* WDT counter read */
  WDT_COUNTER_WRITE,      /* WDT counter write */
  WDT_MATCH_WRITE,        /* WDT match write */
  WDT_SETUP,              /* WDT setup */
  WDT_INT_ENABLE,         /* enable WDT interrupt */
  WDT_INT_DISABLE,        /* disable WDT interrupt */
  WDT_INT_CLEAR           /* clear WDT interrupt */
} WDT_IOCTL_CMD_T;

/* WDT timer control options */
typedef enum
{
  WDT_TIMER_OFF,      /* reset the timer control register */
  WDT_TIMER_STOP,     /* stop the timer */
  WDT_TIMER_RESET,    /* reset the timer = stop + reset */
  WDT_TIMER_RESTART,  /* restart the timer = stop + reset + go */
  WDT_TIMER_GO        /* let the timer run */
} WDT_IOCTL_TIMER_OPT;

/***********************************************************************
 * WDT driver API functions
 **********************************************************************/

/* Open the WDT */
INT_32 wdt_open(void *ipbase, INT_32 arg);

/* Close the WDT */
STATUS wdt_close(INT_32 devid);

/* WDT configuration block */
STATUS wdt_ioctl(INT_32 devid,
                 INT_32 cmd,
                 INT_32 arg);

/* WDT read function (stub only) */
INT_32 wdt_read(INT_32 devid,
                void *buffer,
                INT_32 max_bytes);

/* WDT write function (stub only) */
INT_32 wdt_write(INT_32 devid,
                 void *buffer,
                 INT_32 n_bytes);

/***********************************************************************
 * Other WDT driver functions
 **********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* LPC313X_WDT_DRIVER_H */
