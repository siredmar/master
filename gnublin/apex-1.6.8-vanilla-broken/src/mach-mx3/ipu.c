/* ipu.c

   written by Marc Singer
   17 Apr 2007

   Copyright (C) 2007 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   o I2C Timing.  This code appears to have a slightly different wave
     form from the code in Matt Jakuc's test code.  The start of the
     frame, signaled by the initial falling clock pulse from the
     master has a long delay in this code, but a very short delay in
     Matt's.  I'm not sure why and it doesn't appear to make a
     difference in the correctness of the I2C frames.

   o Releasing the I2C bus.  It appears to take some time for the bus
     to go not-busy once it is released (in i2c_stop).  The trouble
     with this code is that we should allow for the bus to be taken
     over by another master.  In this boot loader, it isn't really an
     issue, but a driver should cope with the fact that sending STOP
     is enough and we really need to wait for the bus to go idle when
     we want to start a transaction.

   o Calculating the IFDR divisor.  Unfortunately, the Freescale I2C
     core doesn't use a simple divisor register to divide the base
     frequency of 66MHz.  We're using a divisor (1280) that yields
     52KHz as the I2C clock.  If we want to be able to calculate the
     divisor from the target frequency, we'll have a tough row to hoe.

   o I2C Node Addressing.  The I2C bus uses a seven bit address field
     where the node address is in the upper seven bits of the byte and
     the R/W bit is in the lowest bit.  The addresses passed around
     are the 'real' address of 0-127.

   o I2C_I2SR_RXAK.  This bit is set when the ACK is not found on the
     transfer of a byte.  It really should be named NRXAK.  We use the
     name that exists in the documentation.

   o I2C writing and reading.  The protocol for reading and writing
     registers on the sensor is kinda odd since we are accessing them
     with an 8 bit bus.  To read a register, write the one-byte
     register address and then read once to get the high byte.  Write
     the special address 0xf0 and read again to get the low byte.
     Writing is similar except that there are no read cycles and the
     special register address is 0xf1.

   o BUF?_RDY.  We would expect that clearing these bits would prevent
     the IDMAC from writing to that buffer, but that doesn't seem to
     be the case.  The IDMAC writes to the buffers regardless of the
     setting of these bits.

*/

#include <config.h>
#include <apex.h>
#include <command.h>
#include <service.h>
#include <error.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <asm/mmu.h>

#include "hardware.h"

#undef ENTRY
//#define ENTRY printf ("%s\n", __FUNCTION__)
#if !defined (ENTRY)
# define ENTRY
#endif

//#define MODE_VF

#define MODE_GENERIC

#if defined (MODE_GENERIC)

//# define FRAME_WIDTH		(752)
//# define FRAME_HEIGHT		(480)

# define FRAME_WIDTH		(640)
# define FRAME_HEIGHT		(480)

//# define FRAME_WIDTH		(170)
//# define FRAME_HEIGHT		(616)

//# define FRAME_WIDTH		(176)
//# define FRAME_HEIGHT		(616)

//# define FRAME_WIDTH		(608)
//# define FRAME_HEIGHT		(170)

//# define FRAME_WIDTH		(160)
//# define FRAME_HEIGHT		(80)
# define FRAME_WIDTH_DIVISOR	(1)
//# define FRAME_HORZ_BLANKING	(43)
//# define FRAME_VERT_BLANKING	(4)

# define BYTES_PER_PEL		(2)
#endif

//#define I2C_IFDR_V		(0x19)			/* Divisor of 1280 */
#define I2C_IFDR_V		(0x3f)			/* Divisor of 2048 */

#define IPU_CONF		__REG(PHYS_IPU + 0x00)
#define IPU_CHA_BUF0_RDY	__REG(PHYS_IPU + 0x04)
#define IPU_CHA_BUF1_RDY	__REG(PHYS_IPU + 0x08)
#define IPU_CHA_DB_MODE_SEL	__REG(PHYS_IPU + 0x0c)
#define IPU_CHA_CUR_BUF		__REG(PHYS_IPU + 0x10)
#define IPU_FS_PROC_FLOW	__REG(PHYS_IPU + 0x14)
#define IPU_DISP_PROC_FLOW	__REG(PHYS_IPU + 0x18)
#define IPU_TASK_STAT		__REG(PHYS_IPU + 0x1c)
#define IPU_IMA_ADDR		__REG(PHYS_IPU + 0x20)
#define IPU_IMA_DATA		__REG(PHYS_IPU + 0x24)
#define IPU_INT_CTRL1		__REG(PHYS_IPU + 0x28)
#define IPU_INT_CTRL2		__REG(PHYS_IPU + 0x2c)
#define IPU_INT_CTRL3		__REG(PHYS_IPU + 0x30)
#define IPU_INT_CTRL4		__REG(PHYS_IPU + 0x34)
#define IPU_INT_CTRL5		__REG(PHYS_IPU + 0x38)
#define IPU_INT_STAT1		__REG(PHYS_IPU + 0x3c)
#define IPU_INT_STAT2		__REG(PHYS_IPU + 0x40)
#define IPU_INT_STAT3		__REG(PHYS_IPU + 0x44)
#define IPU_INT_STAT4		__REG(PHYS_IPU + 0x48)
#define IPU_INT_STAT5		__REG(PHYS_IPU + 0x4c)
#define IPU_BRK_CTRL1		__REG(PHYS_IPU + 0x50)
#define IPU_BRK_CTRL2		__REG(PHYS_IPU + 0x54)
#define IPU_BRK_STAT		__REG(PHYS_IPU + 0x58)
#define IPU_DIAGB_CTRL		__REG(PHYS_IPU + 0x5c)
#define CSI_CONF		__REG(PHYS_IPU + 0x60)
#define CSI_SENS_CONF		__REG(PHYS_IPU + 0x60)
#define CSI_SENS_FRM_SIZE	__REG(PHYS_IPU + 0x64)
#define CSI_ACT_FRM_SIZE	__REG(PHYS_IPU + 0x68)
#define CSI_OUT_FRM_CTRL	__REG(PHYS_IPU + 0x6c)
#define CSI_TST_CTRL		__REG(PHYS_IPU + 0x70)
#define CSI_CCIR_CODE1		__REG(PHYS_IPU + 0x74)
#define CSI_CCIR_CODE2		__REG(PHYS_IPU + 0x78)
#define CSI_CCIR_CODE3		__REG(PHYS_IPU + 0x7c)
#define CSI_FLASH_STROBE1	__REG(PHYS_IPU + 0x80)
#define CSI_FLASH_STROBE2	__REG(PHYS_IPU + 0x84)
#define IC_CONF			__REG(PHYS_IPU + 0x88)
#define IC_PRP_ENC_RSC		__REG(PHYS_IPU + 0x8c)
#define IC_PRP_VF_RSC		__REG(PHYS_IPU + 0x90)
#define	IC_PP_RSC		__REG(PHYS_IPU + 0x94)
#define	PF_CONF			__REG(PHYS_IPU + 0xa0)
#define	IDMAC_CONF		__REG(PHYS_IPU + 0xa4)
#define	IDMAC_CHA_EN		__REG(PHYS_IPU + 0xa8)
#define	IDMAC_CHA_PRI		__REG(PHYS_IPU + 0xac)
#define	IDMAC_CHA_BUSY		__REG(PHYS_IPU + 0xb0)

#define IPU_CONF_CSI_EN		(1<<0)
#define IPU_CONF_IC_EN		(1<<1)
#define IPU_CONF_ROT_EN		(1<<2)
#define IPU_CONF_PF_EN		(1<<3)
#define IPU_CONF_SDC_EN		(1<<4)
#define IPU_CONF_ADC_EN		(1<<5)
#define IPU_CONF_DI_EN		(1<<6)
#define IPU_CONF_DU_EN		(1<<7)
#define IPU_CONF_PXL_ENDIAN	(1<<8)

#define IPU_CHAN_IC0		(1<<0)
#define IPU_CHAN_IC1		(1<<1)
#define IPU_CHAN_IC2		(1<<2)
#define IPU_CHAN_IC3		(1<<3)
#define IPU_CHAN_IC4		(1<<4)
#define IPU_CHAN_IC5		(1<<5)
#define IPU_CHAN_IC6		(1<<6)
#define IPU_CHAN_IC7		(1<<7)
#define IPU_CHAN_IC8		(1<<8)
#define IPU_CHAN_IC9		(1<<9)
#define IPU_CHAN_IC10		(1<<10)
#define IPU_CHAN_IC11		(1<<11)
#define IPU_CHAN_IC12		(1<<12)
#define IPU_CHAN_IC13		(1<<13)
#define IPU_CHAN_SDC0		(1<<14)
#define IPU_CHAN_SDC1		(1<<15)
#define IPU_CHAN_SDC2		(1<<16)
#define IPU_CHAN_SDC3		(1<<17)
#define IPU_CHAN_ADC2		(1<<18)
#define IPU_CHAN_ADC3		(1<<19)
#define IPU_CHAN_ADC4		(1<<20)
#define IPU_CHAN_ADC5		(1<<21)
#define IPU_CHAN_ADC6		(1<<22)
#define IPU_CHAN_ADC7		(1<<23)
#define IPU_CHAN_PF0		(1<<24)
#define IPU_CHAN_PF1		(1<<25)
#define IPU_CHAN_PF2		(1<<26)
#define IPU_CHAN_PF3		(1<<27)
#define IPU_CHAN_PF4		(1<<28)
#define IPU_CHAN_PF5		(1<<29)
#define IPU_CHAN_PF6		(1<<30)
#define IPU_CHAN_PF7		(1<<31)

#define IDMAC_CONF_PRYM_RR		(0<<0) /* Round robin */
#define IDMAC_CONF_PRYM_RAND		(1<<0) /* Random */
#define IDMAC_CONF_SRCNT_SH		(4)
#define IDMAC_CONF_SRCNT_MSK		(0x7)
#define IDMAC_CONF_SINGLE_AHB_M_EN	(1<<8)

#define CSI_CONF_VSYNC_POL		(1<<0)
#define CSI_CONF_HSYNC_POL		(1<<1)
#define CSI_CONF_DATA_POL		(1<<2)
#define CSI_CONF_SENS_PIX_CLK_POL	(1<<3)
#define CSI_CONF_SENS_PRTCL_SH		(4)
#define CSI_CONF_SENS_PRTCL_GATED	(0<<4)
#define CSI_CONF_SENS_PRTCL_NONGATED	(1<<4)
#define CSI_CONF_SENS_PRTCL_CCIR_P	(2<<4)
#define CSI_CONF_SENS_PRTCL_CCIR_NP	(3<<4)
#define CSI_CONF_SENS_CLK_SRC		(1<<7)
#define CSI_CONF_SENS_DATA_FORMAT_RGB	(0<<8)
#define CSI_CONF_SENS_DATA_FORMAT_YUV444 (0<<8)
#define CSI_CONF_SENS_DATA_FORMAT_YUV422 (2<<8)
#define CSI_CONF_SENS_DATA_FORMAT_BAYER (3<<8)
#define CSI_CONF_SENS_DATA_FORMAT_GENERIC (3<<8)
#define CSI_CONF_DATA_WIDTH_4BIT	(0<<10)
#define CSI_CONF_DATA_WIDTH_8BIT	(1<<10)
#define CSI_CONF_DATA_WIDTH_10BIT	(2<<10)
#define CSI_CONF_DATA_WIDTH_15BIT	(3<<10)	/* Bayer or generic */
#define CSI_CONF_EXT_VSYNC		(1<<15)
#define CSI_CONF_DIV_RATIO_SH		(16)

