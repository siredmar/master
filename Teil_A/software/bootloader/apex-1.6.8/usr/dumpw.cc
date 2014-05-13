/** @file dumpw.cc

    written by Marc Singer
    4 Jun 2008

    Copyright (C) 2008 Marc Singer

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    version 2 as published by the Free Software Foundation.  Please
    refer to the file debian/copyright for further details.

    @brief Routine to perform a hexadecimal dump (hd) of a buffer.

*/

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>

void dumpw (const char *rgb, int cb, unsigned long index, int width)
{
  int i;

  if ((width == 2 && ((unsigned long) rgb & 1))
      || (width == 4 && ((unsigned long) rgb & 3))) {
    printf ("%s: unable to display unaligned data\n", __FUNCTION__);
    return;
  }

  while (cb > 0) {
    printf ("%08lx: ", index);
    for (i = 0; i < 16; ++i) {
      if (i < cb) {
	switch (width) {
	  default:
	  case 1:
	    printf ("%02x ", (unsigned char) rgb[i]);
	    break;
	  case 2:
	    printf ("%04x ", *((uint16_t *) & rgb[i]));
	    ++i;
	    break;
	  case 4:
	    printf ("%08x ", *((uint32_t *) & rgb[i]));
	    i += 3;
	    break;
	}
      }
      else
	printf ("   ");
      if (i % 8 == 7)
	putchar (' ');
    }
    for (i = 0; i < 16; ++i) {
      if (i == 8)
	putchar (' ');
      putchar ((i < cb) ? (isprint (rgb[i]) ? rgb[i] : '.') : ' ');
    }
    printf ("\n");

    cb -= 16;
    index += 16;
    rgb += 16;
  }
}



