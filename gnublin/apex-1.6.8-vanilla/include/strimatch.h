/* string.h

   written by Marc Singer
   4 Aug 2008

   Copyright (C) 2008 Marc Singer

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__STRING_H__)
#    define   __STRING_H__

/* ----- Includes */

#include <linux/string.h>

/* ----- Types */

/* ----- Globals */

/* ----- Prototypes */

extern int strimatch (const char*, const char*, __kernel_size_t);

#endif  /* __STRING_H__ */
