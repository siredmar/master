/** @file oprintf.h

    written by Marc Singer
    30 Aug 2007

    Copyright (C) 2007 Marc Singer

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    version 2 as published by the Free Software Foundation.  Please
    refer to the file debian/copyright for further details.

*/

#if !defined (__OPRINTF_H__)
#    define   __OPRINTF_H__

/* ----- Includes */

#include <stdarg.h>
#include <iostream>

/* ----- Types */

struct oprintf {
  char sz[512];
  oprintf (const char* szFormat, ...) {
    va_list ap;
    va_start (ap, szFormat);
    vsnprintf (sz, sizeof (sz), szFormat, ap);
    va_end (ap); }
  friend std::ostream& operator<< (std::ostream& o, const oprintf& op) {
    return o << op.sz; }
};

/* ----- Globals */

/* ----- Prototypes */



#endif  /* __OPRINTF_H__ */
