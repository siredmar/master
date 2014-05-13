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

  /* Note that PE4 must be driven high on the LPD7A404 to prevent the
     CPLD JTAG chain from crashing the board.  See the target_init ()
     code. */

#if defined (CONFIG_MACH_LPD7A40X)
# define DRV_CLCDC_SETUP\
	({ GPIO_PINMUX |= (1<<1) | (1<<0); }) /* LCDVD[15:4] */
#endif

#if defined (CONFIG_MACH_COMPANION)
# define DRV_CLCDC_SETUP\
	({ extern void companion_clcdc_setup (void); \
	   GPIO_PINMUX |= (1<<1) | (1<<0);\
	    companion_clcdc_setup (); })

# define DRV_CLCDC_WAKE\
	({ extern void companion_clcdc_wake (void);\
	   companion_clcdc_wake (); })

# define DRV_CLCDC_SLEEP\
	({ extern void companion_clcdc_sleep (void);\
	   companion_clcdc_sleep (); })

#endif

#if defined (CONFIG_MACH_KARMA)
# define DRV_CLCDC_SETUP\
	({ extern void karma_clcdc_setup (void); \
	   GPIO_PINMUX |= (1<<1) | (1<<0);\
	    karma_clcdc_setup (); })

# define DRV_CLCDC_WAKE\
	({ extern void karma_clcdc_wake (void);\
	   karma_clcdc_wake (); })

# define DRV_CLCDC_SLEEP\
	({ extern void karma_clcdc_sleep (void);\
	   karma_clcdc_sleep (); })

#endif

#if defined (CONFIG_MACH_LPD7A400)
# define DRV_CLCDC_BACKLIGHT_ENABLE\
	({ CPLD_CONTROL |= CPLD_CONTROL_LCD_VEEEN; })
# define DRV_CLCDC_BACKLIGHT_DISABLE\
	({ CPLD_CONTROL &= ~CPLD_CONTROL_LCD_VEEEN; })
#endif

#if defined (CONFIG_MACH_LPD7A404)
# define DRV_CLCDC_BACKLIGHT_ENABLE\
	({ GPIO_PCDD &= ~(1<<3); GPIO_PCD  |= (1<<3); })
# define DRV_CLCDC_BACKLIGHT_DISABLE\
	({ GPIO_PCD  &= ~(1<<3); })
#endif

#if defined (CONFIG_MACH_COMPANION)

# define BL_FREQ	(5*1000)	/* Hz */
# define BL_DUTY	30		/* percentage*100 */
# define PWM_TC3_V	((14745600/BL_FREQ) - 1)
# define PWM_DC3_V	((BL_DUTY*(PWM_TC3_V + 1)/100) - 1)

# define DRV_CLCDC_BACKLIGHT_ENABLE\
	({ PWM_TC3 = PWM_TC3_V; PWM_DC3 = PWM_DC3_V; \
	   PWM_INV3 = 0; PWM_EN3 = 1; })
# define DRV_CLCDC_BACKLIGHT_DISABLE\
	({ PWM_EN3 = 0; })
#endif

#if defined (CONFIG_MACH_KARMA)

# define BL_FREQ	(12*1000)	/* Hz */
# define BL_DUTY	60		/* percentage*100 */
# define PWM_TC3_V	((14745600/BL_FREQ) - 1)
# define PWM_DC3_V	((BL_DUTY*(PWM_TC3_V + 1)/100) - 1)

# define DRV_CLCDC_BACKLIGHT_ENABLE\
	({ PWM_TC3 = PWM_TC3_V; PWM_DC3 = PWM_DC3_V; \
	   PWM_INV3 = 0; PWM_EN3 = 1; })
# define DRV_CLCDC_BACKLIGHT_DISABLE\
	({ PWM_EN3 = 0; })
#endif

#if defined (CONFIG_ARCH_LH7A400)
# define DRV_CLCDC_DISABLE\
	({ HRTFTC_SETUP &= ~(1<<13); }) /* Disable HRTFT controller */
#endif

#if defined (CONFIG_ARCH_LH7A404)
# define DRV_CLCDC_DISABLE\
	({ ALI_SETUP &= ~(1<<13); }) /* Disable ALI */
#endif

#endif  /* __MACH_DRV_CLCDC_H__ */
