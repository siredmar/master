/* variation-nslu2.c

   written by Marc Singer
   6 Jan 2007

   Copyright (C) 2007 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   Service to support variable variations on the nslu2.  Essetially,
   this is the interface with the user to select the variation at
   boot-time.

*/

#include <config.h>
#include <alias.h>
#include <service.h>
#include <apex.h>

#include "hardware.h"

static void variation_init (void)
{
  int button_pressed = (GPIO_INR & (1<<GPIO_I_POWERBUTTON)) != 0;

  if (button_pressed)
    alias_set ("variation", CONFIG_VARIATION_SUFFIX);

  MASK_AND_SET (GPIO_OUTR, 1<<GPIO_I_LEDSTATUS,
		button_pressed ? (1<<GPIO_I_LEDSTATUS): 0);
}

static __service_9 struct service_d variation_service = {
  .init = variation_init,
};
