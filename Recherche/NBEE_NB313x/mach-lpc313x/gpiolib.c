/*
* Author: Miguel Angel Ajo <miguelangel@nbee.es>
*
* Copyright (C) 2010 NBEE Embedded Systems S.L.
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
*/

/*
* This is a very simple wrapper for gpio access on LPC31xx platforms.
* right now it only includes support for GPIO0..GPIO20, but it can be
* extended quite easily putting new gpio_names, and groups inside of 
* gpio_ids structure 
*/

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/errno.h>
#include <linux/gpio.h>
#include <asm/gpio.h>

#include <mach/hardware.h>


/*
 * GPIO names
 */
#define lpc31xx_gpiolib_MAX 21

static char *gpio_names[lpc31xx_gpiolib_MAX] = {
	"gpio0","gpio1","gpio2","gpio3","gpio4","gpio5",
	"gpio6","gpio7","gpio8","gpio9","gpio10","gpio11","gpio12",
	"gpio13","gpio14","gpio15","gpio16","gpio17","gpio18","gpio19",
	"gpio20"
};

#define lpc31xx_gpiolib_GRP 0

unsigned gpio_ids[lpc31xx_gpiolib_MAX] = {
	/* lpc31xx_gpiolib_GRP */
	GPIO_GPIO0, GPIO_GPIO1, GPIO_GPIO2, GPIO_GPIO3, GPIO_GPIO4,
	GPIO_MGPIO5, GPIO_MGPIO6, GPIO_MGPIO7, GPIO_MGPIO8, GPIO_MGPIO9,
	GPIO_MGPIO10,GPIO_GPIO11,GPIO_GPIO12,GPIO_GPIO13,GPIO_GPIO14,
	GPIO_GPIO15,GPIO_GPIO16,GPIO_GPIO17,GPIO_GPIO18,GPIO_GPIO19,
	GPIO_GPIO20,
	/* LPC31XX_xxxxx_GRP <-- extend from here*/  
};


/*
 * GENERIC_GPIO primitives.
 */
static int lpc31xx_gpiolib_dir_input(struct gpio_chip *chip,	unsigned pin)
{
	gpio_direction_input(gpio_ids[chip->base + pin]);
	return 0;
}


static int lpc31xx_gpiolib_get_value(struct gpio_chip *chip, unsigned pin)
{
	return lpc31xx_gpio_get_value(gpio_ids[chip->base + pin]);
}

static int lpc31xx_gpiolib_dir_output(struct gpio_chip *chip, unsigned pin,
	int value)
{

	if (gpio_ids[chip->base+pin]==GPIO_GPIO4)
	{
		value = 0;   // Value != 0 for GPIO4 crashes the system...
	}

	lpc31xx_gpio_set_value(gpio_ids[chip->base + pin],value);
	return 0;
}


static void lpc31xx_gpiolib_set_value(struct gpio_chip *chip, unsigned pin,
	int value)
{
	if (gpio_ids[chip->base+pin]==GPIO_GPIO4)
	{
		value = 0;   // Value != 0 for GPIO4 crashes the system...
	}
	
	lpc31xx_gpio_set_value(gpio_ids[chip->base + pin],value);
}


static int lpc31xx_gpiolib_request(struct gpio_chip *chip, unsigned pin)
{
	if (pin < chip->ngpio)
		return 0;
	return -EINVAL;
}




struct gpio_chip lpc31xx_gpiolibchip = {
	
		.label						= "gpio",
		.direction_input	= lpc31xx_gpiolib_dir_input,
		.get							= lpc31xx_gpiolib_get_value,
		.direction_output	= lpc31xx_gpiolib_dir_output,
		.set							= lpc31xx_gpiolib_set_value,
		.request					= lpc31xx_gpiolib_request,
		.base							= lpc31xx_gpiolib_GRP,
		.ngpio						= lpc31xx_gpiolib_MAX,
#if 0		/* not supported in this kernel version */
		.names					  = gpio_names,
#endif
		.can_sleep		    = 0
	
};

void __init lpc31xx_gpiolib_init(void)
{
	gpiochip_add(&lpc31xx_gpiolibchip);
}

