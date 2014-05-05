/* drv-npe.h

   written by Marc Singer
   21 Jul 2006

   Copyright (C) 2006 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__DRV_NPE_H__)
#    define   __DRV_NPE_H__

/* ----- Includes */

#include <mach/hardware.h>

/* ----- Types */

#define ETH_PHYS	IXP4XX_ETHB_PHYS
#define ETH_NPE_ID	1
#define ETH_PHY_ID	1
#define ETH_ETH_ID	0
#define ETH_RXQ_ID	27
#define ETH_TXQ_ID	24

#endif  /* __DRV-NPE_H__ */
