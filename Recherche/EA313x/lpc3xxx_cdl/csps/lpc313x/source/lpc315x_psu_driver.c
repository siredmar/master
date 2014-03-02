/***********************************************************************
 * $Id:: lpc315x_psu_driver.c 6113 2011-01-11 04:41:49Z ing02124      $
 *
 * Project: LPC315x PSU driver
 *
 * Description:
 *     This file contains driver support for the LPC315x PSU
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

#include "lpc313x_i2c_driver.h"
#include "lpc315x_psu_driver.h"

/***********************************************************************
 * PSU driver package data
***********************************************************************/

/* for i2c usage */
#define S_DCDC_IIC_ADDRESS                       0x0C
#define S_DCDC_BUFFER_LEN                        6

/* defines for address and data masking */
#define S_DCDC_ADDRESS_FIRST_BYTE_MASK           0x00FF
#define S_DCDC_ADDRESS_LAST_BYTE_MASK            0xFF00
#define S_DCDC_DATA_FIRST_BYTE_MASK              0x000000FF
#define S_DCDC_DATA_SECOND_BYTE_MASK             0x0000FF00
#define S_DCDC_DATA_THIRD_BYTE_MASK              0x00FF0000
#define S_DCDC_DATA_LAST_BYTE_MASK               0xFF000000
#define S_DCDC_SHIFT_0BIT                        0
#define S_DCDC_SHIFT_8BIT                        8
#define S_DCDC_SHIFT_16BIT                       16
#define S_DCDC_SHIFT_24BIT                       24
#define S_DCDC_AD_MINOR_VERSION_3                3

/* PSU device configuration structure type */
typedef struct
{
  BOOL_32 init;           /* Device initialized flag */
  AD_REGS_T *regptr;      /* Pointer to AD registers */
} PSU_CFG_T;

/* PSU driver data */
static PSU_CFG_T psudat;

/* I2C device handle */
static INT_32 i2cdev1;

/***********************************************************************
 * PSU driver private functions
 **********************************************************************/

/***********************************************************************
 *
 * Function: psu_dcdc_i2c_read
 *
 * Purpose: Read the PSU register through i2c interface
 *
 * Processing:
 *     If init is not TRUE, then return _ERROR as PSU was not
 *     previously opened. Otherwise, read PSU register.
 *
 * Parameters:
 *     devid: Pointer to I2C config structure
 *     reg_addr: PSU register address
 *     reg_data: Pointer to PSU register data
 *
 * Outputs: None
 *
 * Returns: The status of read operation
 *
 * Notes: None
 *
 **********************************************************************/
STATUS psu_dcdc_i2c_read(INT_32 devid,
                         UNS_32 reg_addr,
                         UNS_32 *reg_data)
{
  STATUS status = _ERROR;
  UNS_8 buf[S_DCDC_BUFFER_LEN];
  I2C_MTXRX_SETUP_T i2c_mtxrx_setup;

  if (psudat.init == TRUE)
  {
    status = _NO_ERROR;

    /*spilt the address into lower byte and higher byte */
    buf[1] = (UNS_8)(((reg_addr / 4) &
                      S_DCDC_ADDRESS_FIRST_BYTE_MASK) >>
                     S_DCDC_SHIFT_0BIT);
    buf[0] = (UNS_8)(((reg_addr / 4) &
                      S_DCDC_ADDRESS_LAST_BYTE_MASK) >>
                     S_DCDC_SHIFT_8BIT);
    buf[5] = 0;
    buf[4] = 0;
    buf[3] = 0;
    buf[2] = 0;

    i2c_mtxrx_setup.addr_mode = ADDR7BIT;
    i2c_mtxrx_setup.sl_addr = S_DCDC_IIC_ADDRESS;
    i2c_mtxrx_setup.tx_data = &buf[0];
    i2c_mtxrx_setup.tx_length = 2;
    i2c_mtxrx_setup.rx_data = &buf[2];
    i2c_mtxrx_setup.rx_length = 4;
    i2c_mtxrx_setup.retransmissions_max = 1;
    i2c_ioctl(devid, I2C_MASTER_TXRX, (INT_32)&i2c_mtxrx_setup);
    while ((i2c_mtxrx_setup.status & I2C_SETUP_STATUS_DONE) == 0);

    *reg_data = (UNS_32)buf[2] << S_DCDC_SHIFT_24BIT ;
    *reg_data = *reg_data | ((UNS_32)buf[3] << S_DCDC_SHIFT_16BIT);
    *reg_data = *reg_data | ((UNS_32)buf[4] << S_DCDC_SHIFT_8BIT);
    *reg_data = *reg_data | (UNS_32)buf[5];
  }

  return status;
}

