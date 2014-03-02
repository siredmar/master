/* cmd-image-apex.c

   written by Marc Singer
   12 Sep 2008

   Copyright (C) 2008 Marc Singer

   -----------
   DESCRIPTION
   -----------

   o rgbHeader.  We copy the header from the file to a buffer because
     this allows us to verify and load the header in one procedure and
     then process the image in another without requiring malloc().
     The intent is to be able to work with images as a stream and not
     require that the source media support seeking.  The limit on the
     size of the header derives from the fact that the header size
     field is 8 bits and it represents a count of 16 byte pieces.
     This 2^8 * 16 or 4KiB.

   o The source region may not specify a length.  It is useful to
     allow passing of a start address for a region without a length.
     In this case, the image handling code timidly extends the region
     to account for the bytes it knows should exist.  It is the
     responsibility of the underlying drivers to cope with an invalid
     length, especially because this length could be set after the
     descriptor is opened.

   o Interruptability.  Once we start to read the payloads, this
     command must be interuptable.  This hasn't been done since
     current tests show that payloads are processed faster than the
     user can type ^C.

   o The check routine needs to be able to scan all of the payloads
     and return a CRC error if one failed.  Currently, it stops at the
     first one that fails.

   o APEX image handlers for load and check do their work on parsing
     the payload length field.  This marker must be the last payload
     marker in the header for each payload.  Thus, when we know the
     lenght of the payload, we also know everything else about it.

   o Overlapping source and destination regions.  There is no support
     in this image handling code to cope with an image that overlaps
     the place in memory where a payload is to be copied.  For the
     most part, this shouldn't be necessary since APEX images can be
     processed as a stream.  In the event that this does come into
     play, we may need to relocate the source image before starting
     the copy. This isn't so simple since we would have to make a map
     of the memory to be used that would have to be more than a simple
     range.  Imagine loading one piece of data at the start of RAM and
     another at the top.  We'd have to be able to relocate to the
     middle.

*/

#undef TALK
//#define TALK 2

#include <config.h>
#include <linux/types.h>
#include <linux/string.h>
#include <apex.h>
#include <command.h>
#include <error.h>
#include <linux/kernel.h>
#include <environment.h>
//#include <service.h>
#include <alias.h>
#include <lookup.h>
#include <driver.h>
#include "region-copy.h"
#include "region-checksum.h"
#include <simple-time.h>
#include "cmd-image.h"
#include <talk.h>
#include <describe.h>

#include <debug_ll.h>

extern unsigned long compute_crc32 (unsigned long crc, const void *pv, int cb);
extern uint32_t compute_crc32_length (uint32_t crc, size_t cb);

static const uint8_t signature[] = { 0x41, 0x69, 0x30, 0xb9 };
static char __xbss(image) g_rgbHeader[(1<<8)*16]; /* Largest possible header */
static size_t g_cbHeader;

enum {
  sizeZero      = 0x3,          // 11b
  sizeOne       = 0x2,          // 10b
  sizeFour      = 0x1,          // 01b
  sizeVariable  = 0x0,          // 00b
};

enum {
  fieldImageDescription                 = 0x10 | sizeVariable,
  fieldImageCreationDate                = 0x14 | sizeFour,
  fieldPayloadLength                    = 0x30 | sizeFour,
  fieldPayloadLoadAddress               = 0x34 | sizeFour,
  fieldPayloadEntryPoint                = 0x38 | sizeFour,
  fieldPayloadType                      = 0x3c | sizeOne,
  fieldPayloadDescription               = 0x40 | sizeVariable,
  fieldPayloadCompression               = 0x44 | sizeZero,
  fieldLinuxKernelArchitectureID        = 0x80 | sizeFour,
  fieldNOP                              = 0xfc | sizeZero,
};

enum {
  typeNULL = 0,
  typeLinuxKernel = 1,
  typeLinuxInitrd = 2,
};

struct payload_info {
  uint32_t v;
  char* szField;

  int type;
  uint32_t addrLoad;
  uint32_t addrEntry;
  size_t length;
  const char* sz;
};

static void clear_info (struct payload_info* info)
{
  info->type      =  0;
  info->addrLoad  = ~0;
  info->addrEntry = ~0;
  info->length    =  0;
  info->sz        =  NULL;
}

static inline uint32_t swabl (uint32_t v)
{
  return 0
    | ((v & (uint32_t) (0xff <<  0)) << 24)
    | ((v & (uint32_t) (0xff <<  8)) <<  8)
    | ((v & (uint32_t) (0xff << 16)) >>  8)
    | ((v & (uint32_t) (0xff << 24)) >> 24);
}

