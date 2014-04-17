/* variation-companion.c

   written by Marc Singer
   23 Jan 2007

   Copyright (C) 2007 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   Variation support for the ComSonics Companion.  There is a PSoC
   attached to UART3 that will emit '!' characters when the user holds
   the first, last, and center button on the keypad while powering the
   system on.  APEX detects this and selects the variation.

*/

#include <config.h>
#include <alias.h>
#include <service.h>
#include <apex.h>

#include "hardware.h"

#undef UART
#define UART			UART3_PHYS

#define UART_DATA		__REG(UART + 0x00)
#define UART_FCON		__REG(UART + 0x04)
#define UART_BRCON		__REG(UART + 0x08)
#define UART_CON		__REG(UART + 0x0c)
#define UART_STATUS		__REG(UART + 0x10)
#define UART_RAWISR		__REG(UART + 0x14)
#define UART_INTEN		__REG(UART + 0x18)
#define UART_ISR		__REG(UART + 0x1c)

static void variation_init (void)
{
	/* Initialize */
  GPIO_PINMUX |= 1<<3;		/* Enable UART3 lines */
  UART_BRCON = 0x3;		/* 115200 */
  UART_FCON = UART_FCON_FEN | UART_FCON_WLEN8;
  UART_INTEN = 0x00; /* Mask interrupts */
  UART_CON = UART_CON_ENABLE;

	/* Flush FIFO */
  while (!(UART_STATUS & UART_STATUS_RXFE))
      UART_DATA;

  msleep (100);

  if (!(UART_STATUS & UART_STATUS_RXFE)) {
    char c = UART_DATA & UART_DATA_DATAMASK;
    if (c == '!')
      alias_set ("variation", CONFIG_VARIATION_SUFFIX);
  }
}

static __service_9 struct service_d variation_service = {
  .init = variation_init,
};
