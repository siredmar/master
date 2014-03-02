/***********************************************************************
* $Id:: lpc313x_mpmc.h 2278 2009-09-22 23:34:00Z pdurgesh             $
*
* Project: LPC313X multi-port memory controller definitions
*
* Description:
*     This file contains the structure definitions and manifest
*     constants for the LPC313X chip family component:
*         multi-port memory controller
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

#ifndef LPC313X_MPMC_H
#define LPC313X_MPMC_H

#include "lpc_types.h"
#include "lpc313x_chip.h"

#ifdef __cplusplus
extern "C"
{
#endif

/***********************************************************************
* multi-port memory controller Register Structures
**********************************************************************/

/* structure for dynamic ram controller */
typedef volatile struct
{
  /* Dynamic Configuration Register */
  volatile UNS_32  config;
  /* Dynamic Memory RAS and CAS Delay for nDCS0 */
  volatile UNS_32  rascas;
  volatile UNS_32  _unused[6];
} MPMC_DYNAMIC_REGS_T;

/* structure for static device controller */
typedef volatile struct
{
  /* Static Memory Configuration for nCS0 */
  volatile UNS_32  config;
  /* Static Memory Write Enable Delay for nCS0 */
  volatile UNS_32  wtwen;
  /* Static Memory Output Enable Delay for nCS0 */
  volatile UNS_32  wtoen;
  /* Static Memory Read Delay for nCS0 */
  volatile UNS_32  wtrd;
  /* Static Memory Page Mode Read Delay for nCS0 */
  volatile UNS_32  wtpg;
  /* Static Memory Write Delay for nCS0 */
  volatile UNS_32  wtwr;
  /* Static Memory Turn Around Delay for nCS0 */
  volatile UNS_32  wtturn;
  volatile UNS_32  _unused;
} MPMC_STATIC_REGS_T;

/* multi-port memory controller Module Register Structure */
typedef volatile struct
{
  /* Control Register */
  volatile UNS_32  control;
  /* Status Register */
  const  volatile UNS_32  status;
  /* Configuration Register */
  volatile UNS_32  config;
  volatile UNS_32  _unused0[5];
  /* Dynamic Memory Control Register */
  volatile UNS_32  sdram_ctrl;
  /* Dynamic Memory Refresh Timer */
  volatile UNS_32  sdram_ref;
  /* Dynamic Memory read configuration Register */
  volatile UNS_32  sdram_rdcfg;
  volatile UNS_32  _unused8[1];
  /* Dynamic Memory Precharge Command Period */
  volatile UNS_32  sdram_rp;
  /* Dynamic Memory Active to Precharge Command Period */
  volatile UNS_32  sdram_ras;
  /* Dynamic Memory Self-Refresh Exit Time */
  volatile UNS_32  sdram_srex;
  /* Dynamic Memory Last Data Out to Active Time */
  volatile UNS_32  sdram_apr;
  /* Dynamic Memory Data-In to Active Command Time */
  volatile UNS_32  sdram_dal;
  /* Dynamic Memory Write Recovery Time */
  volatile UNS_32  sdram_wr;
  /* Dynamic Memory Active to Active Command Period */
  volatile UNS_32  sdram_rc;
  /* Dynamic Memory Auto-Refresh Period, and Auto-Refresh
     to Active Command Period */
  volatile UNS_32  sdram_rfc;
  /* Dynamic Memory Exit Self-Refresh to Active Command Time */
  volatile UNS_32  sdram_xsr;
  /* Dynamic Memory Active Bank A to Active Bank B Time */
  volatile UNS_32  sdram_rrd;
  /* Dynamic Memory Load Mode Register to Active Command Time */
  volatile UNS_32  sdram_mrd;
  volatile UNS_32  _unused1[9];
  /* Static Memory Extended Wait */
  volatile UNS_32  static_ext_wait;
  volatile UNS_32  _unused2[31];
  /* Dynamic Configuration Registers */
  MPMC_DYNAMIC_REGS_T sdram[4];
  volatile UNS_32  _unused3[32];
  MPMC_STATIC_REGS_T  static_mem[4];
  volatile UNS_32  _unused4[0x320];
  volatile UNS_32  itcr     ;
  volatile UNS_32  _unused5[7];
  volatile UNS_32  itip     ;
  volatile UNS_32  _unused6[7];
  volatile UNS_32  itop     ;
  volatile UNS_32  _unused7[35];
  const  volatile UNS_32  periph_id4[4];
  const  volatile UNS_32  periph_id0[4];
  const  volatile UNS_32  pcell_id[4];

} MPMC_REGS_T;

