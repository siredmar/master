/***********************************************************************
 * $Id:: lpc315x_ad.h 1371 2008-11-26 18:45:16Z usb01267              $
 *
 * Project: LPC315x AD definitions
 *
 * Description:
 *     This file contains the structure definitions and manifest
 *     constants for the LPC315x chip family component:
 *         AD
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

#ifndef LPC315X_AD_H
#define LPC315X_AD_H

#include "lpc_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**********************************************************************
* AD register structures
**********************************************************************/

/* AD module register structures */
typedef struct
{
  volatile UNS_32 ad_reg_otgdclic_rw;
  volatile UNS_32 ad_reg_dcdclic_ro;
  volatile UNS_32 ad_reg_cgu_rw;
  volatile UNS_32 rsrved1[13];
  volatile UNS_32 ad_reg_ain_0_rw;
  volatile UNS_32 ad_reg_ain_1_rw;
  volatile UNS_32 ad_reg_aout_rw;
  volatile UNS_32 ad_reg_dec_rw;
  volatile UNS_32 ad_reg_int_0_rw;
  volatile UNS_32 ad_reg_int_1_rw;
  volatile UNS_32 ad_reg_daiomux_rw;
  volatile UNS_32 ad_reg_aoutdecint_ro;
  volatile UNS_32 rsrved2[8];
  volatile UNS_32 ad_reg_rtc_time;
  volatile UNS_32 ad_reg_rtc_alarm_time;
  volatile UNS_32 ad_reg_rtc_status;
  volatile UNS_32 ad_reg_rtc_set_ena_sta;
  volatile UNS_32 ad_reg_rtc_clr_ena_sta;
  volatile UNS_32 rsrved3[986];
  volatile UNS_32 ad_reg_mod_id;
} AD_REGS_T;

/**********************************************************************
* AD_REG_OTGDCLIC_RW register definitions
**********************************************************************/
/* Switch on USBOTG charge pump */
#define OTGDCLIC_RW_USBOTG_CTRL_PON                _BIT(28)
#define OTGDCLIC_RW_USBOTG_CTRL_PON_POS            28
/* DCDC will run on the digital die clock */
#define OTGDCLIC_RW_DCDC_CTRL_CLK_STABLE           _BIT(24)
/* Switch on LDO1 and LDO2 */
#define OTGDCLIC_RW_DCDC_CTRL_LDO_ON               _BIT(23)
/* DCDC1 voltage setting */
#define OTGDCLIC_RW_DCDC_CTRL_DCDC1_ADJUST(n)      (((n) & 0x7) << 20)
#define OTGDCLIC_RW_DCDC_CTRL_DCDC1_ADJUST_POS     20
/* DCDC2 voltage setting */
#define OTGDCLIC_RW_DCDC_CTRL_DCDC2_ADJUST(n)      (((n) & 0x7) << 17)
#define OTGDCLIC_RW_DCDC_CTRL_DCDC2_ADJUST_POS     17
/* LDO3 produces 1.8V */
#define OTGDCLIC_RW_DCDC_CTRL_SEL1V8               _BIT(16)
/* Start charger */
#define OTGDCLIC_RW_LIC_CTRL_CHARGE_ENABLE         _BIT(12)
/* Compensation setting bits */
#define OTGDCLIC_RW_LIC_CTRL_CS_BIT(n)             (((n) & 0xF) << 8)
#define OTGDCLIC_RW_LIC_CTRL_CS_BIT_POS            8
/* Die temperature control loop gain setting */
#define OTGDCLIC_RW_LIC_CTRL_G_BIT(n)              (((n) & 0x7) << 5)
#define OTGDCLIC_RW_LIC_CTRL_G_BIT_POS             5
/* Die temperature control loop threshold setting */
#define OTGDCLIC_RW_LIC_CTRL_TT_BIT(n)             (((n) & 0x7) << 2)
#define OTGDCLIC_RW_LIC_CTRL_TT_BIT_POS            2
/* Power on die temperature control loop */
#define OTGDCLIC_RW_LIC_CTRL_PONTLIM               _BIT(1)
/* Power on vntc(battery) sensing */
#define OTGDCLIC_RW_LIC_CTRL_PONTBAT               _BIT(0)

