/***********************************************************************
 * $Id:: lpc313x_nand_driver.c 8087 2011-09-14 04:13:00Z ing02124       $
 *
 * Project: LPC313x NAND controller driver
 *
 * Description:
 *     This file contains driver support for the LPC313x NAND.
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
 *********************************************************************/

#include "lpc313x_sysreg.h"
#include "lpc313x_nand_driver.h"
#include "lpc313x_cgu_driver.h"
#include "lpc313x_crc_driver.h"
#include "lpc313x_dma_driver.h"
#include "lpc313x_intc_driver.h"
#include "lpc313x_timer_driver.h"
#include "lpc313x_uart_driver.h"
#include "lpc31xx_vmem_driver.h"
#include "string.h"

/***********************************************************************
 * NAND driver package data
***********************************************************************/

/* NAND controller Driver structure */
typedef struct
{
  volatile BOOL_32 init;               	/* Controller initialized flag */
  UNS_32 instance;                    	/* Instance counter */
  NAND_FLASH_CTRL_REGS_T *regptr;     	/* Pointer to NAND registers */
  NAND_DEV_CFG_T dev[NAND_MAX_CHIPS_SUP]; /* device data structures for chips */
  NAND_DEV_CFG_T* cur_dev;                /* pointer to current device data structure */
  volatile UNS_32 tfr_done;			/* Data Transfer Completed Flag */
  volatile BOOL_32 dma;				/* DMA Transfer Flag */
  volatile UNS_32 restart_dma;		/* Restart DMA flag for write */
  volatile UNS_16 steps;			/* Number of data DMA transfers */
} NAND_DRV_T;

/* DMA descriptor setup structure */
typedef struct
{
  UNS_32 src_address;		/* source address */
  UNS_32 dest_address;		/* Destination address */
  UNS_32 trans_length;		/* transfer length */
  UNS_32 cfg;			/* channel configuration */
} NAND_DMA_SETUP_T;

/* DMA scatter-gather list structure */
typedef struct dma_sg_ll
{
  NAND_DMA_SETUP_T setup;	/* DMA Set up info */
  UNS_32 next_entry;		/* Address of Next entry */
} NAND_DMA_SG_LL_T;

#define NAND_PAGE_PROG_TIME		(500)    /* Maximum - 500 usec */
#define NAND_BLK_ERASE_TIME		(3)      /* Maximum - 3 msec */
#define NAND_PAGE_LOAD_TIME		(25)     /* Maximum - 25 usec */

/* NAND driver data structure */
static NAND_DRV_T g_nand_drv;
/* NAND Data buffer */
static UNS_32 data_buf[LARGE_BLOCK_PAGE_SIZE / 4];
/* NAND Bad BLock List array */
static UNS_32 bbt_list[NAND_MAX_NUM_BLKS + 1];
/*  NAND DMA Scatter gather list array */
static NAND_DMA_SG_LL_T sg_ll[NAND_MAX_DESC_NUM + 1];


/* NAND Flash Chip information structures */
static const NAND_BOOT_CFG_PARAMS_T chips[] =
{
  /* Micron MT 256MB NAND Flash */
  {
    "NANDflsh",       /* tag  */
    8,                /* .interface_width */
    0,	          /* reserved 1 */
    2048,             /* page_size_in_bytes */
    512,	          /* page_size_in_32bit_words */
    64,	          /* pages_per_block */
    2048,             /* nbr_of_blocks */
    5,                /* amount_of_address_bytes */
    3,                /* amount_of_erase_address_bytes */
    1,                /* support_read_terminate */
    2,	          /* page_increment_byte_nr */
    { 0x2C, 0xAA, 0xFF, 0x15, 0x20, 'M', 'T', '2', '9', 'F', '2', 'G', '0', '8', },
    /* .device_name */
    /* first 4bytes to fill with readid respon */
    /* assuming 90MHz clock (1clock = 11ns)to NAND block */
    /* Note, timing macros tcopy naake clk +1 values. */
    /* tsrd=3, tals=3, talh=1, tcls=3, tclh=1, */
    NAND_TIM1_TSRD(3) | NAND_TIM1_TALS(3) | NAND_TIM1_TALH(1) | NAND_TIM1_TCLS(3) | NAND_TIM1_TCLH(1),
    /* .timing1 */
    /* tdrd=3, tebi=1, tch=1, tcs=4, treh=2, trp=4, twh=2, twp = 3*/
    NAND_TIM2_TDRD(3) | NAND_TIM2_TEBI(1) | NAND_TIM2_TCH(1) | NAND_TIM2_TCS(4) | NAND_TIM2_TRH(2) |
    NAND_TIM2_TRP(4) | NAND_TIM2_TWH(2) | NAND_TIM2_TWP(3),
    /* timing2 */
    5,                /* ecc_mode */
    {0, 0, 0, },	    /* reserved2 */
  },
  /* Micron MT 512MB NAND Flash */
  {
    "NANDflsh",       /* tag */
    8,		    /* interface_width */
    0,	          /* reserved 1 */
    2048,		    /* page_size_in_bytes */
    512,	          /* page_size_in_32bit_words */
    64,		    /* pages_per_block */
    4096,		    /* nbr_of_blocks */
    5,		    /* amount_of_address_bytes */
    3,		    /* amount_of_erase_address_bytes */
    1,		    /* support_read_terminate */
    2,		    /* page_increment_byte_nr */
    { 0x2C, 0xAC, 0x90, 0x15, 0x54, 'M', 'T', '2', '9', 'F', '4', 'G', '0', '8', },
    /* .device_name */
    /* first 4bytes to fill with readid respon */
    /* assuming 90MHz clock (1clock = 11ns)to NAND block */
    /* Note, timing macros tcopy naake clk +1 values. */
    /* tsrd=3, tals=3, talh=1, tcls=3, tclh=1, */
    NAND_TIM1_TSRD(3) | NAND_TIM1_TALS(3) | NAND_TIM1_TALH(1) | NAND_TIM1_TCLS(3) | NAND_TIM1_TCLH(1),
    /* timing1 */
    /* tdrd=3, tebi=1, tch=1, tcs=4, treh=2, trp=4, twh=2, twp = 3*/
    NAND_TIM2_TDRD(3) | NAND_TIM2_TEBI(1) | NAND_TIM2_TCH(1) | NAND_TIM2_TCS(4) | NAND_TIM2_TRH(2) |
    NAND_TIM2_TRP(4) | NAND_TIM2_TWH(2) | NAND_TIM2_TWP(3),
    /* timing2 */
    5,                /* ecc_mode */
    { 0, 0, 0 },	    /* reserved2 */
  },
  /* Samsung 256MB NAND Flash */
  {
    "NANDflsh",       /* tag */
    8,		    /* interface_width */
    0,	          /* reserved 1 */
    0x0800,		    /* page_size_in_bytes */
    0x0200,		    /* page_size_in_32bit_words */
    0x0040,		    /* pages_per_block */
    0x0800,		    /* nbr_of_blocks */
    5,		    /* amount_of_address_bytes */
    3,		    /* amount_of_erase_address_bytes */
    1,		    /* support_read_terminate */
    2,		    /* page_increment_byte_nr */
    { 0xEC, 0xD3, 0x51, 0x95, 0x20, 'K', '9', 'K', '8', 'G', '0', '8', },
    /* device_name */
    /* first 4bytes to fill with readid response */
    /* assuming 90MHz clock (1clock = 11ns)to NAND block */
    /* Note, timing macros take clk +1 values. */
    /* tsrd=2, tals=3, talh=1, tcls=3, tclh=2, */
    NAND_TIM1_TSRD(2) | NAND_TIM1_TALS(3) | NAND_TIM1_TALH(1) | NAND_TIM1_TCLS(3) | NAND_TIM1_TCLH(2),
    /* timing1 */
    /* tdrd=2, tebi=1, tch=1, tcs=4, treh=2, trp=4, twh=2, twp = 3*/
    NAND_TIM2_TDRD(2) | NAND_TIM2_TEBI(1) | NAND_TIM2_TCH(1) | NAND_TIM2_TCS(4) | NAND_TIM2_TRH(2) |
    NAND_TIM2_TRP(4) | NAND_TIM2_TWH(2) | NAND_TIM2_TWP(3),
    /* timing2 */
    5,	          /* ecc_mode */
    { 0, 0, 0 },	    /* reserved2 */
  },
  /* Toshiba 32MB SPF */
  {
    "NANDflsh", 	    /* tag */
    8,		    /* interface_width */
    0,	          /* reserved 1 */
    512,		    /* page_size_in_bytes */
    128,		    /* page_size_in_32bit_words */
    32,		    /* pages_per_block */
    2048,		    /* nbr_of_blocks */
    3,		    /* amount_of_address_bytes */
    2,		    /* amount_of_erase_address_bytes */
    0,		    /* support_read_terminate */
    2,		    /* page_increment_byte_nr */
    { 0x98, 0x73, 0xA5, 0xBA, 0xFF, 'T', 'C', '5', '8', 'D', 'V', 'M', '8', '2', 'A' },
    /* device_name */
    /* first 4 bytes to fill with readid */
    /* assuming 90MHz clock (1clock = 11ns)to NAND block */
    /* Note, timing macros take clk +1 values. */
    /* tsrd=2, tals=3, talh=1, tcls=3, tclh=2, */
    NAND_TIM1_TSRD(2 + 1) | NAND_TIM1_TALS(3 + 1) | NAND_TIM1_TALH(1 + 1) | NAND_TIM1_TCLS(3 + 1) | NAND_TIM1_TCLH(2 + 1),
    /* timing1 */
    /* tdrd=2, tebi=1, tch=1, tcs=4, treh=2, trp=4, twh=2, twp = 3*/
    NAND_TIM2_TDRD(2 + 1) | NAND_TIM2_TEBI(1 + 1) | NAND_TIM2_TCH(1 + 1) | NAND_TIM2_TCS(4 + 1) | NAND_TIM2_TRH(2 + 1) |
    NAND_TIM2_TRP(4 + 1) | NAND_TIM2_TWH(2 + 1) | NAND_TIM2_TWP(3 + 1),
    /* timing2 */
    5,		    /* ecc_mode */
    { 0, 0, 0 },	    /* reserved2 */
  },
};

/* NAND RAM0 & RAM1 buffer addresses */
static const UNS_32 nand_buff_addr[2] =
{
  NAND_RAM0_BUF, NAND_RAM1_BUF
};

/* NAND RAM0 & RAM1 decode interrupts flag */
static const UNS_32 nand_buff_dec_mask[2] =
{
  NAND_IRQ_ECC_DEC_RAM0, NAND_IRQ_ECC_DEC_RAM1
};

