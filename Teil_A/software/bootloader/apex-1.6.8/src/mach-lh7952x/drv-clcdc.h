/* drv-clcdc.h

   written by Marc Singer
   24 Dec 2005

   Copyright (C) 2005 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__MACH_DRV_CLCDC_H__)
#    define   __MACH_DRV_CLCDC_H__

/* ----- Includes */

/* ----- Types */

/* ----- Globals */

/* ----- Prototypes */

#if defined (CONFIG_ARCH_LH79520)

# define DRV_CLCDC_SETUP_CLOCKS\
	({ RCPC_PERIPHCLKCTRL2 &= ~(1<<0);\
	   RCPC_PERIPHCLKSEL2 &= ~(1<<0); /* Use HCLK */ \
	   RCPC_LCDCLKPRESCALE = (1>>1); /* HCLK/1 */ \
	 })
# define DRV_CLCDC_RELEASE_CLOCKS\
	({ RCPC_PERIPHCLKCTRL2 |= (1<<0); })

# define DRV_CLCDC_SETUP_MUX\
	({ IOCON_LCDMUX |= (1<<28) | (1<<27) | (1<<26) | (1<<25) | (1<<24)\
			 | (1<<21) | (1<<20) | (1<<19) | (1<<18) | (1<<15)\
			 | (1<<12) | (1<<11) | (1<<10) | (1<<3) | (1<<2);\
			 /*  IOCON_LCDMUX = 0x1fbeda9e; */\
	   MASK_AND_SET (IOCON_LCDMUX, 3<<22, 2<<22); /* All for HR-TFT */\
	   MASK_AND_SET (IOCON_LCDMUX, 3<<16, 2<<16);\
	   MASK_AND_SET (IOCON_LCDMUX, 3<<13, 2<<13);\
	   MASK_AND_SET (IOCON_LCDMUX, 3<<8,  2<<8);\
	   MASK_AND_SET (IOCON_LCDMUX, 3<<6,  2<<6);\
	   MASK_AND_SET (IOCON_LCDMUX, 3<<4,  0<<4);\
	   MASK_AND_SET (IOCON_LCDMUX, 3<<0,  2<<0);\
	 })

# endif

#if defined (CONFIG_ARCH_LH79524) || defined (CONFIG_ARCH_LH79525)

# define DRV_CLCDC_SETUP_CLOCKS\
	({ RCPC_PCLKCTRL1  &= ~(1<<0);\
	   RCPC_AHBCLKCTRL &= ~(1<<4);\
	   /*  RCPC_LCDPRE = (8>>1);*/	/* HCLK divisor 8 */\
	   RCPC_LCDPRE      =  (1>>1);	/* HCLK divisor 1 */\
	 })
# define DRV_CLCDC_RELEASE_CLOCKS\
	({ RCPC_PCLKCTRL1  |=   1<<0;\
	   RCPC_AHBCLKCTRL |=   1<<4; })

