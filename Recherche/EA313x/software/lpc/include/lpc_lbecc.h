/***********************************************************************
 * $Id:: lpc_lbecc.h 745 2008-05-13 19:59:29Z pdurgesh                 $
 *
 * Project: NAND software ECC generation and correction
 *
 * Description:
 *     Generates, checks, and corrects 512 byte ECCs.
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

#ifndef LPC_LBECC_H
#define LPC_LBECC_H

#include "lpc_types.h"

#if defined (__cplusplus)
extern "C"
{
#endif

/***********************************************************************
 * ECC support
 **********************************************************************/

/* ECC512 data structure */
typedef UNS_16 LPC_ECC512 [2];

/* Generate parity lookup tables for software mode */
void lpc_eccinittables(void);

/* Generate an software ECC for a range of data */
UNS_32 lpc_eccgenerate512(LPC_ECC512 eccbuf,
                          UNS_8 *datbuf);

/* ECC error check types */
typedef enum
{
  LPC_ECC_NOERR,           /* ECC and data both correct */
  LPC_ECC_CORRECTED,       /* Data has been corrected */
  LPC_ECC_AREA_ERR,        /* Data is correct, but ECC is invalid */
  LPC_ECC_NOTCORRECTABLE   /* Data is not correctable */
} LPC_ECC_ERROR_T;

/* Check and correct ECC and data */
LPC_ECC_ERROR_T lpc_eccCheckAndCorrect(LPC_ECC512 eccgood,
                                       LPC_ECC512 eccerr,
                                       UNS_8 *buf);
#if defined (__cplusplus)
}
#endif /*__cplusplus */

#endif /* LPC_LBECC_H */
