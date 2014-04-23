/* cmd-image-uboot.c

   written by Marc Singer
   12 Sep 2008

   Copyright (C) 2008 Marc Singer

   -----------
   DESCRIPTION
   -----------

   o UBOOT image format.  The UBOOT image format was originally design
     for a single payload.  The file begins with a fixed-length
     header, 64 bytes followed by the payload.

       IMAGE
       +--------+---------+
       | HEADER | PAYLOAD |
       +--------+---------+

     The multi-image format modifies the basic format with an NULL
     terminated array of payload lengths followed by all of the
     payload data.

       MULTI-IMAGE
       +--------+--------+-----+---+---------+
       | HEADER | LENGTH | ... | 0 | PAYLOAD |
       +--------+--------+-----+---+---------+

     In this case, the header is identical, so there is only one CRC
     for all payloads, the data_size field of the header refers to the
     size of all payloads, the is only one entry point, and there is
     only one load address for all payloads.  All payload data is
     loaded starting at the load address in the header.  When booting
     Linux, UBOOT interprets the first two payloads of a multi-image
     as a kernel and initrd.  The load address and length of the
     initrd will be recorded and passed to the kernel.  APEX performs
     the same steps.

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

extern unsigned long compute_crc32_lsb (unsigned long crc,
                                        const void *pv, int cb);


enum {
  archNULL		= 0,
  archALPHA		= 1,
  archARM		= 2,
  archI386		= 3,
  archIA64		= 4,
  archMIPS		= 5,
  archMIPS64		= 6,
  archPPC		= 7,
  archS390		= 8,
  archSH		= 9,
  archSPARC		= 10,
  archSPARC64		= 11,
  archM68K		= 12,
  archNIOS		= 13,
  archMICROBLAZE	= 14,
  archNIOS2		= 15,
};

enum {
  osNULL		= 0,
  osOPENBSD		= 1,
  osNETBSD		= 2,
  osFREEBSD		= 3,
  os44BSD		= 4,
  osGNULinux		= 5,
  osSVR4		= 6,
  osESIX		= 7,
  osSolaris		= 8,
  osIrix		= 9,
  osSCO			= 10,
  osDELL		= 11,
  osNCR			= 12,
  osLYNXOS		= 13,
  osVXWORKS		= 14,
  osPSOS		= 15,
  osQNX			= 16,
  osUBOOT		= 17,
  osRTEMS		= 18,
  osARTOS		= 19,
  osUNITY		= 20,
};

enum {
  typeNULL		= 0,
  typeStandalone	= 1,
  typeKernel		= 2,
  typeRamdisk		= 3,
  typeMulti		= 4,
  typeFirmware		= 5,
  typeScript		= 6,
  typeFilesystem	= 7,
};

enum {
  compNull = 0,
  compGZIP = 1,
  compBZIP2 = 2,
};

struct header {
  uint32_t magic;
  uint32_t header_crc;
  uint32_t time;
  uint32_t size;
  uint32_t load_address;
  uint32_t entry_point;
  uint32_t crc;
  uint8_t  os;
  uint8_t  architecture;
  uint8_t  image_type;
  uint8_t  compression;
  char     image_name[32];
};

static const uint8_t magic[] = { 0x27, 0x05, 0x19, 0x56 };
static char __xbss(image) g_rgbHeader[64]; /* Largest possible header */
static size_t g_cbHeader;
static uint32_t __xbss(image) g_rgSizes[32]; /* Multi-image payload sizes */
static size_t g_cPayloads;

static inline uint32_t swabl (uint32_t v)
{
  return 0
    | ((v & (uint32_t) (0xff <<  0)) << 24)
    | ((v & (uint32_t) (0xff <<  8)) <<  8)
    | ((v & (uint32_t) (0xff << 16)) >>  8)
    | ((v & (uint32_t) (0xff << 24)) >> 24);
}

