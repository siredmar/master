/* drv-clcdc.c

   written by Marc Singer
   7 Dec 2004

   Copyright (C) 2004 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   Sharp LH CLCD controller initialization, splash, and test support.

   This code needs a serious overhaul to remove the ifdef's from
   within the bodies of the functions.  Macros for the custom
   functions please.

   Splash Command
   --------------

   This code is, more or less, complete.  It doesn't read from tftp
   and it probably doesn't read from a filesystem either.  That has to
   be fixed.  It also doesn't double-buffer which would make the
   display snappier.  However, the image decode is robust.


   Target Macros
   -------------

   Some of the functions of this driver are target specific.  Instead
   of including all of the ifdefs within the body of the driver, each
   target supplies these functions as macros in the header
   mach/drv-clcdc.h.  A target may choose to not define a macro in
   which case it will be a NOP.  The macros are as follows.

   DRV_CLCDC_SETUP			- Setup clocks and muxing.
   DRV_CLCDC_POWER_ENABLE		- Enable power to LCD panel
   DRV_CLCDC_BACKLIGHT_ENABLE		- Enable power backlight power
   DRV_CLCDC_BACKLIGHT_DISABLE		- Disable backlight power
   DRV_CLCDC_DISABLE			- Extra LCD controller disabling
   DRV_CLCDC_POWER_DISABLE		- Disable power to LCD panel
   DRV_CLCDC_RELEASE			- Release clocks


   Nomenclature -- Porches
   -----------------------

   Front porches and back porches refer to the sync pulse as opposed
   to the displayed frame.  The front porch is in front of the sync
   pulse, the back porch follows it.

*/

#include <config.h>
#include <driver.h>
#include <service.h>
#include <linux/string.h>
#include <apex.h>
#include <mach/hardware.h>
#include <png.h>
#include <command.h>
#include <error.h>
#include <asm/mmu.h>
#include <mach/drv-clcdc.h>
#include <debug_ll.h>

#if ! defined (DRV_CLCDC_SETUP)
# define DRV_CLCDC_SETUP
#endif
#if ! defined (DRV_CLCDC_POWER_ENABLE)
# define DRV_CLCDC_POWER_ENABLE
#endif
#if ! defined (DRV_CLCDC_BACKLIGHT_ENABLE)
# define DRV_CLCDC_BACKLIGHT_ENABLE
#endif
#if ! defined (DRV_CLCDC_BACKLIGHT_DISABLE)
# define DRV_CLCDC_BACKLIGHT_DISABLE
#endif
#if ! defined (DRV_CLCDC_DISABLE)
# define DRV_CLCDC_DISABLE
#endif
#if ! defined (DRV_CLCDC_POWER_DISABLE)
# define DRV_CLCDC_POWER_DISABLE
#endif
#if ! defined (DRV_CLCDC_RELEASE)
# define DRV_CLCDC_RELEASE
#endif
#if ! defined (DRV_CLCDC_WAKE)
# define DRV_CLCDC_WAKE
#endif
#if ! defined (DRV_CLCDC_SLEEP)
# define DRV_CLCDC_SLEEP
#endif

//#define USE_BORDER
//#define USE_FILL

#define BPP1	(0<<1)
#define BPP2	(1<<1)
#define BPP4	(2<<1)
#define BPP8	(3<<1)
#define BPP16	(4<<1)

#define NS_TO_CLOCK(ns,c)	((((ns)*((c)/1000) + (1000000 - 1))/1000000))
#define CLOCK_TO_DIV(e,c)	(((c) + (e) - 1)/(e))

#if defined (CONFIG_LCD_3_5_QVGA_20)
	/* Sharp PN LQ035Q7DB02 */
# define PANEL_NAME		"LCD 3.5\" QVGA"
# define PEL_CLOCK_EST		(6800*1000)     /* MHz/4.5/?/6.8 */
# define PEL_CLOCK_DIV		CLOCK_TO_DIV(PEL_CLOCK_EST, HCLK)
# define PEL_CLOCK		(HCLK/PEL_CLOCK_DIV)
# define PEL_WIDTH		(240)
# define PEL_HEIGHT		(320)
# define BIT_DEPTH		(16)
# define BITS_PER_PEL_2		BPP16
# define HSYNC_WIDTH		(61)
# define VSYNC_WIDTH		NS_TO_CLOCK (60, PEL_CLOCK)
# define HORZ_BP		(16)
# define HORZ_FP		(21)
# define VERT_BP		(8)		/* 8 */
# define VERT_FP		(5)
# define INVERT_PIXEL_CLOCK
#endif

#if defined (CONFIG_LCD_5_7_QVGA_10)
	/* Sharp PN LQ057Q3DC02, QVGA mode */
# define PANEL_NAME		"LCD 5.7\" QVGA"
# define PEL_CLOCK_EST		(7*1000*1000)     /* MHz/?/6.3/7 */
# define PEL_CLOCK_DIV		CLOCK_TO_DIV(PEL_CLOCK_EST, HCLK)
# define PEL_CLOCK		(HCLK/PEL_CLOCK_DIV)
# define PEL_WIDTH		(320)
# define PEL_HEIGHT		(240)
# define BIT_DEPTH		(16)
# define BITS_PER_PEL_2		BPP16
# define HSYNC_WIDTH		(96)		/* clocks/2/96/200 */
# define VSYNC_WIDTH		(1)		/* lines/2/?/34 */
# define HORZ_BP		(21)
# define HORZ_FP		(15)
# define VERT_BP		(7)		/* 7 */
# define VERT_FP		(5)
# define INVERT_HSYNC
# define INVERT_VSYNC
#endif

