/* drv-nor-cfi.c

   written by Marc Singer
   13 Nov 2004

   Copyright (C) 2004-2008 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   Generic NOR flash IO driver

   o This driver has been hacked to only support a 32 bit NOR
     meta-device consisting of a pair of 16 bit flash chips.  It would
     be good to have a single driver that uses compile-time macros to
     control how it accesses the flash array.  We want to stay away
     from truly dynamic code in order to conserve memory.

     *** buffered write code has not been fixed to handle the 32 bit
     *** array width.

   o Much work has gone into making this driver generic.  It is not
     intended to be able to support multiple organizations at one
     time.  In other words, the generic elements of the driver are
     fixed at compile time.

   o Banking support.  We take a few liberties with the banking
     support in that we assume some things.  Full generality comes at
     a cost and, at this point, I am not willing to pay the full cost.
     The driver can handle two chips, but not two completely different
     chips.  Esentially, it can handle a pair of identical chips or
     one chip with two banks.  It linearizes multiple banks, in
     phys_from_index(), for the convenience of writing large,
     contiguous blocks of data.  Improved flexbility would come from
     always translating from the region structure.  What probably
     needs to happen is the region structure needs to include the
     physical offset of the region.  Handling disparate chips,
     however, will probably never be done as the chips may require
     different control constants.  Phooey.

   o REGA, copy_from, and endianness

     copy_from should be a general implementation of the copy-out
     routine where the user is reading data from flash.  We use memcpy
     in most cases because that's OK.  There is one implementation of
     copy_from that performs a byte swap to cope with the endian
     change on the NSLU2.  In this case, the flash is written BE and
     we need to read it from an LE program.  To get the same view of
     the data, we have to swap.  The FIS driver, too, has some code to
     cope with swapped data.

     REGA does something different.  It has to read the swapped
     half-word in order to get the proper view of the flash.  This, I
     blame on the CPU which is performing a half-word swap.
     Interestingly, the commands and query data don't need anything
     except for the address swapping trick: (^=(1<<1)).

   o nor_write and endianness

     Though the read code is recitifed with REGA and copy_from
     fixes, the write code is still broken on systems where the order
     in flash does not match the order of the running CPU.

     Instead, we use SWAP_ONE in the write code to swap as we send
     bytes to the flash.  This is only used for data since the
     commands appear to be correctly understood without this
     transformation.

   o Spansion support

     Some work has been done to support the Spansion commands set for
     NOR flash.  It hasn't been tested by me because I don't have any
     Spansion memories.  The support also lacks buffered write code
     and unlock code as these were not present in the patch provided
     by (and thanks to) Jean Pierre Lepine.

     I chose to make this a compile-time option as doing so makes the
     code smaller and simpler.  In fact, the code size should not have
     changed for the Intel path since adding support for Spansion.

     Unlock support may be unnecessary for the MirrorBit parts because
     the devices power-on with blocks unlocked.

     This work is based, in part, on information gleaned from the
     S29GLxxxN MirrorBitTM Flash Family data sheet.

   o Cache support

     I figured that we might be able to get some extra performance out
     of the nor flash driver by using the cache.  Turns out that this
     is true.  The USE_CACHE option enables cachability for the
     segment where the cache resides while reading.  It disables it on
     exit as this would interfere with normal memory driver reads as
     well as writing.  There is some mess involved as we need to
     invalidate a cache entry after writing the 0xff to enable
     ReadArray.  Perhaps, we could be smarter about this, but we
     really don't know if a given block is in read mode or not.

     The performance improvement is noteworthy.  Without the cache,
     reading the 6MiB ramdisk image on the NSLU2 takes about 2600ms.
     With the cache it takes about 1800ms.

     It is also important to note that the cache invalidation at the
     end of the nor_read () function is crucial to keeping the driver
     working properly.  Without it, the erase and write functions fail
     because the cache still has resident rows for the flash memory
     and the status reads fail as well as the ID reads.

     An optimization for NOR flash that would permit flash to remain
     cached would be to use a different memory region for the cached
     accesses.  In this case, the MMU bits wouldn't be toggled.
     Instead, the driver would make sure to invaldate NOR flash on
     writes, but let the rest of it operate normally.

   o ReadArray command.  Almost all flash we've seen responds to
     ReadArray as the command 0xff, even Spansion or so I thought.
     The Spansion flash in the MV2120 requires 0xf0.  So, we've
     changed the command to be 0xf0 for Spansion, but it remains to be
     seen whether or not all type 2 flash will accept it.  This is the
     same behavior as we see in the Linux kernel driver.

   o TopBoot/BottomBoot. Based on the Linux kernel driver, AMD PRI
     v1.0 doesn't report the location of the boot blocks.  This is
     important because the order of the reported erase regions does
     not reflect the order in the chip.  We need to know which it is.
     So, we check the PRI version and the device ID.  If we determine
     that this is a top boot AMD/Spansion device then we swap the
     erase regions to properly reflect the order in the chip.  Yikes!

*/

#include <driver.h>
#include <service.h>
#include <linux/string.h>
#include <apex.h>
#include <config.h>
#include <spinner.h>
#include <asm/reg.h>
#include <error.h>

#include <mach/nor-cfi.h>

/* *** We should also check for ARM since the CP15 stuff is arm
   *** specific. */
#if defined (CONFIG_DRIVER_NOR_CFI_USE_CACHE)
# define USE_CACHE		/* Use cache to speed flash reading */
# include <asm/mmu.h>
# include <asm/cp15.h>
#endif

#define USE_DETECT_ENDIAN_MISMATCH

