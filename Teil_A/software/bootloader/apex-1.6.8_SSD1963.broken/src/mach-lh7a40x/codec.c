/* codec.c

   written by Marc Singer
   12 Jul 2005

   Copyright (C) 2005 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   Test code for the AC97 audio codec.

   You can dump an 8 bit file with hexdump.  This command skips to the
   0x2c'th byte before dumping.

     hexdump -v -s 0x2c -n 32768 -e '8 1 "0x%02x, " "\n\t"' k3b_success1.wav

   A 16 bit stereo file:

     hexdump -v -s 0x108b0 -n 100000 -e '8 2 "0x%04x, " "\n\t"' track01.wav

  NOTES
  -----

  o There seems to be more at play in the signed versus unsigned
    arena.  The WAV format certainly uses signed integers for the
    sample values.

*/

#include <config.h>
#include <driver.h>
#include <service.h>
#include <linux/string.h>
#include <apex.h>
#include <command.h>
#include <console.h>
#include "hardware.h"

#define TALK 2

#if defined (TALK)
#define PRINTF(f...)		printf (f)
#define ENTRY(l)		printf ("%s\n", __FUNCTION__)
#define DBG(l,f...)		do { if (TALK >= l) printf (f); } while (0)
#else
#define PRINTF(f...)		do {} while (0)
#define ENTRY(l)		do {} while (0)
#define DBG(l,f...)		do {} while (0)
#endif

#define USE_DMA
#define USE_16
#define USE_STEREO
#define USE_SIGNED_CONVERSION
#define USE_COMPACT_MODE
#define USE_8KHZ
//#define USE_22KHZ
//#define USE_44KHZ
//#define USE_LOOPS	2000
#define USE_LOOPS	2

//#define USE_E5
//#define USE_E5_RIGHT

#define DMA_LENGTH_MAX	(0x10000 - 4)

#define DMAC_CHAN DMAC_M2P4
//#define DMAC_CHAN DMAC_M2P5

//#define US_FRAME	30
#define US_FRAME	100

#if defined (USE_8KHZ)
# include <audio/pcm8-8.h>
# define SAMPLE_FREQUENCY 8021
#endif

#if defined (USE_22KHZ)// || defined (USE_44KHZ)
# include <audio/pcm2205-16.h>
# define SAMPLE_FREQUENCY 22050
#endif

#if defined (USE_44KHZ)// && 0 // || defined (USE_22KHZ)
# include <audio/pcm441-16b.h>
# define SAMPLE_FREQUENCY 44100
#endif

#define AC97_DR1	__REG(AC97_PHYS + 0x00)
#define AC97_RXCR1	__REG(AC97_PHYS + 0x04)
#define AC97_TXCR1	__REG(AC97_PHYS + 0x08)
#define AC97_SR1	__REG(AC97_PHYS + 0x0c)
#define AC97_RISR1	__REG(AC97_PHYS + 0x10)
#define AC97_ISR1	__REG(AC97_PHYS + 0x14)
#define AC97_IE1	__REG(AC97_PHYS + 0x18)

#define AC97_DR2	__REG(AC97_PHYS + 0x20)
#define AC97_RXCR2	__REG(AC97_PHYS + 0x24)
#define AC97_TXCR2	__REG(AC97_PHYS + 0x28)
#define AC97_SR2	__REG(AC97_PHYS + 0x2c)
#define AC97_RISR2	__REG(AC97_PHYS + 0x30)
#define AC97_ISR2	__REG(AC97_PHYS + 0x34)
#define AC97_IE2	__REG(AC97_PHYS + 0x38)

#define AC97_DR3	__REG(AC97_PHYS + 0x40)
#define AC97_RXCR3	__REG(AC97_PHYS + 0x44)
#define AC97_TXCR3	__REG(AC97_PHYS + 0x48)
#define AC97_SR3	__REG(AC97_PHYS + 0x4c)
#define AC97_RISR3	__REG(AC97_PHYS + 0x50)
#define AC97_ISR3	__REG(AC97_PHYS + 0x54)
#define AC97_IE3	__REG(AC97_PHYS + 0x58)

