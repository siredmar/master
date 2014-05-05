/* exception_vectors.c

   written by Marc Singer
   19 Nov 2008

   Copyright (C) 2008 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   Exception vectors for MX31 must be implemented with the MMU because
   there is no way to map SDRAM or flash at physical address 0.

*/

#include <config.h>
#include <apex.h>
#include <asm/bootstrap.h>
#include <asm/system.h>
#include <service.h>
#include "hardware.h"
#include <asm/interrupts.h>
#include <asm/mmu.h>
#include <asm/string.h>
#include <asm/cp15.h>
#include <debug_ll.h>

extern void reset (void);
extern void irq_handler (void);

irq_return_t (*interrupt_handlers[32])(int irq);

void __naked __section (.vector.0) exception_vectors (void)
{
  __asm ("ldr pc, =reset_handler\n\t"		/* reset */
	 "ldr pc, =undef_handler\n\t"		/* undefined instruction */
	 "ldr pc, =exception_error_handler\n\t"	/* software interrupt (SWI) */
	 "ldr pc, =prefetch_handler\n\t"	/* prefetch abort */
	 "ldr pc, =abort_handler\n\t"		/* data abort */
	 "ldr pc, =exception_error_handler\n\t"	/* (reserved) */
	 "ldr pc, =irq_handler\n\t"		/* irq (interrupt) */
	 "ldr pc, =exception_error_handler\n\t"	/* fiq (fast interrupt) */
	 );
}

void __naked reset_handler (void)
{
  PUTC_LL ('x');
  PUTC_LL ('R');

  while (1)
    ;
}

void __naked  undef_handler (void)
{
  unsigned long addr;

  PUTC_LL ('x');
  PUTC_LL ('U');
  PUTC_LL (':');

  __asm volatile ("mov r14, %0" : "=r" (addr));
  PUTHEX_LL (addr);

  while (1)
    ;
}

void __naked prefetch_handler (void)
{
  unsigned long addr;

  PUTC_LL ('x');
  PUTC_LL ('P');
  PUTC_LL (':');

  __asm volatile ("mov r14, %0" : "=r" (addr));
  PUTHEX_LL (addr);

  while (1)
    ;
}

void __naked abort_handler (void)
{
  unsigned long addr;
//  unsigned long far;
//  unsigned long fsr;

  PUTC_LL ('x');
  PUTC_LL ('A');
  PUTC_LL (':');

  __asm volatile ("mov r14, %0" : "=r" (addr));
  PUTHEX_LL (addr);

//  __asm volatile ("mrc p15, 0, %0, c5, c0" : "=r" (fsr));
//  __asm volatile ("mrc p15, 0, %0, c6, c0" : "=r" (far));
//  PUTC_LL (':');
//  PUTHEX_LL (fsr);
//  PUTC_LL (':');
//  PUTHEX_LL (far);

  while (1)
    ;
}

void __naked exception_error_handler (void)
{
  PUTC_LL ('x');
  PUTC_LL ('E');

	/* *** FIXME: this might be a good place to reset the system */
  while (1)
    ;
}

void __irq_handler irq_handler (void)
{
  while (1)
    ;
}

static void mx31_exception_init (void)
{
#if defined (CONFIG_MMU)
  {
    unsigned long v;
    __asm volatile ("mrc p15, 0, %0, c1, c0, 0\n\t"
		    "orr %0, %0, #(1<<13)\n\t"	      /* High vectors */
		    "mcr p15, 0, %0, c1, c0, 0" : "=&r" (v));
    CP15_WAIT;
  }

  {
    void* pv = alloc_uncached (1<<20, 1<<20);
    memcpy (pv + 0x000f0000, &APEX_VMA_VECTOR_START,
            &APEX_VMA_VECTOR_END - &APEX_VMA_VECTOR_START);
	/* Remap our vectors to where the CPU will find them  */
    mmu_map_segment (pv, (void*) 0xfff00000, 1, 1);
  }

#if 0
	/* Install vectors.
	   I didn't use memcpy because the target address is 0--memcpy
	   fails to copy to 0xffff0000. */
  {
    unsigned long v = 0xffff0000;
    unsigned long t;
    void* p = &APEX_VMA_VECTOR_START;
    __asm volatile (
		 "0: ldmia %1!, {%2}\n\t"
		    "stmia %0!, {%2}\n\t"
		    "cmp %1, %3\n\t"
		    "bne 0b\n\t"
		    : "+r" (v), "+r" (p), "=&r" (t)
		    : "r" (&APEX_VMA_VECTOR_END)
		    : "cc"
		  );
  }
#endif

#if defined (CONFIG_INTERRUPTS)
		/* Initialize interrupt stack */
  {
    unsigned long cpsr_save;
    unsigned long cpsr;
    __asm volatile ("mrs %0, cpsr\n\t"
		    "bic %1, %0, #0b11111\n\t"
		    "orr %1, %1, #0b10010\n\t"
		    "msr cpsr_c, %1\n\t"
		    "mov sp, %2\n\t"
		    "msr cpsr_c, %0\n\t"
		    : "=&r" (cpsr_save), "=&r" (cpsr)
		    : "r" (&APEX_VMA_IRQSTACK_START)
		    );
  }
#endif

//  local_irq_enable ();
#endif
}

static void mx31_exception_release (void)
{
//  local_irq_disable ();
}

static void mx31_exception_report (void)
{
}

/* Must follow MMU service */
static __service_2 struct service_d mx31_exception_service = {
  .init    = mx31_exception_init,
  .release = mx31_exception_release,
  .report  = mx31_exception_report,
};
