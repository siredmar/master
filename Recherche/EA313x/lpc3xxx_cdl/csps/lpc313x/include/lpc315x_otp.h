/***********************************************************************
* $Id:: lpc315x_otp.h 2530 2009-11-17 00:39:08Z pdurgesh              $
*
* Project: LPC315X One Time Programmable Memory (OTP) definitions
*
* Description:
*     This file contains the structure definitions and manifest
*     constants for the LPC315x chip family component:
*         One Time Programmable Memory (OTP)
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

#ifndef LPC315X_OTP_H
#define LPC315X_OTP_H

#include "lpc_types.h"
#include "lpc313x_chip.h"

#ifdef __cplusplus
extern "C"
{
#endif

/***********************************************************************
* One Time Programmable Memory (OTP) Register Structures
**********************************************************************/

/* Preset One Time Programmable Memory (OTP) layout Structures*/
typedef volatile struct
{
  volatile UNS_32 unique_id[4]; // 128bit unique id set in FAB
  volatile UNS_32 aes_key[4];   // 128bit aes key
  volatile UNS_32 user[6];      // free user fuses
  volatile UNS_16 usb_pid;    // USB PID used by bootrom
  volatile UNS_16 usb_vid;    // USB VIDs used by bootrom
  volatile UNS_32 chip_cfg;   // Chip Configuration settings
} OTP_DATA_T;

/* One Time Programmable Memory (OTP) Module Register Structure */
typedef volatile struct
{
  volatile UNS_32 con;    // control register (R/W)
  volatile UNS_32 rprot;    // read protection register (R/W)
  volatile UNS_32 wprot;    // write protection register (R/W)
  union
  {
    volatile const UNS_32 data[16]; // otp data register (R)
    OTP_DATA_T map;
  } d;
} OTP_REGS_T;

#define OTP_TOTAL_FUSES_NUM     512
/**********************************************************************
* OTP control register definitions
**********************************************************************/
/* This sticky bit to enable the ARM_JTAG clock */
#define OTP_CON_ARM_JTAG_EN       _BIT(31)
#define OTP_CON_MODE_MASK         _SBF(16, 0x3)
#define OTP_CON_IDLE_MODE         _SBF(16, 0x0)
#define OTP_CON_COPY_MODE         _SBF(16, 0x1)
#define OTP_CON_WRITE_MODE        _SBF(16, 0x2)
#define OTP_CON_16B_ADDR(n)       (((n) & 0x1F) << 4)
#define OTP_CON_ADDR(n)           ((n) & 0x1FF)
#define OTP_CON_ADDR_MASK         0x000001FF

/**********************************************************************
* OTP Read protect register definitions
**********************************************************************/
#define OTP_RPORT_PROTECT_ALL    _BIT(31)
#define OTP_RPORT_PROTECT(n)     _BIT((n))

/**********************************************************************
* OTP Write protect register definitions
**********************************************************************/
#define OTP_WRPORT_PROTECT_ALL    _BIT(31)
#define OTP_WRPORT_PROTECT(n)     _BIT((n))

/**********************************************************************
* Preset(based on bootROM assumptions) OTP fuse data layout definitions
**********************************************************************/
/* chip_cfg register definitions */
#define OTPD_CFG_SEC_MASK         _SBF(29, 0x7)
#define OTPD_CFG_SEC_JTAG_EN      _SBF(29, 0x0)
#define OTPD_CFG_SEC_JTAG_APP_EN  _SBF(29, 0x1)
#define OTPD_CFG_SEC_JTAG_NXP_EN  _SBF(29, 0x3)
#define OTPD_CFG_SEC_JTAG_DISABLE _SBF(29, 0x7)
#define OTPD_CFG_BANDGAP_ADJ_MASK _SBF(25, 0xF)
#define OTPD_CFG_BANDGAP_ADJ_GET(n) (((n) >> 25) & 0xF)
#define OTPD_CFG_AES_VALID        _BIT(24)
#define OTPD_CFG_PIDVID_VALID     _BIT(23)
#define OTPD_CFG_DFU_FALLTHRU     _BIT(22)

/* usb_id register definitions */
#define OTPD_USB_PID_MASK         0xFFFF
#define OTPD_USB_PID_GET(n)       ((n) & 0xFFFF)
#define OTPD_USB_VID_MASK         _SBF(16, 0xFFFF)
#define OTPD_USB_VID_GET(n)       (((n) >> 16) & 0xFFFF)

/**********************************************************************
* Macro to access OTP registers
**********************************************************************/
#define OTP   ((OTP_REGS_T*)OTP_BASE)

#ifdef __cplusplus
}
#endif

#endif /* LPC315X_OTP_H */