bool tag_lengths (uint8_t* pb, int* pcbTag, int* pcbData)
{
  uint8_t tag = *pb;
  *pcbTag = 1;
  *pcbData = 0;

  DBG (2, "tag %x  size %x", tag, tag & 3);
  switch (tag & 3) {
  case sizeZero:	*pcbData = 0; break;
  case sizeOne:		*pcbData = 1; break;
  case sizeFour:	*pcbData = 4; break;
  case sizeVariable:
    DBG (2," (variable)");
    *pcbData = pb[1];
    if (*pcbData > 127)
      return false;
    ++*pcbTag;
    break;
  }
  DBG (2, "  cbTag %d cbData %d\n", *pcbTag, *pcbData);
  return true;
}

int verify_apex_image (struct descriptor_d* d, struct image_info* im_info)
{
  int result = 0;
  ssize_t cbNeed;               /* Most definitely can be negative */
  size_t cbHeader;
  uint32_t crc = 0;

  ENTRY (0);

  cbNeed = 5 - g_cbHeader;
  if (cbNeed > 0) {
    if (im_info->fRegionCanExpand && d->length - d->index < cbNeed)
      d->length = d->index + cbNeed;
	/* Read the signature and header length if not already present */
    result = d->driver->read (d, g_rgbHeader + g_cbHeader, cbNeed);
    if (result != cbNeed)
      ERROR_RETURN (ERROR_IOFAILURE, "signature read error");
    g_cbHeader += cbNeed;
  }
  if (memcmp (g_rgbHeader, signature, sizeof (signature)))
    ERROR_RETURN (ERROR_FAILURE, "invalid signature");
  cbHeader = (unsigned char) g_rgbHeader[4];
  cbHeader *= 16;
  if (!cbHeader)
    ERROR_RETURN (ERROR_FAILURE, "invalid header length");
  if (cbHeader > sizeof (g_rgbHeader))
    ERROR_RETURN (ERROR_FAILURE, "impossible header length");

  cbNeed = cbHeader - g_cbHeader;
  if (cbNeed > 0) {
    if (im_info->fRegionCanExpand && d->length - d->index < cbNeed)
      d->length = d->index + cbNeed;
    result = d->driver->read (d, g_rgbHeader + g_cbHeader, cbNeed);
    if (result != cbNeed)
      ERROR_RETURN (ERROR_IOFAILURE, "header read error");
    g_cbHeader += cbNeed;
  }

  crc = compute_crc32 (0, g_rgbHeader, g_cbHeader);
  if (crc != 0)
    ERROR_RETURN (ERROR_CRCFAILURE, "broken header CRC");

  return result;
}

const char* describe_apex_image_type (unsigned long v)
{
  switch (v) {
  default:
  case typeNULL:	return "undefined-image-type";		break;
  case typeLinuxKernel:	return "Kernel";			break;
  case typeLinuxInitrd:	return "Initrd";			break;
  }
};

#if defined (CONFIG_CMD_IMAGE_SHOW)

/** Handle reporting on APEX image and payloads. */

int handle_report_apex_image (int field,
                              struct descriptor_d* d,
                              struct image_info* im_info,
                              struct payload_info* info)
{
  switch (field) {
  case fieldImageDescription:
    printf ("Image Description:       '%s' (%d bytes)\n", info->szField,
            strlen (info->szField) + 1);
    break;
  case fieldImageCreationDate:
    {
      struct tm tm;
      time_t t = info->v;
      char sz[32];
      asctime_r (gmtime_r (&t, &tm), sz);
      sz[24] = 0;             /* Knock out the newline */
      printf ("Image Creation Date:     %s UTC (0x%x %d)\n",
              sz, info->v, info->v);
    }
    break;
  case fieldPayloadLength:
    printf ("Payload Length:          %s\n", describe_size (info->v));
    break;
  case fieldPayloadLoadAddress:
    printf ("Payload Load Address:    0x%08x\n", info->v);
    break;
  case fieldPayloadEntryPoint:
    printf ("Payload Entry Point:     0x%08x\n", info->v);
    break;
  case fieldPayloadType:
    printf ("Payload Type:            %s\n",
            describe_apex_image_type (info->v));
    break;
  case fieldPayloadDescription:
    printf ("Payload Description:    '%s' (%d bytes)\n", info->szField,
            strlen (info->szField) + 1);
    break;
//    case fieldPayloadCompression:
//      printf ("Payload Compression:    gzip\n");
//      break;
  case fieldLinuxKernelArchitectureID:
    printf ("Linux Kernel Arch ID:    %d (0x%x)\n", info->v, info->v);
    break;
  default:
    break;                    // It's OK to skip unknown tags
  }
  return 0;
}

