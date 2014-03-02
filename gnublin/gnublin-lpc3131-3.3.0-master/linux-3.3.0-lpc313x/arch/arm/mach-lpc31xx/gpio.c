/*
 * Copyright (c) 2012 Hans-Frieder Vogt <hfvogt@gmx.net>
 *
 * GPIO device for LPC313x & LPC315x.
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
#include <linux/platform_device.h>
#include <linux/kernel.h>
#include <linux/string.h>

#include <mach/hardware.h>

static struct resource gpio_resource[] = {
	[0] = {
		.start	= GPIO_PHYS,
		.end	= GPIO_PHYS + SZ_4K - 1,
		.flags	= IORESOURCE_MEM,
	},
};

struct platform_device lpc313x_gpio_device = {
	.name		= "lpc313x-gpio",
	.id		= -1,
	.resource	= gpio_resource,
	.num_resources	= ARRAY_SIZE(gpio_resource),
};

int __init lpc313x_gpio_dev_init(void) {
	return platform_device_register(&lpc313x_gpio_device);
}
