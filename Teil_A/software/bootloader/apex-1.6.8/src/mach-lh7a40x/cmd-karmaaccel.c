/* cmd-karmaaccel.c

   written by Marc Singer
   23 August 2006

   Copyright (C) 2006 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

 Processing:
  Initialize the adc to its default state.
  Map all touchscreen pins to ADC input
  Set up the idle step to prevent false pen auto-triggering.
  Flush the result FIFO.
  Clear the pen and end of sequence interrupts.
  Set up the sequence as follows:
      0. Strong pullup on X+ pin, other pins float.
	 Measure on pin x+. This measurement will be discarded.
	 This step de-glitches any partial triggers.
      1. Weak pullup on X+ pin, X- floats, Y- to Ground and neg
	 ref, Y+ floats (ratiometric).
	 Measure on X+ pin. This measurement is used to verify
	 the pen was down at the beginning of the coordinate
	 measurement.
      2. X+ and X- float, Y- to Ground and neg
	 ref, Y+ to VDDA_ADC and positive reference (ratiometric).
	 Measure on pin x+. This is the X measurement.
      3. Repeat the X measurement 3 more times (we will average
	 4 measurements to improve signal-to-noise.
      4. Y+ and Y- float, X- to Ground and neg
	 ref, X+ to VDDA_ADC and positive reference (ratiometric).
	 Measure on pin Y+. This is the Y measurement.
      5. Repeat the Y measurement 3 more times (we will average
	 4 measurements to improve signal-to-noise.
      6. Strong pullup on X+ pin, other pins float.
	 Measure on pin x+. This measurement will be discarded.
	 This step de-glitches any partial triggers.
      7. Weak pullup on X+ pin, X- floats, Y- to Ground and neg
	 ref, Y+ floats (ratiometric).
	 Measure on X+ pin. This measurement is used to verify
	 the pen was down at the end of the coordinate
	 measurement.


*/

#include <config.h>
#include <driver.h>
#include <service.h>
#include <linux/string.h>
#include <apex.h>
#include <command.h>
#include <mach/hardware.h>
#include <console.h>


//#define TALK 1

#if defined (TALK)
#define PRINTF(f...)		printf (f)
#define ENTRY(l)		printf ("%s\n", __FUNCTION__)
#define DBG(l,f...)		do { if (TALK >= l) printf (f); } while (0)
#else
#define PRINTF(f...)		do {} while (0)
#define ENTRY(l)		do {} while (0)
#define DBG(l,f...)		do {} while (0)
#endif

#define ADC_FS_FFF		(1<<3)
#define ADC_FS_FEMPTY		(1<<2)

#define ADC_IC_BOI		(1<<2)
#define ADC_IC_PEN		(1<<1)
#define ADC_IC_EOS		(1<<0)

#define ADC_IS_BROWNOUT		(1<<4)
#define ADC_IS_PEN		(1<<3)
#define ADC_IS_EOS		(1<<2)

#define ADC_PC_CLKSEL_SHIFT	(8)
#define ADC_PC_CLKSEL_MASK	(7<<ADC_PC_CLKSEL_SHIFT)
#define ADC_PC_REFEN		(1<<5)
#define ADC_PC_BATEN		(1<<4)
#define ADC_PC_NOC_SHIFT	(0)
#define ADC_PC_NOC_MASK		(0xf)
#define ADC_PC_PWM_MASK		(3<<6)
#define ADC_PC_PWM_OFF		(0<<6)
#define ADC_PC_PWM_STANDBY	(1<<6)
#define ADC_PC_PWM_RUN		(2<<6)

#define ADC_HW_SETTIME_SHIFT	(7)
#define ADC_HW_SETTIME_MASK	(0x1ff<<ADC_HW_SETTIME_SHIFT)
#define ADC_HW_INP_SHIFT	(3)
#define ADC_HW_INP_MASK		(0xf<<ADC_HW_INP_SHIFT)
#define ADC_HW_INP_AN0		(0<<3)		/* UL/X+ */
#define ADC_HW_INP_AN1		(1<<3)		/* UR/X- */
#define ADC_HW_INP_AN2		(2<<3)		/* LL/Y+ */
#define ADC_HW_INP_AN3		(3<<3)		/* LR/Y- */
#define ADC_HW_INP_AN4		(4<<3)		/* Wiper */
#define ADC_HW_INP_AN5		(5<<3)
#define ADC_HW_INP_AN6		(6<<3)
#define ADC_HW_INP_AN7		(7<<3)
#define ADC_HW_INP_AN8		(8<<3)
#define ADC_HW_INP_AN9		(9<<3)
#define ADC_HW_INP_VREFN	(10<<3)		/* VREF- */
#define ADC_HW_INN_GND		(1<<2)
#define ADC_HW_INN_REFN		(0<<2)		/* VREF- */
#define ADC_HW_REFP_MASK	(3<<0)
#define ADC_HW_REFP_VREFP	(0<<0)		/* VREF+ */
#define ADC_HW_REFP_AN0		(1<<0)		/* UL/X+ */
#define ADC_HW_REFP_AN2		(2<<0)		/* LL/Y+ */
#define ADC_HW_REFP_AN8		(3<<0)
#define ADC_LW_REFN_VREFN	(0<<0)
#define ADC_LW_REFN_AN1		(1<<0)
#define ADC_LW_REFN_AN3		(2<<0)
#define ADC_LW_REFN_AN9		(3<<0)
#define ADC_LW_BIASCON_SHIFT	(2)
#define ADC_LW_BIASCON_MASK	(0xfff<<2)

