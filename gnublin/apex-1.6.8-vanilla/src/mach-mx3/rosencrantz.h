/* rosencrantz.h

   written by Marc Singer
   31 May 2007

   Copyright (C) 2007 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__ROSENCRANTZ_H__)
#    define   __ROSENCRANTZ_H__

/* ----- Includes */

#include "mx31-pins.h"

/* ----- Types */

/* ----- Globals */

/* ----- Prototypes */

#define PIN_BOARD_ID1		MX31_PIN_GPIO1_4
#define PIN_BOARD_ID2		MX31_PIN_GPIO1_6

#define PIN_ILLUMINATION_EN1	MX31_PIN_KEY_COL5
#define PIN_ILLUMINATION_EN2	MX31_PIN_KEY_COL4

	/* Definitions for GPIO pins to make the code readable. */
#define PIN_SENSOR_PWR_EN	MX31_PIN_CSI_D4
#define PIN_SENSOR_BUF_EN	MX31_PIN_KEY_ROW4
#define PIN_CMOS_STBY		MX31_PIN_KEY_ROW7
#define PIN_NCMOS_RESET		MX31_PIN_KEY_ROW6
#define PIN_CMOS_EXPOSURE	MX31_PIN_KEY_ROW5

#define IS_ROSENCRANTZ_FF \
  GPIO_PIN_VALUE (PIN_BOARD_ID1) && GPIO_PIN_VALUE (PIN_BOARD_ID2)

#define CCM_PDR0_V		(0x2c071d58) /* Custom CSI divider */

#endif  /* __ROSENCRANTZ_H__ */