#ifdef __cplusplus
}
#endif

/***********************************************************************
 * MPMC Controller Bit Field constants
 **********************************************************************/
#define MPMC_CTL_LOW_PWR               _BIT(2)
#define MPMC_CTL_ENABLE                _BIT(0)

/***********************************************************************
 * MPMC status Bit Field constants
 **********************************************************************/
#define MPMC_STATUS_SA                 _BIT(2)
#define MPMC_STATUS_WR_BUF             _BIT(1)
#define MPMC_STATUS_BUSY               _BIT(0)

/***********************************************************************
 * MPMC config Bit Field constants
 **********************************************************************/
#define MPMC_CFG_SDCCLK_1_2            _BIT(8)
#define MPMC_CFG_SDCCLK_1_1            (0)

/***********************************************************************
 * SDRAM Controller Bit Field constants
 **********************************************************************/
#define MPMC_SDRAMC_CTL_DP             _BIT(13)
#define MPMC_SDRAMC_CTL_NORMAL_CMD     _SBF(7,0)
#define MPMC_SDRAMC_CTL_MODE_CMD       _SBF(7,1)
#define MPMC_SDRAMC_CTL_PALL_CMD       _SBF(7,2)
#define MPMC_SDRAMC_CTL_NOP_CMD        _SBF(7,3)
#define MPMC_SDRAMC_CTL_MCC            _BIT(5)
#define MPMC_SDRAMC_CTL_SR             _BIT(2)
#define MPMC_SDRAMC_CTL_CS             _BIT(1)
#define MPMC_SDRAMC_CTL_CE             _BIT(0)

/***********************************************************************
 * SDRAM Config Bit Field constants
 **********************************************************************/
#define MPMC_SDRAMC_CFG_SDRAM_MD       _SBF(3,0)
#define MPMC_SDRAMC_CFG_LOW_PWR_MD     _SBF(3,1)
#define MPMC_SDRAMC_CFG_SYNC_FLASH     _SBF(3,2)
#define MPMC_SDRAMC_CFG_BUF_EN         _BIT(19)
#define MPMC_SDRAMC_CFG_WP             _BIT(20)

/*16-bit external bus high-performance address mapping */
/*16Mb (2Mx8), 2 banks, row length = 11, column length = 9*/
#define SDRAMC_16HP_2Mx8_2B_R11_C9    (_SBF(7,0x00))
/*16Mb (1Mx16), 2 banks, row length = 11, column length = 8*/
#define SDRAMC_16HP_1Mx16_2B_R11_C8   (_SBF(7,0x01))
/*64Mb (8Mx8), 4 banks, row length = 12, column length = 9*/
#define SDRAMC_16HP_8Mx8_4B_R12_C9    (_SBF(7,0x04))
/*64Mb (4Mx16), 4 banks, row length = 12, column length = 8*/
#define SDRAMC_16HP_4Mx16_4B_R12_C8   (_SBF(7,0x05))
/*128Mb (16Mx8), 4 banks, row length = 12, column length = 10*/
#define SDRAMC_16HP_16Mx8_4B_R12_C10  (_SBF(7,0x08))
/*128Mb (8Mx16), 4 banks, row length = 12, column length = 9*/
#define SDRAMC_16HP_8Mx16_4B_R12_C9   (_SBF(7,0x09))
/*256Mb (32Mx8), 4 banks, row length = 13, column length = 10*/
#define SDRAMC_16HP_32Mx8_4B_R13_C10  (_SBF(7,0x0C))
/*256Mb (16Mx16), 4 banks, row length = 13, column length = 9*/
#define SDRAMC_16HP_16Mx16_4B_R13_C9  (_SBF(7,0x0D))
/*512Mb (64Mx8), 4 banks, row length = 13, column length = 11*/
#define SDRAMC_16HP_64Mx8_4B_R13_C11  (_SBF(7,0x10))
/*512Mb (32Mx16), 4 banks, row length = 13, column length = 10*/
#define SDRAMC_16HP_32Mx16_4B_R13_C10 (_SBF(7,0x11))

