/* cp15-cleanall-dcache.c

   written by Marc Singer
   4 Jan 2009

   Copyright (C) 2009 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#include <config.h>
#include <asm/cp15.h>

void cleanall_dcache (void)
{
  CLEANALL_DCACHE;
}
