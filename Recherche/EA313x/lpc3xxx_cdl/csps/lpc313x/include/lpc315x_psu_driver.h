/***********************************************************************
 * $Id:: lpc315x_psu_driver.h 1388 2008-12-01 19:20:06Z pdurgesh      $
 *
 * Project: LPC315x PSU driver
 *
 * Description:
 *     This file contains driver support for the LPC315x PSU.
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

#ifndef LPC315X_PSU_DRIVER_H
#define LPC315X_PSU_DRIVER_H

#include "lpc315x_ad.h"

#ifdef __cplusplus
extern "C"
{
#endif

/***********************************************************************
 * PSU configuration commands (IOCTL commands and arguments)
 **********************************************************************/

/* PSU commands (IOCTL commands) */
typedef enum
{
  PSU_WRITE_VOLTAGE,    /* set the voltage of DCDC1 or DCDC2, use arg as
                           a pointer to a structure of type
						   PSU_DCDC_VOLTAGE_T */
  PSU_READ_VOLTAGE,     /* get the voltage of DCDC1 or DCDC2, use arg as
                           a pointer to a structure of type
						   PSU_DCDC_VOLTAGE_T */
  PSU_SET_STABLE_CLOCK, /* enable/disable the digital die clock, use arg
                           as a value of type PSU_DCDC_CLOCKMODE_T */
  PSU_ENABLE_LDO,       /* enable/disable the LDO, use arg as
                           a value of type PSU_DCDC_LDOCONTROL_T */
  PSU_GET_STATUS,       /* get the status of the PSU, use arg as
                           a value of type PSU_IOCTL_STS_T */
  PSU_SET_1V8_CONTROL,  /* set the voltage of LDO3, use arg as
                           a value of type PSU_DCDC_1V8CONTROL_T */
  PSU_SET_USBOTG_CHARGE_PUMP_CTRL,
  /* switch on/off the USBOTG charge pump, use arg
  as a pointer to a structure of type
  PSU_DCDC_USBOTGCHARGEPUMPCTRL_T */
  PSU_GET_USBOTG_CHARGE_PUMP_CTRL
  /* get the status of the OTG charge pump, use arg
  as a pointer to a structure of type
  PSU_DCDC_USBOTGCHARGEPUMPCTRL_T */
} PSU_IOCTL_CMD_T;

/* PSU arguments for PSU_GET_STATUS command (IOCTL arguments) */
typedef enum
{
  PSU_DCDC_USB_DETECT,          /* When used as arg, will return a '1'
                                   if 5V USB bus detected,
								   otherwise '0' */
  PSU_DCDC_DCDC2_MODE_CONT,     /* When used as arg, will return a '1'
                                   if DCDC2 is not running in
								   continuous conduction mode,
								   otherwise '0' */
  PSU_DCDC_DCDC1_MODE_CONT,     /* When used as arg, will return a '1'
                                   if DCDC1 is not running in
								   continuous conduction mode,
								   otherwise '0' */
  PSU_DCDC_DCDC2_MODE_DISCONT,  /* When used as arg, will return a '1'
                                   if DCDC2 is not running in
								   discontinuous conduction mode,
								   otherwise '0' */
  PSU_DCDC_DCDC1_MODE_DISCONT,  /* When used as arg, will return a '1'
                                   if DCDC1 is not running in
								   discontinuous conduction mode,
								   otherwise '0' */
  PSU_DCDC_DCDC2_MODE_INIT,     /* When used as arg, will return a '1'
                                   if DCDC2 is not running in
								   initialization mode, otherwise '0' */
  PSU_DCDC_DCDC1_MODE_INIT,     /* When used as arg, will return a '1'
                                   if DCDC1 is not running in
								   initialization mode, otherwise '0' */
  PSU_DCDC_DCDC2_MODE_RAMPUP,   /* When used as arg, will return a '1'
                                   if DCDC2 is not running in
								   ramp up mode, otherwise '0' */
  PSU_DCDC_DCDC1_MODE_RAMPUP    /* When used as arg, will return a '1'
                                   if DCDC2 is not running in
								   ramp up mode, otherwise '0' */
} PSU_IOCTL_STS_T;

/* PSU arguments for PSU_WRITE_VOLTAGE, PSU_READ_VOLTAGE command
   (IOCTL arguments) */
typedef enum
{
  PSU_DCDC_DCDC1,
  PSU_DCDC_DCDC2
} PSU_DCDC_SELECT_T;

typedef enum
{
  PSU_DCDC_RANGE_0,
  PSU_DCDC_RANGE_1,
  PSU_DCDC_RANGE_2,
  PSU_DCDC_RANGE_3,
  PSU_DCDC_RANGE_4,
  PSU_DCDC_RANGE_5,
  PSU_DCDC_RANGE_6,
  PSU_DCDC_RANGE_7
} PSU_DCDC_RANGE_T;

typedef struct
{
  PSU_DCDC_SELECT_T psu_dcdc_select;
  PSU_DCDC_RANGE_T *psu_dcdc_range;
} PSU_DCDC_VOLTAGE_T;

/* PSU arguments for PSU_SET_STABLE_CLOCK command
   (IOCTL arguments) */
typedef enum
{
  PSU_DCDC_CLK_OFF,
  PSU_DCDC_CLK_ON
} PSU_DCDC_CLOCKMODE_T;

/* PSU arguments for PSU_ENABLE_LDO command
   (IOCTL arguments) */
typedef enum
{
  PSU_DCDC_LDO_OFF,
  PSU_DCDC_LDO_ON
} PSU_DCDC_LDOCONTROL_T;

/* PSU arguments for PSU_SET_1V8_CONTROL command
   (IOCTL arguments) */
typedef enum
{
  PSU_DCDC_LDO3_1V4,
  PSU_DCDC_LDO3_1V8
} PSU_DCDC_1V8CONTROL_T;

/* PSU arguments for PSU_SET_USBOTG_CHARGE_PUMP_CTRL,
   PSU_GET_USBOTG_CHARGE_PUMP_CTRL command
   (IOCTL arguments) */
typedef enum
{
  PSU_DCDC_USBOTGCHARGEPUMPCTRL_OFF,
  PSU_DCDC_USBOTGCHARGEPUMPCTRL_ON
} PSU_DCDC_USBOTGCHARGEPUMPCTRL_T;

/***********************************************************************
 * PSU driver API functions
 **********************************************************************/

/* Open the PSU */
INT_32 psu_open(void *ipbase, INT_32 arg);

/* Close the PSU */
STATUS psu_close(INT_32 devid);

/* PSU configuration block */
STATUS psu_ioctl(INT_32 devid,
                 INT_32 cmd,
                 INT_32 arg);

/* PSU read function */
INT_32 psu_read(INT_32 devid,
                void *buffer,
                INT_32 max_bytes);

/* PSU write function */
INT_32 psu_write(INT_32 devid,
                 void *buffer,
                 INT_32 n_bytes);

#ifdef __cplusplus
}
#endif

#endif /* LPC315X_PSU_DRIVER_H */
