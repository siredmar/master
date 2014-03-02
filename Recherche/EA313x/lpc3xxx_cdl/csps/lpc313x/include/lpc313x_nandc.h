/***********************************************************************
* $Id:: lpc313x_nandc.h 6112 2011-01-11 04:41:10Z ing02124            $
*
* Project: LPC313X NAND Flash controller definitions
*
* Description:
*     This file contains the structure definitions and manifest
*     constants for the LPC313X chip family component:
*         NAND Flash controller
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

#ifndef LPC313X_NANDC_H
#define LPC313X_NANDC_H

#include "lpc_types.h"
#include "lpc313x_chip.h"

#ifdef __cplusplus
extern "C"
{
#endif

/***********************************************************************
* NAND Flash controller Register Structures
**********************************************************************/

/* NAND Flash controller Module Register Structure */
typedef volatile struct
{
  volatile UNS_32  irq_status;
  volatile UNS_32  irq_mask;
  volatile UNS_32  irq_status_raw;
  volatile UNS_32  config;
  volatile UNS_32  io_config;
  volatile UNS_32  timing1;
  volatile UNS_32  timing2;
  volatile UNS_32  _unused1[1];
  volatile UNS_32  set_cmd;
  volatile UNS_32  set_addr;
  volatile UNS_32  write_data;
  volatile UNS_32  set_ce;
  volatile UNS_32  read_data;
  volatile UNS_32  check_sts;
  volatile UNS_32  control_flow;
  volatile UNS_32  _unused2[1];
  volatile UNS_32  gpio1;
  volatile UNS_32  gpio2;
  volatile UNS_32  irq_status2;
  volatile UNS_32  irq_mask2;
  volatile UNS_32  irq_status_raw2;
  volatile UNS_32  aes_key[4];
  volatile UNS_32  aes_iv[4];
  volatile UNS_32  aes_state;
  volatile UNS_32  ecc_error_stat;
  volatile UNS_32  aes_from_ahb;
} NAND_FLASH_CTRL_REGS_T;

/* NAND Flash configuration parameters structure used by bootROM */
typedef struct
{
  char   tag[8];
  UNS_8  interface_width;
  UNS_8  reserv_1;
  UNS_16 page_size_in_bytes;
  UNS_16 page_size_in_32bit_words;
  UNS_16 pages_per_block;
  UNS_32 nbr_of_blocks;
  UNS_8  amount_of_address_bytes;
  UNS_8  amount_of_erase_address_bytes;
  UNS_8  support_read_terminate;
  UNS_8  page_increment_byte_nr;
  char   device_name[40];
  UNS_32 timing1;
  UNS_32 timing2;
  UNS_8  ecc_mode;
  UNS_8  reserv_2[3];
} NAND_BOOT_CFG_PARAMS_T;

#define NAND_MAX_CHIPS_SUP (4)
#define NAND_MAX_NUM_BLKS  (4096)
#define NAND_CHIP_BB_LEN   (NAND_MAX_NUM_BLKS / 32)

#define NAND_BUF_SIZE      (512)
#define NAND_OOB_BYTES     (16)
#define NAND_OOB_FREE_SIZE (4) /* Only 4 OOB bytes avaialble to user */
#define NAND_BPARAM_PAGE_SZ (256) /* 256 bytes used for CRC computation */
#define NAND_MAX_DESC_NUM	(8)
#define NAND_DMA_CHAN		(4)

/* NAND device configuration structure type */
typedef struct
{
  NAND_BOOT_CFG_PARAMS_T *param; 			/* BOOT_CFG parameters */
  UNS_32 cs;           								/* chip select */
  volatile BOOL_32 init;							/* device initialized flag */
  volatile BOOL_32 block0_prot;				/* Block is protected */
  UNS_32 page_mode;    								/* if set (516 mode) includes 4 bytes for wearleveling */
  volatile BOOL_32 block0_ok;					/* Block is protected */
  UNS_32 nand_bbt[NAND_CHIP_BB_LEN];  /* NAND Bad Block List Array */
} NAND_DEV_CFG_T;

