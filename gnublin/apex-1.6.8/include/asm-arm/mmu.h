/* mmu.h

   written by Marc Singer
   25 Feb 2007

   Copyright (C) 2007 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__MMU_H__)
#    define   __MMU_H__

/* ----- Includes */

/* ----- Types */

/* ----- Globals */

/* ----- Prototypes */

void* alloc_uncached (size_t cb, size_t alignment);
void* alloc_uncached_top_retain (size_t cb, size_t alignment);
void mmu_protsegment (void* pv, int cacheable, int bufferable);
void mmu_map_segment (void* physical, void* virtual,
                      int cacheable, int bufferable);

#endif  /* __MMU_H__ */
