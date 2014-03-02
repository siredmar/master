/* initialize.c

   written by Marc Singer
   22 Jan 2007

   Copyright (C) 2007 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   Hardware initializations.  Some initialization may be left to
   drivers, such as the serial interface initialization.

   o Size.  The setup for the MX31 is substantially more verbose than
     other platforms we've handled.  With OneNAND boot, there is only
     1K of code available before we can relocate to SDRAM.  The
     solution has been complex.  It has involved restructuring of the
     sections and breaking the relocation into two phases.

   o UART initialization.  Again, because of the large size of MX31
     initialization code, we cannot fit the UART initialization into
     the .bootstrap_pre section and have enough room for the early
     relocation to fit in 512 bytes (the amount of code available from
     OneNAND when we come out of reset).  So, we leave it out for now.
     We could do some sort of low-speed setup of the CPU clock speed
     and *maybe* get the UART clock to work correctly, but this is all
     to serve a very small window of UART output in the early
     relocation.  Blah.

   o According to Micron, Mobile DDR SDRAM initialization requires:

     1. To prevent device latch-up, the core power (VDD) and I/O power
     	(VDDQ) must be brought up simultaneously. It is recommended
     	that VDD and VDDQ be from the same power source or VDDQ must
     	never exceed VDD. Assert and hold CKE HIGH.
     2. Once power supply voltages are stable and the CKE has been
     	driven HIGH, it is safe to apply the clock.
     3. Once the clock is stable, a 200μs (minimum) delay is required
     	by the Mobile DDR SDRAM prior to applying an executable
     	command. During this time, NOP or DESELECT commands must be
     	issued on the command bus.
     4. Issue a PRECHARGE ALL command.
     5. Issue NOP or DESELECT commands for at least tRP time.
     6. Issue an AUTO REFRESH command followed by NOP or DESELECT
     	commands for at least tRFC time. Issue a second AUTO REFRESH
     	command followed by NOP or DESELECT commands for at least tRFC
     	time. As part of the initialization sequence, two AUTO REFRESH
     	commands must be issued. Typically, both of these commands are
     	issued at this stage as described above. Alternately, the
     	second AUTO REFRESH command and NOP or DESELECT sequence can
     	be issued after step 10.
     7. Using the LOAD MODE REGISTER command, load the standard mode
     	register as desired.
     8. Issue NOP or DESELECT commands for at least tMRD time.
     9. Using the LOAD MODE REGISTER command, load the extended mode
     	register to the desired operating modes. Note that the
     	sequence in which the standard and extended mode registers are
     	programmed is not critical.
    10. Issue NOP or DESELECT commands for at least tMRD time.

  o Memory size detection.  We support 512Mib (V2) and 1Gib (V1)
    configurations.  We determine which we have by initializing the
    1Gib mode first and checking that writes to a particular address
    are not being reflected in the a different address.  In our case,
    the difference is 0x800 bytes.  If the addressing is OK, a write
    to 0x80000800 will not appear at address 0x80000000.

  o Refresh timer.  The value of the refresh timer is based on the
    number of rows.  A 13 row part must refresh 8192 rows.  To do this
    in 64ms (typical), the timer should be 7.81us.

  o PRECHARGE and MODE SET commands.  Normally, the address sent to
    the SDRAM would be shifted to account for the width of the memory
    devices.  However, The PRECHARGE and MODE SET commands disable
    this shifting.  Thus, to drive A10 the address bit use an address
    or'd with 0x0400.  The mode register value need not be shifted.

  o Extended Mode.  Setting the extended mode requires setting the
    BA[1:0] bits to binary 10.  The placement of these bits in the
    address register is dependendent on the organization of the memory
    array.  Our best estimate, at the moment, is that the table 19-30
    of the reference manual gives the correct offset.  It is hard to
    tell of this is really the case for setting the mode because the
    secont 19.4.3 states that no address multiplexing occurs while
    setting the mode.  Thus, the bit index could be one or two fewer
    than the value determined from the table.  Note that the
    calculation below is for the configured organization regardless of
    the configuration of the memory chips.

    Calculate the memory size: 2^(rows+cols)*(width/2).

      Memory Size	Address line for BA0
    ----------------------------------------
    8MB   (64Mib)		A21
    16MB  (128Mib)		A22
    32MB  (256Mib)		A23
    64MB  (512Mib)		A24
    128MB (1Gib)		A25
    ----------------------------------------

  o ESDCTL_CFG.  An incorrect value for this register has been known
    to prevent proper operation of the system.  The symptom was that
    IO with SDRAM would sometimes return invalid results.  The CFG
    value doesn't have to be exactly and precisely correct in that the
    values for the MT46H32M32LF (1Gib) work for the MT46H16M32LF
    (512Mib).  Just be sure that the timings are close to correct and
    you should be OK.

*/

