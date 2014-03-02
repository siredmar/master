/* cpu.c

   written by Marc Singer
   15 Sep 2008

   Copyright (C) 2008 Marc Singer

   -----------
   DESCRIPTION
   -----------

   CPU specific routines, local to this port.

   It is noteworthy that the documentation for the Sample at Reset
   Register is confusing in how it specifies bits.  The SampleAtReset[0] is
   really the MSB at [25].

   TCLK[1:0] = DEV_D[7:6]
   Mode[3:0] = DEV_D[12], DEV_D[15], DEV_D[4], DEV_D[2]

*/

#include <config.h>
#include <apex.h>
#include <linux/types.h>
#include "hardware.h"

#define MPP_SAMPLE_TCLK(v) (((v)>>8)&0x3)	// 88F5182
#define MPP_SAMPLE_MODE(v) (((v)>>4)&0xf)	// 88F5182

#define MPP_SAMPLE_TCLK_133	(0)
#define MPP_SAMPLE_TCLK_150	(1)
#define MPP_SAMPLE_TCLK_166	(2)

#define MPP_SAMPLE_MODE_333_167	(0)
#define MPP_SAMPLE_MODE_400_200	(1)
#define MPP_SAMPLE_MODE_400_133	(2)
#define MPP_SAMPLE_MODE_500_167	(3)

uint32_t get_tclk (void)
{
  switch (MPP_SAMPLE_TCLK (MPP_SAMPLE)) {
  case MPP_SAMPLE_TCLK_133:		return TCLK_133;
  case MPP_SAMPLE_TCLK_150:		return TCLK_150;
  default:
  case MPP_SAMPLE_TCLK_166:		return TCLK_166;
  }
}

uint32_t get_cpu_clk (void)
{
  switch (MPP_SAMPLE_MODE (MPP_SAMPLE)) {
  case MPP_SAMPLE_MODE_333_167:		return CPUCLK_333;
  case MPP_SAMPLE_MODE_400_200:		return CPUCLK_400;
  case MPP_SAMPLE_MODE_400_133:		return CPUCLK_400;
  default:
  case MPP_SAMPLE_MODE_500_167:		return CPUCLK_500;
  }
}