//#define TALK
//#define NOISY                   /* Use when CFI not detecting properly */

#if defined TALK
# define PRINTF(v...)	printf (v)
#else
# define PRINTF(v...)	do {} while (0)
#endif

#define ENTRY(l) PRINTF ("%s\n", __FUNCTION__)

//#define NO_WRITE		/* Disable writes, for debugging */

#if !defined (NOR_BUS_WIDTH)
# define NOR_BUS_WIDTH NOR_WIDTH
#endif

#if !defined (NOR_CHIP_MULTIPLIER)
# define NOR_CHIP_MULTIPLIER	(1)	/* Number of chips at REGA */
#endif

#if NOR_WIDTH == 32
# define _REGA		__REG		/* Array I/O macro */
# if NOR_CHIP_MULTIPLIER == 1
#  define _REGC	        __REG
# else
#  define _REGC	        __REG16
# endif
# define nor_t		u32
#elif NOR_WIDTH == 16
# if NOR_BUS_WIDTH == 16
#  define _REGA		__REG16		/* Array I/O macro, full width */
#  define _REGC		__REG16		/* Single chip I/O macro, full width */
#  define nor_t		u16
# else
#  define _REGA		__REG8		/* Array I/O macro, byte wide */
#  define _REGC		__REG8		/* Single chip I/O macro, byte wide */
#  define nor_t		u8
# endif
#endif

#if defined (NOR_BIGENDIAN) && defined (CONFIG_LITTLEENDIAN)
# define REGA(a) _REGA((a)^(1<<1))
# define REGC(a) _REGC((a)^(1<<1))

# if NOR_WIDTH == 32
#  define SWAP_ONE(v) (  ((v & 0xff)<<24)    | ((v & 0xff00)<<8)\
		       | ((v & 0xff0000)>>8) | ((v & 0xff000000)>>24))
# else
#  define SWAP_ONE(v) (  ((v & 0xff)<<8)     | ((v & 0xff00)>>8))
# endif
#else
# define REGA(a) _REGA(a)
# define REGC(a) _REGC(a)
# define SWAP_ONE(v)	(v)
#endif

#if NOR_CHIP_MULTIPLIER == 2
/* *** FIXME, this may not work for anything except a pair of 16 bit parts */
# define CMD(v)		((v) | ((v)<<16))
# define STAT(v)	((v) | ((v)<<16))
# define QRY(v)		((v) | ((v)<<16))
#else
# define CMD(v)		(v)
# define STAT(v)	(v)
# define QRY(v)		(v)
#endif

#if NOR_WIDTH == 16
/* *** This has only be tested to work with a single chip and a bus
   width of 16. */
# if ! defined (CONFIG_SMALL)\
  && ! defined (CONFIG_DRIVER_NOR_CFI_NO_BUFFERED)\
  && ! defined (CONFIG_DRIVER_NOR_CFI_TYPE_SPANSION)
#  define USE_BUFFERED_WRITE	/* Use write buffer for faster operation */
# endif
#endif

#define WIDTH_SHIFT	(NOR_WIDTH>>4)	/* Bit shift for addresses */

#define ReadQuery	0x98    /* CFI query is standardized */

#if defined (CONFIG_DRIVER_NOR_CFI_TYPE_INTEL)
	/* CFI compliant parameters */
#define ReadArray	0xff
#define ReadID		0x90
#define ReadStatus	0x70
#define ClearStatus	0x50
#define Erase		0x20
#define EraseConfirm	0xd0
#define Program		0x40
#define ProgramBuffered	0xe8
#define ProgramConfirm	0xd0
#define Suspend		0xb0
#define Resume		0xd0
#define STSConfig	0xb8
#define Configure	0x60
#define LockSet		0x01
#define LockClear	0xd0
#define ProgramOTP	0xc0

#endif

#if defined (CONFIG_DRIVER_NOR_CFI_TYPE_SPANSION)

#define ADR_UN1		0x555
#define ADR_UN2		0x2aa

#define ReadArray	0xf0    /* aka Reset */
#define ReadID		0x90
#define ReadStatus	0x70
#define ClearStatus	0x50
#define EraseSetup	0x80
#define Erase		0x30
#define Program		0xa0
#define ProgramBuffered 0xe8
#define ProgramConfirm  0xd0
#define Suspend		0xb0
#define Resume		0x30
#define STSConfig	0xb8
#define Configure	0x60
#define LockSet		0x01
#define LockClear	0xd0
#define ProgramOTP	0xc0
#define UnlockData1	0xaa
#define UnlockData2	0x55
#define DQ1		(1<<1)
#define DQ2		(1<<2)
#define DQ3		(1<<3)
#define DQ5		(1<<5)
#define DQ6		(1<<6)

#endif

/* Status bits.  While these are really Intel protocol status bits,
   the Spansion code returns these values to emulate the Intel
   behavior. */

#define Ready		(1<<7)
#define EraseSuspended	(1<<6)
#define EraseError	(1<<5)
#define ProgramError	(1<<4)
#define VPEN_Low	(1<<3)
#define ProgramSuspended (1<<2)
#define DeviceProtected	(1<<1)

#if !defined (VPEN_ENABLE)
# define VPEN_ENABLE
#endif
#if !defined (VPEN_DISABLE)
# define VPEN_DISABLE
#endif

struct nor_region {
  int size;
  int count;
  unsigned long start;		/* Relative offsets from 0 of the start of */
  unsigned long next;		/*   this region and the start of the next */
};

#define C_REGIONS_MAX	16 	/* Reasonable storage for erase region info */