int verify_uboot_image (struct descriptor_d* d, struct image_info* info)
{
  int result = 0;
  ssize_t cbNeed;               /* Most definitely can be negative */
  size_t cbHeader;
  uint32_t crc;
  uint32_t crc_calc;

  ENTRY (0);

  cbNeed = 4 - g_cbHeader;
  if (cbNeed > 0) {
    if (info->fRegionCanExpand && d->length - d->index < cbNeed)
      d->length = d->index + cbNeed;
	/* Read the signature and header length if not already present */
    result = d->driver->read (d, g_rgbHeader + g_cbHeader, cbNeed);
    if (result != cbNeed)
      ERROR_RETURN (ERROR_IOFAILURE, "magic read error");
    g_cbHeader += cbNeed;
  }
  if (memcmp (g_rgbHeader, magic, sizeof (magic)))
    ERROR_RETURN (ERROR_FAILURE, "invalid magic");
  cbHeader = 64;
  if (cbHeader > sizeof (g_rgbHeader))
    ERROR_RETURN (ERROR_FAILURE, "impossible header length");

  cbNeed = cbHeader - g_cbHeader;
  if (cbNeed > 0) {
    if (info->fRegionCanExpand && d->length - d->index < cbNeed)
      d->length = d->index + cbNeed;
    result = d->driver->read (d, g_rgbHeader + g_cbHeader, cbNeed);
    if (result != cbNeed)
      ERROR_RETURN (ERROR_IOFAILURE, "header read error");
    g_cbHeader += cbNeed;
  }

	/* Load length array */
  g_cPayloads = 0;
  if (((struct header*) g_rgbHeader)->image_type == typeMulti) {
    uint32_t cbPayload;
    cbNeed = sizeof (g_rgSizes);
    if (info->fRegionCanExpand && d->length - d->index < cbNeed)
      d->length = d->index + cbNeed;
    for (; cbPayload; ++g_cPayloads) {
      result = d->driver->read (d, &cbPayload, sizeof (cbPayload));
      if (result != sizeof (cbPayload))
        ERROR_RETURN (ERROR_IOFAILURE, "size array read error");
      if ((g_rgSizes[g_cPayloads] = cbPayload) == 0)
        break;
    }
  }

  crc = ((struct header*) g_rgbHeader)->header_crc;
  ((struct header*) g_rgbHeader)->header_crc = 0;
  crc_calc = compute_crc32_lsb (0, g_rgbHeader, g_cbHeader);
  if (swabl (crc) != crc_calc)
    ERROR_RETURN (ERROR_CRCFAILURE, "broken header CRC");
  ((struct header*) g_rgbHeader)->header_crc = crc;

  return result;
}

#if defined (CONFIG_CMD_IMAGE_SHOW)

static const char* describe_uboot_architecture (unsigned long v)
{
  switch (v) {
  default:
  case archNULL:	return "unknown-architecture";		break;
  case archALPHA:	return "Alpha";				break;
  case archARM:		return "ARM";				break;
  case archI386:	return "Intel-x86";			break;
  case archIA64:	return "Intel-ia64";			break;
  case archMIPS:	return "MIPS";				break;
  case archMIPS64:	return "MIPS64";			break;
  case archPPC:		return "PowerPC";			break;
  case archS390:	return "IBM-S390";			break;
  case archSH:		return "SH";				break;
  case archSPARC:	return "Sun-Sparc";			break;
  case archSPARC64:	return "Sun-Sparc64";			break;
  case archM68K:	return "Motorola-68K";			break;
  case archNIOS:	return "NIOS";				break;
  case archMICROBLAZE:	return "MICROBLAZE";			break;
  case archNIOS2:	return "NIOS2";				break;
  }
}

static const char* describe_uboot_os (unsigned long v)
{
  switch (v) {
  default:
  case osNULL:		return "unknown-os";			break;
  case osOPENBSD:	return "OpenBSD";			break;
  case osNETBSD:	return "NetBSD";			break;
  case osFREEBSD:	return "FreeBSD";			break;
  case os44BSD:		return "BSD-4.4";			break;
  case osGNULinux:	return "GNU/Linux";			break;
  case osSVR4:		return "SVR4";				break;
  case osESIX:		return "ESIX";				break;
  case osSolaris:	return "Solaris";			break;
  case osIrix:		return "Irix";				break;
  case osSCO:		return "SCO";				break;
  case osDELL:		return "DELL";				break;
  case osNCR:		return "NCR";				break;
  case osLYNXOS:	return "LYNXOS";			break;
  case osVXWORKS:	return "VXWORKS";			break;
  case osPSOS:		return "PSOS";				break;
  case osQNX:		return "QNX";				break;
  case osUBOOT:		return "UBOOT";				break;
  case osRTEMS:		return "RTEMS";				break;
  case osARTOS:		return "ARTOS";				break;
  case osUNITY:		return "UNITY";				break;
  }
};

static const char* describe_uboot_image_type (unsigned long v)
{
  switch (v) {
  default:
  case typeNULL:	return "unknown-image-type";		break;
  case typeStandalone:	return "Standalone";			break;
  case typeKernel:	return "Kernel";			break;
  case typeRamdisk:	return "Ramdisk";			break;
  case typeMulti:	return "Multi";				break;
  case typeFirmware:	return "Firmware";			break;
  case typeScript:	return "Script";			break;
  case typeFilesystem:	return "Filesystem";			break;
  }
};

const char* describe_uboot_compression (unsigned long v)
{
  switch (v) {
  default:
  case compNull:	return "no-compression";		break;
  case compGZIP:	return "gzip-compression";		break;
  case compBZIP2:	return "bzip2-compression";		break;
  }
};


/** Handle reporting on UBOOT image and payloads. */

