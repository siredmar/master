/* spinner.c

   written by Marc Singer
   8 Nov 2004

   Copyright (C) 2004 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   Implements a user feedback mechanism while performing long running
   commands.

   The hook function takes as parameter the timer delta.  If the
   parameter is ~0, the spinner hook may choose to clear its display.

*/

#include <config.h>
#include <apex.h>

#include <spinner.h>

void (*hook_spinner) (unsigned value);

void spinner_step (void)
{
  static int value;
  static int step;
  static const unsigned char rgch[]
    = { '|', '/', '-', '\\', '|', '/', '-', '\\' } ;
  unsigned v = timer_delta (0, timer_read ());

  if (hook_spinner)
    hook_spinner (v);

  v = v/128;
  if (v == value)
    return;
  value = v;

  putchar ('\r');
  putchar (rgch[step]);
  step = (step + 1)%8;
}

void spinner_clear (void)
{
  if (hook_spinner)
    hook_spinner (~0);
}
