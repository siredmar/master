/* pci.c

   written by Marc Singer
   8 Mar 2005

   Copyright (C) 2005 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   This service module exists solely to initialize PCI.

*/

#include <config.h>
#include <service.h>
#include <apex.h>

#include "hardware.h"

/* pci_init

   performs essential boot-time PCI controller initializations.  This
   may not be necessary as the kernel (since 2.6.17 or so) has been
   doing this initialization as well.  Earlier kernels requires this
   code, so it stays in.  It isn't large, doesn't take much time, it
   it doesn't hurt.

*/

static void pci_init (void)
{
#if defined (CONFIG_MACH_NSLU2)

  /* Configure the PCI clock, reset the PCI devices, and configure the
     USB chip interrupt line */

#define PCI_RESET       (GPIO_OUT_CLEAR (GPIO_I_PCI_RESET))
#define PCI_NORESET     (GPIO_OUT_SET   (GPIO_I_PCI_RESET))
#define PCI_CLK_DISABLE (GPIO_CLKR &= ~GPIO_CLKR_MUX14)
#define PCI_CLK_ENABLE  (GPIO_CLKR |=  GPIO_CLKR_MUX14)
#define PCI_CLK_CONFIG\

  (GPIO_CLKR |= ((0xf << GPIO_CLKR_CLK0DC_SHIFT)\
		 | (0xf << GPIO_CLKR_CLK0TC_SHIFT)));

  PCI_RESET;
  PCI_CLK_DISABLE;
  PCI_CLK_CONFIG;

  GPIO_OUT_ENABLE  (GPIO_I_PCI_CLOCK);
  GPIO_OUT_ENABLE  (GPIO_I_PCI_RESET);
  GPIO_OUT_DISABLE (GPIO_I_PCI_INTA);
  GPIO_INT_TYPE	   (GPIO_I_PCI_INTA, GPIO_INT_TYPE_ACTIVELO);
  GPIO_OUT_DISABLE (GPIO_I_PCI_INTB);
  GPIO_INT_TYPE	   (GPIO_I_PCI_INTB, GPIO_INT_TYPE_ACTIVELO);
  GPIO_OUT_DISABLE (GPIO_I_PCI_INTC);
  GPIO_INT_TYPE	   (GPIO_I_PCI_INTC, GPIO_INT_TYPE_ACTIVELO);

  usleep (1000);		/* delay before clearing reset */
  PCI_CLK_ENABLE;
  usleep (100);
  PCI_NORESET;

#endif

	/* PCI initialization */
  PCI_PCIMEMBASE = 0x48494a4b;
  PCI_AHBMEMBASE = 0x00010203;

  PCI_CONFIG_WRITE32 (PCI_CFG_BAR_0, 0x00000000);
  PCI_CONFIG_WRITE32 (PCI_CFG_BAR_1, 0x01000000);
  PCI_CONFIG_WRITE32 (PCI_CFG_BAR_2, 0x02000000);
  PCI_CONFIG_WRITE32 (PCI_CFG_BAR_3, 0x03000000);
  PCI_CONFIG_WRITE32 (PCI_CFG_BAR_4, 0x80000000); /* Put these */
  PCI_CONFIG_WRITE32 (PCI_CFG_BAR_5, 0x90000000); /*  out of reach */

  PCI_ISR = PCI_ISR_PSE | PCI_ISR_PFE | PCI_ISR_PPE | PCI_ISR_AHBE;
  PCI_CSR = PCI_CSR_IC  | PCI_CSR_ABE
#if defined CONFIG_BIGENDIAN
    | PCI_CSR_PDS | PCI_CSR_ADS
#endif
    ;
  PCI_CONFIG_WRITE16 (PCI_CFG_COMMAND,
		      PCI_CFG_COMMAND_MASTER | PCI_CFG_COMMAND_MEMORY);

  //  printf ("pci did/vid 0x%lx\n", PCI_CONFIG_READ32 (PCI_CFG_DIDVID));
}

static __service_6 struct service_d pci_service = {
  .init = pci_init,
#if !defined (CONFIG_SMALL)
  //  .report = npe_report,
#endif
};
