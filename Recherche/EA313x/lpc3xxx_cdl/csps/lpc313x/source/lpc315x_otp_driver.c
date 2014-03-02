/***********************************************************************
 * $Id:: lpc315x_otp_driver.c 2530 2009-11-17 00:39:08Z pdurgesh      $
 *
 * Project: LPC315x standard OTP driver
 *
 * Description:
 *     This file contains driver support for the LPC315x OTP block
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

#include "lpc315x_otp_driver.h"
#include "lpc313x_cgu_driver.h"
#include "lpc313x_timer_driver.h"

/***********************************************************************
 * OTP driver package data
***********************************************************************/

/* OTP device configuration structure type */
typedef struct
{
  BOOL_32 init;
  PFV set_read_voltage;
  PFV set_write_voltage;
} OTP_CFG_T;

/* OTP driver data */
static OTP_CFG_T g_otp_drv_cfg;

/***********************************************************************
 * OTP driver private functions
 **********************************************************************/


/***********************************************************************
 *
 * Function: otp_copy_fuses
 *
 * Purpose: Copy polyfuses state to data registers in OTP block.
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     pcfg     : Pointer to a OTP driver configuration structure
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
static void otp_copy_fuses(OTP_CFG_T *pcfg)
{
  UNS_32 i, jtag;

  /* disable external enabling while copying the fuses */
  cgu_clk_set_exten(CGU_SB_OTP_PCLK_ID, FALSE);

  /* apply read voltage to VPP pin */
  if (pcfg->set_read_voltage != NULL)
    pcfg->set_read_voltage();

  /* save JTAG enable field */
  jtag = OTP->con & OTP_CON_ARM_JTAG_EN;
  /* now read 16 fuses at a time */
  for (i = 0; i < 32; i++)
  {
    /* set 16 bit bock in idle mode */
    OTP->con = OTP_CON_16B_ADDR(i) | OTP_CON_IDLE_MODE | jtag;
    /* set 16 bit bock in copy mode */
    OTP->con = OTP_CON_16B_ADDR(i) | OTP_CON_COPY_MODE | jtag;
    /* give atleast 15 nsec for data to be copied */
    timer_wait_us(NULL, 1);
    /* set 16 bit bock in idle mode */
    OTP->con = OTP_CON_16B_ADDR(i) | OTP_CON_IDLE_MODE | jtag;
  }
  /* enable external enabling for conserving power */
  cgu_clk_set_exten(CGU_SB_OTP_PCLK_ID, TRUE);

}


/***********************************************************************
 * OTP driver public functions
 **********************************************************************/

/***********************************************************************
 *
 * Function: otp_open
 *
 * Purpose: Open the OTP
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     ipbase: Pointer to a OTP peripheral block
 *     arg   : Pointer to OTP setup structure (OTP_OPEN_ARG_T) or NULL
 *
 * Outputs: None
 *
 * Returns: The pointer to a OTP driver config structure or NULL
 *
 * Notes: None
 *
 **********************************************************************/
INT_32 otp_open(void *ipbase,
                INT_32 arg)
{
  OTP_OPEN_ARG_T* pargs = (OTP_OPEN_ARG_T*)arg;

  /* Has the OTP been previously initialized? */
  if (g_otp_drv_cfg.init == FALSE)
  {
    /* OTP is free */
    g_otp_drv_cfg.init = TRUE;

    /* set voltage driver funtions */
    if (pargs != NULL)
    {
      g_otp_drv_cfg.set_read_voltage = pargs->set_read_voltage;
      g_otp_drv_cfg.set_write_voltage = pargs->set_write_voltage;
    }

    /* Enable OTP system clock */
    cgu_clk_en_dis(CGU_SB_OTP_PCLK_ID, TRUE);

    /* Copy polyfuse data to mirror data registers */
    otp_copy_fuses(&g_otp_drv_cfg);
  }

  /* Return pointer to specific OTP structure */
  return (INT_32) &g_otp_drv_cfg;
}

/***********************************************************************
 *
 * Function: otp_close
 *
 * Purpose: Close the OTP
 *
 * Processing:
 *     If init is not TRUE, then return _ERROR to the caller as the
 *     device was not previously opened. Otherwise, disable the OTP,
 *     set init to FALSE, and return _NO_ERROR to the caller.
 *
 * Parameters:
 *     devid: Pointer to OTP config structure
 *
 * Outputs: None
 *
 * Returns: The status of the close operation
 *
 * Notes: None
 *
 **********************************************************************/
STATUS otp_close(INT_32 devid)
{
  return _NO_ERROR;
}