#if defined (CONFIG_LCD_6_4_VGA_10)
	/* Sharp PN LQ64D343 */
# define PANEL_NAME		"LCD 6.4\" VGA"
# define PEL_CLOCK_EST		(28330000)     /* MHz/?/25.18/28.33 */
# define PEL_CLOCK_DIV		CLOCK_TO_DIV(PEL_CLOCK_EST, HCLK)
# define PEL_CLOCK		(HCLK/PEL_CLOCK_DIV)
# define PEL_WIDTH		(640)
# define PEL_HEIGHT		(480)
# define BIT_DEPTH		(16)
# define BITS_PER_PEL_2		BPP16
# define HORZ_CYCLE		(800)		/* clock/770/800/900 */
# define VERT_CYCLE		(525)		/* line/515/525/560 */
# define HSYNC_WIDTH		(96)		/* clocks/2/96/200 */
# define VSYNC_WIDTH		(2)		/* clocks/2/?/34 */
# define HORZ_BP		(32)
# define VERT_BP		(34)		/* 34 */
# define HORZ_FP		(HORZ_CYCLE-HORZ_BP-HSYNC_WIDTH-PEL_WIDTH)
# define VERT_FP		(VERT_CYCLE-VERT_BP-VSYNC_WIDTH-PEL_HEIGHT)
# define INVERT_HSYNC
# define INVERT_VSYNC
#endif

#if defined (CONFIG_LCD_10_4_VGA_10)
	/* Sharp PN LQ10D368 */
# define PANEL_NAME		"LCD 10.4\" VGA"
# define PEL_CLOCK_EST		(28330*1000)     /* MHz/?/25.18/28.33 */
# define PEL_CLOCK_DIV		CLOCK_TO_DIV(PEL_CLOCK_EST, HCLK)
# define PEL_CLOCK		(HCLK/PEL_CLOCK_DIV)
# define PEL_WIDTH		(640)
# define PEL_HEIGHT		(480)
# define BIT_DEPTH		(16)
# define BITS_PER_PEL_2		BPP16
# define HORZ_CYCLE		(800)		/* clock/750/800/900 */
# define VERT_CYCLE		(525)		/* line/515/525/560 */
# define HSYNC_WIDTH		(96)		/* clocks/2/96/200 */
# define VSYNC_WIDTH		(2)		/* lines/1/?/34 */
# define HORZ_BP		(21)
# define VERT_BP		(34)		/* lines/34 (480 line mode)*/
# define HORZ_FP		(HORZ_CYCLE-HORZ_BP-HSYNC_WIDTH-PEL_WIDTH)
# define VERT_FP		(VERT_CYCLE-VERT_BP-VSYNC_WIDTH-PEL_HEIGHT)
# define INVERT_HSYNC
# define INVERT_VSYNC
#endif

#if defined (CONFIG_LCD_12_1_SVGA_10)
	/* Sharp PN LQ121S1DG41, was LQ121S1DG31 */
#define PANEL_NAME		"LCD 12.1\" SVGA"
/* *** FIXME: this target frequency range isn't achievable with HCLK
   *** at 99993900 Hz. */
# define PEL_CLOCK_EST		(25*1000*1000)     /* MHz/35/40/42 */
# define PEL_CLOCK_DIV		CLOCK_TO_DIV(PEL_CLOCK_EST, HCLK)
# define PEL_CLOCK		(HCLK/PEL_CLOCK_DIV)
# define PEL_WIDTH		(800)
# define PEL_HEIGHT		(600)
# define BIT_DEPTH		(16)
# define BITS_PER_PEL_2		BPP16
# define HORZ_BP		(86)
# define HORZ_FP		(15)
# define VERT_BP		(23)		/* 23 */
# define VERT_FP		(5)
# define HSYNC_WIDTH		(128)		/* clocks/2/128/200 */
# define VSYNC_WIDTH		(1)		/* lines/2/4/6 */
# define INVERT_HSYNC
# define INVERT_VSYNC
#endif

#if defined (CONFIG_LCD_NL2432HC22_40A)
	/* NEC QVGA 2432HC22-40A 240x320 8.9cm*/
# define PANEL_NAME		"NEC LCD 8.9cm Portrait QVGA"
# define PEL_CLOCK_EST		(5000*1000)	/* MHz/?/5/? */
# define PEL_CLOCK_DIV		CLOCK_TO_DIV(PEL_CLOCK_EST, HCLK)
# define PEL_CLOCK		(HCLK/PEL_CLOCK_DIV)
# define PEL_WIDTH		(240)
# define PEL_HEIGHT		(320)
# define BIT_DEPTH		(16)
# define BITS_PER_PEL_2		BPP16
# define HORZ_CYCLE		(256)		/* clock/?/256/? */
# define VERT_CYCLE		(324)		/* line/?/324/? */
# define HORZ_BP		(4)
# define VERT_BP		(1)		/* lines/?*/
# define HSYNC_WIDTH		(8)		/* clocks/?/8/? */
# define VSYNC_WIDTH		(2)		/* lines/?/1/? */
# define HORZ_FP		(HORZ_CYCLE-HORZ_BP-HSYNC_WIDTH-PEL_WIDTH)
# define VERT_FP		(VERT_CYCLE-VERT_BP-VSYNC_WIDTH-PEL_HEIGHT)
# define INVERT_HSYNC
# define INVERT_VSYNC
#endif