#define CSI_FRM_SIZE_WIDTH_SH		(0)
#define CSI_FRM_SIZE_HEIGHT_SH		(16)
#define CSI_FRM_SIZE_WIDTH_MSK		(0xfff)
#define CSI_FRM_SIZE_HEIGHT_MSK		(0xfff)

#define IC_CONF_CSI_MEM_WR_EN		(1<<31)
#define IC_CONF_CSI_RWS_EN		(1<<30)
#define IC_CONF_IC_KEY_COLOR_EN		(1<<29)
#define IC_CONF_IC_GLB_LOC_A		(1<<28)
#define IC_CONF_PP_PROT_EN		(1<<20)
#define IC_CONF_PP_CMB			(1<<19)
#define IC_CONF_PP_CSC2			(1<<18)
#define IC_CONF_PP_CSC1			(1<<17)
#define IC_CONF_PP_EN			(1<<16)
#define IC_CONF_PRPVF_ROT_EN		(1<<12)
#define IC_CONF_PRPVF_CMB		(1<<11)
#define IC_CONF_PRPVF_CSC2		(1<<10)
#define IC_CONF_PRPVF_CSC1		(1<<9)
#define IC_CONF_PRPVF_PRPVF_EN		(1<<8)
#define IC_CONF_PRPENC_ROT_EN		(1<<2)
#define IC_CONF_PRPENC_CSC1		(1<<1)
#define IC_CONF_PRPENC_EN		(1<<0)

#define IPU_INT_STAT3_CSI_EOF		(1<<6)
#define IPU_INT_STAT3_CSI_NF		(1<<5)

#define IPU_INT_STAT5_BAYER_FRM_LOST_ERR (1<<11)

#define SENSOR_WINDOW_WIDTH		 (0x04)
#define SENSOR_WINDOW_HEIGHT		 (0x03)
#define SENSOR_MAXIMUM_SHUTTER_WIDTH	 (0xbd)
#define SENSOR_HORIZONTAL_BLANKING	 (0x05)
#define SENSOR_VERTICAL_BLANKING	 (0x06)
#define SENSOR_LED_POLARITY		 (0x1b)

#define CB_FRAME			(1024*1024)
static char* rgbFrameA;
static char* rgbFrameB;
static char* rgbFrameC;
static char* rgbFrameD;
static char* rgbFrameE;
static char* rgbFrameF;

#define ADDR_NO_STOP		(1<<14)
#define ADDR_RESTART		(1<<13)
#define ADDR_CONTINUE		(1<<12)

/* IPU control word 0 offsets and lengths */
#define IPU_CW_XV	  0, 10
#define IPU_CW_YV	 10, 10
#define IPU_CW_XB	 20, 12
#define IPU_CW_YB	 32, 12
#define IPU_CW_SCE	 44,  1
#define IPU_CW_NSB	 46,  1
#define IPU_CW_LNPB	 47,  6
#define IPU_CW_FW	108, 12
#define IPU_CW_FH	120, 12

		/* non-interleaved group */
#define IPU_CW_UB0	 53, 26
#define IPU_CW_UB1	 79, 26

		/* interleaved group */
#define IPU_CW_SX	 53, 10
#define IPU_CW_SY	 63, 10
#define IPU_CW_NS	 73, 10
#define IPU_CW_SM	 83, 10
#define IPU_CW_SDX	 93,  5
#define IPU_CW_SDY	 98,  5
#define IPU_CW_SDRX	103,  1
#define IPU_CW_SDRY	104,  1
#define IPU_CW_SCRQ	105,  1

/* IPU control word 1 offsets and lengths */
#define IPU_CW_EBA0	  0, 32
#define IPU_CW_EBA1	 32, 32
#define IPU_CW_BPP	 64,  3
#define IPU_CW_SL	 67, 14
#define IPU_CW_PFS	 81,  3
#define IPU_CW_BAM	 84,  3
#define IPU_CW_NPB	 89,  6
#define IPU_CW_SAT	 96,  2
#define IPU_CW_SCC	 98,  1
#define IPU_CW_OFS0	 99,  5
#define IPU_CW_OFS1	104,  5
#define IPU_CW_OFS2	109,  5
#define IPU_CW_OFS3	114,  5
#define IPU_CW_WID0	119,  3
#define IPU_CW_WID1	122,  3
#define IPU_CW_WID2	125,  3
#define IPU_CW_WID3	128,  3
#define IPU_CW_DEC_SEL	131,  1

