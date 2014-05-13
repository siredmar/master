/* 
 * lpc313x_adc.c
 * 
 * * LAST CHANGE: 3. jan. 2012 Nils Stec
 * 
 *  Authors:    Nils Stec, "krumeltee", <info@fi-no.de>, (c) 2011,2012     - the kernel module on top of the adc code
 *              Michael Schwarz, (c) 2011                                  - the adc code itself
 *              some module parts are by LKMPG                             - taken from version "2007-05-18 ver 2.6.4"
 * 				Benedikt Niedermayr (niedermayr@embedded-projects.net)
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

#include <asm/uaccess.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <mach/registers.h>
#include <linux/cdev.h>       /* Support for /sys/class */
#include <linux/device.h>

#include "lpc313x_adc.h"

static int Device_Open = 0;	/* Is device open? */
static char msg[BUF_LEN];	/* The msg the device will give when asked */
static char *msg_Ptr;

static dev_t driv_number;
static struct cdev *driv_object;
static struct class *driv_class;
static struct device *driv_dev;

static struct file_operations fops = {
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release
};

static int adc_resolution;	
static int adc_channel;
static int adc_powersave;
static int adc_debug;
static int adc_averaging;
static int adc_averagingloops;

/*  Called when a process tries to open the device file */
static int device_open(struct inode *inode, struct file *file) {
	u16 adcval;
	int i;
	unsigned int addval = 0;
	if(Device_Open) return -EBUSY;
	Device_Open++;
	
	if((adc_powersave == 0)||(adc_powersave == ADC_POWERSAVE_AUTO)) {
		if(adc_powersave == ADC_POWERSAVE_AUTO) lpc313x_init_adc();
		
		if(adc_averaging == 1) {
			for(i = 0; i < adc_averagingloops; i++) {
				addval += lpc313x_adc_read(adc_channel, adc_resolution);
			}
			adcval = addval / adc_averagingloops;
			sprintf(msg, "0x%03x\n", adcval);
		} else {
			adcval = lpc313x_adc_read(adc_channel, adc_resolution);
			sprintf(msg, "0x%03x\n", adcval);
		}
		
		if(adc_powersave == ADC_POWERSAVE_AUTO) lpc313x_deinit_adc();
	} else {
		sprintf(msg, "adc is in powersave mode!\n");
	}

	msg_Ptr = msg;
	try_module_get(THIS_MODULE);
	return SUCCESS;
}

static ssize_t device_write(struct file *filp, const char *buff, size_t len, loff_t * off) {
	char message_from_user[BUF_LEN];
	long ltemp;
	u16 config_word;
	
	config_word = 0;

	if(copy_from_user(message_from_user, buff, (len < BUF_LEN) ? len : BUF_LEN)) return -EINVAL;
	message_from_user[4] = '\0';
	ltemp = simple_strtol(message_from_user, NULL, 16);
	
	config_word = (u16)(ltemp&0xffff);
	
	if(config_word & 0x0080) adc_debug = 1; else adc_debug = 0;
	if(adc_debug) printk(KERN_INFO "[lpc313x adc] got config word 0x%04x, setting up adc:\n", config_word);
	
	// first of all, check if user want's to see status, if, print it and exit
	if(((config_word >> 12) & 0x000f)>0) {
		printk(KERN_INFO "[lpc313x adc] channel:'%d', resolution:'%d'\n", adc_channel, adc_resolution);
		printk(KERN_INFO "[lpc313x adc] averaging:'%d'loops, on:'%d'\n", adc_averagingloops, adc_averaging);
		switch(adc_powersave) {
			case ADC_POWERSAVE_OFF:  printk(KERN_INFO "[lpc313x adc] powersave off, adc always on\n"); break;
			case ADC_POWERSAVE_ON:   printk(KERN_INFO "[lpc313x adc] powersave on, adc always off\n"); break;
			case ADC_POWERSAVE_AUTO: printk(KERN_INFO "[lpc313x adc] powersave auto (default)\n");     break;
		}
		return len;
	}	
	
	adc_channel = config_word & 0x0003;	// get adc channel config
	
	switch(((config_word >> 4) & 0x000f)) {	// get resolution config
		case 0:
			printk(KERN_INFO "set res to 10\n");
			adc_resolution = 10;
			break;
		case 1:
			printk(KERN_INFO "set res to 8\n");
			adc_resolution = 8;
			break;
		case 2:
			printk(KERN_INFO "set res to 4\n");
			adc_resolution = 4;
			break;
		default: 
			printk(KERN_INFO "set res to 10\n");
			adc_resolution = 10;
			break;
	}
	
	

	switch(((config_word >> 10) & 0x0003)) { // get averaging config
		case 0:
			adc_averaging = 0;
			adc_averagingloops = 0;
			break;
		case 1:
			adc_averaging = 1;
			adc_averagingloops = 10;
			break;
		case 2:
			adc_averaging = 1;
			adc_averagingloops = 75;
			break;
		case 3:
			adc_averaging = 1;
			adc_averagingloops = 150;
			break;
		default:
			adc_averaging = 0;
			break;
	}

	switch(((config_word >> 8) & 0x0003)) {	// get adc powersave mode config
		case 0:
			adc_powersave = ADC_POWERSAVE_OFF;	// adc powersave off, adc always on
			lpc313x_init_adc();			// turn on adc
			break;
		case 1:
			adc_powersave = ADC_POWERSAVE_AUTO;	// adc powersave auto, on/off when needed, takes more time, saves power (default)
			lpc313x_deinit_adc();			// turn off adc
			break;
		case 2:
			adc_powersave = ADC_POWERSAVE_ON;	// adc always off
			lpc313x_deinit_adc();			// turn off adc
			break;
		default:
			adc_powersave = ADC_POWERSAVE_AUTO;	// default behaviour, AUTO
			lpc313x_deinit_adc();			// turn off adc
			break;
	}	

	if(adc_debug) {
		printk(KERN_INFO "[lpc313x adc] channel:'%d', resolution:'%d'\n", adc_channel, adc_resolution);
		printk(KERN_INFO "[lpc313x adc] averaging:'%d'loops, on:'%d'\n", adc_averagingloops, adc_averaging);
			switch(adc_powersave) {
				case ADC_POWERSAVE_OFF:  printk(KERN_INFO "[lpc313x adc] powersave off, adc always on\n"); break;
				case ADC_POWERSAVE_ON:   printk(KERN_INFO "[lpc313x adc] powersave on, adc always off\n"); break;
				case ADC_POWERSAVE_AUTO: printk(KERN_INFO "[lpc313x adc] powersave auto (default)\n");     break;
		}
	}
	return len;
}


