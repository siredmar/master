/* drv-ata.c

   written by Marc Singer
   21 Sep 2008

   Copyright (C) 2008 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   o Testing

     # ubootimage -c -L 0x8000 -t kernel apex.bin apex.u

 dhcp ; setenv serverip 192.168.8.1 ; tftpboot 0x400000 apex.u ; bootm 0x400000
 im load ext2://2/boot/uImage

 ext2load

boot console=ttyS0,115200 root=/dev/md1 ro BOOT_MODE=normal runintime=14400 serialNo=5561n01d01g72800061j0sb


   -----------
   DESCRIPTION
   -----------

   Generic ATA/IDE driver.  This code is cribbed from the CF driver.
   It may be the case that this code can be merged into the CF driver
   and there be a single driver for both.  The CF standard is based on
   the ATA standard, so we expect that this is the case.  Still, it is
   safer to let this code start independently and merge once it works.


   o From the ATA/ATA6 standard:

   		CS1	CS0	DA2	DA1	DA0

     DataPort	0	0	X	X	X
     Data	0	1	0	0	0
     Error/	0	1	0	0	1
      Features	0	1	0	0	1
     Sector	0	1	0	1	0
     LBA Low	0	1	0	1	1
     LBA Mid	0	1	1	0	0
     LBA High	0	1	1	0	1
     Device	0	1	1	1	0
     Status	0	1	1	1	1
     DevCtrl	1	0	1	1	0


   o Generalizing the driver.  In general, this ought to be easy to do
     as long as the hardware supports the PIO ATA interface.  The
     trouble comes from errors in hardware design such as the Logic
     implementations of the LH7x microprocessors.  In the Logic
     designs, the hardware is incapable of performing byte I/O which
     would be necessary to isolate the DEVICE register from the
     COMMAND register.  A write to the DEVICE register must include a
     write to the COMMAND register.  To solve this, we can prepare a
     transaction and write an execute function that takes all of the
     parameters and performs the transaction.

     The LH7x designs suffer from another problem which is specific to
     the architecture, the IOBARRIER requirement.  In order to
     terminate a memory transaction, the CS line must be released by
     the CPU.  It will only do so when another CS line is selected.
     The ATA_IOBARRIER_PHYS macro declares the address that may be
     safely read to complete a memory transaction as in the Compact
     Flash implementation for the LH7x.


   o Identification data read from the MV2120 hard drive
00000000: 5a 0c ff 3f 37 c8 10 00  00 00 00 00 3f 00 00 00  Z..?7... ....?...
00000010: 00 00 00 00 20 20 20 20  20 20 20 20 20 20 20 20  ....
00000020: 51 39 32 47 48 39 53 4d  00 00 00 80 04 00 2e 33  Q92GH9SM .......3
00000030: 48 41 20 47 20 20 54 53  35 33 30 30 33 36 41 30  HA G  TS 530036A0
00000040: 20 53 20 20 20 20 20 20  20 20 20 20 20 20 20 20   S
00000050: 20 20 20 20 20 20 20 20  20 20 20 20 20 20 10 80                 ..
00000060: 00 00 00 2f 00 40 00 02  00 02 07 00 ff 3f 10 00  .../.@.. .....?..
00000070: 3f 00 10 fc fb 00 10 00  ff ff ff 0f 00 00 07 04  ?....... ........
00000080: 03 00 78 00 78 00 78 00  78 00 00 00 00 00 00 00  ..x.x.x. x.......
00000090: 00 00 00 00 00 00 1f 00  06 05 00 00 48 00 40 00  ........ ....H.@.
000000a0: fe 00 00 00 69 30 01 7c  23 40 69 30 01 3c 23 40  ....i0.| #@i0.<#@
000000b0: 3f 00 00 00 00 00 fe fe  fe ff 00 00 00 d0 00 00  ?....... ........
000000c0: 00 00 00 00 00 00 00 00  30 60 38 3a 00 00 00 00  ........ 0`8:....
000000d0: 00 00 00 00 00 40 00 00  00 00 00 00 00 00 00 00  .....@.. ........
000000e0: 00 00 00 00 00 00 00 00  00 00 00 01 00 00 06 00  ........ ........
000000f0: 04 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  ........ ........

00000000: 0c5a 3fff c837 0010  0000 0000 003f 0000  Z..?7... ....?...
00000010: 0000 0000 2020 2020  2020 2020 2020 2020  ....
00000020: 3951 4732 3948 4d53  0000 8000 0004 332e  Q92GH9SM .......3
00000030: 4148 4720 2020 5354  3335 3030 3633 3041  HA G  TS 530036A0
00000040: 5320 2020 2020 2020  2020 2020 2020 2020   S
00000050: 2020 2020 2020 2020  2020 2020 2020 8010                 ..
00000060: 0000 2f00 4000 0200  0200 0007 3fff 0010  .../.@.. .....?..
00000070: 003f fc10 00fb 0010  ffff 0fff 0000 0407  ?....... ........
00000080: 0003 0078 0078 0078  0078 0000 0000 0000  ..x.x.x. x.......
00000090: 0000 0000 0000 001f  0506 0000 0048 0040  ........ ....H.@.
000000a0: 00fe 0000 3069 7c01  4023 3069 3c01 4023  ....i0.| #@i0.<#@
000000b0: 003f 0000 0000 fefe  fffe 0000 d000 0000  ?....... ........
000000c0: 0000 0000 0000 0000  6030 3a38 0000 0000  ........ 0`8:....
000000d0: 0000 0000 4000 0000  0000 0000 0000 0000  .....@.. ........
000000e0: 0000 0000 0000 0000  0000 0100 0000 0006  ........ ........
000000f0: 0004 0000 0000 0000  0000 0000 0000 0000  ........ ........

   o Distinguising Compact Flash (CFA) from ATA.  The ATA standard
     states that the first word of the IDENTIFY DEVICE record will
     0x848a for CFA devices.  Bit 2 of word 83 will also be set
     indicating that the CFA commands are available.


   ==============================

   Generic CompactFlash IO driver

   o Byte-Swapping

     Nothing has been done to be careful about byte swapping and
     endianness.  I believe that adding this should be simple enough
     given access to a big-endian machine.  The hard part is that the
     CF standard has some built-in ordering.

   o Parameter Alignment

     The 3 byte padding in the parameter structure is added because it
     keeps the enclosing structure from misaligning.  This deserves
     more exploration.

   o This driver can be complicated on platforms where access to the
     CF device is challenged by the hardware design.

     o The LPD boards tend to require an IO_BARRIER after writes to
       latch the written data.  The need for an IO_BARRIER on read is
       less clear.

     o The Sharp LH79524 has a feature where the address lines to 16
       bit accessed devices are shifted one, granting a larger address
       space, but also changing the alignment of address lines to the
       CF slot on the LPD SDK board.

     o Early LPD SDK boards drive the low address line which may
       confuse some CF cards.  The LPD behavior is not compliant.  It
       appears that SanDisk CF cards are tolerant of the behavior and
       will perform in spite of A0 being driven.  (IIRC)

   o 16 bit addressing.

     This driver presently only allows for 16 bit access to the CF
     slot.  Other bus widths will be implemented when the hardware is
     available.

   o Timeout on status read

     It is imperative that a timeout be implemented in the status
     read.  As the code stands, it will hang if the CF card
     identifies, but the interface fails to show a ready status.

   o Attributes and & 0xff

     The attribute memory is only byte-valid on even addresses.  We
     mask the value with 0xff to prevent problems on some cards that
     return the same value in the high and low bytes of the 16 bit
     read.

*/