/* NAND RAM0 & RAM1 encode interrupts flag */
static const UNS_32 nand_buff_enc_mask[2] =
{
  NAND_IRQ_ECC_ENC_RAM0, NAND_IRQ_ECC_ENC_RAM1
};

/* NAND RAM0 & RAM1 data write interrupts flag */
static const UNS_32 nand_buff_wr_mask[2] =
{
  NAND_IRQ_WR_RAM0, NAND_IRQ_WR_RAM1
};

/***********************************************************************
 * NAND driver private functions
 **********************************************************************/
/***********************************************************************
 *
 * Function: nand_sel_device
 *
 * Purpose: Select the specified NAND device
 *
 * Processing:
 *     If init is not TRUE, then return _ERROR as chip was not
 *     initialised. Otherwise, set the NAND config parameters in HW.
 *
 * Parameters:
 *     pdev:  Pointer to NAND controller config structure
 *
 * Outputs: None
 *
 * Returns: The status of select operation
 *
 * Notes: None
 *
 **********************************************************************/
static STATUS nand_sel_device(NAND_DEV_CFG_T* pdev)
{
  STATUS ret = _ERROR;
  NAND_FLASH_CTRL_REGS_T *pregs = g_nand_drv.regptr;
  UNS_32 config = NAND_CFG_DC;

  /* check whether the correspondig chip select is initialized */
  if (pdev->init == TRUE)
  {
    /* set timings per selected device */
    pregs->timing1 = pdev->param->timing1;
    pregs->timing2 = pdev->param->timing2;

    /* build config value for the device */
    /* set ecc type */
    switch (pdev->param->ecc_mode)
    {
      case 8:
        config |= NAND_CFG_8BIT_ECC;
      case 5:
        config |= NAND_CFG_EC;
        break;
      default:
        break;
    }
    /* set width */
    if (pdev->param->interface_width == 16)
      config |= NAND_CFG_WD;

    /* set page mode - 512 bytes mode or 516 bytes mode */
    if (pdev->page_mode)
      config |= NAND_CFG_M;

    /* write the final config value */
    pregs->config = config;

    /* select bank0 */
    pregs->set_ce = NAND_SETCE_WP | NAND_SETCE_CV(pdev->cs);

    /* finaly update the current dev pointer */
    g_nand_drv.cur_dev = pdev;

    ret = _NO_ERROR;
  }

  return ret;
}

/***********************************************************************
 *
 * Function: nand_check_ids
 *
 * Purpose: Check the MAF, DEV IDs & get Device Information
 *
 * Processing:
 *     Read the MAF & DEV IDs from the chip.
 *     Check if read MAF & DEV IDs are valid by comparing with IDs
 *		 with device info structure. If IDs are matching, get the
 *		 corresponding device info structure
 *
 * Parameters:
 *     pdev: Pointer to NAND controller config structure
 *     ids:  Pointer to IDs buffer
 *
 * Outputs: None
 *
 * Returns: The status of Check operation
 *
 * Notes: None
 *
 **********************************************************************/
static STATUS nand_check_ids(NAND_DEV_CFG_T* pdev, UNS_8 *ids)
{
  int num_devs, id;
  STATUS ret = _NO_ERROR;

  /* Get Dev Info structure */
  num_devs = sizeof(chips) / sizeof(NAND_BOOT_CFG_PARAMS_T);
  for (id = 0; id < num_devs; id++)
  {
    if ((chips[id].device_name[0] == ids[0]) &&
        (chips[id].device_name[1] == ids[1]))
    {
      /* Overwrite Boot config info, if device found */
      pdev->param = (NAND_BOOT_CFG_PARAMS_T *)(&chips[id]);
      break;
    }
  }

  /* If Device not found, return with error */
  if (id == num_devs)
  {
    ret = _ERROR;
  }

  return ret;
}

/***********************************************************************
 *
 * Function: nand_setbb_array_bit
 *
 * Purpose: Mark a block as bad in memory based BBT
 *
 * Processing:
 *     Clear the corresponding bit for the block in BBT.
 *
 * Parameters:
 *     pdev: Pointer to NAND controller config structure
 *     block: Block number
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
static void nand_setbb_array_bit(NAND_DEV_CFG_T *pdev, UNS_32 block)
{
  UNS_32 idx = block / 32;
  UNS_32 offs = 1 << (block - (idx * 32));

  pdev->nand_bbt[idx] &= ~offs;
}

/***********************************************************************
 *
 * Function: nand_checkbb_array_bit
 *
 * Purpose: Check if a block is good in memory based BBT
 *
 * Processing:
 *     If the bit is 0, corresponding block is bad.
 *     If the bit is !0, corresponding block is good.
 *
 * Parameters:
 *     pdev: Pointer to NAND controller config structure
 *     block: Block number
 *
 * Outputs: None
 *
 * Returns: Status of the block
 *
 * Notes: None
 *
 **********************************************************************/
static int nand_checkbb_array_bit(NAND_DEV_CFG_T *pdev, UNS_32 block)
{
  UNS_32 idx = block / 32;
  UNS_32 offs = 1 << (block - (idx * 32));

  /* Returns '!0' if the block is good */
  return (int)(pdev->nand_bbt[idx] & offs);
}

/***********************************************************************
 *
 * Function: nand_send_addr
 *
 * Purpose: Send address cycles to the NAND Flash chip
 *
 * Processing:
 *     Send address cycles to the chip.
 *
 * Parameters:
 *     pdev: Pointer to NAND controller config structure
 *     block_num: Block address
 *     page_num: Page address
 *     column: Column address within page
 *
 * Outputs: None
 *
 * Returns: Status of the operation
 *
 * Notes: None
 *
 **********************************************************************/
static STATUS nand_send_addr(NAND_DEV_CFG_T *pdev,
                             UNS_32 block_num, UNS_32 page_num, UNS_32 column)
{
  STATUS status = _NO_ERROR;
  NAND_FLASH_CTRL_REGS_T *pregs = g_nand_drv.regptr;

  /* Send Column address 0 */
  pregs->set_addr = (column & 0xFF);

  /* Send column & page addresses */
  if (pdev->param->page_size_in_bytes > SMALL_BLOCK_PAGE_MAIN_AREA_SIZE)
  {
    /* LPF addressing */
    /* Second column address */
    pregs->set_addr = ((column >> 8) & 0xFF);

    /* Page address 0 */
    pregs->set_addr = (((page_num >> 0) & 0x003F) |
                       ((block_num << 6) & 0x00C0));

    /* Page address 1 */
    pregs->set_addr = ((block_num >> 2) & 0x00FF);

    if (pdev->param->amount_of_address_bytes == 5)
    {
      /* Page address 2 */
      pregs->set_addr = ((block_num >> 10) & 0x0003);
    }
  }
  else
  {
    /* SPF addressing */
    /* Page address 0 */
    pregs->set_addr = ((block_num << 5) & 0xE0) |
                      (page_num & 0x1F);

    /* Page address 1 */
    pregs->set_addr = ((block_num << 5) >> 8) & 0xFF;

    if (pdev->param->amount_of_address_bytes == 4)
    {
      /* Page address 2 */
      pregs->set_addr = ((block_num << 5) >> 8) & 0xFF;
    }
  }

  return status;
}

/***********************************************************************
 *
 * Function: nand_get_status
 *
 * Purpose: Read NAND Flash chip status from the chip
 *
 * Processing:
 *     Send READ STATUS command to chip, get status information.
 *
 * Parameters:
 *     status: status value
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
static UNS_8 nand_get_status(void)
{
  UNS_8 status;
  NAND_FLASH_CTRL_REGS_T *pregs = g_nand_drv.regptr;

  /* Read status from chip */
  pregs->set_cmd = 	NAND_CMD_STATUS;
  status = (UNS_8) pregs->read_data;
  return status;
}

/***********************************************************************
 *
 * Function: nand_ram_read
 *
 * Purpose: Set the flow control to read from NAND SRAM buffer
 *
 * Processing:
 *     Set the flow control to read from corrresponding NAND SRAM buffer.
 *
 * Parameters:
 *     bufnum: SRAM bufffer number
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
static void nand_ram_read(int bufnum)
{
  NAND_FLASH_CTRL_REGS_T *pregs = g_nand_drv.regptr;

  if (bufnum == 0)
  {
    /* Use RAM buffer 0 */
    pregs->control_flow = NAND_CTRL_RD_RAM0;
  }
  else
  {
    /* Use RAM buffer 1 */
    pregs->control_flow = NAND_CTRL_RD_RAM1;
  }

  /* Enable corresponding interrupt */
  pregs->irq_mask &= ~nand_buff_dec_mask[bufnum];
}

/***********************************************************************
 *
 * Function: nand_ram_write
 *
 * Purpose: Set the flow control to write into NAND SRAM buffer
 *
 * Processing:
 *     Set the flow control to write into the corrresponding NAND SRAM buffer.
 *
 * Parameters:
 *     bufnum: SRAM bufffer number
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
static void nand_ram_write(int bufnum)
{
  NAND_FLASH_CTRL_REGS_T *pregs = g_nand_drv.regptr;

  if (bufnum == 0)
  {
    /* Use RAM buffer 0 */
    pregs->control_flow = NAND_CTRL_WR_RAM0;
  }
  else
  {
    /* Use RAM buffer 1 */
    pregs->control_flow = NAND_CTRL_WR_RAM1;
  }

  /* Enable corresponding interrupt */
  pregs->irq_mask &= ~nand_buff_wr_mask[bufnum];
}

