/* timer.c

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

*/

#include <config.h>
#include <service.h>
#include "hardware.h"

unsigned long timer_read (void)
{
  return OST_TS;
}


/* timer_delta

   returns the difference in time in milliseconds.

   The base counter rate is 66.66MHz or a 15ns cycle time.  1us takes
   67 counts, 1ms takes 66660 counts.  It wraps at about 64 seconds.

   The math works out such that wrapping around the end will return
   the expected result.

*/

unsigned long timer_delta (unsigned long start, unsigned long end)
{
  return (end - start)/66660;
}