#endif


/** Handle loading of APEX image payloads.  It loads the payload into
    memory at the load address.  For kernel payloads, it sets
    environment variables for the entry point and Linux kernel
    architecture ID.  For initrd payloads it sets the environment
    variables for the start address and size of the initrd.  Also, if
    an image has no load address, the payload will be loaded to the
    default address built into APEX. */

int handle_load_apex_image (int field,
                            struct descriptor_d* d, struct image_info* im_info,
                            struct payload_info* info)
{
  int result = 0;
  struct descriptor_d dout;
  unsigned long crc;
  unsigned long crc_calc = 0;
  ssize_t cbPadding = 16 - ((info->length + sizeof (crc)) & 0xf);

  switch (field) {
  case fieldImageDescription:
    printf ("Image '%s'\n", info->szField);
    break;
  case fieldPayloadLength:
    printf ("# %s mem:0x%08x+0x%08x %s\n",
            describe_apex_image_type (info->type), info->addrLoad,
            info->length, info->sz ? info->sz : "");

    if (info->addrLoad == ~0)
      ERROR_RETURN (ERROR_FAILURE, "no load address for payload");

    /* Copy image and check CRC */
    if (im_info->fRegionCanExpand
        && d->length - d->index < info->length + 4 + cbPadding)
      d->length = d->index + info->length + 4 + cbPadding;

    parse_descriptor_simple ("memory", info->addrLoad, info->length, &dout);
    result = region_copy (&dout, d, regionCopySpinner);
    if (result >= 0) {
      parse_descriptor_simple ("memory", info->addrLoad, info->length, &dout);
      result = region_checksum (0, &dout,
                                regionChecksumSpinner | regionChecksumLength,
                                &crc_calc);
    }
    printf ("\r");
    if (result < 0)
      return result;
    if (d->driver->read (d, &crc, sizeof (crc)) != sizeof (crc))
      ERROR_RETURN (ERROR_IOFAILURE, "payload CRC missing");
    crc = swabl (crc);
    if (crc != ~crc_calc) {
      DBG (1, "crc 0x%08lx  crc_calc 0x%08lx\n", crc, ~crc_calc);
      ERROR_RETURN (ERROR_CRCFAILURE, "payload CRC error");
    }
    while (cbPadding--) {
      if (d->driver->read (d, &crc, 1) != 1)
        ERROR_RETURN (ERROR_IOFAILURE, "payload padding missing");
    }
#if defined (CONFIG_ALIASES)
    if (info->type == typeLinuxKernel && info->addrEntry != ~0) {
      unsigned addr = lookup_alias_or_env_unsigned ("bootaddr", ~0);
      if (addr != info->addrEntry)
        alias_set_hex ("bootaddr", info->addrEntry);
    }
    if (info->type == typeLinuxInitrd) {
      unsigned size = lookup_alias_or_env_unsigned ("ramdisksize", ~0);
      if (size != info->length)
        alias_set_hex ("ramdisksize", info->length);
      if (info->addrLoad != ~0) {
        unsigned addr = lookup_alias_or_env_unsigned ("ramdiskaddr", ~0);
        if (addr != info->addrLoad)
          alias_set_hex ("ramdiskaddr", info->addrLoad);
      }
    }
#endif
    printf ("%d bytes transferred\n", info->length);
    break;
  default:
    break;
  }

  return 0;
}


/** Handle checking APEX image payloads.  The header must be loaded
    into the global g_rgbHeader and the descriptor must be ready to read
    the first byte of the first payload.  The payload CRCs will be
    checked without copying the data to the memory. */

int handle_check_apex_image (int field,
                             struct descriptor_d* d, struct image_info* im_info,
                             struct payload_info* info)
{
  int result = 0;
  unsigned long crc;
  unsigned long crc_calc = 0;
  ssize_t cbPadding = 16 - ((info->length + sizeof (crc)) & 0xf);

