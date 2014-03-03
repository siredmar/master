/* orion5x.h

   written by Marc Singer
   14 Sep 2008

   Copyright (C) 2008 Marc Singer

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__ORION5X_H__)
#    define   __ORION5X_H__

/* ----- Includes */

#include <asm/reg.h>


/* ----- Macros */

#define INTERNAL_PHYS	(0xf1000000)

#define MPP_PHYS	(INTERNAL_PHYS + 0x00010000)
#define UART0_PHYS      (INTERNAL_PHYS + 0x00012000)
#define UART1_PHYS      (INTERNAL_PHYS + 0x00012100)
#define CPU_PHYS	(INTERNAL_PHYS + 0x00020100)
#define DDR_PHYS	(INTERNAL_PHYS + 0x00000000)
#define SATAHC_PHYS	(INTERNAL_PHYS + 0x00080000)
#define EDMA_PHYS(i)	(INTERNAL_PHYS + 0x00080000 + ((i) + 1)*0x2000)
#define EDMA0_PHYS	(INTERNAL_PHYS + 0x00082000)
#define EDMA1_PHYS	(INTERNAL_PHYS + 0x00084000)
#define ATAX_PHYS(i)	(EDMA_PHYS(i) + 0x100)
#define ATA0_PHYS	(EDMA0_PHYS + 0x0100)
#define ATA1_PHYS	(EDMA1_PHYS + 0x0100)

#define CPU_CONFIG	__REG(CPU_PHYS + 0x0000)
#define CPU_CTRL_STAT	__REG(CPU_PHYS + 0x0004)
#define CPU_RSTOUTN	__REG(CPU_PHYS + 0x0008)
#define CPU_SOFT_RESET	__REG(CPU_PHYS + 0x000c)
#define CPU_TIMERS_CTRL	__REG(CPU_PHYS + 0x0200)
#define CPU_TIMER0_LOAD	__REG(CPU_PHYS + 0x0210)
#define CPU_TIMER0	__REG(CPU_PHYS + 0x0214)
#define CPU_TIMER1_LOAD	__REG(CPU_PHYS + 0x0218)
#define CPU_TIMER1	__REG(CPU_PHYS + 0x021c)
#define CPU_WDOG_LOAD	__REG(CPU_PHYS + 0x0220)
#define CPU_WDOG	__REG(CPU_PHYS + 0x0224)

#define MPP_SAMPLE	__REG(MPP_PHYS + 0x0010) /* Pin samples at CPU reset */

#define MPP_SAMPLE_TCLK(v) (((v)>>8)&0x3)	// 88F5182
#define MPP_SAMPLE_MODE(v) (((v)>>4)&0xf)	// 88F5182

#define MPP_SAMPLE_TCLK_133	(0)
#define MPP_SAMPLE_TCLK_150	(1)
#define MPP_SAMPLE_TCLK_166	(2)

#define CPU_TIMERS_CTRL_0_EN	(1<<0)
#define CPU_TIMERS_CTRL_0_AUTO	(1<<1)
#define CPU_TIMERS_CTRL_1_EN	(1<<2)
#define CPU_TIMERS_CTRL_1_AUTO	(1<<3)
#define CPU_TIMERS_CTRL_WD_EN	(1<<4)
#define CPU_TIMERS_CTRL_WD_AUTO	(1<<5)

#define SDRAM_BANK0_PHYS	(0x00000000)
#define SDRAM_BANK1_PHYS	(0x10000000)
#define SDRAM_BANK2_PHYS	(0x20000000)
#define SDRAM_BANK3_PHYS	(0x30000000)
#define SDRAM_BANK_SIZE		(0x10000000)

#define TCLK_100		(100000000)
#define TCLK_125		(125000000)
#define TCLK_133		(133333333)
#define TCLK_150		(150000000)
#define TCLK_166		(166666667)
#define TCLK_200		(200000000)

#define SYSCLK_100		(100000000)
#define SYSCLK_133		(133333333)
#define SYSCLK_166		(166666667)
#define SYSCLK_200		(200000000)

#define CPUCLK_333		(333333333)
#define CPUCLK_400		(400000000)
#define CPUCLK_500		(500000000)

#define SATAHC_CONF		__REG (SATAHC_PHYS + 0x00)
#define SATAHC_RQ_OUTP		__REG (SATAHC_PHYS + 0x04)
#define SATAHC_RQ_INP		__REG (SATAHC_PHYS + 0x08)
#define SATAHC_INT_COALESCE	__REG (SATAHC_PHYS + 0x0c)
#define SATAHC_INT_THRESH	__REG (SATAHC_PHYS + 0x10)
#define SATAHC_INT_CAUSE	__REG (SATAHC_PHYS + 0x14)
#define SATAHC_MAIN_CAUSE	__REG (SATAHC_PHYS + 0x20)
#define SATAHC_MAIN_MASK	__REG (SATAHC_PHYS + 0x24)
#define SATAHC_LED		__REG (SATAHC_PHYS + 0x2c)
#define SATAHC_WIN0_CTRL	__REG (SATAHC_PHYS + 0x30)
#define SATAHC_WIN0_BASE	__REG (SATAHC_PHYS + 0x34)
#define SATAHC_WIN1_CTRL	__REG (SATAHC_PHYS + 0x40)
#define SATAHC_WIN1_BASE	__REG (SATAHC_PHYS + 0x44)
#define SATAHC_WIN2_CTRL	__REG (SATAHC_PHYS + 0x50)
#define SATAHC_WIN2_BASE	__REG (SATAHC_PHYS + 0x54)
#define SATAHC_WIN3_CTRL	__REG (SATAHC_PHYS + 0x60)
#define SATAHC_WIN3_BASE	__REG (SATAHC_PHYS + 0x64)