#define AC97_DR4	__REG(AC97_PHYS + 0x60)
#define AC97_RXCR4	__REG(AC97_PHYS + 0x64)
#define AC97_TXCR4	__REG(AC97_PHYS + 0x68)
#define AC97_SR4	__REG(AC97_PHYS + 0x6c)
#define AC97_RISR4	__REG(AC97_PHYS + 0x70)
#define AC97_ISR4	__REG(AC97_PHYS + 0x74)
#define AC97_IE4	__REG(AC97_PHYS + 0x78)

#define AC97_S1DATA	__REG(AC97_PHYS + 0x80)
#define AC97_S2DATA	__REG(AC97_PHYS + 0x84)
#define AC97_S12DATA	__REG(AC97_PHYS + 0x88)
#define AC97_RGIS	__REG(AC97_PHYS + 0x8c)
#define AC97_GIS	__REG(AC97_PHYS + 0x90)
#define AC97_GIEN	__REG(AC97_PHYS + 0x94)
#define AC97_GEOI	__REG(AC97_PHYS + 0x98)
#define AC97_GCR	__REG(AC97_PHYS + 0x9c)
#define AC97_RESET	__REG(AC97_PHYS + 0xa0)
#define AC97_SYNC	__REG(AC97_PHYS + 0xa4)
#define AC97_GCIS	__REG(AC97_PHYS + 0xa8)

#define AC97_CR_EN		(1<<0)
#define AC97_CR_SLOT(c)		(1<<(c))
#define AC97_CR_SIZE_16		(0<<13)
#define AC97_CR_SIZE_18		(1<<13)
#define AC97_CR_SIZE_20		(2<<13)
#define AC97_CR_SIZE_12		(3<<13)
#define AC97_CR_CM		(1<<15)
#define AC97_CR_FDIS		(1<<16)

#define AC97_SR_TXUE		(1<<6)
#define AC97_SR_RXOE		(1<<5)
#define AC97_SR_TXBUSY		(1<<4)
#define AC97_SR_TXFF		(1<<3)
#define AC97_SR_RXFF		(1<<2)
#define AC97_SR_TXFE		(1<<1)
#define AC97_SR_RXFE		(1<<0)

#define AC97_GCR_IFE		(1<<0)
#define AC97_GCR_LOO		(1<<1)
#define AC97_GCR_OCR		(1<<1)

#define AC97_ISR_SLOT1TXCMP	(1<<0)
#define AC97_ISR_SLOT2INTRX	(1<<1)
#define AC97_ISR_GPIOTXCMP	(1<<2)
#define AC97_ISR_GPIOINTRX	(1<<3)
#define AC97_ISR_WIS		(1<<4)
#define AC97_ISR_CODECREADY	(1<<5)
#define AC97_ISR_SLOT2TXCMP	(1<<6)

#define AC97_RESET_TIMEDRESET	(1<<0)

#define CODEC_RATE_8000			0x1f40
#define CODEC_RATE_11025		0x2b11
#define CODEC_RATE_16000		0x3e80
#define CODEC_RATE_22050		0x5622
#define CODEC_RATE_32000		0x7d00
#define CODEC_RATE_44100		0xac44
#define CODEC_RATE_48000		0xbb80

#define REG_RESET			0x00
#define REG_MASTER_VOLUME		0x02
#define REG_AUX_VOLUME			0x04
#define REG_HEADPHONE_VOLUME		0x04
#define REG_MONO_VOLUME			0x06
#define REG_MASTER_TONE			0x08
#define REG_PC_BEEP			0x0a
#define REG_PHONE_VOLUME		0x0c
#define REG_MIC_VOLUME			0x0e
#define REG_LINEIN_VOLUME		0x10
#define REG_CD_VOLUME			0x12
#define REG_VIDEO_VOLUME		0x14
#define REG_AUXIN_VOLUME		0x16
#define REG_PCMOUT_VOLUME		0x18
//#define reg_				0x72
#define REG_RECORD_SELECT		0x1a
#define REG_RECORD_GAIN			0x1c
#define REG_GP				0x20
#define REG_POWER			0x26
#define REG_STATUS			0x26
#define REG_EXTENDED_ID			0x28
#define REG_EXTENDED_STATUS		0x2a

#define REG_RATE_DAC_FRONT		0x2c
#define REG_RATE_DAC_SURR		0x2e
#define REG_RATE_DAC_LFE		0x30
#define REG_RATE_ADC_LR			0x32
#define REG_RATE_ADC_AUX		0x34

