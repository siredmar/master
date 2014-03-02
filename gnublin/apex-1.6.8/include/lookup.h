/* lookup.h

   written by Marc Singer
   20 Jul 2006

   Copyright (C) 2006 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__LOOKUP_H__)
#    define   __LOOKUP_H__

/* ----- Includes */

/* ----- Types */

/* ----- Globals */

/* ----- Prototypes */

#if defined (CONFIG_ALIASES) || defined (CONFIG_ENV)

int lookup_alias_or_env_int (const char* szKey, int valueDefault);
unsigned lookup_alias_or_env_unsigned (const char* szKey,
                                       unsigned valueDefault);
const char* lookup_alias_or_env (const char* szKey, const char* szDefault);

#else

inline int lookup_alias_or_env_int (const char* szKey, int valueDefault) {
  return valueDefault; }
inline unsigned lookup_alias_or_env_unsigned (const char* szKey,
                                              unsigned valueDefault) {
  return valueDefault; }
inline const char* lookup_alias_or_env (const char* szKey,
                                        const char* szDefault) {
  return szDefault; }

#endif



#endif  /* __LOOKUP_H__ */
