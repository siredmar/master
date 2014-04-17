/* codec.c

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

#if defined (CONFIG_ARCH_LH79524) || defined (CONFIG_ARCH_LH79525)
# define USE_CPU_MASTER
# define USE_DMA
#endif
#define USE_I2S
//#define USE_DMA_CAP
#define USE_16
#define USE_STEREO
//#define USE_LOOPBACK_I2S
#define USE_SIGNED_CONVERSION
//#define USE_LOOPBACK_SSP
#define USE_8KHZ
//#define USE_22KHZ
//#define USE_44KHZ
#define USE_LOOPS	2000
//#define USE_LOOPS	5

//#define USE_E5
//#define USE_E5_RIGHT


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

#define SSP_CTRL0	__REG(SSP_PHYS + 0x00)
#define SSP_CTRL1	__REG(SSP_PHYS + 0x04)
#define SSP_DR_PHYS	(SSP_PHYS + 0x08)
#define SSP_DR		__REG(SSP_PHYS + 0x08)
#define SSP_SR		__REG(SSP_PHYS + 0x0c)
#define SSP_CPSR	__REG(SSP_PHYS + 0x10)
#define SSP_IMSC	__REG(SSP_PHYS + 0x14)
#define SSP_RIS		__REG(SSP_PHYS + 0x18)
#define SSP_MIS		__REG(SSP_PHYS + 0x1c)
#define SSP_ICR		__REG(SSP_PHYS + 0x20)
#define SSP_DCR		__REG(SSP_PHYS + 0x24)

#define SSP_CTRL1_MS	(1<<2)
#define SSP_CTRL1_LBM	(1<<0)
#define SSP_CTRL1_SSE	(1<<1)

#define SSP_SR_BSY	(1<<4)
#define SSP_SR_REFI	(1<<3)
#define SSP_SR_RNE	(1<<2)	/* Receive FIFO not empty */
#define SSP_SR_TNF	(1<<1)	/* Transmit FIFO not full */
#define SSP_SR_TFE	(1<<0)	/* Transmit FIFO empty */

#if defined (CONFIG_ARCH_LH79524) || defined (CONFIG_ARCH_LH79525)
# define I2S_CTRL	__REG(I2S_PHYS + 0x00)
# define I2S_STAT	__REG(I2S_PHYS + 0x04)
# define I2S_IMSC	__REG(I2S_PHYS + 0x08)
# define I2S_RIS	__REG(I2S_PHYS + 0x0c)
# define I2S_MIS	__REG(I2S_PHYS + 0x10)
# define I2S_ICR	__REG(I2S_PHYS + 0x14)

# define I2S_CTRL_LOOP	(1<<5)
# define I2S_CTRL_MCLKINV (1<<4)
# define I2S_CTRL_WSDEL	(1<<3)
# define I2S_CTRL_WSINV	(1<<2)
# define I2S_CTRL_I2SEN	(1<<1)
# define I2S_CTRL_I2SEL	(1<<0)

# define I2S_STAT_MS	(1<<8)
# define I2S_STAT_RFF	(1<<7)
# define I2S_STAT_RFE	(1<<6)	/* Receive FIFO not empty from SSP */
# define I2S_STAT_TFF	(1<<5)
# define I2S_STAT_TFE	(1<<4)	/* Transmit FIFO empty from SSP */
# define I2S_STAT_TXWS	(1<<3)
# define I2S_STAT_RXWS	(1<<2)
# define I2S_STAT_WS	(1<<1)
# define I2S_STAT_LBM	(1<<0)
#endif

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

void ssp_set_speed (int speed);

static void msleep (int ms)
{
  unsigned long time = timer_read ();

  do {
  } while (timer_delta (time, timer_read ()) < ms);
}

static void enable_cs (void)
{
#if defined (CONFIG_MACH_LPD79520)
  CPLD_SPIC |= CPLD_SPIC_CODEC;
#endif
#if defined (CONFIG_MACH_LPD79524)
  CPLD_SPI &= ~(1<<5);
#endif
  usleep (T_CSS);
}