#define REG_VENDORID1			0x7c
#define REG_VENDORID2			0x7e

#define VOL_LOCKED(v)			(((v) & 0x7f) | (((v) & 0x7f)<<8))
#define VOL_MUTE(v)			((1<<15) | VOL_LOCKED(v))

static u16 ac97_fetch (int reg)
{
  int count;

  AC97_S2DATA;			/* Clear slot2 IRQ status */
  AC97_S1DATA = reg;

  for (count = 1024; !(AC97_RGIS & AC97_ISR_SLOT1TXCMP) && count; --count)
    ;
  DBG (3,"tx terminal count %d\n", count);

  for (count = 1024; !(AC97_RGIS & AC97_ISR_SLOT2INTRX) && count; --count)
    ;

  DBG (3,"rx terminal count %d\n", count);

  usleep (US_FRAME);	/* 21.1.8: delay one frame before reading  */

  if (count)
    return AC97_S2DATA;

  return 0;	/* *** FIXME: shouldn't this be an error? */
}

static inline int wait_for_powerup (void)
{
  int count = 1024;

  do {
    if (ac97_fetch (REG_POWER))
      return 1;
  } while (count--);

  return 0;
}

static void ac97_store (int reg, u16 value)
{
  int count;

  AC97_S2DATA = value;		/* Set value */
  AC97_S1DATA = reg;		/*  before register */

  usleep (US_FRAME);			/* ??? */

  for (count = 1024; !(AC97_RGIS & AC97_ISR_SLOT1TXCMP) && count; --count)
    ;

  if (reg == REG_RESET)
    wait_for_powerup ();
}

static void codec_configure (int frequency, int sample_size)
{
  unsigned short v;

  ac97_store (REG_EXTENDED_STATUS, 1<<0);	/* Variable Rate Audio */

  switch (frequency) {
  case 8000:			/* 8 kHz */
  case 8021:			/* 8 kHz */
    v = CODEC_RATE_8000;
    break;
  case 11025:
    v = CODEC_RATE_11025;
    break;
  case 16000:
    v = CODEC_RATE_16000;
    break;
  case 22050:
    v = CODEC_RATE_22050;
    break;
  case 32000:
    v = CODEC_RATE_32000;
    break;
  case 44100:
    v = CODEC_RATE_44100;
    break;
  default:
  case 48000:
    v = CODEC_RATE_48000;
    break;
  }

  DBG (2,"configuring codec for %d Hz with 0x%x\n", frequency, v);

  ac97_store (REG_RATE_DAC_FRONT, v);

  DBG (3, "sampling 0x%04x 0x%04x 0x%04x\n",
       ac97_fetch (REG_RATE_DAC_FRONT),
       ac97_fetch (REG_RATE_DAC_SURR),
       ac97_fetch (REG_RATE_DAC_LFE));
}

static void codec_unmute (void)
{
  ac97_store (REG_MASTER_VOLUME,	VOL_LOCKED (1));
  ac97_store (REG_HEADPHONE_VOLUME,	VOL_LOCKED (1));
  ac97_store (REG_MONO_VOLUME,		VOL_LOCKED (1));
  ac97_store (REG_PCMOUT_VOLUME,	VOL_LOCKED (1));
}

static void codec_mute (void)
{
  ac97_store (REG_MASTER_VOLUME,	VOL_MUTE (1));
  ac97_store (REG_HEADPHONE_VOLUME,	VOL_MUTE (1));
  ac97_store (REG_MONO_VOLUME,		VOL_MUTE (1));
  ac97_store (REG_PCMOUT_VOLUME,	VOL_MUTE (1));
}

static void codec_init (void)
{
  ENTRY (0);

  __REG(GPIO_PHYS + GPIO_PINMUX) &= ~(1<<2); /* AC97 CODECON */
  AC97_GCR |= AC97_GCR_IFE;		/* Enable AC97 link */

  DBG (1, "%s: reset\n", __FUNCTION__);
  AC97_RESET |= AC97_RESET_TIMEDRESET;

  DBG (1, "%s: waiting for powerup\n", __FUNCTION__);
  if (!wait_for_powerup ()) {
    DBG (0, "codec didn't power-up\n");
    return;
  }

  DBG (0, "%s: power-up\n", __FUNCTION__);

  DBG (1, "%s: reset 0x%0x\n", __FUNCTION__, ac97_fetch (REG_RESET));
#if TALK >= 1
  {
    u16 id1 = ac97_fetch (REG_VENDORID1);
    u16 id2 = ac97_fetch (REG_VENDORID2);

    DBG (1, "%s: vendor id '%c%c%c%c' (0x%0x 0x%0x)\n", __FUNCTION__,
	 id1 >> 8, id1 & 0xff, id2 >> 8, (id2 & 0xff) + '0', id1, id2);
  }
#endif

  codec_mute ();

  codec_configure (SAMPLE_FREQUENCY, 16);

//  codec_enable ();
}