  switch (field) {
  case fieldImageDescription:
    printf ("Image '%s'\n", info->szField);
    break;
  case fieldPayloadLength:
    printf ("# %s mem:0x%08x+0x%08x  %s\n",
            describe_apex_image_type (info->type), info->addrLoad,
            info->length, info->sz ? info->sz : "");

    if (im_info->fRegionCanExpand
        && d->length - d->index < info->length + 4 + cbPadding)
      d->length = d->index + info->length + 4 + cbPadding;

    result = region_checksum (info->length, d,
                              regionChecksumSpinner | regionChecksumLength,
                              &crc_calc);
    if (result < 0)
      return result;
    DBG (2, " %d %lx %d %ld\n",
         im_info->fRegionCanExpand, d->start, d->index, d->length);
    if (d->driver->read (d, &crc, sizeof (crc)) != sizeof (crc))
      ERROR_RETURN (ERROR_IOFAILURE, "payload CRC missing");
    crc = swabl (crc);
    while (cbPadding--) {
      char b;
      if (d->driver->read (d, &b, 1) != 1)
        ERROR_RETURN (ERROR_IOFAILURE, "payload padding missing");
    }
    printf ("\r%d bytes checked, CRC 0x%08lx ", info->length, crc);
    if (crc == ~crc_calc)
      printf ("OK\n");
    else
      printf ("!= 0x%08lx ERR\n", ~crc_calc);
    if (crc != ~crc_calc)
      ERROR_RETURN (ERROR_CRCFAILURE, "payload CRC error");
    break;
  case fieldLinuxKernelArchitectureID:
    printf ("Linux Kernel Arch ID:    %d (0x%x)\n", info->v, info->v);
    break;

  default:
    break;
  }

  return 0;
}


/** Process APEX image payloads.  The header must be loaded into the
    global g_rgbHeader and the descriptor must be ready to read the
    first byte of the first payload.

    The pfn parameter is a handler function that is called for each
    field.  This function accumulates payload information into a
    structure so that the handler function doesn't need to do this
    common task. */

int apex_image (int (*pfn) (int, struct descriptor_d*,
                            struct image_info*, struct payload_info*),
                struct descriptor_d* d, struct image_info* im_info)
{
  int result = 0;
  int cbHeader = g_rgbHeader[4]*16;

  struct payload_info info;
  int i;

  clear_info (&info);

  for (i = 5; i < cbHeader - 4; ) {
    int cbTag;
    int cbData;
//    uint32_t v = 0;
//    char* sz = NULL;

    if (!tag_lengths (g_rgbHeader + i, &cbTag, &cbData)) {
      DBG (1, "header parse error %d/%d 0x%x\n", i, cbHeader, g_rgbHeader[i]);
      ERROR_RETURN (ERROR_FAILURE, "invalid header tag");
    }

    switch (g_rgbHeader[i] & 3) {
    case sizeZero:
      break;
    case sizeOne:
      info.v = (unsigned char) g_rgbHeader[i + cbTag];
      break;
    case sizeFour:
      memcpy (&info.v, g_rgbHeader + i + cbTag, sizeof (uint32_t));
      info.v = swabl (info.v);
      break;
    case sizeVariable:
      info.szField = g_rgbHeader + i + cbTag;
      break;
    }

    switch (g_rgbHeader[i]) {
    case fieldPayloadLength:
      /* Make sure we have a good load address */
      if (info.addrLoad == ~0 && info.type == typeLinuxKernel)
        info.addrLoad = lookup_alias_or_env_int ("bootaddr", info.addrLoad);
      if (info.addrLoad == ~0 && info.type == typeLinuxInitrd)
        ;                       /* *** FIXME: we should have a place */

      info.length = info.v;
      break;
    case fieldPayloadLoadAddress:
      info.addrLoad = info.v;
      break;
    case fieldPayloadEntryPoint:
      info.addrLoad = info.v;
      break;
    case fieldPayloadType:
      info.type = info.v;
      break;
    case fieldPayloadDescription:
      info.sz = info.szField;
      break;
    default:
      break;                    // It's OK to skip unknown tags
    }

    result = pfn (g_rgbHeader[i], d, im_info, &info);
    if (result < 0)
      return result;

    if (g_rgbHeader[i] == fieldPayloadLength)
      clear_info (&info);

    i += cbTag + cbData;
  }

  return 0;
}

int handle_apex_image (int op, struct descriptor_d* d,
                       struct image_info* im_info)
{
  int result;

  if ((result = verify_apex_image (d, im_info)) < 0) {
    DBG (1, "header verification failed %d\n", result);
    return result;
  }

  switch (op) {
  case 'c':
    result = apex_image (handle_check_apex_image, d, im_info);
    break;
#if defined (CONFIG_CMD_IMAGE_SHOW)
  case 's':
    result = apex_image (handle_report_apex_image, d, im_info);
    break;
#endif
  case 'l':
    result = apex_image (handle_load_apex_image, d, im_info);
    break;
  }
  return result;
}


pfn_handle_image is_apex_image (const char* rgb, size_t cb)
{
  ENTRY (0);

  if (cb < sizeof (signature))
    return NULL;
  if (memcmp (rgb, signature, sizeof (signature)))
    return NULL;

  printf ("# APEX image\n");

  memcpy (g_rgbHeader, rgb, cb);
  g_cbHeader = cb;

  return handle_apex_image;
}