/** called when module loaded */
static int __init mod_init(void) {
	adc_resolution = 10;	
	adc_channel = 0;
	adc_powersave = ADC_POWERSAVE_AUTO;
	adc_debug = 0;
	adc_averaging = 0;
	adc_averagingloops = 0;
	
	/* Driver loading with Sysfs support --BN */
	if(alloc_chrdev_region(&driv_number,0,1,"lpc313x_adc_dev") < 0 ) return -EIO;
	driv_object = cdev_alloc(); 

	if( driv_object==NULL)
	goto free_device_number;

	driv_object->owner = THIS_MODULE;
	driv_object->ops   = &fops;

	if( cdev_add(driv_object,driv_number,1))
	goto free_cdev;

	driv_class = class_create(THIS_MODULE, "lpc313x_adc");
	if( IS_ERR(driv_class) ) {
	pr_err(	"[lpc313x_adc] no sysfs support\n");
	goto free_cdev;
	}


	driv_dev = device_create(driv_class, NULL, driv_number, NULL, "%s", "lpc313x_adc");


	switch(adc_powersave) {
		case ADC_POWERSAVE_AUTO:
		case ADC_POWERSAVE_ON:
			lpc313x_deinit_adc();
			break;
		case ADC_POWERSAVE_OFF:
			lpc313x_init_adc();
			break;
	}	
	
	dev_info(driv_dev, "[lpc313x_adc] driver loaded\n");
	
	return SUCCESS;


free_cdev:
	kobject_put(&driv_object->kobj);
free_device_number:
	unregister_chrdev_region(driv_number, 1);
	return -EIO;
}

/** called when module unloaded */
static void __exit mod_exit(void) {
	lpc313x_deinit_adc();
	
	device_release_driver(driv_dev);
	device_destroy(driv_class,driv_number);
	class_destroy(driv_class);
	cdev_del(driv_object);
	unregister_chrdev_region(driv_number,1);

	dev_info(driv_dev, "[lpc313x_adc] driver unloaded\n");
}

/* Called when a process closes the device file. */
static int device_release(struct inode *inode, struct file *file) {
	Device_Open--;		/* We're now ready for our next caller */
				/* Decrement the usage count, or else once you opened the file, you'll never get get rid of the module. */
	module_put(THIS_MODULE);
	return 0;
}

/* Called when a process, which already opened the dev file, attempts to read from it. */
static ssize_t device_read(struct file *filp, char *buffer, size_t length, loff_t * offset) {
	int bytes_read = 0;
	
	if(*msg_Ptr == 0) return 0;
	
	while(length && *msg_Ptr) {
		put_user(*(msg_Ptr++), buffer++);
		length--;
		bytes_read++;
	}
	
	return bytes_read;
}