#define ADC_LW_FET_AN0_VDD	(1<<2)
#define ADC_LW_FET_AN0_VDD100K	(1<<12)
#define ADC_LW_FET_AN1_VDD	(1<<3)
#define ADC_LW_FET_AN1_GND	(1<<4)
#define ADC_LW_FET_AN2_VDD	(1<<5)
#define ADC_LW_FET_AN2_GND	(1<<6)
#define ADC_LW_FET_AN3_GND	(1<<7)
#define ADC_LW_FET_AN4_VDD100K	(1<<13)
#define ADC_LW_FET_AN4_GND	(1<<8)

#define ADC_GC_SSM_MASK		(3<<0)
#define ADC_GC_SSM_SSB_PEN	(0<<0)
#define ADC_GC_SSM_PEN		(1<<0)
#define ADC_GC_SSM_SSB		(2<<0)
#define ADC_GC_SSM_CONT		(3<<0)
#define ADC_GC_SSB		(1<<2)
#define ADC_GC_FIFOWMK_SHIFT	(3)
#define ADC_GC_FIFOWMK_MASK	(0xf<<ADC_GC_FIFOWMK_SHIFT)

# define US_SETTLING		(20)		/* Signal stablization */

	/* We're aiming for sampling at about 400KHz */

#if defined (CONFIG_ARCH_LH79524) || defined (CONFIG_ARCH_LH79525)
# define ADC_CLKSEL		(0x7)
# define CLKSEL			(1<<ADC_CLKSEL)
# define RCPC_ADCPRE_V		(1)
# define A2DCLK			(50803200/RCPC_ADCPRE_V/CLKSEL)
#endif

#if defined (CONFIG_ARCH_LH7A404)
# define ADC_CLKSEL		(0x2)
# define CLKSEL			(1<<ADC_CLKSEL)
# define A2DCLK			(1843200/CLKSEL)
#endif

#define ADC_PC_CLKSEL_V		(ADC_CLKSEL<<ADC_PC_CLKSEL_SHIFT)

#define SETTIME(us)		(((A2DCLK*(us) + 1000000/2)/1000000)\
				 <<ADC_HW_SETTIME_SHIFT)

#define SAMPLES			 (12)

static void accel_setup (void)
{
  int i;

  ENTRY (0);

  PRINTF ("adc: A2DCLK %d  SETTIME(US) %0xx\n", A2DCLK, SETTIME(US_SETTLING));

  ADC_IC = ADC_IC_BOI | ADC_IC_PEN | ADC_IC_EOS;

				/* Flush the results FIFO */
  while ((ADC_FS & ADC_FS_FEMPTY) == 0)
    ADC_RR;

  MASK_AND_SET (ADC_PC,
		ADC_PC_NOC_MASK | ADC_PC_PWM_MASK,
		((SAMPLES - 1)<<ADC_PC_NOC_SHIFT)|(ADC_PC_PWM_OFF)
		| ADC_PC_REFEN
		);

  MASK_AND_SET (ADC_GC,
		ADC_GC_SSM_MASK | ADC_GC_FIFOWMK_MASK,
//		ADC_GC_SSM_SSB
		ADC_GC_SSM_SSB_PEN
		//		| (12<<ADC_GC_FIFOWMK_SHIFT)
		);

//  ADC_PC |= ADC_PC_REFEN;

#define _HI(p)	(SETTIME (US_SETTLING) | (p) | ADC_HW_INN_REFN)
#define _LO(f)	(f)

	/* Idle */
  ADC_IHWCTRL = _HI (ADC_HW_INP_AN9);
  ADC_ILWCTRL = _LO (ADC_LW_REFN_AN9);	/* No bias control necessary */

	/* Sampling */
  for (i = 0; i < 16; ++i) {
    switch (i) {
    case 0:
    case 2:
    case 4:
    case 6:
    case 8:
    case 10:
    case 12:
    case 14:
      __REG(ADC_HWC_BASE_PHYS + i*4) = _HI (ADC_HW_INP_AN0 | ADC_HW_REFP_AN8);
      __REG(ADC_LWC_BASE_PHYS + i*4) = _LO (ADC_LW_REFN_AN9);
      break;
    case 1:
    case 3:
    case 5:
    case 7:
    case 9:
    case 11:
    case 13:
    case 15:
      __REG(ADC_HWC_BASE_PHYS + i*4) = _HI (ADC_HW_INP_AN1 | ADC_HW_REFP_AN8);
      __REG(ADC_LWC_BASE_PHYS + i*4) = _LO (ADC_LW_REFN_AN9);
      break;
    default:
      __REG(ADC_HWC_BASE_PHYS + i*4) = 0;
      __REG(ADC_LWC_BASE_PHYS + i*4) = 0;
      break;
    }
  }

  MASK_AND_SET (ADC_PC, ADC_PC_PWM_MASK, ADC_PC_PWM_STANDBY);
}