static void codec_release (void)
{
//  AC97_GCR &= ~AC97_GCR_IFE;		/* Disable AC97 link */
}


#if defined (USE_16)
typedef unsigned short buffer_t;
#else
typedef unsigned char buffer_t;
#endif
static buffer_t __attribute__((section(".pcm.xbss"))) buffer[128*1024];

/* convert_source

   returns the number of samples used in the buffer.

*/

static int convert_source (void)
{
  int source_samples = C_SAMPLES;
  int buffer_samples = sizeof (buffer)/sizeof (*buffer);
  sample_t* rgb = (sample_t*) rgbPCM;
  int is;
  int ib;

#if defined (SOURCE_MONO) && defined (USE_STEREO)
  source_samples *= 2;
#endif

#if defined (SOURCE_STEREO) && !defined (USE_STEREO)
  source_samples /= 2;
#endif

  if (buffer_samples > source_samples)
    buffer_samples = source_samples;

  PRINTF ("source");
#if defined (SOURCE_SIGNED)
  PRINTF (" signed");
#else
  PRINTF (" unsigned");
#endif
#if defined (SOURCE_8BIT)
  PRINTF (" 8 bit");
#endif
#if defined (SOURCE_16BIT)
  PRINTF (" 16 bit");
#endif
#if defined (SOURCE_MONO)
  PRINTF (" mono");
#endif
#if defined (SOURCE_STEREO)
  PRINTF (" stereo");
#endif
  PRINTF (" %d samples to", source_samples);
#if defined (USE_16)
  PRINTF (" 16 bit");
#else
  PRINTF (" 8 bit");
#endif
#if defined (USE_STEREO)
  PRINTF (" stereo");
#else
  PRINTF (" mono");
#endif
  PRINTF (" %d samples\n", buffer_samples);

  is = 0;
  ib = 0;
  while (ib < buffer_samples) {
    buffer_t v;
#if defined (SOURCE_8BIT) && !defined (USE_16)
    v = rgb[is++];
#elif defined (SOURCE_8BIT) &&  defined (USE_16)
    v = rgb[is]<<8;
    //    v = rgb[is]<<8 | rgb[is];
    ++is;
#elif defined (SOURCE_16BIT) && !defined (USE_16)
    v = rgb[is++]>>8;
#elif defined (SOURCE_16BIT) &&  defined (USE_16)
    v = rgb[is++];
#else
    error "Whoops!";
#endif

    /* Convert to unsigned */
#if defined (USE_SIGNED_CONVERSION)
#if defined (SOURCE_SIGNED)
# if defined (USE_16)
    v = (signed short) v + 0x8000;
# else
    v = (signed char)  v + 0x80;
# endif
#endif
#endif

    /* Modulate everyone for the sake of headphones */
    v >>= 1;


#if defined (USE_E5)
# if defined (USE_16)
    if (is & 1)
      v = 0x80bb;
    else
      v = 0x7f02;
# else
    if (is & 1)
      v = 0x8b;
    else
      v = 0x7f;
# endif
#endif

#if defined (USE_E5_RIGHT)
# if defined (USE_16)
    if (is & 1)
      v = 0x80bb;
# else
    if (is & 1)
      v = 0x8b;
# endif
#endif

#if   defined (SOURCE_MONO)   && !defined (USE_STEREO)
    buffer[ib++] = v;
#elif defined (SOURCE_STEREO) &&  defined (USE_STEREO)
    buffer[ib++] = v;
#elif defined (SOURCE_MONO)   &&  defined (USE_STEREO)
    buffer[ib++] = v;
    buffer[ib++] = v;
#elif defined (SOURCE_STEREO) && !defined (USE_STEREO)
    if ((is & 1) == 0)
      buffer[ib++] = v;
#else
error
#endif
  }

  return buffer_samples;
}

