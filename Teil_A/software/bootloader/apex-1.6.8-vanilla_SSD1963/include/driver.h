/* driver.h

   written by Marc Singer
   1 Nov 2004

   Copyright (C) 2004 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__DRIVER_H__)
#    define   __DRIVER_H__

/* ----- Includes */

#include <linux/types.h>
#include <attributes.h>

/* ----- Types */

struct driver_d;

#if defined CONFIG_DRIVER_LONG_LONG_SIZE
 typedef int64_t  driver_off_t;
 typedef uint64_t driver_size_t;
 typedef int64_t  driver_ssize_t;
#else
 typedef int32_t  driver_off_t;
 typedef uint32_t driver_size_t;
 typedef int32_t  driver_ssize_t;
#endif

struct descriptor_d {
  struct driver_d* driver;
  char driver_name[32];		/* *** FIXME: should be removed */
  driver_size_t start;
  driver_size_t length;
  driver_size_t index;		/* From zero to length */
  int width;                    /* Request for witdh of access */

				/* Paths */
  char rgb[256];	   	/* *** FIXME: contributes to stack bloat */
  char* pb[32];
  int c;                        /* Total elements in path */
  int iRoot;                    /* Index of root path element */

  unsigned long private;	/* Available to driver */
};

#define DRIVER_LENGTH_MAX	(0x7fffffff)

#define SEEK_SET	0
#define SEEK_CUR	1
#define SEEK_END	2

#define QUERY_START	1	/* Physical address of device */
#define QUERY_SIZE	2	/* Total size of device */
#define QUERY_ERASEBLOCKSIZE 3	/* Erase block size at given index */

#define DRIVER_SERIAL	(1<<1)
#define DRIVER_CONSOLE	(1<<2)
#define DRIVER_MEMORY	(1<<3)
#define DRIVER_NET	(1<<4)			/* May receive packets */
#define DRIVER_PRESENT	(1<<8)
//#define DRIVER_DESCRIP_REGION	(1<<9)		/* Uses region descriptors */
#define DRIVER_DESCRIP_FS	(1<<10)		/* Uses filesystem descript. */
#define DRIVER_DESCRIP_STREAM	(1<<11)		/* Uses stream descriptors */
#define DRIVER_DESCRIP_NET	(1<<12)		/* Uses network descriptors */
#define DRIVER_DESCRIP_SIMPLEPATH (1<<13)	/* Uses unparsed paths */
#define DRIVER_WRITEPROGRESS_MASK (0xf)
#define DRIVER_WRITEPROGRESS_SHIFT (24)
#define DRIVER_WRITEPROGRESS(n) (((n)&0xf)<<24)	/* 2^(N+10) bytes per spin */
#define DRIVER_READPROGRESS_MASK (0xf)
#define DRIVER_READPROGRESS_SHIFT (28)
#define DRIVER_READPROGRESS(n) (((n)&0xf)<<28)	/* 2^(N+10) bytes per spin */
#define DRIVER_PRIVATE_SHIFT (16)
#define DRIVER_PRIVATE_MASK  (0xff)
#define DRIVER_PRIVATE(n) (((n)&0xff)<<16)

#define DRIVER_PROGRESS(din,dou) ({\
int i = ((din)->driver->flags >> DRIVER_READPROGRESS_SHIFT)\
	 &DRIVER_READPROGRESS_MASK;\
int o = ((dou)->driver->flags >> DRIVER_WRITEPROGRESS_SHIFT)\
	 &DRIVER_WRITEPROGRESS_MASK;\
(i > o) ? i : o; })

#define driver_can_seek(p)  ((p)->seek != NULL)
#define driver_can_read(p)  ((p)->read != NULL)
#define driver_can_write(p) ((p)->write != NULL)

#define descriptor_query(d,i,pv)\
	((d)->driver->query\
	 ? (d)->driver->query ((d),(i),(pv))\
	 : ERROR_UNSUPPORTED)

struct driver_d {
  const char* name;
  const char* description;
  unsigned long flags;
  void* priv;			/* Driver's private data */
  int		(*open)  (struct descriptor_d*);
  void		(*close) (struct descriptor_d*);
  ssize_t	(*read)  (struct descriptor_d*, void* pv, size_t cb);
  ssize_t	(*write) (struct descriptor_d*, const void* pv, size_t cb);
  ssize_t	(*poll)  (struct descriptor_d*, size_t cb);
  void		(*erase) (struct descriptor_d*, size_t cb);
//  size_t	(*seek)  (struct descriptor_d*, driver_off_t cb, int whence);
  driver_off_t	(*seek)  (struct descriptor_d*, driver_off_t cb, int whence);
  int		(*info)  (struct descriptor_d*);
  int		(*query) (struct descriptor_d*, int, void*);
  void		(*flush) (struct descriptor_d*);
};

#define __driver_0 __used __section(.driver.0) /* serial */
#define __driver_1 __used __section(.driver.1) /* memory */
#define __driver_2 __used __section(.driver.2)
#define __driver_3 __used __section(.driver.3) /* flash */
#define __driver_4 __used __section(.driver.4) /* ethernet */
#define __driver_5 __used __section(.driver.5) /* cf/mmc/sd */
#define __driver_6 __used __section(.driver.6) /* filesystems */
#define __driver_7 __used __section(.driver.7)

/* ----- Globals */

/* ----- Prototypes */

extern void   close_helper (struct descriptor_d* d);
extern void   close_descriptor (struct descriptor_d* d);
extern int    is_descriptor_open (struct descriptor_d* d);
extern int    open_descriptor (struct descriptor_d* d);
extern int    parse_descriptor (const char* sz, struct descriptor_d* d);
extern int    parse_descriptor_simple (const char* sz, unsigned long start,
				       unsigned long length,
				       struct descriptor_d* d);
//extern size_t seek_helper (struct descriptor_d* d, ssize_t ib, int whence);
extern driver_off_t seek_helper (struct descriptor_d* d, driver_off_t ib,
                                 int whence);

#endif  /* __DRIVER_H__ */
