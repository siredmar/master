/** @file talk.h

    written by Marc Singer
    10 Jun 2008

    Copyright (C) 2008 Marc Singer

    @brief Debug macros.

*/

#if !defined (__TALK_H__)
#    define   __TALK_H__

/* ----- Includes */

#include <iostream>
#include <fstream>

/* ----- Types */

/* ----- Globals */

/* ----- Prototypes */

extern std::ofstream cnull;

#if defined (TALK) && TALK > 0
#define PRINTF(f...)	printf (f)
#define ENTRY(l)	do { if (TALK >= l) printf ("%s\n", __FUNCTION__); }\
			while (0)
#define EXIT(l)		do { if (TALK >= l) printf ("%s!\n", __FUNCTION__); }\
			while (0)
#define DBG(l,f...)	do { if (TALK >= l) printf (f); } while (0)
#define DBGP(l,s,f...)	do { if (TALK >= l)\
			      printf ("%s: " s "%s",__FUNCTION__,f,""); }\
			while (0)
#define COUT(l)		((TALK >= l) ? std::cout : cnull)
#else
#define PRINTF(f...)	do {} while (0)
#define ENTRY(l)	do {} while (0)
#define EXIT(l)		do {} while (0)
#define DBG(l,f...)	do {} while (0)
#define DBGP(l,f...)	do {} while (0)
#define COUT(...)	cnull
#endif

#endif  /* __TALK_H__ */
