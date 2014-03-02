/* console-printf.c

   written by Marc Singer
   2 Nov 2004

   Copyright (C) 2004 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   The \r insertion code is, on the whole, relatively expensive
   code-size wise.  It could be more elegant, but that would be an
   even larger piece of code.  Still, we do it so that the program
   code is more sensible.

*/

#include <stdarg.h>
#include <driver.h>
#include <linux/string.h>
#include <apex.h>
#include <console.h>
#include <debug_ll.h>


int printf (const char* fmt, ...)
{
  static char __xbss(console) rgb[CB_PRINTF_MAX];
  ssize_t cb;
  va_list ap;
  extern ssize_t console_write (struct descriptor_d* d,
				const void* pv, size_t cb);


  //  PUTC_LL ('P');

  if (console == NULL)
    return 0;

  //  PUTC_LL ('P');

  va_start (ap, fmt);

  //  PUTC_LL ('P');
  cb = vsnprintf (rgb, sizeof (rgb) - 1, fmt, ap);
  va_end (ap);
  rgb[cb] = 0;

  //  PUTC_LL ('P');

  {
    char* pb = rgb;
    for (; *pb; ++pb) {
      if (*pb == '\n')
	console->write (0, "\r", 1);
      console->write (0, pb, 1);
    }
  }

  //  PUTC_LL ('_');

  return cb;
}