#include <config.h>
#include <asm/bootstrap.h>
#include <service.h>
#include <sdramboot.h>
#include <asm/cp15.h>

#include "hardware.h"
#include <debug_ll.h>

#if defined (CONFIG_USES_DM9000)
# include "drv-dm9000.h"
#endif

//#define NO_INIT

#define ESDCTL_CTL0_SDE		(1<<31)	/* Enable */
#define ESDCTL_CTL0_ROW13	(2<<24)	/* 13 rows */
#define ESDCTL_CTL0_COL9	(1<<20)	/* 9 columns */
#define ESDCTL_CTL0_COL10	(2<<20)	/* 10 columns */
#define ESDCTL_CTL0_DSIZ32	(2<<16)	/* 32 bit interface */
#define ESDCTL_CTL0_SREFR7_81	(3<<13)	/* 7.81 us refresh */
#define ESDCTL_CTL0_SREFR3_91	(4<<13)	/* 3.91 us refresh */
#define ESDCTL_CTL0_BL8		(1<<7)	/* burst length of 8 */

#define ESDCTL_CTL0_SREFR_V	ESDCTL_CTL0_SREFR7_81

#define ESDCTL_CTL0_V1 0\
	| ESDCTL_CTL0_SDE\
	| ESDCTL_CTL0_ROW13\
	| ESDCTL_CTL0_COL10\
	| ESDCTL_CTL0_DSIZ32\
	| ESDCTL_CTL0_SREFR_V\
	| ESDCTL_CTL0_BL8

#define ESDCTL_CTL0_V2 0\
	| ESDCTL_CTL0_SDE\
	| ESDCTL_CTL0_ROW13\
	| ESDCTL_CTL0_COL9\
	| ESDCTL_CTL0_DSIZ32\
	| ESDCTL_CTL0_SREFR_V\
	| ESDCTL_CTL0_BL8

#define ADDR_CHECK_0	(0x80000000)
#define ADDR_CHECK_1	(0x80000800)

//#define ESDCTL_CFG_V	(0x0075e73a)
#define ESDCTL_CFG_V	(0x0029672e)	/* 1Gib   MT46H32M32LF-6 */
//#define ESDCTL_CFG_V	(0x0069e728)	/* 512Mib MT46H16M32LF-6 */

#define BA0(r,c,w)	((r) + (c) + (w)/16)
#define BA1(r,c,w)	((r) + (c) + (w)/16 + 1)

void __naked __section (.bootstrap.early) bootstrap_early (void)
{
#if 0
  unsigned long l;
  LOAD_REMAP_PERIPHERAL_PORT (l);
  if (l != (0x40000000 | 0x15))
#endif
    STORE_REMAP_PERIPHERAL_PORT (0x40000000 | 0x15); /* 1GiB @ 1GiB */
}

void __naked __section (.bootstrap.early.exit) bootstrap_early_exit (void)
{
}


/* usleep

   this function accepts a count of microseconds and will wait at
   least that long before returning.  It depends on the timer being
   setup.

   Note that this function is neither __naked nor static.  It is
   available to the rest of the application as is.

   The source for the timer is really the IPG clock and not the high
   clock regardless of the setting for CLKSRC as either HIGH or IPG.
   In both cases, the CPU uses the IPG clock.  The value for IPGCLK
   comes from the main PLL and is available in a constant.  We divide
   down to get a 1us interval.

 */

