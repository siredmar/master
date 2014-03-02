/***********************************************************************
 * $Id:: lpc313x_crc_driver.h 1519 2008-12-17 23:16:59Z pdurgesh     $
 *
 * Project: LPC313x CRC32 driver
 *
 * Description:
 *     This file contains driver finctions to compute CRC32 using
 *  lookup table present in LPC313x bootROM.
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

#ifndef LPC313X_CRC_DRIVER_H
#define LPC313X_CRC_DRIVER_H

#include "lpc_types.h"
#include "lpc313x_chip.h"

#ifdef __cplusplus
extern "C"
{
#endif



/***********************************************************************
 * CRC driver API functions
 **********************************************************************/

/*crc32_initialise - Initialize a CRC for computation */
extern void crc32_initialise(UNS_32 *pcrc);

/*crc32_generate - compute a CRC on memory block */
extern void crc32_generate(UNS_32 *pcrc, UNS_8 *data, int length);


/*crc32_complete - Completes a CRC computation */
extern void crc32_complete(UNS_32 *pcrc);

/* crc32_compute - compute CRC32 on memory block
 *                       Initialise/Generate/Completes */
extern UNS_32 crc32_compute(UNS_8 *data, int length);


#ifdef __cplusplus
}
#endif

#endif /* LPC313X_CRC_DRIVER_H */
