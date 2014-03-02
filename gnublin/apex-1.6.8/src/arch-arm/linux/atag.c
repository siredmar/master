/* atag.c

   written by Marc Singer
   6 Nov 2004

   Copyright (C) 2004 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#include <atag.h>
#include <linux/string.h>
#include <apex.h>
#include <config.h>

void build_atags (void)
{
	extern char APEX_ATAG_START;
	extern char APEX_ATAG_END;

	struct atag_d* d;
	struct tag* p = (struct tag*) CONFIG_ATAG_PHYS;

	for (d = (struct atag_d*) &APEX_ATAG_START;
	     d < (struct atag_d*) &APEX_ATAG_END;
	     ++d)
		p = d->append_atag (p);
}

/* atag_header

   builds the tag list header which is a CORE tag.  We don't presently
   have any need to set the fields in the CORE tag, so we pass a null
   structure with the tag.

*/

struct tag* atag_header (struct tag* p)
{
	p->hdr.tag = ATAG_CORE;
	p->hdr.size = 2;	/* As documented. */
//	p->hdr.size = tag_size (tag_core);
//	memzero (&p->u.core, sizeof (p->u.core));

# if !defined (CONFIG_SMALL)
	printf ("ATAG_HEADER\n");
# endif

	return tag_next (p);
}

struct tag* atag_end (struct tag* p)
{
	p->hdr.tag = ATAG_NONE;
	p->hdr.size = 0;

# if !defined (CONFIG_SMALL)
	printf ("ATAG_END\n");
# endif

	return tag_next (p);
}


static __atag_0 struct atag_d _atag_header = { atag_header };
static __atag_7 struct atag_d _atag_end    = { atag_end };