void __section (.bootstrap.sdram.func) usleep (unsigned long us)
{
  unsigned long mode =
    EPT_CR_CLKSRC_IPG
    | EPT_CR_IOVW
    | ((IPGCLK/1000000 - 1) << EPT_CR_PRESCALE_SH)
    | EPT_CR_DBGEN
    | EPT_CR_SWR
    ;
  __asm volatile ("str %1, [%2, #0]\n\t"	/* EPITCR <- mode w/reset */
		  "orr %1, %1, #1\n\t"		/* |=  EPT_CR_EN */
		  "bic %1, %1, #0x10000\n\t"	/* &= ~EPT_CR_SWR  */
		  "str %1, [%2, #0]\n\t"	/* EPITCR <- mode w/EN */
		  "str %0, [%2, #8]\n\t"	/* EPITLR <- count */
	       "0: ldr %0, [%2, #4]\n\t"	/* EPITSR */
		  "tst %0, #1\n\t"
		  "beq 0b\n\t"
		  : "+r" (us)
		  :  "r" (mode),
		     "r" (PHYS_EPIT1)
		  : "cc");
}


/* bootstrap_pre

   performs mandatory, pre-SDRAM initializations that were not
   performed in the bootstrap_early function, if one even existed.  In
   this case, we wait until now to setup the PLLs.

*/

void __naked __section (.bootstrap.pre) bootstrap_pre (void)
{
  __REG (PHYS_L2CC + 0x0100) = 0; /* Disable L2CC, should be redundant */

  /* This is an apparent work-around for some sort of bug in the IPU
     related to clock setup. */
  __REG (PHYS_IPU + 0x00) = 0x40;			/* Enable DI. */
//  __REG (PHYS_CCM + 0x00) = 0x074B0B7D;

  /* Reset clock controls.  This seems to make APEX behave better when
     we're being executed after the clocks and memory have been
     initialized. *** FIXME: we need to determine exactly what needs
     to be done here so that we can remove cruft. */
//  CCM_COSR  = 0x00000280;
//  CCM_PDR1  = 0x49fcfe7f;

//  CCM_MPCTL = 0x04001800;
//  CCM_PDR0  = 0xff870b48;

#if !defined (NO_INIT)
  barrier ();
  CCM_CCMR  &= ~(1<<3);				/* Disable PLL */
  CCM_CCMR   = 0x074b0bf5;			/* Source CKIH; MCU bypass */
  { int i = 0x1000; while (--i) ; }		/* Delay */
  CCM_CCMR   = 0x074b0bfd;
  CCM_CCMR   = 0x074b0b7d;
//  CCM_CCMR  |=  (1<<3);				/* Enable PLL */
//  CCM_CCMR  &= ~(1<<7);				/* MCU from PLL */
  CCM_PDR0   = CCM_PDR0_V;
  CCM_MPCTL  = CCM_MPCTL_V;
  CCM_PDR1   = 0x49fcfe7f;			/* Default value */
  CCM_UPCTL  = CCM_UPCTL_V;
  CCM_COSR   = CCM_COSR_V;
  barrier ();

  //;WM32  0xb8002050 0x0000dcf6            ; Configure PSRAM on CS5
  //;WM32  0xb8002054 0x444a4541
  //;WM32  0xb8002058 0x44443302
  //;WM32  0xB6000000 0xCAFECAFE

  CCM_CGR0 |= CCM_CGR_RUN << CCM_CGR0_EPIT1_SH;	/* Enable EPIT1 clock  */

  /* NOR flash initialization, though this shouldn't be necessary
     unless we're going to reduce timing latencies.  */
  //  WEIM_UCR(0) = 0x0000CC03; // ; Start 16 bit NorFlash on CS0
  //  WEIM_LCR(0) = 0xa0330D01; //
  //  WEIM_ACR(0) = 0x00220800; //

#endif

#if defined (CONFIG_MACH_MX31ADS)
	/* Configure CPLD on CS4 -- necessary for ADS UART */
  WEIM_UCR(4) = 0x0000DCF6;
  WEIM_LCR(4) = 0x444A4541;
  WEIM_ACR(4) = 0x44443302;
#endif

#if defined (CONFIG_STARTUP_UART)
  INITIALIZE_CONSOLE_UART;
  PUTC ('A');
#endif

#if defined LED_ON
  LED_ON (0);
#endif

  __asm volatile ("b bootstrap_pre_exit");
}

void __naked __section (.bootstrap.pre.exit) bootstrap_pre_exit (void)
{
}


