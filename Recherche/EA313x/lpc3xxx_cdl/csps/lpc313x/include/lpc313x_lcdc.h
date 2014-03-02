/***********************************************************************
* $Id:: lpc313x_lcdc.h 1537 2009-01-13 00:00:54Z pdurgesh             $
*
* Project: LPC313X LCD Interface controller definitions
*
* Description:
*     This file contains the structure definitions and manifest
*     constants for the LPC313X chip family component:
*         LCD Interface Controller
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

#ifndef LPC313X_LCDC_H
#define LPC313X_LCDC_H

#include "lpc_types.h"
#include "lpc313x_chip.h"

#ifdef __cplusplus
extern "C"
{
#endif

/***********************************************************************
* LCD Interface controller Register Structures
**********************************************************************/

/* LCD Interface Controller Module Register Structure */
typedef volatile struct
{
  // LSB
  volatile const UNS_32 status; // Status register (RO)
  volatile UNS_32 control;      // Control register (R/W)
  volatile const UNS_32 int_raw;// Interrupt without masking Status register (RO)
  volatile UNS_32 int_clear;    // Interrupt Clear register (WO)
  volatile UNS_32 int_mask;     // Interrupt Masking register (R/W)
  volatile UNS_32 read_cmd;     // Read Command register (WO)
  volatile UNS_32 _unused1[2];
  volatile UNS_32 inst_byte;    // Instruction byte register (R/W)
  volatile UNS_32 _unused2[3];
  volatile UNS_32 data_byte;    // Data byte register (R/W)
  volatile UNS_32 _unused3[3];
  volatile UNS_32 inst_word;    // Instruction word register (WO)
  volatile UNS_32 _unused4[15];
  volatile UNS_32 data_word;    // Data word register (WO)
  // MSB
} LCD_INTERFACE_REGS_T;

/***********************************************************************
* status register defines
***********************************************************************/
#define LCD_STS_COUNT_GET(n)    (((n) >> 5) & 0x1F)
#define LCD_STS_BUSY            _BIT(4)
#define LCD_STS_READ_VALID      _BIT(3)
#define LCD_STS_FIFO_OVRN       _BIT(2)
#define LCD_STS_FIFO_HALF       _BIT(1)
#define LCD_STS_FIFO_EMPTY      _BIT(0)

/***********************************************************************
* Control register defines
***********************************************************************/
#define LCD_CTRL_BYASYNC        _BIT(20)
#define LCD_CTRL_IF_16          _BIT(19)
#define LCD_CTRL_LOOPBACK       _BIT(18)
#define LCD_CTRL_MSB_FIRST      _BIT(17)
#define LCD_CTRL_INVERT_E_RD    _BIT(16)
#define LCD_CTRL_INVERT_CS      _BIT(15)
#define LCD_CTRL_BUSY_RS        _BIT(14)
#define LCD_CTRL_BUSY_BIT_NR(n) _SBF(10, ((n) & 0xF))
#define LCD_CTRL_BUSY_FLAG      _BIT(9)
#define LCD_CTRL_BUSY_CHECK_EN  _BIT(8)
#define LCD_CTRL_SERIAL_READ_0  _SBF(6, 0x0)
#define LCD_CTRL_SERIAL_READ_25 _SBF(6, 0x1)
#define LCD_CTRL_SERIAL_READ_50 _SBF(6, 0x2)
#define LCD_CTRL_SERIAL_READ_75 _SBF(6, 0x3)
#define LCD_CTRL_SERIAL_MODE_0  _SBF(4, 0x0)
#define LCD_CTRL_SERIAL_MODE_1  _SBF(4, 0x1)
#define LCD_CTRL_SERIAL_MODE_2  _SBF(4, 0x2)
#define LCD_CTRL_SERIAL_MODE_3  _SBF(4, 0x3)
#define LCD_CTRL_IF_8           0
#define LCD_CTRL_IF_4           _BIT(3)
#define LCD_CTRL_8080           0
#define LCD_CTRL_6800           _BIT(2)
#define LCD_CTRL_PS             _BIT(1)


/**********************************************************************
* Macro to access LCD I/F registers
**********************************************************************/
#define LCD_IF       ((LCD_INTERFACE_REGS_T*)LCD_INTERFACE_BASE)

#ifdef __cplusplus
}
#endif


#endif /* LPC313X_LCDC_H */
