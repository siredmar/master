/* drv-smc91x.h

   written by Marc Singer
   9 Jul 2008

   Copyright (C) 2008 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__DRV_SMC91X_H__)
#    define   __DRV_SMC91X_H__

#include <mach/hardware.h>

/* The PCM037 puts the SMC91x on CS1, 0xa8000000 */
# define SMC_IOBASE		(0xa8000000)

#define RPC_LSA_DEFAULT		RPC_LS_TX_RX
#define RPC_LSB_DEFAULT		RPC_LS_100_10

#endif  /* __DRV_SMC91X_H__ */