/* bootstrap_sdram

   performs the SDRAM initialization.  This piece of code is skipped
   by the reset() code if it determines that APEX is already running
   in SDRAM.

*/

void __naked __section (.bootstrap.sdram) bootstrap_sdram (void)
{
		/* Initialize IOMUX for SDRAM */
  {
    /* Initialize IOMUX from 0x43fac26c to 0x43fac2dc */
    int i;
    for (i = 0; i < 29; ++i) {
      if (i == (0x43fac27c - 0x43fac26c)/4) /* Skip CS2 */
	continue;
      __REG (0x43fac26c + i*4) = 0;
    }
  }
  __REG (0x43fac27c) = 0x1000; // ; CS2 (CSD0)

	/* SDRAM initialization */
  ESDCTL_CTL0 = 0;
  ESDCTL_MISC = ESDCTL_MISC_MDDREN;	/* BDI setup? Enable DDR */
  ESDCTL_CFG0 = ESDCTL_CFG_V;
  ESDCTL_MISC = ESDCTL_MISC_RST;	/* Reset */
  ESDCTL_MISC = ESDCTL_MISC_MDDREN;	/* Enable DDR */
  usleep (1);			/* > 200ns */
  ESDCTL_CTL0 = 0x92120000;	/* SDE,Precharge,13row,9col,32bit */
  __REG8 (0x80000000
          | 1<<10) = 0;         /* DDR; precharge all */
  ESDCTL_CTL0 = 0xa2120000;	/* SDE,Precharge,13row,9col,32bit */
  __REG (0x80000000) = 0;
  __REG (0x80000000) = 0;
  ESDCTL_CTL0 = 0xb2120000;	/* SDE,LoadMode,13row,9col,32bit */
  __REG8 (0x80000000
          | 0x33) = 0;     	/* Standard Mode <- 0x33; CAS3, BURST8 */
  __REG8 (0x80000000
          | (2<<BA0(13,9,32))
          | 0x00) = 0;          /* Extended Mode <- 0x00 */
  PUTC ('1');
  ESDCTL_CTL0 = ESDCTL_CTL0_V1;
  __REG (0x80000000) = 0;
  /* *** FIXME: we should check for DDR here.  we can test CTL0_V */
  ESDCTL_MISC = ESDCTL_MISC_RST | ESDCTL_MISC_MDDREN;
  __REG (ADDR_CHECK_0 + 0) = 0x55555555;
  __REG (ADDR_CHECK_0 + 4) = 0xaaaaaaaa;
  __REG (ADDR_CHECK_1 + 0) = 0xaaaaaaaa;
  __REG (ADDR_CHECK_1 + 4) = 0x55555555;

//  __asm volatile ("nop\n\tnop");

  /* Try second organization if the first fails  */
  if (   __REG (ADDR_CHECK_0 + 0) != 0x55555555
      || __REG (ADDR_CHECK_0 + 4) != 0xaaaaaaaa
      || 0) {
    PUTC ('2');
    ESDCTL_CTL0 = ESDCTL_CTL0_V2;
    __REG (0x80000000) = 0;
    ESDCTL_MISC = ESDCTL_MISC_RST | ESDCTL_MISC_MDDREN;
//    __REG (ADDR_CHECK_0) = 0x55555555;
//    __REG (ADDR_CHECK_1) = 0xaaaaaaaa;
  }

  __asm volatile ("mov r0, #0");		/* SDRAM initialized */
  __asm volatile ("b bootstrap_sdram_exit");
}

void __naked __section (.bootstrap.sdram.exit) bootstrap_sdram_exit (void)
{
}


/* target_init

   performs the rest of the hardware initialization that didn't have
   to be performed during the bootstrap phase.

*/

