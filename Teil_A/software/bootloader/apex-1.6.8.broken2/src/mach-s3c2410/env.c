/* env.c

   written by Marc Singer
   7 Nov 2004

   with modifications by David Anders
   06 Nov 2005

   Copyright (C) 2004 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   Environment for the s3c2410.

*/

#include <config.h>
#include <environment.h>
#include <driver.h>
#include <service.h>

#if ! defined (CONFIG_ENV_DEFAULT_CMDLINE)

__env struct env_d e_cmdline = {
	.key = "cmdline",
	.default_value = "console=ttySAC0 root=/dev/ram0 ",
	.description = "Linux kernel command line",
};

#endif
