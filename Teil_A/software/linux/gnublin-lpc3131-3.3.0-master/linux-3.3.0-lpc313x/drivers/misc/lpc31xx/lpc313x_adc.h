/* 
 * lpc3131_adc.h
 * 
 * * LAST CHANGE: 26. dec. 2011 Nils Stec
 *
 *  Authors:    Nils Stec, "krumeltee", <info@fi-no.de>, (c) 2011     - the kernel module on top of the adc code
 *              Michael Schwarz, (c) 2011                             - the adc code itself
 *              some module parts are by LKMPG                        - taken from version "2007-05-18 ver 2.6.4"
 * 	
 * ADC driver for LPC313x (gnublin)
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

#ifndef __GNUBLIN_IO_H__
#define __GNUBLIN_IO_H__

#include <asm/uaccess.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <asm/io.h>

#define SUCCESS		0

#define DEVICE_NAME	"lpc313x_adc"	/* Dev name as it appears in /proc/devices   */
#define BUF_LEN 	8		/* length of the receive/send buffer, max 192 byte = 191 chars + trailing zero */
#define DRIVER_AUTHOR	"2011 Nils Stec \"krumeltee\" <info@fi-no.de>, 2011 Michael Schwarz"
#define DRIVER_DESC	"lpc313x adc driver for gnublin"

/* if you haven't applied the adc-registers patch, you will need these defines. */
/*
#define ADC_CON_REG            __REG (ADC_PHYS + 0x20)
#define ADC_R0_REG             __REG (ADC_PHYS + 0x00)
#define ADC_R1_REG             __REG (ADC_PHYS + 0x04)
#define ADC_R2_REG             __REG (ADC_PHYS + 0x08)
#define ADC_R3_REG             __REG (ADC_PHYS + 0x0C)
#define ADC_CSEL_REG           __REG (ADC_PHYS + 0x24)
#define ADC_INT_ENABLE_REG     __REG (ADC_PHYS + 0x28)
#define ADC_INT_STATUS_REG     __REG (ADC_PHYS + 0x2C)
#define ADC_INT_CLEAR_REG      __REG (ADC_PHYS + 0x30)
*/

#define ADC_INT_ENABLE  1
#define ADC_Rx_DATA     1023
#define ADC_ENABLE      (1 << 1)
#define ADC_CSCAN       (1 << 2)
#define ADC_SSCAN       (~ADC_CSCAN)
#define ADC_START       (1 << 3)
#define ADC_STOP        (~ADC_START)
#define ADC_STATUS      (1 << 4)
#define ADC_INT_STATUS  1
#define ADC_INT_CLEAR   1

#define ADC_CON_DEFAULT          0
#define ADC_CSEL_DEFAULT         0  
#define ADC_INT_ENABLE_DEFAULT   0
#define ADC_INT_STATUS_DEFAULT   0
#define ADC_INT_CLEAR_DEFAULT    0

#define ADC_CALC_CSEL(ch, res) ((res) << ((ch) * 4))
#define ADC_DATA_MASK(res) ((1 << (res)) - 1)

#define ADC_POWERSAVE_AUTO 666
#define ADC_POWERSAVE_ON  1
#define ADC_POWERSAVE_OFF 0


#ifndef __ADC_DRV_C__
int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
#endif // __ADC_DRV_C__

static u16 lpc313x_adc_read(int channel, int resolution);
static int lpc313x_init_adc(void);
static int lpc313x_deinit_adc(void);
//static int lpc313x_adc_powersave_on(void);
//static int lpc313x_adc_powersave_off(void);

#endif // __GNUBLIN_IO_H__