static void disable_cs (void)
{
  usleep (T_CS);
#if defined (CONFIG_MACH_LPD79520)
  CPLD_SPIC &= ~CPLD_SPIC_CODEC;
#endif
#if defined (CONFIG_MACH_LPD79524)
  CPLD_SPI |=  (1<<5);
#endif
  usleep (T_CS);
}

static void pulse_clock (void)
{
  unsigned char reg = CPLD_SPI & ~CPLD_SPI_SCLK;
  CPLD_SPI = reg | CPLD_SPI_SCLK;
  usleep (T_SKH);
  CPLD_SPI = reg;
  usleep (T_SKL);
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
  unsigned char reg;

  PRINTF ("spi 0x%04x -> 0x%x\n", v & 0x1ff, (v >> CODEC_ADDR_SHIFT) & 0x7f);
  enable_cs ();

  reg = CPLD_SPI & ~CPLD_SPI_TX;
  v <<= CPLD_SPI_TX_SHIFT; /* Correction for position of SPI_TX bit */
  while (cwrite--) {
    CPLD_SPI = reg | ((v >> cwrite) & CPLD_SPI_TX);
    usleep (T_DIS);
    pulse_clock ();
  }

  disable_cs ();
}

static void codec_enable (void)
{
  execute_spi_command (CMD (CODEC_DIGITAL_ACTIVATE, (1<<0)), 16);
}

static void codec_disable (void)
{
  execute_spi_command (CMD (CODEC_DIGITAL_ACTIVATE, (0<<0)), 16);
}

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

static void codec_configure (int frequency, int sample_size)
{
  unsigned short v = 0;

  switch (frequency) {
  default:
  case 4000:			/* 4 kHz */
  case 4010:			/* 4 kHz */
    v |= (0xb<<2) /* SR3-SR0 */
      |  (0<<1); /* BOSR */
    v |= (1<<7)|(1<<6);		/* MCLK/2 */
    break;
  case 8000:			/* 8 kHz */
  case 8021:			/* 8 kHz */
    v |= (0xb<<2) /* SR3-SR0 */
      |  (0<<1); /* BOSR */
    break;
  case 32100:
    break;
  case 22050:
    v |= (0x8<<2) /* SR3-SR0 */
      |  (0<<1); /* BOSR */
    v |= (1<<7)|(1<<6);		/* MCLK/2 */
    break;
  case 44100:
    v |= (0x8<<2) /* SR3-SR0 */
      |  (0<<1); /* BOSR */
    break;
  case 48000:
    break;
  case 88200:
    v |= (0xf<<2) /* SR3-SR0 */
      |  (0<<1); /* BOSR */
    break;
  case 96000:
    break;
  }

  PRINTF ("configuring codec for %d Hz with 0x%x\n", frequency, v);
  execute_spi_command (CMD (CODEC_SAMPLE_RATE, v), 16);

  MASK_AND_SET (SSP_CTRL0, 0xf, (sample_size - 1) & 0xf);

#if defined (USE_CPU_MASTER)
  ssp_set_speed (frequency);
#else
  RCPC_CTRL |= RCPC_CTRL_UNLOCK;
  MASK_AND_SET (RCPC_CTRL, 3<<5, 0<<5);	/* System osc. -> CLKOUT */
  RCPC_CTRL &= ~RCPC_CTRL_UNLOCK;
#endif
}

void ssp_set_speed (int speed)
{
  int rcpc_prescale;
  int ssp_dvsr;
  int ssp_cpd;

  switch (speed) {
  default:
  case 8012:
    /* .08% error at HCLK 50MHz */
    rcpc_prescale = 32;
    ssp_dvsr = 2;
    ssp_cpd = 99;
    break;
  case 22050:
    /* No error at HCLK 50MHz */
    rcpc_prescale = 32 ;
    ssp_dvsr = 2;
    ssp_cpd = 36;
    break;
  case 44100:
    /* No error at HCLK 50MHz */
    rcpc_prescale = 32 ;
    ssp_dvsr = 2;
    ssp_cpd = 18;
    break;
  }

  if (ssp_cpd == 0)
    ssp_cpd = 1;

  rcpc_prescale /= 32;		/* Compensate for the frame size */

  PRINTF ("ssp_set_speed  rcpc_ssppre %d  dvsr %d  cpd %d\n",
	  rcpc_prescale, ssp_dvsr, ssp_cpd);

  RCPC_CTRL |= RCPC_CTRL_UNLOCK;
  RCPC_PCLKSEL1 &= ~(1<<1);	/* HCLK -> SSP clock */
  RCPC_SSPPRE = rcpc_prescale>>1;
  SSP_CPSR = ssp_dvsr;
  MASK_AND_SET (SSP_CTRL0, (0xff<<8), (ssp_cpd - 1)<<8);
  RCPC_CTRL &= ~RCPC_CTRL_UNLOCK;
}