#include <config.h>
#include <apex.h>
#include <driver.h>
#include <service.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <spinner.h>
#include <asm/reg.h>
#include <error.h>

#include <mach/drv-ata.h>
#include <mach/hardware.h> /* *** FIXME: should be no need for generic ATA */

//#define TALK 1
//#define TALK_ATTRIB
#include "talk.h"


#if ATA_WIDTH == 16
# define REG __REG16
#elif ATA_WIDTH == 32
# define REG __REG
#else
# error Unable to build ATA driver with specified ATA_WIDTH
#endif

#if !defined (ATA_DEVICE_MAX)
#define ATA_DEVICE_MAX	(1)
#endif

#if !defined (ATA_REG)
# define ATA_REG 0
#endif
#if !defined (ATA_ALT)
# define ATA_ALT 0
#endif
#if !defined (ATA_ATTRIB)
# define ATA_ATTRIB 0
#endif

#define DRIVER_NAME	"ata-generic"

#define SECTOR_SIZE	512

#define IDE_SELECT_BITS	(0x00)  /* Used to be 0xa0 */

#define IDE_CMD_IDENTIFY	0xec
#define IDE_CMD_IDLE		0xe1
#define IDE_CMD_READSECTOR	0x20

#define IDE_REG_DATA		0x00
#define IDE_REG_FEATURES	0x02
#define IDE_REG_ERROR		0x02
#define IDE_REG_SECTORCOUNT	0x04
#define IDE_REG_LBALOW		0x06
#define IDE_REG_LBAMID		0x08
#define IDE_REG_LBAHIGH		0x0a
#define IDE_REG_DEVICE		0x0c
#define IDE_REG_COMMAND		0x0e
#define IDE_REG_STATUS		0x0e
#define IDE_REG_CONTROL		0x10
#define IDE_REG_ALTSTATUS	0x10

