/* arp.c

   written by Marc Singer
   7 Jul 2005

   Copyright (C) 2005 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   ARP command for testing.

*/

#include <config.h>
#include <linux/string.h>
#include <linux/types.h>
//#include <linux/ctype.h>
#include <apex.h>
#include <command.h>
#include <driver.h>
#include <error.h>
//#include <spinner.h>
#include <console.h>

#include <network.h>
#include <ethernet.h>

//#define TALK

#if defined (TALK) && TALK > 0
# define DBG(l,f...)		if (l <= TALK) printf (f);
#else
# define DBG(l,f...)		do {} while (0)
#endif

//extern void eth_diag (int);

int console_terminate (void* pv)
{
//  char ch;

  if (console->poll (0, 0))
    return 1;
  return 0;
}

int cmd_arp (int argc, const char** argv)
{
  struct descriptor_d d;
  int result;

  if (   (result = parse_descriptor (szNetDriver, &d))
      || (result = open_descriptor (&d)))
    return result;

  DBG (2,"%s: open %s -> %d\n", __FUNCTION__, szNetDriver, result);

  result = ethernet_service (&d, console_terminate, NULL);

  close_descriptor (&d);
  return result ? ERROR_BREAK : 0;
}

static __command struct command_d c_arp = {
  .command = "arp",
  .description = "test arp protocol",
  .func = cmd_arp,
  COMMAND_HELP(
"arp\n"
"  Enter a loop to handle arp requests.\n"
  )
};
