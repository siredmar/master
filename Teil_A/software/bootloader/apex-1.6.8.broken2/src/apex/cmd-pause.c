/* cmd-pause.c

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

   This function should never be included in production builds.  It's
   available for testing timers.  A general purpose pause command
   would be welcome, one that accepts a timeout parameter.


   NOTES
   -----

   o Should modify this to allow for a parameter to control which
     timer and the interval.

*/

#include <linux/types.h>
#include <apex.h>
#include <command.h>
#include <error.h>
#include <linux/string.h>
#include <linux/kernel.h>

#define SEC_PAUSE 5

int cmd_pause (int argc, const char** argv)
{
#if 0
  u8 b;
  u16 s;
  u32 w;
  u32 addr;

  if (argc < 2)
    return -1;

  addr = simple_strtoul (argv[1], NULL, 0);
  addr &= ~3;
  b = *(u8*) addr;
  s = *(u16*) addr;
  w = *(u32*) addr;

  printf ("b 0x%x s 0x%x w 0x%x\n", b, s, w);
#endif

#if 1

  int mode = 0;
  printf ("pausing for %d seconds\n", SEC_PAUSE);

  if (argc > 1) {
    if (argc > 2)
      return ERROR_PARAM;
    switch (*argv[1]) {
    case 't':
      mode = 0;
      break;
    case 'u':
      mode = 1;
      break;
    default:
      return ERROR_PARAM;
    }
  }

  if (mode == 0) {
    unsigned long time = timer_read ();
    while (timer_delta (time, timer_read ()) < SEC_PAUSE*1000)
      ;
  }

  if (mode == 1) {
    int i = SEC_PAUSE*1000*1000;
    i /= 32*1000;
    while (i--)
      usleep (32*1000);
  }


#if 0
  usleep (SEC_PAUSE*1000*1000);
#endif

  printf ("done\n");
#endif
  return 0;
}

static __command struct command_d c_pause = {
  .command = "pause",
  .func = cmd_pause,
  COMMAND_DESCRIPTION ("pause for 2 seconds")
  COMMAND_HELP(
"pause [MODE]\n"
"  Pause for 5 seconds using one of several timing functions.\n"
"  This command is used to test the timing functions within APEX.\n"
"  The optional MODE selects which timer function to use.  Both methods\n"
"  should be reasonably accurate.\n"
"    t  - use the timer functions (default)\n"
"    u  - use the usleep timer\n"
)
};