/***********************************************************************
 *
 * Function: nand_create_dma_sglist
 *
 * Purpose: Create DMA Scatter Gather list for DMA transfer
 *
 * Processing:
 *     Populate the DMA descriptors for the data transfer.
 *
 * Parameters:
 *     pdev: Pointer to NAND controller config structure
 *     pblockpage: Pointer to Block page structure
 *		 chn: DMA channel number
 *		 read: Read operation (1 : Read, 0: Write)
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
static void nand_create_dma_sglist(NAND_DEV_CFG_T *pdev,
                                   NAND_BLOCKPAGE_T *pblockpage, UNS_32 chn, UNS_32 read)
{
  int bufrdy = 0;
  UNS_32 index = 0;
  UNS_32 buf = lpc31xx_va_to_pa(pblockpage->buffer);
  UNS_32 next_entry = lpc31xx_va_to_pa(&sg_ll[0]);
  DMAC_REGS_T *pdma_regs = dma_get_base();

  /* Create SG entries to transfer pay load */
  g_nand_drv.steps = pdev->param->page_size_in_bytes / NAND_BUF_SIZE;
  for (index = 0; index < g_nand_drv.steps; index++)
  {
    /* Entry to read Main page data */
    sg_ll[index].setup.src_address = (read ? nand_buff_addr[bufrdy] :
                                      buf);
    sg_ll[index].setup.dest_address = (read ? buf :
                                       nand_buff_addr[bufrdy]);
    sg_ll[index].setup.trans_length = (NAND_BUF_SIZE >> 2) - 1;
    sg_ll[index].setup.cfg = DMA_CFG_CMP_CH_EN |
                             DMA_CFG_CMP_CH_NR(chn) | DMA_CFG_TX_WORD;
    next_entry = next_entry + sizeof(NAND_DMA_SG_LL_T);
    sg_ll[index].next_entry = next_entry;

    bufrdy ^= 1;
    buf += NAND_BUF_SIZE;
  }

  /* For DMA write, for first decsriptor,
  	 disable companion channel */
  if (!read)
  {
    sg_ll[0].setup.cfg =
      DMA_CFG_CMP_CH_NR(chn) | DMA_CFG_TX_WORD;
  }

  /* Last descriptor to generate SOFT interrupt */
  sg_ll[index].setup.src_address = lpc31xx_va_to_pa(pblockpage->buffer);
  sg_ll[index].setup.dest_address = (UNS_32) & pdma_regs->soft_int;
  sg_ll[index].setup.trans_length = 0;
  sg_ll[index].setup.cfg = DMA_CFG_TX_WORD;
  sg_ll[index].next_entry = (UNS_32)NULL;

  /* Cache coherence */
  lpc31xx_flush_dcache_range((UNS_32 *)sg_ll, (UNS_32 *)&sg_ll[index+1]);

  return;
}

/***********************************************************************
 *
 * Function: nand_dma_callback
 *
 * Purpose: DMA callback function for NAND
 *
 * Processing:
 *     Dummy function.
 *
 * Parameters:
 *		 None.
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
void nand_dma_callback(void)
{
  return;
}

/***********************************************************************
 *
 * Function: nand_interrupt
 *
 * Purpose: NAND ISR function
 *
 * Processing:
 *     For Non-DMA transfer, when NAND interrupts occur,
 *		 set the completed flag to true & disable the interrupts.
 *		 For DMA transfer, when NAND interrupts happen,
 *		 clear the interrupt & disable the interrupt. Enable the
 *	 	 next interrupt.
 *
 * Parameters:
 *		 None.
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
void nand_interrupt(void)
{
  NAND_FLASH_CTRL_REGS_T *pregs = g_nand_drv.regptr;
  DMAC_REGS_T *pdma_regs = dma_get_base();

  if (g_nand_drv.dma == FALSE)
  {
    /* Set done flag */
    g_nand_drv.tfr_done = pregs->irq_status;

    /* Disable NAND interrupts */
    pregs->irq_mask |= ~0;
  }
  else
  {
    if (pregs->irq_status & NAND_IRQ_ECC_DEC_RAM0)
    {
      /* Clear RAM0 decoded interrupt */
      pregs->irq_status |= NAND_IRQ_ECC_DEC_RAM0;

      /* Disable RAM0 decoded interupt */
      pregs->irq_mask |= NAND_IRQ_ECC_DEC_RAM0;

      if (g_nand_drv.steps)
      {
        g_nand_drv.steps--;

        /* Enable RAM1 flow control */
        nand_ram_read(1);
      }
    }
    else if (pregs->irq_status & NAND_IRQ_ECC_DEC_RAM1)
    {
      /* Clear RAM1 decoded interrupt */
      pregs->irq_status |= NAND_IRQ_ECC_DEC_RAM1;

      /* Disable RAM1 decoded interupt */
      pregs->irq_mask |= NAND_IRQ_ECC_DEC_RAM1;

      if (g_nand_drv.steps)
      {
        g_nand_drv.steps--;

        /* Enable RAM0 flow control */
        nand_ram_read(0);
      }
    }
    else if (pregs->irq_status & NAND_IRQ_ECC_ENC_RAM0)
    {
      /* Clear RAM0 encoded interrupt */
      pregs->irq_status |= NAND_IRQ_ECC_ENC_RAM0;

      /* Disable RAM0 encoded interrupt */
      pregs->irq_mask |= NAND_IRQ_ECC_ENC_RAM0;

      if (g_nand_drv.restart_dma)
      {
        /* For DMA write, after first 512 bytes are encoded,
        	 Enable external Enable Flow control & restart the DMA
        	 */
        /* Reset restart_dma flag */
        g_nand_drv.restart_dma = 0;

        /* Disable the channels 4 & 5 */
        pdma_regs->channel[NAND_DMA_CHAN].enable = 0;
        pdma_regs->channel[NAND_DMA_CHAN + 1].enable = 0;
        pdma_regs->channel[NAND_DMA_CHAN].enable = 0;

        /* Stop linked list transfer to channel 4 -
         Now external Enable flow control should take over */
        pdma_regs->channel[NAND_DMA_CHAN + 1].config =
          DMA_CFG_CMP_CH_NR(NAND_DMA_CHAN);

        /* Enable External DMA flow control - Enable for channel 4 */
        pregs->config |= NAND_CFG_DE;

        /* Restart DMA channel 5 */
        pdma_regs->channel[NAND_DMA_CHAN + 1].enable = 1;
      }

      /* Enable RAM0 Flow control - Move data to Flash */
      nand_ram_write(0);
    }
    else if (pregs->irq_status & NAND_IRQ_ECC_ENC_RAM1)
    {
      /* Clear RAM1 encoded interrupt */
      pregs->irq_status |= NAND_IRQ_ECC_ENC_RAM1;

      /* Disable RAM1 encoded interrupt */
      pregs->irq_mask |= NAND_IRQ_ECC_ENC_RAM1;

      /* Enable RAM1 Flow control - Move data to Flash */
      nand_ram_write(1);
    }
    else if (pregs->irq_status & NAND_IRQ_WR_RAM0)
    {
      /* Clear RAM0 page write done interrupt */
      pregs->irq_status |= NAND_IRQ_WR_RAM0;

      /* Disable RAM0 page write done interrupt */
      pregs->irq_mask |= NAND_IRQ_WR_RAM0;

      /* Enable RAM1 encoded interrupt */
      pregs->irq_mask &= ~NAND_IRQ_ECC_ENC_RAM1;
    }
    else if (pregs->irq_status & NAND_IRQ_WR_RAM1)
    {
      /* Clear RAM1 page write done interrupt */
      pregs->irq_status |= NAND_IRQ_WR_RAM1;

      /* Disable RAM1 page write done interrupt */
      pregs->irq_mask |= NAND_IRQ_WR_RAM1;

      /* Enable RAM0 encoded interrupt */
      pregs->irq_mask &= ~NAND_IRQ_ECC_ENC_RAM0;
    }
  }
}

/***********************************************************************
 *
 * Function: nand_read_page_dma
 *
 * Purpose: Read page data using DMA
 *
 * Processing:
 *     Read page data using the DMA.
 *
 * Parameters:
 *     pdev: Pointer to NAND controller config structure
 *		 pblockpage: Pointer to block page structure
 *
 * Outputs: None
 *
 * Returns: Status of Read operation
 *
 * Notes: None
 *
 **********************************************************************/
static STATUS nand_read_page_dma(NAND_DEV_CFG_T *pdev,
                                 NAND_BLOCKPAGE_T *pblockpage)
{
  INT_32 dma_chan;
  volatile UNS_32 tmp;
  STATUS status = _NO_ERROR;
  NAND_FLASH_CTRL_REGS_T *pregs = g_nand_drv.regptr;
  DMAC_REGS_T *pdma_regs = dma_get_base();

  /* Allocate DMA channel */
  dma_chan = dma_alloc_sg_channel(NAND_DMA_CHAN, (PF_DMACB)nand_dma_callback,
                                  NULL, NULL, NULL, FALSE);
  if (dma_chan < 0)
  {
    status = _ERROR;
    return status;
  }

  /* Set up DMA SG list */
  nand_create_dma_sglist(pdev, pblockpage, dma_chan, 1);

  /* Program channel 5 to transfer SG list to companion channel */
  pdma_regs->channel[dma_chan].source = lpc31xx_va_to_pa(sg_ll);
  pdma_regs->channel[dma_chan].destination =
    (UNS_32)(&pdma_regs->altchannel[dma_chan - 1]);
  pdma_regs->channel[dma_chan].length = 0x4;
  pdma_regs->channel[dma_chan].config =
    DMA_CFG_CMP_CH_NR(dma_chan - 1);

  /* Clear DMA interrupts */
  pdma_regs->irq_status_clear = 0xFFFFFFFF;

  /* Clear NAND interrupts */
  pregs->irq_status_raw = 0xFFFFFFFF;

  /* Send Read1 Command */
  pregs->set_cmd = NAND_CMD_READ1;

  /* Send address cycles */
  status = nand_send_addr(pdev, pblockpage->block_num,
                          pblockpage->page_num, 0);
  if (status == _ERROR)
  {
    /* Error in sending the address cycles */
    dma_free_sg_channel(dma_chan);
    return status;
  }

  /* Send Read Start command, if LPF */
  if (pdev->param->page_size_in_bytes > SMALL_BLOCK_PAGE_MAIN_AREA_SIZE)
    pregs->set_cmd = NAND_CMD_READSTART;

  /* Wait for Page load to Complete */
  timer_wait_us(NULL, NAND_PAGE_LOAD_TIME);

  /* Wait for ready status */
  while (!((tmp = pregs->check_sts) &
           NAND_CHK_STS_RB_LVL(pdev->cs)));
  //NAND_CHK_STS_RB_EDGE(pdev->cs)));

  /* Enable External DMA - Enable channel 4 */
  pregs->config |= NAND_CFG_DE;

  /* Enable DMA channel 5 */
  pdma_regs->channel[dma_chan].enable = 1;

  /* Start read into RAM0 */
  nand_ram_read(0);

  /* Wait for the DMA channel 5 - done interrupt */
  while (!(pdma_regs->irq_status_clear & ((DMA_IRQ_DONE(dma_chan))  |
                                          (DMA_IRQ_ABORT))));

  /* Check status */
  if (pdma_regs->irq_status_clear & DMA_IRQ_ABORT)
  {
    pdma_regs->irq_status_clear = DMA_IRQ_ABORT;
    status = _ERROR;
    goto out2;
  }

  /* Clear channel 5 interrupt */
  pdma_regs->irq_status_clear = DMA_IRQ_DONE(dma_chan);

  /* Wait for the DMA channel 4 - soft interrupt */
  while (!(pdma_regs->irq_status_clear & DMA_IRQ_SOFT));

  /* Cache coherence */
  lpc31xx_invalidate_dcache_range((UNS_32 *)pblockpage->buffer,
                                  (UNS_32 *)(pblockpage->buffer + pdev->param->page_size_in_bytes));

out2:
  /* Clear DMA interrupts */
  pdma_regs->irq_status_clear = 0xFFFFFFFF;

  /* Disbale DE feature */
  pregs->config &= ~NAND_CFG_DE;
  pdma_regs->channel[dma_chan].enable = 0;

  /* Disable interrupts */
  pregs->irq_mask |= ~(0);

  /* Free DMA channels */
  dma_free_sg_channel(dma_chan);

  return status;
}