struct nor_chip {
  unsigned long total_size;
  int command_set;         	/* Command set identifier, [0x13] */
  int manufacturer_id;
  int device_id;
  int writebuffer_size;		/* Size (bytes) of buffered write buffer */
  int regions;
  struct nor_region region[C_REGIONS_MAX];
};

const static struct nor_chip* chip;
static struct nor_chip chip_probed;
#if defined (USE_DETECT_ENDIAN_MISMATCH)
int endian_error;
#endif

static unsigned long phys_from_index (unsigned long index)
{
#if defined (NOR_1_PHYS)
  return index
    + ((index < NOR_0_LENGTH) ? NOR_0_PHYS : (NOR_1_PHYS - NOR_0_LENGTH));
#else
  return index + NOR_0_PHYS;
#endif
}


/* vpen_enable/vpen_disable

   while it wouldn't be much to implement these as inline macros, the
   code size tends to be larger.  In this case, size is the primary
   optimization goal.

*/

static void vpen_enable (void)
{
  VPEN_ENABLE;
}

static void vpen_disable (void)
{
  VPEN_DISABLE;
}

static unsigned long nor_read_one (unsigned long index)
{
  return REGA (index);
}
#define READ_ONE(i) nor_read_one (i)

static void nor_write_one (unsigned long phys, unsigned long v)
{
//  PRINTF ("%s: [%lx]<-%x\n", __FUNCTION__, phys, (nor_t) v);
  REGA (phys) = (nor_t) v;
}
#define WRITE_ONE(i,v) nor_write_one ((i), (v))

#define CLEAR_STATUS(i) nor_write_one(i, CMD (ClearStatus))

/* nor_region

   returns a pointer to the erase region structure for the given index.

*/

static const struct nor_region* nor_region (unsigned long index)
{
  int i;
  for (i = 0; i < chip->regions; ++i)
    if (index < chip->region[i].next)
      return &chip->region[i];
  return &chip->region[0];		/* A very serious error */
}

#if defined (CONFIG_DRIVER_NOR_CFI_TYPE_INTEL)

/* intel:nor_status
*/

static unsigned long nor_status (unsigned long index)
{
  unsigned long status;
  unsigned long time = timer_read ();
  do {
    status = READ_ONE (index);
  } while (   (status & STAT (Ready)) != STAT (Ready)
	   && timer_delta (time, timer_read ()) < 6*1000);
//  PRINTF ("status: 0x%02lx\n", status);
  return status;
}

static inline void nor_write_perform (unsigned long index, unsigned long data)
{
  WRITE_ONE (index, CMD (Program));
  WRITE_ONE (index, data);
}

static inline void nor_erase_perform (unsigned long index)
{
  PRINTF ("%s: %lx\n", __FUNCTION__, index);
  WRITE_ONE (index, CMD (Erase));
  WRITE_ONE (index, CMD (EraseConfirm));
}

/* intel:nor_unlock_page

   will conditionally unlock the page if it is locked.  Conditionally
   performing this operation results in minimal wear.

   We check if either block is locked and unlock both if one is
   locked.

*/

static unsigned long nor_unlock_page (unsigned long index)
{
  index &= ~ (nor_region (index)->size - 1);

  WRITE_ONE (index, CMD (ReadID));
  PRINTF ("nor_unlock_page 0x%lx %x\n", index,
	  REGA (index + (0x02 << WIDTH_SHIFT)));
  if ((REGA (index + (0x02 << WIDTH_SHIFT)) & QRY (0x1)) == 0)
    return STAT (Ready);

  WRITE_ONE (index, CMD (Configure));
  WRITE_ONE (index, CMD (LockClear));
  return nor_status (index);
}

#endif

#if defined (CONFIG_DRIVER_NOR_CFI_TYPE_SPANSION)

/* spansion:nor_status

   while Spansion flash memories are busy successive reads to any
   address return a toggling DQ6 bit.  When this bit stops toggling,
   the function has completed.

   This function, on recommendation of the data sheet, rechecks the
   toggling of DQ6 after it sees the DQ5.  I suppose we could also
   check that DQ5 was set in both dq and dqPrev, but this way is the
   'recommended' flow.

*/

static unsigned long nor_status (unsigned long index)
{
  nor_t dqPrev;
  nor_t dq;
  int count = 0;

//  usleep (4);			/* Required delay before reading status */

  dqPrev = READ_ONE (index);

  while (((dq = READ_ONE (index)) ^ dqPrev) & STAT (DQ6)) {
    ++count;
    if (dq & STAT (DQ5)) {
      dqPrev = READ_ONE (index);
      dq = READ_ONE (index);
      if (!((dq ^ dqPrev) & STAT (DQ5)))
	break;			/* No longer toggling -> Ready */
      return ((dq ^ dqPrev) & STAT (DQ2))
	? STAT (EraseError)
	: STAT (ProgramError);
    }
    dqPrev = dq;
  }

  PRINTF ("%s: status 0x%x 0x%x, toggled %d times\n",
          __FUNCTION__, dq, dqPrev, count);

  return STAT (Ready);
}

static inline void nor_write_perform (unsigned long phys, unsigned long data)
{
  PRINTF ("%s: [%lx]<-%x\n", __FUNCTION__, phys, (nor_t) data);
  WRITE_ONE (phys_from_index (ADR_UN1 << WIDTH_SHIFT), CMD (UnlockData1));
  WRITE_ONE (phys_from_index (ADR_UN2 << WIDTH_SHIFT), CMD (UnlockData2));
  WRITE_ONE (phys_from_index (ADR_UN1 << WIDTH_SHIFT), CMD (Program));
  WRITE_ONE (phys, data);
}

