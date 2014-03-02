/***********************************************************************
 * $Id:: lpc_irq_fiq.h 2285 2009-10-02 16:31:28Z pdurgesh              $
 *
 * Project: ARM core interrupt functions
 *
 * Description:
 *     This file contains the function declarations for routines for
 *     enabling, disabling and restoring the state of IRQ and FIQ
 *     exceptions.
 *
 ***********************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
 **********************************************************************/

#ifndef LPC_IRQ_FIQ_H
#define LPC_IRQ_FIQ_H

#ifdef __GNUC__
static __inline UNS_32 disable_irq(void)
{
  register UNS_32 ret, tmp;

  __asm__ volatile
  (
    "MRS %0, CPSR" "\n\t"
    "ORR %1, %0, #0x80" "\n\t"
    "MSR CPSR_c, %1" "\n\t"
  : "=r"(ret), "=r"(tmp)
  );

  return ret;
}

static __inline UNS_32 disable_fiq(void)
{
  register UNS_32 ret, tmp;

  __asm__ volatile
  (
    "MRS %0, CPSR" "\n\t"
    "ORR %1, %0, #0x40" "\n\t"
    "MSR CPSR_c, %1" "\n\t"
  : "=r"(ret), "=r"(tmp)
  );

  return ret;
}

static __inline UNS_32 disable_irq_fiq(void)
{
  register UNS_32 ret, tmp;

  __asm__ volatile
  (
    "MRS %0, CPSR" "\n\t"
    "ORR %1, %0, #0xC0" "\n\t"
    "MSR CPSR_c, %1" "\n\t"
  : "=r"(ret), "=r"(tmp)
  );

  return ret;
}

static __inline UNS_32 enable_irq(void)
{
  register UNS_32 ret, tmp;

  __asm__ volatile
  (
    "MRS %0, CPSR" "\n\t"
    "BIC %1, %0, #0x80" "\n\t"
    "MSR CPSR_c, %1" "\n\t"
  : "=r"(ret), "=r"(tmp)
  );

  return ret;
}

static __inline UNS_32 enable_fiq(void)
{
  register UNS_32 ret, tmp;

  __asm__ volatile
  (
    "MRS %0, CPSR" "\n\t"
    "BIC %1, %0, #0x40" "\n\t"
    "MSR CPSR_c, %1" "\n\t"
  : "=r"(ret), "=r"(tmp)
  );

  return ret;
}

static __inline UNS_32 enable_irq_fiq(void)
{
  register UNS_32 ret, tmp;

  __asm__ volatile
  (
    "MRS %0, CPSR" "\n\t"
    "BIC %1, %0, #0xC0" "\n\t"
    "MSR CPSR_c, %1" "\n\t"
  : "=r"(ret), "=r"(tmp)
  );

  return ret;
}

static __inline UNS_32 disable_irq_fiq_mask(UNS_32 mask)
{
  register UNS_32 ret, tmp;

  __asm__ volatile
  (
    "BIC %1, %2, #0x2F" "\n\t"
    "MRS %0, CPSR" "\n\t"
    "ORR %1, %0, %1" "\n\t"
    "MSR CPSR_c, %1" "\n\t"
  : "=r"(ret), "=r"(tmp)
        : "r"(mask)
      );

  return ret;
}

static __inline UNS_32 enable_irq_fiq_mask(UNS_32 mask)
{
  register UNS_32 ret, tmp;

  __asm__ volatile
  (
    "BIC %1, %2, #0x2F" "\n\t"
    "MRS %0, CPSR" "\n\t"
    "BIC %1, %0, %1" "\n\t"
    "MSR CPSR_c, %1" "\n\t"
  : "=r"(ret), "=r"(tmp)
        : "r"(mask)
      );

  return ret;
}

static __inline UNS_32 restore_exceptions(UNS_32 old)
{
  register UNS_32 ret, tmp, tmp2;

  __asm__ volatile
  (
    "BIC %1, %3, #0x2F" "\n\t"
    "MRS %0, CPSR" "\n\t"
    "BIC %2, %0, #0xC0" "\n\t"
    "ORR %2, %2, %1" "\n\t"
    "MSR CPSR_c, %2" "\n\t"
  : "=r"(ret), "=r"(tmp), "=r"(tmp2)
        : "r"(old)
      );

  return ret;
}