/**********************************************************************
* Register description of irq_status
**********************************************************************/
#define NAND_IRQ_RB4_POS_EDGE       _BIT(31)
#define NAND_IRQ_RB3_POS_EDGE       _BIT(30)
#define NAND_IRQ_RB2_POS_EDGE       _BIT(29)
#define NAND_IRQ_RB1_POS_EDGE       _BIT(28)
#define NAND_IRQ_ERASED_RAM1        _BIT(27)
#define NAND_IRQ_ERASED_RAM0        _BIT(26)
#define NAND_IRQ_WR_RAM1            _BIT(25)
#define NAND_IRQ_WR_RAM0            _BIT(24)
#define NAND_IRQ_RD_RAM1            _BIT(23)
#define NAND_IRQ_RD_RAM0            _BIT(22)
#define NAND_IRQ_ECC_DEC_RAM0       _BIT(21)
#define NAND_IRQ_ECC_ENC_RAM0       _BIT(20)
#define NAND_IRQ_ECC_DEC_RAM1       _BIT(19)
#define NAND_IRQ_ECC_ENC_RAM1       _BIT(18)
#define NAND_IRQ_NOERR_RAM0         _BIT(17)
#define NAND_IRQ_ERR1_RAM0          _BIT(16)
#define NAND_IRQ_ERR2_RAM0          _BIT(15)
#define NAND_IRQ_ERR3_RAM0          _BIT(14)
#define NAND_IRQ_ERR4_RAM0          _BIT(13)
#define NAND_IRQ_ERR5_RAM0          _BIT(12)
#define NAND_IRQ_ERR_UNR_RAM0       _BIT(11)
#define NAND_IRQ_NOERR_RAM1         _BIT(10)
#define NAND_IRQ_ERR1_RAM1          _BIT(9)
#define NAND_IRQ_ERR2_RAM1          _BIT(8)
#define NAND_IRQ_ERR3_RAM1          _BIT(7)
#define NAND_IRQ_ERR4_RAM1          _BIT(6)
#define NAND_IRQ_ERR5_RAM1          _BIT(5)
#define NAND_IRQ_ERR_UNR_RAM1       _BIT(4)
#define NAND_IRQ_AES_DONE_RAM1      _BIT(1)
#define NAND_IRQ_AES_DONE_RAM0      _BIT(0)

#define NAND_IRQ_RB_POS_EDGE(n)     _BIT(28 + (n))

/**********************************************************************
* Register description of config
**********************************************************************/
#define NAND_CFG_8BIT_ECC           _BIT(12)
#define NAND_CFG_TL_528             _SBF(10, 0x0)
#define NAND_CFG_TL_516             _SBF(10, 0x2)
#define NAND_CFG_TL_512             _SBF(10, 0x3)
#define NAND_CFG_TL_MASK            _SBF(10, 0x3)
#define NAND_CFG_EO                 _BIT(9)
#define NAND_CFG_DC                 _BIT(8)
#define NAND_CFG_M                  _BIT(7)
#define NAND_CFG_LC_0               _SBF(5, 0x0)
#define NAND_CFG_LC_1               _SBF(5, 0x1)
#define NAND_CFG_LC_2               _SBF(5, 0x2)
#define NAND_CFG_LC_MASK            _SBF(5, 0x3)
#define NAND_CFG_ES                 _BIT(4)
#define NAND_CFG_DE                 _BIT(3)
#define NAND_CFG_AO                 _BIT(2)
#define NAND_CFG_WD                 _BIT(1)
#define NAND_CFG_EC                 _BIT(0)

/**********************************************************************
* Register description of io_config
**********************************************************************/
#define NAND_IO_CFG_IO_DRIVE        _BIT(24)
#define NAND_IO_CFG_DATA_DEF(n)     _SBF(8, ((n) & 0xFFFF))
#define NAND_IO_CFG_CLE_1           _SBF(6, 0x01)
#define NAND_IO_CFG_ALE_1           _SBF(4, 0x01)
#define NAND_IO_CFG_WE_1            _SBF(2, 0x01)
#define NAND_IO_CFG_RE_1            _SBF(0, 0x01)