static inline void nor_erase_perform (unsigned long phys)
{
  WRITE_ONE (phys_from_index (ADR_UN1 << WIDTH_SHIFT), CMD (UnlockData1));
  WRITE_ONE (phys_from_index (ADR_UN2 << WIDTH_SHIFT), CMD (UnlockData2));
  WRITE_ONE (phys_from_index (ADR_UN1 << WIDTH_SHIFT), CMD (EraseSetup));
  WRITE_ONE (phys_from_index (ADR_UN1 << WIDTH_SHIFT), CMD (UnlockData1));
  WRITE_ONE (phys_from_index (ADR_UN2 << WIDTH_SHIFT), CMD (UnlockData2));
  WRITE_ONE (phys, CMD (Erase));
}

/* spansion:nor_unlock_page

   is a NULL operation for the time being.  This may be required in
   the future, we just don't have any hardware that needs this.

*/

static unsigned long nor_unlock_page (unsigned long index)
{
  return STAT (Ready);
}

#endif

#if defined (NOR_BIGENDIAN) && defined (CONFIG_LITTLEENDIAN)

/* copy_from

   implements a byte-swapping memory copy.  This code uses the REGA
   macro to read the correct half-words from memory in order, but it
   also needs to swap bytes when it stores them in RAM.

*/

void copy_from (void* _dst, const void* _src, size_t cb)
{
  char* dst = (char*) _dst;
  unsigned long src = (unsigned long) _src;
  if (cb <= 0)
    return;
  if ((src & 1) != 0) {
    u16 v = REGA (src & ~1);
    *dst++ = v & 0xff;
    ++src;
    --cb;
  }
  while (cb > 1) {
    u16 v = REGA (src);
    *dst++ = (v >> 8) & 0xff;	/* High byte first */
    *dst++ = v & 0xff;		/*   then low byte */
    src += 2;
    cb -= 2;
  }
  if (cb) {
    u16 v = REGA (src);
    *dst++ = (v >> 8) & 0xff;
  }
}
#else
# define copy_from memcpy
#endif

