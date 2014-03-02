/***********************************************************************
* $Id:: lpc315x_analog.h 6423 2011-02-10 07:21:33Z ing02124           $
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
#define AD_CGU_I2C_CLK_256FS_PD      _BIT(0)
#define AD_CGU_INT_CLK_DSP_PD        _BIT(1)
#define AD_CGU_INT_CLK_NS_PD         _BIT(2)
#define AD_CGU_DEC_CLK_PD            _BIT(3)
#define AD_CGU_I2STX_BCK_PD          _BIT(4)
#define AD_CGU_I2STX_SYSCLK_PD       _BIT(5)
#define AD_CGU_I2SRX_BCK_PD          _BIT(6)
#define AD_CGU_I2SRX_SYSCLK_PD       _BIT(7)
#define AD_CGU_LIC_CLK_PD            _BIT(8)
#define AD_CGU_AOUT_CLK_DAC_PD       _BIT(11)
#define AD_CGU_AIN_CLK_ADC1_PD       _BIT(12)
#define AD_CGU_AIN_CLK_ADC2_PD       _BIT(13)
#define AD_CGU_AIN_CLK_ADCSYS_PD     _BIT(14)
#define AD_CGU_INT_SEL_CLKNS_256FS   _BIT(16) /* 0 - 128FS, 1- 256FS */
#define AD_CGU_AOUT_SEL_CLKDAC_256FS _BIT(17) /* 0 - 128FS, 1- 256FS */
#define AD_CGU_AIN_SEL_ADC2_128FS    _BIT(18) /* 0-tied to 1,1-128FS */
#define AD_CGU_AIN_SEL_ADC1_OFF_1    _BIT(19) /* 1-tied to 1,0-128FS */
#define AD_CGU_AIN_SEL_ADCSYS_256FS  _BIT(20) /* 0 - 128FS,1 - 256FS */
#define AD_CGU_I2C_CLK_256FS         _BIT(21) /* 0-SYSCLK,1-CLK_256FS*/
/* Level Shifter in RTC: 0 - On, 1 - Off */
#define AD_CGU_RTC_LS_OFF            _BIT(22)
#define AD_CGU_DAC_CLK_PHASE         _BIT(23)

/***********************************************************************
 * Analog Input Control AIN0 Register definitions
 **********************************************************************/
#define AD_AIN0_CTRL_PGA_R(n)        (((n) & 0xF) << 0)
#define AD_AIN0_CTRL_PGA_R_POS       (0)
#define AD_AIN0_CTRL_PGA_M(n)        (((n) & 0xF) << 4)
#define AD_AIN0_CTRL_PGA_M_POS       (4)
#define AD_AIN0_CTRL_PGA_L(n)        (((n) & 0xF) << 8)
#define AD_AIN0_CTRL_PGA_L_POS       (8)

/***********************************************************************
 * Analog Input Control AIN1 Register definitions
 **********************************************************************/
#define AD_AIN1_CTRL_XFBADR          _BIT(0)
#define AD_AIN1_CTRL_XFBADL          _BIT(1)
#define AD_AIN1_CTRL_DITHERR         _BIT(2)
#define AD_AIN1_CTRL_DITHERL         _BIT(3)
#define AD_AIN1_CTRL_VCOM_REF1_PD    _BIT(4)
#define AD_AIN1_CTRL_BIAS1_PD        _BIT(5)
#define AD_AIN1_CTRL_ADCR_PD         _BIT(6)
#define AD_AIN1_CTRL_ADCL_PD         _BIT(7)
#define AD_AIN1_CTRL_SDCR_PD         _BIT(8)
#define AD_AIN1_CTRL_SDCM_PD         _BIT(9)
#define AD_AIN1_CTRL_SDCL_PD         _BIT(10)
#define AD_AIN1_CTRL_PGAR_PD         _BIT(11)
#define AD_AIN1_CTRL_PGAM_PD         _BIT(12)
#define AD_AIN1_CTRL_PGAL_PD         _BIT(13)
#define AD_AIN1_CTRL_LNA_PD          _BIT(14)
#define AD_AIN1_CTRL_MUX_R(n)        (((n) & 0x3) << 15)
#define AD_AIN1_CTRL_MUX_R_POS       (15)
#define AD_AIN1_CTRL_MUX_L(n)        (((n) & 0x3) << 17)
#define AD_AIN1_CTRL_MUX_L_POS       (17)

/***********************************************************************
 * Decimator Control Register definitions
 **********************************************************************/