static void codec_init (void)
{
  SSP_CTRL0 = 0;
  SSP_CTRL1 = 0;

  RCPC_CTRL |= RCPC_CTRL_UNLOCK;
  RCPC_PCLKCTRL1 &= ~(1<<1); /* Enable SSP clock */
  RCPC_CTRL &= ~RCPC_CTRL_UNLOCK;


#if defined (I2S_CTRL) && defined (USE_I2S)
  I2S_CTRL = 0;
#endif

  SSP_IMSC &= 0xff;		/* Mask everything */

  MASK_AND_SET (IOCON_MUXCTL5,
		(3<<6)|(3<<4)|(3<<2)|(3<<0),
		(1<<6)|(1<<4)|(1<<2)|(1<<0));	/* SSP/I2S signals */
//  IOCON_RESCTL5 &= ~((3<<6)|(3<<4)|(3<<2)|(3<<0));

  MASK_AND_SET (SSP_CTRL0, (3<<4), (1<<4)); /* TI mode */
#if defined (USE_CPU_MASTER)
  SSP_CTRL1 &= ~(1<<2);	/* CPU as master */
#else
  SSP_CTRL1 |=  (1<<2);	/* CPU as slave */
#endif

#if defined (USE_LOOPBACK_SSP)
  SSP_CTRL1 |= SSP_CTRL1_LBM;	/* Loopback */
#endif

#if defined (USE_16)
  MASK_AND_SET (SSP_CTRL0, 0xf, ((16 - 1) & 0xf));
#else
  MASK_AND_SET (SSP_CTRL0, 0xf, ((8  - 1) & 0xf));
#endif

#if defined (USE_DMA)
  RCPC_CTRL |= RCPC_CTRL_UNLOCK;
  RCPC_AHBCLKCTRL &= ~(1<<0); /* Enable DMA AHB clock */
  RCPC_CTRL &= ~RCPC_CTRL_UNLOCK;

  DMA0_CTRL
    = (0<<13)			/* Peripheral source */
    | (0<<9)			/* Load base addresses on start  */
    | (1<<7)			/* Destination size 2 bytes */
    | (1<<3)			/* Source size is 2 bytes */
    | (0<<2)			/* Destination fixed */
    | (0<<1);			/* Source fixed */

  DMA1_CTRL
    = (1<<13)			/* Peripheral destination */
#if defined (USE_16)
    | (1<<7)			/* Destination size 2 bytes */
    | (1<<3)			/* Source size is 2 bytes */
/* These are removed because it seems that they break the DMA controller */
//    | (1<<5)			/* Source burst 4 incrementing */
#else
    | (0<<7)			/* Destination size 1 bytes */
    | (0<<3)			/* Source size is 1 byte */
//    | (1<<5)			/* Source burst 4 incrementing */
#endif
    | (0<<9)			/* Wrapping: Load base addresses on start  */
    | (0<<2)			/* Destination fixed */
    | (1<<1);			/* Source incremented */

  DMA_CLR   = 0xff;
  DMA_MASK |= (1<<1);		/* Enable of DMA channel 1 */
#endif

#if defined (USE_DMA)
  SSP_DCR = 0
    | (1<<1)			/* TX DMA enabled */
    | (1<<0)			/* RX DMA enabled */
    ;
#else
  SSP_DCR = 0;
#endif

//  codec_mute ();

  execute_spi_command (CMD (CODEC_RESET, 0), 16);
  execute_spi_command (CMD (CODEC_DIGITAL_CTRL,
			    0
			    |(2<<1) /* 44.1 kHz deemphasis */
			    ), 16);
  execute_spi_command (CMD (CODEC_DIGITAL_FORMAT,
			    0
#if defined (USE_CPU_MASTER)
			    | (0<<6) /* Slave */
#else
			    | (1<<6) /* Master */
#endif
			    | (1<<4) /* LRP: right channel on LRC low */
			    | (0<<2) /* 16 bit */
//			    | (3<<2) /* 32 bit */
#if defined (USE_I2S)
			    | (2<<0) /* I2S format */
#else
			    | (3<<0) /* DSP format */
#endif
			    ), 16);
  codec_configure (SAMPLE_FREQUENCY, 16);

  codec_enable ();

//  printf ("ssp: status 0x%lx\n", SSP_SR);
}