/**********************************************************************
* AD_REG_DCDCLIC_RO register definitions
**********************************************************************/
/* 5V USB bus detected */
#define DCDCLIC_RO_DCDC_CTRL_USB_DETECT            _BIT(24)
/* DCDC2 is not running in continuous conduction mode */
#define DCDCLIC_RO_DCDC_CTRL_INVCONT_1V8           _BIT(23)
/* DCDC1 is not running in continuous conduction mode */
#define DCDCLIC_RO_DCDC_CTRL_INVCONT_3V3           _BIT(22)
/* DCDC2 is not running in discontinuous conduction mode */
#define DCDCLIC_RO_DCDC_CTRL_INVDISC_1V8           _BIT(21)
/* DCDC1 is not running in discontinuous conduction mode */
#define DCDCLIC_RO_DCDC_CTRL_INVDISC_3V3           _BIT(20)
/* DCDC2 is not running in initialization mode */
#define DCDCLIC_RO_DCDC_CTRL_INVINIT_1V8           _BIT(19)
/* DCDC1 is not running in initialization mode */
#define DCDCLIC_RO_DCDC_CTRL_INVINIT_3V3           _BIT(18)
/* DCDC2 is not running in ramp up mode */
#define DCDCLIC_RO_DCDC_CTRL_INVRAMP_1V8           _BIT(17)
/* DCDC1 is not running in ramp up mode */
#define DCDCLIC_RO_DCDC_CTRL_INVRAMP_3V3           _BIT(16)
/* Charger on indicator */
#define DCDCLIC_RO_LIC_CTRL_CHARGER_ON             _BIT(6)
/* Battery full indicator */
#define DCDCLIC_RO_LIC_CTRL_BATTERY_FULL           _BIT(5)
/* Trickle-charge indicator */
#define DCDCLIC_RO_LIC_CTRL_TRICKLE_CHARGE         _BIT(4)
/* Fast-charge indicator (or CC-charge indicator) */
#define DCDCLIC_RO_LIC_CTRL_FAST_CHARGE            _BIT(3)
/* CV-charge indicator */
#define DCDCLIC_RO_LIC_CTRL_CV_CHARGE              _BIT(2)
/* No-ntc indicator */
#define DCDCLIC_RO_LIC_CTRL_NO_NTC                 _BIT(1)
/* Tempbat-ok indicator */
#define DCDCLIC_RO_LIC_CTRL_TEMPBAT_OK             _BIT(0)

/**********************************************************************
* AD_REG_CGU_RW register definitions
**********************************************************************/
/* Power down DCDC clock */
#define CGU_RW_PD_DCDC_CLK                         _BIT(10)
/* Power down USBOTG charge pump clock */
#define CGU_RW_PD_USBOTG_CLK                       _BIT(9)
/* Power down LIC clock */
#define CGU_RW_PD_LIC_CLK                          _BIT(8)

/**********************************************************************
* AD_REG_MOD_ID register definitions
**********************************************************************/
/* ID */
#define MOD_ID_ID(n)                               (((n) & 0xFFFF) << 16)
#define MOD_ID_ID_POS                              16
/* MAJOR_REVISION */
#define MOD_ID_MAJOR_REVISION(n)                   (((n) & 0xF) << 12)
#define MOD_ID_MAJOR_REVISION_POS                  12
/* MINOR_REVISION */
#define MOD_ID_MINOR_REVISION(n)                   (((n) & 0xF) << 8)
#define MOD_ID_MINOR_REVISION_POS                  8
/* APERTURE_SIZE */
#define MOD_ID_APERTURE_SIZE(n)                    (((n) & 0xFF) << 0)
#define MOD_ID_APERTURE_SIZE_POS                   0

/* Macro pointing to PSU registers */
#define AD ((AD_REGS_T *)(0))

#ifdef __cplusplus
}
#endif

#endif /* LPC315X_AD_H */