static void target_init (void)
{
#if defined (CONFIG_MACH_EXBIBLIO_ROSENCRANTZ)
  IOMUX_PIN_CONFIG_GPIO  (PIN_ILLUMINATION_EN1);
  GPIO_PIN_CONFIG_OUTPUT (PIN_ILLUMINATION_EN1);
  GPIO_PIN_CLEAR	 (PIN_ILLUMINATION_EN1);

  IOMUX_PIN_CONFIG_GPIO  (PIN_ILLUMINATION_EN2);
  GPIO_PIN_CONFIG_OUTPUT (PIN_ILLUMINATION_EN2);
  GPIO_PIN_CLEAR	 (PIN_ILLUMINATION_EN2);

  IOMUX_PIN_CONFIG_GPIO  (PIN_BOARD_ID1);
  GPIO_PIN_CONFIG_INPUT  (PIN_BOARD_ID1);
  IOMUX_PIN_CONFIG_GPIO  (PIN_BOARD_ID2);
  GPIO_PIN_CONFIG_INPUT  (PIN_BOARD_ID2);

#endif

#if !defined (NO_INIT)
  /* Initialize AIPS (AHB to IP bus) */
  AIPS1_MPR1 = 0x77777777;
  AIPS1_MPR2 = 0x77777777;
  AIPS2_MPR1 = 0x77777777;
  AIPS2_MPR2 = 0x77777777;
  AIPS1_OPACR1 = 0;
  AIPS1_OPACR2 = 0;
  AIPS1_OPACR3 = 0;
  AIPS1_OPACR4 = 0;
  AIPS1_OPACR5 &= ~0xff000000;
  AIPS2_OPACR1 = 0;
  AIPS2_OPACR2 = 0;
  AIPS2_OPACR3 = 0;
  AIPS2_OPACR4 = 0;
  AIPS2_OPACR5 &= ~0xff000000;

  /* Initialize MAX (Multi-layer AHB Crossbar Switch) */
  MAX_MPR0 = 0x00302154;
  MAX_MPR1 = 0x00302154;
  MAX_MPR2 = 0x00302154;
  MAX_MPR3 = 0x00302154;
  MAX_MPR4 = 0x00302154;
  MAX_SGPCR0 = 0x10;
  MAX_SGPCR1 = 0x10;
  MAX_SGPCR2 = 0x10;
  MAX_SGPCR3 = 0x10;
  MAX_SGPCR4 = 0x10;
  MAX_MGPCR0 = 0;
  MAX_MGPCR1 = 0;
  MAX_MGPCR2 = 0;
  MAX_MGPCR3 = 0;
  MAX_MGPCR4 = 0;

  /* Initialize M3IF (Multi-Master Memory Interface) */
  M3IF_CTL = (1<<M3IF_M_IPU1);
#endif

#if defined (CONFIG_USES_DM9000)

	/* Configure the DM9000 for Asynchronous level sensitive DTACK mode */
  WEIM_UCR(DM_WEIM_CS) = 0
    | (3<<14)                   /* CNC, AHB clock width minimum CS pulse */
    | (3<<8)                    /* WSC */
    | (1<<7)                    /* EW, DTACK level sensitive */
    | (2<<4)                    /* WWS, two extra clocks for write setup */
	/* DM9000 needs 2 clocks @20ns each to recover from a transaction */
    | (6<<0)                    /* EDC, 6 cycles between transactions */
    ;

  WEIM_LCR(DM_WEIM_CS) = 0
	/* DM9000 needs a slight delay within CS for nOE */
    | (4<<28)                   /* OEA, 1/2 AHB clock delay til assert OE */
    | (4<<24)                   /* OEN, 1/2 AHB clock delay til deassert OE */
    | (5<<8)                    /* DSZ, 16 bit bus width */
    | (1<<0)                    /* CSEN, enable CS */
    ;

  WEIM_ACR(DM_WEIM_CS) = 0
	/* DM9000 needs a slight delay within CS for R_Wn */
	/* Karma needs a delay so that the address, which passes
           through a buffer, is ready before the R_Wn signal is
           asserted.  Rosencrantz doesn't need this, but it doesn't
           really hurt. */
    | (4<<20)                   /* RWA, 1/2 AHB clock delay til assert RW */
    | (4<<16)                   /* RWN, 1/2 AHB clock delay til deassert RW */
    | (3<<4)                    /* DCT, AHB clock delay til nDTACK check */
    ;
#endif
}


static void target_release (void)
{
}

static __service_0 struct service_d mx3x_target_service = {
  .init    = target_init,
  .release = target_release,
};


#if 0

