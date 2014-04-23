/* codec-lh79520.c

   written by Marc Singer
   8 Dec 2004

   Copyright (C) 2004 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   *** This is a hacked version of the original codec for the lh79524.
   *** As there is no I2S module in the CPU, the Logic PD designers
   *** added an I2S module to the CPLD.  This means that the
   *** configuration is somewhat more elaborate.  This implementation
   *** exists so that I can get the code working before attempting to
   *** integrate it.

   ========================================

   Test code for the audio codec.

   You can dump an 8 bit file with hexdump.  This one skips to the
   0x2c'th byte before dumping.

     hexdump -v -s 0x2c -n 32768 -e '8 1 "0x%02x, " "\n\t"' k3b_success1.wav

   A 16 bit stereo file:

     hexdump -v -s 0x108b0 -n 100000 -e '8 2 "0x%04x, " "\n\t"' track01.wav

  NOTES
  -----

  o There seems to be more at play in the signed versus unsigned
    arena.  The WAV format certainly uses signed integers for the
    sample values.
  o As slave, the CPU appears to properly sequence data out in that
    the data starts one (or perhaps two) cycles after the start of the
    IWS signal transition.
  o As master, the CPU appears to correctly delay the data for one
    clock cycle.  According to the I2S spec, the waveform is correct
    with the LSB appearing just after then IWS transition
  o In I2S CPU master mode, setting WSDEL to 1 breaks I2S framing.
    The SSP pulse isn't converted to a proper IWS pulse.

*/

#include <config.h>
#include <driver.h>
#include <service.h>
#include <linux/string.h>
#include <apex.h>
#include <command.h>
#include "hardware.h"

#define TALK 1

#if defined (TALK)
#define PRINTF(f...)		printf (f)
#else
#define PRINTF(f...)		do {} while (0)
#endif

#if defined (CONFIG_ARCH_LH79524)
# define USE_CPU_MASTER
# define USE_DMA
#endif
#define USE_I2S
#define USE_16
#define USE_STEREO
//#define USE_LOOPBACK_I2S
#define USE_SIGNED_CONVERSION
//#define USE_LOOPBACK_SSP
//#define USE_8KHZ
//#define USE_22KHZ
#define USE_44KHZ
#define USE_LOOPS	2000
//#define USE_LOOPS	5

//#define USE_E5
//#define USE_E5_RIGHT

#define DMA_CHANNEL	2
#define DMA_COUNT_MAX	(0xfff0)

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

#define T_SKH	1		/* Clock time high (us) */
#define T_SKL	1		/* Clock time low (us) */
#define T_CS	1		/* Minimum chip select low time (us)  */
#define T_CSS	1		/* Minimum chip select setup time (us)  */
#define T_DIS	1		/* Data setup time (us) */

#define CODEC_LIN_VOLUME	(0x0)
#define CODEC_RIN_VOLUME	(0x1)
#define CODEC_LOUT_VOLUME	(0x2)
#define CODEC_ROUT_VOLUME	(0x3)
#define CODEC_ANALOG_CTRL	(0x4)
#define CODEC_DIGITAL_CTRL	(0x5)
#define CODEC_POWER_CTRL	(0x6)
#define CODEC_DIGITAL_FORMAT	(0x7)
#define CODEC_SAMPLE_RATE	(0x8)
#define CODEC_DIGITAL_ACTIVATE	(0x9)
#define CODEC_RESET		(0xf)

#define CODEC_ADDR_SHIFT	(9)

#define CMD(a,c)	((((a) & 0x7f)<<CODEC_ADDR_SHIFT)|((c) & 0x1ff))

static void msleep (int ms)
{
  unsigned long time = timer_read ();

  do {
  } while (timer_delta (time, timer_read ()) < ms);
}

/* execute_spi_command

   sends a spi command to a device.  It first sends cwrite bits from
   v.  If cread is greater than zero it will read cread bits
   (discarding the leading 0 bit) and return them.  If cread is less
   than zero it will check for completetion status and return 0 on
   success or -1 on timeout.  If cread is zero it does nothing other
   than sending the command.

*/

