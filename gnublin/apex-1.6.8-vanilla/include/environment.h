/* environment.h

   written by Marc Singer
   7 Nov 2004

   Copyright (C) 2004 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__ENVIRONMENT_H__)
#    define   __ENVIRONMENT_H__

#include <attributes.h>

/* ----- Types */

#if defined (CONFIG_ENV_REGION)
extern struct descriptor_d d_env;
extern struct descriptor_d d_envmem;
extern struct descriptor_d* pd_env;
#endif

#if defined (CONFIG_ENV_SIZE)
extern char __xbss(env) g_rgbEnv[CONFIG_ENV_SIZE];
#endif

struct env_d {
  char* key;
  char* default_value;
  char* description;
};

struct env_link_1 {		/* Legacy */
  unsigned long magic;
  void* apex_start;
  void* apex_end;
  void* env_start;
  void* env_end;
  unsigned long env_d_size;
  const char region[];
};

struct env_link {
  unsigned long magic;
  const char apexversion[16];
  void* env_link;		/* Align loader within flash */
  void* apex_start;
  void* apex_end;
  void* env_start;
  void* env_end;
  unsigned long env_d_size;
  const char region[];
};

#define __env  __used __section(.env)

#define ENV_CB_MAX	(512)

#define ENV_MAGIC_0	'A'
#define ENV_MAGIC_1	'e'
#define ENV_LINK_MAGIC	(unsigned long)(('A'<<0)|('E'<<8)|('L'<<16)|('2'<<24))
#define ENV_LINK_MAGIC_1 (unsigned long)(('A'<<0)|('E'<<8)|('L'<<16)|('1'<<24))

/* ----- Prototypes */

const char* env_fetch (const char* szKey);
int	    env_fetch_int (const char* szKey, int valueDefault);
int	    env_store (const char* szKey, const char* szValue);
void	    env_erase (const char* szKey);
void*	    env_enumerate (void* pv, const char** pszKey,
			   const char** pszValue, int* fDefault);
void        env_erase_all (void);
int	    env_check_magic (int);


#endif  /* __ENVIRONMENT_H__ */
