/* linux/arch/arm/mach-lpc31xx/include/mach/gpio.h
 *
 * Copyright (c) 2012 Hans-Frieder Vogt <hfvogt@gmx.net>
 *
 * LPC313X GPIO defines
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#ifndef _LPC313X_GPIO_H
#define _LPC313X_GPIO_H

#define __ARM_GPIOLIB_COMPLEX

#include <mach/gpio-lpc313x.h>

/* gpiolib fastpath and aliases */

#define IOCONF_FAST_GPIO IOCONF_GPIO
#define NUM_FAST_GPIO LPC313X_GPIO_GPIO_MAX

/*
 * The following functions provide "fastpath" access to the
 * first group of pins, which are all exclusive GPIOs without
 * device functions.
 *
 * Access to these pins will be faster than for other pins
 * at the cost of proper gpiolib error handling.
 *
 * If you must use bit banging, use these pins preferentially.
 */

static inline int gpio_get_value(unsigned gpio)
{
	if (gpio < NUM_FAST_GPIO) {
		int real_pin = gpio_pins_gpio_map[gpio];
		if (real_pin >= 0) {
			if (GPIO_STATE(IOCONF_FAST_GPIO) & (1 << real_pin))
				return 1;
			else
				return 0;
		} else
			return -EINVAL;
	} else
		return __gpio_get_value(gpio);
}

/* it is assumed here that the GPIO pin is already set to direction output */
static inline void gpio_set_value(unsigned gpio, int value)
{
	if (gpio < NUM_FAST_GPIO) {
		int real_pin = gpio_pins_gpio_map[gpio];
		if (real_pin >= 0) {
			if (value)
				GPIO_M0_SET(IOCONF_FAST_GPIO) = 1 << real_pin;
			else
				GPIO_M0_RESET(IOCONF_FAST_GPIO) = 1 << real_pin;
		}
	} else {
		__gpio_set_value(gpio, value);
	}
}

static inline int gpio_cansleep(unsigned gpio)
{
	/* gpio_cansleep() is non-trivial on lpc313x */
	return __gpio_cansleep(gpio);
}

static inline int gpio_to_irq(unsigned gpio)
{
	/* gpio_to_irq() is non-trivial on lpc313x */
	return __gpio_to_irq(gpio);
}

#endif /*_LPC313X_GPIO_H*/
