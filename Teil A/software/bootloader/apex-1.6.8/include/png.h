/* png.h

   written by Marc Singer
   3 Mar 2005

   Copyright (C) 2005 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__PNG_H__)
#    define   __PNG_H__

/* ----- Includes */

#include <driver.h>

/* ----- Types */

struct png_header {
  int width;
  int height;
  unsigned char bit_depth;
  unsigned char color_type;
  unsigned char compression;
  unsigned char filter;
  unsigned char interleave;
} __attribute__((packed));

/* ----- Globals */

/* ----- Prototypes */

//void*		     open_png (const void* pv, size_t cb);
//int		     read_png_ihdr (void* pv, struct png_header* hdr);
//const unsigned char* read_png_row (void* pv);
//void		     close_png (void* pv);

void*		     open_pngr (struct descriptor_d* d);
int		     read_pngr_ihdr (void* pv, struct png_header* hdr);
const unsigned char* read_pngr_row (void* pv);
void		     close_pngr (void* pv);
int		     palette_pngr (void* pv, unsigned char** prgPalette);

#endif  /* __PNG_H__ */