int handle_report_uboot_image (struct descriptor_d* d, struct image_info* info,
                               struct header* header)
{
  time_t t = swabl (header->time);
  struct tm tm;
  char sz[32];
  uint32_t crc;
  uint32_t crc_calc;

  ENTRY (0);

  asctime_r (gmtime_r (&t, &tm), sz);
  sz[24] = 0;             /* Knock out the newline */

  printf ("Image Name:    %-32.32s\n",
	  *header->image_name ? header->image_name : "<null>");
  printf ("Creation Date: %s UTC (0x%x %d)\n", sz, (uint32_t) t, (uint32_t) t);
  printf ("Image Type:    %s %s %s (%s)\n",
	  describe_uboot_architecture (header->architecture),
	  describe_uboot_os (header->os),
	  describe_uboot_image_type (header->image_type),
	  describe_uboot_compression (header->compression));
  printf ("Data Size:     %s\n",
          describe_size (swabl (header->size)));
  printf ("Load Address:  0x%08x\n", swabl (header->load_address));
  printf ("Entry Point:   0x%08x\n", swabl (header->entry_point));

  crc = header->header_crc;
  header->header_crc = 0;
  crc_calc = compute_crc32_lsb (0, header, sizeof (*header));
  printf ("Header CRC:    0x%08x", swabl (crc));
  if (swabl (crc) == crc_calc)
    printf (" OK\n");
  else
    printf (" ERROR (!= 0x%08x)\n", crc_calc);
  header->header_crc = crc;

  return 0;
}

#endif


/** Handle loading of UBOOT image payloads.  It loads the payload into
    memory at the load address.  For kernel payloads, it sets
    environment variables for the entry point and Linux kernel
    architecture ID.  For initrd payloads it sets the environment
    variables for the start address and size of the initrd.  Also, if
    an image has no load address, the payload will be loaded to the
    default address built into APEX. */

int handle_load_uboot_image (struct descriptor_d* d, struct image_info* info,
                             struct header* header)
{
  int result = 0;
  struct descriptor_d dout;
  unsigned long crc = swabl (header->crc);
  unsigned long crc_calc = 0;
  uint32_t addrLoad = swabl (header->load_address);
  uint32_t addrEntry = swabl (header->entry_point);
  uint32_t addrLoadInitrd = ~0;
  size_t cbCheck = swabl (header->size);
  size_t cb = cbCheck;

  if (header->image_type == typeMulti)
    cb -= (g_cPayloads + 1)*sizeof (*g_rgSizes); /* Correct data_size */

  if (header->image_type == typeMulti && g_cPayloads == 2) {
    printf ("# Kernel (%s) mem:0x%08x+0x%08x %s\n",
            describe_uboot_image_type (header->image_type), addrLoad,
            swabl (g_rgSizes[0]), header->image_name);
    addrLoadInitrd = addrLoad + swabl (g_rgSizes[0]);
    printf ("# Initrd (%s) mem:0x%08x+0x%08x %s\n",
            describe_uboot_image_type (header->image_type),
            addrLoadInitrd, swabl (g_rgSizes[1]), header->image_name);
  }
  else
    printf ("# %s mem:0x%08x+0x%08x %s\n",
            describe_uboot_image_type (header->image_type), addrLoad,
            cb, header->image_name);


    /* Copy image and check CRC */
  if (info->fRegionCanExpand
      && d->length - d->index < cb)
    d->length = d->index + cb;

  parse_descriptor_simple ("memory", addrLoad, cb, &dout);
  result = region_copy (&dout, d, regionCopySpinner);
  if (result >= 0) {
    parse_descriptor_simple ("memory", addrLoad, cb, &dout);
    if (header->image_type == typeMulti)
      crc_calc = compute_crc32_lsb (crc_calc, g_rgSizes,
                                    (g_cPayloads + 1)*sizeof (*g_rgSizes));
    result = region_checksum (cb, &dout,
                              regionChecksumSpinner | regionChecksumLSB,
                              &crc_calc);
  }

  printf ("\r");
  if (result < 0)
    return result;
  if (crc != crc_calc) {
    DBG (1, "crc 0x%08lx  crc_calc 0x%08lx\n", crc, crc_calc);
    ERROR_RETURN (ERROR_CRCFAILURE, "payload CRC error");
  }