#if defined (CONFIG_LCD_LQ036Q1DA01) && 0
	/* Sharp QVGA LQ036Q1DA01 320x240 9.1cm w/ASIC */
/*
  clcd:   buffer 0xc0300000  red 5<<0  green 5<<5  blue 5<<10
          ctrl 0x10829  clk 4.166MHz [ Sharp LCD Landscape QVGA w/ASIC ]
          timing0 0x140a114c  timing1 0x040404ef  timing2 0x013f3016
          hpels 320 hbp 21 hsw 18 hfp 11 ioe 0 ipc 1 ihs 1 ivs 0
          vpels 240 vbp 4 vsw 2 vfp 4
*/

#define PANEL_NAME		"Sharp LCD Landscape QVGA w/ASIC"

#define PANEL_TIMING0		(0x140a114c)
#define PANEL_TIMING1		(0x040404ef)
#define PANEL_TIMING2		(0x013f3016)
#define PANEL_CONTROL		(0x00010929)

#define PANEL_ALI_SETUP		(0x33fd)
#define PANEL_ALI_CONTROL	(0x0003)
#define PANEL_ALI_TIMING1	(0x082d)
#define PANEL_ALI_TIMING2	(0x420d)

#endif

#if defined (CONFIG_LCD_LQ036Q1DA01)
	/* Sharp QVGA LQ036Q1DA01 320x240 */
/*
  clcd:   buffer 0xc0300000  red 5<<0  green 5<<5  blue 5<<10
          ctrl 0x10028  clk 6.666MHz [ Sharp LCD Landscape 9.1cm QVGA w/ASIC ]
          timing0 0x03090d4c  timing1 0x030308ef  timing2 0x013f000d
          hpels 320 hbp 4 hsw 14 hfp 10 ioe 0 ipc 0 ihs 0 ivs 0
          vpels 240 vbp 3 vsw 3 vfp 3
*/

#define PANEL_NAME		"Sharp LCD Landscape 9.1cm QVGA w/ASIC"

#define PANEL_TIMING0		(0x03090d4c)
#define PANEL_TIMING1		(0x030308ef)
#define PANEL_TIMING2		(0x013f000d)
#define PANEL_CONTROL		(0x00010829)

#define PANEL_ALI_SETUP		(0x33f0)
#define PANEL_ALI_CONTROL	(0x0000)
#define PANEL_ALI_TIMING1	(0x0000)
#define PANEL_ALI_TIMING2	(0x0000)

#endif

#if defined (CONFIG_LCD_LQ035Q7DH06)
	/* Sharp QVGA LQ035Q7DH06 240x320 */
#define PANEL_NAME		"Sharp LCD Portrait QVGA w/ASIC"

#define PANEL_TIMING0		(0x480f203c)
#define PANEL_TIMING1		(0x080f153f)
#define PANEL_TIMING2		(0x271f1806)
#define PANEL_CONTROL		(0x00010829)

#define PANEL_ALI_SETUP		(0x27f0)
#define PANEL_ALI_CONTROL	(0x0000)
#define PANEL_ALI_TIMING1	(0x0000)
#define PANEL_ALI_TIMING2	(0x0000)

#endif

#if defined (CONFIG_LCD_TD035TTEA1)
	/* One Stop Display QVGA TD035TTEA1 320x240 */
# define PANEL_NAME		"OSD LCD Landscape QVGA"
//# define PEL_CLOCK_EST		(8650*1000)     /* MHz/4/8/8.65 */
# define PEL_CLOCK_EST		(8000*1000)     /* MHz/4/8/8.65 */
//# define PEL_CLOCK_EST		(8650*1000)     /* MHz/4/8/8.65 */
# define PEL_CLOCK_DIV		CLOCK_TO_DIV(PEL_CLOCK_EST, HCLK)
# define PEL_CLOCK		(HCLK/PEL_CLOCK_DIV)
# define PEL_WIDTH		(320)
# define PEL_HEIGHT		(240)
# define BIT_DEPTH		(16)
# define BITS_PER_PEL_2		BPP16
# define HORZ_CYCLE		(440)		/* clock/326/440/472 */
# define VERT_CYCLE		(264)		/* line/246/264/282 */
# define HSYNC_WIDTH		(38)		/* clocks/2/38/256 */
# define VSYNC_WIDTH		(8)		/* lines/2/8/14 */
# define HORZ_BP		(40)		/* clocks/2/40/256 */
# define VERT_BP		(8)		/* lines/2/8/14 */
# define HORZ_FP		(HORZ_CYCLE-HORZ_BP-HSYNC_WIDTH-PEL_WIDTH)
# define VERT_FP		(VERT_CYCLE-VERT_BP-VSYNC_WIDTH-PEL_HEIGHT)
# define INVERT_HSYNC
# define INVERT_VSYNC
# define INVERT_PIXEL_CLOCK
//# define INVERT_OE
#endif

