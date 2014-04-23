/* cmd-karmatouch.c

   written by Marc Singer
   23 August 2007

   Copyright (C) 2007 Marc Singer

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
#include <command.h>
//#include <service.h>
#include <console.h>

#include "hardware.h"

//#define TALK
#if defined (TALK)
# define PRINTF(v...)	printf (v)
#else
# define PRINTF(v...)	do {} while (0)
#endif


#define GPIO_KBDCTL	__REG (GPIO_PHYS + 0x28)

#define GPIO_KBDCTL_HIGHZ     (0x2)
#define GPIO_KBDCTL_DRIVE(c)  (0x8 + (c))


/* cmd_scan

   watches the GPIO pins where buttons and sensors are attached.
   There is a matrix of 3x8 that uses the column drivers 0-2 that are
   read on PH, and a pair of buttons on driver 3 read on PA2 and PA3.
   The two shoulder buttons are on PF3 and PF4 and are active high.
   The magnetic game lid sensor is on PF6 and is active low.

   In the first spin of the board, the capacitive touch sensors were
   wired to emit a heartbeat signal.  This signal looks like a button
   press except that it is very short in duration.  The debounce code
   masks these false events.

*/

static int cmd_scan (int argc, const char** argv)
{
  int i;
  int rg[5] = { 0, 0, 0, 0, 0 };

  while (!console->poll (0, 0)) {
    for (i = 0; i < sizeof (rg)/sizeof (*rg); ++i) {
      int valuePrev = ~0;
      int value;
      while (1) {
	switch (i) {
	default:
	case 0:
	case 1:
	case 2:
	  GPIO_KBDCTL = GPIO_KBDCTL_DRIVE (i);
	  mdelay (1);
	  value = GPIO_PHD;
	  GPIO_KBDCTL = GPIO_KBDCTL_HIGHZ;
	  break;
	case 3:
	  GPIO_KBDCTL = GPIO_KBDCTL_DRIVE (i);
	  mdelay (1);
	  value = GPIO_PAD;
	  GPIO_KBDCTL = GPIO_KBDCTL_HIGHZ;
	  value = (value >> 2) & 0x3;
	  break;
	case 4:
	  value = GPIO_PFD;
	  value = 0
	    | ((value & (1<<3)) >> 3)
	    | ((value & (1<<4)) >> 2)
	    | ((value & (1<<6)) >> 5);
	  value ^= (1<<1);		/* Invert sense of magnetic sensor */
	  break;
	}
	if (value == valuePrev)
	  break;
	valuePrev = value;
	mdelay (10);
      }
      if (value != rg[i]) {
	int j = 0;
	int m;
	for (m = 1; m < 0x100; m <<= 1, ++j) {
	  if ((value & m) != (rg[i] & m)) {
	    switch (i) {
	    default:
	    case 0:
	    case 1:
	    case 2:
	      printf ("%c", 'a' + i + j*3 + ((value & m) ? 0 : 'A' - 'a'));
	      break;
	    case 3:
	      printf ("%c", 'y' + j       + ((value & m) ? 0 : 'A' - 'a'));
	      break;
	    case 4:
	      printf ("%c", '<' + j       + ((value & m) ? 0 : '[' - '<'));
	      break;
	    }
	  }
	}
	PRINTF ("%d: 0x%x\n", i, value);
	rg[i] = value;
      }
    }
  }

  return 0;
}

static __command struct command_d c_scan = {
  .command = "scan",
  .func = (command_func_t) cmd_scan,
  COMMAND_DESCRIPTION ("scan touchscreen")
  COMMAND_HELP(
"scan\n"
"  Scan the touchscreen.\n"
  )
};
