/* interrupts.h
     $Id$

   written by Marc Singer
   21 Jan 2005

   Copyright (C) 2005 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   Support for interrupt handlers.

*/

#if !defined (__INTERRUPTS_H__)
#    define   __INTERRUPTS_H__

/* ----- Includes */

/* ----- Types */

enum {
  IRQ_NONE = 0,
  IRQ_HANDLED = 1,
};

typedef int irq_return_t;

/* ----- Globals */

extern irq_return_t (*interrupt_handlers[])(int irq);

/* ----- Prototypes */



#endif  /* __INTERRUPTS_H__ */
