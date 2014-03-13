/* cmd-reset.c

   written by Marc Singer
   20 Sep 2008

   Copyright (C) 2007 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#include <config.h>
#include <apex.h>
#include <command.h>
#include <service.h>

#include "hardware.h"

/** Immediately reset the CPU. */

static void cmd_reset (int argc, const char** argv)
{
  release_services ();		/* Primarily to prep NOR flash */

  CPU_RSTOUTN    |= 1<<2;       /* Assert RSTOUTn on setting software reset */
  CPU_SOFT_RESET |= 1<<0;       /* Software reset */

  /* The code should never get here, but if it does, we ought to let
     the user know it's a problem.  Trouble is, we don't have a serial
     driver any more.  Oh, well.*/

  while (1)
    ;
}

static __command struct command_d c_reset = {
  .command = "reset",
  .func = (command_func_t) cmd_reset,
  COMMAND_DESCRIPTION ("reset target")
  COMMAND_HELP(
"reset\n"
"  Reset the system.\n"
"  This will perform an immediate, hard reset of the CPU core.\n"
  )
};