static void accel_init (void)
{
  int i;

  ENTRY (0);

  ADC_IM = 0; /* Disable all interrupts */

  ADC_PC = ADC_PC_CLKSEL_V | ADC_PC_PWM_OFF | (1<<ADC_PC_NOC_SHIFT);
  ADC_GC =
    ADC_GC_SSM_SSB
    | (0xf<<ADC_GC_FIFOWMK_SHIFT);


  for (i = 0; i < SAMPLES; ++i) {
    __REG(ADC_HWC_BASE_PHYS + i*4) = (0x1ff << ADC_HW_SETTIME_SHIFT)
      | (i << ADC_HW_INP_SHIFT) | ADC_HW_INN_GND | ADC_HW_REFP_VREFP;
    __REG(ADC_LWC_BASE_PHYS + i*4) = ADC_LW_REFN_VREFN;
  }

  ADC_IHWCTRL = (0x1ff << ADC_HW_SETTIME_SHIFT)
    | ADC_HW_INP_AN0 | ADC_HW_INN_GND | ADC_HW_REFP_VREFP;
  ADC_ILWCTRL = ADC_LW_REFN_VREFN;

				/* Flush the results FIFO */
  while ((ADC_FS & ADC_FS_FEMPTY) == 0)
    ADC_RR;

  ADC_IC = ADC_IC_BOI | ADC_IC_PEN | ADC_IC_EOS;

  accel_setup ();
}

#define MS_TIMEOUT 2*1000
#define MS_ADC_DELAY 600

#define REF	     ((1023*33)/10)

static int cmd_accel (int argc, const char** argv)
{
  unsigned long status;
  int i;
  unsigned long timeStart;
  int x = 0;
  int y = 0;

  while (1) {
//    printf ("\n\n\n\n\n");

    status = ADC_GS;
				/* Flush the results FIFO */
    while ((ADC_FS & ADC_FS_FEMPTY) == 0)
      ADC_RR;

    PRINTF ("[%4lx %4lx] ", ADC_FS, status);
//    printf ("start %lx\n", status);

    ADC_IC |= ADC_IC_EOS | ADC_IC_PEN;
    ADC_GC |= (1<<2);		/* Start sampling */

    timeStart = timer_read ();

    do {
      if (ADC_IS & ADC_IS_EOS)
	break;
#if 0
      unsigned long l = ADC_GS;
      if ((l & (0xf<<4)) != (status & (0xf<<4)))
	printf ("state %lx\n", (l >> 4) & 0xf);
      status = l;
      if (((status >> 4) & 0xf) == 0x4) /* End of sequence */
	break;
      if (((status >> 4) & 0xf) == 0x1) /* Idle, which is really an error */
	break;
#endif
    } while (timer_delta (timeStart, timer_read ()) < MS_TIMEOUT);

    PRINTF ("-> [gs %lx %lx]\n", ADC_GS, ADC_FS);
//    printf ("fifo %lx\n", ADC_FS);

    for (i = 0; i < SAMPLES; ++i) {
      unsigned long v = ADC_RR;
      if (i == 0)
	x = v>>6;
      if (i == 1)
	y = v>>6;
//      printf ("%2d rr %5ld (%lx) - (%lx)\n", i, (v >> 6), (v & 0xf), ADC_FS);
      PRINTF (" %4ld", (v >> 6));
    }

    PRINTF ("\n");

    printf ("x %d.%d (%d)  y %d.%d (%d)\n",
	    33*x/(1023*10),
	    (33*x/1023)%10,
	    x,
	    33*y/(1023*10),
	    (33*y/1023)%10,
	    y);


    PRINTF ("fifo %4lx ", ADC_FS);
    PRINTF ("hw 0x%4lx  lw 0x%4lx\n", ADC_HW, ADC_LW);

    timeStart = timer_read ();
    while (timer_delta (timeStart, timer_read ()) < MS_ADC_DELAY)
      if (console->poll (0, 0))
	goto done;
//    printf ("\n\n\n\n\n");
  }

 done:

  return 0;
}


static __command struct command_d c_accel = {
  .command = "accel",
  .description = "test accelerometer",
  .func = cmd_accel,
};

static __service_7 struct service_d karma_accel_service = {
  .init = accel_init,
};
