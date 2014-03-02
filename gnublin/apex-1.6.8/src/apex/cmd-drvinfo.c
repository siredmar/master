/* cmd-drvinfo.c

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

#include <linux/types.h>
#include <apex.h>
#include <command.h>
#include <driver.h>

static int cmd_drvinfo (int argc, const char** argv)
{
  extern char APEX_DRIVER_START;
  extern char APEX_DRIVER_END;
  struct driver_d* d;

  for (d = (struct driver_d*) &APEX_DRIVER_START;
       d < (struct driver_d*) &APEX_DRIVER_END;
       ++d) {
    if (!d->name)
      continue;
    printf (" %-*.*s - %s\n", 16, 16, d->name,
	    d->description ? d->description : "?");
  }

  return 0;
}

static __command struct command_d c_drvinfo = {
  .command = "drvinfo",
  .func = cmd_drvinfo,
  COMMAND_DESCRIPTION ("list available drivers")
  COMMAND_HELP(
"drvinfo\n"
"  Lists available drivers\n"
  )
};