static const char* describe_channels (unsigned long v)
{
  static char sz[128];
  *sz = 0;

#define _(v,b)\
	if ((v) & (IPU_CHAN_##b))\
	  snprintf (sz + strlen (sz), sizeof (sz) - strlen (sz), " %s", #b)

  _(v,IC0);	_(v,IC1);	_(v,IC2);	_(v,IC3);
  _(v,IC4);	_(v,IC5);	_(v,IC6);	_(v,IC7);
  _(v,IC8);	_(v,IC9);	_(v,IC10);	_(v,IC11);
  _(v,IC12);	_(v,IC13);
  _(v,SDC0);	_(v,SDC1);	_(v,SDC2);	_(v,SDC3);
  _(v,ADC2);	_(v,ADC3);	_(v,ADC4);	_(v,ADC5);
  _(v,ADC6);	_(v,ADC7);
  _(v,PF0);	_(v,PF1);	_(v,PF2);	_(v,PF3);
  _(v,PF4);	_(v,PF5);	_(v,PF6);	_(v,PF7);

#undef _
  return sz;
}

static int describe_bits_per_pixel (int v)
{
  return "\x20\x18\x10\x08\x04\x01\x00\x00"[v%8];
}

static const char* describe_pfs (int v)
{
  switch (v) {
  case 0: return "code";
  case 1: return "YUV444 (NI)";
  case 2: return "YUV422 (NI)";
  case 3: return "YUV420 (NI)";
  case 4: return "RGB (pack/unpack)";
  case 6: return "YUV422 (I)";
  case 7: return "Generic";
  default:
    return "unknown";
  }
}

static const char* describe_ic_conf (unsigned long v)
{
  static char sz[80];
  *sz = 0;

  if (v & IC_CONF_CSI_MEM_WR_EN)
    snprintf (sz + strlen (sz), sizeof (sz) - strlen (sz),
	      " %s", "CSI_MEM_WR_EN");
  if (v & IC_CONF_CSI_RWS_EN)
    snprintf (sz + strlen (sz), sizeof (sz) - strlen (sz),
	      " %s", "CSI_RWS_EN");
  if (v & IC_CONF_IC_KEY_COLOR_EN)
    snprintf (sz + strlen (sz), sizeof (sz) - strlen (sz),
	      " %s", "IC_KEY_COLOR_EN");
  if (v & IC_CONF_IC_GLB_LOC_A)
    snprintf (sz + strlen (sz), sizeof (sz) - strlen (sz),
	      " %s", "IC_GLB_LOC_A");
  if (v & IC_CONF_PP_PROT_EN)
    snprintf (sz + strlen (sz), sizeof (sz) - strlen (sz),
	      " %s", "PP_PROT_EN");
  if (v & IC_CONF_PP_CMB)
    snprintf (sz + strlen (sz), sizeof (sz) - strlen (sz),
	      " %s", "PP_CMB");
  if (v & IC_CONF_PP_CSC2)
    snprintf (sz + strlen (sz), sizeof (sz) - strlen (sz),
	      " %s", "PP_CSC2");
  if (v & IC_CONF_PP_CSC1)
    snprintf (sz + strlen (sz), sizeof (sz) - strlen (sz),
	      " %s", "PP_CSC1");
  if (v & IC_CONF_PP_EN)
    snprintf (sz + strlen (sz), sizeof (sz) - strlen (sz),
	      " %s", "PP_EN");
  if (v & IC_CONF_PRPVF_ROT_EN)
    snprintf (sz + strlen (sz), sizeof (sz) - strlen (sz),
	      " %s", "PRPVF_ROT_EN");
  if (v & IC_CONF_PRPVF_CMB)
    snprintf (sz + strlen (sz), sizeof (sz) - strlen (sz),
	      " %s", "PRPVF_CMB");
  if (v & IC_CONF_PRPVF_CSC2)
    snprintf (sz + strlen (sz), sizeof (sz) - strlen (sz),
	      " %s", "PRPVF_CSC2");
  if (v & IC_CONF_PRPVF_CSC1)
    snprintf (sz + strlen (sz), sizeof (sz) - strlen (sz),
	      " %s", "PRPVF_CSC1");
  if (v & IC_CONF_PRPVF_PRPVF_EN)
    snprintf (sz + strlen (sz), sizeof (sz) - strlen (sz),
	      " %s", "PRPVF_PRPVF_EN");
  if (v & IC_CONF_PRPENC_ROT_EN)
    snprintf (sz + strlen (sz), sizeof (sz) - strlen (sz),
	      " %s", "PRPENC_ROT_EN");
  if (v & IC_CONF_PRPENC_CSC1)
    snprintf (sz + strlen (sz), sizeof (sz) - strlen (sz),
	      " %s", "PRPENC_CSC1");
  if (v & IC_CONF_PRPENC_EN)
    snprintf (sz + strlen (sz), sizeof (sz) - strlen (sz),
	      " %s", "PRPENC_EN");
  return sz;
}

const char* describe_stat3 (unsigned long v)
{
  static char sz[80];
  *sz = 0;

  if (v & IPU_INT_STAT3_CSI_EOF)
    snprintf (sz + strlen (sz), sizeof (sz) - strlen (sz),
	      " %s", "CSI_EOF");
  if (v & IPU_INT_STAT3_CSI_NF)
    snprintf (sz + strlen (sz), sizeof (sz) - strlen (sz),
	      " %s", "CSI_NF");
  return sz;
}

const char* describe_stat5 (unsigned long v)
{
  static char sz[80];
  *sz = 0;

  if (v & IPU_INT_STAT5_BAYER_FRM_LOST_ERR)
    snprintf (sz + strlen (sz), sizeof (sz) - strlen (sz),
	      " %s", "BAYER_FRM_LOST_ERR");
  return sz;
}

const char* describe_i2sr (int v)
{
  static char sz[80];
  snprintf (sz, sizeof (sz), "(0x%02x)", v);

#define _(b,s)\
  if (v & (b)) snprintf (sz + strlen (sz), sizeof (sz) - strlen (sz),\
			 " %s", " " s);

  _(I2C_I2SR_ICF,  "ICF");
  _(I2C_I2SR_IAAS, "IAAS");
  _(I2C_I2SR_IBB,  "IBB");
  _(I2C_I2SR_IAL,  "IAL");
  _(I2C_I2SR_SRW,  "SRW");
  _(I2C_I2SR_IIF,  "IIF");
  _(I2C_I2SR_RXAK, "RXAK");

#undef _

  return sz;
}

const char* describe_i2cr (int v)
{
  static char sz[80];
  snprintf (sz, sizeof (sz), "(0x%02x)", v);

#define _(b,s)\
  if (v & (b)) snprintf (sz + strlen (sz), sizeof (sz) - strlen (sz),\
			 " %s", " " s);

  _(I2C_I2CR_IEN,   "IEN");
  _(I2C_I2CR_MSTA,  "MSTA");
  _(I2C_I2CR_MTX,   "MTX");
  _(I2C_I2CR_TXAK,  "TXAK");
  _(I2C_I2CR_RSATA, "RSATA");

#undef _

  return sz;
}


/* write_huge

   writes the given value to the very long control word buffer.  This
   code only works in little-endian mode.

*/

static void write_huge (char* rgb, unsigned long value, int shift, int width)
{
  //  int index = 0;
//  printf ("compose 0x%08lx %3d %2d\n", value, shift, width);

	/* Eliminate leader */
  if (shift/8) {
    rgb += shift/8;
    shift %= 8;
//    index += c;
  }

  while (width > 0) {
    int avail = width;
    unsigned long mask;
    if (avail > 8 - shift)
      avail = 8 - shift;
    mask = ((1 << avail) - 1);
//    printf ("writing 0x%08lx [%2d] 0x%02lx, mask 0x%02lx"
//	    " with shift %d (0x%02lx)\n",
//	    value, index, value & mask, mask, shift, mask << shift);
    *rgb = (*rgb & ~(mask << shift)) | (value & mask) << shift;
    ++rgb;
    width -= avail;
    value >>= avail;
    shift = 0;			/* Always zero after first byte */
//    ++index;
  }
}


/* read_huge

   reads the given bits from the very long (aka huge) control word
   into a normalized value.

*/

static unsigned long read_huge (const char* rgb, int shift, int width)
{
  int index = 0;
  int offset = 0;
  unsigned long value = 0;

  //  printf ("decompose %3d %2d\n", shift, width);

	/* Eliminate leader */
  while (shift >= 8) {
    ++rgb;
    shift -= 8;
    ++index;
  }

  while (width > 0) {
    int avail = width;
    unsigned long mask;
    if (avail > 8 - shift)
      avail = 8 - shift;
    mask = ((1 << avail) - 1);
//    printf ("reading [%2d] mask 0x%02lx with shift %d (0x%02lx) at %d\n",
//	    index, mask, shift, mask << shift, offset);
    value |= ((*rgb++ & (mask << shift)) >> shift) << offset;
    width -= avail;
    offset += avail;
    shift = 0;			/* Always zero after first byte */
    ++index;
  }

  return value;
}


/* ipu_channel_ready

   marks a channel as ready

*/

static inline void ipu_channel_ready (int channel, int buffer)
{
  if (buffer)
    IPU_CHA_BUF1_RDY |= 1<<channel;
  else
    IPU_CHA_BUF0_RDY |= 1<<channel;
}

/* ipu_channel_busy

   checks to see if a channel is busy.

*/

static inline int ipu_channel_busy (int channel, int buffer)
{
  if (buffer)
    return (IPU_CHA_BUF1_RDY & (1<<channel)) != 0;
  else
    return (IPU_CHA_BUF0_RDY & (1<<channel)) != 0;
}


/* ipu_write_ima

   writes the given number of bits from rgb to the Internal Memory
   Access register.  We depend on the feature of the IMA hardware to
   increment the word pointer after each write.  We also depend on the
   fact that the endian-ness of the ARM matches the endian-ness of the
   of the register.

*/

static void ipu_write_ima (int mem_nu, int row_nu, const char* rgb, int bits)
{
  IPU_IMA_ADDR
    = ((mem_nu & 0xf) << 16)
    | ((row_nu & 0x1fff) << 3);

  while (bits > 0) {
    IPU_IMA_DATA = *(unsigned long*) rgb;
    rgb += 4;
    bits -= 32;
  }
}

static void ipu_read_ima (int mem_nu, int row_nu, const char* rgb, int bits)
{
  IPU_IMA_ADDR
    = ((mem_nu & 0xf) << 16)
    | ((row_nu & 0x1fff) << 3);

  while (bits > 0) {
//    printf ("read %d\n", bits);
    *(unsigned long*) rgb = IPU_IMA_DATA;
    rgb += 4;
    bits -= 32;
  }
}


/* ipu_report_irq

   displays the IPU irq registers.

*/

static void ipu_report_irq (void)
{
  printf ("0x%08lx 0x%08lx 0x%08lx 0x%08lx 0x%08lx\n",
	  IPU_INT_STAT1, IPU_INT_STAT2, IPU_INT_STAT3,
	  IPU_INT_STAT4, IPU_INT_STAT5);
  printf (" pf7_eof %d  adc7_eof %d  ic7_eof %d\n",
	  (IPU_INT_STAT1 & (1<<31)) != 0,
	  (IPU_INT_STAT1 & (1<<23)) != 0,
	  (IPU_INT_STAT1 & (1<<7)) != 0);
  printf (" pf7_nfack %d  adc7_nfack %d  ic7_nfack %d  \n",
	  (IPU_INT_STAT2 & (1<<31)) != 0,
	  (IPU_INT_STAT2 & (1<<23)) != 0,
	  (IPU_INT_STAT2 & (1<<7)) != 0);
  printf (" csi_eof %d  csi_nf %d\n",
	  (IPU_INT_STAT3 & (1<<6)) != 0,
	  (IPU_INT_STAT3 & (1<<5)) != 0);
}

static void ipu_report (void)
{
  printf ("IPU_CONF             0x%08lx", IPU_CONF);
  printf (" %s%s%s%s%s%s%s%s%s\n",
	  (IPU_CONF & IPU_CONF_CSI_EN) ? " CSI_EN" : "",
	  (IPU_CONF & IPU_CONF_IC_EN) ? " IC_EN" : "",
	  (IPU_CONF & IPU_CONF_ROT_EN) ? " ROT_EN" : "",
	  (IPU_CONF & IPU_CONF_PF_EN) ? " PF_EN" : "",
	  (IPU_CONF & IPU_CONF_SDC_EN) ? " SDC_EN" : "",
	  (IPU_CONF & IPU_CONF_ADC_EN) ? " ADC_EN" : "",
	  (IPU_CONF & IPU_CONF_DI_EN) ? " DI_EN" : "",
	  (IPU_CONF & IPU_CONF_DU_EN) ? " DU_EN" : "",
	  (IPU_CONF & IPU_CONF_PXL_ENDIAN) ? " PXL_ENDIAN" : "");
  printf ("IPU_CHA_BUF0_RDY     0x%08lx %s\n",
	  IPU_CHA_BUF0_RDY, describe_channels (IPU_CHA_BUF0_RDY));
  printf ("IPU_CHA_BUF1_RDY     0x%08lx %s\n",
	  IPU_CHA_BUF1_RDY, describe_channels (IPU_CHA_BUF1_RDY));
  printf ("IPU_CHA_DB_MODE_SEL  0x%08lx %s\n",
	  IPU_CHA_DB_MODE_SEL, describe_channels (IPU_CHA_DB_MODE_SEL));
  printf ("IPU_CHA_CUR_BUF      0x%08lx %s\n",
	  IPU_CHA_CUR_BUF, describe_channels (IPU_CHA_CUR_BUF));
  printf ("IPU_FS_PROC_FLOW     0x%08lx\n", IPU_FS_PROC_FLOW);
  printf ("IPU_DISP_PROC_FLOW   0x%08lx\n", IPU_DISP_PROC_FLOW);
  printf ("IPU_TASK_STAT        0x%08lx\n", IPU_TASK_STAT);
  printf ("IPU_INT_CTRL1        0x%08lx %s\n",
	  IPU_INT_CTRL1, describe_channels (IPU_INT_CTRL1));
  printf ("IPU_INT_CTRL2        0x%08lx %s\n",
	  IPU_INT_CTRL2, describe_channels (IPU_INT_CTRL2));
  printf ("IPU_INT_CTRL3        0x%08lx\n", IPU_INT_CTRL3);
  printf ("IPU_INT_CTRL4        0x%08lx %s\n",
	  IPU_INT_CTRL4, describe_channels (IPU_INT_CTRL4));
  printf ("IPU_INT_CTRL5        0x%08lx\n", IPU_INT_CTRL5);
  printf ("IPU_INT_STAT1 EOF    0x%08lx %s\n",
	  IPU_INT_STAT1, describe_channels (IPU_INT_STAT1));
  printf ("IPU_INT_STAT2 NFACK  0x%08lx %s\n",
	  IPU_INT_STAT2, describe_channels (IPU_INT_STAT2));
  printf ("IPU_INT_STAT3        0x%08lx %s\n", IPU_INT_STAT3,
	  describe_stat3 (IPU_INT_STAT3));
  printf ("IPU_INT_STAT4 EOFERR 0x%08lx %s\n",
	  IPU_INT_STAT4, describe_channels (IPU_INT_STAT4));
  printf ("IPU_INT_STAT5        0x%08lx %s\n", IPU_INT_STAT5,
	  describe_stat5 (IPU_INT_STAT5));
  printf ("IPU_BRK_CTRL1        0x%08lx\n", IPU_BRK_CTRL1);
  printf ("IPU_BRK_CTRL2        0x%08lx\n", IPU_BRK_CTRL2);
  printf ("IPU_BRK_STAT         0x%08lx\n", IPU_BRK_STAT);
  printf ("IPU_DIAGB_CTRL       0x%08lx\n", IPU_DIAGB_CTRL);
  printf ("CSI_SENS_CONF        0x%08lx%s%s %*.*s %d bpp\n",
	  CSI_SENS_CONF,
	  (CSI_SENS_CONF & (1<<15)) ? " ext_vsync" : " int_vsync",
	  (CSI_SENS_CONF & (1<<7)) ? " hsp_clk" : " sensb_sens_clk",
	  6, 6, "RGBYUVReservYUV422Generc" + ((CSI_SENS_CONF >> 8) & 3)*6,
	  "\x04\x08\x0a\x0f"[(CSI_SENS_CONF >> 10) & 3]);
  printf ("CSI_SENS_FRM_SIZE    0x%08lx (%ld x %ld)\n", CSI_SENS_FRM_SIZE,
	  ((CSI_SENS_FRM_SIZE >> 0) & 0xfff) + 1,
	  ((CSI_SENS_FRM_SIZE >> 16) & 0xfff) + 1);
  printf ("CSI_ACT_FRM_SIZE     0x%08lx (%ld x %ld)\n", CSI_ACT_FRM_SIZE,
	  ((CSI_ACT_FRM_SIZE >> 0) & 0xfff) + 1,
	  ((CSI_ACT_FRM_SIZE >> 16) & 0xfff) + 1);
  printf ("CSI_OUT_FRM_CTRL     0x%08lx\n", CSI_OUT_FRM_CTRL);
  printf ("CSI_TST_CTRL         0x%08lx\n", CSI_TST_CTRL);
  printf ("CSI_CCIR_CODE1       0x%08lx\n", CSI_CCIR_CODE1);
  printf ("CSI_CCIR_CODE2       0x%08lx\n", CSI_CCIR_CODE2);
  printf ("CSI_CCIR_CODE3       0x%08lx\n", CSI_CCIR_CODE3);
  printf ("CSI_FLASH_STROBE1    0x%08lx\n", CSI_FLASH_STROBE1);
  printf ("CSI_FLASH_STROBE2    0x%08lx\n", CSI_FLASH_STROBE2);
  printf ("IC_CONF              0x%08lx%s\n", IC_CONF,
	  describe_ic_conf (IC_CONF));
  printf ("IC_PRP_ENC_RSC       0x%08lx\n", IC_PRP_ENC_RSC);
  printf ("IC_PRP_VF_RSC        0x%08lx\n", IC_PRP_VF_RSC);
  printf ("IC_PP_RSC            0x%08lx\n", IC_PP_RSC);
  printf ("PF_CONF              0x%08lx\n", PF_CONF);
  printf ("IDMAC_CONF           0x%08lx\n", IDMAC_CONF);
  printf ("IDMAC_CHA_EN         0x%08lx %s\n",
	  IDMAC_CHA_EN, describe_channels (IDMAC_CHA_EN));
  printf ("IDMAC_CHA_PRI        0x%08lx %s\n",
	  IDMAC_CHA_PRI, describe_channels (IDMAC_CHA_PRI));
  printf ("IDMAC_CHA_BUSY       0x%08lx %s\n",
	  IDMAC_CHA_BUSY, describe_channels (IDMAC_CHA_BUSY));
  {
    int i;
    char rgb0[(132 + 7)/8];
    char rgb1[(132 + 7)/8];
    unsigned v = IDMAC_CHA_EN | IDMAC_CHA_PRI | IDMAC_CHA_BUSY
      | IPU_CHA_BUF0_RDY | IPU_CHA_BUF1_RDY
      | IPU_CHA_DB_MODE_SEL | IPU_CHA_CUR_BUF
//      | (1<<7)
      ;
    for (i = 0; i < 32; ++i) {
      int interleaved;
      if (v & (1<<i)) {
	char* sz;
	/* Determine the name of the channel.  We would use an array
	   of pointers to these strings, but the compiler wants to put
	   them in the .data segment which we don't have. */
	switch (i) {
	default: sz = ""; break;
#define _(i,s) case i: sz = s; break;
	_( 0, "IC0  IC  -> MEM pre  data IC encoder to MEM")
	_( 1, "IC1  IC  -> MEM pre  data IC viewfinder to MEM or ADC")
	_( 2, "IC2  IC  -> MEM post data IC to MEM")
	_( 3, "IC3  MEM -> IC  graphics data for combining")
	_( 4, "IC4  MEM -> IC  graphics data for combining, post processing")
	_( 5, "IC5  MEM -> IC  post data from MEM")
	_( 6, "IC6  MEM -> IC  post data from sensor in MEM (e.g. bayer)")
	_( 7, "IC7  IC  -> MEM direct from IC to MEM")
	_( 8, "IC8  IC  -> MEM pre  data after rotation, encoder task")
	_( 9, "IC9  IC  -> MEM pre  data after rotation, viewfinder task")
	_(10, "IC10 MEM -> IC  pre  data for rotation, encoder task")
	_(11, "IC11 MEM -> IC  pre  data for rotation, viewfinder task")
	_(12, "IC12 IC  -> MEM post data after rotation")
	_(13, "IC13 MEM -> IC  post data for rotation")
	_(14, "SDC0 MEM -> SDC background data (full refresh)")
	_(15, "SDC1 MEM -> SDC foreground data")
	_(16, "SDC2 MEM -> SDC mask data")
	_(17, "SDC3 MEM -> SDC background data (partial refresh)")
	_(18, "ADC2 MEM -> ADC system channel 1 write data")
	_(19, "ADC3 MEM -> ADC system channel 2 write data")
	_(20, "ADC4 MEM -> ADC command stream channel 1")
	_(21, "ADC5 MEM -> ADC command stream channel 2")
	_(22, "ADC6 ADC -> MEM system channel 1 read data")
	_(23, "ADC7 ADC -> MEM system channel 2 read data")
	_(24, "PF0  MEM -> PF  PF parameters MPEG4/H.264")
	_(25, "PF1  MEM -> PF  PF parameters H.264")
	_(26, "PF2  MEM -> PF  Y input data")
	_(27, "PF3  MEM -> PF  U input data")
	_(28, "PF4  MEM -> PF  V input data")
	_(29, "PF5  PF  -> MEM Y output data")
	_(30, "PF6  PF  -> MEM U output data")
	_(31, "PF7  PF  -> MEM V output data")
#undef _
	}

	printf (" -- Channel %2d -- %s\n", i, sz);
	memset (rgb0, 0, sizeof (rgb0));
	memset (rgb1, 0, sizeof (rgb1));
	ipu_read_ima (1, 2*i + 0, rgb0, 132);
	ipu_read_ima (1, 2*i + 1, rgb1, 132);
	dumpw (rgb0, sizeof (rgb0), 0, 0);
	dumpw (rgb1, sizeof (rgb1), 0, 0);
	switch (read_huge (rgb1, IPU_CW_PFS)) {
	case 0:
	default:
	  continue;		/* Code or NULL configuraton, we ignore it */
	case 1:			/* YUV444 non-interleaved */
	case 2:			/* YUV422 non-interleaved */
	case 3:			/* YUV420 non-interleaved */
	  interleaved = 0;
	  break;
	case 4:			/* RGB pack/unpack */
	case 6:			/* YUV422 interleaved */
	case 7:			/* Generic data */
	  interleaved = 1;
	  break;
	}

	printf ("  XV %4ld  YV %4ld  XB %4ld  YB %4ld  SCE %ld  "
		"NSB %ld  LNFB %2ld\n",
		read_huge (rgb0, IPU_CW_XV),
		read_huge (rgb0, IPU_CW_YV),
		read_huge (rgb0, IPU_CW_XB),
		read_huge (rgb0, IPU_CW_YB),
		read_huge (rgb0, IPU_CW_SCE),
		read_huge (rgb0, IPU_CW_NSB),
		read_huge (rgb0, IPU_CW_LNPB));
	if (interleaved)
	  printf ("  SX/Y %4ld %4ld  NS %4ld  SM %4ld  SDX/Y %2ld %2ld"
		  "  SDRX/Y %ld %ld  SCRQ %ld\n",
		  read_huge (rgb0, IPU_CW_SX),
		  read_huge (rgb0, IPU_CW_SY),
		  read_huge (rgb0, IPU_CW_NS),
		  read_huge (rgb0, IPU_CW_SM),
		  read_huge (rgb0, IPU_CW_SDX),
		  read_huge (rgb0, IPU_CW_SDY),
		  read_huge (rgb0, IPU_CW_SDRX),
		  read_huge (rgb0, IPU_CW_SDRY),
		  read_huge (rgb0, IPU_CW_SCRQ));
	else
	  printf ("  UB0 %8ld  UB1 %8ld\n",
		  read_huge (rgb0, IPU_CW_UB0),
		  read_huge (rgb0, IPU_CW_UB1));
	printf ("  FW %4ld  FH %4ld  EBA0 %08lx  EBA1 %08lx  "
		"BPP %ld (%d bpp)  \n",
		read_huge (rgb0, IPU_CW_FW),
		read_huge (rgb0, IPU_CW_FH),
		read_huge (rgb1, IPU_CW_EBA0),
		read_huge (rgb1, IPU_CW_EBA1),
		read_huge (rgb1, IPU_CW_BPP),
		describe_bits_per_pixel (read_huge (rgb1, IPU_CW_BPP)));
	printf ("  SL %4ld  PFS %ld '%s'"
		"  BAM %ld  NPB %2lx (%ld)  SAT %ld\n",
		read_huge (rgb1, IPU_CW_SL),
		read_huge (rgb1, IPU_CW_PFS),
		describe_pfs (read_huge (rgb1, IPU_CW_PFS)),
		read_huge (rgb1, IPU_CW_BAM),
		read_huge (rgb1, IPU_CW_NPB),
		read_huge (rgb1, IPU_CW_NPB) + 1,
		read_huge (rgb1, IPU_CW_SAT));
	if (interleaved) {
	  printf ("  SCC %ld  OFS0 %2ld  OFS1 %2ld  OFS2 %2ld"
		  "  OFS3 %2ld\n",
		  read_huge (rgb1, IPU_CW_SCC),
		  read_huge (rgb1, IPU_CW_OFS0),
		  read_huge (rgb1, IPU_CW_OFS1),
		  read_huge (rgb1, IPU_CW_OFS2),
		  read_huge (rgb1, IPU_CW_OFS3));
	  printf ("  WID0 %2ld  WID1 %2ld  WID2 %2ld  "
		  "WID3 %2ld  DEC_SEL %ld\n",
		  read_huge (rgb1, IPU_CW_WID0),
		  read_huge (rgb1, IPU_CW_WID1),
		  read_huge (rgb1, IPU_CW_WID2),
		  read_huge (rgb1, IPU_CW_WID3),
		  read_huge (rgb1, IPU_CW_DEC_SEL));
	}
      }
    }
  }
}

static void ipu_zero (void)
{
  int i;
  char rgb[(132 + 7)/8];
  memset (rgb, 0, sizeof (rgb));

  printf ("clearing IPU control registers\n");

  IDMAC_CHA_EN = 0;
  for (i = 0; i < 32; ++i) {
    ipu_write_ima (1, i*2 + 0, rgb, 132);
    ipu_write_ima (1, i*2 + 1, rgb, 132);
  }
}


/* i2c_report

*/

static void i2c_report (void)
{
  printf ("  I2ADR 0x%02x  I2IF 0x%02x  I2CR %s  I2SR %s\n",
	  I2C_IADR, I2C_IFDR,
	  describe_i2cr (I2C_I2CR), describe_i2sr (I2C_I2SR));
}

/* i2c_stop

   halts transmission on the I2C bus.  The kernel version will wait
   for 16 attempts of 3us for the BB to clear.  The return value is 0
   for success.

*/

static int i2c_stop (void)
{
//  unsigned long time;
  ENTRY;

//  printf ("--\n");
  I2C_I2CR &= ~(I2C_I2CR_MSTA | I2C_I2CR_MTX
		| I2C_I2CR_TXAK
		);
  I2C_I2SR = 0;

  return 0;
#if 0
  time = timer_read ();
  while ((I2C_I2SR & I2C_I2SR_IBB)	/* Wait for STOP condition */
	 && timer_delta (time, timer_read ()) < 10)
    usleep (3);

  return (I2C_I2SR & I2C_I2SR_IBB) != 0;
#endif
}

static void i2c_setup (void)
{
  ENTRY;

  IOMUX_PIN_CONFIG_FUNC  (MX31_PIN_I2C_CLK);
  IOMUX_PIN_CONFIG_FUNC  (MX31_PIN_I2C_DAT);

  I2C2_I2CR = 0;		/* Disable other I2C controllers */
  I2C3_I2CR = 0;

  I2C_IFDR = I2C_IFDR_V;
  I2C_IADR = 0;			/* Our address is zero  */

  I2C_I2SR = 0;
  i2c_stop ();
  I2C_I2CR = I2C_I2CR_IEN;	/* Enable I2C */
}

/* i2c_wait_for_interrupt

   waits for an interrupt and returns the status.

*/

static int i2c_wait_for_interrupt (void)
{
  unsigned long time;
  ENTRY;
  time = timer_read ();
  while (!(I2C_I2SR & I2C_I2SR_IIF)	/* Wait for interrupt */
	 && timer_delta (time, timer_read ()) < 1000*2)
    ;
  if (!(I2C_I2SR & I2C_I2SR_IIF))
    printf ("timeout waiting for interrupt\n");

  I2C_I2SR = 0;				/* Only IIF can be cleared */
  return I2C_I2SR;
}

#if 0
static int i2c_wait_for_idle (void)
{
  if (I2C_I2SR & I2C_I2SR_IBB) {
    printf ("%s error: i2c bus busy %d\n", __FUNCTION__, address);
    i2c_stop ();
    return 1;
  }
  return 0
}
#endif


/* i2c_start

   sends the slave the frame START and target address.  Return value
   is 0 on success.

*/

static int i2c_start (char address, int reading)
{
  int retries = 16;
  int restart = (address & ADDR_RESTART) != 0;
  int sr;

  ENTRY;

  {
    unsigned long time = timer_read ();
    while (timer_delta (time, timer_read ()) < 10)
      if (!(I2C_I2SR & I2C_I2SR_IBB))
	break;
    if (I2C_I2SR & I2C_I2SR_IBB) {
      printf ("start aborted, unable to acquire bus sr %s cr %s\n",
	      describe_i2sr (I2C_I2SR), describe_i2cr (I2C_I2CR));
      return 1;			/* Unable to acquire, bus still busy */
    }
  }

  address &= 0x7f;

  I2C_I2CR |= I2C_I2CR_MSTA	/* Acquire bus */
    | (restart ? I2C_I2CR_RSATA : 0); /* Assert restart */

  do {
    int sr = I2C_I2SR;
    if (sr & I2C_I2SR_IBB)	/* Arbitration won */
      break;
    usleep (3);
  } while (retries-- > 0);
  if (!(I2C_I2SR & I2C_I2SR_IBB))
    return 1;			/* Failed to acquire */
  I2C_I2SR = 0;
  I2C_I2CR |= I2C_I2CR_MTX;	/* Prepare for transmit */
//  printf (" --> 0x%02x\n", (address << 1) | (reading ? 1 : 0));
  I2C_I2DR = (address << 1) | (reading ? 1 : 0);

  sr = i2c_wait_for_interrupt ();
  I2C_I2CR &= ~I2C_I2CR_RSATA;	/* Clear repeat start */

				/* Address must be ACK'd */
  return (sr & I2C_I2SR_RXAK) ? 1 : 0;
}


/* i2c_write

   writes the buffer to the given address.  It returns 0 on success.

*/

static int i2c_write (int address, char* rgb, int cb)
{
  int i;
//  int continuing = (address & ADDR_CONTINUE) != 0;
//  int restart    = (address & ADDR_RESTART) != 0;

  ENTRY;

//  printf ("%s\n", __FUNCTION__);
//  dumpw (rgb, cb, 0, 0);

#if 0
  if (!continuing && !restart && (I2C_I2SR & I2C_I2SR_IBB)) {
    printf ("%s error: i2c bus busy 0x%x\n", __FUNCTION__, address);
    i2c_stop ();
    return 1;
  }
#endif

  if (i2c_start (address, 0)) {
    if (i2c_stop ())
      printf ("%s error on stop on failed start sr %s cr %s\n",
	      __FUNCTION__, describe_i2sr (I2C_I2SR),
	      describe_i2cr (I2C_I2CR));
    return 1;
  }

  I2C_I2CR |= I2C_I2CR_MTX;
  for (i = 0; i < cb; ++i) {
//    printf (" ==> 0x%02x\n", *rgb);
    I2C_I2DR = *rgb++;
    if (i2c_wait_for_interrupt () & I2C_I2SR_RXAK) {
      i2c_stop ();
      return 1;
    }
  }

  if (!(address & ADDR_NO_STOP) && i2c_stop ())
    printf ("%s error on stop sr %s cr %s\n",
	    __FUNCTION__, describe_i2sr (I2C_I2SR), describe_i2cr (I2C_I2CR));

  return 0;
}


/* i2c_read

   reads the buffer from the given address.  It returns 0 on success.

*/

static int i2c_read (int address, char* rgb, int cb)
{
  int i;
//  int cr;
  ENTRY;

  address &= 0x7f;

#if 0
  if (I2C_I2SR & I2C_I2SR_IBB) {
    printf ("%s error: i2c bus busy %d\n", __FUNCTION__, address);
    i2c_stop ();
    return 1;
  }
#endif

  if (i2c_start (address, 1)) {
    if (i2c_stop ())
      printf ("%s error on stop after failed start sr %s cr %s\n",
	      __FUNCTION__, describe_i2sr (I2C_I2SR),
	      describe_i2cr (I2C_I2CR));
    return 1;
  }

  I2C_I2CR &= ~I2C_I2CR_MTX;

  /* If we are reading only one byte, we need to set the TXAK bit
     before we discard the dummy read and before we wait for the
     interrupt.  */
  if (cb == 1)
    I2C_I2CR |= I2C_I2CR_TXAK;
  for (i = 0; i < cb; ++i) {
    int sr;
//    if (cb == 1 || i == cb - 2)
//      cr |= I2C_I2CR_TXAK;
//    else
//      cr &= ~I2C_I2CR_TXAK;
//    printf (" <<< i %d cb %d  cr %s\n", i, cb, describe_i2cr (I2C_I2CR));
//    I2C_I2CR = cr;
    if (i == 0) {
//      char b = I2C_I2DR;
//      printf (" <== 0x%x (discard)\n", b);
      I2C_I2DR;			/* Dummy read after address cycle */
    }
    sr = i2c_wait_for_interrupt ();

    if (i + 2 == cb)
      I2C_I2CR |= I2C_I2CR_TXAK;
    else if (i + 1 >= cb) {
      if (i2c_stop ())
	printf ("%s error on stop sr %s, cr %s\n",
		__FUNCTION__, describe_i2sr (I2C_I2SR),
		describe_i2cr (I2C_I2CR));
    }
    {
      char b = I2C_I2DR;
//      printf (" <== 0x%x\n", b);
      *rgb++ = b;
//      *rgb++ = I2C_I2DR;
    }
  }


  return 0;
}

static int i2c_sensor_read (int reg)
{
  char rgb[2];
//  int v = 0;
  char r = reg & 0xff;

  ENTRY;

  i2c_write (0x5c, &r, 1);

#if 0
  i2c_read  (0x5c, &((char*) &v)[1], 1);
  i2c_write (0x5c, "\xf0", 1);
  i2c_read  (0x5c, &((char*) &v)[0], 1);
  return v;
#endif

  rgb[0] = rgb[1] = 0;
  i2c_read (0x5c, rgb, 2);

  return (rgb[0] << 8) | rgb[1];
}

static void i2c_sensor_write (int reg, int value)
{
  char rgb[] = { reg & 0xff, (value >> 8) & 0xff,
			     (value >> 0) & 0xff };
  ENTRY;
//  dumpw (rgb, sizeof (rgb), 0, 0);
  i2c_write (0x5c, rgb, 3);
}

static void i2c_setup_sensor_i2c (void)
{
  ENTRY;

#if defined (PIN_SENSOR_PWR_EN)
  IOMUX_PIN_CONFIG_GPIO	 (PIN_SENSOR_PWR_EN);
  GPIO_PIN_CONFIG_OUTPUT (PIN_SENSOR_PWR_EN);
  GPIO_PIN_SET		 (PIN_SENSOR_PWR_EN);	/* Enable sensor power */

  IOMUX_PIN_CONFIG_GPIO  (PIN_SENSOR_BUF_EN);
  GPIO_PIN_CONFIG_OUTPUT (PIN_SENSOR_BUF_EN);
  if (IS_ROSENCRANTZ_FF)
    GPIO_PIN_CLEAR	 (PIN_SENSOR_BUF_EN);	/* Enable sensor signal bufr */
  else
    GPIO_PIN_SET	 (PIN_SENSOR_BUF_EN);	/* Enable sensor signal bufr */

  usleep (1000);		/* delay 1ms while camera powers on */

  IOMUX_PIN_CONFIG_FUNC  (MX31_PIN_CSI_PIXCLK);
  IOMUX_PIN_CONFIG_FUNC  (MX31_PIN_CSI_MCLK);
#endif

	/* Post divider for CSI. */
  MASK_AND_SET (CCM_PDR0, (0x1ff << 23), (0x58 << 23));

#if 0
  {
    char v1;
    char v2;
    i2c_write (0x5c, "\x00", 1);
    i2c_read  (0x5c, &v1, 1);
    i2c_write (0x5c, "\xf0", 1);
    i2c_read  (0x5c, &v2, 1);
    printf ("i2c %02x%02x\n", v1, v2);
  }
#endif
}

static void setup_sensor (void)
{
  printf ("sensor setup %dx%d\n", FRAME_WIDTH, FRAME_HEIGHT);
  i2c_sensor_write (SENSOR_WINDOW_WIDTH, FRAME_WIDTH);
  usleep (1000);
  i2c_sensor_write (SENSOR_WINDOW_HEIGHT, FRAME_HEIGHT);
  usleep (1000);
  i2c_sensor_write (SENSOR_MAXIMUM_SHUTTER_WIDTH, 340);
  usleep (1000);

  i2c_sensor_write (SENSOR_LED_POLARITY, 1<<1); /* LED output is active low */
  usleep (1000);

#if defined (FRAME_HORZ_BLANKING)
  i2c_sensor_write (SENSOR_HORIZONTAL_BLANKING, FRAME_HORZ_BLANKING);
  usleep (1000);
  i2c_sensor_write (SENSOR_VERTICAL_BLANKING, FRAME_VERT_BLANKING);
  usleep (1000);
#endif
}

static void i2c_sensor_probe (void)
{
  int v = i2c_sensor_read (0);
  printf ("camera version %04x\n", v);
}

static void ipu_setup_sensor (void)
{
  ENTRY;

#if defined (PIN_CMOS_STBY)
  IOMUX_PIN_CONFIG_GPIO  (PIN_CMOS_STBY);
  GPIO_PIN_CONFIG_OUTPUT (PIN_CMOS_STBY);
  GPIO_PIN_SET		 (PIN_CMOS_STBY);	/* Put camera in standby */

  IOMUX_PIN_CONFIG_GPIO  (PIN_NCMOS_RESET);
  GPIO_PIN_CONFIG_OUTPUT (PIN_NCMOS_RESET);
  GPIO_PIN_SET		 (PIN_NCMOS_RESET);	/* Prep camera reset signal */

  IOMUX_PIN_CONFIG_GPIO  (PIN_CMOS_EXPOSURE);
  GPIO_PIN_CONFIG_OUTPUT (PIN_CMOS_EXPOSURE);
  GPIO_PIN_CLEAR	 (PIN_CMOS_EXPOSURE);

  IOMUX_PIN_CONFIG_GPIO  (PIN_ILLUMINATION_EN1);
  GPIO_PIN_CONFIG_OUTPUT (PIN_ILLUMINATION_EN1);
  GPIO_PIN_CLEAR	 (PIN_ILLUMINATION_EN1);

  IOMUX_PIN_CONFIG_GPIO  (PIN_ILLUMINATION_EN2);
  GPIO_PIN_CONFIG_OUTPUT (PIN_ILLUMINATION_EN2);
  GPIO_PIN_CLEAR	 (PIN_ILLUMINATION_EN2);

  IOMUX_PIN_CONFIG_FUNC  (MX31_PIN_CSI_D15);
  IOMUX_PIN_CONFIG_FUNC  (MX31_PIN_CSI_D14);
  IOMUX_PIN_CONFIG_FUNC  (MX31_PIN_CSI_D13);
  IOMUX_PIN_CONFIG_FUNC  (MX31_PIN_CSI_D12);
  IOMUX_PIN_CONFIG_FUNC  (MX31_PIN_CSI_D11);
  IOMUX_PIN_CONFIG_FUNC  (MX31_PIN_CSI_D10);
  IOMUX_PIN_CONFIG_FUNC  (MX31_PIN_CSI_D9);
  IOMUX_PIN_CONFIG_FUNC  (MX31_PIN_CSI_D8);
  IOMUX_PIN_CONFIG_FUNC  (MX31_PIN_CSI_D7);
  IOMUX_PIN_CONFIG_FUNC  (MX31_PIN_CSI_D6);

  IOMUX_PIN_CONFIG_FUNC  (MX31_PIN_CSI_VSYNC);
  IOMUX_PIN_CONFIG_FUNC  (MX31_PIN_CSI_HSYNC);

  GPIO_PIN_CLEAR	 (PIN_NCMOS_RESET);
  udelay (1);
  GPIO_PIN_SET		 (PIN_NCMOS_RESET);
  udelay (1);
  GPIO_PIN_CLEAR	 (PIN_CMOS_STBY); /* Release camera from standby */
#endif
}

#if defined (MODE_VF)

void channel_rgb_setup (int channel, int dx, int dy, void* pa, void* pb)
{
  char rgb[(132 + 7)/8];

  memset (rgb, 0, sizeof (rgb));
  write_huge (rgb, 1, IPU_CW_NSB);
  write_huge (rgb, dx - 1, IPU_CW_FW);
  write_huge (rgb, dy - 1, IPU_CW_FH);
  ipu_write_ima (1, 2*channel, rgb, 132);

  memset (rgb, 0, sizeof (rgb));
  write_huge (rgb, (unsigned long) pa, IPU_CW_EBA0);
  write_huge (rgb, (unsigned long) pb, IPU_CW_EBA1);

#if 1
  /* 16 bpp */
  write_huge (rgb, 2, IPU_CW_BPP); /* 16 bpp */
  write_huge (rgb, 16-1, IPU_CW_NPB);
  write_huge (rgb, 2, IPU_CW_SAT);
  write_huge (rgb, 4, IPU_CW_PFS);
  write_huge (rgb, dx*2 - 1, IPU_CW_SL);
  write_huge (rgb, 4, IPU_CW_WID0);
  write_huge (rgb, 5, IPU_CW_WID1);
  write_huge (rgb, 4, IPU_CW_WID2);
  write_huge (rgb, 0, IPU_CW_WID3);
  write_huge (rgb, 0, IPU_CW_OFS0);
  write_huge (rgb, 5, IPU_CW_OFS1);
  write_huge (rgb, 11, IPU_CW_OFS2);
  write_huge (rgb, 16, IPU_CW_OFS3);
#else
  /* 32 bpp */
  write_huge (rgb, 0, IPU_CW_BPP); /* 32 bpp */
  write_huge (rgb, 8-1, IPU_CW_NPB);
  write_huge (rgb, 2, IPU_CW_SAT);
  write_huge (rgb, 4, IPU_CW_PFS);
  write_huge (rgb, dx*4 - 1, IPU_CW_SL);
  write_huge (rgb, 10, IPU_CW_WID0);
  write_huge (rgb, 10, IPU_CW_WID1);
  write_huge (rgb, 10, IPU_CW_WID2);
  write_huge (rgb,  0, IPU_CW_WID3);
  write_huge (rgb,  0, IPU_CW_OFS0);
  write_huge (rgb, 10, IPU_CW_OFS1);
  write_huge (rgb, 20, IPU_CW_OFS2);
  write_huge (rgb, 30, IPU_CW_OFS3);
#endif

  ipu_write_ima (1, 2*channel + 1, rgb, 132);

  if (pb)
    IPU_CHA_DB_MODE_SEL |= 1<<channel;
  ipu_channel_ready (channel, 0);
  if (pb)
    ipu_channel_ready (channel, 1);
}

static void csc1_setup (void)
{
  char rgb[(42 + 7)/8];

  memset (rgb, 0, sizeof (rgb));
  write_huge (rgb, 0x12579c00, 0, 32);
  write_huge (rgb, 0x000002f2, 32, 32);
  ipu_write_ima (0, 0x5a5, rgb, 42);

  memset (rgb, 0, sizeof (rgb));
  write_huge (rgb, 0x50012A95, 0, 32);
  write_huge (rgb, 0x00000008, 32, 32);
  ipu_write_ima (0, 0x5a6, rgb, 42);

  memset (rgb, 0, sizeof (rgb));
  write_huge (rgb, 0xb33330ff, 0, 32);
  write_huge (rgb, 0x000000ee, 32, 32);
  ipu_write_ima (0, 0x5a7, rgb, 42);
}

#endif

static void ipu_setup (void)
{
  int channel = 7;
  printf ("ipu_setup\n");

  /* ==== Configure common parameters */

  IPU_CONF	&= ~(1<<8);			/* Setting endian-ness only */
  IC_CONF	= 0;				/* Disable all tasks */
  IDMAC_CONF	= IDMAC_CONF_SINGLE_AHB_M_EN;

  /* ==== SYSTEM Configure sensor interface */

  CSI_CONF	= 0
    | CSI_CONF_SENS_PRTCL_GATED
//    | CSI_CONF_SENS_PRTCL_NONGATED
#if defined (MODE_GENERIC)
    | CSI_CONF_SENS_DATA_FORMAT_GENERIC
    | CSI_CONF_DATA_WIDTH_15BIT
#endif
    ;

  CSI_SENS_FRM_SIZE = 0
    | ((FRAME_WIDTH/FRAME_WIDTH_DIVISOR - 1)  << CSI_FRM_SIZE_WIDTH_SH)
    | ((FRAME_HEIGHT - 1) << CSI_FRM_SIZE_HEIGHT_SH);


  /* ==== TASK Configure and initialize CSI IC IDMAC */

  CSI_ACT_FRM_SIZE = 0
    | ((FRAME_WIDTH/FRAME_WIDTH_DIVISOR - 1)  << CSI_FRM_SIZE_WIDTH_SH)
    | ((FRAME_HEIGHT - 1) << CSI_FRM_SIZE_HEIGHT_SH)
    ;

  CSI_OUT_FRM_CTRL = 0;		/* Should be redundant */

#if defined (MODE_GENERIC)
  IC_PRP_ENC_RSC = 0x20002000;	/* 100% scaling */
#endif

				/* IC Task options */
  IC_CONF = 0
#if defined (MODE_GENERIC)
    | IC_CONF_CSI_MEM_WR_EN	/* Allow direct write from CSI to memory */
//    | IC_CONF_CSI_RWS_EN	/* Raw sensor mode */
//    | IC_CONF_PRPENC_EN		/* Enable the encoder */
#else
    | IC_CONF_CSI_MEM_WR_EN	/* Allow direct write from CSI to memory */
#endif
#if defined (MODE_VF)
    | IC_CONF_PRPVF_PRPVF_EN
    | IC_CONF_PRPVF_CSC1
#endif
    ;

	/* Initialize IDMAC register file */
  {
    char rgb[(132 + 7)/8];

    /* Interleaved */
    memset (rgb, 0, sizeof (rgb));
    write_huge (rgb, 0, IPU_CW_XV);
    write_huge (rgb, 0, IPU_CW_YV);
    write_huge (rgb, 0, IPU_CW_XB);
    write_huge (rgb, 0, IPU_CW_YB);
    write_huge (rgb, 0, IPU_CW_SCE);
    write_huge (rgb, 1, IPU_CW_NSB);
    write_huge (rgb, 0, IPU_CW_LNPB);
    write_huge (rgb, 0, IPU_CW_SX);
    write_huge (rgb, 0, IPU_CW_SY);
    write_huge (rgb, 0, IPU_CW_NS);
    write_huge (rgb, 0, IPU_CW_SM);
    write_huge (rgb, 0, IPU_CW_SDX);
    write_huge (rgb, 0, IPU_CW_SDY);
    write_huge (rgb, 0, IPU_CW_SDRX);
    write_huge (rgb, 0, IPU_CW_SDRY);
    write_huge (rgb, 0, IPU_CW_SCRQ);
    write_huge (rgb, FRAME_WIDTH/FRAME_WIDTH_DIVISOR - 1,
			  IPU_CW_FW);
    write_huge (rgb, FRAME_HEIGHT - 1, IPU_CW_FH);
//    dumpw (rgb, sizeof (rgb), 0, 0);
    ipu_write_ima (1, 2*channel + 0, rgb, 132);

//    memset (rgb, 0, sizeof (rgb));
//    ipu_read_ima (1, 2*channel + 0, rgb, 132);
//    dumpw (rgb, sizeof (rgb), 0, 0);

    memset (rgb, 0, sizeof (rgb));
    write_huge (rgb, (unsigned long)rgbFrameA, IPU_CW_EBA0);
    write_huge (rgb, (unsigned long)rgbFrameB, IPU_CW_EBA1);
#if defined (MODE_GENERIC)
    write_huge (rgb, 2, IPU_CW_BPP);
    write_huge (rgb, (FRAME_WIDTH/FRAME_WIDTH_DIVISOR)*2 - 1, IPU_CW_SL);
//    write_huge (rgb, (FRAME_WIDTH/FRAME_WIDTH_DIVISOR)*4 - 1, IPU_CW_SL);
//    write_huge (rgb, (FRAME_WIDTH/FRAME_WIDTH_DIVISOR)*8 - 1, IPU_CW_SL);
    write_huge (rgb, 7, IPU_CW_PFS);
#endif
    write_huge (rgb, 0, IPU_CW_BAM);
//    write_huge (rgb, 8 - 1, IPU_CW_NPB);
    write_huge (rgb, 16 - 1, IPU_CW_NPB);
    write_huge (rgb, 2, IPU_CW_SAT);
    write_huge (rgb, 2, IPU_CW_SCC);
    write_huge (rgb, 0, IPU_CW_OFS0);
    write_huge (rgb, 0, IPU_CW_OFS1);
    write_huge (rgb, 0, IPU_CW_OFS2);
    write_huge (rgb, 0, IPU_CW_OFS3);
    write_huge (rgb, 0, IPU_CW_WID0);
    write_huge (rgb, 0, IPU_CW_WID1);
    write_huge (rgb, 0, IPU_CW_WID2);
    write_huge (rgb, 0, IPU_CW_WID3);
    write_huge (rgb, 0, IPU_CW_DEC_SEL);
//    dumpw (rgb, sizeof (rgb), 0, 0);
    ipu_write_ima (1, 2*channel + 1, rgb, 132);

//    memset (rgb, 0, sizeof (rgb));
//    ipu_read_ima (1, 2*channel + 1, rgb, 132);
//    dumpw (rgb, sizeof (rgb), 0, 0);

  }

#if defined (MODE_VF)
  csc1_setup ();
  channel_rgb_setup (1, FRAME_WIDTH, FRAME_HEIGHT,
		     rgbFrameC, rgbFrameD); /* viewfinder setup */
  IPU_FS_PROC_FLOW = 3;		/* VF source data is valid */
#endif

  IDMAC_CHA_PRI |= 1<<channel;	/* This channel is high priority */
  IPU_CHA_DB_MODE_SEL |= 1<<channel;
//  IPU_CHA_DB_MODE_SEL &= ~(1<<channel);
  ipu_channel_ready (channel, 0);
  ipu_channel_ready (channel, 1);

  /* ==== Initialize sensors */

	/* Initialize sensor via i2c if needed. */
  setup_sensor ();

  /* ==== Enabling tasks */

  IDMAC_CHA_EN |= 1<<channel;	/* Enable channel 7 */
#if defined (MODE_VF)
  IDMAC_CHA_EN |= 1<<(1);	/* Enable channel 1 */
#endif
  IPU_CONF     |= IPU_CONF_CSI_EN | IPU_CONF_IC_EN;

  /* ==== Resuming tasks */

  /* ==== Reconfiguring and resuming tasks */

  /* ==== Disabling tasks */
}

static void ipu_setup_diagb (void)
{
  IOMUX_PIN_CONFIG_ALT_OUT (MX31_PIN_I2C_CLK, 3);	/* IPU_DIAGB[0] */
  IOMUX_PIN_CONFIG_ALT_OUT (MX31_PIN_I2C_DAT, 3);	/* IPU_DIAGB[1] */
  IOMUX_PIN_CONFIG_ALT_OUT (MX31_PIN_STXD3, 3);		/* IPU_DIAGB[2] */
  IOMUX_PIN_CONFIG_ALT_OUT (MX31_PIN_SRXD3, 3);		/* IPU_DIAGB[3] */
  IOMUX_PIN_CONFIG_ALT_OUT (MX31_PIN_SCK3, 3);		/* IPU_DIAGB[4] */
  IOMUX_PIN_CONFIG_ALT_OUT (MX31_PIN_SFS3, 3);		/* IPU_DIAGB[5] */
  IOMUX_PIN_CONFIG_ALT_OUT (MX31_PIN_STXD4, 3);		/* IPU_DIAGB[6] */
  IOMUX_PIN_CONFIG_ALT_OUT (MX31_PIN_SRXD4, 3);		/* IPU_DIAGB[7] */
  IOMUX_PIN_CONFIG_ALT_OUT (MX31_PIN_SCK4, 3);		/* IPU_DIAGB[8] */
  IOMUX_PIN_CONFIG_ALT_OUT (MX31_PIN_SFS4, 3);		/* IPU_DIAGB[9] */
  IOMUX_PIN_CONFIG_ALT_OUT (MX31_PIN_STXD5, 3);		/* IPU_DIAGB[10] */
  IOMUX_PIN_CONFIG_ALT_OUT (MX31_PIN_SRXD5, 3);		/* IPU_DIAGB[11] */
  IOMUX_PIN_CONFIG_ALT_OUT (MX31_PIN_SCK5, 3);		/* IPU_DIAGB[12] */
  IOMUX_PIN_CONFIG_ALT_OUT (MX31_PIN_SFS5, 3);		/* IPU_DIAGB[13] */
  IOMUX_PIN_CONFIG_ALT_OUT (MX31_PIN_STXD6, 3);		/* IPU_DIAGB[14] */
  IOMUX_PIN_CONFIG_ALT_OUT (MX31_PIN_SRXD6, 3);		/* IPU_DIAGB[15] */
  IOMUX_PIN_CONFIG_ALT_OUT (MX31_PIN_SCK6, 3);		/* IPU_DIAGB[16] */
  IOMUX_PIN_CONFIG_ALT_OUT (MX31_PIN_SFS6, 3);		/* IPU_DIAGB[17] */
  IOMUX_PIN_CONFIG_ALT_OUT (MX31_PIN_CSPI1_MOSI, 3);	/* IPU_DIAGB[18] */
  IOMUX_PIN_CONFIG_ALT_OUT (MX31_PIN_CSPI1_MISO, 3);	/* IPU_DIAGB[19] */
  IOMUX_PIN_CONFIG_ALT_OUT (MX31_PIN_CSPI1_SS0, 3);	/* IPU_DIAGB[20] */
  IOMUX_PIN_CONFIG_ALT_OUT (MX31_PIN_CSPI1_SS2, 3);	/* IPU_DIAGB[21] */
  IOMUX_PIN_CONFIG_ALT_OUT (MX31_PIN_CSPI1_SS2, 3);	/* IPU_DIAGB[22] */
  IOMUX_PIN_CONFIG_ALT_OUT (MX31_PIN_CSPI1_SCLK, 3);	/* IPU_DIAGB[23] */
  IOMUX_PIN_CONFIG_ALT_OUT (MX31_PIN_CSPI1_SPI_RDY, 3);	/* IPU_DIAGB[24] */
  IOMUX_PIN_CONFIG_ALT_OUT (MX31_PIN_RI_DTE1, 3);	/* IPU_DIAGB[25] */
  IOMUX_PIN_CONFIG_ALT_OUT (MX31_PIN_DCD_DTE1, 3);	/* IPU_DIAGB[26] */
  IOMUX_PIN_CONFIG_ALT_OUT (MX31_PIN_DTR_DCE2, 3);	/* IPU_DIAGB[27] */
  IOMUX_PIN_CONFIG_ALT_OUT (MX31_PIN_RXD2, 3);		/* IPU_DIAGB[28] */
  IOMUX_PIN_CONFIG_ALT_OUT (MX31_PIN_TXD2, 3);		/* IPU_DIAGB[29] */
  IOMUX_PIN_CONFIG_ALT_OUT (MX31_PIN_RTS2, 3);		/* IPU_DIAGB[30] */
  IOMUX_PIN_CONFIG_ALT_OUT (MX31_PIN_CTS2, 3);		/* IPU_DIAGB[31] */

  IPU_CONF |= (1<<7);		/* DU_EN */
  IPU_DIAGB_CTRL = 0x1;		/* CSI module monitoring */

  printf ("[5] SFS3  0x%x 0x%lx %p\n",
	  MX31_PIN_SFS3, IOMUX_PIN_REG (MX31_PIN_SFS3),
	  &IOMUX_PIN_REG (MX31_PIN_SFS3));
  printf ("[6] STXD4 0x%x 0x%lx %p\n",
	  MX31_PIN_STXD4, IOMUX_PIN_REG (MX31_PIN_STXD4),
	  &IOMUX_PIN_REG (MX31_PIN_STXD4));
}


static void csi_time_frames (void)
{
  unsigned long time = timer_read ();
  int c = 0;

  while (timer_delta (time, timer_read ()) < 1000) {
    IPU_INT_STAT3 = (1<<6);	/* CSI_EOF */
    while (!(IPU_INT_STAT3 & (1<<6)))
      if (timer_delta (time, timer_read ()) > 2000) {
	printf ("  timeout (%d frames read)\n", c);
	return;
      }
    if (c == 0)
      time = timer_read ();	/* Reset timer so we get a full second */
    ++c;
  }

  printf ("count is %d for 1 second\n", c);
}

static void ic7_time_frames (void)
{
  unsigned long time = timer_read ();
  int c = 0;
  int c_err = 0;

  while (timer_delta (time, timer_read ()) < 1000) {
    IPU_INT_STAT1 = (1<<7);	/* IC7_EOF */
    while (!(IPU_INT_STAT1 & (1<<7)))
      if (timer_delta (time, timer_read ()) > 2000) {
	printf ("  timeout (%d frames read)\n", c);
	return;
      }
    if (c == 0)
      time = timer_read ();	/* Reset timer so we get a full second */
    ++c;
    if (IPU_INT_STAT5 & IPU_INT_STAT5_BAYER_FRM_LOST_ERR)
      ++c_err;
  }

  printf ("count is %d for 1 second, %d errors\n", c, c_err);
}

static void rdy_time_frames (void)
{
  unsigned long time = timer_read ();
  int c = 0;
  int c_err = 0;
  static const int channel = 7;

  ipu_channel_ready (channel, 0);
  ipu_channel_ready (channel, 1);
  IPU_INT_STAT5 = IPU_INT_STAT5_BAYER_FRM_LOST_ERR;

  while (timer_delta (time, timer_read ()) < 1000) {
    ipu_channel_ready (channel, 0);
    ipu_channel_ready (channel, 1);

    while (ipu_channel_busy (channel, 0) && ipu_channel_busy (channel, 1))
      if (timer_delta (time, timer_read ()) > 2000) {
	printf ("  timeout (%d frames read)\n", c);
	return;
      }
    if (c == 0)
      time = timer_read ();	/* Reset timer so we get a full second */
    ++c;
    if (IPU_INT_STAT5 & IPU_INT_STAT5_BAYER_FRM_LOST_ERR)
      ++c_err;
  }

  printf ("count is %d for 1 second, %d errors\n", c, c_err);
}

static int cmd_ipu (int argc, const char** argv)
{
  ENTRY;
  if (argc < 2)
    return ERROR_PARAM;

  if (rgbFrameA == 0 && 0) {
    rgbFrameA = alloc_uncached (CB_FRAME, 4096);
    rgbFrameB = alloc_uncached (CB_FRAME, 4096);
    rgbFrameC = alloc_uncached (CB_FRAME, 4096);
    rgbFrameD = alloc_uncached (CB_FRAME, 4096);
    rgbFrameE = alloc_uncached (CB_FRAME, 4096);
    rgbFrameF = alloc_uncached (CB_FRAME, 4096);
    memset ((void*) rgbFrameA, 0xa5, CB_FRAME);
    memset ((void*) rgbFrameB, 0xa5, CB_FRAME);
    memset ((void*) rgbFrameC, 0xa5, CB_FRAME);
    memset ((void*) rgbFrameD, 0xa5, CB_FRAME);
    memset ((void*) rgbFrameE, 0xa5, CB_FRAME);
    memset ((void*) rgbFrameF, 0xa5, CB_FRAME);

    printf ("Frames [%p %p %p %p %p %p]\n",
	    rgbFrameA, rgbFrameB, rgbFrameC,
	    rgbFrameD, rgbFrameE, rgbFrameF);
  }

  /* Initialize */
  if (strcmp (argv[1], "i") == 0) {
    i2c_setup_sensor_i2c ();
    i2c_setup ();
    ipu_setup_sensor ();
//    ipu_setup_diagb ();
    ipu_setup ();
  }

  if (strcmp (argv[1], "diag") == 0) {
    printf ("initializing diagb\n");
    ipu_setup_diagb ();
  }

  /* Query */
  if (strcmp (argv[1], "q") == 0) {
    ipu_report_irq ();
    i2c_report ();
  }

  /* Report */
  if (strcmp (argv[1], "r") == 0) {
    ipu_report ();
  }

  /* Zero */
  if (strcmp (argv[1], "0") == 0) {
    ipu_zero ();
  }

  /* Time */
  if (strcmp (argv[1], "t") == 0) {
    csi_time_frames ();
  }

  /* Time */
  if (strcmp (argv[1], "tt") == 0) {
    ic7_time_frames ();
  }

  /* Time */
  if (strcmp (argv[1], "t2") == 0) {
    rdy_time_frames ();
  }


  /* Clear */
  if (strcmp (argv[1], "c") == 0) {
    printf ("clearing status\n");
    IPU_INT_STAT3 = (1<<6);   /* CSI_EOF */
    IPU_INT_STAT3 = (1<<5);   /* CSI_NF */
    IPU_INT_STAT5 = IPU_INT_STAT5_BAYER_FRM_LOST_ERR;
    IPU_INT_STAT1 = (1<<7);   /* IC7_EOF */
    IPU_INT_STAT2 = (1<<7);   /* IC7_NFACK */
  }

#if 0
  /* Dump */
  if (strcmp (argv[1], "d") == 0) {

    char rgb[(132 + 7)/8];
    memset (rgb, 0, sizeof (rgb));
    ipu_read_ima (1, 2*14 + 0, rgb, 132);
    dumpw (rgb, sizeof (rgb), 0, 0);
    memset (rgb, 0, sizeof (rgb));
    ipu_read_ima (1, 2*14 + 1, rgb, 132);
    dumpw (rgb, sizeof (rgb), 0, 0);


#if 0

    char rgb[96*64/8];
    memset (rgb, 0, sizeof (rgb));
//    ipu_read_ima (7, 0, rgb, sizeof (rgb)*8);
    ipu_read_ima (8, 0, rgb, 64);
    dumpw (rgb, sizeof (rgb), 0, 0);
#endif
  }
#endif

  /* IDMAC config read */
  if (strcmp (argv[1], "ir") == 0) {
    char rgb[(132 + 7)/8];
    memset (rgb, 0, sizeof (rgb));
    ipu_read_ima (1, 2*7 + 0, rgb, 132);
    dumpw (rgb, sizeof (rgb), 0, 0);
    memset (rgb, 0, sizeof (rgb));
    ipu_read_ima (1, 2*7 + 1, rgb, 132);
    dumpw (rgb, sizeof (rgb), 0, 0);
  }

  if (strcmp (argv[1], "size") == 0) {
    int dx = simple_strtoul (argv[2], NULL, 10);
    int dy = simple_strtoul (argv[3], NULL, 10);
    printf ("reprogramming to %dx%d\n", dx, dy);
    i2c_sensor_write (4, dx);
    usleep (1000);
    i2c_sensor_write (3, dy);
    usleep (1000);
    printf ("sensor width  %d\n", i2c_sensor_read (4));
    printf ("sensor height  %d\n", i2c_sensor_read (3));
  }

  if (strcmp (argv[1], "p") == 0) {
    i2c_sensor_probe ();
    printf ("sensor width  %d\n", i2c_sensor_read (4));
    printf ("sensor height %d\n", i2c_sensor_read (3));
    printf ("sensor test register 0x%x\n", i2c_sensor_read (0x7f));
    printf ("sensor noise corr 0x%x\n", i2c_sensor_read (0x70));
  }

  if (strcmp (argv[1], "s") == 0) { /* Single frame capture */
    int channel = 7;
    printf ("single frame capture\n");
//    IDMAC_CHA_EN &= 1<<channel;
    IPU_CONF     &= ~(IPU_CONF_CSI_EN | IPU_CONF_IC_EN);
    IDMAC_CHA_EN = 0;
    memset (rgbFrameA, 0xa5, CB_FRAME);
    memset (rgbFrameB, 0xa5, CB_FRAME);
    IPU_CONF     |= IPU_CONF_CSI_EN;
    IPU_INT_STAT3 = (1<<6);	/* CSI_EOF */
    while (!(IPU_INT_STAT3 & (1<<6)))
      ;
    IPU_CHA_BUF0_RDY |= 1<<channel;
    IPU_CHA_BUF1_RDY |= 1<<channel;
    IDMAC_CHA_EN = 1<<channel;
    IPU_CONF     |= IPU_CONF_IC_EN;
    IPU_INT_STAT3 = (1<<6);	/* CSI_EOF */
    while (!(IPU_INT_STAT3 & (1<<6)))
      ;
    IPU_CONF     &= ~(IPU_CONF_CSI_EN | IPU_CONF_IC_EN);
    IDMAC_CHA_EN = 0;
//    IPU_CHA_BUF0_RDY &= 1<<channel;
//    IPU_CHA_BUF1_RDY &= 1<<channel;
  }

  if (strcmp (argv[1], "g") == 0) {
    int channel = 7;
    printf ("restart capturing\n");
    IDMAC_CHA_EN |= 1<<channel;
    IPU_CHA_BUF0_RDY |= 1<<channel;
    IPU_CHA_BUF1_RDY |= 1<<channel;
    IPU_CONF	     |= IPU_CONF_CSI_EN | IPU_CONF_IC_EN;
  }

#if 1
  if (strcmp (argv[1], "w") == 0) {
    printf ("reprogramming to %dx%d\n", FRAME_WIDTH, FRAME_HEIGHT);
    i2c_sensor_write (SENSOR_WINDOW_WIDTH, FRAME_WIDTH);
    usleep (1000);
    i2c_sensor_write (SENSOR_WINDOW_HEIGHT, FRAME_HEIGHT);
    usleep (1000);
    i2c_sensor_write (SENSOR_MAXIMUM_SHUTTER_WIDTH, 340);
    usleep (1000);
#if defined (FRAME_HORZ_BLANKING)
    i2c_sensor_write (SENSOR_HORIZONTAL_BLANKING, FRAME_HORZ_BLANKING);
    usleep (1000);
    i2c_sensor_write (SENSOR_VERTICAL_BLANKING, FRAME_VERT_BLANKING);
    usleep (1000);
#endif
    printf ("sensor width    %d\n", i2c_sensor_read (SENSOR_WINDOW_WIDTH));
    printf ("sensor height   %d\n", i2c_sensor_read (SENSOR_WINDOW_HEIGHT));
    printf ("sensor h blank  %d\n",
	    i2c_sensor_read (SENSOR_HORIZONTAL_BLANKING));
    printf ("sensor v blank  %d\n",
	    i2c_sensor_read (SENSOR_VERTICAL_BLANKING));
  }
#endif

  if (strcmp (argv[1], "cap") == 0) {
    memset ((void*) rgbFrameA, 0xa5, CB_FRAME);
    memset ((void*) rgbFrameB, 0xa5, CB_FRAME);
    IPU_CHA_BUF0_RDY |= (1<<7);
    IPU_CHA_BUF1_RDY |= (1<<7);
  }

  if (strcmp (argv[1], "test") == 0) {
    char mode = 0;
    if (argc > 2)
      mode = *argv[2];
    i2c_setup_sensor_i2c ();
    i2c_setup ();
    i2c_sensor_write (0x70, 0x14);
    usleep (1000);
    switch (mode) {
    case 'D':
    case 'd':
      i2c_sensor_write (0x7f, (1<<13)|(3<<11)); /* Diagonal */
      break;
    case 'H':
    case 'h':
    default:
      i2c_sensor_write (0x7f, (1<<13)|(2<<11)); /* Horizontal */
      break;
    case 'V':
    case 'v':
      i2c_sensor_write (0x7f, (1<<13)|(1<<11)); /* Vertical */
      break;
    }
    usleep (1000);
    printf ("0x70 -> %x\n", i2c_sensor_read (0x70));
    printf ("0x7f -> %x\n", i2c_sensor_read (0x7f));
  }

  if (strcmp (argv[1], "i2c") == 0) {
    int address;
    i2c_setup_sensor_i2c ();
    i2c_setup ();
    for (address = 0; address < 128; ++address) {
      char rgb[2] = { 0, 0 };
      int result = i2c_write (address, rgb, 2);
      if (result == 0)
	printf ("I2C device found at address %d (0x%x)\n", address, address);
    }

  }

  if (strcmp (argv[1], "registers") == 0) {
    unsigned short rgs[256];
    int i;
    for (i = 0; i < 256; ++i)
      rgs[i] = i2c_sensor_read (i);
    dumpw ((void*) rgs, sizeof (rgs), 0, 2);
  }

  return 0;
}

static __command struct command_d c_ipu = {
  .command = "ipu",
  .func = (command_func_t) cmd_ipu,
  COMMAND_DESCRIPTION ("ipu test")
  COMMAND_HELP(
"ipu\n"
"  IPU tests.\n"
"  i		initialize\n"
"  diag		enable DIAGB\n"
"  q		query interrupts\n"
"  r		report all registers\n"
"  0		zero IDMAC control register\n"
"  t		time frames per second\n"
"  ir		IDMAC register read/dump\n"
"  p		probe\n"
"  i2c		i2c setup/query\n"
  )
};