/***********************************************************************
 *
 * Function: nand_read_page_memcpy
 *
 * Purpose: Read page data using memcpy
 *
 * Processing:
 *     Read page data using the memcpy.
 *
 * Parameters:
 *     pdev: Pointer to NAND controller config structure
 *		 pblockpage: Pointer to block page structure
 *
 * Outputs: None
 *
 * Returns: Status of Read operation
 *
 * Notes: None
 *
 **********************************************************************/
static STATUS nand_read_page_memcpy(NAND_DEV_CFG_T *pdev,
                                    NAND_BLOCKPAGE_T *pblockpage)
{
  volatile UNS_32 tmp;
  STATUS status = _NO_ERROR;
  NAND_FLASH_CTRL_REGS_T *pregs = g_nand_drv.regptr;
  int i, curbuf = 0, bufrdy = -1;
  int size = NAND_BUF_SIZE;
  int steps = pdev->param->page_size_in_bytes / size;
  UNS_8 *p = pblockpage->buffer;
  UNS_8 *oob = pblockpage->oobbuf;

  /* Disable Ext Enable Flow control */
  pregs->config &= ~NAND_CFG_DE;

  /* Clear NAND interrupts */
  pregs->irq_status_raw = 0xFFFFFFFF;

  /* Send Read1 Command */
  pregs->set_cmd = 	NAND_CMD_READ1;

  /* Send address cycles */
  status = nand_send_addr(pdev, pblockpage->block_num,
                          pblockpage->page_num, 0);
  if (status == _ERROR)
  {
    /* Error in sending the address cycles */
    return status;
  }

  /* Send Read Start command, if LPF */
  if (pdev->param->page_size_in_bytes > SMALL_BLOCK_PAGE_MAIN_AREA_SIZE)
    pregs->set_cmd = 	NAND_CMD_READSTART;

  /* Wait for Page load to Complete */
  timer_wait_us(NULL, NAND_PAGE_LOAD_TIME);

  /* Wait for ready status */
  while (!((tmp = pregs->check_sts) &
           NAND_CHK_STS_RB_EDGE(pdev->cs)));

  /* Read data from SRAM - use both SRAM buffers */
  for (i = steps; i > 0; i--)
  {
    /* Clear all current statuses */
    pregs->irq_status_raw = 0xFFFFFFFF;

    /* Set Pending Flag */
    g_nand_drv.tfr_done = 0;

    /* Start read into RAM0 or RAM1 */
    nand_ram_read(curbuf);

    /* Read current buffer while next buffer is loading */
    if (bufrdy >= 0)
    {
      /* Read payload portion of the transfer */
      memcpy((void *)p, (void *)nand_buff_addr[bufrdy], size);
      p += size;

      /* If OOB buffer present, read OOB data */
      if (oob)
      {
        memcpy((void *)oob, (void *)(nand_buff_addr[bufrdy] + size), NAND_OOB_BYTES);
        oob += NAND_OOB_BYTES;
      }
    }

    /* Interrupt based wait operation */
    while (!(g_nand_drv.tfr_done));

    /* Use Next Buffer */
    bufrdy = curbuf;
    curbuf = 1 - curbuf;

    /* Check the ECC status, set error status if ECC failure */
    if (pdev->param->ecc_mode == 5)
    {
      if (pregs->irq_status_raw & (NAND_IRQ_ERR_UNR_RAM0 | NAND_IRQ_ERR_UNR_RAM1))
      {
        /* ECC uncorrecatble error */
        status = _ERROR;
      }
    }
  }

  /* Read payload portion of the transfer */
  memcpy((void *)p, (void *)nand_buff_addr[bufrdy], size);

  /* Read OOB data portion of the transfer */
  if (oob)
    memcpy((void *)oob, (void *)(nand_buff_addr[bufrdy] + size), NAND_OOB_BYTES);

  /* Clear pending interrupts */
  pregs->irq_status_raw = 0xFFFFFFFF;

  /* Disable interrupts */
  pregs->irq_mask |= ~(0);

  return status;
}

/***********************************************************************
 *
 * Function: nand_read_page
 *
 * Purpose: Read page data
 *
 * Processing:
 *     Read page data. If DMA read request & chip is LPF &
 *		 no OB data, then use DMA.
 *	 	 Else use the memcpy operation to read page data.
 *
 * Parameters:
 *     pdev: Pointer to NAND controller config structure
 *		 pblockpage: Pointer to block page structure
 *
 * Outputs: None
 *
 * Returns: Status of Read operation
 *
 * Notes: None
 *
 **********************************************************************/
static STATUS nand_read_page(NAND_DEV_CFG_T *pdev,
                             NAND_BLOCKPAGE_T *pblockpage)
{
  STATUS status = _NO_ERROR;

  /* Check if block num & page num are valid */
  if ((pblockpage->block_num >= pdev->param->nbr_of_blocks) ||
      (pblockpage->page_num >= pdev->param->pages_per_block))
  {
    /* Invalid Block/page numbers */
    return _ERROR;
  }

  /* If bad block return with error */
  if (pdev->block0_ok == TRUE)
  {
    if (!(nand_checkbb_array_bit(pdev, pblockpage->block_num)))
    {
      /* Block is bad */
      return _ERROR;
    }
  }

  /* Check if device is already selected */
  if (pdev != g_nand_drv.cur_dev)
  {
    /* If not selected, select the device */
    if (nand_sel_device(pdev) != _NO_ERROR)
    {
      /* Error in selecting the device */
      status = _ERROR;
    }
  }

  /* Read data */
  if (status == _NO_ERROR)
  {
    /* If DMA is enabled, check it is LPF.
       If LPF, use DMA (External Enable DMA feature can be used
    	 only for LPF chips)
     */
    if ((pblockpage->dma == TRUE) &&
        (pdev->param->page_size_in_bytes > NAND_BUF_SIZE) &&
        (pblockpage->oobbuf == NULL))
    {
      /* Use DMA for reading */
      g_nand_drv.dma = TRUE;
      status = nand_read_page_dma(pdev, pblockpage);
    }
    else
    {
      /* Non-DMA for reading */
      g_nand_drv.dma = FALSE;
      status = nand_read_page_memcpy(pdev, pblockpage);
    }
  }

  return status;
}

/***********************************************************************
 *
 * Function: nand_write_page_dma
 *
 * Purpose: Write page data using DMA
 *
 * Processing:
 *     Write page data using the DMA.
 *
 * Parameters:
 *     pdev: Pointer to NAND controller config structure
 *		 pblockpage: Pointer to block page structure
 *
 * Outputs: None
 *
 * Returns: Status of Write operation
 *
 * Notes: None
 *
 **********************************************************************/
static STATUS nand_write_page_dma(NAND_DEV_CFG_T *pdev,
                                  NAND_BLOCKPAGE_T *pblockpage)
{
  volatile UNS_32 tmp;
  INT_32 dma_chan;
  STATUS status = _NO_ERROR;
  NAND_FLASH_CTRL_REGS_T *pregs = g_nand_drv.regptr;
  DMAC_REGS_T *pdma_regs = dma_get_base();

  /* Cache coherence */
  lpc31xx_flush_dcache_range((UNS_32 *)pblockpage->buffer,
                             (UNS_32 *)(pblockpage->buffer + pdev->param->page_size_in_bytes));

  /* For DMA, start encoding after 512 bytes of data */
  pregs->config |= NAND_CFG_M;

  /* Allocate DMA channel */
  dma_chan = dma_alloc_sg_channel(NAND_DMA_CHAN, (PF_DMACB)nand_dma_callback, NULL,
                                  NULL, NULL, FALSE);
  if (dma_chan < 0)
  {
    status = _ERROR;
    return status;
  }

  /* Set up DMA SG list */
  nand_create_dma_sglist(pdev, pblockpage, dma_chan, 0);

  /* Program channel to transfer SG list to companion channel */
  pdma_regs->channel[dma_chan].source = lpc31xx_va_to_pa(sg_ll);
  pdma_regs->channel[dma_chan].destination =
    (UNS_32)(&pdma_regs->altchannel[dma_chan-1]);
  pdma_regs->channel[dma_chan].length = 0x4;
  /* Enable companion channel for first descriptor only */
  pdma_regs->channel[dma_chan].config = DMA_CFG_CMP_CH_EN |
                                        DMA_CFG_CMP_CH_NR(dma_chan - 1);

  /* Send SEQIN command */
  pregs->set_cmd = 	NAND_CMD_SDIN;

  /* Send address cycles */
  status = nand_send_addr(pdev, pblockpage->block_num,
                          pblockpage->page_num, 0);
  if (status == _ERROR)
  {
    /* Error in sending the address cycles */
    dma_free_sg_channel(dma_chan);
    return status;
  }

  /* Clear DMA interrupts */
  pdma_regs->irq_status_clear = 0xFFFFFFFF;

  /* Clear NAND interrupts */
  pregs->irq_status_raw = 0xFFFFFFFF;

  /* Enable RAM0 Encode interrupt */
  pregs->irq_mask &= ~(NAND_IRQ_ECC_ENC_RAM0);

  /* For first time, restart DMA in ISR */
  g_nand_drv.restart_dma = 1;

  /* Enable DMA channel 5 */
  pdma_regs->channel[dma_chan].enable = 1;

  /* Wait for the DMA done interrupt */
  while (!(pdma_regs->irq_status_clear & ((DMA_IRQ_DONE(dma_chan))  |
                                          (DMA_IRQ_ABORT))));

  /* Check status */
  if (pdma_regs->irq_status_clear & DMA_IRQ_ABORT)
  {
    status = _ERROR;
    pdma_regs->irq_status_clear = 0xFFFFFFFF;
    goto out1;
  }

  /* Clear channel 5 interrupt */
  pdma_regs->irq_status_clear = DMA_IRQ_DONE(dma_chan);

  /* Wait for DMA channel 5 - soft int */
  while (!(pdma_regs->irq_status_clear & DMA_IRQ_SOFT));

  /* Send Program command */
  pregs->set_cmd = 	NAND_CMD_PAGEPROG;

  /* Wait for Page Program to Complete */
  timer_wait_us(NULL, NAND_PAGE_PROG_TIME);

  /* Wait till program is completed */
  while (!((tmp = pregs->check_sts) &
           NAND_CHK_STS_RB_EDGE(pdev->cs)));

  /* Read Program operation status from chip */
  pregs->set_cmd = 	NAND_CMD_STATUS;
  if (pregs->read_data & NAND_FLASH_FAILED)
  {
    status = _ERROR;
  }

out1:
  /* Disable NAND interrupts */
  pregs->irq_mask |= ~(0);

  /* DMA int clear */
  pdma_regs->irq_status_clear = 0xFFFFFFFF;

  /* Disbale DMA chanel 5 & DE feature */
  pdma_regs->channel[dma_chan].enable = 0;
  pregs->config &= ~(NAND_CFG_DE | NAND_CFG_M);

  /* Free DMA channels */
  dma_free_sg_channel(dma_chan);

  return status;
}

