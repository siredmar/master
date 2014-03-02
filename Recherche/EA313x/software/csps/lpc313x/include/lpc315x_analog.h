/***********************************************************************
* $Id:: lpc315x_analog.h 1180 2008-09-11 19:32:40Z pdurgesh           $
*
* Project: LPC315X Analog Die definitions
*
* Description:
*     This file contains the structure definitions and manifest
*     constants for the LPC315x chip family component:
*         Analog Die
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

#ifndef LPC315X_INTC_H
#define LPC315X_INTC_H

#include "lpc_types.h"
#include "lpc313x_chip.h"
#include "lpc313x_i2c.h"

#ifdef __cplusplus
extern "C"
{
#endif
/***********************************************************************
* Analog die I2C constants
**********************************************************************/
#define AD_I2C_WRITE_ADR    (_SBF(1, 0xC) | I2C_TXFF_START_CND)
#define AD_I2C_READ_ADR     (_SBF(1, 0xC) | 0x1 | I2C_TXFF_START_CND)

/***********************************************************************
* Analog die I2C Register offsets
**********************************************************************/

#define    AD_REG_OTGDCLIC_RW                            0x0000
#define    AD_REG_DCDCLIC_RO                             0x0001
#define    AD_REG_CGU_RW                                 0x0002
#define    AD_REG_AIN_0_RW                               0x0010
#define    AD_REG_AIN_1_RW                               0x0011
#define    AD_REG_AOUT_RW                                0x0012
#define    AD_REG_DEC_RW                                 0x0013
#define    AD_REG_INT_0_RW                               0x0014
#define    AD_REG_INT_1_RW                               0x0015
#define    AD_REG_DAIOMUX_RW                             0x0016
#define    AD_REG_AOUTDECINT_RO                          0x0017
#define    AD_REG_RTC_TIME                               0x0020
#define    AD_REG_RTC_ALARM_TIME                         0x0021
#define    AD_REG_RTC_STATUS                             0x0022
#define    AD_REG_RTC_SET_ENA_STAT                       0x0023
#define    AD_REG_RTC_CLR_ENA_STAT                       0x0024
#define    AD_REG_MOD_ID                                 0x03FF

#define    AD_REGS_NUMBER                                17

/***********************************************************************
 * OTGDCLIC register definitions
 **********************************************************************/

/***********************************************************************
 * CGU register definitions
 **********************************************************************/
#define AD_CGU_DAC_CLK_PHASE        _BIT(23)
#define AD_CGU_RTC_LS_OFF           _BIT(22)
#define AD_CGU_I2C_CLK_256FS        _BIT(21)



#ifdef __cplusplus
}
#endif

#endif /* LPC315X_INTC_H */
