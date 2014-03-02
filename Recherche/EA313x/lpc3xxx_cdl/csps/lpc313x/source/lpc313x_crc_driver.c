/***********************************************************************
 * $Id:: lpc313x_crc_driver.c 1519 2008-12-17 23:16:59Z pdurgesh        $
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

#include "lpc313x_crc_driver.h"
#include "lpc313x_cgu_driver.h"

/***********************************************************************
 * CRC32 lookup table present in BootROM
 **********************************************************************/
/* First, the polynomial itself and its table of feedback terms.  The  */
/* polynomial is                                                       */
/* X^32+X^26+X^23+X^22+X^16+X^12+X^11+X^10+X^8+X^7+X^5+X^4+X^2+X^1+X^0 */
/* Note that we take it "backwards" and put the highest-order term in  */
/* the lowest-order bit.  The X^32 term is "implied"; the LSB is the   */
/* X^31 term, etc.  The X^0 term (usually shown as "+1") results in    */
/* the MSB being 1.                                                    */
/* The feedback terms table consists of 256, 32-bit entries.  Notes:   */
/*                                                                     */
/*  1. The table can be generated at runtime if desired; code to do so */
/*     is shown later.  It might not be obvious, but the feedback      */
/*     terms simply represent the results of eight shift/xor opera-    */
/*     tions for all combinations of data and CRC register values.     */
/*                                                                     */
/*  2. The CRC accumulation logic is the same for all CRC polynomials, */
/*     be they sixteen or thirty-two bits wide.  You simply choose the */
/*     appropriate table.  Alternatively, because the table can be     */
/*     generated at runtime, you can start by generating the table for */
/*     the polynomial in question and use exactly the same "updcrc",   */
/*     if your application needn't simultaneously handle two CRC       */
/*     polynomials.  (Note, however, that XMODEM is strange.)          */
/*                                                                     */
/*  3. For 16-bit CRCs, the table entries need be only 16 bits wide;   */
/*     of course, 32-bit entries work OK if the high 16 bits are zero. */
/*                                                                     */
/*  4. The values must be right-shifted by eight bits by the "updcrc"  */
/*     logic; the shift must be unsigned (bring in zeroes).  On some   */
/*     hardware you could probably optimize the shift in assembler by  */
/*     using byte-swap instructions.                                   */
static const UNS_32* crc32table = (const UNS_32*)ISROM_CRC32_TABLE;

/***********************************************************************
 * CRC32 driver public functions
 **********************************************************************/

/***********************************************************************
 *
 * Function: crc32_initialise
 *
 * Purpose: Initialize a CRC for computation
 *
 * Processing:
 *     Enable ISRAM clock and set initial CRC32 value.
 *
 * Parameters:
 *     crc: Pointer to CRC storage location
 *
 * Outputs: None
 *
 * Returns: NONE
 *
 * Notes: None
 *
 **********************************************************************/
void crc32_initialise(UNS_32 *crc)
{
  cgu_clk_en_dis(CGU_SB_ISROM_CLK_ID, 1);
  *crc = 0xFFFFFFFF;
}

/***********************************************************************
 *
 * Function: crc32_generate
 *
 * Purpose: Compute CRC on memory block
 *
 * Processing:
 *     Compute CRC on memory block.
 *
 * Parameters:
 *     pcrc: Pointer to the CRC value to be updated
 *     data: Pointer to data to be added to CRC calculation
 *     length: Length of 'data' in bytes
 *
 * Outputs: None
 *
 * Returns: NONE
 *
 * Notes: None
 *
 **********************************************************************/
void crc32_generate(UNS_32 *pcrc, UNS_8 *data, int length)
{
  UNS_32 crc = *pcrc;

  while (length--)
  {
    crc = crc32table[(crc ^ *data++) & 0xFF] ^(crc >> 8);
  }

  *pcrc = crc;
}

/***********************************************************************
 *
 * Function: crc32_complete
 *
 * Purpose: Completes a CRC computation
 *
 * Processing:
 *     Completes a CRC computation.
 *
 * Parameters:
 *     pcrc: Pointer to the CRC value to be updated
 *
 * Outputs: None
 *
 * Returns: NONE
 *
 * Notes: None
 *
 **********************************************************************/
void crc32_complete(UNS_32 *pcrc)
{
  *pcrc ^= 0xFFFFFFFF;
}
/***********************************************************************
 *
 * Function: crc32_compute
 *
 * Purpose: Compute CRC on memory block
 *
 * Processing:
 *     Compute CRC on memory block.
 *
 * Parameters:
 *     data: Pointer to data to be added to CRC calculation
 *     length: Length of 'data' in bytes
 *
 * Outputs: None
 *
 * Returns: CRC32 value computed
 *
 * Notes: None
 *
 **********************************************************************/
UNS_32 crc32_compute(UNS_8 *data, int length)
{
  UNS_32 crc ;

  crc32_initialise(&crc);
  crc32_generate(&crc, data, length);
  crc32_complete(&crc);
  return crc;
}