/**********************************************************************
* Register description of timing1
**********************************************************************/
#define NAND_TIM1_TSRD(n)           _SBF(20, ((n) & 0x3))
#define NAND_TIM1_TALS(n)           _SBF(16, ((n) & 0x7))
#define NAND_TIM1_TALH(n)           _SBF(12, ((n) & 0x7))
#define NAND_TIM1_TCLS(n)           _SBF(4, ((n) & 0x7))
#define NAND_TIM1_TCLH(n)           ((n) & 0x7)

/**********************************************************************
* Register description of timing2
**********************************************************************/
#define NAND_TIM2_TDRD(n)           _SBF(28, ((n) & 0x7))
#define NAND_TIM2_TEBI(n)           _SBF(24, ((n) & 0x7))
#define NAND_TIM2_TCH(n)            _SBF(20, ((n) & 0x7))
#define NAND_TIM2_TCS(n)            _SBF(16, ((n) & 0x7))
#define NAND_TIM2_TRH(n)            _SBF(12, ((n) & 0x7))
#define NAND_TIM2_TRP(n)            _SBF(8, ((n) & 0x7))
#define NAND_TIM2_TWH(n)            _SBF(4, ((n) & 0x7))
#define NAND_TIM2_TWP(n)            ((n) & 0x7)

/**********************************************************************
* Register description of set_ce
**********************************************************************/
#define NAND_SETCE_OVR_EN(n)        _BIT(((n) & 0x3) + 12)
#define NAND_SETCE_OVR_V(n)         _BIT(((n) & 0x3) + 8)
#define NAND_SETCE_WP               _BIT(4)
#define NAND_SETCE_CV_MASK          0x0F
#define NAND_SETCE_CV(n)            (0x0F & ~_BIT(((n) & 0x3)))

/**********************************************************************
* Register description of check_sts
**********************************************************************/
#define NAND_CHK_STS_RB4_EDGE       _BIT(8)
#define NAND_CHK_STS_RB3_EDGE       _BIT(7)
#define NAND_CHK_STS_RB2_EDGE       _BIT(6)
#define NAND_CHK_STS_RB1_EDGE       _BIT(5)
#define NAND_CHK_STS_RB_EDGE(n)     _BIT(5 + (n))

#define NAND_CHK_STS_RB4_LVL        _BIT(4)
#define NAND_CHK_STS_RB3_LVL        _BIT(3)
#define NAND_CHK_STS_RB2_LVL        _BIT(2)
#define NAND_CHK_STS_RB1_LVL        _BIT(1)
#define NAND_CHK_STS_RB_LVL(n)      _BIT(1 + (n))

#define NAND_CHK_STS_APB_BSY        _BIT(0)

/**********************************************************************
* Register description of aes_from_ahb
**********************************************************************/
#define NAND_CTRL_WR_RAM1           _BIT(5)
#define NAND_CTRL_WR_RAM0           _BIT(4)
#define NAND_CTRL_RD_RAM1           _BIT(1)
#define NAND_CTRL_RD_RAM0           _BIT(0)

/**********************************************************************
* Register description of control_flow
**********************************************************************/
#define NAND_AES_AHB_EN             _BIT(7)
#define NAND_AES_AHB_DCRYPT_RAM1    _BIT(1)
#define NAND_AES_AHB_DCRYPT_RAM0    _BIT(0)



/* Macro pointing to CGU switch box registers */
#define NAND_CTRL  ((NAND_FLASH_CTRL_REGS_T  *)(NANDFLASH_CTRL_CFG_BASE))
#define NAND_RAM0_BUF (NANDFLASH_CTRL_S0_BASE)
#define NAND_RAM1_BUF (NANDFLASH_CTRL_S0_BASE + 0x400)


#ifdef __cplusplus
}
#endif

#endif /* LPC313X_NANDC_H */
