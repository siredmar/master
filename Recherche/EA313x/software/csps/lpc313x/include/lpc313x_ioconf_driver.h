/***********************************************************************
 * $Id:: lpc313x_ioconf_driver.h 1338 2008-11-20 23:03:38Z pdurgesh    $
 *
 * Project: LPC313X GPIO driver
 *
 * Description:
 *     This file contains driver support for the LPC313X GPIO block.
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

#ifndef LPC313X_IOCONF_DRIVER_H
#define LPC313X_IOCONF_DRIVER_H

#include "lpc313x_ioconf.h"

#ifdef __cplusplus
extern "C"
{
#endif


/***********************************************************************
 * GPIO driver functions
 **********************************************************************/

/* Returns current input states of the input pin */
static __inline BOOL_32 gpio_get_pin_state(IOCONF_BLOCK_T gpio_block, UNS_32 bitnum)
{
  /* If high, return TRUE. If low, return FALSE */
  return (IOCONF->block[gpio_block].pins & _BIT(bitnum));
}

/* Sets GPIO pin as input pin */
static __inline void gpio_set_as_input(IOCONF_BLOCK_T gpio_block, UNS_32 bitnum)
{
  IOCONF->block[gpio_block].mode1_clear = _BIT(bitnum);
  IOCONF->block[gpio_block].mode0_clear = _BIT(bitnum);
}

/* Sets GPIO output pin to high state */
static __inline void gpio_set_outpin_high(IOCONF_BLOCK_T gpio_block, UNS_32 bitnum)
{
  IOCONF->block[gpio_block].mode1_set = _BIT(bitnum);
  IOCONF->block[gpio_block].mode0_set = _BIT(bitnum);
}

/* Sets GPIO output pin to high state */
static __inline void gpio_set_outpin_low(IOCONF_BLOCK_T gpio_block, UNS_32 bitnum)
{
  IOCONF->block[gpio_block].mode1_set = _BIT(bitnum);
  IOCONF->block[gpio_block].mode0_clear = _BIT(bitnum);
}

/* Sets GPIO/MUX pin as driven by IP pin */
static __inline void gpio_set_as_ip_driven(IOCONF_BLOCK_T gpio_block, UNS_32 bitnum)
{
  IOCONF->block[gpio_block].mode1_clear = _BIT(bitnum);
  IOCONF->block[gpio_block].mode0_set = _BIT(bitnum);
}

/***********************************************************************
 * IOCONF Pin muxing driver functions
 **********************************************************************/


#ifdef __cplusplus
}
#endif

#endif /* LPC313X_IOCONF_DRIVER_H */
