/* region-copy.h

   written by Marc Singer
   22 Aug 2008

   Copyright (C) 2008 Marc Singer

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__REGION_COPY_H__)
#    define   __REGION_COPY_H__

/* ----- Includes */

/* ----- Types */

/* ----- Globals */

/* ----- Prototypes */

enum {
  regionCopySpinner	= (1<<0),
  regionCopySwap	= (1<<1),
  regionCopyVerify	= (1<<2),
  regionCopyQuiet	= (1<<3),
};

int region_copy (struct descriptor_d* dout, struct descriptor_d* din,
                 unsigned flags);

#endif  /* __REGION_COPY_H__ */