static void codec_release (void)
{
  RCPC_CTRL      |= RCPC_CTRL_UNLOCK;
  RCPC_PCLKCTRL1 |= (1<<1);	/* Disable SSP clock */
  RCPC_CTRL	 &= ~RCPC_CTRL_UNLOCK;
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

#if defined (USE_DMA)
  codec_unmute ();

#if defined (USE_DMA_CAP)
  DMA0_CTRL    &= ~(1<<0); /* Disable */
  DMA0_SOURCELO =  SSP_DR_PHYS        & 0xffff;
  DMA0_SOURCEHI = (SSP_DR_PHYS >> 16) & 0xffff;
  DMA0_DESTLO   =  ((unsigned long)&cap)        & 0xffff;
  DMA0_DESTHI   = (((unsigned long)&cap) >> 16) & 0xffff;
  DMA0_MAX	= 0xffff;
#endif

 restart:
  index = 0;

 play_more:
  count = samples;
  if (index + count > samples)
    count = samples - index;
  if (count > 65534)
    count = 65534;

  DMA1_CTRL    &= ~(1<<0); /* Disable */
  DMA1_SOURCELO =  ((unsigned long)(buffer + index))        & 0xffff;
  DMA1_SOURCEHI = (((unsigned long)(buffer + index)) >> 16) & 0xffff;
  DMA1_DESTLO   =  SSP_DR_PHYS        & 0xffff;
  DMA1_DESTHI   = (SSP_DR_PHYS >> 16) & 0xffff;
  DMA1_MAX	= count;

  DMA_CLR = 0xff;

#if defined (USE_DMA_CAP)
  DMA0_CTRL |= (1<<0);		/* Enable RX DMA */
#endif
  DMA1_CTRL |= (1<<0);		/* Enable TX DMA */

#if defined (I2S_CTRL) && defined (USE_I2S)
  I2S_CTRL |= 0
    | I2S_CTRL_I2SEN | I2S_CTRL_I2SEL
#if defined (USE_LOOPBACK_I2S)
    | I2S_CTRL_LOOP
#endif
    ;
#endif
  SSP_CTRL1 |= SSP_CTRL1_SSE;		/* Enable SSP  */

				/* Wait for completion */
  while ((DMA_STATUS & (1<<1)) == 0)
    ;

  index += count;
  if (index < samples)
    goto play_more;

  if (loops--) {
    goto restart;
  }

#else

  codec_unmute ();

  SSP_CTRL1 |= (1<<1);		/* Enable SSP */
#if defined (USE_I2S)
  I2S_CTRL
    = I2S_CTRL_I2SEN | I2S_CTRL_I2SEL
#if defined (USE_LOOPBACK_I2S)
    | I2S_CTRL_LOOP
#endif
    ;
#endif

  {
    int j =
#if defined USE_LOOPS
      USE_LOOPS
#else
      1
#endif
      ;
    while (j--) {
      int i;
      for (i = 0; i < samples; ++i) {
	/* Wait for room in the FIFO */
	while (!(SSP_SR & SSP_SR_TNF))
	  ;
	SSP_DR = buffer[i];
      }
    }
  }

#endif /* !USE_DMA */

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