static void nor_probe_chip (unsigned long phys)
{
  int iRegionFirst = chip_probed.regions;
  int i;
  unsigned long start;
#if defined (CONFIG_DRIVER_NOR_CFI_TYPE_SPANSION)
  int reverse_regions = false;
#endif

  PRINTF ("%s: probing %lx (%d %d); [%lx+%x]<-%x\n", __FUNCTION__, phys,
	  NOR_WIDTH, NOR_CHIP_MULTIPLIER, phys, (0x55 << WIDTH_SHIFT),
          CMD(ReadQuery));

  REGA (phys) = CMD (ReadArray);
  REGA (phys + (0x55 << WIDTH_SHIFT)) = CMD (ReadQuery);

  PRINTF ("nor phys %p %x %x %x %x\n", (void*) phys,
	  (unsigned) REGA (phys + (0x10 << WIDTH_SHIFT)),
	  (unsigned) REGA (phys + (0x11 << WIDTH_SHIFT)),
	  (unsigned) REGA (phys + (0x12 << WIDTH_SHIFT)),
	  (unsigned) REGA (phys + (0x13 << WIDTH_SHIFT)));

#if defined (USE_DETECT_ENDIAN_MISMATCH)
  if (   REGA (phys + (0x11 << WIDTH_SHIFT)) == QRY('Q')
      || REGA (phys + (0x10 << WIDTH_SHIFT)) == QRY('R')
      || REGA (phys + (0x13 << WIDTH_SHIFT)) == QRY('Y')) {
    endian_error = 1;
    WRITE_ONE (phys, CMD (ReadArray)); /* Put array back into read mode */
    return;
  }
#endif

  if (   REGA (phys + (0x10 << WIDTH_SHIFT)) != QRY('Q')
      || REGA (phys + (0x11 << WIDTH_SHIFT)) != QRY('R')
      || REGA (phys + (0x12 << WIDTH_SHIFT)) != QRY('Y')) {
#if defined (TALK)
    PRINTF ("%s: failed\n", __FUNCTION__);
    dump ((void*) phys, 256, 0);
#endif
    return;
  }

#if defined (TALK)
  PRINTF ("%s: success\n", __FUNCTION__);
  dump ((void*) phys, 256, 0);
#endif

#if defined (ADR_UN1)
	/* Read manufacturer ID probe */
  WRITE_ONE (phys, CMD (ReadArray));
  REGC (phys_from_index (ADR_UN1 << WIDTH_SHIFT)) = CMD (UnlockData1);
  REGC (phys_from_index (ADR_UN2 << WIDTH_SHIFT)) = CMD (UnlockData2);
  REGC (phys_from_index (ADR_UN1 << WIDTH_SHIFT)) = CMD (ReadID);

  chip_probed.manufacturer_id
    = REGC (phys_from_index (0x00 << WIDTH_SHIFT));
  chip_probed.device_id
    = REGC (phys_from_index (0x01 << WIDTH_SHIFT));

	/* Return to Query */
  REGA (phys + (0x55 << WIDTH_SHIFT)) = CMD (ReadQuery);
#endif


#if defined (CONFIG_DRIVER_NOR_CFI_TYPE_SPANSION)
	  /* Fixup the erase regions for top-boot, AMD devices since
             the CFI table is upside-down.  Moreover, older devices
             don't make it easy to detect whether or not the device is
             top-boot, so we do our best to divine this fact. */
  {
    unsigned long pri = REGC (phys_from_index (0x15 << WIDTH_SHIFT))
      | (REGC (phys_from_index (0x16 << WIDTH_SHIFT)) << 8);
    unsigned version = REGC (phys_from_index ((pri + 3) << WIDTH_SHIFT)) << 8
      | REGC (phys_from_index ((pri + 4) << WIDTH_SHIFT));
    int top_bottom = REGC (phys_from_index ((pri + 0xf) << WIDTH_SHIFT));
    if (version < 0x3131) {     /* top_bottom isn't trustworthy */
      switch (chip_probed.manufacturer_id << 16 | chip_probed.device_id) {
      case (0xc2 << 16) | 0x0089:                /* Macronix MX29LV400CT */
      case (0xc2 << 16) | 0x2289:
        top_bottom = 3;
        break;
      case (0xc2 << 16) | 0x008a:                /* Macronix MX29LV400CB */
      case (0xc2 << 16) | 0x228a:
        top_bottom = 2;
        break;
      default:
        /* The Linux kernel just makes this simple mask test, but it
           may not be valid for all Macronix parts.  It's probably OK
           for AMD since they don't make NOR flash anymore. */
        top_bottom = (chip_probed.device_id & 0x80) ? 3 : 2;
        break;
      }
    }
    if (top_bottom == 3)
      reverse_regions = true;
  }
#endif


  start = chip_probed.total_size;
  i = chip_probed.regions;

  chip_probed.command_set  = REGC (phys + (0x13 << WIDTH_SHIFT));
  chip_probed.regions     += REGC (phys + (0x2c << WIDTH_SHIFT));
  chip_probed.total_size
    += (1<<REGC (phys + (0x27 << WIDTH_SHIFT)))*NOR_CHIP_MULTIPLIER;

  chip_probed.writebuffer_size
    = (1<<(   REGC (phys + (0x2a << WIDTH_SHIFT))
	   | (REGC (phys + (0x2b << WIDTH_SHIFT)) << 8)))
    *NOR_CHIP_MULTIPLIER;

	/* Discover erase regions.  Unfortunately, this has to be done
	   so that the erase and unlock IO is reasonably efficient. */
  for (; i < chip_probed.regions; ++i) {
    int offset
#if !defined (CONFIG_DRIVER_NOR_CFI_TYPE_SPANSION)
      = (i - iRegionFirst)*4;
#else
	    /* If we've got a top-boot Spansion part, we need to
               reverse the order of the erase regions. */
      = (reverse_regions ? chip_probed.regions - 1 - i : (i - iRegionFirst))*4;
#endif
    PRINTF ("  ");
    PRINTF (" %02x %02x %02x %02x ",
            REGC (phys + ((0x2d + offset) << WIDTH_SHIFT)),
            REGC (phys + ((0x2e + offset) << WIDTH_SHIFT)),
            REGC (phys + ((0x2f + offset) << WIDTH_SHIFT)),
            REGC (phys + ((0x30 + offset) << WIDTH_SHIFT)));

    chip_probed.region[i].size
      = 256*(   REGC (phys + ((0x2f + offset) << WIDTH_SHIFT))
             | (REGC (phys + ((0x30 + offset) << WIDTH_SHIFT)) << 8))
      *NOR_CHIP_MULTIPLIER;
    chip_probed.region[i].count
      = 1 + (   REGC (phys + ((0x2d + offset) << WIDTH_SHIFT))
             | (REGC (phys + ((0x2e + offset) << WIDTH_SHIFT)) << 8));

    PRINTF ("  region %d %d %d\n", i,
	    chip_probed.region[i].size, chip_probed.region[i].count);
    chip_probed.region[i].start = start;
    start += chip_probed.region[i].size*chip_probed.region[i].count;
    chip_probed.region[i].next = start;
  }
}