/*16-bit external bus low power SDRAM address mapping */
/*16Mb (2Mx8), 2 banks, row length = 11, column length = 9*/
#define SDRAMC_16LP_2Mx8_2B_R11_C9    (_SBF(7,0x20))
/*16Mb (1Mx16), 2 banks, row length = 11, column length = 8*/
#define SDRAMC_16LP_1Mx16_2B_R11_C8   (_SBF(7,0x21))
/*64Mb (8Mx8), 4 banks, row length = 12, column length = 9*/
#define SDRAMC_16LP_8Mx8_4B_R12_C9    (_SBF(7,0x24))
/*64Mb (4Mx16), 4 banks, row length = 12, column length = 8*/
#define SDRAMC_16LP_4Mx16_4B_R12_C8   (_SBF(7,0x25))
/*128Mb (16Mx8), 4 banks, row length = 12, column length = 10*/
#define SDRAMC_16LP_16Mx8_4B_R12_C10  (_SBF(7,0x28))
/*128Mb (8Mx16), 4 banks, row length = 12, column length = 9*/
#define SDRAMC_16LP_8Mx16_4B_R12_C9   (_SBF(7,0x29))
/*256Mb (32Mx8), 4 banks, row length = 13, column length = 10*/
#define SDRAMC_16LP_32Mx8_4B_R13_C10  (_SBF(7,0x2C))
/*256Mb (16Mx16), 4 banks, row length = 13, column length = 9*/
#define SDRAMC_16LP_16Mx16_4B_R13_C9  (_SBF(7,0x2D))
/*512Mb (64Mx8), 4 banks, row length = 13, column length = 11*/
#define SDRAMC_16LP_64Mx8_4B_R13_C11  (_SBF(7,0x30))
/*512Mb (32Mx16), 4 banks, row length = 13, column length = 10*/
#define SDRAMC_16LP_32Mx16_4B_R13_C10 (_SBF(7,0x31))


/***********************************************************************
 * SDRAM Read Config Bit Field constants
 **********************************************************************/
#define MPMC_SDRAMC_RDCFG_CLKOUTDELAY_STG       _SBF(0,0)
#define MPMC_SDRAMC_RDCFG_CMDDELAY_STG          _SBF(0,1)
#define MPMC_SDRAMC_RDCFG_CMDDELAY_P1_STG       _SBF(0,2)
#define MPMC_SDRAMC_RDCFG_CMDDELAY_P2_STG       _SBF(0,3)

/***********************************************************************
 * SDRAM RASCAS Bit Field constants
 **********************************************************************/
#define MPMC_SDRAMC_RASCAS_CAS0        _SBF(8,0)
#define MPMC_SDRAMC_RASCAS_CAS1        _SBF(8,1)
#define MPMC_SDRAMC_RASCAS_CAS2        _SBF(8,2)
#define MPMC_SDRAMC_RASCAS_CAS3        _SBF(8,3)
#define MPMC_SDRAMC_RASCAS_RAS0        _SBF(0,0)
#define MPMC_SDRAMC_RASCAS_RAS1        _SBF(0,1)
#define MPMC_SDRAMC_RASCAS_RAS2        _SBF(0,2)
#define MPMC_SDRAMC_RASCAS_RAS3        _SBF(0,3)

/***********************************************************************
 * Macros to calculate SDRAM timings
 **********************************************************************/
/* TBD: Check RTL if divide by 2 possible. If true change macro to the
   following:
   (hclk/(((MPMC->config & _BIT(8))>>8)+1)) */
#define MPMC_CLOCK(hclk)               (hclk)

#define MPMC_SDRAMC_REFRESH(trf,hclk)  \
  (((UNS_64)(trf * hclk)/16000000000ull) & _BITMASK(11))

