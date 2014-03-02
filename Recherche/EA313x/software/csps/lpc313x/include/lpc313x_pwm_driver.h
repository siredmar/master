/***********************************************************************
 * $Id:: lpc313x_pwm_driver.h 1519 2008-12-17 23:16:59Z pdurgesh     $
 *
 * Project: LPC313X pwm driver
 *
 * Description:
 *     This file contains driver support for the LPC313X pwm.
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

#ifndef LPC313X_PWM_DRIVER_H
#define LPC313X_PWM_DRIVER_H

#include "lpc313x_pwm.h"

#ifdef __cplusplus
extern "C"
{
#endif

/***********************************************************************
 * PWM device configuration commands (IOCTL commands and arguments)
 **********************************************************************/

/* PWM device commands (IOCTL commands) */
typedef enum
{
  PWM_ENABLE, /* Enable PWM (clocks), arg = 0 disables the module */
  PWM_PDM,    /* PDM control, arg = 0 disables PDM */
  PWM_HI,     /* PWM ouptut high control, arg = 0 disables it */
  PWM_HIGH,   /* PWM ouptut high control, arg = 0 disables it */
  PWM_LOOP,   /* PWM LOOP control, arg = 0 disable, else set M */
  PWM_CLK,    /* PWM CLK sel, arg = 0/1/2/3 for pwm_clk/1/2/4/8 */
  PWM_DUTY_A, /* Set PWM duty-cycle, copy arg into the tmr */
  PWM_DUTY_R  /* Set PWM duty-cycle, convert arg [%] to tmr */
} PWM_IOCTL_CMD_T;


/***********************************************************************
 * PWM driver API functions
 **********************************************************************/

/* Open the pwm */
INT_32 pwm_open(void *ipbase,
                INT_32 arg);

/* Close the pwm */
STATUS pwm_close(INT_32 devid);

/* PWM configuration block */
STATUS pwm_ioctl(INT_32 devid,
                 INT_32 cmd,
                 INT_32 arg);

/* PWM read function (stub only) */
INT_32 pwm_read(INT_32 devid,
                void *buffer,
                INT_32 max_bytes);

/* PWM write function (stub only) */
INT_32 pwm_write(INT_32 devid,
                 void *buffer,
                 INT_32 n_bytes);

#ifdef __cplusplus
}
#endif

#endif /* LPC313X_PWM_DRIVER_H */
