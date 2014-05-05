/* apex.h

   written by Marc Singer
   3 Nov 2004

   Copyright (C) 2004 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__INIT_H__)
#    define   __INIT_H__

/* ----- Includes */

#include <stdarg.h>
#include <linux/types.h>
#include <config.h>

/* ----- Constants */

#if defined (CONFIG_SMALL)
# define C_ARG_MAX  (20)	/* Maximum number of command line word */
# define CB_COMMAND_MAX (80*2)
# define CB_PRINTF_MAX (80*2)
#else
# define C_ARG_MAX  (64)	/* Maximum number of command line word */
# define CB_COMMAND_MAX (1024)
# define CB_PRINTF_MAX (2*1024)
#endif

/* ----- Types */

/* ----- Globals */

/* ----- Prototypes */

#if !defined (barrier)
# define barrier() __asm volatile ("":::"memory")
#endif

extern void init_drivers (void);
extern int __attribute__((format (printf, 1, 2)))
     printf (const char * fmt, ...);
extern int putchar (int ch);
extern int puts (const char * fmt);
extern int snprintf(char * buf, size_t size, const char * fmt, ...);
extern int sprintf(char * buf, const char * fmt, ...);
extern int read_command (const char* szPrompt,
			 int* pargc, const char*** pargv);
extern int parse_command (char* rgb, int* pargc, const char*** pargv);
extern int vsnprintf(char *buf, size_t size, const char *fmt, va_list args);

extern void __attribute__((noreturn)) exec_monitor (void);

extern unsigned long timer_read (void);
extern unsigned long timer_delta (unsigned long, unsigned long);
extern void usleep (unsigned long);
#define udelay usleep		/* Just for convenience */
static inline void msleep (int c) {	/* Only way to guarantee the range */
  while (c-- > 0) udelay (1000); }
#define mdelay msleep


#define dump(p,c,i) dumpw(p,c,i,0)
extern void dumpw (const char* rgb, int cb,
		   unsigned long index, int width);

#if defined (CONFIG_COMMAND_HISTORY)
void history_add (const char* sz);
int history_retrieve (int backward, char* rgb, size_t cbMax);
#endif

#endif  /* __INIT_H__ */