static void nor_init (void)
{
  ENTRY (0);

  vpen_disable ();

  chip_probed.total_size = 0;	/* Should be redundant */
  chip_probed.regions = 0;	/* Should be redundant */

  nor_probe_chip (NOR_0_PHYS);
#if defined (NOR_1_PHYS)
  nor_probe_chip (NOR_1_PHYS);
#endif

  if (!chip_probed.total_size)
    return;

  chip = &chip_probed;

#if defined (NOISY)

  PRINTF ("\nNOR flash ");

  if (chip) {
    PRINTF (" %ld.%02ld MiB total",
	    chip->total_size/(1024*1024), ((chip->total_size/1024)%1024)/10);
    PRINTF (", %dB write buffer",
	    chip->writebuffer_size);
    PRINTF ("\n");
  }

  PRINTF ("command set 0x%x\n",
	  REGC (NOR_0_PHYS + (0x13 << WIDTH_SHIFT)));
  PRINTF ("extended table 0x%x\n",
	  REGC (NOR_0_PHYS + (0x15 << WIDTH_SHIFT)));
  PRINTF ("alternate command set 0x%x\n",
	  REGC (NOR_0_PHYS + (0x17 << WIDTH_SHIFT)));
  PRINTF ("alternate address 0x%x\n",
	  REGC (NOR_0_PHYS + (0x19 << WIDTH_SHIFT)));

  {
    int typical;
    int max;

    typical = REGC (NOR_0_PHYS + (0x1f << WIDTH_SHIFT));
    max	    = REGC (NOR_0_PHYS + (0x23 << WIDTH_SHIFT));
    PRINTF ("single word write %d us (max %d us)\n",
	    1<<typical, (1<<typical)*max);
    typical = REGC (NOR_0_PHYS + (0x20 << WIDTH_SHIFT));
    max	    = REGC (NOR_0_PHYS + (0x24 << WIDTH_SHIFT));
    PRINTF ("write-buffer write %d us (max %d us)\n",
	    1<<typical, (1<<typical)*max);
    typical = REGC (NOR_0_PHYS + (0x21 << WIDTH_SHIFT));
    max	    = REGC (NOR_0_PHYS + (0x25 << WIDTH_SHIFT));
    PRINTF ("block erase %d ms (max %d ms)\n",
	    1<<typical, (1<<typical)*max);

    typical = REGC (NOR_0_PHYS + (0x22 << WIDTH_SHIFT));
    max     = REGC (NOR_0_PHYS + (0x26 << WIDTH_SHIFT));
    if (typical)
      PRINTF ("chip erase %d us (max %d us)\n",
	      1<<typical, (1<<typical)*max);
  }

  PRINTF ("device size 0x%x\n",
	  REGC (NOR_0_PHYS + (0x27 << WIDTH_SHIFT)));
  PRINTF ("flash interface 0x%x\n",
	  REGC (NOR_0_PHYS + (0x28 << WIDTH_SHIFT))
	  | (REGC (NOR_0_PHYS + (0x28 << WIDTH_SHIFT)) << 8));
  PRINTF ("write buffer size %d bytes\n",
	  1<<(REGC (NOR_0_PHYS + (0x2a << WIDTH_SHIFT))
	      | (REGC (NOR_0_PHYS + (0x2b << WIDTH_SHIFT)) << 8)));
  PRINTF ("erase block region count %d\n",
	  REGC (NOR_0_PHYS + (0x2c << WIDTH_SHIFT)));
  PRINTF ("erase block info 0x%x\n",
	  REGC (NOR_0_PHYS + (0x2d << WIDTH_SHIFT))
	  | (REGC (NOR_0_PHYS + (0x2e << WIDTH_SHIFT)) << 8)
	  | (REGC (NOR_0_PHYS + (0x2f << WIDTH_SHIFT)) << 16)
	  | (REGC (NOR_0_PHYS + (0x30 << WIDTH_SHIFT)) << 24));
#endif

  REGA (NOR_0_PHYS) = CMD (ClearStatus);
}

static int nor_open (struct descriptor_d* d)
{
  if (!chip)
    return -1;

  /* perform bounds check */

  return 0;
}

static ssize_t nor_read (struct descriptor_d* d, void* pv, size_t cb)
{
  ssize_t cbRead = 0;
#if defined (USE_CACHE)
  unsigned long base = phys_from_index (d->start + d->index);
#endif

  if (d->index + cb > d->length)
    cb = d->length - d->index;

#if defined (USE_CACHE)
  mmu_protsegment ((void*) base, 1, 0);
#endif

  while (cb) {
    unsigned long index = d->start + d->index;
    int available = cb;
    if (index < chip->total_size && index + available > chip->total_size)
      available = chip->total_size - index;
    index = phys_from_index (index);

    d->index += available;
    cb -= available;
    cbRead += available;

    //    printf ("nor: 0x%p 0x%08lx %d\n", pv, index, available);
    WRITE_ONE (index, CMD (ReadArray));
#if defined (USE_CACHE)
    invalidate_dcache_va (index);
    cp15_wait ();
#endif
    copy_from (pv, (void*) index, available);

    pv += available;
  }

#if defined (USE_CACHE)
  /* Uncache the segment and then invalidate the DCACHE rows we read. */
  mmu_protsegment ((void*) phys_from_index (d->start + d->index), 0, 0);
  cb = (cbRead + 31) & ~0x1f;
  base = base & ~0x1f;
  for (; cb; base += 32, cb -= 32)
    invalidate_dcache_va (base);
  cp15_wait ();
#endif

  return cbRead;
}


#if defined (USE_BUFFERED_WRITE)

/* nor_write

   performs a buffered write to the flash device.  The unbuffered
   write, below, is adequate but this version is much faster.  Like
   the single short write method below, we fuss a bit with the
   alignment so that we emit the data efficiently.  Moreover, we make
   an attempt to align the write buffer to a write buffer aligned
   boundary.

*/