static void execute_spi_command (int v, int cwrite)
{
  PRINTF ("spi 0x%04x -> 0x%x\n", v & 0x1ff, (v >> CODEC_ADDR_SHIFT) & 0x7f);

  CPLD_SPID = (v >> 8) & 0xff;
  CPLD_SPIC = CPLD_SPIC_LOAD | CPLD_SPIC_CS_CODEC;
  while (!(CPLD_SPIC & CPLD_SPIC_LOADED))
    ;
  CPLD_SPIC = CPLD_SPIC_CS_CODEC;
  while (!(CPLD_SPIC & CPLD_SPIC_DONE))
    ;

  CPLD_SPID = v & 0xff;
  CPLD_SPIC = CPLD_SPIC_LOAD | CPLD_SPIC_CS_CODEC;
  while (!(CPLD_SPIC & CPLD_SPIC_LOADED))
    ;
  CPLD_SPIC = CPLD_SPIC_CS_CODEC;
  while (!(CPLD_SPIC & CPLD_SPIC_DONE))
    ;

  CPLD_SPIC = 0;
}

static void codec_enable (void)
{
  execute_spi_command (CMD (CODEC_DIGITAL_ACTIVATE, (1<<0)), 16);
}

#if 0
static void codec_disable (void)
{
  execute_spi_command (CMD (CODEC_DIGITAL_ACTIVATE, (0<<0)), 16);
}
#endif

static void codec_unmute (void)
{
  execute_spi_command (CMD (CODEC_POWER_CTRL, 0
			    | (1<<2) /* Disable ADC  */
			    | (1<<1) /* Disable MIC  */
			    | (1<<0) /* Disable LINE IN */
			    ), 16);
  execute_spi_command (CMD (CODEC_DIGITAL_CTRL,
			    0
			    |(2<<1) /* 44.1 kHz deemphasis */
			    ), 16);
}

static void codec_mute (void)
{
  execute_spi_command (CMD (CODEC_DIGITAL_CTRL,
			    (1<<3) /* Soft mute */
			    |(2<<1) /* 44.1 kHz deemphasis */
			    ), 16);
}


/* codec_configure

   configures the codec sampling rate.  On the LPD79520, the base
   frequency is a crystal oscillator at 5.6448 MHz.  Unfortunately,
   this means that the codec is only capable of 44.1KHz, 22.05 KHz,
   and 4.011 KHz.

*/

static void codec_configure (int frequency, int sample_size)
{
  unsigned short v = 0;

  switch (frequency) {
  case 4000:			/* 4 kHz */
  case 4010:			/* 4 kHz */
  case 8021:			/* 4 kHz */
    v |= (0xb<<2)	/* SR3-SR0 */
      |  (0<<1)		/* BOSR (256) */
      |  (0<<0);	/* Normal mode */
    v |= (0<<7)|(0<<6);	/* !MCLK/2 */
//    v |= (1<<7)|(1<<6);	/* MCLK/2 */
    break;
  case 22050:			/* 22.05 KHz */
    v |= (0x8<<2)	/* SR3-SR0 */
      |  (0<<1)		/* BOSR (256) */
      |  (0<<0);	/* Normal mode */
    v |= (0<<7)|(0<<6);	/* !MCLK/2 */
    break;
  case 11025:			/* 11.25 KHz */
    v |= (0x8<<2)	/* SR3-SR0 */
      |  (0<<1)		/* BOSR (256) */
      |  (0<<0);	/* Normal mode */
    v |= (1<<7)|(1<<6);	/* MCLK/2 */
    break;
  default:
  case 44100:			/* 44.1 KHz */
    v |= (0xf<<2)	/* SR3-SR0 */
      |  (0<<1)		/* BOSR (256) */
      |  (0<<0);	/* Normal mode */
    v |= (0<<7)|(0<<6);	/* !MCLK/2 */
    break;
  }

  PRINTF ("configuring codec for %d Hz with 0x%x\n", frequency, v);
  execute_spi_command (CMD (CODEC_SAMPLE_RATE, v), 16);

  RCPC_CTRL |= RCPC_CTRL_UNLOCK;
  MASK_AND_SET (RCPC_CTRL, 3<<5, 0<<5);	/* System osc. -> CLKOUT */
  RCPC_CTRL &= ~RCPC_CTRL_UNLOCK;
}