#if defined (CONFIG_LCD_NOKIA_WIDE)
	/* Wide NOKIA Display */
# define PANEL_NAME		"Nokia Wide Landscape HVGA"
# define PEL_CLOCK_EST		(8*1000*1000)
# define PEL_CLOCK_DIV		CLOCK_TO_DIV(PEL_CLOCK_EST, HCLK)
# define PEL_CLOCK		(HCLK/PEL_CLOCK_DIV)
# define PEL_WIDTH		(640)
# define PEL_HEIGHT		(200)
# define BIT_DEPTH		(16)
# define BITS_PER_PEL_2		BPP16
# define HORZ_CYCLE		(640+32+62+48)
# define VERT_CYCLE		(200+1+1+1)
# define HSYNC_WIDTH		(32)
# define VSYNC_WIDTH		(1)
# define HORZ_BP		(62)
# define VERT_BP		(1)
# define HORZ_FP		(HORZ_CYCLE-HORZ_BP-HSYNC_WIDTH-PEL_WIDTH)
# define VERT_FP		(VERT_CYCLE-VERT_BP-VSYNC_WIDTH-PEL_HEIGHT)
# define INVERT_HSYNC
# define INVERT_VSYNC
# define INVERT_PIXEL_CLOCK
//# define INVERT_OE
#endif

	/* Inverse timing calculations for peculiar Sharp panels
	   without complete datasheets. */
#if defined (PANEL_TIMING0)

#define PEL_CLOCK_DIV		((PANEL_TIMING2 & 0x1f) + 2)
#define PEL_WIDTH		(((PANEL_TIMING2 >> 16) & 0x3ff) + 1)
#define PEL_HEIGHT		((PANEL_TIMING1 & 0x1ff) + 1)
#define HORZ_BP			(((PANEL_TIMING0 >> 24) & 0xff) + 1)
#define HORZ_FP			(((PANEL_TIMING0 >> 16) & 0xff) + 1)
#define VERT_BP			((PANEL_TIMING1 >> 24) & 0xff)
#define VERT_FP			((PANEL_TIMING1 >> 16) & 0xff)

#define HSYNC_WIDTH		(((PANEL_TIMING0 >> 8) & 0x7f) + 1)
#define VSYNC_WIDTH		(((PANEL_TIMING1 >> 10) & 0x3f) + 1)

#define P_INVERT_OE		(PANEL_TIMING2  & (1<<14))
#define P_INVERT_PIXEL_CLOCK	(PANEL_TIMING2  & (1<<13))
#define P_INVERT_HSYNC		(PANEL_TIMING2  & (1<<12))
#define P_INVERT_VSYNC		(PANEL_TIMING2  & (1<<11))

#define PEL_CLOCK_EST		PEL_CLOCK_DIV
#define PEL_CLOCK		(HCLK/PEL_CLOCK_DIV)
#define BIT_DEPTH		16
#define BITS_PER_PEL_2		BPP16

# if P_INVERT_PIXEL_CLOCK
#  define INVERT_PIXEL_CLOCK
# endif

# if P_INVERT_OE
#  define INVERT_OE
# endif

# if P_INVERT_HSYNC
#  define INVERT_HSYNC
# endif

# if P_INVERT_VSYNC
#  define INVERT_VSYNC
# endif

#endif


#define HBP(v)	((((v) - 1) & 0xff)<<24)
#define HFP(v)	((((v) - 1) & 0xff)<<16)
#define HSW(v)	((((v) - 1) & 0xff)<<8)
#define PPL(v)	((((v)/16 - 1) & 0x7f)<<2)

#define VBP(v)	(((v) & 0xff)<<24)
#define VFP(v)	(((v) & 0xff)<<16)
#define VSW(v)	((((v) - 1) & 0x3f)<<10)
#define LPP(v)	(((v) - 1) & 0x3ff)

#define BCD	(1<<26)		/* Bypass pixel clock divider */
#define CPL	((PEL_WIDTH - 1) & 0x3ff)<<16
#define IOE	(1<<14)
#define IPC	(1<<13)
#define IHS	(1<<12)
#define IVS	(1<<11)
#define PCD(v)	(((v) - 2) & 0x1f)
#define WATERMARK (1<<16)
#define PWR	(1<<11)
#define BEPO	(1<<10)
#define BEBO	(1<<9)
#define BGR	(1<<8)
#define DUAL	(1<<7)
#define TFT	(1<<5)
#define BW	(1<<4)
#define LCDEN	(1<<0)

	// Decode macros
#define D_PEL_CLOCK_DIV		((CLCDC_TIMING2 & 0x1f) + 2)
#define D_PEL_WIDTH		(((CLCDC_TIMING2 >> 16) & 0x3ff) + 1)
#define D_PEL_HEIGHT		((CLCDC_TIMING1 & 0x1ff) + 1)
#define D_HBP			(((CLCDC_TIMING0 >> 24) & 0xff) + 1)
#define D_HFP			(((CLCDC_TIMING0 >> 16) & 0xff) + 1)
#define D_VBP			((CLCDC_TIMING1 >> 24) & 0xff)
#define D_VFP			((CLCDC_TIMING1 >> 16) & 0xff)