;; 0x10 MISC
;;  WM32  0xB8001010 0x00000004		; Initialization script for 32 bit DDR on Tortola EVB
;; 0x04 CFG0
;;  WM32  0xB8001004 0x006ac73a
;; 0x00 CTL0
;;  WM32  0xB8001000 0x92100000
;;  WM32  0x80000f00 0x12344321
;;  WM32  0xB8001000 0xa2100000
;;  WM32  0x80000000 0x12344321
;;  WM32  0x80000000 0x12344321
;;  WM32  0xB8001000 0xb2100000
;;  WM8   0x80000033 0xda
;;  WM8   0x81000000 0xff
;;  WM32  0xB8001000 0x82226080
;;  WM32  0x80000000 0xDEADBEEF
;;  WM32  0xB8001010 0x0000000c
;;  WGPR  15         0x83F00000		; boot secret

+    setup_sdram ddr X32 DDR 0

+SDRAM_PARAM1_DDR:          .word       0x4
+SDRAM_PARAM1_SDR:          .word       0x0
+SDRAM_PARAM2_DDR:          .word       0x80000F00
+SDRAM_PARAM2_SDR:          .word       0x80000400
+SDRAM_PARAM3_DDR:       .word   0x00100000
+SDRAM_PARAM3_SDR:       .word   0x0
+SDRAM_PARAM4_X32:       .word   0x00010000
+SDRAM_PARAM4_X16:       .word   0x0
+SDRAM_0x55555555:       .word   0x55555555
+SDRAM_0xAAAAAAAA:       .word   0xAAAAAAAA
+SDRAM_0x92100000:       .word   0x92100000
+SDRAM_0xA2100000:       .word   0xA2100000
+SDRAM_0xB2100000:       .word   0xB2100000
+SDRAM_0x82116080:       .word   0x82116080
+SDRAM_0x0075E73A:       .word   0x0075E73A
+#define SDRAM_FULL_PAGE_BIT     0x100
+#define SDRAM_FULL_PAGE_MODE    0x37
+#define SDRAM_BURST_MODE        0x33


+    .macro setup_sdram, name, bus_width, mode, full_page
+        /* It sets the "Z" flag in the CPSR at the end of the macro */
+        ldr r0, ESDCTL_BASE_W
+        mov r2, #SDRAM_BASE_ADDR
+        ldr r1, SDRAM_0x0075E73A
+        str r1, [r0, #0x4]
+        ldr r1, =0x2            // reset
+        str r1, [r0, #0x10]
+        ldr r1, SDRAM_PARAM1_\mode
+        str r1, [r0, #0x10]
+        // Hold for more than 200ns
+        ldr r1, =0x10000
+1:
+        subs r1, r1, #0x1
+        bne 1b
+
+        ldr r1, SDRAM_0x92100000
+        str r1, [r0]
+        ldr r1, =0x0
+        ldr r12, SDRAM_PARAM2_\mode
+        str r1, [r12]
+        ldr r1, SDRAM_0xA2100000
+        str r1, [r0]
+        ldr r1, =0x0
+        str r1, [r2]
+        ldr r1, SDRAM_0xB2100000
+        str r1, [r0]
+
+        ldr r1, =0x0
+        .if \full_page
+        strb r1, [r2, #SDRAM_FULL_PAGE_MODE]
+        .else
+        strb r1, [r2, #SDRAM_BURST_MODE]
+        .endif
+
+        ldr r1, =0xFF
+        ldr r12, =0x81000000
+        strb r1, [r12]
+        ldr r3, SDRAM_0x82116080
+        ldr r4, SDRAM_PARAM3_\mode
+        add r3, r3, r4
+        ldr r4, SDRAM_PARAM4_\bus_width
+        add r3, r3, r4
+        .if \full_page
+        add r3, r3, #0x100   /* Force to full page mode */
+        .endif
+
+        str r3, [r0]
+        ldr r1, =0x0
+        str r1, [r2]
+        /* Below only for DDR */
+        ldr r1, [r0, #0x10]
+        ands r1, r1, #0x4
+        ldrne r1, =0x0000000C
+        strne r1, [r0, #0x10]
+        /* Testing if it is truly DDR */
+        ldr r1, SDRAM_0x55555555
+        ldr r0, =SDRAM_BASE_ADDR
+        str r1, [r0]
+        ldr r2, SDRAM_0xAAAAAAAA
+        str r2, [r0, #0x4]
+        ldr r2, [r0]
+        cmp r1, r2
+    .endm


#endif
