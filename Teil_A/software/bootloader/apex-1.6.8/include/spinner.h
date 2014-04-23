/* spinner.h

   written by Marc Singer
   8 Nov 2004

   Copyright (C) 2004 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__SPINNER_H__)
#    define   __SPINNER_H__

/* ----- Includes */

/* ----- Types */

/* ----- Globals */

/* ----- Prototypes */

#if defined (CONFIG_SPINNER)
# define SPINNER_STEP spinner_step()
# define SPINNER_CLEAR spinner_clear()
#else
# define SPINNER_STEP
# define SPINNER_CLEAR
#endif

extern void (*hook_spinner) (unsigned value);

void spinner_step (void);
void spinner_clear (void);

#endif  /* __SPINNER_H__ */