/***********************************************************************
 *
 * Function: nand_write_page_memcpy
 *
 * Purpose: Write page data using memcpy
 *
 * Processing:
 *     Write page data using the memcpy.
 *
 * Parameters:
 *     pdev: Pointer to NAND controller config structure
 *		 pblockpage: Pointer to block page structure
 *
 * Outputs: None
 *
 * Returns: Status of Write operation
 *
 * Notes: None
 *
 **********************************************************************/
static STATUS nand_write_page_memcpy(NAND_DEV_CFG_T *pdev,
                                     NAND_BLOCKPAGE_T *pblockpage)
{
  int i;
  volatile UNS_32 tmp;
  STATUS status = _NO_ERROR;
  NAND_FLASH_CTRL_REGS_T *pregs = g_nand_drv.regptr;
  int curbuf = 0, bufrdy = 0;
  int size = NAND_BUF_SIZE;
  int steps = pdev->param->page_size_in_bytes / size;
  UNS_8 *p = pblockpage->buffer;
  UNS_8 *oob = pblockpage->oobbuf;

  /* Disable Ext Enable Flow control */
  pregs->config &= ~NAND_CFG_DE;

  /* Clear NAND interrupts */
  pregs->irq_status_raw = 0xFFFFFFFF;

  /* Reset NAND data trasnfer complete flag */
  g_nand_drv.tfr_done = 0;

  /* If OOB data is present, enable 516 bytes mode */
  if (oob)
  {
    pregs->config &= ~NAND_CFG_M;
  }
  else
  {
    /* Enable 512 bytes mode */
    pregs->config |= NAND_CFG_M;
  }

  /* Send SEQIN command */
  pregs->set_cmd = 	NAND_CMD_SDIN;

  /* Send address cycles */
  status = nand_send_addr(pdev, pblockpage->block_num,
                          pblockpage->page_num, 0);
  if (status == _ERROR)
  {
    /* Error in sending the addresses to chip */
    return status;
  }

  /* Write data into SRAM0 */
  memcpy((void *) nand_buff_addr[bufrdy], p, size);
  p += size;
  if (oob)
  {
    memcpy((void *)(nand_buff_addr[bufrdy] + size), oob,
           NAND_OOB_FREE_SIZE);
    oob += NAND_OOB_BYTES;
  }

  /* Wait for ECC Encoding interrupt */
  while (!(pregs->irq_status_raw & nand_buff_enc_mask[bufrdy]));

  /* Write other data */
  for (i = steps; i > 0; i--)
  {
    /* Buffer management */
    curbuf = bufrdy;
    bufrdy = 1 - bufrdy;

    /* Start the transfer to the device */
    g_nand_drv.tfr_done = 0;

    /* Clear NAND interrupts */
    pregs->irq_status_raw = 0xFFFFFFFF;

    /* Write data to next buffer */
    nand_ram_write(curbuf);

    /* Copy next payload and OOB data to the buffer while current
       buffer is transferring */
    if (i > 1)
    {
      memcpy((void *) nand_buff_addr[bufrdy], p, size);
      p += size;
      if (oob)
      {
        memcpy((void *)(nand_buff_addr[bufrdy] + size), oob,
               NAND_OOB_FREE_SIZE);
        oob += NAND_OOB_BYTES;
      }

      /* Wait for ECC Encoding of SRAM */
      while (!(pregs->irq_status_raw & nand_buff_enc_mask[bufrdy]));
    }

    /* Polling for buffer loaded & Encoded */
    while (!(g_nand_drv.tfr_done));
  }

  /* Clear pending interrupts */
  pregs->irq_status_raw = 0xFFFFFFFF;

  /* Send Program command */
  pregs->set_cmd = 	NAND_CMD_PAGEPROG;

  /* Wait for Page Program to Complete */
  timer_wait_us(NULL, NAND_PAGE_PROG_TIME);

  /* Wait till program is completed */
  while (!((tmp = pregs->check_sts) &
           NAND_CHK_STS_RB_EDGE(pdev->cs)));

  /* Read Program operation status from chip */
  pregs->set_cmd = 	NAND_CMD_STATUS;
  if (pregs->read_data & NAND_FLASH_FAILED)
  {
    status = _ERROR;
  }

  /* Clear pending interrupts */
  pregs->irq_status_raw = 0xFFFFFFFF;

  /* Disable NAND interrupts */
  pregs->irq_mask |= ~(0);

  return status;
}

/***********************************************************************
 *
 * Function: nand_write_page
 *
 * Purpose: Write page data
 *
 * Processing:
 *     Write page data. If DMA write request & chip is LPF, use DMA.
 *	 	 Else use the memcpy operation to write page data.
 *
 * Parameters:
 *     pdev: Pointer to NAND controller config structure
 *		 pblockpage: Pointer to block page structure
 *
 * Outputs: None
 *
 * Returns: Status of Write operation
 *
 * Notes: None
 *
 **********************************************************************/
static STATUS nand_write_page(NAND_DEV_CFG_T *pdev,
                              NAND_BLOCKPAGE_T *pblockpage)
{
  STATUS status = _NO_ERROR;

  /* Check if block num & page num are valid */
  if ((pblockpage->block_num >= pdev->param->nbr_of_blocks) ||
      (pblockpage->page_num >= pdev->param->pages_per_block))
  {
    /* Invalid Block/page number */
    return _ERROR;
  }

  /* If bad block return with error */
  if (pdev->block0_ok == TRUE)
  {
    if (!(nand_checkbb_array_bit(pdev, pblockpage->block_num)))
    {
      /* Block is bad */
      return _ERROR;
    }
  }

  /* Check if device is selected */
  if (pdev != g_nand_drv.cur_dev)
  {
    /* If not selected, select the device */
    if (nand_sel_device(pdev) != _NO_ERROR)
    {
      /* Error in selecting the device */
      status = _ERROR;
    }
  }

  /* Read data */
  if (status == _NO_ERROR)
  {
    /* If DMA is enabled, check it is LPF.
       If LPF, use DMA (External Enable DMA feature can be used
    	 only for LPF chips)
     */
    if ((pblockpage->dma == TRUE) &&
        (pdev->param->page_size_in_bytes > NAND_BUF_SIZE) &&
        (pblockpage->oobbuf == NULL))
    {
      /* Use DMA for writing */
      g_nand_drv.dma = TRUE;
      status = nand_write_page_dma(pdev, pblockpage);
    }
    else
    {
      /* Non-DMA for writing */
      g_nand_drv.dma = FALSE;
      status = nand_write_page_memcpy(pdev, pblockpage);
    }
  }

  return status;
}

/***********************************************************************
 *
 * Function: nand_block0_ok
 *
 * Purpose: Check if block0 has BBT
 *
 * Processing:
 *     If Block 0 does not contains Bad Block Info & Boot Cfg info,
 *		 return with 0. If it has Bad Block Info, create memory based BBT.
 *
 * Parameters:
 *     pdev: Pointer to NAND controller config structure
 *
 * Outputs: None
 *
 * Returns: 0 - Info not found, !0 - if found
 *
 * Notes: None
 *
 **********************************************************************/
static int nand_block0_ok(NAND_DEV_CFG_T *pdev)
{
  UNS_8	tag[9];
  //STATUS status;
  UNS_32 val;
  NAND_BLOCKPAGE_T blkpage;
  UNS_32 idx = 0xFFFFFFFF, page = 0;
  int offs = 0, entries;
  BOOL_32	tag_found = FALSE, bbt_found = FALSE;

  /* Read block 0, page 0, offset 0 */
  blkpage.dma = FALSE;
  blkpage.block_num = 0;
  blkpage.page_num = 0;
  blkpage.buffer = (UNS_8 *)data_buf;
  blkpage.oobbuf = (UNS_8 *)(data_buf) + pdev->param->page_size_in_bytes;

  /* Check if tag is available */
  while (page < pdev->param->pages_per_block)
  {
    //status = nand_read_page(pdev, &blkpage); /* Ignore Status */
    nand_read_page(pdev, &blkpage); /* Ignore Status */
    memcpy((void *)tag, (void *)data_buf, 8 * sizeof(UNS_8));
    tag[8] = '\0';
    if (strcmp((char *)tag, "NANDflsh") == 0)
    {
      tag_found = TRUE;
      break;
    }

    /* Find in 0, 16, 32, .. pages */
    page += 16;
  }

  if (tag_found == FALSE)
  {
    /* Tag not found, return with flag */
    pdev->block0_ok = FALSE;
    return 0;
  }

  /* Reset the memory based BBT array */
  memset((void*)pdev->nand_bbt, 0xFF, sizeof(pdev->nand_bbt));

  /* Read Bad Block Info starting from page 1 block 0 */
  page += 1;
  blkpage.page_num = page;
  //status = nand_read_page(pdev, &blkpage);
  nand_read_page(pdev, &blkpage);

  /* Get the number of bad blocks in Bad Block List */
  idx = data_buf[offs];
  if (idx != 0xFFFFFFFF)
  {
    bbt_found = TRUE;
  }

  if (bbt_found == FALSE)
  {
    /* Something is wrong */
    pdev->block0_ok = FALSE;
    return 0;
  }

  /* Generate list from entries */
  entries = (int) idx;
  offs += 1;
  while (entries > 0)
  {
    val = data_buf[offs];
    if ((val & 0x00FFFFFF) == ((((UNS_32) 'B') << 0) |
                               (((UNS_32) 'A') << 8) |
                               (((UNS_32) 'D') << 16)))
    {
      /* Go to the next page and continue */
      page += 1 ;
      offs = 0;
      blkpage.page_num = page;
      page++;
      //status = nand_read_page(pdev, &blkpage);
      nand_read_page(pdev, &blkpage);
    }
    else
    {
      /* Update Bad Block info in RAM Table */
      nand_setbb_array_bit(pdev, val);
      offs += 1;
      entries--;
    }
  }

  pdev->block0_ok = TRUE;
  return 1;
}


