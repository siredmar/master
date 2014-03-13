/* test-companion.c

   written by Marc Singer
   17 Oct 2006

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
#include <apex.h>
#include "hardware.h"
#include <command.h>
#include <console.h>

#include <debug_ll.h>

int cmd_click (int argc, const char** argv)
{
  int c = 120;
  int i;
  printf ("%s\n", __FUNCTION__);

  for (; c-- && !console->poll (0, 0); mdelay (500))
    for (i = 0; i < 15; ++i) {
      GPIO_PGD |= (1<<7);
      udelay (125);
      GPIO_PGD &= ~(1<<7);
      udelay (125);
    }

  return 0;
}

static __command struct command_d c_click = {
  .command = "click",
  .func = cmd_click,
  COMMAND_DESCRIPTION ("buzzer click")
  COMMAND_HELP (
"click\n"
"\n")
};
