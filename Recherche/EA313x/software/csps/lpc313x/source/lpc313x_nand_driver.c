/***********************************************************************
 * $Id:: lpc313x_nand_driver.c 3557 2010-05-20 00:31:30Z usb10131       $
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

/***********************************************************************
 * WDT driver package data
***********************************************************************/

/* NAND controller configuration structure type */
typedef struct
{
  BOOL_32 init;                       /* Controller initialized flag */
  NAND_FLASH_CTRL_REGS_T *regptr;     /* Pointer to NAND registers */
  NAND_DEV_T dev[4];                  /* device dat for each chip select*/
  NAND_DEV_T* cur_dev;                /* pointer to current device */
} NAND_DRV_T;

/* WDT driver data */
static NAND_DRV_T g_nand_drv;

/***********************************************************************
 * NAND driver private functions
 **********************************************************************/
int nand_sel_device(NAND_DEV_T* pdev)
{
  int ret = _ERROR;
  NAND_FLASH_CTRL_REGS_T *pregs = g_nand_drv.regptr;
  UNS_32 config = NAND_CFG_DC;

  /* check whether the correspondig chip select is initialized */
  if (pdev->init)
  {
    /* set timings per selected device */
    pregs->timing1 = pdev->param.timing1;
    pregs->timing2 = pdev->param.timing2;

    /* build config value for the device */
    /* set ecc type */
    switch (pdev->param.ecc_mode)
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
    if (pdev->param.interface_width == 16)
      config |= NAND_CFG_WD;

    /* set page mode */
    if (pdev->page_mode)
      config |= NAND_CFG_M;

    /* write the final config value */
    pregs->config = config;

    /* select bank0*/
    pregs->set_ce = NAND_SETCE_WP | NAND_SETCE_CV(pdev->cs);

    /* finaly update the current dev pointer */
    g_nand_drv.cur_dev = pdev;

    ret = _NO_ERROR;
  }

  return ret;
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
  NAND_DEV_T* pdev = (NAND_DEV_T*)devid;
  UNS_8 *bptr = buf;
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
    /* ID read (1) command */
    pregs->set_cmd = NAND_CMD_READID;

    /* Column Address */
    pregs->set_addr = 0;

    /* Wait for MLC NAND ready */
    while (!(pregs->check_sts & NAND_CHK_STS_RB_EDGE(pdev->cs)));

    /* Read ID_SIZE/4 words from MLC NAND flash data register */
    for (idx = 0; idx < (ID_SIZE / 4); idx++)
    {
      *bptr = (UNS_8)NAND_CTRL->read_data;
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
STATUS nand_erase_block(INT_32 devid, UNS_32 block_num)
{
  STATUS status = _NO_ERROR;
  NAND_FLASH_CTRL_REGS_T *pregs = g_nand_drv.regptr;
  NAND_DEV_T* pdev = (NAND_DEV_T*)devid;

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
    if (pdev->param.page_size_in_bytes <= SMALL_BLOCK_PAGE_SIZE)
    {
      /* Wait for MLC NAND ready */
      while (!(pregs->check_sts & NAND_CHK_STS_RB_LVL(pdev->cs)));

      /* Auto block erase 1-st command */
      pregs->set_cmd = NAND_CMD_ERASE1ST;

      /* Page Address 1st */
      pregs->set_addr = ((block_num << 5) & 0xE0);

      /* Page Address 2nd */
      pregs->set_addr = ((block_num << 5) >> 8) &
                        _BITMASK(8);

      if (pdev->param.amount_of_address_bytes == ADDR_CYCLES_4)
      {
        /* Page Address 3rd */
        pregs->set_addr = ((block_num << 5) >> 16) &
                          _BITMASK(8);
      }

      /* Auto block erase 2-nd command */
      pregs->set_cmd = NAND_CMD_ERASE2ND;

      /* Wait for MLC NAND ready */
      while (!(pregs->check_sts & NAND_CHK_STS_RB_EDGE(pdev->cs)));

      /* Status read (1) command */
      pregs->set_cmd = NAND_CMD_STATUS;

      /* Operation status */
      if (pregs->read_data & NAND_FLASH_FAILED)
      {
        status = _ERROR;
      }
    }
    else if (pdev->param.page_size_in_bytes == LARGE_BLOCK_PAGE_SIZE)
    {
      /* Wait for MLC NAND ready */
      while (!(pregs->check_sts & NAND_CHK_STS_RB_LVL(pdev->cs)));

      /* Auto block erase 1-st command */
      pregs->set_cmd = NAND_CMD_ERASE1ST;

      /* Page Address 1st */
      pregs->set_addr = ((block_num << 6) & 0xC0);

      /* Page Address 2nd */
      pregs->set_addr = ((block_num << 6) >> 8) &
                        _BITMASK(8);

      if (pdev->param.amount_of_address_bytes == ADDR_CYCLES_5)
      {
        /* Page Address 3rd */
        pregs->set_addr = ((block_num << 6) >> 16) &
                          _BITMASK(8);
      }

      /* Auto block erase 2-nd command */
      pregs->set_cmd = NAND_CMD_ERASE2ND;

      /* Wait for MLC NAND ready */
      while (!(pregs->check_sts & NAND_CHK_STS_RB_EDGE(pdev->cs)));

      /* Status read (1) command */
      pregs->set_cmd = NAND_CMD_STATUS;

      /* Operation status */
      if (pregs->read_data & NAND_FLASH_FAILED)
      {
        status = _ERROR;
      }
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
  NAND_DEV_T* pdev;
  UNS_32 tmp;

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
    g_nand_drv.init = TRUE;
  }
  /* configure NAND flash based on the open_cfg structure*/
  if (arg != 0)
  {
    pdev = &g_nand_drv.dev[ parg->cs ];
    /* check if device already opened */
    if (pdev->init == FALSE)
    {
      /* check the parms passed are good */
      if (parg->cs < 4)
      {
        /* copy dev cofigurations */
        *pdev = *parg;
        /* set init */
        pdev->init = TRUE;
        /* config the controller for this device */
        if (nand_sel_device(pdev) == _NO_ERROR)
        {
          /* clear status */
          tmp = NAND_CTRL->check_sts;
          /* reset nand */
          NAND_CTRL->set_cmd = NAND_CMD_RESET;
          /* wait for device ready */
          while (!((tmp = NAND_CTRL->check_sts) & NAND_CHK_STS_RB_EDGE(pdev->cs)));

          /* now do READ ID  and verify params were ok ???*/

          return (INT_32)pdev;
        }
        /* if control came here then something went wrong. so deinit device.*/
        pdev->init = FALSE;
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
  NAND_DEV_T* pdev = (NAND_DEV_T*)devid;

  /* check if device is selected */
  if (pdev != g_nand_drv.cur_dev)
  {
    /* select the device */
    if (nand_sel_device(pdev) == _NO_ERROR)
    {
      /* check requested bytes size is page aligned */
      if (max_bytes & 0x1FF)
        return 0;

      /* issue read command */

    }
  }

  return 0;
}

/***********************************************************************
 *
 * Function: nand_write
 *
 * Purpose: WDT write function (stub only)
 *
 * Processing:
 *     Return 0 to the caller.
 *
 * Parameters:
 *     devid:   Pointer to WDT descriptor
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


INT_32 nand_format()
{
  /* check if the falsh is formatted already */

  /* construct nand param structre page */

  /* write nand param page */

  /* construct bad block list */

  /* write bad block list page */

  return 0;
}

