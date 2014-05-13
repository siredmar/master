/* linux/arch/arm/mach-lpc313x/include/mach/gpio.h
 * 
 * changes by Ingo Albrecht, see his patches at lpclinux.com
 * 0007-lpc313x-gpiolib-support.patch
 *  
 *  Author:	Durgesh Pattamatta
 *  Copyright (C) 2009 NXP semiconductors
 *
 * GPIO defines & routines for LPC313x and LPC315x SoCs.
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
#define _LPC313X_GPIO_H    /* added 2011-12-11, --hh */


/* needed for IOCONF register definitions */
#include <mach/hardware.h>


/* gpio defines (by group) */

#define BASE_GPIO_GPIO 0
#define NUM_GPIO_GPIO 19
#define GPIO_GPIO1  (BASE_GPIO_GPIO + 0)
#define GPIO_GPIO0  (BASE_GPIO_GPIO + 1)
#define GPIO_GPIO2  (BASE_GPIO_GPIO + 2)
#define GPIO_GPIO3  (BASE_GPIO_GPIO + 3)
#define GPIO_GPIO4  (BASE_GPIO_GPIO + 4)
#define GPIO_GPIO11 (BASE_GPIO_GPIO + 5)
#define GPIO_GPIO12 (BASE_GPIO_GPIO + 6)
#define GPIO_GPIO13 (BASE_GPIO_GPIO + 7)
#define GPIO_GPIO14 (BASE_GPIO_GPIO + 8)
#define GPIO_GPIO15 (BASE_GPIO_GPIO + 9)
#define GPIO_GPIO16 (BASE_GPIO_GPIO + 10)
#define GPIO_GPIO17 (BASE_GPIO_GPIO + 11)
#define GPIO_GPIO18 (BASE_GPIO_GPIO + 12)
#define GPIO_GPIO19 (BASE_GPIO_GPIO + 13)
#define GPIO_GPIO20 (BASE_GPIO_GPIO + 14)

#define BASE_GPIO_EBI_MCI 15
#define NUM_GPIO_EBI_MCI 32
#define GPIO_MGPIO9           (BASE_GPIO_EBI_MCI + 0)
#define GPIO_MGPIO6           (BASE_GPIO_EBI_MCI + 1)
#define GPIO_MLCD_DB_7        (BASE_GPIO_EBI_MCI + 2)
#define GPIO_MLCD_DB_4        (BASE_GPIO_EBI_MCI + 3)
#define GPIO_MLCD_DB_2        (BASE_GPIO_EBI_MCI + 4)
#define GPIO_MNAND_RYBN0      (BASE_GPIO_EBI_MCI + 5)
#define GPIO_MI2STX_CLK0      (BASE_GPIO_EBI_MCI + 6)
#define GPIO_MI2STX_BCK0      (BASE_GPIO_EBI_MCI + 7)
#define GPIO_EBI_A_1_CLE      (BASE_GPIO_EBI_MCI + 8)
#define GPIO_EBI_NCAS_BLOUT   (BASE_GPIO_EBI_MCI + 9)
#define GPIO_MLCD_DB_0        (BASE_GPIO_EBI_MCI + 10)
#define GPIO_EBI_DQM_0_NOE    (BASE_GPIO_EBI_MCI + 11)
#define GPIO_MLCD_CSB         (BASE_GPIO_EBI_MCI + 12)
#define GPIO_MLCD_DB_1        (BASE_GPIO_EBI_MCI + 13)
#define GPIO_MLCD_E_RD        (BASE_GPIO_EBI_MCI + 14)
#define GPIO_MLCD_RS          (BASE_GPIO_EBI_MCI + 15)
#define GPIO_MLCD_RW_WR       (BASE_GPIO_EBI_MCI + 16)
#define GPIO_MLCD_DB_3        (BASE_GPIO_EBI_MCI + 17)
#define GPIO_MLCD_DB_5        (BASE_GPIO_EBI_MCI + 18)
#define GPIO_MLCD_DB_6        (BASE_GPIO_EBI_MCI + 19)
#define GPIO_MLCD_DB_8        (BASE_GPIO_EBI_MCI + 20)
#define GPIO_MLCD_DB_9        (BASE_GPIO_EBI_MCI + 21)
#define GPIO_MLCD_DB_10       (BASE_GPIO_EBI_MCI + 22)
#define GPIO_MLCD_DB_11       (BASE_GPIO_EBI_MCI + 23)
#define GPIO_MLCD_DB_12       (BASE_GPIO_EBI_MCI + 24)
#define GPIO_MLCD_DB_13       (BASE_GPIO_EBI_MCI + 25)
#define GPIO_MLCD_DB_14       (BASE_GPIO_EBI_MCI + 26)
#define GPIO_MLCD_DB_15       (BASE_GPIO_EBI_MCI + 27)
#define GPIO_MGPIO5           (BASE_GPIO_EBI_MCI + 28)
#define GPIO_MGPIO7           (BASE_GPIO_EBI_MCI + 29)
#define GPIO_MGPIO8           (BASE_GPIO_EBI_MCI + 30)
#define GPIO_MGPIO10          (BASE_GPIO_EBI_MCI + 31)