# define DRV_CLCDC_SETUP_MUX\
	({ MASK_AND_SET (IOCON_MUXCTL1,\
			 (3<<2)|(3<<0),\
			 (1<<2)|(1<<0));\
	   MASK_AND_SET (IOCON_RESCTL1,\
			 (3<<2)|(3<<0),\
			 (0<<2)|(0<<0));\
	   MASK_AND_SET (IOCON_MUXCTL19,\
			 (3<<12)|(3<<8)|(3<<4)|(3<<2),\
			 /* LCDVEEN, LCDVDDEN, LCDREV, LCDCLS */\
			 (1<<12)|(1<<8)|(2<<4)|(1<<2));\
	   MASK_AND_SET (IOCON_RESCTL19,\
			 (3<<12)|(3<<8)|(3<<4)|(3<<2),\
			 (0<<12)|(0<<8)|(0<<4)|(0<<2));\
	   MASK_AND_SET (IOCON_MUXCTL20,\
			 (3<<14)|(3<<10)|(3<<6)|(3<<2)|(3<<0),\
			 /* LCDPS, LCDDCLK, LCDHRLP, LCDSPS, LCDSPL */\
			 (1<<14)|(1<<10)|(2<<6)|(2<<2)|(2<<0));\
	   MASK_AND_SET (IOCON_RESCTL20,\
			 (3<<14)|(3<<10)|(3<<6)|(3<<2)|(3<<0),\
			 (0<<14)|(0<<10)|(0<<6)|(0<<2)|(0<<0));\
/* *** this needs to be different for the LH79525 */
/*
		MASK_AND_SET (IOCON_MUXCTL21,
			      (3<<10)|(3<<6)|(3<<2)|(3<<0),
			      (1<<10)|(1<<6)|(1<<2)|(1<<0));
		MASK_AND_SET (IOCON_RESCTL21,
			      (3<<10)|(3<<6)|(3<<2)|(3<<0),
			      (1<<10)|(1<<6)|(1<<2)|(1<<0));
*/
	   MASK_AND_SET (IOCON_MUXCTL21,\
			 (3<<10)|(3<<8)|(3<<6)|(3<<4)|(3<<2)|(3<<0),\
			 (1<<10)|(1<<8)|(1<<6)|(1<<4)|(1<<2)|(1<<0));\
	   MASK_AND_SET (IOCON_RESCTL21,\
			 (3<<10)|(3<<8)|(3<<6)|(3<<4)|(3<<2)|(3<<0),\
			 (1<<10)|(1<<8)|(1<<6)|(1<<4)|(1<<2)|(1<<0));\
	   MASK_AND_SET (IOCON_MUXCTL22,\
			 (3<<14)|(3<<12)|(3<<10)|(3<<8)\
			 |(3<<6)|(3<<4)|(3<<2)|(3<<0),\
			 (1<<14)|(1<<12)|(1<<10)|(1<<8)\
			 |(1<<6)|(1<<4)|(1<<2)|(1<<0));\
	   MASK_AND_SET (IOCON_RESCTL22,\
			 (3<<14)|(3<<12)|(3<<10)|(3<<8)\
			 |(3<<6)|(3<<4)|(3<<2)|(3<<0),\
			 (0<<14)|(0<<12)|(0<<10)|(0<<8)\
			 |(0<<6)|(0<<4)|(0<<2)|(0<<0));\
	 })

#endif


#define DRV_CLCDC_SETUP\
	({ RCPC_CTRL |= (1<<9); /* Unlock */\
	   DRV_CLCDC_SETUP_CLOCKS;\
	   RCPC_CTRL &= ~(1<<9); /* Lock */\
	   DRV_CLCDC_SETUP_MUX;\
	 })

#define DRV_CLCDC_RELEASE\
	({ RCPC_CTRL |= (1<<9); /* Unlock */\
	   DRV_CLCDC_RELEASE_CLOCKS;\
	   RCPC_CTRL &= ~(1<<9); /* Lock */\
	 })

#if defined (CPLD_CTRL1_LCD_POWER_EN)
# define DRV_CLCDC_POWER_ENABLE\
	({ MASK_AND_SET (CPLD_CTRL1,\
			 CPLD_CTRL1_LCD_POWER_EN | CPLD_CTRL1_LCD_OE,\
			 CPLD_CTRL1_LCD_POWER_EN); })
# define DRV_CLCDC_POWER_DISABLE\
	({ MASK_AND_SET (CPLD_CTRL1,\
			 CPLD_CTRL1_LCD_POWER_EN | CPLD_CTRL1_LCD_OE,\
			 CPLD_CTRL1_LCD_OE); })
#endif

#if defined (CPLD_CTRL1_LCD_BACKLIGHT_EN)
# define DRV_CLCDC_BACKLIGHT_ENABLE\
  ({ CPLD_CTRL1 |= CPLD_CTRL1_LCD_BACKLIGHT_EN; })
# define DRV_CLCDC_BACKLIGHT_DISABLE\
  ({ CPLD_CTRL1 &= ~CPLD_CTRL1_LCD_BACKLIGHT_EN; })
#endif

#endif  /* __MACH_DRV_CLCDC_H__ */
