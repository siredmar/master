/* describe-size.c

   written by Marc Singer
   21 Dec 2008

   Copyright (C) 2008 Marc Singer

   -----------
   DESCRIPTION
   -----------

*/

#include <config.h>
#include <apex.h>
#include <command.h>

const char* describe_size (uint32_t cb)
{
  static __xbss(image) char sz[60];

  int s = cb/1024;
  int rem = cb%1024;
  char unit = 'K';

  if (s > 1024) {
    rem = s%1024;
    s /= 1024;
    unit = 'M';
  }
  snprintf (sz, sizeof (sz), "%d bytes (%d.%02d %ciB)",
            cb, (int) s, (rem*100)/1024, unit);
  return sz;
}