#endif /* __GNUC__ */


#ifdef __ghs__
__asm UNS_32 disable_irq(void)
{
  STMFD [sp]!, {r1}
  MRS r0, CPSR
  ORR r1, r0, #0x80
  MSR CPSR_c, r1
  LDMFD [sp]!, {r1}
}

__asm UNS_32 disable_fiq(void)
{
  STMFD [sp]!, {r1}
  MRS r0, CPSR
  ORR r1, r0, #0x40
  MSR CPSR_c, r1
  LDMFD [sp]!, {r1}
}

__asm UNS_32 disable_irq_fiq(void)
{
  STMFD [sp]!, {r1}
  MRS r0, CPSR
  ORR r1, r0, #0xC0
  MSR CPSR_c, r1
  LDMFD [sp]!, {r1}
}

__asm UNS_32 enable_irq(void)
{
  STMFD [sp]!, {r1}
  MRS r0, CPSR
  bic r1, r0, #0x80
  MSR CPSR_c, r1
  LDMFD [sp]!, {r1}
}

__asm UNS_32 enable_fiq(void)
{
  STMFD [sp]!, {r1}
  MRS r0, CPSR
  bic r1, r0, #0x40
  MSR CPSR_c, r1
  LDMFD [sp]!, {r1}
}

__asm UNS_32 enable_irq_fiq(void)
{
  STMFD [sp]!, {r1}
  MRS r0, CPSR
  bic r1, r0, #0xC0
  MSR CPSR_c, r1
  LDMFD [sp]!, {r1}
}

__asm UNS_32 disable_irq_fiq_mask(UNS_32 mask)
{
  % reg mask
  STMFD [sp]!, {r1}
  BIC r1, mask, #0x2F
  MRS r0, CPSR
  ORR r1, r0, r1
  MSR CPSR_c, r1
  LDMFD [sp]!, {r1}
  % con mask
  STMFD [sp]!, {r1}
  MOV r1, mask
  BIC r1, r1, #0x2F
  MRS r0, CPSR
  ORR r1, r0, r1
  MSR CPSR_c, r1
  LDMFD [sp]!, {r1}
  % error
}

__asm UNS_32 enable_irq_fiq_mask(UNS_32 mask)
{
  % reg mask
  STMFD [sp]!, {r1}
  BIC r1, mask, #0x2F
  MRS r0, CPSR
  BIC r1, r0, r1
  MSR CPSR_c, r1
  LDMFD [sp]!, {r1}
  % con mask
  STMFD [sp]!, {r1}
  MOV r1, mask
  BIC r1, r1, #0x2F
  MRS r0, CPSR
  BIC r1, r0, r1
  MSR CPSR_c, r1
  LDMFD [sp]!, {r1}
  % error
}

__asm UNS_32 restore_exceptions(UNS_32 old)
{
  % reg old
  STMFD [sp]!, {r1 - r2}
  BIC r1, old, #0x2F
  MRS r0, CPSR
  BIC r2, r0, #0xC0
  ORR r2, r2, r1
  MSR CPSR_c, r2
  LDMFD [sp]!, {r1 - r2}
  % con old
  STMFD [sp]!, {r1 - r2}
  MOV r1, old
  BIC r1, r1, #0x2F
  MRS r0, CPSR
  BIC r2, r0, #0xC0
  ORR r2, r2, r1
  MSR CPSR_c, r2
  LDMFD [sp]!, {r1 - r2}
  % error
}

#endif /* GHS */

#ifdef __arm

static __inline UNS_32 disable_irq(void)
{
  register UNS_32 ret, rx;
  __asm
  {
    mrs ret , CPSR;
    orr rx, ret, #0x80;
    msr CPSR_c, rx
  }
  return ret;
}

static __inline UNS_32 disable_fiq(void)
{
  register UNS_32 ret, rx;
  __asm
  {
    mrs ret, CPSR;
    orr rx, ret, #0x40;
    msr CPSR_c, rx
  }
  return ret;
}

static __inline UNS_32 disable_irq_fiq(void)
{
  register UNS_32 ret, rx;
  __asm
  {
    mrs ret, CPSR;
    orr rx, ret, #0xc0;
    msr CPSR_c, rx
  }
  return ret;
}

