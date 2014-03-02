/***********************************************************************
 * $Id:: lpc_colors.c 745 2008-05-13 19:59:29Z pdurgesh                $
 *
 * Project: Color definitions
 *
 * Description:
 *     See the SMA_colors.h header file for a description of this
 *     package.
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

#include "lpc_colors.h"

/***********************************************************************
 * Functions
 **********************************************************************/

/***********************************************************************
 *
 * Function: lpc_colors_set_palette
 *
 * Purpose:  Generate a palette table (only in 8-bit mode).
 *
 * Processing:
 *     Depending on the target LCD color mapping (either 555 or 565), a
 *     palette table will be generated to convert colors stored in 233
 *     format to either 555 or 565 format through a lookup table.
 *
 * Parameters:
 *     palette_table : Pointer of where to put the 256 8-bit to 16-bit
 *                     palette conversion entries.
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes:
 *     If compiled in 16-bit color mode, this will be a NULL function.
 *     Select the appropriate define in this function for 555 or 565
 *     color mode displays when using an 256 color frame buffer.
 *
 **********************************************************************/
void lpc_colors_set_palette(UNS_16 *palette_table)
{
#if COLORS_DEF == 8
  INT_32 idx;
  UNS_16 entry, r, g, b;

  /* 256 entries */
  for (idx = 0; idx < NUM_COLORS; idx++)
  {
    r = ((UNS_16) idx & REDMASK) >> REDSHIFT;
    g = ((UNS_16) idx & GREENMASK) >> GREENSHIFT;
    b = ((UNS_16) idx & BLUEMASK) >> BLUESHIFT;

#ifdef COLORS_8_565_MODE
    /* Strip out and scale colors */
    r = r * 0x1F / ((REDMASK >> REDSHIFT) + 1);
    g = g * 0x3F / ((GREENMASK >> GREENSHIFT) + 1);
    b = b * 0x1F / ((BLUEMASK >> BLUESHIFT) + 1);
    entry = b + (g << 5) + (r << 11);

#else
    /* Strip out and scale colors */
    r = r * 0x1F / ((REDMASK >> REDSHIFT) + 1);
    g = g * 0x1F / ((GREENMASK >> GREENSHIFT) + 1);
    b = b * 0x1F / ((BLUEMASK >> BLUESHIFT) + 1);
    entry = b + (g << 5) + (r << 10);
#endif

    /* Save palette entry */
    palette_table [idx] = entry;
  }
#endif /* COLORS_DEF == 8 */
}
