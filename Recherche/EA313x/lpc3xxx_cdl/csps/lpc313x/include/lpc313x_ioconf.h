/***********************************************************************
* $Id:: lpc313x_ioconf.h 1180 2008-09-11 19:32:40Z pdurgesh           $
*
* Project: LPC313X IO Configuration (IOCONF) controller definitions
*
* Description:
*     This file contains the structure definitions and manifest
*     constants for the LPC313X chip family component:
*         IO Configuration (IOCONF) controller
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

#ifndef LPC313X_IOCONF_H
#define LPC313X_IOCONF_H

#include "lpc_types.h"
#include "lpc313x_chip.h"

#ifdef __cplusplus
extern "C"
{
#endif

/***********************************************************************
* IO Configuration (IOCONF) controller Register Structures
**********************************************************************/

/* IOCONF Function Block Register Structure */
typedef volatile struct
{
  volatile UNS_32 pins;         /* R- 0x00 : current status of pins */
  volatile UNS_32 _d1[3];
  volatile UNS_32 mode0;        /* RW 0x10 : */
  volatile UNS_32 mode0_set;    /* RW 0x14 : */
  volatile UNS_32 mode0_clear;  /* RW 0x18 : */
  volatile UNS_32 _d2[1];
  volatile UNS_32 mode1;        /* RW 0x20 : */
  volatile UNS_32 mode1_set;    /* RW 0x24 : */
  volatile UNS_32 mode1_clear;  /* RW 0x28 : */
  volatile UNS_32 _d3[5];
} IOCONF_FUNC_REGS_T;

/* IOCONF Register Structure */
typedef volatile struct
{
  IOCONF_FUNC_REGS_T block[13];
} IOCONF_REGS_T;

typedef enum
{
  IOCONF_MUX0,
  IOCONF_MUX1,
  IOCONF_CGU,
  IOCONF_DAI0,
  IOCONF_DAI1,
  IOCONF_DAO1,
  IOCONF_EBI,
  IOCONF_GPIO,
  IOCONF_I2C1,
  IOCONF_SPI,
  IOCONF_NAND_CTRL,
  IOCONF_PWM,
  IOCONF_UART
} IOCONF_BLOCK_T;

/**********************************************************************
*  GPIO PIN defines
**********************************************************************/


/**********************************************************************
* Macro to access IOCONF registers
**********************************************************************/
#define IOCONF      ((IOCONF_REGS_T*)IOCONF_BASE)

#ifdef __cplusplus
}
#endif

#endif /* LPC313X_IOCONF_H */
