/* cmd-reset.c

   written by Durgesh Pattamatta
   1 Nov 2004

   Copyright (C) 2008 NXP Semiconductors

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
  release_services ();		

  /* Disable watchdog */
  WDT_TCR = 0;
  WDT_MCR = WDT_MCR_STOP_MR1 | WDT_MCR_INT_MR1;

  /*  If TC and MR1 are equal a reset is generated. */
  WDT_PR  = 0x00000002;
  WDT_TC  = 0x00000FF0;
  WDT_MR0 = 0x0000F000;
  WDT_MR1 = 0x00001000;
  WDT_EMR = WDT_EMR_CTRL1(0x3);
  /* Enable watchdog timer; assert reset at timer timeout */
  WDT_TCR = WDT_TCR_CNT_EN;

  while(1);	/* loop forever and wait for reset to happen */

  /*NOTREACHED*/
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