#define EDMA_CONF(i)            __REG (EDMA_PHYS(i) + 0x00)
#define EDMA_TIMER(i)           __REG (EDMA_PHYS(i) + 0x04)
#define EDMA_ERROR_CAUSE(i)     __REG (EDMA_PHYS(i) + 0x08)
#define EDMA_ERROR_MASK(i)      __REG (EDMA_PHYS(i) + 0x0c)
#define EDMA_REQ_BASE_HIGH(i)   __REG (EDMA_PHYS(i) + 0x10)
#define EDMA_REQ_INP(i)         __REG (EDMA_PHYS(i) + 0x14)
#define EDMA_REQ_OUTP(i)        __REG (EDMA_PHYS(i) + 0x18)
#define EDMA_RSP_BASE_HIGH(i)   __REG (EDMA_PHYS(i) + 0x1c)
#define EDMA_RSP_INP(i)         __REG (EDMA_PHYS(i) + 0x20)
#define EDMA_RSP_OUTP(i)        __REG (EDMA_PHYS(i) + 0x24)
#define EDMA_CMD(i)             __REG (EDMA_PHYS(i) + 0x28)
#define EDMA_TEST(i)            __REG (EDMA_PHYS(i) + 0x2c)
#define EDMA_STATUS(i)          __REG (EDMA_PHYS(i) + 0x30)
#define EDMA_IORDY_TIMEOUT(i)   __REG (EDMA_PHYS(i) + 0x34)
#define EDMA_CMD_DELAY(i)       __REG (EDMA_PHYS(i) + 0x40)
#define EDMA_HALT(i)            __REG (EDMA_PHYS(i) + 0x60)
#define EDMA_NCQ0_STATUS(i)     __REG (EDMA_PHYS(i) + 0x94)
#define EDMA_NCQ1_STATUS(i)     __REG (EDMA_PHYS(i) + 0x98)
#define EDMA_NCQ2_STATUS(i)     __REG (EDMA_PHYS(i) + 0x9c)
#define EDMA_NCQ3_STATUS(i)     __REG (EDMA_PHYS(i) + 0xa0)

#define SATAI_SSTATUS(i)        __REG (EDMA_PHYS(i) + 0x300)
#define SATAI_SERROR(i)         __REG (EDMA_PHYS(i) + 0x304)
#define SATAI_SERROR_MASK(i)    __REG (EDMA_PHYS(i) + 0x340)
#define SATAI_SCONTROL(i)       __REG (EDMA_PHYS(i) + 0x308)
#define SATAI_LTMODE(i)         __REG (EDMA_PHYS(i) + 0x30c)
#define SATAI_PHYMODE3(i)       __REG (EDMA_PHYS(i) + 0x310)
#define SATAI_PHYMODE4(i)       __REG (EDMA_PHYS(i) + 0x314)
#define SATAI_PHYMODE1(i)       __REG (EDMA_PHYS(i) + 0x32c)
#define SATAI_PHYMODE2(i)       __REG (EDMA_PHYS(i) + 0x330)
#define SATAI_BIST_CTRL(i)      __REG (EDMA_PHYS(i) + 0x334)
#define SATAI_BIST_DW1(i)       __REG (EDMA_PHYS(i) + 0x338)
#define SATAI_BIST_DW2(i)       __REG (EDMA_PHYS(i) + 0x33c)
#define SATAI_CONF(i)           __REG (EDMA_PHYS(i) + 0x050)
#define SATAI_CONTROL(i)        __REG (EDMA_PHYS(i) + 0x344)
#define SATAI_TEST(i)           __REG (EDMA_PHYS(i) + 0x348)
#define SATAI_STATUS(i)         __REG (EDMA_PHYS(i) + 0x34c)
#define SATAI_VENDOR(i)         __REG (EDMA_PHYS(i) + 0x35c)
#define SATAI_FIS_CONF(i)       __REG (EDMA_PHYS(i) + 0x360)
#define SATAI_FIS_CAUSE(i)      __REG (EDMA_PHYS(i) + 0x364)
#define SATAI_FIS_MASK(i)       __REG (EDMA_PHYS(i) + 0x368)
#define SATAI_FIS_DW0(i)        __REG (EDMA_PHYS(i) + 0x370)
#define SATAI_FIS_DW1(i)        __REG (EDMA_PHYS(i) + 0x374)
#define SATAI_FIS_DW2(i)        __REG (EDMA_PHYS(i) + 0x378)
#define SATAI_FIS_DW3(i)        __REG (EDMA_PHYS(i) + 0x37c)
#define SATAI_FIS_DW4(i)        __REG (EDMA_PHYS(i) + 0x380)
#define SATAI_FIS_DW5(i)        __REG (EDMA_PHYS(i) + 0x384)
#define SATAI_FIS_DW6(i)        __REG (EDMA_PHYS(i) + 0x388)

#define SATAI_SCONTROL_RQINIT	(1<<0)
#define SATAI_SCONTROL_NOPWR    (3<<1) /* Disable power management */

#define SATAI_SSTATUS_DET_MSK	(0xf)
#define SATAI_SSTATUS_DET_SH	(0)
#define SATAI_SSTATUS_SPD_MSK	(0xf)
#define SATAI_SSTATUS_SPD_SH	(4)
#define SATAI_SSTATUS_IPM_MSK	(0xf)
#define SATAI_SSTATUS_IPM_SH	(8)


#endif  /*__ORION5X_H__ */