#define IDE_STATUS_BUSY		(1<<7)
#define IDE_STATUS_DRDY		(1<<6)
#define IDE_STATUS_DRQ		(1<<3)
#define IDE_STATUS_ERROR	(1<<0)

#if 0
#define ATTRIB_OPTION		0x00
#define ATTRIB_OPTION_LEVELREQ	(1<<6)
#define ATTRIB_CONFIGSTATUS	0x02
#define ATTRIB_PIN		0x04
#define ATTRIB_PIN_READY	(1<<1)
#endif


#define MS_READY_WAIT_TIMEOUT	(1*1000)

  /* IO_BARRIER_READ necessary on some platforms where the chip select
     lines don't transition sufficiently.  It is necessary on reads as
     well as writes, however without a cache the code herein works
     without a read barrier because the instruction fetches intervene.
  */

#if defined (ATA_IOBARRIER_PHYS)
# define IOBARRIER_READ	(*(volatile uint8_t*) ATA_IOBARRIER_PHYS)
#else
# define IOBARRIER_READ
#endif

#define USE_LBA

  /* This sleep was added for the sake of the reading a particular
     card on the 79520.  It isn't clear why it would be necessary as
     the 79524 has no problems and other cards are OK.  Better to
     perform the sleep and waste loads of time figuring out precise
     timings...for now.  Unfortunately, this delay make the IO
     unbearably slow.  Oh well. */
//#define DELAY usleep(1)
#define DELAY

enum {
  typeATA = 0x080000000,
  typeMultifunction = 0,
  typeMemory = 1,
  typeSerial = 2,
  typeParallel = 3,
  typeDisk = 4,
  typeVideo = 5,
  typeNetwork = 6,
  typeAIMS = 7,
  typeSCSI = 8,
  typeSecurity = 9,
};

struct ata_info {
  int type;
  char szFirmware[9];		/* Card firmware revision */
  char szName[41];		/* Name of card */
  int64_t total_sectors;
  int speed;			/* Timing value from CF info */

  /* *** FIXME: buffer should be in .xbss section */
  char rgb[SECTOR_SIZE];	/* Sector buffer */
  int sector;			/* Buffered sector */
  uint16_t identity[128];       /* IDE_CMD_IDENTIFY data */
};

static struct ata_info ata_d;
u8 drive_select;

#if 0
static uint8_t read8 (unsigned long phys, int reg)
{
  uint16_t v;
//  IOBARRIER_READ;
  v = REG (phys | ATA_REG | (reg & ~1)*ATA_ADDR_MULT);
  IOBARRIER_READ;
  return (reg & 1) ? (v >> 8) : (v & 0xff);
}

static uint8_t reada8 (unsigned long phys, int reg)
{
  uint16_t v;
  v = REG (phys | ATA_ATTRIB | (reg & ~1)*ATA_ADDR_MULT);
  IOBARRIER_READ;
  return (reg & 1) ? (v >> 8) : (v & 0xff);
}

static void write8 (unsigned long phys, int reg, uint8_t value)
{
  uint16_t v;
  v = REG (phys | ATA_REG | (reg & ~1)*ATA_ADDR_MULT);
//  printf ("write8 0x%x -> 0x%x  ",
//	  ATA_PHYS | ATA_REG | (reg & ~1)*ATA_ADDR_MULT, v);
  IOBARRIER_READ;
  v = (reg & 1) ? ((v & 0x00ff) | (value << 8)) : ((v & 0xff00) | value);
//  printf (" 0x%x\n", v);
  DELAY;
  REG (phys | ATA_REG | (reg & ~1)*ATA_ADDR_MULT) = v;
  IOBARRIER_READ;
}
#endif