#define D_HSW			(((CLCDC_TIMING0 >> 8) & 0x7f) + 1)
#define D_VSW			(((CLCDC_TIMING1 >> 10) & 0x3f) + 1)

#define D_IOE			(CLCDC_TIMING2  & (1<<14))
#define D_IPC			(CLCDC_TIMING2  & (1<<13))
#define D_IHS			(CLCDC_TIMING2  & (1<<12))
#define D_IVS			(CLCDC_TIMING2  & (1<<11))


#define CB_BUFFER ((PEL_WIDTH*PEL_HEIGHT*BIT_DEPTH)/8)

/* The component shifters are an optimzation for the splash decoder.
   We don't want to shift twice for every component, so we aggregate
   the shifts and include the operator.  This make a big difference in
   the ARM assembler. */

# define RED_WIDTH		5
# define BLUE_WIDTH		5

# define RED_MASK		(0xf8)
# define BLUE_MASK		(0xf8)

#if defined (CONFIG_LCD_565)
# define GREEN_WIDTH		6
# define GREEN_MASK		(0xfc)
#else
# define GREEN_WIDTH		5
# define GREEN_MASK		(0xf8)
#endif

#if defined (CONFIG_LCD_BGR)

# define BLUE_SHIFT		0
# define GREEN_SHIFT		(BLUE_SHIFT + BLUE_WIDTH)
# define RED_SHIFT		(GREEN_SHIFT + GREEN_WIDTH)

# define BLUE_SHIFT_COMP	>>3
# define GREEN_SHIFT_COMP	<<(GREEN_SHIFT-(8-GREEN_WIDTH))

# define RED_SHIFT_COMP		<<(RED_SHIFT-(8-RED_WIDTH))

#else

# define RED_SHIFT		0
# define GREEN_SHIFT		(RED_SHIFT + RED_WIDTH)
# define BLUE_SHIFT		(GREEN_SHIFT + GREEN_WIDTH)

# define RED_SHIFT_COMP		>>3
# define GREEN_SHIFT_COMP	<<(GREEN_SHIFT-(8-GREEN_WIDTH))

# define BLUE_SHIFT_COMP	<<(BLUE_SHIFT-(8-BLUE_WIDTH))

#endif

unsigned short* buffer;


/* *** FIXME: need to sort out msleep so that we properly cope with
   *** the inline implementation. */

/* _msleep

   only works after the timer has been initialized.

*/

static void _msleep (int ms)
{
  unsigned long time = timer_read ();

  do {
  } while (timer_delta (time, timer_read ()) < ms);
}

/* cannot do scaling without __divsi3 */
//#define I(c,i) ((c)*(i)/255)
#define I(c,i) (c)

#define RGB(r,g,b) ( (((r) & RED_MASK)   RED_SHIFT_COMP)\
		    |(((g) & GREEN_MASK) GREEN_SHIFT_COMP)\
		    |(((b) & BLUE_MASK)  BLUE_SHIFT_COMP))


#define RGBI(r,g,b,i) ( (((r) & RED_MASK)   RED_SHIFT_COMP)\
		       |(((g) & GREEN_MASK) GREEN_SHIFT_COMP)\
		       |(((b) & BLUE_MASK)  BLUE_SHIFT_COMP)\
		       |(((i) & 1) << 15))

