/* linux/arch/arm/mach-lpc313x/include/mach/gpio-lpc313x.h
 *
 * changes by Ingo Albrecht, see his patches at lpclinux.com
 * 0007-lpc313x-gpiolib-support.patch
 *  Author:	Durgesh Pattamatta
 *  Copyright (C) 2009 NXP semiconductors
 *  Modified linux 3.x by Hans-Frieder Vogt <hfvogt@gmx.net>
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
#ifndef _LPC313X_GPIO_LPC313X_H
#define _LPC313X_GPIO_LPC313X_H

#include <linux/irqflags.h>
#include <mach/hardware.h>

#define LPC313X_GPIO_GPIO_MAX 21
#define LPC313X_GPIO_EBI_MCI_MAX 32
#define LPC313X_GPIO_EBI_I2STX_0_MAX 10
#define LPC313X_GPIO_CGU_MAX 1
#define LPC313X_GPIO_I2SRX_0_MAX 3
#define LPC313X_GPIO_I2SRX_1_MAX 3
#define LPC313X_GPIO_I2STX_1_MAX 4
#define LPC313X_GPIO_EBI_MAX 16
#define LPC313X_GPIO_I2C1_MAX 2
#define LPC313X_GPIO_SPI_MAX 5
#define LPC313X_GPIO_NAND_CTRL_MAX 4
#define LPC313X_GPIO_PWM_MAX 1
#define LPC313X_GPIO_UART_MAX 2

#define LPC313X_GPIO_GPIO_GRP 0
#define LPC313X_GPIO_EBI_MCI_GRP 21
#define LPC313X_GPIO_EBI_I2STX_0_GRP 53
#define LPC313X_GPIO_CGU_GRP 63
#define LPC313X_GPIO_I2SRX_0_GRP 64
#define LPC313X_GPIO_I2SRX_1_GRP 67
#define LPC313X_GPIO_I2STX_1_GRP 70
#define LPC313X_GPIO_EBI_GRP 74
#define LPC313X_GPIO_I2C1_GRP 90
#define LPC313X_GPIO_SPI_GRP 92
#define LPC313X_GPIO_NAND_CTRL_GRP 97
#define LPC313X_GPIO_PWM_GRP 101
#define LPC313X_GPIO_UART_GRP 102

#define GPIO_GPIO0            (LPC313X_GPIO_GPIO_GRP + 0)
#define GPIO_GPIO1            (LPC313X_GPIO_GPIO_GRP + 1)
#define GPIO_GPIO2            (LPC313X_GPIO_GPIO_GRP + 2)
#define GPIO_GPIO3            (LPC313X_GPIO_GPIO_GRP + 3)
#define GPIO_GPIO4            (LPC313X_GPIO_GPIO_GRP + 4)
#define GPIO_GPIO11           (LPC313X_GPIO_GPIO_GRP + 11)
#define GPIO_GPIO12           (LPC313X_GPIO_GPIO_GRP + 12)
#define GPIO_GPIO13           (LPC313X_GPIO_GPIO_GRP + 13)
#define GPIO_GPIO14           (LPC313X_GPIO_GPIO_GRP + 14)
#define GPIO_GPIO15           (LPC313X_GPIO_GPIO_GRP + 15)
#define GPIO_GPIO16           (LPC313X_GPIO_GPIO_GRP + 16)
#define GPIO_GPIO17           (LPC313X_GPIO_GPIO_GRP + 17)
#define GPIO_GPIO18           (LPC313X_GPIO_GPIO_GRP + 18)
#define GPIO_GPIO19           (LPC313X_GPIO_GPIO_GRP + 19)
#define GPIO_GPIO20           (LPC313X_GPIO_GPIO_GRP + 20)
                
#define GPIO_MGPIO9           (LPC313X_GPIO_EBI_MCI_GRP + 0)
#define GPIO_MGPIO6           (LPC313X_GPIO_EBI_MCI_GRP + 1)
#define GPIO_MLCD_DB_7        (LPC313X_GPIO_EBI_MCI_GRP + 2)
#define GPIO_MLCD_DB_4        (LPC313X_GPIO_EBI_MCI_GRP + 3)
#define GPIO_MLCD_DB_2        (LPC313X_GPIO_EBI_MCI_GRP + 4)
#define GPIO_MNAND_RYBN0      (LPC313X_GPIO_EBI_MCI_GRP + 5)
#define GPIO_MI2STX_CLK0      (LPC313X_GPIO_EBI_MCI_GRP + 6)
#define GPIO_MI2STX_BCK0      (LPC313X_GPIO_EBI_MCI_GRP + 7)
#define GPIO_EBI_A_1_CLE      (LPC313X_GPIO_EBI_MCI_GRP + 8)
#define GPIO_EBI_NCAS_BLOUT   (LPC313X_GPIO_EBI_MCI_GRP + 9)
#define GPIO_MLCD_DB_0        (LPC313X_GPIO_EBI_MCI_GRP + 10)
#define GPIO_EBI_DQM_0_NOE    (LPC313X_GPIO_EBI_MCI_GRP + 11)
#define GPIO_MLCD_CSB         (LPC313X_GPIO_EBI_MCI_GRP + 12)
#define GPIO_MLCD_DB_1        (LPC313X_GPIO_EBI_MCI_GRP + 13)
#define GPIO_MLCD_E_RD        (LPC313X_GPIO_EBI_MCI_GRP + 14)
#define GPIO_MLCD_RS          (LPC313X_GPIO_EBI_MCI_GRP + 15)
#define GPIO_MLCD_RW_WR       (LPC313X_GPIO_EBI_MCI_GRP + 16)
#define GPIO_MLCD_DB_3        (LPC313X_GPIO_EBI_MCI_GRP + 17)
#define GPIO_MLCD_DB_5        (LPC313X_GPIO_EBI_MCI_GRP + 18)
#define GPIO_MLCD_DB_6        (LPC313X_GPIO_EBI_MCI_GRP + 19)
#define GPIO_MLCD_DB_8        (LPC313X_GPIO_EBI_MCI_GRP + 20)
#define GPIO_MLCD_DB_9        (LPC313X_GPIO_EBI_MCI_GRP + 21)
#define GPIO_MLCD_DB_10       (LPC313X_GPIO_EBI_MCI_GRP + 22)
#define GPIO_MLCD_DB_11       (LPC313X_GPIO_EBI_MCI_GRP + 23)
#define GPIO_MLCD_DB_12       (LPC313X_GPIO_EBI_MCI_GRP + 24)
#define GPIO_MLCD_DB_13       (LPC313X_GPIO_EBI_MCI_GRP + 25)
#define GPIO_MLCD_DB_14       (LPC313X_GPIO_EBI_MCI_GRP + 26)
#define GPIO_MLCD_DB_15       (LPC313X_GPIO_EBI_MCI_GRP + 27)
#define GPIO_MGPIO5           (LPC313X_GPIO_EBI_MCI_GRP + 28)
#define GPIO_MGPIO7           (LPC313X_GPIO_EBI_MCI_GRP + 29)
#define GPIO_MGPIO8           (LPC313X_GPIO_EBI_MCI_GRP + 30)
#define GPIO_MGPIO10          (LPC313X_GPIO_EBI_MCI_GRP + 31)
                
#define GPIO_MNAND_RYBN1      (LPC313X_GPIO_EBI_I2STX_0_GRP + 0)
#define GPIO_MNAND_RYBN2      (LPC313X_GPIO_EBI_I2STX_0_GRP + 1)
#define GPIO_MNAND_RYBN3      (LPC313X_GPIO_EBI_I2STX_0_GRP + 2)
#define GPIO_MUART_CTS_N      (LPC313X_GPIO_EBI_I2STX_0_GRP + 3)
#define GPIO_MUART_RTS_N      (LPC313X_GPIO_EBI_I2STX_0_GRP + 4)
#define GPIO_MI2STX_DATA0     (LPC313X_GPIO_EBI_I2STX_0_GRP + 5)
#define GPIO_MI2STX_WS0       (LPC313X_GPIO_EBI_I2STX_0_GRP + 6)
#define GPIO_EBI_NRAS_BLOUT   (LPC313X_GPIO_EBI_I2STX_0_GRP + 7)
#define GPIO_EBI_A_0_ALE      (LPC313X_GPIO_EBI_I2STX_0_GRP + 8)
#define GPIO_EBI_NWE          (LPC313X_GPIO_EBI_I2STX_0_GRP + 9)
                 
#define GPIO_CGU_SYSCLK_O     (LPC313X_GPIO_CGU_GRP + 0)

#define GPIO_I2SRX_BCK0       (LPC313X_GPIO_I2SRX_0_GRP + 0)
#define GPIO_I2SRX_DATA0      (LPC313X_GPIO_I2SRX_0_GRP + 1)
#define GPIO_I2SRX_WS0        (LPC313X_GPIO_I2SRX_0_GRP + 2)
                  
#define GPIO_I2SRX_DATA1      (LPC313X_GPIO_I2SRX_1_GRP + 0)
#define GPIO_I2SRX_BCK1       (LPC313X_GPIO_I2SRX_1_GRP + 1)
#define GPIO_I2SRX_WS1        (LPC313X_GPIO_I2SRX_1_GRP + 2)
                  
#define GPIO_I2STX_DATA1      (LPC313X_GPIO_I2STX_1_GRP + 0)
#define GPIO_I2STX_BCK1       (LPC313X_GPIO_I2STX_1_GRP + 1)
#define GPIO_I2STX_WS1        (LPC313X_GPIO_I2STX_1_GRP + 2)
#define GPIO_I2STX_256FS_O    (LPC313X_GPIO_I2STX_1_GRP + 3)
 
#define GPIO_EBI_D_9          (LPC313X_GPIO_EBI_GRP + 0)
#define GPIO_EBI_D_10         (LPC313X_GPIO_EBI_GRP + 1)
#define GPIO_EBI_D_11         (LPC313X_GPIO_EBI_GRP + 2)
#define GPIO_EBI_D_12         (LPC313X_GPIO_EBI_GRP + 3)
#define GPIO_EBI_D_13         (LPC313X_GPIO_EBI_GRP + 4)
#define GPIO_EBI_D_14         (LPC313X_GPIO_EBI_GRP + 5)
#define GPIO_EBI_D_4          (LPC313X_GPIO_EBI_GRP + 6)
#define GPIO_EBI_D_0          (LPC313X_GPIO_EBI_GRP + 7)
#define GPIO_EBI_D_1          (LPC313X_GPIO_EBI_GRP + 8)
#define GPIO_EBI_D_2          (LPC313X_GPIO_EBI_GRP + 9)
#define GPIO_EBI_D_3          (LPC313X_GPIO_EBI_GRP + 10)
#define GPIO_EBI_D_5          (LPC313X_GPIO_EBI_GRP + 11)
#define GPIO_EBI_D_6          (LPC313X_GPIO_EBI_GRP + 12)
#define GPIO_EBI_D_7          (LPC313X_GPIO_EBI_GRP + 13)
#define GPIO_EBI_D_8          (LPC313X_GPIO_EBI_GRP + 14)
#define GPIO_EBI_D_15         (LPC313X_GPIO_EBI_GRP + 15)
                  
#define GPIO_I2C_SDA1         (LPC313X_GPIO_I2C1_GRP + 0)
#define GPIO_I2C_SCL1         (LPC313X_GPIO_I2C1_GRP + 1)
                          
#define GPIO_SPI_MISO         (LPC313X_GPIO_SPI_GRP + 0)
#define GPIO_SPI_MOSI         (LPC313X_GPIO_SPI_GRP + 1)
#define GPIO_SPI_CS_IN        (LPC313X_GPIO_SPI_GRP + 2)
#define GPIO_SPI_SCK          (LPC313X_GPIO_SPI_GRP + 3)
#define GPIO_SPI_CS_OUT0      (LPC313X_GPIO_SPI_GRP + 4)
                 
#define GPIO_NAND_NCS_3       (LPC313X_GPIO_NAND_CTRL_GRP + 0)
#define GPIO_NAND_NCS_0       (LPC313X_GPIO_NAND_CTRL_GRP + 1)
#define GPIO_NAND_NCS_1       (LPC313X_GPIO_NAND_CTRL_GRP + 2)
#define GPIO_NAND_NCS_2       (LPC313X_GPIO_NAND_CTRL_GRP + 3)
                 
#define GPIO_PWM_DATA         (LPC313X_GPIO_PWM_GRP + 0)
                  
#define GPIO_UART_RXD         (LPC313X_GPIO_UART_GRP + 0)
#define GPIO_UART_TXD         (LPC313X_GPIO_UART_GRP + 1)
                
/* mapping table for exclusive GPIO group */

static const int gpio_pins_gpio_map[] = {
	1, 0, 2, 3,
	4, -1, -1, -1,
	-1, -1, -1, 5,
	6, 7, 8, 9,
	10, 11, 12, 13,
	14
};

#include <asm-generic/gpio.h>

#endif /*_LPC313X_GPIO_LPC313X_H*/
