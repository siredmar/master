/***********************************************************************
 * $Id:: lpc313x_pwm_driver.c 1519 2008-12-17 23:16:59Z pdurgesh     $
 *
 * Project: LPC313x pwm driver
 *
 * Description:
 *     This file contains driver support for the LPC313x pwm.
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

#include "lpc313x_pwm_driver.h"
#include "lpc313x_cgu_driver.h"

/***********************************************************************
 * PWM driver package data
***********************************************************************/

/* PWM device configuration structure type */
typedef struct
{
  BOOL_8 init;
  PWM_REGS_T *regptr;
} PWM_CFG_T;

/* PWM driver data */
static PWM_CFG_T pwmdat;

/***********************************************************************
 * PWM driver private functions
 **********************************************************************/


/***********************************************************************
 * PWM driver public functions
 **********************************************************************/

/***********************************************************************
 *
 * Function: pwm_open
 *
 * Purpose: Open the pwm
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     ipbase: Pointer to a pwm peripheral block
 *     arg   : Not used
 *
 * Outputs: None
 *
 * Returns: The pointer to a pwm config structure or NULL
 *
 * Notes: None
 *
 **********************************************************************/
INT_32 pwm_open(void *ipbase,
                INT_32 arg)
{
  PWM_CFG_T *pPWM;
  INT_32 pwmptr = (INT_32) NULL;

  pPWM = &pwmdat;
  /* Has the PWM been previously initialized? */
  if (pPWM->init == FALSE)
  {
    /* PWM is free */
    pPWM->init = TRUE;
    pPWM->regptr = (PWM_REGS_T *) ipbase;

    /* Enable PWM system clock */
    cgu_clk_en_dis(CGU_SB_PWM_PCLK_ID, TRUE);
    cgu_clk_en_dis(CGU_SB_PWM_PCLK_REGS_ID, TRUE);
    cgu_clk_en_dis(CGU_SB_PWM_CLK_ID, TRUE);

    /* Setup default PWM state as standard PWM mode, LOOP
       disabled, HI disabled, and select pwm_clk */
    cgu_soft_reset_module(PWM_RESET_AN_SOFT);
    /* Return pointer to specific PWM structure */
    pwmptr = (INT_32) pPWM;
  }

  return pwmptr;
}

/***********************************************************************
 *
 * Function: pwm_close
 *
 * Purpose: Close the PWM
 *
 * Processing:
 *     If init is not TRUE, then return _ERROR to the caller as the
 *     device was not previously opened. Otherwise, disable the PWM,
 *     set init to FALSE, and return _NO_ERROR to the caller.
 *
 * Parameters:
 *     devid: Pointer to PWM config structure
 *
 * Outputs: None
 *
 * Returns: The status of the close operation
 *
 * Notes: None
 *
 **********************************************************************/
STATUS pwm_close(INT_32 devid)
{
  PWM_CFG_T *pPWM;
  STATUS status = _ERROR;

  /* Get PWM device structure */
  pPWM = (PWM_CFG_T *) devid;
  if (pPWM->init == TRUE)
  {
    /* Disable all PWM functions */
    pPWM->regptr->cntl = 0x00;

    /* Disable PWM related clocks */
    cgu_clk_en_dis(CGU_SB_PWM_PCLK_ID, FALSE);
    cgu_clk_en_dis(CGU_SB_PWM_PCLK_REGS_ID, FALSE);
    cgu_clk_en_dis(CGU_SB_PWM_CLK_ID, FALSE);

    /* Set PWM as uninitialized */
    pPWM->init = FALSE;

    /* Successful operation */
    status = _NO_ERROR;
  }

  return status;
}

/***********************************************************************
 *
 * Function: pwm_ioctl
 *
 * Purpose: PWM configuration block
 *
 * Processing:
 *     This function is a large case block. Based on the passed function
 *     and option values, set or get the appropriate PWM parameter.
 *
 * Parameters:
 *     devid: Pointer to PWM config structure
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
STATUS pwm_ioctl(INT_32 devid,
                 INT_32 cmd,
                 INT_32 arg)
{
  UNS_32 tmp, tmp_reg;
  PWM_CFG_T *pPWM;
  INT_32 status = _ERROR;

  /* Get PWM device structure */
  pPWM = (PWM_CFG_T *) devid;
  if (pPWM->init == TRUE)
  {
    status = _NO_ERROR;

    switch (cmd)
    {
      case PWM_ENABLE:
        if (arg == 0)
        {
          /* Disable PWM related clocks */
          cgu_clk_en_dis(CGU_SB_PWM_PCLK_ID, FALSE);
          cgu_clk_en_dis(CGU_SB_PWM_PCLK_REGS_ID, FALSE);
          cgu_clk_en_dis(CGU_SB_PWM_CLK_ID, FALSE);
        }
        else
        {
          /* Enable PWM related clocks */
          cgu_clk_en_dis(CGU_SB_PWM_PCLK_ID, TRUE);
          cgu_clk_en_dis(CGU_SB_PWM_PCLK_REGS_ID, TRUE);
          cgu_clk_en_dis(CGU_SB_PWM_CLK_ID, TRUE);
        }
        break;

      case PWM_PDM:
        if (arg == 0)
          pPWM->regptr->cntl &= ~PWM_PDM_BIT;
        else
          pPWM->regptr->cntl |= PWM_PDM_BIT;
        break;

      case PWM_HI:
      case PWM_HIGH:
        if (arg == 0)
          pPWM->regptr->cntl &= ~PWM_HI_BIT;
        else
          pPWM->regptr->cntl |= PWM_HI_BIT;
        break;

      case PWM_LOOP:
        if (arg == 0)
          pPWM->regptr->cntl &= ~PWM_LOOP_BIT;
        else
        {
          pPWM->regptr->tmr = (pPWM->regptr->tmr & 0xFF) | ((arg & 0xF) << 8);
          pPWM->regptr->cntl |= PWM_LOOP_BIT;
        }
        break;

      case PWM_CLK:
        pPWM->regptr->cntl = (pPWM->regptr->cntl & ~0x3) | PWM_CLK_BIT(arg);
        break;

      case PWM_DUTY_A:
        pPWM->regptr->tmr = arg & 0xFFF;
        break;

      case PWM_DUTY_R:
        tmp = arg;
        if (((signed long int) tmp) < 0) tmp = -((signed long int)tmp);
        if (tmp >= 100)
        {
          pPWM->regptr->cntl |= PWM_HI_BIT;
        }
        else
        {
          tmp_reg = (tmp * 4095) / 100;
          pPWM->regptr->tmr = tmp_reg;
        }
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
 * Function: pwm_read
 *
 * Purpose: PWM read function (stub only)
 *
 * Processing:
 *     Return 0 to the caller.
 *
 * Parameters:
 *     devid:     Pointer to PWM descriptor
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
INT_32 pwm_read(INT_32 devid,
                void *buffer,
                INT_32 max_bytes)
{
  return 0;
}

/***********************************************************************
 *
 * Function: pwm_write
 *
 * Purpose: PWM write function (stub only)
 *
 * Processing:
 *     Return 0 to the caller.
 *
 * Parameters:
 *     devid:   Pointer to pwm descriptor
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
INT_32 pwm_write(INT_32 devid,
                 void *buffer,
                 INT_32 n_bytes)
{
  return 0;
}