static void clcdc_init (void)
{
#if defined (CONFIG_LCD_RETAIN_SPLASH)
  /* The alignment here has to do with the kernel.  It didn't like
     receiving a block of memory that wasn't MB boundary aligned.  Not
     sure why and I don't care for now. */
  buffer = alloc_uncached_top_retain (CB_BUFFER, 1024*1024);
#else
  buffer = alloc_uncached (CB_BUFFER, 1024*1024);
#endif

#if defined (USE_FILL)

  {
    int i;
    for (i = 0; i < PEL_WIDTH*PEL_HEIGHT; ++i) {
      if (i >= 3*(PEL_WIDTH*PEL_HEIGHT)/4)
	buffer[i] = 0xefff;
      else if (i >= 2*(PEL_WIDTH*PEL_HEIGHT)/4)
	buffer[i] = RGB(0,0,(i%PEL_WIDTH)*255/PEL_WIDTH);
      else if (i >= 1*(PEL_WIDTH*PEL_HEIGHT)/4)
	buffer[i] = RGB(0,(i%PEL_WIDTH)*255/PEL_WIDTH,0);
      else if (i >= 0*(PEL_WIDTH*PEL_HEIGHT)/4)
	buffer[i] = RGB((i%PEL_WIDTH)*255/PEL_WIDTH,0,0);
    }
  }

#if defined (USE_BORDER)

  {
    int row;
    //    printf ("rgb value %x\n", RGB (0x80, 0x80, 0x80));
    for (row = 0; row < PEL_HEIGHT; ++row) {
      buffer[row*PEL_WIDTH] = RGB (0x80, 0x80, 0x80);
      buffer[row*PEL_WIDTH + 1] = 0;
      buffer[row*PEL_WIDTH + PEL_WIDTH - 1] = RGB (0x80, 0x80, 0x80);
      buffer[row*PEL_WIDTH + PEL_WIDTH - 2] = 0;
      //      buffer[row*PEL_WIDTH] = 0xffff;
    }
  }

#endif
#endif

//  printf ("lcd setup\n");
  DRV_CLCDC_SETUP;

  CLCDC_TIMING0 = HBP (HORZ_BP) | HFP (HORZ_FP) | HSW (HSYNC_WIDTH)
    | PPL (PEL_WIDTH);
  CLCDC_TIMING1 = VBP (VERT_BP) | VFP (VERT_FP) | VSW (VSYNC_WIDTH)
    | LPP (PEL_HEIGHT);
  CLCDC_TIMING2   = CPL
#if defined (INVERT_OE)
    | IOE
#endif
#if defined (INVERT_PIXEL_CLOCK)
    | IPC
#endif
#if defined (INVERT_HSYNC)
    | IHS
#endif
#if defined (INVERT_VSYNC)
    | IVS
#endif
    | PCD (PEL_CLOCK_DIV);

  CLCDC_UPBASE    = (unsigned long) buffer;
  CLCDC_CTRL      = WATERMARK | TFT | BITS_PER_PEL_2
#if defined (CONFIG_LCD_BGR)
    | BGR
#endif
    ;

#if defined (CONFIG_ARCH_LH7952X)
  ALI_SETUP	  = 0x00000efd;
#endif

#if defined (CONFIG_LCD_3_5_QVGA_20)

# if defined (CONFIG_ARCH_LH79520)
  ALI_CTRL	  = 0x00000003; /* SPS & CLS */
# endif
# if defined (CONFIG_ARCH_LH79524) || defined (CONFIG_ARCH_LH79525)
  ALI_CTRL	  = 0x00000013;	/* VEEEN & SPS & CLS*/
# endif

# if defined (CONFIG_ARCH_LH7952X)
  ALI_TIMING1     = 0x0000087d;
  ALI_TIMING2     = 0x00009ad0;
# endif

# if defined (CONFIG_ARCH_LH7A400)
#  if defined (PANEL_ALI_SETUP)
  HRTFTC_SETUP   = PANEL_ALI_SETUP;
  HRTFTC_CON     = PANEL_ALI_CONTROL;
  HRTFTC_TIMING1 = PANEL_ALI_TIMING1;
  HRTFTC_TIMING2 = PANEL_ALI_TIMING2;
#  else
  HRTFTC_SETUP   = 0x00002efd;
  HRTFTC_CON     = 0x00000003;
  HRTFTC_TIMING1 = 0x0000087d;
  HRTFTC_TIMING2 = 0x00009ad0;
#  endif
# endif

# if defined (CONFIG_ARCH_LH7A404)
#  if defined (PANEL_ALI_SETUP)
  ALI_SETUP   = PANEL_ALI_SETUP;
  ALI_CONTROL = PANEL_ALI_CONTROL;
  ALI_TIMING1 = PANEL_ALI_TIMING1;
  ALI_TIMING2 = PANEL_ALI_TIMING2;
#  else
  ALI_SETUP   = 0x00002efd;
  ALI_CONTROL = 0x00000003;
  ALI_TIMING1 = 0x0000087d;
  ALI_TIMING2 = 0x00009ad0;
#  endif
# endif

#endif

}

static void clcdc_release (void)
{
#if !defined (CONFIG_LCD_RETAIN_SPLASH)
  /* *** This sequence, as well as that which is in the initialization
     function, should be better documented. */

  DRV_CLCDC_SLEEP;

  DRV_CLCDC_BACKLIGHT_DISABLE;

  CLCDC_CTRL &= ~PWR;		/* Remove power */
  _msleep (20);			/* Wait 20ms */
  CLCDC_CTRL &= ~LCDEN;		/* Disable CLCDC controller output */
  DRV_CLCDC_DISABLE;

  DRV_CLCDC_POWER_DISABLE;

  /* Clobber the configuration registers just so we know that the
     kernel is configuring the controller. */
  CLCDC_TIMING0 = CLCDC_TIMING1 = CLCDC_TIMING2 = 0;
  CLCDC_UPBASE    = 0;

  /* Make sure not to access the CLCDC controller after
     DRV_CLCDC_RELEASE as the driver is likely to disable clocks
     making the registers unavailable.  */
  DRV_CLCDC_RELEASE;
#endif
}

