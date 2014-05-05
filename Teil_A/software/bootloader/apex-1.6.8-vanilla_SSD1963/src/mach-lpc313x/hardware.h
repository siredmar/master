/* hardware.h

   written by Durgesh Pattamatta
   10 Oct 2008

   Copyright (C) 2008 NXP Semiconductors

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__HARDWARE_H__)
#    define   __HARDWARE_H__

/* ----- Includes */

#include <config.h>

#include "lpc313x.h"



/* ----- Types */
#define ANALOG_3V3_RAIL    0
#define ANALOG_1V2_RAIL    1
#define ANALOG_1V8_RAIL    2

/* ----- Globals */

/* ----- Prototypes */

extern void nandsys_init(void);
void setcorevoltage(unsigned long mv);

extern void analog_reg_init(void);
extern void analog_reg_read(u32 reg_addr, u32 * pReg_value);
extern void analog_reg_write(u32 reg_addr, u32 reg_value);
extern void psu_set_voltage(u32 rail, u32 volt);


#endif  /* __HARDWARE_H__ */