/***********************************************************************
 *
 * Function: nand_read_id
 *
 * Purpose: Read NAND flash id
 *
 * Processing:
 *     If init is not TRUE, then return _ERROR as mlc was not
 *     previously opened. Otherwise, read NAND flash id.
 *
 * Parameters:
 *     devid: Pointer to NAND controller config structure
 *     buf:   Pointer to data buffer to copy to
 *
 * Outputs: None
 *
 * Returns: The status of read id operation
 *
 * Notes: None
 *
 **********************************************************************/
STATUS nand_read_id(INT_32 devid, void *buf)
{
  STATUS status = _NO_ERROR;
  NAND_FLASH_CTRL_REGS_T *pregs = g_nand_drv.regptr;
  NAND_DEV_CFG_T* pdev = (NAND_DEV_CFG_T*)devid;
  UNS_8 *bptr = buf;
  volatile UNS_32 tmp;
  int idx;

  /* check if device is selected */
  if (pdev != g_nand_drv.cur_dev)
  {
    /* select the device */
    if (nand_sel_device(pdev) != _NO_ERROR)
    {
      status = _ERROR;
    }
  }

  if (status == _NO_ERROR)
  {
    /* Clear NAND interrupts */
    pregs->irq_status_raw = 0xFFFFFFFF;

    /* ID read (1) command */
    pregs->set_cmd = NAND_CMD_READID;

    /* Column Address */
    pregs->set_addr = 0;

    /* Wait for NAND ready */
    while (!((tmp = pregs->check_sts) &
             NAND_CHK_STS_RB_LVL(pdev->cs)));

    /* Read ID words from NAND flash data register */
    for (idx = 0; idx < 4; idx++)
    {
      bptr[idx] = (UNS_8)pregs->read_data;
    }
  }

  return status;
}

/***********************************************************************
 *
 * Function: nand_read_check_ids
 *
 * Purpose: Read NAND flash id
 *
 * Processing:
 *     If init is not TRUE, then return _ERROR as mlc was not
 *     previously opened. Otherwise, read NAND flash id.
 *
 * Parameters:
 *     devid: Pointer to NAND controller config structure
 *     buf:   Pointer to data buffer which has device IDs
 *
 * Outputs: None
 *
 * Returns: The status of read id operation
 *
 * Notes: None
 *
 **********************************************************************/
STATUS nand_read_check_ids(INT_32 devid, void *buf)
{
  STATUS status = _NO_ERROR;
  NAND_DEV_CFG_T* pdev = (NAND_DEV_CFG_T*)devid;

  /* Read DEV, MAF Ids from chip */
  status = nand_read_id(devid, buf);
  if (status == _NO_ERROR)
  {
    /* Check if ids & device info are valid */
    if (nand_check_ids(pdev, buf) == _ERROR)
    {
      status = _ERROR;
    }
  }

  return status;
}

/***********************************************************************
 *
 * Function: nand_erase_block
 *
 * Purpose: Erase NAND flash block
 *
 * Processing:
 *     If init is not TRUE, then return _ERROR as mlc was not
 *     previously opened. Otherwise, erase NAND flash block.
 *
 * Parameters:
 *     devid: Pointer to NAND controller config structure
 *     block_num: Block to be erased
 *
 * Outputs: None
 *
 * Returns: The status of block erase operation
 *
 * Notes: None
 *
 **********************************************************************/
STATUS nand_erase_block(NAND_DEV_CFG_T *pdev, UNS_32 block_num)
{
  STATUS status = _NO_ERROR;
  NAND_FLASH_CTRL_REGS_T *pregs = g_nand_drv.regptr;
  volatile UNS_32 tmp;

  /* Check if it is valid block number */
  if (block_num >= pdev->param->nbr_of_blocks)
  {
    /* Block 0 is protected, can not be erased */
    return _ERROR;
  }

  /* If bad block return with error */
  if (pdev->block0_ok == TRUE)
  {
    if (!(nand_checkbb_array_bit(pdev, block_num)))
    {
      /* Block 0 is protected, can not be erased */
      return _ERROR;
    }
  }

  /* Check if block 0 & block 0 is protected */
  if ((pdev->block0_prot == TRUE) && (block_num == 0))
  {
    /* Block 0 is protected, can not be erased */
    return _ERROR;
  }

  /* check if device is selected */
  if (pdev != g_nand_drv.cur_dev)
  {
    /* select the device */
    if (nand_sel_device(pdev) != _NO_ERROR)
    {
      status = _ERROR;
    }
  }

  if (status == _NO_ERROR)
  {
    /* Clear NAND interrupts */
    pregs->irq_status_raw = 0xFFFFFFFF;

    /* Auto block erase 1-st command */
    pregs->set_cmd = NAND_CMD_ERASE1ST;

    if (pdev->param->page_size_in_bytes >
        SMALL_BLOCK_PAGE_MAIN_AREA_SIZE)
    {
      /* Page Address 1st */
      pregs->set_addr = ((block_num << 6) & 0xC0);

      /* Page Address 2nd */
      pregs->set_addr = ((block_num << 6) >> 8) &
                        _BITMASK(8);

      if (pdev->param->amount_of_address_bytes == 5)
      {
        /* Page Address 3rd */
        pregs->set_addr = ((block_num << 6) >> 16) &
                          _BITMASK(8);
      }
    }
    else
    {
      /* Page Address 1st */
      pregs->set_addr = ((block_num << 5) & 0xE0);

      /* Page Address 2nd */
      pregs->set_addr = ((block_num << 5) >> 8) &
                        _BITMASK(8);

      if (pdev->param->amount_of_address_bytes == 4)
      {
        /* Page Address 3rd */
        pregs->set_addr = ((block_num << 5) >> 16) &
                          _BITMASK(8);
      }
    }

    /* Auto block erase 2-nd command */
    pregs->set_cmd = NAND_CMD_ERASE2ND;

    /* Wait for Erase operation to complete */
    timer_wait_ms(NULL, NAND_BLK_ERASE_TIME);

    /* Wait for NAND ready */
    while (!((tmp = pregs->check_sts) &
             (NAND_CHK_STS_RB_EDGE(pdev->cs))));

    /* Status read (1) command */
    pregs->set_cmd = NAND_CMD_STATUS;

    /* Operation status */
    if (pregs->read_data & NAND_FLASH_FAILED)
    {
      status = _ERROR;
    }
  }

  return status;
}

/***********************************************************************
 *
 * Function: nand_write_bootcfg
 *
 * Purpose: Write NAND Boot Configuration into Flash chip
 *
 * Processing:
 *     The NAND Boot Configuration info is written into page 0 block 0
 *		 of NAND Flash.
 *
 * Parameters:
 *     pdev: Pointer to NAND controller config structure
 *
 * Outputs: None
 *
 * Returns: The status of write operation
 *
 * Notes: None
 *
 **********************************************************************/
STATUS nand_write_bootcfg(NAND_DEV_CFG_T *pdev)
{
  UNS_32 *crc;
  NAND_BLOCKPAGE_T blkpage;
  STATUS status = _NO_ERROR;
  UNS_8 *buf_ptr = (UNS_8 *) data_buf;

  /* Copy Default boot parameters into data buffer */
  memset((void *)buf_ptr, 0x0, NAND_BPARAM_PAGE_SZ);
  memset((void *)(buf_ptr + NAND_BPARAM_PAGE_SZ), 0xFF,
         (LARGE_BLOCK_PAGE_SIZE - NAND_BPARAM_PAGE_SZ));
  memcpy((void *)buf_ptr, pdev->param, sizeof(NAND_BOOT_CFG_PARAMS_T));

  /* Compute CRC for Boot CFG parameters */
  crc = (UNS_32 *) & buf_ptr[NAND_BPARAM_PAGE_SZ - 4];
  *crc = crc32_compute(buf_ptr, (NAND_BPARAM_PAGE_SZ - 4));

  /* Store Boot CFG to page 0 block 0 */
  blkpage.dma = FALSE;
  blkpage.block_num = 0;
  blkpage.page_num = 0;
  blkpage.buffer = buf_ptr;
  blkpage.oobbuf = buf_ptr + pdev->param->page_size_in_bytes;
  status = nand_write_page(pdev, &blkpage);
  return status;
}

/***********************************************************************
 *
 * Function: nand_read_oob
 *
 * Purpose: Read OOB data
 *
 * Processing:
 *     Read OOB data as per Flash chip layout. This is used for
 *     checking the bad block byte marker
 *
 * Parameters:
 *     pdev: Pointer to NAND controller config structure
 *
 * Outputs: None
 *
 * Returns: The status of write operation
 *
 * Notes: None
 *
 **********************************************************************/
STATUS nand_read_oob(NAND_DEV_CFG_T *pdev,
                     NAND_BLOCKPAGE_T *pblockpage)
{
  STATUS status = _NO_ERROR;
  NAND_FLASH_CTRL_REGS_T *pregs = g_nand_drv.regptr;
  int i, size;
  volatile UNS_32 tmp;

  /* Clear all current statuses */
  pregs->irq_status_raw = 0xFFFFFFFF;

  /* Send Read OOB command for SPF */
  /* Send Read Read1 command fpr LPF */
  if (pdev->param->page_size_in_bytes > SMALL_BLOCK_PAGE_MAIN_AREA_SIZE)
  {
    size = 64;

    pregs->set_cmd = 	NAND_CMD_READ1;
    nand_send_addr(pdev, pblockpage->block_num,
                   pblockpage->page_num, 0);
    pregs->set_cmd = 	NAND_CMD_READSTART;

    /* Wait for read to complete */
    while (!((tmp = pregs->check_sts) &
             NAND_CHK_STS_RB_EDGE(pdev->cs)));

    /* Clear NAND interrupts */
    pregs->irq_status_raw = 0xFFFFFFFF;

    /* Send Random read commands to read OOB data */
    pregs->set_cmd = 	LPCNAND_CMD_RANDOM_READ1;

    /* First column address */
    pregs->set_addr = (pdev->param->page_size_in_bytes & 0xFF);

    /* Second column address */
    pregs->set_addr = ((pdev->param->page_size_in_bytes >> 8)
                       & 0xFF);

    /* Send Random read commands to read OOB data */
    pregs->set_cmd = 	LPCNAND_CMD_RANDOM_READ2;

    /* Wait for read to complete */
    while (!((tmp = pregs->check_sts) &
             NAND_CHK_STS_RB_LVL(pdev->cs)));
  }
  else
  {
    size = 16;

    pregs->set_cmd = 	NAND_CMD_READ3;
    nand_send_addr(pdev, pblockpage->block_num,
                   pblockpage->page_num, 0);

    /* Wait for read to complete */
    while (!((tmp = pregs->check_sts) &
             NAND_CHK_STS_RB_EDGE(pdev->cs)));
  }

  /* Read data from register */
  for (i = 0; i < size; i++)
  {
    pblockpage->buffer[i] = (UNS_8)	pregs->read_data;
  }

  return status;
}