  if (addrLoadInitrd != ~0 && info->initrd_relocation != ~0) {
    printf ("# Initrd (%s) mem:0x%08x+0x%08x [relocation]\n",
            describe_uboot_image_type (header->image_type),
            info->initrd_relocation, swabl (g_rgSizes[1]));
    memcpy ((void*) info->initrd_relocation, (void*) addrLoadInitrd,
            swabl (g_rgSizes[1]));
    addrLoadInitrd = info->initrd_relocation;
  }

#if defined (CONFIG_ALIASES)
  if (header->image_type == typeKernel && addrEntry != ~0) {
    unsigned addr = lookup_alias_or_env_unsigned ("bootaddr", ~0);
    if (addr != addrEntry)
      alias_set_hex ("bootaddr", addrEntry);
  }
  if (header->image_type == typeRamdisk) {
    size_t size = lookup_alias_or_env_unsigned ("ramdisksize", ~0);
    if (size != cb)
      alias_set_hex ("ramdisksize", cb);
    if (addrLoad != ~0) {
      unsigned addr = lookup_alias_or_env_unsigned ("ramdiskaddr", ~0);
      if (addr != addrLoad)
        alias_set_hex ("ramdiskaddr", addrLoad);
    }
  }
  if (header->image_type == typeMulti && addrEntry != ~0) {
    unsigned addr = lookup_alias_or_env_unsigned ("bootaddr", ~0);
    if (addr != addrEntry)
      alias_set_hex ("bootaddr", addrEntry);
    if (g_cPayloads > 1) {      /* We have an initrd as well */
      size_t   size = lookup_alias_or_env_unsigned ("ramdisksize", ~0);
      unsigned addr = lookup_alias_or_env_unsigned ("ramdiskaddr", ~0);
      if (size != swabl (g_rgSizes[1]))
        alias_set_hex ("ramdisksize", swabl (g_rgSizes[1]));
      if (addr != addrLoadInitrd)
        alias_set_hex ("ramdiskaddr", addrLoadInitrd);
    }
  }
#endif

  printf ("%d bytes transferred\n", cb);

  return 0;
}


/** Handle checking UBOOT image payloads.  The header must be loaded
    into the global g_rgbHeader and the descriptor must be ready to read
    the first byte of the first payload.  The payload CRCs will be
    checked without copying the data to the memory. */

int handle_check_uboot_image (struct descriptor_d* d, struct image_info* info,
                              struct header* header)
{
  int result = 0;
  unsigned long crc = swabl (header->crc);
  unsigned long crc_calc = 0;
  size_t cbCheck = swabl (header->size);
  size_t cb = cbCheck;

  if (info->fRegionCanExpand
      && d->length - d->index < cb)
      d->length = d->index + cb;

  if (header->image_type == typeMulti) {
    crc_calc = compute_crc32_lsb (crc_calc, g_rgSizes,
                                  (g_cPayloads + 1)*sizeof (*g_rgSizes));
    cb -= (g_cPayloads + 1)*sizeof (*g_rgSizes); /* Correct data_size */
  }
  result = region_checksum (cb, d, regionChecksumSpinner | regionChecksumLSB,
                            &crc_calc);

  if (result < 0)
    return result;
  printf ("\r%d bytes checked, CRC 0x%08lx ", cbCheck, crc);
  if (crc == crc_calc)
    printf ("OK\n");
  else
    printf ("!= 0x%08lx ERR\n", crc_calc);
  if (crc != crc_calc)
    ERROR_RETURN (ERROR_CRCFAILURE, "payload CRC error");

  return 0;
}


/** Process UBOOT image payloads.  The header must be loaded into the
    global g_rgbHeader and the descriptor must be ready to read the
    first byte of the first payload.

    The pfn parameter is a handler function that is called for each
    field.  This function accumulates payload information into a
    structure so that the handler function doesn't need to do this
    common task. */

int uboot_image (int (*pfn) (struct descriptor_d*, struct image_info*,
                             struct header*),
                struct descriptor_d* d, struct image_info* info)
{
  int result = 0;
  struct header* header = (struct header*) g_rgbHeader;

  ENTRY (1);
  DBG (1, "pfn %p\n", pfn);

  result = pfn (d, info, header);
  if (result < 0)
    return result;

  return 0;
}

int handle_uboot_image (int op, struct descriptor_d* d, struct image_info* info)
{
  int result;

  if ((result = verify_uboot_image (d, info)) < 0) {
    DBG (1, "header verification failed %d\n", result);
    return result;
  }

  DBG (1, "op is %c\n", op);

  switch (op) {
  case 'c':
    result = uboot_image (handle_check_uboot_image, d, info);
    break;
#if defined (CONFIG_CMD_IMAGE_SHOW)
  case 's':
    result = uboot_image (handle_report_uboot_image, d, info);
    break;
#endif
  case 'l':
    result = uboot_image (handle_load_uboot_image, d, info);
    break;
  }
  return result;
}


pfn_handle_image is_uboot_image (const char* rgb, size_t cb)
{
  ENTRY (0);

  if (cb < sizeof (magic))
    return NULL;
  if (memcmp (rgb, magic, sizeof (magic)))
    return NULL;

  printf ("# UBOOT image\n");

  memcpy (g_rgbHeader, rgb, cb);
  g_cbHeader = cb;

  return handle_uboot_image;
}

