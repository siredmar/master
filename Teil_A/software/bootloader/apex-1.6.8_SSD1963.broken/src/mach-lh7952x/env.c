/* env.c

   written by Marc Singer
   7 Nov 2004

   Copyright (C) 2004 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   Environment for the lh7952x.

*/

#include <config.h>
#include <environment.h>
#include <driver.h>
#include <service.h>

#if defined (CONFIG_MACH_LPD79520) && !defined (CONFIG_ENV_DEFAULT_CMDLINE)

__env struct env_d e_cmdline = {
  .key = "cmdline",
  .default_value = "console=ttyAMA1"
		   " root=/dev/hda1"
//		   " root=/dev/mtdblock1 rootfstype=jffs2"
		   " mtdparts="
		   "phys_mapped_flash:2m(boot)ro,-(root)"
  ,
  .description = "Linux kernel command line",
};

#endif

#if defined (CONFIG_MACH_LPD79524) && !defined (CONFIG_ENV_DEFAULT_CMDLINE)

__env struct env_d e_cmdline = {
  .key = "cmdline",
  .default_value = "console=ttyAMA0"
#if 0
		   " root=/dev/hda2 3" /* Boot to CF card */
#else
		   " root=/dev/hda1"
//		   " root=/dev/mtdblock1 rootfstype=jffs2"
		   " mtdparts="
		   "phys_mapped_flash:2m(boot)ro,-(root)"
		   ";"
//		   "lpd79524_nandflash:2m(boot)ro,-(root)"
#endif
  ,
  .description = "Linux kernel command line",
};

#endif

#if (defined (CONFIG_MACH_KEV79524) || defined (CONFIG_MACH_KEV79525))\
 && !defined (CONFIG_ENV_DEFAULT_CMDLINE)

__env struct env_d e_cmdline = {
  .key = "cmdline",
  .default_value = "console=ttyAMA0"
  //		   " root=/dev/hda1"
		   " root=/dev/mtdblock1 rootfstype=jffs2"
		   " mtdparts="
		   "lpd79524_norflash:2m(boot)ro,-(root)"
  ,
  .description = "Linux kernel command line",
};

#endif
