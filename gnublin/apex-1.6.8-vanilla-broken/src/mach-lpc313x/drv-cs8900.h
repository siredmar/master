/*  Cirrus Logic CS8900A Ethernet

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------
   This code is derived from u-boot CS8900 driver code.
   
 */
#if !defined (__DRV_CS8900_H__)
#    define   __DRV_CS8900_H__

#include <driver.h>
#include <service.h>
#include <apex.h>		/* printf, only */
#include "lpc313x.h"		


#define CS8900_BASE     (EXT_SRAM1_PHYS + 0x10000)

	 /* Use the second static bank, with fast timings, as an IOBARRIER */
#define CS8900_IOBARRIER	(*(volatile unsigned char __force*) 0x60000000)

#define CS8900_IO_OVERRIDE         /* We have a platform implementation */

static inline u16 CS_REG16 (u32 reg)
{
  u16 v;
  CS8900_IOBARRIER;
  v = __REG16(reg);
  CS8900_IOBARRIER;
  return v;
}


#endif /*__DRV_CS8900_H__*/