static __inline UNS_32 enable_irq(void)
{
  register UNS_32 ret, rx;
  __asm
  {
    mrs ret, CPSR;
    bic rx, ret, #0x80;
    msr CPSR_c, rx
  }
  return ret;
}

static __inline UNS_32 enable_fiq(void)
{
  register UNS_32 ret, rx;
  __asm
  {
    mrs ret, CPSR;
    bic rx, ret, #0x40;
    msr CPSR_c, rx
  }
  return ret;
}

static __inline UNS_32 enable_irq_fiq(void)
{
  register UNS_32 ret, rx;
  __asm
  {
    mrs ret, CPSR;
    bic rx, ret, #0xc0;
    msr CPSR_c, rx
  }
  return ret;
}

static __inline UNS_32  disable_irq_fiq_mask(UNS_32 mask)
{
  register UNS_32 ret, rx, ry;
  __asm
  {
    bic rx, mask, #0x2f;
    mrs ret, CPSR;
    orr ry, ret, rx;
    msr CPSR_c, ry
  }
  return ret;
}

static __inline UNS_32  enable_irq_fiq_mask(UNS_32 mask)
{
  register UNS_32 ret, rx, ry;
  __asm
  {
    bic rx, mask, #0x2f;
    mrs ret, CPSR;
    bic ry, ret, rx;
    msr CPSR_c, ry
  }
  return ret;
}

static __inline UNS_32 restore_exceptions(UNS_32 old)
{
  register UNS_32 ret, rx, ry, rp;
  __asm
  {
    bic rx, old, #0x2f;
    mrs ret, CPSR;
    bic ry, ret, #0xc0;
    orr rp, ry, rx;
    msr CPSR_c, rp
  }
  return ret;
}
#endif /* __arm */

/* IAR CC uses R0 for single input argument to function.
   Return value is also R0 */
#ifdef __ICCARM__

#include "intrinsics.h"

#pragma inline
static UNS_32 disable_irq(void)
{
  UNS_32 c_cpsr;

  c_cpsr = __get_CPSR();
  __set_CPSR(c_cpsr | 0x80);

  return c_cpsr;
}

#pragma inline
static UNS_32 disable_fiq(void)
{
  UNS_32 c_cpsr;

  c_cpsr = __get_CPSR();
  __set_CPSR(c_cpsr | 0x40);

  return c_cpsr;
}

#pragma inline
static UNS_32 disable_irq_fiq(void)
{
  UNS_32 c_cpsr;

  c_cpsr = __get_CPSR();
  __set_CPSR(c_cpsr | 0xC0);

  return c_cpsr;
}

#pragma inline
static UNS_32 enable_irq(void)
{
  UNS_32 c_cpsr;

  c_cpsr = __get_CPSR();
  __set_CPSR(c_cpsr & ~0x80);

  return c_cpsr;
}

#pragma inline
static UNS_32 enable_fiq(void)
{
  UNS_32 c_cpsr;

  c_cpsr = __get_CPSR();
  __set_CPSR(c_cpsr & ~0x40);

  return c_cpsr;
}

#pragma inline
static UNS_32 enable_irq_fiq(void)
{
  UNS_32 c_cpsr;

  c_cpsr = __get_CPSR();
  __set_CPSR(c_cpsr & ~0xC0);

  return c_cpsr;
}

#pragma inline
static UNS_32 disable_irq_fiq_mask(UNS_32 mask)
{
  UNS_32 c_cpsr;

  c_cpsr = __get_CPSR();
  __set_CPSR((c_cpsr & ~0x2F) | mask);

  return c_cpsr;
}

#pragma inline
static UNS_32 enable_irq_fiq_mask(UNS_32 mask)
{
  UNS_32 c_cpsr;

  c_cpsr = __get_CPSR();
  __set_CPSR((c_cpsr & ~0x2F) & mask);

  return c_cpsr;
}

#pragma inline
static UNS_32 restore_exceptions(UNS_32 old)
{
  /* Not implemented yet */
  return 0;
}

#endif /* __ICCARM__ */

#endif /* LPC_IRQ_FIQ_H */