static ssize_t nor_write (struct descriptor_d* d, const void* pv, size_t cb)
{
  size_t cbWrote = 0;
  unsigned long pageLast = ~0;

  if (!chip->writebuffer_size)
    ERROR_RETURN (ERROR_UNSUPPORTED,
		  "flash chip doesn't support buffered writes");

  if (d->index + cb > d->length)
    cb = d->length - d->index;

  while (cb > 0) {
    unsigned long index = d->start + d->index;
    unsigned long page = index & ~ (nor_region (index)->size - 1);
    unsigned short status;
    int available
      = chip->writebuffer_size - (index & (chip->writebuffer_size - 1));

    if (available > cb)
      available = cb;

    index = phys_from_index (index);

    PRINTF ("nor write: 0x%p 0x%08lx %d\n", pv, index, available);

    vpen_enable ();

#if !defined (NO_WRITE)
    if (page != pageLast) {
      status = nor_unlock_page (index);
      if (status & (ProgramError | VPEN_Low | DeviceProtected))
	goto fail;
      pageLast = page;
    }
#endif

	/* === Initiate buffer write */

#if defined (NO_WRITE)
    printf ("  available %d  cb %d\n", available, cb);
    printf ("0x%lx <= 0x%x\n", index & ~(NOR_BUS_WIDTH/8 - 1), ProgramBuffered);
#else
    WRITE_ONE (index & ~(NOR_BUS_WIDTH/8 - 1), ProgramBuffered);
#endif

#if !defined (NO_WRITE)
    status = nor_status (index & ~(NOR_BUS_WIDTH/8 - 1));
    if (!(status & Ready)) {
      PRINTF ("nor_write failed program start 0x%lx (0x%x)\n",
	      index & ~(NOR_BUS_WIDTH/8 - 1), status);
      goto fail;
    }
#endif

	/* === Send the extent of the write.  We optimize (though I
	   don't really know why) if we don't need to write a whole
	   full buffer. */
    {
      int av = available + (index & (NOR_BUS_WIDTH/8 - 1));
#if defined (NO_WRITE)
      printf ("0x%lx <= 0x%02x\n", index & ~(NOR_BUS_WIDTH/8 - 1),
	      /* *** FIXME for 32 bit  */
	      av - av/2 - 1);
#else
      WRITE_ONE (index & ~(NOR_BUS_WIDTH/8 - 1),
	      /* *** FIXME for 32 bit  */
		 av - av/2 - 1);
#endif
    }

	/* === Either write the data, because we're aligned (first
	   case), or construct a buffer we can write because we're
	   not. */

    if (available == chip->writebuffer_size && ((unsigned long) pv & 1) == 0) {
      int i;
      for (i = 0; i < available; i += NOR_BUS_WIDTH/8) {
#if defined (NO_WRITE)
	printf ("0x%lx := 0x%04x\n", index + i,
                ((nor_t*)pv)[i/(NOR_BUS_WIDTH/8)]);
#else
	nor_t v = SWAP_ONE (((nor_t*)pv)[i/(NOR_BUS_WIDTH/8)]);
	WRITE_ONE (index + i, v);
#endif
      }
    }
    else {
      int i;
      char __aligned rgb[chip->writebuffer_size];
		     /* Fill with FFs to mask the unwritten bytes */
      memset (rgb, 0xff, chip->writebuffer_size);
//      rgb[0] = 0xff;						/* First */
//      rgb[((available + (index & 1) + 1)&~1) - 1] = 0xff;	/* Last */
//      printf ("  last %ld\n", ((available + (index & 1) + 1)&~1) - 1);
      memcpy (rgb + (index & (NOR_BUS_WIDTH/8 - 1)), pv, available);
      for (i = 0; i < available + (index & 1); i += NOR_BUS_WIDTH/8) {
#if defined (NO_WRITE)
	printf ("0x%lx #= 0x%04x\n",
		(index & ~(NOR_BUS_WIDTH/8 - 1)) + i,
		((nor_t*)rgb)[i/(NOR_BUS_WIDTH/8)]);
#else
	nor_t v = SWAP_ONE (((nor_t*)rgb)[i/(NOR_BUS_WIDTH/8)]);
	WRITE_ONE ((index & ~(NOR_BUS_WIDTH/8 - 1)) + i, v);
#endif
      }
    }

#if defined (NO_WRITE)
    printf ("0x%lx <= 0x%x\n", index & ~(NOR_BUS_WIDTH/8 - 1), ProgramConfirm);
#else
    WRITE_ONE (index & ~(NOR_BUS_WIDTH/8 - 1), ProgramConfirm);
#endif
    SPINNER_STEP;
    status = nor_status (index);

    vpen_disable ();

    SPINNER_STEP;

#if !defined (NO_WRITE)
    if (status & (ProgramError | VPEN_Low | DeviceProtected)) {
    fail:
      printf ("Program failed at 0x%p (%x)\n", (void*) index, status);
      CLEAR_STATUS (index);
      return cbWrote;
    }
#endif

    d->index += available;
    pv += available;
    cb -= available;
    cbWrote += available;
    //    printf ("  cb %x  cbWrote 0x%x  pv %p\n", cb, cbWrote, pv);
  }

  return cbWrote;
}

#else

/* nor_write

   does a little bit of fussing in order to handle writing single
   bytes and odd addresses.  This implementation is coded for a 16 bit
   device.  It is probably OK for 32 bit devices, too, since most of
   those are really pairs of 16 bit devices.

   Most chips can perform buffered writes.  This routine only writes
   one cell at a time because the logic is simpler and adequate.

   For saving on code size, this version is a win.

*/

static ssize_t nor_write (struct descriptor_d* d, const void* pv, size_t cb)
{
  size_t cbWrote = 0;
  unsigned long pageLast = ~0;

  while (cb) {
    unsigned long index = d->start + d->index;
    unsigned long page = index & ~ (nor_region (index)->size - 1);
    unsigned long data = 0;
    unsigned long status;
    int step = NOR_BUS_WIDTH/8;

    index = phys_from_index (index);

    if ((index & (NOR_BUS_WIDTH/8 - 1)) || cb < step) {
      step = (NOR_BUS_WIDTH/8) - (index & (NOR_BUS_WIDTH/8 - 1)); /* Max at index */
      if (step > cb)
	step = cb;
      data = ~0;
      memcpy ((unsigned char*) &data + (index & (NOR_BUS_WIDTH/8 - 1)), pv, step);
      index &= ~(NOR_BUS_WIDTH/8 - 1);
    }
    else
      memcpy (&data, pv, step);

    data = SWAP_ONE (data);	/* *** FIXME: do we really need memcpy? */

    vpen_enable ();
    if (page != pageLast) {
      status = nor_unlock_page (index);
      if (status & STAT (ProgramError | VPEN_Low | DeviceProtected))
	goto fail;
      pageLast = page;
    }

#if defined (NO_WRITE)
    printf ("nor_write [0x%lx]<-0x%0*lx  page 0x%lx  step %d  cb 0x%x\n",
	    index, NOR_BUS_WIDTH/4, (nor_t) data, page, step, cb);
    status = STAT (Ready);
#else
    nor_write_perform (index, data);
    status = nor_status (index);
#endif
    vpen_disable ();

    SPINNER_STEP;

    if (status & STAT (ProgramError | VPEN_Low | DeviceProtected)) {
    fail:
      printf ("Program failed at 0x%p (%lx)\n", (void*) index, status);
      CLEAR_STATUS (index);
      return cbWrote;
    }

    d->index += step;
    pv += step;
    cb -= step;
    cbWrote += step;
  }

  return cbWrote;
}

