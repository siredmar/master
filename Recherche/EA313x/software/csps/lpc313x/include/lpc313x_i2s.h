/***********************************************************************
* $Id:: lpc313x_i2s.h 2027 2009-07-22 22:56:51Z wellsk                 $
*
* Project: LPC313X I2S controller definitions
*
* Description:
*     This file contains the structure definitions and manifest
*     constants for the LPC313X chip family component:
*         I2S Controllers present in Audio Subsystem (ADSS)
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

#ifndef LPC313X_I2S_H
#define LPC313X_I2S_H

#include "lpc_types.h"
#include "lpc313x_chip.h"

#ifdef __cplusplus
extern "C"
{
#endif

/***********************************************************************
* Audio Subsystem (ADSS) Register Structures
**********************************************************************/

/* I2S Controller Module Register Structure */
typedef volatile struct
{
  volatile UNS_32 left_16;
  volatile UNS_32 right_16;
  volatile UNS_32 left_24;
  volatile UNS_32 right_24;
  volatile UNS_32 int_status;
  volatile UNS_32 int_mask;
  volatile UNS_32 __reserved0;
  volatile UNS_32 __reserved1;
  volatile UNS_32 left_32[8];
  volatile UNS_32 right_32[8];
  volatile UNS_32 interleaved[8];

} I2S_REGS_T;

/* Audio Subsystem configuration Register Structure */
typedef volatile struct
{
  volatile UNS_32  i2s_format_settings;
  volatile UNS_32  audioss_mux_settings;
  volatile UNS_32  reserved[2];
  volatile UNS_32  n_sof_counter;

} AUDIOSS_REGS_T;

#ifdef __cplusplus
}
#endif

#endif /* LPC313X_I2S_H */
