/* talk.h

   written by Marc Singer
   10 Sep 2008

   Copyright (C) 2008 Marc Singer

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__TALK_H__)
#    define   __TALK_H__

/* ----- Includes */

/* ----- Types */

/* ----- Globals */

/* ----- Prototypes */

#if defined (TALK)
#define PRINTF(f...)    printf (f)
#define ENTRY(l)        do { if (TALK >= l) printf ("%s\n", __FUNCTION__); }\
                        while (0)
#define EXIT(l)         do { if (TALK >= l) printf ("%s!\n", __FUNCTION__); }\
                        while (0)
#define DBG(l,f...)     do { if (TALK >= l) printf (f); } while (0)
#define DBGP(l,s,f...)  do { if (TALK >= l)\
                              printf ("%s: " s "%s",__FUNCTION__,f,""); }\
                        while (0)
#else
#define PRINTF(f...)    do {} while (0)
#define ENTRY(l)        do {} while (0)
#define EXIT(l)         do {} while (0)
#define DBG(l,f...)     do {} while (0)
#define DBGP(l,f...)    do {} while (0)
#endif

#endif  /* __TALK_H__ */