#if 0
static void writea8 (int reg, uint8_t value)
{
  uint16_t v;
  v = REG (ATA_PHYS | ATA_ATTRIB | (reg & ~1)*ATA_ADDR_MULT);
  IOBARRIER_READ;
  v = (reg & 1) ? ((v & 0x00ff) | (value << 8)) : ((v & 0xff00) | value);
  DELAY;
  REG (ATA_PHYS | ATA_ATTRIB | (reg & ~1)*ATA_ADDR_MULT) = v;
  IOBARRIER_READ;
}
#endif

static uint16_t read16 (unsigned long phys, int reg)
{
  uint16_t value;
  value = REG (phys | ATA_REG | (reg & ~1)*ATA_ADDR_MULT);
  IOBARRIER_READ;
  return value;
}

static void write16 (unsigned long phys, int reg, uint16_t value)
{
//  IOBARRIER_READ;
//  printf ("write16: 0x%x <- 0x%x\n",
//	  ATA_PHYS | ATA_REG | (reg & ~1)*ATA_ADDR_MULT, value);
  REG (phys | ATA_REG | (reg & ~1)*ATA_ADDR_MULT) = value;
  IOBARRIER_READ;
}


static void ready_wait (unsigned long phys)
{
  unsigned long time = timer_read ();

  ENTRY (0);

  while (read16 (phys, IDE_REG_STATUS) & IDE_STATUS_BUSY
         && timer_delta (time, timer_read ()) < MS_READY_WAIT_TIMEOUT)
    ;
}

#if defined (TALK)
static void talk_registers (void)
{
#if 0
  printf ("ide: sec %04x cyl %04x stat %04x data %04x\n",
	  read16 (IDE_SECTORCOUNT),
	  read16 (IDE_CYLINDER),
	  read16 (IDE_SELECT),
	  read16 (IDE_DATA));
#endif
}
#endif

static void select (unsigned long phys, int drive, int head)
{
  ENTRY (0);

  drive_select = IDE_SELECT_BITS
#if defined USE_LBA
    | (1<<6)		/* Enable LBA mode */
#endif
    | (drive ? (1<<4) : 0);

  write16 (phys, IDE_REG_DEVICE, drive_select); // | (IDE_IDLE << 8));

  ready_wait (phys);
}

static void seek (unsigned long phys, unsigned sector)
{
  DBG (1,"[s %d", sector);

#if defined (USE_LBA)
  select (phys, 0, (sector >> 24) & 0xf);
  write16 (phys, IDE_REG_LBAHIGH, (sector >> 16) & 0xff);
  write16 (phys, IDE_REG_LBAMID,  (sector >>  8) & 0xff);
  write16 (phys, IDE_REG_LBALOW,  (sector >>  0) & 0xff);

  ready_wait (phys);

#else
  {
    unsigned head     = sector/(ata_d.cylinders*ata_d.sectors_per_track);
    unsigned cylinder = (sector%(ata_d.cylinders*ata_d.sectors_per_track))
      /ata_d.sectors_per_track;
    sector  %= ata_d.sectors_per_track;

    select (phys, 0, head);
    write16 (phys, IDE_SECTORCOUNT, 1 | (sector << 8));
    write16 (phys, IDE_CYLINDER, cylinder);
  }

  ready_wait (phys);

  DBG (1," %d %d %d", head, cylinder, sector);

#endif

  DBG (1,"]\n");

#if defined (TALK)
  talk_registers ();
#endif
}