#define AD_DEC_CTRL_GAIN_R(n)        (((n) & 0xFF) << 0)
#define AD_DEC_CTRL_GAIN_R_POS       (0)
#define AD_DEC_CTRL_GAIN_L(n)        (((n) & 0xFF) << 8)
#define AD_DEC_CTRL_GAIN_L_POS       (0)
#define AD_DEC_CTRL_EN_DEL_DBLIN     _BIT(16)
#define AD_DEC_CTRL_EN_DBLIN         _BIT(17)
#define AD_DEC_CTRL_EN_DCFILTO       _BIT(18)
#define AD_DEC_CTRL_EN_DCFILTI       _BIT(19)
#define AD_DEC_CTRL_EN_POL_INV       _BIT(20)
#define AD_DEC_CTRL_EN_MUTE          _BIT(21)
#define AD_DEC_CTRL_EN_AGC           _BIT(22)
#define AD_DEC_CTRL_AGCLVL(n)        (((n) & 0x3) << 23)
#define AD_DEC_CTRL_AGCLVL_POS       (23)
#define AD_DEC_CTRL_AGCTIM(n)        (((n) & 0x7) << 25)
#define AD_DEC_CTRL_AGCTIM_POS       (25)

/***********************************************************************
 * Analog Output Control Register definitions
 **********************************************************************/
#define AD_AOUT_CTRL_AVCR_PD         _BIT(0)
#define AD_AOUT_CTRL_AVCL_PD         _BIT(1)
#define AD_AOUT_CTRL_SET_DWA         _BIT(2)
#define AD_AOUT_CTRL_I2S_FMT         _BIT(3)
#define AD_AOUT_CTRL_SDAC_R_PD       _BIT(4)
#define AD_AOUT_CTRL_SDAC_L_PD       _BIT(5)
#define AD_AOUT_CTRL_CUR_LMT_R(n)    (((n) & 0x3) << 6)
#define AD_AOUT_CTRL_CUR_LMT_R_POS   (6)
#define AD_AOUT_CTRL_CUR_LMT_C(n)    (((n) & 0x3) << 8)
#define AD_AOUT_CTRL_CUR_LMT_C_POS   (8)
#define AD_AOUT_CTRL_CUR_LMT_L(n)    (((n) & 0x3) << 10)
#define AD_AOUT_CTRL_CUR_LMT_L_POS   (10)
#define AD_AOUT_CTRL_HP_R_PD         _BIT(12)
#define AD_AOUT_CTRL_HP_C_PD         _BIT(13)
#define AD_AOUT_CTRL_HP_L_PD         _BIT(14)
#define AD_AOUT_CTRL_VREF_SLOW_PD    _BIT(15)
#define AD_AOUT_CTRL_GAIN_AVC_COA    (((n) & 0x7FF) << 16)
#define AD_AOUT_CTRL_GAIN_AVC_COA_MSK (0x7FF << 16)
#define AD_AOUT_CTRL_GAIN_AVC_COA_POS (16)
#define AD_AOUT_CTRL_GAIN_AVC_FIN    (((n) & 0x7) << 27)
#define AD_AOUT_CTRL_GAIN_AVC_FIN_MSK (0x7 << 27)
#define AD_AOUT_CTRL_GAIN_AVC_FIN_POS (27)
#define AD_AOUT_CTRL_VREF_SLOW_UP    _BIT(30)
#define AD_AOUT_CTRL_SWDAC_ON        _BIT(31)

/***********************************************************************
 * Interpolator Control 0 Register definitions
 **********************************************************************/
#define AD_INT0_CTRL_DEEMP_CHAN1     (((n) & 0x7) << 0)
#define AD_INT0_CTRL_DEEMP_CHAN1_POS (0)
#define AD_INT0_CTRL_SET_SIL         _BIT(3)
#define AD_INT0_CTRL_SIL_WIN_TIM     (((n) & 0x3) << 4)
#define AD_INT0_CTRL_SIL_WIN_TIM_POS (4)
#define AD_INT0_CTRL_SIL_DET_EN      _BIT(6)
#define AD_INT0_CTRL_DAC_PD          _BIT(7)
#define AD_INT0_CTRL_SET_PD_SLOPE    _BIT(8)
#define AD_INT0_CTRL_SET_NS          (((n) & 0x3) << 9)
#define AD_INT0_CTRL_SET_NS_POS      (9)
#define AD_INT0_CTRL_SET_IN_RATE     (((n) & 0x3) << 11)
#define AD_INT0_CTRL_SET_IN_RATE_POS (11)
#define AD_INT0_CTRL_SET_FILT_COEFF  (((n) & 0x3) << 13)
#define AD_INT0_CTRL_SET_FILT_COEFF_POS (13)
#define AD_INT0_CTRL_POL_INV         _BIT(15)

/***********************************************************************
 * Interpolator Control 1 Register definitions
 **********************************************************************/
#define AD_INT1_CTRL_MUTE           _BIT(16)

#ifdef __cplusplus
}
#endif

#endif /* LPC315X_INTC_H */
