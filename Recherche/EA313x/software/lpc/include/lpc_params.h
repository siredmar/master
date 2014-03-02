/***********************************************************************
 * $Id:: lpc_params.h 745 2008-05-13 19:59:29Z pdurgesh                $
 *
 * Project: Common structures used in various drivers
 *
 * Description:
 *     This file contains structures used in various drivers. These
 *     structures are common amond drivers of different designs and
 *     tend to control generic aspects of the driver.
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

#ifndef LPC_PARAMS_H
#define LPC_PARAMS_H

#include "lpc_types.h"

#if defined (__cplusplus)
extern "C"
{
#endif

/***********************************************************************
 * Common UART control typedefs
 **********************************************************************/

/* Parity settings */
typedef enum
{
  UART_PAR_NONE,
  UART_PAR_EVEN,
  UART_PAR_ODD
} UART_PAR_T;

/* UART control */
typedef struct
{
  UNS_32 baud_rate;     /* Device baud rate */
  UART_PAR_T parity;    /* Parity selection */
  UNS_32 databits;      /* Number of data bits */
  UNS_32 stopbits;      /* Number of stop bits */
} UART_CONTROL_T;

#if defined (__cplusplus)
}
#endif /*__cplusplus */

#endif /* LPC_PARAMS_H */