#if !defined (CONFIG_SMALL)
static void clcdc_report (void)
{
  unsigned long clk = HCLK/((CLCDC_TIMING2 & 0x1f) + 2);
  printf ("  clcd:   buffer 0x%p  red %d<<%d  green %d<<%d  blue %d<<%d\n",
	  buffer,
	  RED_WIDTH, RED_SHIFT,
	  GREEN_WIDTH, GREEN_SHIFT,
	  BLUE_WIDTH,  BLUE_SHIFT);
  printf ("          ctrl 0x%lx", CLCDC_CTRL);
  if (clk < 1000000)
    printf ("  clk %ldHz", clk);
  else
    printf ("  clk %ld.%03ldMHz",
	    clk/(1000*1000), (clk/1000)%1000);
  printf (" [ " PANEL_NAME " ]\n");
  printf ("          timing0 0x%08lx  timing1 0x%08lx  timing2 0x%08lx\n",
	  CLCDC_TIMING0, CLCDC_TIMING1, CLCDC_TIMING2);
  printf ("          hpels %ld hbp %ld hsw %ld hfp %ld "
	  "ioe %d ipc %d ihs %d ivs %d\n",
	  D_PEL_WIDTH, D_HBP, D_HSW, D_HFP,
	  D_IOE != 0, D_IPC != 0, D_IHS != 0, D_IVS != 0);
  printf ("          vpels %ld vbp %ld vsw %ld vfp %ld\n",
	  D_PEL_HEIGHT, D_VBP, D_VSW, D_VFP);
}
#endif

static __service_7 struct service_d lh7_clcdc_service = {
  .init    = clcdc_init,
  .release = clcdc_release,
#if !defined (CONFIG_SMALL)
  .report = clcdc_report,
#endif
};


#if defined (CONFIG_CMD_CLCDC_SPLASH)

int cmd_splash (const char* region)
{
  int result;
  struct descriptor_d d;
  void* pv;
  struct png_header hdr;
  unsigned short* ps = buffer;
  int cPalette = 0;
  unsigned char* rgbPalette;
  int i;
  int j;

  if (   (result = parse_descriptor (region, &d))
      || (result = open_descriptor (&d))) {
    printf ("Unable to open source %s\n", region);
    goto fail_early;
  }

  pv = open_pngr (&d);
  if (pv == NULL)
    goto fail_early;

  if (read_pngr_ihdr (pv, &hdr))
    goto fail_close;

  if (hdr.bit_depth != 8)
    goto fail_close;


  memset (buffer, 0, CB_BUFFER);

  {
    for (i = hdr.height; i--; ps += PEL_WIDTH - hdr.width) {
      const unsigned char* pb = read_pngr_row (pv);
      if (pb == NULL) {
	printf ("%s: read failed at row %d\n", __FUNCTION__, i);
	goto fail_close;
      }

      switch (hdr.color_type) {

      case 2:		/* RGB */
	for (j = 0; j < hdr.width; ++j, ++ps)
	  *ps = ((pb[j*3    ] & RED_MASK)   RED_SHIFT_COMP)
	    +   ((pb[j*3 + 1] & GREEN_MASK) GREEN_SHIFT_COMP)
	    +   ((pb[j*3 + 2] & BLUE_MASK)  BLUE_SHIFT_COMP)
	    ;
	break;

      case 3:		/* Palettized */
	if (!cPalette)
	  cPalette = palette_pngr (pv, &rgbPalette);
	switch (hdr.bit_depth) {
	case 1:
	case 2:
	case 4:
	  break;		/* unimplemented depths */
	case 8:
	  for (j = 0; j < hdr.width; ++j, ++ps) {
	    unsigned char* color = &rgbPalette[pb[j]*3];
	    *ps = ((color[0] & RED_MASK)   RED_SHIFT_COMP)
	      +   ((color[1] & GREEN_MASK) GREEN_SHIFT_COMP)
	      +   ((color[2] & BLUE_MASK)  BLUE_SHIFT_COMP)
	    ;
	  }
	}
	break;

      case 6:		/* RGBA */
	for (j = 0; j < hdr.width; ++j, ++ps)
	  *ps = ((pb[j*4    ] & RED_MASK)   RED_SHIFT_COMP)
	    +   ((pb[j*4 + 1] & GREEN_MASK) GREEN_SHIFT_COMP)
	    +   ((pb[j*4 + 2] & BLUE_MASK)  BLUE_SHIFT_COMP);
	break;
      }
    }
  }

 fail_close:
  close_pngr (pv);

 fail_early:
  close_descriptor (&d);

  return result;
}

#endif


