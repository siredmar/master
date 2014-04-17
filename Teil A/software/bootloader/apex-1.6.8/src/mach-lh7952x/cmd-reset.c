/* cmd-reset.c

   written by Marc Singer
   12 Nov 2004

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
#include <apex.h>
#include <command.h>
#include <service.h>

#include "hardware.h"


static void cmd_reset (int argc, const char** argv)
{
  release_services ();		/* Primarily to prep NOR flash */

  RCPC_CTRL      |= (1<<9); /* Unlock */
  RCPC_SOFTRESET  = 0xdead;
}

static __command struct command_d c_reset = {
  .command = "reset",
  .func = (command_func_t) cmd_reset,
  COMMAND_DESCRIPTION ("reset target")
  COMMAND_HELP(
"reset\n"
"  Reset the system.\n"
"  This will perform an immediate, hard reset of the CPU.\n"
  )
};
