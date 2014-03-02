/***********************************************************************
 * $Id:: lpc_api.h 745 2008-05-13 19:59:29Z pdurgesh                   $
 *
 * Project: Standard API
 *
 * Description:
 *  This file implements non hardware specific IO system mechanism
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

#ifndef	LPC_API_H
#define	LPC_API_H

#include "lpc_types.h"

#if defined (__cplusplus)
extern "C"
{
#endif

/* System API data structure  */
typedef struct API_S
{
  /* Open the device */
  PFI open;
  /* Close the device */
  PFI close;
  /* Read data from the device */
  PFI read;
  /* Wrote data to the device  */
  PFI write;
  /* Device control and configuration */
  PFI ioctl;
} API_T, *PAPI_T;

/* Api system device lookup table */
typedef struct API_TABLE_S
{
  /* Device driver callbacks */
  API_T  driver;
  /* Device Id */
  INT_32 id;
  /* Driver device id */
  INT_32 devid;
  /* File descriptor */
  INT_32 fd;
  /* Driver state */
  INT_32 opened;
} API_TABLE_T, *PAPI_TABLE_T;

/* Max number of devices in the subsystem */
#define MAX_API_TABLE (20)


/*
 * Public APIs used to access device drivers that are registered
 * with the API sub system.
 */
EXTERN void lpc_api_init(void* cfg);
EXTERN INT_32 lpc_open(INT_32 devid, INT_32 arg);
EXTERN INT_32 lpc_close(INT_32 fd);
EXTERN INT_32 lpc_read(INT_32 fd, CHAR* buffer, INT_32 max_bytes);
EXTERN INT_32 lpc_write(INT_32 fd, CHAR* buffer, INT_32 n_bytes);
EXTERN INT_32 lpc_ioctl(INT_32 fd, INT_32 cmd, INT_32 arg);
EXTERN INT_32 lpc_api_register(INT_32 devid,
                               void*  open,
                               void*  close,
                               void*  read,
                               void*  write,
                               void*  ioctl);

#if defined (__cplusplus)
}
#endif /*__cplusplus */

#endif	/* LPC_API_H */