/***********************************************************************
 *
 * Function: nand_write_bbt_info
 *
 * Purpose: Write NAND Bad Block List into Flash
 *
 * Processing:
 *     The NAND Bad Block list is written into Flash. The data will be
 * 		 stored from page 1 block 0 of NAND Flash.
 *
 * Parameters:
 *     pdev: Pointer to NAND controller config structure
 *
 * Outputs: None
 *
 * Returns: The status of write operation
 *
 * Notes: None
 *
 **********************************************************************/
STATUS nand_write_bbt_info(NAND_DEV_CFG_T *pdev)
{
  NAND_BLOCKPAGE_T blkpage;
  STATUS status = _NO_ERROR;
  UNS_8 *buf_ptr = (UNS_8 *) data_buf;
  UNS_32 curr_pos = 0;
  UNS_32 magic_word_pos;
  UNS_32 bb_marker, page_nr;
  UNS_32 blk_num, num_bbs = 0;

  /* Reset the BBT list arrays */
  memset((void *)data_buf, 0xFF, LARGE_BLOCK_PAGE_SIZE);
  memset((void *)bbt_list, 0xFF, NAND_MAX_NUM_BLKS * sizeof(UNS_32));

  /* Block, page info for writing BBT info */
  blkpage.dma = FALSE;
  blkpage.block_num = 0;
  blkpage.buffer = buf_ptr;
  blkpage.oobbuf = buf_ptr + pdev->param->page_size_in_bytes;

  /* Build BBT info */
  if (pdev->block0_ok == TRUE)
  {
    /* If RAM BBT already present, but Bad Blocks updated in RAM BBT.
    	 Hence get bad block info from RAM BBT table &
    	 write it into Flash */
    /* Build bad block list page */
    for (blk_num = 1; blk_num < pdev->param->nbr_of_blocks; blk_num++)
    {
      if (!(nand_checkbb_array_bit(pdev, blk_num)))
      {
        /* Bad Block found */
        ++num_bbs;

        /* Store in BBT list array */
        bbt_list[num_bbs] = blk_num;

        /* Bail out if Num of BBs is larger than BBT list array */
        if (num_bbs > NAND_MAX_NUM_BLKS)
          break;
      }
    }
  }
  else
  {
    /* RAM based BBT not present, hence get Bad Block info from Flash
    	 (Bad Block marker byte), create RAM BBT & store in Flash */
    /* Reset RAM BBT info array */
    memset((void *)pdev->nand_bbt, 0xFF, sizeof(pdev->nand_bbt));

    /* Get the bad block marker byte */
    /* For LPF, byte 0 is bad block marker byte */
    /* For SPF, byte 5 is bad block marker byte */
    bb_marker = (pdev->param->page_size_in_bytes >
                 SMALL_BLOCK_PAGE_MAIN_AREA_SIZE) ? 0 : 5;

    /* Build bad block list page */
    for (blk_num = 1; blk_num < pdev->param->nbr_of_blocks; blk_num++)
    {
      /* Index to spare area */
      blkpage.block_num = blk_num;
      status = nand_read_oob(pdev, &blkpage);
      if (status == _ERROR)
      {
        return status;
      }

      /* Check Bad block marker byte */
      if (buf_ptr[bb_marker] != 0xFF)
      {
        /* Bad Block found */
        ++num_bbs;

        /* Update RAM BBT */
        nand_setbb_array_bit(pdev, blk_num);

        /* Store in BBT list array */
        bbt_list[num_bbs] = blk_num;

        /* Bail out if Num of BBs is larger than BBT list array */
        if (num_bbs > NAND_MAX_NUM_BLKS)
          break;
      }
    }
  }

  /* Update bad block count at 0 in BBT list array */
  bbt_list[0] = num_bbs;

  /* Write the BBT info into Flash starting from page 1 */
  curr_pos = 0;
  page_nr = 1;
  while (curr_pos < (num_bbs + 1))
  {
    /* Check if BBT info can fit in 1 page. */
    /* Move the data to subsequent pages if not possibble in 1 page */
    if ((num_bbs + 1 - curr_pos) <
        (pdev->param->page_size_in_32bit_words - 2))
    {
      magic_word_pos = num_bbs + 1;
    }
    else
    {
      magic_word_pos = curr_pos + pdev->param->page_size_in_32bit_words - 2;
    }

    /* Copy the data to write buffer */
    memcpy((void *)data_buf, (void *) &bbt_list[curr_pos],
           magic_word_pos * sizeof(UNS_32));

    /* Innsert magic word */
    data_buf[magic_word_pos] = ((((UNS_32) 'B') << 0) |
                                (((UNS_32) 'A') << 8) | (((UNS_32) 'D') << 16) |
                                (((UNS_32)(page_nr & 0xFF)) << 24));

    /* compute CRC32 */
    data_buf[magic_word_pos + 1] = crc32_compute((UNS_8 *) data_buf,
                                                 magic_word_pos * sizeof(UNS_32));

    /* Write page data into Flash */
    blkpage.dma = FALSE;
    blkpage.block_num = 0;
    blkpage.page_num = page_nr;
    blkpage.buffer = (UNS_8 *)data_buf;
    blkpage.oobbuf = (UNS_8 *) data_buf + pdev->param->page_size_in_bytes;
    status = nand_write_page(pdev, &blkpage);
    if (status == _ERROR)
    {
      return status;
    }

    /* move to next page */
    page_nr++;

    /* update current position in bad block list */
    curr_pos += magic_word_pos;
  }

  return status;
}

/***********************************************************************
 *
 * Function: nand_check_bb
 *
 * Purpose: Check if the block is bad
 *
 * Processing:
 *     Check if the block is bad. From the RAM Bad Block Table present,
 *		 get bad block info & update bad block flag
 *		 in user structure.
 *
 * Parameters:
 *     pdev: Pointer to NAND controller config structure
 *     bb_info: Pointer to Bad Block information structure
 *
 * Outputs: None
 *
 * Returns: The status of bad block check operation
 *
 * Notes: None
 *
 **********************************************************************/
STATUS nand_check_bb(NAND_DEV_CFG_T *pdev,
                     NAND_BB_CHECK_T *bb_info)
{
  STATUS status = _NO_ERROR;

  /* Check if it is valid block number */
  if (bb_info->blk_num >= pdev->param->nbr_of_blocks)
  {
    /* Return with error */
    return _ERROR;
  }

  /* If no bad block inf available, return with error */
  if (pdev->block0_ok == FALSE)
  {
    status = _ERROR;
  }

  /* Check block info */
  if (status == _NO_ERROR)
  {
    /* Check BB info from RAM Table */
    if (!(nand_checkbb_array_bit(pdev, bb_info->blk_num)))
    {
      bb_info->blk_bad = TRUE;
    }
    else
    {
      bb_info->blk_bad = FALSE;
    }
  }

  return status;
}

/***********************************************************************
 *
 * Function: nand_mark_bb
 *
 * Purpose: Mark block as bad
 *
 * Processing:
 *     Update the block info in RAM Bad Block Table.
 *		 Update the block info in Flash Boot Info table.
 *
 * Parameters:
 *     pdev: Pointer to NAND controller config structure
 *     blk_num: Block number
 *
 * Outputs: None
 *
 * Returns: The status of mark bad block operation
 *
 * Notes: None
 *
 **********************************************************************/
STATUS nand_mark_bb(NAND_DEV_CFG_T *pdev, UNS_32 blk_num)
{
  STATUS status = _NO_ERROR;

  /* Check if it is valid block number */
  if (blk_num >= pdev->param->nbr_of_blocks)
  {
    /* Return with error */
    return _ERROR;
  }

  /* If no bad block info available, return with error */
  if (pdev->block0_ok == FALSE)
  {
    status = _ERROR;
  }

  /* Check block info */
  if (status == _NO_ERROR)
  {
    /* If it is already marked as bad, then just return */
    if ((nand_checkbb_array_bit(pdev, blk_num)))
    {
      /* Mark block as bad in RAM BBT */
      nand_setbb_array_bit(pdev, blk_num);

      /* Update block info in Flash info */
      pdev->block0_prot = FALSE;
      status = nand_format((INT_32)pdev);
      pdev->block0_prot = TRUE;
    }
  }

  return status;
}

/***********************************************************************
 * NAND driver public functions
 **********************************************************************/

/***********************************************************************
 *
 * Function: nand_open
 *
 * Purpose: Open the NAND
 *
 * Processing:
 *     If the passed register base is valid and the NAND
 *     driver isn't already initialized, then setup the NAND
 *     into a default initialized state that is disabled. Return
 *     a pointer to the driver's data structure or NULL if driver
 *     initialization failed.
 *
 * Parameters:
 *     ipbase: Pointer to a NAND peripheral block
 *     arg   : Not used
 *
 * Outputs: None
 *
 * Returns: The pointer to a NAND config structure or NULL
 *
 * Notes: None
 *
 **********************************************************************/
