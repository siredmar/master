/* variation-fsg3.c

   written by Rod Whitby
   2 Jan 2008

   based on variation-nslu2.c
   written by Marc Singer
   6 Jan 2007

   Copyright (C) 2007 Marc Singer
   Copyright (C) 2008 Rod Whitby

   -----------
   DESCRIPTION
   -----------

   Service to support variable variations on the fsg3.  Essetially,
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
  int button_pressed = (GPIO_INR & (1<<GPIO_I_SYNCBUTTON)) == 0;

  if (button_pressed)
    alias_set ("variation", CONFIG_VARIATION_SUFFIX);
}

static __service_9 struct service_d variation_service = {
  .init = variation_init,
};
