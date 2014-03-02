/* cmd-reset.c

   written by Marc Singer
   14 Jan 2005

   Copyright (C) 2005 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   Perform system reset using the watchdog timer.

*/

#include <config.h>
#include <apex.h>
#include <command.h>
#include <service.h>
#include "hardware.h"

#define KEY			0x482e
#define OST_WDOG_ENAB_RESET	(1<<0) /* Allow watchdog to reset CPU */
#define OST_WDOG_ENAB_CNT_EN	(1<<2) /* Enable watchdog countdown */

static int cmd_reset (int argc, const char** argv)
{
  printf ("Performing watchdog reset\n");

  release_services ();		/* Primarily ensures NOR flash is readable */

  OST_WDOG_KEY = KEY;		/* Unlock watchdog registers */
  OST_WDOG = 1;			/* Short count */
  OST_WDOG_ENAB = 0
    | OST_WDOG_ENAB_RESET
    | OST_WDOG_ENAB_CNT_EN;

  return 0;			/* Control will never reach this point */
}

static __command struct command_d c_reset = {
  .command = "reset",
  .func = cmd_reset,
  COMMAND_DESCRIPTION ("reset target")
};