static void codec_init (void)
{
  RCPC_CTRL |=  RCPC_CTRL_UNLOCK;
  RCPC_AHBCLKCTRL &= ~(1<<0); /* Enable DMA AHB clock */
  RCPC_CTRL &= ~RCPC_CTRL_UNLOCK;

  IOCON_DMAMUX |= (1<<2) | (1<<1) | (1<<0); /* Allow DREQ0, nDACK0, DEOT0 */

  DMA_CTRL(DMA_CHANNEL) = 0
    | (1<<13)			/* Peripheral destination */
    | (2<<7)			/* Destination size 4 bytes */
    | (2<<3)			/* Source size is 4 bytes */
    | (0<<9)			/* Wrapping: Load base addresses on start  */
    | (0<<2)			/* Destination fixed */
    | (1<<1);			/* Source incremented */

  DMA_CLR   = 0xff;
  DMA_MASK |= (1 << DMA_CHANNEL); /* Enable of DMA channel */

//  codec_mute ();

  execute_spi_command (CMD (CODEC_RESET, 0), 16);
  execute_spi_command (CMD (CODEC_DIGITAL_CTRL,
			    0
			    |(2<<1) /* 44.1 kHz deemphasis */
			    ), 16);
  execute_spi_command (CMD (CODEC_DIGITAL_FORMAT,
			    0
			    | (1<<6) /* Master */
//			    | (1<<4) /* LRP: right channel on LRC low */
			    | (0<<2) /* 16 bit */
//			    | (3<<2) /* 32 bit */
			    | (2<<0) /* I2S format */
			    ), 16);
  codec_configure (SAMPLE_FREQUENCY, 16);

  codec_enable ();
}

static void codec_release (void)
{
}


#if defined (USE_16)
typedef unsigned short buffer_t;
#else
typedef unsigned char buffer_t;
#endif
static buffer_t __attribute__((section(".pcm.bss"))) buffer[128*1024];

/* convert_source

   returns the number of samples used in the buffer.

*/

static int convert_source (void)
{
  int source_samples = C_SAMPLES;
  int buffer_samples = sizeof (buffer)/sizeof (buffer[0]);
  sample_t* rgb = (sample_t*) &rgbPCM[0];
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
    //    v >>= 1;


#if defined (USE_E5)
# if defined (USE_16)
    if (is & 1)
      v = 0x80bb;
      //      v = 0xaaaa;
    else
      v = 0x7f02;
      //      v = 0xaaaa;
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

static int cmd_codec_test (int argc, const char** argv)
{
  int samples = convert_source ();
  int index;			/* Index for DMA */
  int count;
  int loops =
#if defined USE_LOOPS
      USE_LOOPS
#else
      0
#endif
    ;

  codec_unmute ();

  samples /= 2;			/* We're DMA'ing  32 bits wide */
  printf ("samples %d\n", samples);

 restart:
  index = 0;

//  CPLD_GPIO ^= (1<<0);

  DMA_DESTLO(DMA_CHANNEL)   =  CPLD_I2S_PHYS & 0xffff;
  DMA_DESTHI(DMA_CHANNEL)   = (CPLD_I2S_PHYS >> 16) & 0xffff;

 play_more:
  count = samples - index;
  if (count > DMA_COUNT_MAX)
    count = DMA_COUNT_MAX;

  DMA_CTRL(DMA_CHANNEL) &= ~(1<<0); /* Disable */
  DMA_SOURCELO(DMA_CHANNEL)
    =  ((unsigned long)(buffer + (index*2))) & 0xffff;
  DMA_SOURCEHI(DMA_CHANNEL)
    = (((unsigned long)(buffer + (index*2))) >> 16) & 0xffff;
  DMA_MAX(DMA_CHANNEL)	= count;

  //  printf ("source 0x%lx\n", (unsigned long)(buffer + index));

  DMA_CLR = 0xff;

  DMA_CTRL(DMA_CHANNEL) |= (1<<0);		/* Enable TX DMA */

				/* Wait for completion */
  while ((DMA_STATUS & (1 << DMA_CHANNEL)) == 0)
    ;

  index += count;
  if (index < samples)
    goto play_more;

  if (loops--) {
    goto restart;
  }


  codec_mute ();
  return 0;
}

static __service_7 struct service_d lpd79524_codec_service = {
  .init = codec_init,
  .release = codec_release,
};

static __command struct command_d c_codec = {
  .command = "codec",
  .description = "test codec",
  .func = cmd_codec_test,
};
