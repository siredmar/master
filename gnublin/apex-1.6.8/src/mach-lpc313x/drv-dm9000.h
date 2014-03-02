/* drv-dm9000.h

   written by Marc Singer
   2 June 2006

   Copyright (C) 2006 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__MACH_DRV_DM9000_H__)
#    define   __MACH_DRV_DM9000_H__

/* ----- Includes */
#include "lpc313x.h"
/* ----- Types */

/* ----- Globals */

/* ----- Prototypes */

# define DM_WIDTH	16

/* Use the second static bank, with fast timings, as an IOBARRIER. ARM MPMC contoller
as part of low power design doesn't de-assert nCS and nOE for consecutive reads but 
just changes address. But DM9000 requires nCS and nOE change between address. So access
other chip select area to force de-assertion of nCS and nOE. Or else wait for long time 
such as 80 usecs. 
LPC313x has external logic outside of MPMC IP to toggle nOE to split consecutive reads.
For this work SYS_MPMC_WTD_DEL0 & SYS_MPMC_WTD_DEL1 should be programmed with MPMC_STWTRD0
& MPMC_STWTRD1 values. The logic only deactivates the nOE for one clock cycle which is
11.1nsec but DM9000 needs 80nsec between nOEs. So lets add some dummy instruction such as
udealy(0) to compensate for extra 70nsec.
*/

/* if EA_DISP_QVGA_LCD_V2 is used then use following macro else use udelay macro */
# define DM_IO_DELAY	do { *(volatile unsigned char __force*) (EXT_SRAM0_PHYS) = 0;} while(0)
//# define DM_IO_DELAY	do { GPIO_STATE(0);} while(0)

# define DM_PHYS	(EXT_SRAM1_PHYS)		/* Dev MAC/PHY */
# define DM_PHYS_INDEX	(DM_PHYS + 0)
# define DM_PHYS_DATA	(DM_PHYS + 0x10000)
# define DM_NAME	"dev"


#endif  /* __MACH_DRV_DM9000_H__ */
