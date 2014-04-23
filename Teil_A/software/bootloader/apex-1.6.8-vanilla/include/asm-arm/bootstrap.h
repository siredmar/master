/* bootstrap.h
     $Id$

   written by Marc Singer
   9 Nov 2004

   Copyright (C) 2004 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__BOOTSTRAP_H__)
#    define   __BOOTSTRAP_H__

/* ----- Includes */

#include <attributes.h>

/* ----- Types */

/* ----- Globals */

extern char APEX_VMA_ENTRY;
extern char APEX_VMA_START;
extern char APEX_VMA_VECTOR_START;
extern char APEX_VMA_VECTOR_END;
extern char APEX_VMA_COPY_START;
extern char APEX_VMA_COPY_END;
extern char APEX_VMA_STACKS_START;
extern char APEX_VMA_STACKS_END;
extern char APEX_VMA_STACK_START;
extern char APEX_VMA_IRQSTACK_START;
extern char APEX_VMA_BSS_START;
extern char APEX_VMA_BSS_END;
extern char APEX_VMA_BOOTSTRAP_STACK_START;

/* ----- Prototypes */


#endif  /* __BOOTSTRAP_H__ */