int cmd_clcdc (int argc, const char** argv)
{
  if (argc == 1) {
    printf ("clcdc: initialized %s at 0x%p\n", PANEL_NAME, buffer);
    printf ("  clk %d/%d->%d (%d)  sync (%d %d)  porch (%d %d %d %d)\n",
	    HCLK, PEL_CLOCK_DIV, PEL_CLOCK, PEL_CLOCK_EST,
	    HSYNC_WIDTH, VSYNC_WIDTH,
	    HORZ_BP, HORZ_FP, VERT_BP, VERT_FP);
    return 0;
  }

#if defined (CONFIG_CMD_CLCDC_SPLASH)
  if (PARTIAL_MATCH (argv[1], "sp", "lash") == 0) {
    if (argc != 3)
      return ERROR_PARAM;
    return cmd_splash (argv[2]);
  }
#endif

#if defined (CONFIG_CMD_CLCDC_TEST)
  if (PARTIAL_MATCH (argv[1], "ba", "rs") == 0) {
    int i;
    for (i = 0; i < PEL_HEIGHT*PEL_WIDTH; ++i) {
      if (i < PEL_WIDTH
	  || i%PEL_WIDTH == 0
	  || i%PEL_WIDTH == PEL_WIDTH - 1
	  || i > (PEL_HEIGHT - 1)*PEL_WIDTH)
	buffer[i] = 0xffff;
      else if (i < PEL_WIDTH*2
	  || i%PEL_WIDTH == 1
	  || i%PEL_WIDTH == PEL_WIDTH - 2
	  || i > (PEL_HEIGHT - 2)*PEL_WIDTH)
	buffer[i] = 0x0000;
      else if (i > 3*(PEL_HEIGHT*PEL_WIDTH)/4)
	buffer[i] = 0xffff;
      else if (i > 2*(PEL_HEIGHT*PEL_WIDTH)/4)
	buffer[i] = I(0x1f,(i%PEL_WIDTH)*255/255)<<BLUE_SHIFT;
      else if (i > 1*(PEL_HEIGHT*PEL_WIDTH)/4)
	/* *** FIXME: this is wrong because we need to make the width
	   of green greater in 565 mode. */
	buffer[i] = I(0x1f,(i%PEL_WIDTH)*255/255)<<GREEN_SHIFT;
      else if (i > 0*(PEL_HEIGHT*PEL_WIDTH)/4)
	buffer[i] = I(0x1f,(i%PEL_WIDTH)*255/255)<<RED_SHIFT;
    }
    return 0;
  }
  if (PARTIAL_MATCH (argv[1], "w", "hite") == 0) {
    memset (buffer, 0xff, PEL_HEIGHT*PEL_WIDTH*(BIT_DEPTH/8));
    return 0;
  }
  if (PARTIAL_MATCH (argv[1], "bla", "ck") == 0) {
    memset (buffer, 0, PEL_HEIGHT*PEL_WIDTH*(BIT_DEPTH/8));
    return 0;
  }
# if (BIT_DEPTH==16)
  if (PARTIAL_MATCH (argv[1], "r", "ed") == 0) {
    int i;
    for (i = 0; i < PEL_HEIGHT*PEL_WIDTH; ++i)
      buffer[i] = (0xff & RED_MASK) RED_SHIFT_COMP;
    return 0;
  }
  if (PARTIAL_MATCH (argv[1], "g", "reen") == 0) {
    int i;
    for (i = 0; i < PEL_HEIGHT*PEL_WIDTH; ++i)
      buffer[i] = (0xff & GREEN_MASK) GREEN_SHIFT_COMP;
    return 0;
  }
  if (PARTIAL_MATCH (argv[1], "blu", "e") == 0) {
    int i;
    for (i = 0; i < PEL_HEIGHT*PEL_WIDTH; ++i)
      buffer[i] = (0xff & BLUE_MASK) BLUE_SHIFT_COMP;
    return 0;
  }
# endif
#endif

  if (PARTIAL_MATCH (argv[1], "on", "") == 0) {
    printf ("lcd on\n");
    CLCDC_CTRL      |= LCDEN;	/* Enable CLCDC */
    DRV_CLCDC_POWER_ENABLE;
    _msleep (20);		/* Wait 20ms for digital signals  */
    CLCDC_CTRL      |= PWR;	/* Apply power */

    _msleep (20);		/* *** Hack */

    DRV_CLCDC_WAKE;

    /* *** FIXME: this value is calculable based on the LCD controller
       parameters and the frame size. 40ms is the time for two frame
       displays at 800x600 with a 25MHz pixel clock.  100ms was the
       shortest time found that gave a solid image when the backlight
       came on (with above parameters). */
    _msleep (100);		/* Wait for the display image to settle */
    DRV_CLCDC_BACKLIGHT_ENABLE;
    return 0;
  }

  if (PARTIAL_MATCH (argv[1], "of", "f") == 0) {
    DRV_CLCDC_BACKLIGHT_DISABLE;

    DRV_CLCDC_SLEEP;

    CLCDC_CTRL &= ~PWR;		/* Remove power */
    _msleep (20);		/* Wait 20ms */
    CLCDC_CTRL &= ~LCDEN;	/* Disable CLCDC controller */
    DRV_CLCDC_DISABLE;

    DRV_CLCDC_POWER_DISABLE;
    return 0;
  }

  return ERROR_PARAM;
}

static __command struct command_d c_clcdc = {
  .command = "clcdc",
  .func = cmd_clcdc,
  COMMAND_DESCRIPTION ("commands for controlling the color LCD controller")
  COMMAND_HELP(
"clcdc [SUBCOMMAND [PARAMETER]]\n"
" Without a SUBCOMMAND, it displays the LCD setup parameters.\n"
" on         - enable the controller and LCD backlight\n"
" off        - disable the controller and LCD backlight\n"
#if defined (CONFIG_CMD_CLCDC_TEST)
" bars       - draw color bars into frame buffer\n"
" white      - draw white into frame buffer\n"
" black      - draw black into frame buffer\n"
# if BIT_DEPTH==16
" red        - draw red into frame buffer\n"
" green      - draw green into frame buffer\n"
" blue       - draw blue into frame buffer\n"
# endif
#endif
#if defined (CONFIG_CMD_CLCDC_SPLASH)
" splash SRC - draw splash image (PNG) into frame buffer\n"
#endif
"\n"
"  e.g.  clcdc on\n"
#if defined (CONFIG_CMD_CLCDC_SPLASH)
"        clcdc splash fat://1/logo.png\n"
#endif
  )
};