u16 lpc313x_adc_read(int channel, int resolution) {
	u16 result = 0xffff;
	int timeout = 0xffffff;
 
	
	if((ADC_INT_STATUS_REG & 1)) {			// wait if there is a conversion
		while(ADC_INT_STATUS_REG & 1) 
			ADC_INT_CLEAR_REG |= (ADC_INT_CLEAR);
	}
	
	ADC_INT_ENABLE_REG |= ADC_INT_ENABLE;		// enable ADC interrupt
	
	
	if((ADC_INT_STATUS_REG & 1)) {			// wait until interrupt is cleared
		while(ADC_INT_STATUS_REG & 1)
			ADC_INT_CLEAR_REG |= (ADC_INT_CLEAR);
	} 
 
	ADC_CSEL_REG = ADC_CALC_CSEL(channel & 3, resolution);	// select channel and resolution
	ADC_CON_REG &= ADC_SSCAN;				// single conversion mode
	ADC_CON_REG |= ADC_START;				// start
 	while((ADC_INT_STATUS_REG == 0) && timeout--);		// wait until a conversion is done
	
	if(timeout == 0) return 0xffff;
 	
	ADC_INT_CLEAR_REG |= ADC_INT_CLEAR;			// wait until a conversion is done
	
	switch(channel) {					// read in the 10 bit value of the converted channel
		case 0:
			result = (u16)ADC_R0_REG & ADC_Rx_DATA;
			break;
		case 1:
			result = (u16)ADC_R1_REG & ADC_Rx_DATA;
			break;
		case 2:
			result = (u16)ADC_R2_REG & ADC_Rx_DATA;
			break;
		case 3:
			result = (u16)ADC_R3_REG & ADC_Rx_DATA;
			break;
	}
	ADC_CON_REG &= ADC_STOP;				// read in the 10 bit value of the converted channel
	while(ADC_INT_STATUS_REG & 1) ADC_INT_CLEAR_REG |= ADC_INT_CLEAR;	// wait until the interrupt status bit is cleared
	return result;
}

int lpc313x_init_adc(void) {
	int dummy;
	u32 timeout;
	
	SYS_ADC_PD = 0;
	
	/* enable clock for ADC */
	cgu_clk_en_dis(CGU_SB_ADC_CLK_ID, 1);
	cgu_clk_en_dis(CGU_SB_ADC_PCLK_ID, 1);
 
	/* reset to default */
	ADC_CON_REG = ADC_CON_DEFAULT;
	ADC_CSEL_REG = ADC_CSEL_DEFAULT;
	ADC_INT_ENABLE_REG = ADC_INT_ENABLE_DEFAULT;
	ADC_INT_CLEAR_REG = ADC_INT_CLEAR_DEFAULT;

	/* dummy interupt status register read */
	/** TODO: here we have to use a timeout, find out why and change, if neccessary. 
	    if you re-enable the adc the whole board freezes without this timeout. **/
	dummy = ADC_INT_STATUS_REG;
	timeout = 0x2ffff;	// if timeout is too high, we have to wait a bit (system freezes for this time) 
				// for the driver while reenabling the adc. use 0x2fff.
	while((ADC_INT_STATUS_REG & 1) && timeout--);
	
	/* enable ADC interrupt */
	ADC_INT_ENABLE_REG |= ADC_INT_ENABLE;
	
	/* set ADC power up */
	ADC_CON_REG |= ADC_ENABLE;	
	
	return 0;
}

int lpc313x_deinit_adc(void) {
	/* wait while there is a conversion */
	while (ADC_INT_STATUS_REG & 0x10);

	/* set ADC to default state */
	ADC_CON_REG = ADC_CON_DEFAULT;
	ADC_CSEL_REG = ADC_CSEL_DEFAULT;
	ADC_INT_ENABLE_REG = ADC_INT_ENABLE_DEFAULT;
	ADC_INT_CLEAR_REG = ADC_INT_CLEAR_DEFAULT;

	/* disable clock for ADC */
	cgu_clk_en_dis(CGU_SB_ADC_CLK_ID, 0);
	cgu_clk_en_dis(CGU_SB_ADC_PCLK_ID, 0);
	
	return 0;
}
/*
int lpc313x_adc_powersave_on(void) {
	ADC_CON_REG = ADC_CON_DEFAULT;
	return 0;
}

int lpc313x_adc_powersave_off(void) {
	ADC_CON_REG |= ADC_ENABLE;
	return 0;
}
*/

module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_SUPPORTED_DEVICE("lpc313x adc on gnublin");
