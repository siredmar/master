/* spinner-nslu2.c

   written by Marc Singer
   11 Feb 2005

   Copyright (C) 2005 Marc Singer

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

#include "hardware.h"

#include <spinner.h>

void nslu2_spinner (unsigned v)
{
  static int step;
  static int value;

  if (v == ~0) {		/* Clear */
    value = 0;
    step = 0;
    _LG(LED0);
    return;
  }

  v = v/128;
  if (v == value)
    return;

  value = v;
  switch (step) {
  case 0:
    _LG(LED1); break;
  case 1:
    _LG(LED5); break;
  case 2:
    _LG(LEDd); break;
  case 3:
    _LG(LEDc); break;
  case 4:
    _LG(LED8); break;
  case 5:
    _LG(LED0); break;
  }
  //  step = (step + 1)%6;
  ++step;
  if (step > 5)
    step = 0;
}

static void spinner_init (void)
{
  hook_spinner = nslu2_spinner;
  _LG(LED0);			/* All off once we've init'd */
}

static void spinner_release (void)
{
  _LG(LEDf);			/* All on while kernel decompresses */
}

static __service_7 struct service_d spinner_service = {
  .init    = spinner_init,
  .release = spinner_release,
};
