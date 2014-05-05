/* cmd-version.c

   written by Marc Singer
   3 Nov 2004

   Copyright (C) 2004 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#include <config.h>
#include <linux/types.h>
#include <apex.h>
#include <command.h>
#include <service.h>
#include <environment.h>
#include <alias.h>

extern char APEX_VMA_COPY_START;
extern char APEX_VMA_COPY_END;

int cmd_version (int argc, const char** argv)
{
  printf (
"\n\nAPEX Boot Loader " APEXVERSION
" -- Copyright (c) 2004-2008 Marc Singer\n"
#if defined (CONFIG_TARGET_DESCRIPTION) && !defined (CONFIG_SMALL)
"  compiled for " CONFIG_TARGET_DESCRIPTION " on " BUILDDATE "\n"
#endif
"\n    APEX comes with ABSOLUTELY NO WARRANTY."
#if defined (CONFIG_SMALL)
"\n\n"
#endif
  );

#if !defined (CONFIG_SMALL)
  printf (
"  It is free software and\n"
"    you are welcome to redistribute it under certain circumstances.\n"
"    For details, refer to the file COPYING in the program source."
"\n\n");
  {
    char sz[40];
    snprintf (sz, sizeof (sz), "mem:0x%p+0x%lx",
	      (void*) &APEX_VMA_COPY_START,
	      (unsigned long )(&APEX_VMA_COPY_END - &APEX_VMA_COPY_START));
    printf ("  apex => %-23.23s (%ld bytes)\n",
	    sz, (unsigned long )(&APEX_VMA_COPY_END - &APEX_VMA_COPY_START));
  }
#if defined (CONFIG_CMD_SETENV)
  printf ("  env  => %-23.23s (", CONFIG_ENV_REGION);
  switch (env_check_magic (1)) {
  case 0:
    printf ("in-use");
    break;
  case 1:
    printf ("empty");
    break;
  default:
  case -1:
    printf ("no-write");
    break;
  case -2:
    printf ("bad-region");
    break;
  }
  printf (")\n");
#endif

#if defined (CONFIG_ALIASES) && defined (CONFIG_VARIATIONS)
  {
    const char* sz = alias_lookup ("variation");
    if (sz)
      printf ("  variation => %s\n", sz);
  }
#endif

#endif

  if (argc >= 0) {
#if !defined (CONFIG_SMALL)
    extern char APEX_SERVICE_START;
    extern char APEX_SERVICE_END;
    struct service_d* service;

    putchar ('\n');

    for (service = (struct service_d*) &APEX_SERVICE_START;
	 service < (struct service_d*) &APEX_SERVICE_END;
	 ++service)
      if (service->report)
	service->report ();
#endif
  }

#if defined (CONFIG_ALLHELP)
  printf ("\n    Use the command 'help help' to get started.\n\n");
#endif

  return 0;
}

static __command struct command_d c_version = {
  .command = "version",
  .func = cmd_version,
  COMMAND_DESCRIPTION ("show version and copyright")
  COMMAND_HELP(
"version\n"
"  Display version, copyright, and system summary information\n"
)
};
