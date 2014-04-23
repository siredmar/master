/* entry.c

   written by Marc Singer
   15 Nov 2004

   Copyright (C) 2004 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   Default loader entry point.

*/

#include <config.h>
#include <asm/bootstrap.h>

#if 0
void __naked __section (.entry) entry (void)
{
  /* Fallthrough to reset section.  Exception vectors are handled
     differently.  This entry point exists for two reasons.  First,
     it's used to tell the linker where to enter.  We use the symbol
     entry() and the program entry point.  Second, this function
     allows a target to override the first executed instruction in
     case there is something it has to do.  The replacement function
     should be naked and fall-through to the .reset section. */
}
#endif
