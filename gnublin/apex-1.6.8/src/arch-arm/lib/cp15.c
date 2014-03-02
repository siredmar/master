/* cp15.c

   written by Marc Singer
   25 Jul 2006

   Copyright (C) 2006 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#include <linux/string.h>
#include <config.h>

const char* cp15_ctrl (unsigned long ctrl)
{
#if defined (CONFIG_CPU_ARMV6)
  static const char bits[] = "at ..ev xuf. .... ..vi zfrs bldp wcam";
#else
  static const char bits[] =                     "vi zfrs bldp wcam";
#endif
  const int cbits = sizeof (bits) - 1;
  static char sz[sizeof (bits)];
  int i;

  strcpy (sz, bits);

  for (i = 0; i < cbits; ++i) {
    if (sz[i] == '.' || sz[i] == ' ')
      continue;
    if (ctrl & (1<<(cbits - i - 1 - ((cbits - i)/5))))
      sz[i] += 'A' - 'a';
  }
  return sz;
}