static void print_status (unsigned long v)
{
  printf (" 0x%04lx: b %ld  nb %ld  s (%ld)",
	  v,
	  (v >> 7) & 0x1f,
	  (v >> 6) & 1,
	  (v >> 4) & 3);
  switch ((v >> 4) & 3) {
  default:
  case 0: printf (" idle"); break;
  case 1: printf (" stall"); break;
  case 2: printf (" on"); break;
  case 3: printf (" next"); break;
  }
  if (v & (1<<3))
    printf (" cherint");
  if (v & (1<<1))
    printf (" nfbrint");
  if (v & (1<<0))
    printf (" stallrint");
}

static int cmd_codec_test (int argc, const char** argv)
{
  int samples = convert_source ();
  int loops =
#if defined USE_LOOPS
      USE_LOOPS
#else
      0
#endif
    ;

#if defined (USE_DMA)

 {
   int index;			/* Index for DMA */
   int count;
   int length;			/* Byte length of audio array */
   int ac97_enabled = 0;

   DBG (2, "%s: codec setup\n", __FUNCTION__);

   codec_unmute ();

   AC97_GCR &= ~AC97_GCR_IFE;	/* Clear the FIFOs */
   usleep (100);
   AC97_GCR |= AC97_GCR_IFE;
   usleep (100);

   AC97_RXCR1 = 0;		/* Disable */
   AC97_TXCR1 = 0;

//   CSC_PWRCNT &= ~(CSC_PWRCNT_DMAC_M2P4_EN | CSC_PWRCNT_DMAC_M2P5_EN);
//   usleep (100);
   CSC_PWRCNT |= CSC_PWRCNT_DMAC_M2P4_EN | CSC_PWRCNT_DMAC_M2P5_EN;
//   usleep (100);
//   DMAC_P_PPALLOC (DMAC_CHAN) = 2;	  /* Port 4/5, AC97-1 */

   DMAC_P_MAXCNT0 (DMAC_CHAN) = 0;
   DMAC_P_BASE0 (DMAC_CHAN) = 0;
   DMAC_P_MAXCNT1 (DMAC_CHAN) = 0;
   DMAC_P_BASE1 (DMAC_CHAN) = 0;


   DBG (2, "%s: dma enable\n", __FUNCTION__);
//   DMAC_P_PCONTROL (DMAC_CHAN) &= ~DMAC_PCONTROL_ENABLE;
   DMAC_P_PCONTROL (DMAC_CHAN) |= DMAC_PCONTROL_ENABLE;
   DMAC_P_PCONTROL (DMAC_CHAN);
   //   DBG (2, "%s: dma set count\n", __FUNCTION__);
   //   DMAC_P_MAXCNT1 (DMAC_CHAN)   = 0;
   //   DBG (2, "%s: dma set base\n", __FUNCTION__);
   //   DMAC_P_BASE1 (DMAC_CHAN)     = 0; // (unsigned long) buffer;

//   DMAC_P_PINTERRUPT (DMAC_CHAN) = 0xb;

   DBG (2, "RGIS 0x%lx  RISR 0x%lx\n", AC97_RGIS, AC97_RISR1);

   length = samples*2;

 restart:
   printf ("restart\n");
   index = 0;

 play_more:
   count = length - index;
   if (count > DMA_LENGTH_MAX)
     count = DMA_LENGTH_MAX;

   if (DMAC_P_PSTATUS (DMAC_CHAN) & DMAC_PSTATUS_NEXTBUF) {
     DBG (2, "%s: load 1 %d\n   ", __FUNCTION__, index);
     print_status (DMAC_P_PSTATUS (DMAC_CHAN));
     printf ("\n");
     DMAC_P_MAXCNT1 (DMAC_CHAN)   = count;
     DMAC_P_BASE1 (DMAC_CHAN)     = (unsigned long) ((void*)buffer + index);
   }
   else {
     DBG (2, "%s: load 0 %d\n   ", __FUNCTION__, index);
     print_status (DMAC_P_PSTATUS (DMAC_CHAN));
     printf ("\n");
     DMAC_P_MAXCNT0 (DMAC_CHAN)   = count;
     DMAC_P_BASE0 (DMAC_CHAN)     = (unsigned long) ((void*)buffer + index);
   }

   printf (" ->");
   print_status (DMAC_P_PSTATUS (DMAC_CHAN));
   printf ("\n");

//   DBG (2, "%s: waiting for completion of %d %d %d\n", __FUNCTION__,
//	count, index, length);

   index += count;

   if (!ac97_enabled) {
     AC97_TXCR1 = AC97_CR_EN
       | AC97_CR_SLOT(3) | AC97_CR_SLOT(4)
       | AC97_CR_SIZE_16
#if defined (USE_COMPACT_MODE)
       | AC97_CR_CM
#endif
       ;
   }

				/* Wait for rooom in the queue */
   if (index < length || loops--) {
//     static unsigned long vPrev = 0;
     while (((DMAC_P_PSTATUS (DMAC_CHAN) >> 4) & 0x3) == 0x3) {
//       unsigned long v = DMAC_P_PSTATUS (DMAC_CHAN);
//       v &= ~(0x1f << 7);
//       if (v != vPrev) {
//	 print_status (v);
//	 printf ("\n");
//	 vPrev = v;
//       }
       if (console->poll (0, 1)) {
	 int ch;
	 console->read (0, &ch, 1);
	 goto done;
       }
     }
     if (index < length)
       goto play_more;

     goto restart;		/* loops */
   }


   printf ("AC97_S 0x%lx\n", AC97_SR1);
   DBG (2, "%s: waiting for stall\n", __FUNCTION__);
   print_status (DMAC_P_PSTATUS (DMAC_CHAN));
   printf ("\n");

				/* Wait for stall */
   while ((DMAC_P_PSTATUS (DMAC_CHAN) & (1<<0)) == 0) {
     if (console->poll (0, 1)) {
       int ch;
       console->read (0, &ch, 1);
       goto done;
     }
   }

 done:
   ;

   AC97_RXCR1 = 0;
   AC97_TXCR1 = 0;

   {
     unsigned long status;
     for (status = DMAC_P_PSTATUS (DMAC_CHAN);
	  ((status >> 7) & 0x1f) & 0x3;
	  status = DMAC_P_PSTATUS (DMAC_CHAN)) {
       AC97_TXCR1 = AC97_CR_EN
	 | AC97_CR_SLOT(5)
	 | AC97_CR_SIZE_16
#if defined (USE_COMPACT_MODE)
	 | AC97_CR_CM
#endif
	 ;
       AC97_TXCR1 = 0;

       DBG (1,"term: st 0x%lx  b %ld\n",  status, (status >> 7) & 0x1f);
     }
   }

   DMAC_P_PCONTROL (DMAC_CHAN) = 0; /* Disable the channel */
   DMAC_P_PCONTROL (DMAC_CHAN);

   CSC_PWRCNT &= ~(CSC_PWRCNT_DMAC_M2P4_EN | CSC_PWRCNT_DMAC_M2P5_EN);
 }

#endif

#if !defined (USE_DMA)

  AC97_RXCR1 = 0;		/* Disable */
  AC97_TXCR1 = AC97_CR_EN
    | AC97_CR_SLOT(3) | AC97_CR_SLOT(4)
    | AC97_CR_SIZE_16
#if defined (USE_COMPACT_MODE)
    | AC97_CR_CM
#endif
    ;

  codec_unmute ();

  {
    int j = 1;

#if defined USE_LOOPS
    j = USE_LOOPS;
#endif

    while (j--) {
      int i;
//      DBG (1, "sending data\n");
      for (i = 0; i < samples; ) {
	/* Wait for room in the FIFO */
	while (AC97_SR1 & AC97_SR_TXFF)
	  ;
#if defined (USE_COMPACT_MODE)
	AC97_DR1 = buffer[i] | (buffer[i+1]<<16);
	i += 2;
#else
	AC97_DR1 = buffer[i];
	++i;
#endif

	if (console->poll (0, 1)) {
	  int ch;
	  console->read (0, &ch, 1);
	  j = 0;
	  break;
	}
      }
    }
  }

#endif /* !USE_DMA */

  codec_mute ();
  return 0;
}

static __service_7 struct service_d lpd7a40x_codec_service = {
  .init = codec_init,
  .release = codec_release,
};

static __command struct command_d c_codec = {
  .command = "codec",
  .description = "test codec",
  .func = cmd_codec_test,
};
