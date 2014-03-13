/* reg.h

   written by Marc Singer
   11 Nov 2004

   Copyright (C) 2004 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   Macros to make register file access somewhat efficient in C.  The
   4K constant comes from the fact that ARM instructions are limited
   to a 4K immediate offset.

*/

#if !defined (__REG_H__)
#    define   __REG_H__

/* ----- Includes */

/* ----- Types */

/* ----- Globals */

/* ----- Prototypes */

typedef struct { volatile unsigned long  offset[4096>>2]; } __reg32;
typedef struct { volatile unsigned short offset[4096>>1]; } __reg16;
typedef struct { volatile unsigned char  offset[4096>>0]; } __reg08;

#define __REG_M(t,x,s) ((t*)((x)&~(4096-1)))->offset[((x)&(4096-1))>>s]

#define __REG(x)	__REG_M (__reg32, (x), 2)
#define __REG16(x)	__REG_M (__reg16, (x), 1)
#define __REG8(x)	__REG_M (__reg08, (x), 0)

#define MASK_AND_SET(v,m,s)	({ (v) = ((v) & ~(m)) | (s); })

#endif  /* __REG_H__ */
