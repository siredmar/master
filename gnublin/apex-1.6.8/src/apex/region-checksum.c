/* region-checksum.c

   written by Marc Singer
   22 Aug 2008

   Copyright (C) 2008 Marc Singer

   -----------
   DESCRIPTION
   -----------

*/

//#define TALK 2

#include <config.h>
#include <apex.h>
#include <driver.h>
#include <error.h>
#include <spinner.h>
#include "region-checksum.h"
#include <talk.h>

extern unsigned long compute_crc32 (unsigned long crc, const void *pv, int cb);
extern unsigned long compute_crc32_lsb (unsigned long crc,
                                        const void *pv, int cb);

/** Compute the CRC32 checksum for a region.  The flags includes a bit
    to add each byte of the length, in MSB (or LSB) order, as is used
    by the POSIX cksum command.  If the cbCheck parameter is non-zero,
    it will limit the extent of the check to that number of bytes,
    otherwise the whole region will be checked. Note that the crc_result

*/

int region_checksum (size_t cbCheck, struct descriptor_d* d, unsigned flags,
                     unsigned long* crc)
{
  ssize_t extent = d->length - d->index;
  int index = 0;

  if (cbCheck && extent > cbCheck)
    extent = cbCheck;

  DBG (2, "%s: av %d\n", __FUNCTION__, cbCheck);

  while (index < extent) {
    char __aligned rgb[512];
    size_t available = sizeof (rgb);
    int cb;

    if (available > extent - index)
      available = extent - index;
    cb = d->driver->read (d, rgb, available);
    if (cb < 0)
      return ERROR_IOFAILURE;
    if (flags & regionChecksumSpinner)
      SPINNER_STEP;
#if defined (CONFIG_CRC32_LSB)
    if (flags & regionChecksumLSB)
      *crc = compute_crc32_lsb (*crc, rgb, cb);
    else
#endif
      *crc = compute_crc32 (*crc, rgb, cb);
    index += cb;
  }

  /* Add the length to the computation */
  if (flags & regionChecksumLength) {
    unsigned char b;
    unsigned long v;
    for (v = extent; v; v >>= 8) {
      b = v & 0xff;
      *crc = compute_crc32 (*crc, &b, 1);
    }
  }

  return 0;
}
