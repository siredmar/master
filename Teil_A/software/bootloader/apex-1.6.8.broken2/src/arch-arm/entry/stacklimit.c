/* stacklimit.c

   written by Marc Singer
   10 July 2006

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
#include <service.h>

extern char APEX_VMA_STACKS_START;
extern char APEX_VMA_STACK_START;

/* stacklimit_report

   probes the stack to determine how much of the allocated stack space
   has been used.

*/

void stacklimit_report (void)
{
  int i;
  unsigned long* p = (unsigned long*) &APEX_VMA_STACKS_START;
  unsigned max = (&APEX_VMA_STACK_START - &APEX_VMA_STACKS_START)/4;
  for (i = 1; i < max; ++i)
    if (p[i] != 0xe5e5e5e5)
      break;

  printf ("   stack: %d used (%d)\n",
	  (max - i)*4, max*4);
}

static __service_0 struct service_d stacklimit_service = {
  .report = stacklimit_report,
};