#endif

static void nor_erase (struct descriptor_d* d, size_t cb)
{
  if (d->index + cb > d->length)
    cb = d->length - d->index;

  SPINNER_STEP;

  while (cb > 0) {
    unsigned long index = d->start + d->index;
    const struct nor_region* region = nor_region (index);
    unsigned long available = region->size - (index & (region->size - 1));
    unsigned long status;

    index = phys_from_index (index);

    if (available > cb)
      available = cb;

    index &= ~(region->size - 1);

    vpen_enable ();
    status = nor_unlock_page (index);
#if defined (NO_WRITE)
    printf ("  unlock status %lx\n", status);
#endif
    if (status & STAT (ProgramError | VPEN_Low | DeviceProtected))
      goto fail;
#if defined (NO_WRITE)
    WRITE_ONE (index, CMD (ReadArray));
    printf ("fake erasing 0x%04x\n", REGA(index));
    printf ("nor_erase index 0x%lx  available 0x%lx\n",
	    index, available);
    status = STAT (Ready);
#else
    nor_erase_perform (index);
    status = nor_status (index);
#endif
    vpen_disable ();

    SPINNER_STEP;

    if (status & STAT (EraseError | VPEN_Low | DeviceProtected)) {
    fail:
      printf ("Erase failed at 0x%p (%lx)\n", (void*) index, status);
      CLEAR_STATUS (index);
      return;
    }

    cb -= available;
    d->index += available;
  }
}

int nor_query (struct descriptor_d* d, int index, void* pv)
{
  if (!chip)
    return ERROR_UNSUPPORTED;

  switch (index) {
  default:
    return ERROR_UNSUPPORTED;
  case QUERY_START:
    *(unsigned long*)pv = phys_from_index (0);
    break;
  case QUERY_SIZE:
    *(unsigned long*)pv = chip->total_size;
    break;
  case QUERY_ERASEBLOCKSIZE:
    *(unsigned long*)pv = nor_region (d->start + d->index)->size;
    break;
  }

  return 0;
}


/* nor_release

   has the sole mission to put the nor flash into read-array mode.
   This is necessary to guarantee that the reset command will be able
   to execute from flash.  Without this, it it possible that the reset
   command will pass control to a NOR flash array where the data read
   is the array status.

*/

static void nor_release (void)
{
  REGA (NOR_0_PHYS) = CMD (ReadArray);
#if defined (NOR_1_PHYS)
  REGA (NOR_1_PHYS) = CMD (ReadArray);
#endif
}


#if !defined (CONFIG_SMALL)

static void nor_report (void)
{
  int i;
  int size;
  char size_multiplier;
  int mismatched = false;

#if defined (USE_DETECT_ENDIAN_MISMATCH)
  if (endian_error) {
    printf ("  nor:    *** Endian Mismatch ***\n");
    return;
  }
#endif

  if (!chip)
    return;

  size = chip->total_size/1024;
  size_multiplier = 'K';
  if (size > 1024) {
    size /= 1024;
    size_multiplier = 'M';
  }

#if defined (CONFIG_DRIVER_NOR_CFI_TYPE_INTEL)
  if (chip->command_set != 1 && chip->command_set != 3)
    mismatched = true;
#endif
#if defined (CONFIG_DRIVER_NOR_CFI_TYPE_SPANSION)
  if (chip->command_set != 2 && chip->command_set != 4)
    mismatched = true;
#endif


  printf ("  nor:    0x%-8lx 0x%08lx (%3d %ciB) %dB write buffer"
#if defined (CONFIG_DRIVER_NOR_CFI_TYPE_INTEL)
	  " (Intel %d%s)"
#endif
#if defined (CONFIG_DRIVER_NOR_CFI_TYPE_SPANSION)
	  " (Spansion/AMD %d%s)"
#endif
	  "\n",
          phys_from_index (chip->region[0].start), chip->total_size,
          size, size_multiplier,
          chip->writebuffer_size,
          chip->command_set, mismatched ? " ERR" : ""
	  );

  printf ("          manufacturer 0x%02x device 0x%02x\n",
          chip->manufacturer_id, chip->device_id);

  for (i = 0; i < chip->regions; ++i)
    printf ("          region %d: %3d block%c of %6d (0x%05x) bytes\n",
	    i,
	    chip->region[i].count,
	    (chip->region[i].count > 1) ? 's' : ' ',
	    chip->region[i].size, chip->region[i].size);
}

#endif

static __driver_3 struct driver_d nor_driver = {
  .name = "nor-cfi",
  .description = "CFI NOR flash driver",
#if defined (USE_BUFFERED_WRITE)
  .flags = DRIVER_WRITEPROGRESS(5),
#else
  .flags = DRIVER_WRITEPROGRESS(2),
#endif
  .open = nor_open,
  .close = close_helper,
  .read = nor_read,
  .write = nor_write,
  .erase = nor_erase,
  .seek = seek_helper,
  .query = nor_query,
};

static __service_6 struct service_d cfi_nor_service = {
  .init = nor_init,
  .release = nor_release,
#if !defined (CONFIG_SMALL)
  .report = nor_report,
#endif
};