#define MPMC_SDRAMC_TRP(trp,hclk)      \
  (((UNS_64)(trp * MPMC_CLOCK(hclk))/1000000000ull) & _BITMASK(4))
#define MPMC_SDRAMC_TRAS(tras,hclk)    \
  (((UNS_64)(tras * MPMC_CLOCK(hclk))/1000000000ull) & _BITMASK(4))
#define MPMC_SDRAMC_TSREX(tsrex,hclk)  \
  (((UNS_64)(tsrex * MPMC_CLOCK(hclk))/1000000000ull) & _BITMASK(4))
#define MPMC_SDRAMC_TDAL(tdal,hclk)    \
  (((UNS_64)(tdal * MPMC_CLOCK(hclk))/1000000000ull) & _BITMASK(4))
#define MPMC_SDRAMC_TWR(twr,hclk)      \
  (((UNS_64)(twr * MPMC_CLOCK(hclk))/1000000000ull) & _BITMASK(4))
#define MPMC_SDRAMC_TRC(trc,hclk)      \
  (((UNS_64)(trc * MPMC_CLOCK(hclk))/1000000000ull) & _BITMASK(5))
#define MPMC_SDRAMC_TRFC(trfc,hclk)    \
  (((UNS_64)(trfc * MPMC_CLOCK(hclk))/1000000000ull) & _BITMASK(5))
#define MPMC_SDRAMC_TXSR(txsr,hclk)    \
  (((UNS_64)(txsr * MPMC_CLOCK(hclk))/1000000000ull) & _BITMASK(5))
#define MPMC_SDRAMC_TRRD(trrd,hclk)    \
  (((UNS_64)(trrd * MPMC_CLOCK(hclk))/1000000000ull) & _BITMASK(5))
#define MPMC_SDRAMC_TMRD(tmrd,hclk)    \
  (((UNS_64)(tmrd * MPMC_CLOCK(hclk))/1000000000ull) & _BITMASK(5))


/***********************************************************************
 * Static Memory config Bit Field constants
 **********************************************************************/
#define MPMC_STATIC_CFG_MW8      _SBF(0,0)  /* Memory width 8 bits */
#define MPMC_STATIC_CFG_MW16     _SBF(0,1)  /* Memory width 16 bits */
#define MPMC_STATIC_CFG_MW32     _SBF(0,2)  /* Memory width 16 bits */

#define MPMC_STATIC_CFG_PM       _BIT(3)
#define MPMC_STATIC_CFG_PC       _BIT(6)
#define MPMC_STATIC_CFG_BLS      _BIT(7)
#define MPMC_STATIC_CFG_EW       _BIT(8)
#define MPMC_STATIC_CFG_B        _BIT(19)
#define MPMC_STATIC_CFG_P        _BIT(20)

/***********************************************************************
 * Macros to calculate static memory timings
 **********************************************************************/
#define MPMC_STATIC_EXWAIT(twait,hclk)   \
  (((UNS_64)(twait * hclk)/16000000000) & _BITMASK(10))

#define MPMC_STATIC_WAIT_WEN(twen,hclk)  \
  (((UNS_64)(twen * hclk)/1000000000) & _BITMASK(4))

#define MPMC_STATIC_WAIT_OEN(toen,hclk)  \
  (((UNS_64)(toen * hclk)/1000000000) & _BITMASK(4))

#define MPMC_STATIC_WAIT_RD(trd,hclk)    \
  (((UNS_64)(trd * hclk)/1000000000) & _BITMASK(5))

#define MPMC_STATIC_WAIT_PAGE(tpg,hclk)  \
  (((UNS_64)(tpg * hclk)/1000000000) & _BITMASK(5))

#define MPMC_STATIC_WAIT_WR(twr,hclk)    \
  (((UNS_64)(twr * hclk)/1000000000) & _BITMASK(5))

#define MPMC_STATIC_WAIT_TURN(trn,hclk)  \
  (((UNS_64)(trn * hclk)/1000000000) & _BITMASK(4))

/* Macro pointing to SysRegs registers */
#define MPMC  ((MPMC_REGS_T  *)(MPMC_CFG_BASE))

#endif /* LPC313X_MPMC_H */
