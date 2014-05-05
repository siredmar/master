/* env.c

   written by Marc Singer
   16 Dec 2006

   Copyright (C) 2006 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#include <config.h>
#include <environment.h>
#include <driver.h>
#include <service.h>

#if ! defined (CONFIG_ENV_DEFAULT_CMDLINE)

__env struct env_d e_cmdline = {
  .key = "cmdline",
  .default_value = "console=ttyS0 root=/dev/sda1 "
  ,
  .description = "Linux kernel command line",
};

#endif