static int ata_identify (unsigned long phys)
{
  ENTRY (1);


//  DBG (1, "selecting\n");
  select (phys, 0, 0);
//  DBG (1, "wait\n");
  ready_wait (phys);

//  write8 (IDE_COMMAND, IDE_IDENTIFY);
//  write16 (IDE_SELECT, (IDE_IDENTIFY << 8) | drive_select);
//  DBG (1, "identify command\n");
  write16 (phys, IDE_REG_COMMAND, IDE_CMD_IDENTIFY);
//  DBG (1, "wait\n");
  ready_wait (phys);

//  DBG (1, "reading data\n");
  {
    int i;
    memset (ata_d.identity, 0, sizeof (ata_d.identity));
    DBG (1, "reading identity data\n");
    for (i = 0; i < 128; ++i)
      ata_d.identity[i] = read16 (phys, IDE_REG_DATA);
//      rgs[i] = read16 (0);

//    DBG (1, "flush\n");
	/* Flush */
//    for (i = 0; i < 128; ++i)
    {
      int count;
      for (count = 128; count
             && (read16 (phys, IDE_REG_STATUS) & IDE_STATUS_DRQ);
           --count)
        read16 (phys, IDE_REG_DATA);
    }

#if defined (TALK) && TALK > 2
//    dump ((void*) ata_d.identity, 256, 0);
    dumpw ((void*) ata_d.identity, 256, 0, 2);
#endif

    if (ata_d.identity[0] == 0x848a)
      ata_d.type = typeMemory;
    else
      ata_d.type = typeATA;
//    {
//      ata_d.type = -1;
//      ERROR_RETURN (ERROR_IOFAILURE, "unexpected data read from CF card");
//    }

    if (ata_d.identity[83] & (1<<10)) {
      ata_d.total_sectors
        = (((uint64_t)ata_d.identity[102]) << 32)
        | (           ata_d.identity[101]  << 16)
        | (           ata_d.identity[100]  <<  0);
    }
    else
      ata_d.total_sectors
        = (ata_d.identity[ 61] << 16)
        | (ata_d.identity[ 60] <<  0);


    if (ata_d.total_sectors == 0) /* No device present */
      return -1;

    if (ata_d.identity[53] & (1<<1)) {
      ata_d.speed = ata_d.identity[67];
    }
    else
      ata_d.speed = -1;

#if defined (TALK)
    printf ("ata: 53 %x 80 %x\n", ata_d.identity[53], ata_d.identity[80]);
//    printf ("");
    printf ("    %Ld sectors\n", ata_d.total_sectors);
    printf ("%s: cap %x  pio_tim %x  trans %x  adv_pio %x\n",
	    __FUNCTION__, ata_d.identity[49], ata_d.identity[51],
            ata_d.identity[53], ata_d.identity[64]);
    printf ("  pio_tran %x  pio_trans_iordy %x\n",
            ata_d.identity[67], ata_d.identity[68]);
#endif

    for (i = 23; i < 27; ++i) {
      ata_d.szFirmware[(i - 23)*2]     =  ata_d.identity[i] >> 8;
      ata_d.szFirmware[(i - 23)*2 + 1] = (ata_d.identity[i] & 0xff);
    }
    for (i = 27; i < 47; ++i) {
      ata_d.szName[(i - 27)*2]     =  ata_d.identity[i] >> 8;
      ata_d.szName[(i - 27)*2 + 1] = (ata_d.identity[i] & 0xff);
    }
  }

  ata_d.sector = -1;

  return 0;
}

static int ata_open (struct descriptor_d* d)
{
  int result;
  int device = 0;
  unsigned long phys;
  ENTRY (0);

  /* Select the base address based on the disk number */
  if (d->iRoot)
    device = simple_strtoul (d->pb[0], NULL, 10);

  if (device < 0 || device >= ATA_DEVICE_MAX)
    device = 0;

  phys = ATA_PHYS (device);

  if ((result = ata_identify (phys)))
    return result;

  d->private = phys;

  DBG (1,"%s: opened #%d @%ld +%ld\n",
       __FUNCTION__, device, d->start, d->length);

  return 0;
}

static ssize_t ata_read (struct descriptor_d* d, void* pv, size_t cb)
{
  unsigned long phys = d->private;
  ssize_t cbRead = 0;

  if (d->index + cb > d->length)
    cb = d->length - d->index;

  DBG(1,"%s: @ 0x%lx\n", __FUNCTION__, d->start + d->index);

  while (cb) {
    unsigned long index = d->start + d->index;
    int availableMax = SECTOR_SIZE - (index & (SECTOR_SIZE - 1));
    int available = cb;
    int sector = index/SECTOR_SIZE;

    if (available > availableMax)
      available = availableMax;

		/* Read sector into buffer  */
    if (sector != ata_d.sector) {
      int i;

      DBG (2,"ata reading %d\n", sector);

      seek (phys, sector);
      write16 (phys, IDE_REG_COMMAND, IDE_CMD_READSECTOR);
      ready_wait (phys);
      ata_d.sector = sector;

      for (i = 0; i < SECTOR_SIZE/2; ++i)
	*(uint16_t*) &ata_d.rgb[i*2] = read16 (phys, IDE_REG_DATA);
    }

    memcpy (pv, ata_d.rgb + (index & (SECTOR_SIZE - 1)), available);

    d->index += available;
    cb -= available;
    cbRead += available;
    pv += available;
  }

  return cbRead;
}