/***********************************************************************
 *
 * Function: psu_dcdc_i2c_write
 *
 * Purpose: Write the PSU register through i2c interface
 *
 * Processing:
 *     If init is not TRUE, then return _ERROR as PSU was not
 *     previously opened. Otherwise, write PSU register.
 *
 * Parameters:
 *     devid: Pointer to I2C config structure
 *     reg_addr: PSU register address
 *     reg_data: Pointer to PSU register data
 *
 * Outputs: None
 *
 * Returns: The status of write operation
 *
 * Notes: None
 *
 **********************************************************************/
STATUS psu_dcdc_i2c_write(INT_32 devid,
                          UNS_32 reg_addr,
                          UNS_32 *reg_data)
{
  STATUS status = _ERROR;
  UNS_8 buf[S_DCDC_BUFFER_LEN];
  I2C_MTX_SETUP_T i2c_mtx_setup;

  if (psudat.init == TRUE)
  {
    status = _NO_ERROR;

    buf[1] = (UNS_8)(((reg_addr / 4) &
                      S_DCDC_ADDRESS_FIRST_BYTE_MASK) >>
                     S_DCDC_SHIFT_0BIT);
    buf[0] = (UNS_8)(((reg_addr / 4) &
                      S_DCDC_ADDRESS_LAST_BYTE_MASK) >>
                     S_DCDC_SHIFT_8BIT);
    buf[5] = (UNS_8)((*reg_data & S_DCDC_DATA_FIRST_BYTE_MASK) >>
                     S_DCDC_SHIFT_0BIT);
    buf[4] = (UNS_8)((*reg_data & S_DCDC_DATA_SECOND_BYTE_MASK) >>
                     S_DCDC_SHIFT_8BIT);
    buf[3] = (UNS_8)((*reg_data & S_DCDC_DATA_THIRD_BYTE_MASK) >>
                     S_DCDC_SHIFT_16BIT);
    buf[2] = (UNS_8)((*reg_data & S_DCDC_DATA_LAST_BYTE_MASK) >>
                     S_DCDC_SHIFT_24BIT);

    i2c_mtx_setup.addr_mode = ADDR7BIT;
    i2c_mtx_setup.sl_addr = S_DCDC_IIC_ADDRESS;
    i2c_mtx_setup.tx_data = &buf[0];
    i2c_mtx_setup.tx_length = 6;
    i2c_mtx_setup.retransmissions_max = 1;
    i2c_ioctl(devid, I2C_MASTER_TX, (INT_32)&i2c_mtx_setup);
    while ((i2c_mtx_setup.status & I2C_SETUP_STATUS_DONE) == 0);
  }

  return status;
}

/***********************************************************************
 * PSU driver public functions
 **********************************************************************/

/***********************************************************************
 *
 * Function: psu_open
 *
 * Purpose: Open the PSU
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     ipbase: Pointer to a PSU block
 *     arg   : Not used
 *
 * Outputs: None
 *
 * Returns: The pointer to a PSU config structure or
 *          NULL
 *
 * Notes: None
 *
 **********************************************************************/
