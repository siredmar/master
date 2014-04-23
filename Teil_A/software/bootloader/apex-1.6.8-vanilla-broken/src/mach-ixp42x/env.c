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

   Environment for the IXP42x.

*/

#include <config.h>
#include <environment.h>
#include <driver.h>
#include <service.h>

#define _s(v) #v
#define _t(v) _s(v)

#if defined (CONFIG_MACH_NSLU2) && !defined (CONFIG_ENV_DEFAULT_CMDLINE)

__env struct env_d e_cmdline = {
  .key = "cmdline",
  .default_value = "console=ttyS0,115200"
  /* Acording to dyoung, we need to use /dev/slug as the root device
     to get the slug to boot properly.  I hope this will change. */
		   " root=/dev/ram"
  //		   " root=/dev/slug"
  //		   " rootfstype=ext2,jffs2"
  //#if defined (CONFIG_RAMDISK_LMA)
  //		   " initrd=" _t(CONFIG_RAMDISK_LMA) ",10m"
  //#endif
  ,
  .description = "Linux kernel command line",
};

#endif