/** Initialize ATA interface for Orion5x.  This code shall be moved to
    the machine specific modules once the driver works.

*/

static void ata_init (void)
{
  SATAHC_INT_COALESCE = 4;      /* Incident interrupts before IRQ signaled */
  SATAHC_INT_THRESH = 0x1d4c;   /* Clocks before IRQ signaled */

  SATAHC_MAIN_MASK = 0;         /* Mask all interrupts */
  /* *** enable LEDs */

/*     /\* disable Flash controller clock *\/ */
/*     regVal = MV_REG_READ_DWORD (pAdapter->adapterIoBaseAddress, */
/* 				MV_PCI_REGS_OFFSET + */
/* 				MV_PCI_EXPANSION_ROM_CONTROL_REG_OFFSET); */
/*     regVal &= ~(MV_BIT0); */
/*     MV_REG_WRITE_DWORD (pAdapter->adapterIoBaseAddress, */
/* 			MV_PCI_REGS_OFFSET + */
/* 			MV_PCI_EXPANSION_ROM_CONTROL_REG_OFFSET, regVal); */

//  SATAHC_MAIN_MASK = (1<<8) | (1<<2) | (1<<0);

	/* Initialize the channels */
  {
    int channel;
    for (channel = 0; channel < 2; ++channel) {
      SATAI_SCONTROL(channel) = SATAI_SCONTROL_NOPWR | SATAI_SCONTROL_RQINIT;
      udelay (1);                   /* 1ms delay *** VERIFY */
      SATAI_SCONTROL(channel) = SATAI_SCONTROL_NOPWR;
    }
  }
}


#if !defined (CONFIG_SMALL)

static void ata_report (void)
{
  int channel;

  ENTRY (0);

  for (channel = 0; channel < ATA_DEVICE_MAX; ++channel) {
    unsigned long status = SATAI_SSTATUS (channel);
    bool present = (((status >> SATAI_SSTATUS_DET_SH)
                     & SATAI_SSTATUS_DET_MSK) == 3);
    const char* speed;
    int size = 0;
    int fraction = 0;
    char chSize = 'M';

    if (present) {
      ata_identify (ATA_PHYS (channel));

      size = ata_d.total_sectors >> 11;     /* /2/1024 */
      fraction = ((ata_d.total_sectors & ((1<<11) - 1))*100)>>10;
      if (size >= 1000) {
        fraction = ((size & ((1<<10) - 1))*100) >> 10;
        size >>= 10;
        chSize = 'G';
      }
    }

    switch ((status >> SATAI_SSTATUS_SPD_SH) & SATAI_SSTATUS_SPD_MSK) {
    case 0:
    default:
      speed = "(speed?)";
      break;
    case 1:
      speed = "Gen1 (1.5 Mbps)";
      break;
    case 2:
      speed = "Gen2 (3.0 Mbps)";
      break;
    }

  /* *** FIXME: there is a function in the image code to convert a
     *** count of bytes into a human readale size string.  It could be
     *** adapted to cope with 48 bit LBAs by shifting by 10 on calling
     *** the function.. */

    printf ("  %s    #%d ", channel ? "    " : "ata:", channel);
    if (present) {
      printf ("%d.%02d %ciB %s",
              size, fraction, chSize, speed);
      if (ata_d.speed != -1)
        printf (" (%d ns)", ata_d.speed);
      printf ("\n%*s%s %s\n", 20, "", ata_d.szFirmware, ata_d.szName);
    }
    else
      printf ("absent\n");
  }
}

#endif

static __driver_3 struct driver_d ata_driver = {
  .name = DRIVER_NAME,
  .description = "ATA driver",
  .flags = DRIVER_WRITEPROGRESS(6) | DRIVER_DESCRIP_FS,
  .open = ata_open,
  .close = close_helper,
  .read = ata_read,
//  .write = ata_write,
//  .erase = ata_erase,
  .seek = seek_helper,
};

static __service_6 struct service_d ata_service = {
  .init   = ata_init,
#if !defined (CONFIG_SMALL)
  .report = ata_report,
#endif
};