#define BASE_GPIO_EBI_I2STX_0 47
#define NUM_GPIO_EBI_I2STX_0 10
#define GPIO_MNAND_RYBN1      (BASE_GPIO_EBI_I2STX_0 + 0)
#define GPIO_MNAND_RYBN2      (BASE_GPIO_EBI_I2STX_0 + 1)
#define GPIO_MNAND_RYBN3      (BASE_GPIO_EBI_I2STX_0 + 2)
#define GPIO_MUART_CTS_N      (BASE_GPIO_EBI_I2STX_0 + 3)
#define GPIO_MUART_RTS_N      (BASE_GPIO_EBI_I2STX_0 + 4)
#define GPIO_MI2STX_DATA0     (BASE_GPIO_EBI_I2STX_0 + 5)
#define GPIO_MI2STX_WS0       (BASE_GPIO_EBI_I2STX_0 + 6)
#define GPIO_EBI_NRAS_BLOUT   (BASE_GPIO_EBI_I2STX_0 + 7)
#define GPIO_EBI_A_0_ALE      (BASE_GPIO_EBI_I2STX_0 + 8)
#define GPIO_EBI_NWE          (BASE_GPIO_EBI_I2STX_0 + 9)

#define BASE_GPIO_CGU 57
#define NUM_GPIO_CGU 1
#define GPIO_CGU_SYSCLK       (BASE_GPIO_CGU + 0)

#define BASE_GPIO_I2SRX_0 58
#define NUM_GPIO_I2SRX_0 3
#define GPIO_I2SRX_BCK0     (BASE_GPIO_I2SRX_0 + 0)
#define GPIO_I2SRX_DATA0    (BASE_GPIO_I2SRX_0 + 1)
#define GPIO_I2SRX_WS0      (BASE_GPIO_I2SRX_0 + 2)

#define BASE_GPIO_I2SRX_1 61
#define NUM_GPIO_I2SRX_1 3
#define GPIO_I2SRX_DATA1    (BASE_GPIO_I2SRX_1 + 0)
#define GPIO_I2SRX_BCK1     (BASE_GPIO_I2SRX_1 + 1)
#define GPIO_I2SRX_WS1      (BASE_GPIO_I2SRX_1 + 2)

#define BASE_GPIO_I2STX_1 64
#define NUM_GPIO_I2STX_1 4
#define GPIO_I2STX_DATA1      (BASE_GPIO_I2STX_1 + 0)
#define GPIO_I2STX_BCK1       (BASE_GPIO_I2STX_1 + 1)
#define GPIO_I2STX_WS1        (BASE_GPIO_I2STX_1 + 2)
#define GPIO_I2STX_256FS_0    (BASE_GPIO_I2STX_1 + 3)