INT_32 psu_open(void *ipbase, INT_32 arg)
{
  UNS_32 reg_value;
  INT_32 status = 0;
  I2C_SETUP_T i2c_setup;

  if ((AD_REGS_T *) ipbase == AD)
  {
    /* Has PSU driver been previously initialized? */
    if (psudat.init == FALSE)
    {
      /* Device not initialized and it usable, so set it to
         used */
      psudat.init = TRUE;

      /* Save address of register block */
      psudat.regptr = AD;

      /* open I2C1 */
      i2cdev1 = i2c_open((void *)I2C1, 0);

      /* formally assign a 7-bit address for I2C1,
         clock is 100 kHz, 50% duty cycle. */
      i2c_setup.addr_mode = ADDR7BIT;
      i2c_setup.rate_option = I2C_RATE_RELATIVE;
      i2c_setup.rate       = 50000;
      i2c_setup.low_phase  = 50;
      i2c_setup.high_phase = 50;
      i2c_ioctl(i2cdev1, I2C_SETUP, (INT_32)&i2c_setup);

      /* read the ad_reg_cgu_rw register through i2c
         interface */
      if (psu_dcdc_i2c_read(i2cdev1,
                            (UNS_32)&(psudat.regptr->ad_reg_cgu_rw),
                            &reg_value) == _ERROR)
      {
        return status;
      }

      /* Power up DCDC clock */
      reg_value &= ~((UNS_32)CGU_RW_PD_DCDC_CLK);

      /* write the ad_reg_cgu_rw register through i2c
         interface */
      if (psu_dcdc_i2c_write(i2cdev1,
                             (UNS_32)&(psudat.regptr->ad_reg_cgu_rw),
                             &reg_value) == _ERROR)
      {
        return status;
      }

      /* Return pointer to PSU config structure */
      status = (INT_32) & psudat;
    }
  }

  return status;
}

/***********************************************************************
 *
 * Function: psu_close
 *
 * Purpose: Close the PSU
 *
 * Processing:
 *     If init is not TRUE, then return _ERROR to the caller as the
 *     device was not previously opened. Otherwise, close the PSU,
 *     set init to FALSE, and return _NO_ERROR to the caller.
 *
 * Parameters:
 *     devid: Pointer to PSU config structure
 *
 * Outputs: None
 *
 * Returns: The status of the close operation
 *
 * Notes: None
 *
 **********************************************************************/
STATUS psu_close(INT_32 devid)
{
  UNS_32 reg_value;
  STATUS status = _ERROR;

  if ((PSU_CFG_T *) devid == &psudat)
  {
    if (psudat.init == TRUE)
    {
      /* read the ad_reg_cgu_rw register through i2c
         interface */
      if (psu_dcdc_i2c_read(i2cdev1,
                            (UNS_32)&(psudat.regptr->ad_reg_cgu_rw),
                            &reg_value) == _ERROR)
      {
        return status;
      }

      /* Power down DCDC clock */
      reg_value |= ((UNS_32)CGU_RW_PD_DCDC_CLK);

      /* write the ad_reg_cgu_rw register through i2c
         interface */
      if (psu_dcdc_i2c_write(i2cdev1,
                             (UNS_32)&(psudat.regptr->ad_reg_cgu_rw),
                             &reg_value) == _ERROR)
      {
        return status;
      }

      /* close I2C1 */
      i2c_close(i2cdev1);

      /* Set device as uninitialized */
      psudat.init = FALSE;

      /* Successful operation */
      status = _NO_ERROR;
    }
  }

  return status;
}