INT_32 nand_open(void *ipbase, INT_32 arg)
{
  NAND_DEV_T* parg = (NAND_DEV_T*)arg;
  NAND_DEV_CFG_T* pdev;
  volatile UNS_32 tmp;
  UNS_8 ids[4];
  NAND_BOOT_CFG_PARAMS_T def_cfg;

  if ((NAND_FLASH_CTRL_REGS_T *)ipbase == NAND_CTRL)
  {
    if (g_nand_drv.init == FALSE)
    {
      /* enable NAND clocks */
      cgu_clk_en_dis(CGU_SB_NANDFLASH_S0_CLK_ID, TRUE);
      cgu_clk_en_dis(CGU_SB_NANDFLASH_ECC_CLK_ID, TRUE);
      cgu_clk_en_dis(CGU_SB_NANDFLASH_NAND_CLK_ID, TRUE);
      cgu_clk_en_dis(CGU_SB_NANDFLASH_PCLK_ID, TRUE);

      /* needed for LPC315x series only */
      cgu_clk_en_dis(CGU_SB_NANDFLASH_AES_CLK_ID, TRUE);

      /* reset NAND controller */
      cgu_soft_reset_module(NANDFLASH_NAND_RESET_N_SOFT);
      cgu_soft_reset_module(NANDFLASH_ECC_RESET_N_SOFT);

      /* need for LPC315x series only */
      cgu_soft_reset_module(NANDFLASH_AES_RESET_N_SOFT);

      /* check NAND mux signals */
      SYS_REGS->mux_nand_mci_sel = 0;

      /* controller initialised */
      g_nand_drv.regptr = NAND_CTRL;
      g_nand_drv.init = TRUE;
    }

    /* Configure NAND flash based on the open_cfg structure */
    if (arg != 0)
    {
      pdev = &g_nand_drv.dev[ parg->cs ];
      /* Check if device already opened,
      	if it is already opened, return with error
      	(Only one instance is allowed per device)
      */
      if (pdev->init == FALSE)
      {
        /* check the parms passed are good */
        if (parg->cs < NAND_MAX_CHIPS_SUP)
        {
          /* copy dev cofigurations, Initialise default config */
          pdev->cs = parg->cs;
          pdev->page_mode = 0; /* Default - 516 bytes */

          /* Initialise Default config for
          	 identification of device */
          def_cfg.interface_width = parg->interface_width;
          def_cfg.ecc_mode = 5; 	/* Default - Enable ECC */
          /* Default Time settings */
          /* tsrd=3, tals=3, talh=1, tcls=3, tclh=1, */
          def_cfg.timing1 = (NAND_TIM1_TSRD(3) | NAND_TIM1_TALS(3) |
                             NAND_TIM1_TALH(1) | NAND_TIM1_TCLS(3) | NAND_TIM1_TCLH(1));
          /* tdrd=3, tebi=1, tch=1, tcs=4, treh=2, */
          /* trp=4, twh=2, twp = 3*/
          def_cfg.timing2 = (NAND_TIM2_TDRD(3) | NAND_TIM2_TEBI(1) |
                             NAND_TIM2_TCH(1) | NAND_TIM2_TCS(4) | NAND_TIM2_TRH(2) |
                             NAND_TIM2_TRP(4) | NAND_TIM2_TWH(2) | NAND_TIM2_TWP(3));
          pdev->param = &def_cfg;

          /* set init */
          pdev->init = TRUE;

          /* config the controller for this device */
          if (nand_sel_device(pdev) == _NO_ERROR)
          {
            /* Clear NAND interrupts */
            NAND_CTRL->irq_status_raw = 0xFFFFFFFF;

            /* reset nand */
            NAND_CTRL->set_cmd = NAND_CMD_RESET;

            /* wait for device ready */
            while (!((tmp = NAND_CTRL->check_sts) &
                     NAND_CHK_STS_RB_EDGE(pdev->cs)));

            /* READ IDs and verify params were ok */
            if (nand_read_check_ids((INT_32)pdev, (void *)ids) == _NO_ERROR)
            {
              /* Set the actual chip values after identification */
              g_nand_drv.cur_dev = NULL;

              if (nand_sel_device(pdev) == _NO_ERROR)
              {
                /* Install NAND ISR if first instance */
                if (g_nand_drv.instance == 0)
                {
                  int_install_irq_handler(IRQ_NAND_FLASH, (PFV) nand_interrupt);
                  int_enable(IRQ_NAND_FLASH);

                  dma_init();
                }

                /* Check if block 0 contains valid data */
                if (!nand_block0_ok(pdev))
                {
                  pdev->block0_prot = FALSE;

                  /* Format NAND block 0 */
                  if (nand_format((INT_32)pdev) == _ERROR)
                  {
                    pdev->init = FALSE;

                    int_install_irq_handler(IRQ_NAND_FLASH, (PFV) NULL);
                    int_disable(IRQ_NAND_FLASH);

                    return 0;
                  }
                  pdev->block0_ok = TRUE;
                }

                pdev->block0_prot = TRUE;
                g_nand_drv.instance++;

                return (INT_32)pdev;
              }
            }
          }

          /* if control came here then something went wrong. so deinit device.*/
          pdev->init = FALSE;
        }
      }
    }
  }

  return 0;
}

/***********************************************************************
 *
 * Function: nand_close
 *
 * Purpose: Close the NAND
 *
 * Processing:
 *     If init is not TRUE, then return _ERROR to the caller as the
 *     device was not previously opened. Otherwise, disable the
 *     NAND, set init to FALSE, and return _NO_ERROR to the caller.
 *
 * Parameters:
 *     devid: Pointer to NAND config structure
 *
 * Outputs: None
 *
 * Returns: The status of the close operation
 *
 * Notes: None
 *
 **********************************************************************/
STATUS nand_close(INT_32 devid)
{
  STATUS status = _ERROR;
  NAND_DEV_CFG_T* pdev = (NAND_DEV_CFG_T *)devid;

  /* Check if device is initialised */
  if (pdev->init == TRUE)
  {
    /* Deinitialise device */
    pdev->init = FALSE;
    pdev->block0_ok = FALSE;
    pdev->block0_ok = FALSE;

    /* Decerement instance count */
    g_nand_drv.instance--;
  }

  /* If last instance, deinitialise the controller */
  if ((!g_nand_drv.instance) && (g_nand_drv.init == TRUE))
  {
    /* controller initialised */
    g_nand_drv.init = FALSE;
    g_nand_drv.dma = FALSE;
    g_nand_drv.cur_dev = NULL;

    /* Uninstall ISR */
    int_disable(IRQ_NAND_FLASH);
    int_install_irq_handler(IRQ_NAND_FLASH, (PFV) NULL);

    /* Disable NAND clocks */
    cgu_clk_en_dis(CGU_SB_NANDFLASH_S0_CLK_ID, FALSE);
    cgu_clk_en_dis(CGU_SB_NANDFLASH_ECC_CLK_ID, FALSE);
    cgu_clk_en_dis(CGU_SB_NANDFLASH_NAND_CLK_ID, FALSE);
    cgu_clk_en_dis(CGU_SB_NANDFLASH_PCLK_ID, FALSE);

    /* needed for LPC315x series only */
    cgu_clk_en_dis(CGU_SB_NANDFLASH_AES_CLK_ID, FALSE);
  }

  return status;
}

/***********************************************************************
 *
 * Function: nand_ioctl
 *
 * Purpose: NAND configuration block
 *
 * Processing:
 *     This function is a large case block. Based on the passed function
 *     and option values, set or get the appropriate timer parameter.
 *
 * Parameters:
 *     devid: Pointer to NAND config structure
 *     cmd:   ioctl command
 *     arg:   ioctl argument
 *
 * Outputs: None
 *
 * Returns: The status of the ioctl operation
 *
 * Notes: None
 *
 **********************************************************************/
STATUS nand_ioctl(INT_32 devid,
                  INT_32 cmd,
                  INT_32 arg)
{
  STATUS status = _ERROR;
  NAND_DEV_CFG_T* pdev = (NAND_DEV_CFG_T *)devid;
  NAND_DEV_INFO_T *dev_info;

  /* Check if device is initialised */
  if (pdev->init == TRUE)
  {
    status = _NO_ERROR;
    switch (cmd)
    {
      case NAND_GET_STATUS:
        status = nand_get_status();
        *((NAND_STATUS_T *)arg) = (status & NAND_FLASH_READY) ?
                                  NAND_READY : NAND_BUSY;
        break;

      case NAND_GET_DEV_INFO:
        /* Read NAND Flash id */
        dev_info = (NAND_DEV_INFO_T *)arg;
        status = nand_read_id((INT_32)pdev, (void *) dev_info->id);
        if (status == _NO_ERROR)
        {
          dev_info->page_size = pdev->param->page_size_in_bytes;
          dev_info->oob_size = ((pdev->param->page_size_in_bytes / NAND_BUF_SIZE)
                                * NAND_OOB_BYTES);
          dev_info->pages_per_block = pdev->param->pages_per_block;
          dev_info->nbr_of_blocks = pdev->param->nbr_of_blocks;
        }
        break;

      case NAND_ERASE_BLOCK:
        /* Erase NAND Flash block */
        status = nand_erase_block(pdev, (UNS_32) arg);
        break;

      case NAND_READ_PAGE:
        /* Read NAND flash page */
        status = nand_read_page(pdev,
                                (NAND_BLOCKPAGE_T *) arg);
        break;

      case NAND_WRITE_PAGE:
        /* Write NAND flash page */
        status = nand_write_page(pdev,
                                 (NAND_BLOCKPAGE_T *) arg);
        break;

      case NAND_CHECK_BB:
        status = nand_check_bb(pdev,
                               (NAND_BB_CHECK_T *) arg);
        break;

      case NAND_MARK_BB:
        status = nand_mark_bb(pdev,
                              (UNS_32) arg);
        break;

      default:
        /* Unsupported parameter */
        status = LPC_BAD_PARAMS;
    }
  }

  return status;
}

/***********************************************************************
 *
 * Function: nand_read
 *
 * Purpose: NAND read function
 *
 * Processing:
 *     Return 0 to the caller.
 *
 * Parameters:
 *     devid:     Pointer to NAND device descriptor
 *     buffer:    Pointer to data buffer to copy to
 *     max_bytes: Number of bytes to read
 *
 * Outputs: None
 *
 * Returns: Number of bytes actually read (always 0)
 *
 * Notes: None
 *
 **********************************************************************/
INT_32 nand_read(INT_32 devid,
                 void *buffer,
                 INT_32 max_bytes)
{
  return 0;
}

/***********************************************************************
 *
 * Function: nand_write
 *
 * Purpose: NAND write function (stub only)
 *
 * Processing:
 *     Return 0 to the caller.
 *
 * Parameters:
 *     devid:   Pointer to NAND descriptor
 *     buffer:  Pointer to data buffer to copy from
 *     n_bytes: Number of bytes to write
 *
 * Outputs: None
 *
 * Returns: Number of bytes actually written (always 0)
 *
 * Notes: None
 *
 **********************************************************************/
INT_32 nand_write(INT_32 devid,
                  void *buffer,
                  INT_32 n_bytes)
{
  return 0;
}

/***********************************************************************
 *
 * Function: nand_format
 *
 * Purpose: NAND Format function
 *
 * Processing:
 *     Restore the block 0 contents in Flash chip if not already
 *		 not available.
 *
 * Parameters:
 *     devid:   Pointer to NAND descriptor
 *
 * Outputs: None
 *
 * Returns: status of the format operation
 *
 * Notes: None
 *
 **********************************************************************/
STATUS nand_format(INT_32 devid)
{
  NAND_DEV_CFG_T* pdev = (NAND_DEV_CFG_T *)devid;
  STATUS status = _NO_ERROR;

  /* Check if the Flash is already formatted & protected */
  if ((pdev->block0_ok == TRUE) &&
      (pdev->block0_prot == TRUE))
  {
    /* NAND already formatted, just return */
    return status;
  }

  /* Erase Block0 */
  status = nand_erase_block(pdev, 0);
  if (status == _ERROR)
  {
    /* Erase Failure */
    goto exit1;
  }

  /* construct nand param structre page */
  /* write nand param page */
  status = nand_write_bootcfg(pdev);
  if (status == _ERROR)
  {
    /* Write failure */
    goto exit1;
  }

  /* construct bad block list */
  /* write bad block list page */
  status = nand_write_bbt_info(pdev);

exit1:
  return status;
}