/***********************************************************************
 *
 * Function: otp_ioctl
 *
 * Purpose: OTP configuration block
 *
 * Processing:
 *     This function is a large case block. Based on the passed function
 *     and option values, set or get the appropriate OTP parameter.
 *
 * Parameters:
 *     devid: Pointer to OTP config structure
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
STATUS otp_ioctl(INT_32 devid,
                 INT_32 cmd,
                 INT_32 arg)
{
  OTP_CFG_T *potp = (OTP_CFG_T *) devid;
  STATUS status = _ERROR;

  /* Close and disable device if it was previously initialized */
  if (potp->init != FALSE)
  {
    status = _NO_ERROR;

    switch (cmd)
    {
      case OTP_REREAD_FUSES:
        otp_copy_fuses(potp);
        break;

      case OTP_SET_WRITE_PROTECT:
        OTP->wprot = (UNS_32)arg;
        break;

      case OTP_SET_READ_PROTECT:
        OTP->rprot = (UNS_32)arg;
        break;

      case OTP_GET_JTAG_SEC_LEVEL:
        status = OTP->d.map.chip_cfg & OTPD_CFG_SEC_MASK;
        break;

      case OTP_GET_AES_KEY:
        if (arg != 0)
        {
          OTP_AES_KEY_T* pKey = (OTP_AES_KEY_T*)arg;
          pKey->data[0] = OTP->d.map.aes_key[0];
          pKey->data[1] = OTP->d.map.aes_key[1];
          pKey->data[2] = OTP->d.map.aes_key[2];
          pKey->data[3] = OTP->d.map.aes_key[3];
        }
        break;

      case OTP_GET_BANDGAP_ADJ:
        status = OTPD_CFG_BANDGAP_ADJ_GET(OTP->d.map.chip_cfg);
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
 * Function: otp_read
 *
 * Purpose: OTP read function
 *
 * Processing:
 *     Read the passed number of bytes in the passed buffer. Since there
 *     are 512 OTP fuses max bytes to read is 64.
 *
 * Parameters:
 *     devid:     Pointer to OTP descriptor
 *     buffer:    Pointer to data buffer to copy to
 *     max_bytes: Number of bytes to read
 *
 * Outputs: None
 *
 * Returns: Number of bytes actually read
 *
 * Notes: None
 *
 **********************************************************************/
INT_32 otp_read(INT_32 devid,
                void *buffer,
                INT_32 max_bytes)
{
  UNS_32 *pbuff = (UNS_32 *) buffer;
  INT_32 wread;

  if (max_bytes < OTP_TOTAL_FUSES_NUM / sizeof(UNS_32))
  {
    /* we read 32 bits at time */
    max_bytes = max_bytes / sizeof(UNS_32);
  }
  for (wread = 0; wread < max_bytes; wread++)
  {
    *pbuff = OTP->d.data[wread];
    pbuff++;
  }

  return (wread * sizeof(UNS_32));
}

/***********************************************************************
 *
 * Function: otp_write
 *
 * Purpose: Timer OTP function
 *
 * Processing:
 *     Write the passed OTP fuse data. Since there are 512 OTP fuses
 *     this function expects n_bytes to be 64.
 *
 * Parameters:
 *     devid:   Pointer to OTP descriptor
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
INT_32 otp_write(INT_32 devid,
                 void *buffer,
                 INT_32 n_bytes)
{
  INT_32 bwrite = 0;
  OTP_CFG_T *pcfg = (OTP_CFG_T *) devid;
  UNS_32 *data = (UNS_32 *) buffer;
  UNS_32 i, jtag;

  if (n_bytes == (OTP_TOTAL_FUSES_NUM / 8))
  {
    /* disable external enabling while writing the fuses */
    cgu_clk_set_exten(CGU_SB_OTP_PCLK_ID, FALSE);

    /* apply write voltage to VPP pin */
    if (pcfg->set_write_voltage != NULL)
      pcfg->set_write_voltage();

    /* save JTAG enable field */
    jtag = OTP->con & OTP_CON_ARM_JTAG_EN;

    /* now read 16 fuses at a time */
    for (i = 0; i < OTP_TOTAL_FUSES_NUM; i++)
    {
      /* if bit is set then blow the fuse */
      if (data[i >> 5] & _BIT((i & 0x1F)))
      {
        /* set the fuse for this bit in idle mode */
        OTP->con = OTP_CON_ADDR(i) | OTP_CON_IDLE_MODE | jtag;
        /* give 6 usec to for the fuse to blow */
        timer_wait_us(NULL, 6);
        /* set the fuse for this bit in write mode ~ blow it! */
        OTP->con = OTP_CON_ADDR(i) | OTP_CON_WRITE_MODE | jtag;
        /* give 6 usec to for the fuse to blow */
        timer_wait_us(NULL, 6);
        /* set the fuse for this bit in idle mode */
        OTP->con = OTP_CON_ADDR(i) | OTP_CON_IDLE_MODE | jtag;
      }
    }
    bwrite = OTP_TOTAL_FUSES_NUM / sizeof(UNS_32);
    /* apply read voltage to VPP pin */
    if (pcfg->set_read_voltage != NULL)
      pcfg->set_read_voltage();

    /* enable external enabling for conserving power */
    cgu_clk_set_exten(CGU_SB_OTP_PCLK_ID, TRUE);
  }

  return bwrite;
}