/***********************************************************************
 *
 * Function: psu_ioctl
 *
 * Purpose: PSU configuration block
 *
 * Processing:
 *     This function is a large case block. Based on the passed function
 *     and option values, set or get the appropriate PSU parameter.
 *
 * Parameters:
 *     devid: Pointer to PSU config structure
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
STATUS psu_ioctl(INT_32 devid,
                 INT_32 cmd,
                 INT_32 arg)
{
  UNS_32 reg_value;
  UNS_32 volt_value;
  UNS_32 minor_rev;
  INT_32 status = _ERROR;

  if ((PSU_CFG_T *) devid == &psudat)
  {
    if (psudat.init == TRUE)
    {
      status = _NO_ERROR;

      switch (cmd)
      {
        case PSU_WRITE_VOLTAGE:
          /* read the ad_reg_otgdclic_rw register through i2c
             interface */
          if (psu_dcdc_i2c_read(i2cdev1,
                                (UNS_32)&(psudat.regptr->ad_reg_otgdclic_rw),
                                &reg_value) == _ERROR)
          {
            return _ERROR;
          }
          /*check whether DCDC1 or DCDC2 is selected */
          switch (((PSU_DCDC_VOLTAGE_T *) arg)->psu_dcdc_select)
          {
            case PSU_DCDC_DCDC1:
              /* get the DCDC1 control bits to the lower byte */
              volt_value = (reg_value &
                            OTGDCLIC_RW_DCDC_CTRL_DCDC1_ADJUST(7)) >>
                           OTGDCLIC_RW_DCDC_CTRL_DCDC1_ADJUST_POS;

              /* if previous range greater than the specified range */
              if ((PSU_DCDC_RANGE_T)volt_value >
                  *(((PSU_DCDC_VOLTAGE_T *)arg)->psu_dcdc_range))
              {
                /* do a step wise decrement of the present range and
                   set it, until it becomes equal to the specified
                   range */
                while ((PSU_DCDC_RANGE_T)volt_value >
                       *(((PSU_DCDC_VOLTAGE_T *)arg)->psu_dcdc_range))
                {
                  /* reset the control bits for DCDC1 */
                  reg_value &=
                    (~(UNS_32)OTGDCLIC_RW_DCDC_CTRL_DCDC1_ADJUST(7));

                  /* reduce the present range by one */
                  volt_value--;

                  /*set the intermidiate range */
                  reg_value |= (volt_value <<
                                OTGDCLIC_RW_DCDC_CTRL_DCDC1_ADJUST_POS);

                  /* write the ad_reg_otgdclic_rw register through i2c
                     interface */
                  if (psu_dcdc_i2c_write(i2cdev1,
                                         (UNS_32)&(psudat.regptr->ad_reg_otgdclic_rw),
                                         &reg_value) == _ERROR)
                  {
                    return _ERROR;
                  }
                }
              }
              else
              {
                /* do a step wise increment of the present range and
                   set it, until it becomes equal to the specified
                   range */
                while ((PSU_DCDC_RANGE_T)volt_value <
                       *(((PSU_DCDC_VOLTAGE_T *)arg)->psu_dcdc_range))
                {
                  /* reset the control bits for DCDC1 */
                  reg_value &=
                    (~(UNS_32)OTGDCLIC_RW_DCDC_CTRL_DCDC1_ADJUST(7));

                  /* increase the present range by one */
                  volt_value++;

                  /*set the intermidiate range */
                  reg_value |= (volt_value <<
                                OTGDCLIC_RW_DCDC_CTRL_DCDC1_ADJUST_POS);

                  /* write the ad_reg_otgdclic_rw register through i2c
                     interface */
                  if (psu_dcdc_i2c_write(i2cdev1,
                                         (UNS_32)&(psudat.regptr->ad_reg_otgdclic_rw),
                                         &reg_value) == _ERROR)
                  {
                    return _ERROR;
                  }
                }
              }
              break;

            case PSU_DCDC_DCDC2:
              /* get the DCDC2 control bits to the lower byte */
              volt_value = (reg_value &
                            OTGDCLIC_RW_DCDC_CTRL_DCDC2_ADJUST(7)) >>
                           OTGDCLIC_RW_DCDC_CTRL_DCDC2_ADJUST_POS;

              /* if previous range greater than the specified range */
              if ((PSU_DCDC_RANGE_T)volt_value >
                  *(((PSU_DCDC_VOLTAGE_T *)arg)->psu_dcdc_range))
              {
                /* do a step wise decrement of the present range and
                   set it, until it becomes equal to the specified
                   range */
                while ((PSU_DCDC_RANGE_T)volt_value >
                       *(((PSU_DCDC_VOLTAGE_T *)arg)->psu_dcdc_range))
                {
                  /* reset the control bits for DCDC2 */
                  reg_value &=
                    (~(UNS_32)OTGDCLIC_RW_DCDC_CTRL_DCDC2_ADJUST(7));

                  /* reduce the present range by one */
                  volt_value--;

                  /*set the intermidiate range */
                  reg_value |= (volt_value <<
                                OTGDCLIC_RW_DCDC_CTRL_DCDC2_ADJUST_POS);

                  /* write the ad_reg_otgdclic_rw register through i2c
                     interface */
                  if (psu_dcdc_i2c_write(i2cdev1,
                                         (UNS_32)&(psudat.regptr->ad_reg_otgdclic_rw),
                                         &reg_value) == _ERROR)
                  {
                    return _ERROR;
                  }
                }
              }
              else
              {
                /* do a step wise increment of the present range and
                   set it, until it becomes equal to the specified
                   range */
                while ((PSU_DCDC_RANGE_T)volt_value <
                       *(((PSU_DCDC_VOLTAGE_T *)arg)->psu_dcdc_range))
                {
                  /* reset the control bits for DCDC2 */
                  reg_value &=
                    (~(UNS_32)OTGDCLIC_RW_DCDC_CTRL_DCDC2_ADJUST(7));

                  /* increase the present range by one */
                  volt_value++;

                  /*set the intermidiate range */
                  reg_value |= (volt_value <<
                                OTGDCLIC_RW_DCDC_CTRL_DCDC2_ADJUST_POS);

                  /* write the ad_reg_otgdclic_rw register through i2c
                     interface */
                  if (psu_dcdc_i2c_write(i2cdev1,
                                         (UNS_32)&(psudat.regptr->ad_reg_otgdclic_rw),
                                         &reg_value) == _ERROR)
                  {
                    return _ERROR;
                  }
                }
              }
              break;

            default:
              status = _ERROR;
              break;
          }
          break;

        case PSU_READ_VOLTAGE:
          /* read the ad_reg_otgdclic_rw register through i2c
             interface */
          if (psu_dcdc_i2c_read(i2cdev1,
                                (UNS_32)&(psudat.regptr->ad_reg_otgdclic_rw),
                                &reg_value) == _ERROR)
          {
            return _ERROR;
          }
          /*check whether DCDC1 or DCDC2 is selected */
          switch (((PSU_DCDC_VOLTAGE_T *) arg)->psu_dcdc_select)
          {
            case PSU_DCDC_DCDC1:
              /* get the DCDC1 control bits to the lower byte */
              volt_value = (reg_value &
                            OTGDCLIC_RW_DCDC_CTRL_DCDC1_ADJUST(7)) >>
                           OTGDCLIC_RW_DCDC_CTRL_DCDC1_ADJUST_POS;

              *(((PSU_DCDC_VOLTAGE_T *) arg)->psu_dcdc_range)
              = (PSU_DCDC_RANGE_T)volt_value;
              break;

            case PSU_DCDC_DCDC2:
              /* get the DCDC2 control bits to the lower byte */
              volt_value = (reg_value &
                            OTGDCLIC_RW_DCDC_CTRL_DCDC2_ADJUST(7)) >>
                           OTGDCLIC_RW_DCDC_CTRL_DCDC2_ADJUST_POS;

              *(((PSU_DCDC_VOLTAGE_T *) arg)->psu_dcdc_range)
              = (PSU_DCDC_RANGE_T)volt_value;
              break;

            default:
              status = _ERROR;
              break;
          }
          break;

        case PSU_SET_STABLE_CLOCK:
          /* read the ad_reg_otgdclic_rw register through i2c
             interface */
          if (psu_dcdc_i2c_read(i2cdev1,
                                (UNS_32)&(psudat.regptr->ad_reg_otgdclic_rw),
                                &reg_value) == _ERROR)
          {
            return _ERROR;
          }

          switch (arg)
          {
            case PSU_DCDC_CLK_OFF:
              reg_value &= ~((UNS_32)OTGDCLIC_RW_DCDC_CTRL_CLK_STABLE);
              /* write the ad_reg_otgdclic_rw register through i2c
                 interface */
              if (psu_dcdc_i2c_write(i2cdev1,
                                     (UNS_32)&(psudat.regptr->ad_reg_otgdclic_rw),
                                     &reg_value) == _ERROR)
              {
                return _ERROR;
              }
              break;

            case PSU_DCDC_CLK_ON:
              reg_value |= ((UNS_32)OTGDCLIC_RW_DCDC_CTRL_CLK_STABLE);
              /* write the ad_reg_otgdclic_rw register through i2c
                 interface */
              if (psu_dcdc_i2c_write(i2cdev1,
                                     (UNS_32)&(psudat.regptr->ad_reg_otgdclic_rw),
                                     &reg_value) == _ERROR)
              {
                return _ERROR;
              }
              break;

            default:
              status = _ERROR;
              break;
          }
          break;

        case PSU_ENABLE_LDO:
          /* read the ad_reg_mod_id register through i2c
             interface */
          if (psu_dcdc_i2c_read(i2cdev1,
                                (UNS_32)&(psudat.regptr->ad_reg_mod_id),
                                &minor_rev) == _ERROR)
          {
            return _ERROR;
          }
          minor_rev = (minor_rev & MOD_ID_MINOR_REVISION(15)) >>
                      MOD_ID_MINOR_REVISION_POS;

          /* read the ad_reg_otgdclic_rw register through i2c
             interface */
          if (psu_dcdc_i2c_read(i2cdev1,
                                (UNS_32)&(psudat.regptr->ad_reg_otgdclic_rw),
                                &reg_value) == _ERROR)
          {
            return _ERROR;
          }

          switch (arg)
          {
            case PSU_DCDC_LDO_OFF:
              if (minor_rev >= S_DCDC_AD_MINOR_VERSION_3)
              {
                reg_value |= ((UNS_32)OTGDCLIC_RW_DCDC_CTRL_LDO_ON);
              }
              else
              {
                reg_value &= ~((UNS_32)OTGDCLIC_RW_DCDC_CTRL_LDO_ON);
              }

              /* write the ad_reg_otgdclic_rw register through i2c
                 interface */
              if (psu_dcdc_i2c_write(i2cdev1,
                                     (UNS_32)&(psudat.regptr->ad_reg_otgdclic_rw),
                                     &reg_value) == _ERROR)
              {
                return _ERROR;
              }
              break;

            case PSU_DCDC_LDO_ON:
              if (minor_rev >= S_DCDC_AD_MINOR_VERSION_3)
              {
                reg_value &= ~((UNS_32)OTGDCLIC_RW_DCDC_CTRL_LDO_ON);
              }
              else
              {
                reg_value |= ((UNS_32)OTGDCLIC_RW_DCDC_CTRL_LDO_ON);
              }
              /* write the ad_reg_otgdclic_rw register through i2c
                 interface */
              if (psu_dcdc_i2c_write(i2cdev1,
                                     (UNS_32)&(psudat.regptr->ad_reg_otgdclic_rw),
                                     &reg_value) == _ERROR)
              {
                return _ERROR;
              }
              break;

            default:
              status = _ERROR;
              break;
          }
          break;

        case PSU_GET_STATUS:
          /* read the ad_reg_dcdclic_ro register through i2c
             interface */
          if (psu_dcdc_i2c_read(i2cdev1,
                                (UNS_32)&(psudat.regptr->ad_reg_dcdclic_ro),
                                &reg_value) == _ERROR)
          {
            return _ERROR;
          }

          switch (arg)
          {
            case PSU_DCDC_USB_DETECT:
              if (reg_value & DCDCLIC_RO_DCDC_CTRL_USB_DETECT)
              {
                status = 1;
              }
              else
              {
                status = 0;
              }
              break;

            case PSU_DCDC_DCDC2_MODE_CONT:
              if (reg_value & DCDCLIC_RO_DCDC_CTRL_INVCONT_1V8)
              {
                status = 1;
              }
              else
              {
                status = 0;
              }
              break;

            case PSU_DCDC_DCDC1_MODE_CONT:
              if (reg_value & DCDCLIC_RO_DCDC_CTRL_INVCONT_3V3)
              {
                status = 1;
              }
              else
              {
                status = 0;
              }
              break;

            case PSU_DCDC_DCDC2_MODE_DISCONT:
              if (reg_value & DCDCLIC_RO_DCDC_CTRL_INVDISC_1V8)
              {
                status = 1;
              }
              else
              {
                status = 0;
              }
              break;

            case PSU_DCDC_DCDC1_MODE_DISCONT:
              if (reg_value & DCDCLIC_RO_DCDC_CTRL_INVDISC_3V3)
              {
                status = 1;
              }
              else
              {
                status = 0;
              }
              break;

            case PSU_DCDC_DCDC2_MODE_INIT:
              if (reg_value & DCDCLIC_RO_DCDC_CTRL_INVINIT_1V8)
              {
                status = 1;
              }
              else
              {
                status = 0;
              }
              break;

            case PSU_DCDC_DCDC1_MODE_INIT:
              if (reg_value & DCDCLIC_RO_DCDC_CTRL_INVINIT_3V3)
              {
                status = 1;
              }
              else
              {
                status = 0;
              }
              break;

            case PSU_DCDC_DCDC2_MODE_RAMPUP:
              if (reg_value & DCDCLIC_RO_DCDC_CTRL_INVRAMP_1V8)
              {
                status = 1;
              }
              else
              {
                status = 0;
              }
              break;

            case PSU_DCDC_DCDC1_MODE_RAMPUP:
              if (reg_value & DCDCLIC_RO_DCDC_CTRL_INVRAMP_3V3)
              {
                status = 1;
              }
              else
              {
                status = 0;
              }
              break;

            default:
              /* Unsupported parameter */
              status = LPC_BAD_PARAMS;
          }
          break;

        case PSU_SET_1V8_CONTROL:
          /* read the ad_reg_otgdclic_rw register through i2c
             interface */
          if (psu_dcdc_i2c_read(i2cdev1,
                                (UNS_32)&(psudat.regptr->ad_reg_otgdclic_rw),
                                &reg_value) == _ERROR)
          {
            return _ERROR;
          }

          switch (arg)
          {
            case PSU_DCDC_LDO3_1V4:
              reg_value &= ~((UNS_32)OTGDCLIC_RW_DCDC_CTRL_SEL1V8);
              /* write the ad_reg_otgdclic_rw register through i2c
                 interface */
              if (psu_dcdc_i2c_write(i2cdev1,
                                     (UNS_32)&(psudat.regptr->ad_reg_otgdclic_rw),
                                     &reg_value) == _ERROR)
              {
                return _ERROR;
              }
              break;

            case PSU_DCDC_LDO3_1V8:
              reg_value |= ((UNS_32)OTGDCLIC_RW_DCDC_CTRL_SEL1V8);
              /* write the ad_reg_otgdclic_rw register through i2c
                 interface */
              if (psu_dcdc_i2c_write(i2cdev1,
                                     (UNS_32)&(psudat.regptr->ad_reg_otgdclic_rw),
                                     &reg_value) == _ERROR)
              {
                return _ERROR;
              }
              break;

            default:
              status = _ERROR;
              break;
          }
          break;

        case PSU_SET_USBOTG_CHARGE_PUMP_CTRL:
          switch (arg)
          {
            case PSU_DCDC_USBOTGCHARGEPUMPCTRL_OFF:
              /* read the ad_reg_otgdclic_rw register through i2c
                 interface */
              if (psu_dcdc_i2c_read(i2cdev1,
                                    (UNS_32)&(psudat.regptr->ad_reg_otgdclic_rw),
                                    &reg_value) == _ERROR)
              {
                return _ERROR;
              }

              reg_value &= ~((UNS_32)OTGDCLIC_RW_USBOTG_CTRL_PON);

              /* write the ad_reg_otgdclic_rw register through i2c
                 interface */
              if (psu_dcdc_i2c_write(i2cdev1,
                                     (UNS_32)&(psudat.regptr->ad_reg_otgdclic_rw),
                                     &reg_value) == _ERROR)
              {
                return _ERROR;
              }
              /* read the ad_reg_cgu_rw register through i2c
                 interface */
              if (psu_dcdc_i2c_read(i2cdev1,
                                    (UNS_32)&(psudat.regptr->ad_reg_cgu_rw),
                                    &reg_value) == _ERROR)
              {
                return _ERROR;
              }

              /* Power down USBOTG clock */
              reg_value |= ((UNS_32)CGU_RW_PD_USBOTG_CLK);

              /* write the ad_reg_cgu_rw register through i2c
                 interface */
              if (psu_dcdc_i2c_write(i2cdev1,
                                     (UNS_32)&(psudat.regptr->ad_reg_cgu_rw),
                                     &reg_value) == _ERROR)
              {
                return _ERROR;
              }
              break;

            case PSU_DCDC_USBOTGCHARGEPUMPCTRL_ON:
              /* read the ad_reg_otgdclic_rw register through i2c
                 interface */
              if (psu_dcdc_i2c_read(i2cdev1,
                                    (UNS_32)&(psudat.regptr->ad_reg_otgdclic_rw),
                                    &reg_value) == _ERROR)
              {
                return _ERROR;
              }

              reg_value |= ((UNS_32)OTGDCLIC_RW_USBOTG_CTRL_PON);

              /* write the ad_reg_otgdclic_rw register through i2c
                 interface */
              if (psu_dcdc_i2c_write(i2cdev1,
                                     (UNS_32)&(psudat.regptr->ad_reg_otgdclic_rw),
                                     &reg_value) == _ERROR)
              {
                return _ERROR;
              }

              /* read the ad_reg_cgu_rw register through i2c
                 interface */
              if (psu_dcdc_i2c_read(i2cdev1,
                                    (UNS_32)&(psudat.regptr->ad_reg_cgu_rw),
                                    &reg_value) == _ERROR)
              {
                return _ERROR;
              }

              /* Power up USBOTG clock */
              reg_value &= ~((UNS_32)CGU_RW_PD_USBOTG_CLK);

              /* write the ad_reg_cgu_rw register through i2c
                 interface */
              if (psu_dcdc_i2c_write(i2cdev1,
                                     (UNS_32)&(psudat.regptr->ad_reg_cgu_rw),
                                     &reg_value) == _ERROR)
              {
                return _ERROR;
              }
              break;

            default:
              status = _ERROR;
              break;
          }
          break;

        case PSU_GET_USBOTG_CHARGE_PUMP_CTRL:
          /* read the ad_reg_otgdclic_rw register through i2c
             interface */
          if (psu_dcdc_i2c_read(i2cdev1,
                                (UNS_32)&(psudat.regptr->ad_reg_otgdclic_rw),
                                &reg_value) == _ERROR)
          {
            return _ERROR;
          }

          reg_value = (reg_value &
                       OTGDCLIC_RW_USBOTG_CTRL_PON) >>
                      OTGDCLIC_RW_USBOTG_CTRL_PON_POS;

          *(PSU_DCDC_USBOTGCHARGEPUMPCTRL_T *)arg
          = (PSU_DCDC_USBOTGCHARGEPUMPCTRL_T)reg_value;
          break;

        default:
          /* Unsupported parameter */
          status = LPC_BAD_PARAMS;
      }
    }
  }

  return status;
}

/***********************************************************************
 *
 * Function: psu_read
 *
 * Purpose: PSU read function (stub only)
 *
 * Processing:
 *     Return 0 to the caller.
 *
 * Parameters:
 *     devid:     Pointer to PSU descriptor
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
INT_32 psu_read(INT_32 devid,
                void *buffer,
                INT_32 max_bytes)
{
  return 0;
}

/***********************************************************************
 *
 * Function: psu_write
 *
 * Purpose: PSU write function (stub only)
 *
 * Processing:
 *     Return 0 to the caller.
 *
 * Parameters:
 *     devid:   Pointer to PSU descriptor
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
INT_32 psu_write(INT_32 devid,
                 void *buffer,
                 INT_32 n_bytes)
{
  return 0;
}