#define BASE_GPIO_EBI 68
#define NUM_GPIO_EBI 16
#define GPIO_EBI_D_9          (BASE_GPIO_EBI + 0)
#define GPIO_EBI_D_10         (BASE_GPIO_EBI + 1) 
#define GPIO_EBI_D_11         (BASE_GPIO_EBI + 2)  
#define GPIO_EBI_D_12         (BASE_GPIO_EBI + 3)
#define GPIO_EBI_D_13         (BASE_GPIO_EBI + 4)
#define GPIO_EBI_D_14         (BASE_GPIO_EBI + 5)
#define GPIO_EBI_D_4          (BASE_GPIO_EBI + 6)
#define GPIO_EBI_D_0          (BASE_GPIO_EBI + 7)
#define GPIO_EBI_D_1          (BASE_GPIO_EBI + 8)
#define GPIO_EBI_D_2          (BASE_GPIO_EBI + 9)
#define GPIO_EBI_D_3          (BASE_GPIO_EBI + 10)
#define GPIO_EBI_D_5          (BASE_GPIO_EBI + 11)
#define GPIO_EBI_D_6          (BASE_GPIO_EBI + 12)
#define GPIO_EBI_D_7          (BASE_GPIO_EBI + 13)
#define GPIO_EBI_D_8          (BASE_GPIO_EBI + 14)
#define GPIO_EBI_D_15         (BASE_GPIO_EBI + 15)

#define BASE_GPIO_I2C1 84
#define NUM_GPIO_I2C1 2
#define GPIO_I2C_SDA1         (BASE_GPIO_I2C1 + 0)
#define GPIO_I2C_SCL1         (BASE_GPIO_I2C1 + 1)

#define BASE_GPIO_SPI 86
#define NUM_GPIO_SPI 5
#define GPIO_SPI_MISO         (BASE_GPIO_SPI + 0)
#define GPIO_SPI_MOSI         (BASE_GPIO_SPI + 1)
#define GPIO_SPI_CS_IN        (BASE_GPIO_SPI + 2)
#define GPIO_SPI_SCK          (BASE_GPIO_SPI + 3)
#define GPIO_SPI_CS_OUT0      (BASE_GPIO_SPI + 4)

#define BASE_GPIO_NAND_CTRL 91
#define NUM_GPIO_NAND_CTRL 4
#define GPIO_NAND_NCS_3       (BASE_GPIO_NAND_CTRL + 0)
#define GPIO_NAND_NCS_0       (BASE_GPIO_NAND_CTRL + 1)
#define GPIO_NAND_NCS_1       (BASE_GPIO_NAND_CTRL + 2)
#define GPIO_NAND_NCS_2       (BASE_GPIO_NAND_CTRL + 3)

#define BASE_GPIO_PWM 95
#define NUM_GPIO_PWM 1
#define GPIO_PWM_DATA         (BASE_GPIO_PWM + 0)

#define BASE_GPIO_UART 96
#define NUM_GPIO_UART 2
#define GPIO_UART_RXD         (BASE_GPIO_UART + 0)
#define GPIO_UART_TXD         (BASE_GPIO_UART + 1)


/* gpiolib init function */

extern void lpc313x_gpiolib_init(void);


/* gpiolib fastpath and aliases */

#define IOCONF_FAST_GPIO IOCONF_GPIO
#define NUM_FAST_GPIO NUM_GPIO_GPIO

#include <asm-generic/gpio.h>

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
	if(gpio < NUM_FAST_GPIO) {
		unsigned bit = (1 << gpio);
		if(GPIO_STATE(IOCONF_FAST_GPIO) & bit) {
			return 1;
		} else {
			return 0;
		}
	} else {
		return __gpio_get_value(gpio);
	}
}

static inline void gpio_set_value(unsigned gpio, int value)
{
	if(gpio < NUM_FAST_GPIO) {
		unsigned bit = (1 << gpio);
		if(value) {
			GPIO_M0_SET(IOCONF_FAST_GPIO) = bit;
		} else {
			GPIO_M0_RESET(IOCONF_FAST_GPIO) = bit;
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
