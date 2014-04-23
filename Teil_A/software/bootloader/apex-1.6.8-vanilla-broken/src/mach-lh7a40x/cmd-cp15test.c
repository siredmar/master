/* cmd-reset.c

   written by Marc Singer
   27 Dec 2005

   Copyright (C) 2005 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   Support for fiddling with the ARM920T/ARM922T CP15 test
   registers.

*/

#include <config.h>
#include <apex.h>
#include <command.h>
#include <linux/string.h>

#define TEST_DCACHE_STREAMING	(1<<12)
#define TEST_ICACHE_STREAMING	(1<<11)
#define TEST_DCACHE_FILL	(1<<10)
#define TEST_ICACHE_FILL	(1<<9)

static void cmd_cp15test (int argc, const char** argv)
{
  const char* pch = NULL;
  unsigned long test = 0;

  if (argc > 1)
    pch = argv[1];

  __asm volatile ("mrc p15, 0, %0, c15, c0, 0" : "=r" (test));

  if (strcmp (pch, "ds") == 0)
    test &= ~TEST_DCACHE_STREAMING;
  if (strcmp (pch, "nods") == 0)
    test |=  TEST_DCACHE_STREAMING;

  if (strcmp (pch, "is") == 0)
    test &= ~TEST_ICACHE_STREAMING;
  if (strcmp (pch, "nois") == 0)
    test |=  TEST_ICACHE_STREAMING;

  if (strcmp (pch, "df") == 0)
    test &= ~TEST_DCACHE_FILL;
  if (strcmp (pch, "nodf") == 0)
    test |=  TEST_DCACHE_FILL;

  if (strcmp (pch, "if") == 0)
    test &= ~TEST_ICACHE_FILL;
  if (strcmp (pch, "noif") == 0)
    test |=  TEST_ICACHE_FILL;

  if (strcmp (pch, "clear") == 0)
    test = 0;

  printf ("cp15test 0x%04lx\n", test);

  if (pch)
    __asm volatile ("mcr p15, 0, %0, c15, c0, 0" :: "r" (test));

}

static __command struct command_d c_cp15test = {
  .command = "cp15test",
  .func = (command_func_t) cmd_cp15test,
  COMMAND_DESCRIPTION ("cp15test [OP]")
  COMMAND_HELP(
"test\n"
"  Modify the CP15 test register.  Beware, these are dangerous features.\n"
"  The valid OPs are:\n"
"    ds    - enable DCache streaming\n"
"    is    - enable ICache streaming\n"
"    nods  - disable DCache streaming\n"
"    nois  - disable ICache streaming\n"
"    df    - enable DCache fills\n"
"    if    - enable ICache fills\n"
"    nodf  - disable DCache fills\n"
"    noif  - disable ICache fills\n"
"    clear - clear all tests\n"
  )
};
