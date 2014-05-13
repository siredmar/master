/* arch-arm.h

   written by Marc Singer
   25 Jul 2006

   Copyright (C) 2006 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__ARCH_ARM_H__)
#    define   __ARCH_ARM_H__

/* ----- Includes */

/* ----- Types */

/* ----- Globals */

/* ----- Prototypes */

extern void preinitialization (void);
extern void relocate_apex (void);
extern const char* cp15_ctrl (unsigned long);

#endif  /* __ARCH_ARM_H__ */
