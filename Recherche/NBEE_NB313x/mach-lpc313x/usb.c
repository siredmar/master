/* linux/arch/arm/mach-lpc313x/usb.c -- platform level USB initialization
 *
 *  Author:	Durgesh Pattamatta
 *  Copyright (C) 2009 NXP semiconductors
 *
 * USB initialization code.
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

#undef	DEBUG

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/usb/otg.h>
#include <linux/fsl_devices.h>
#include <linux/io.h>
#include <linux/interrupt.h>

#include <asm/irq.h>
#include <asm/system.h>
#include <mach/board.h>

/****************************************************************************
* USBOTG register definition
****************************************************************************/
#define USB_DEV_OTGSC			__REG(USBOTG_PHYS + 0x1A4)

/* bit defines for OTGSC register */
#define OTGSC_VD          _BIT(0)
#define OTGSC_VC          _BIT(1)
#define OTGSC_HAAR        _BIT(2)
#define OTGSC_OT          _BIT(3)
#define OTGSC_DP          _BIT(4)
#define OTGSC_IDPU        _BIT(5)
#define OTGSC_HADP        _BIT(6)
#define OTGSC_HABA        _BIT(7)

#define OTGSC_ID_INT      0
#define OTGSC_AVV_INT     1
#define OTGSC_ASV_INT     2
#define OTGSC_BSV_INT     3
#define OTGSC_BSE_INT     4
#define OTGSC_1mST_INT    5
#define OTGSC_DPS_INT     6
#define OTGSC_STATUS(n)   _BIT(8 + (n))
#define OTGSC_INT_STAT(n) _BIT(16 + (n))
#define OTGSC_INT_EN(n)   _BIT(24 + (n))
#define OTGSC_INT_STAT_MASK (0x007F0000)


/*-------------------------------------------------------------------------*/
static void	lpc313x_usb_release(struct device *dev);

static struct resource lpc313x_udc_resource[] = {
		[0] = {
			.start = USBOTG_PHYS,
			.end   = USBOTG_PHYS + SZ_4K,
			.flags = IORESOURCE_MEM,
		},
		[1] = {
			.start = IRQ_USB,
			.end   = IRQ_USB,
			.flags = IORESOURCE_IRQ,
		}
};

struct fsl_usb2_platform_data lpc313x_fsl_config = {

	.operating_mode = FSL_USB2_DR_DEVICE,
	.phy_mode = FSL_USB2_PHY_UTMI,
};

static u64 udc_dmamask = 0xffffffffUL;;

static struct platform_device lpc313x_udc_device = {
	.name = "fsl-usb2-udc",
	.dev = {
		.dma_mask          = &udc_dmamask,
		.coherent_dma_mask = 0xffffffff,
		.release           = lpc313x_usb_release,
		.platform_data     = &lpc313x_fsl_config,
	},
	.num_resources = ARRAY_SIZE(lpc313x_udc_resource),
	.resource      = lpc313x_udc_resource,
};


static struct resource ehci_lpc_resources[] = {
	[0] = {
		.start	= USBOTG_PHYS,
		.end	= USBOTG_PHYS + SZ_4K,
		.flags	= IORESOURCE_MEM,
	} ,
	[1] = {
		.start	= IRQ_USB,
		.end	= IRQ_USB,
		.flags	= IORESOURCE_IRQ,
	},
};
static u64 ehci_dmamask = 0xffffffffUL;

static struct platform_device ehci_lpc_device = {
	.name		= "lpc-ehci",
	.num_resources	= ARRAY_SIZE(ehci_lpc_resources),
	.dev		= {
		.dma_mask		= &ehci_dmamask,
		.coherent_dma_mask	= 0xffffffff,
		.release = lpc313x_usb_release,
	},
	.resource	= ehci_lpc_resources,
};


struct lpc_otg_dev_t
{
	int current_state;
	struct platform_device* curr_dev;

};
static struct lpc_otg_dev_t g_otg_dev;

static void change_state(struct work_struct *work);

DECLARE_WORK(swap_work, change_state);

/*-------------------------------------------------------------------------*/
static void	lpc313x_usb_release(struct device *dev)
{
	// do nothing
}

static void change_state(struct work_struct *work)
{
	struct lpc_otg_dev_t *pdev = &g_otg_dev;
	struct platform_device* reg_dev = NULL;
	int retval = 0;
	u32 stat = USB_DEV_OTGSC;

	printk(KERN_INFO "USB OTG changing device state from %d \n", pdev->current_state);

	if (stat & OTGSC_STATUS(OTGSC_ID_INT)) {
		/* check if we are already in B state. Then do nothing. */
		if (pdev->current_state != OTG_STATE_B_IDLE) {
			reg_dev = &lpc313x_udc_device;
			pdev->current_state = OTG_STATE_B_IDLE;
			lpc313x_vbus_power(0);
			printk(KERN_INFO "USB OTG changing device state to B device \n");
		}
	} 
	else {
		/* check if we are already in A state. Then do nothing. */
		if (pdev->current_state != OTG_STATE_A_IDLE) {
			reg_dev = &ehci_lpc_device;
			pdev->current_state = OTG_STATE_A_IDLE;
			lpc313x_vbus_power(1);
			printk(KERN_INFO "USB OTG changing device state to A device \n");
		}
	}
	disable_irq(IRQ_USB);

	if (pdev->curr_dev != NULL) {
		platform_device_unregister(pdev->curr_dev);
	}
	pdev->curr_dev = reg_dev;
	if (reg_dev != NULL) {
		retval = platform_device_register(reg_dev);
		if ( 0 != retval )
			printk(KERN_ERR "Can't register %s device (%d)\n",reg_dev->name, retval);

	}
	USB_DEV_OTGSC |= OTGSC_INT_EN(OTGSC_ID_INT);
	enable_irq(IRQ_USB);
}

irqreturn_t lpc313x_otg_irq(int irq, void *_dev)
{
	struct lpc_otg_dev_t *pdev = _dev;
	u32 stat;

	if (!(USB_DEV_OTGSC & OTGSC_IDPU)) {
		//printk( "ID pull-up disabled(0x%08x)\n", USB_DEV_OTGSC);
		USB_DEV_OTGSC |= OTGSC_IDPU;
	}

	stat = USB_DEV_OTGSC & OTGSC_INT_STAT_MASK;  /* Device Interrupt Status */
	
	if (stat)
		USB_DEV_OTGSC |= OTGSC_INT_STAT_MASK;


	if (stat & OTGSC_INT_STAT(OTGSC_ID_INT)) {
		USB_DEV_OTGSC = (USB_DEV_OTGSC & 0x0F);
		schedule_work(&swap_work);
		//change_state(NULL);
	}


	return IRQ_HANDLED;
}


/*-------------------------------------------------------------------------*/
int __init usbotg_init(void)
{
	u32 bank = EVT_GET_BANK(EVT_usb_atx_pll_lock);
	u32 bit_pos = EVT_usb_atx_pll_lock & 0x1F;
	int retval = 0;

	/* enable USB to AHB clock */
	cgu_clk_en_dis(CGU_SB_USB_OTG_AHB_CLK_ID, 1);
	/* enable clock to Event router */
	cgu_clk_en_dis(CGU_SB_EVENT_ROUTER_PCLK_ID, 1);

	/* reset USB block */
	cgu_soft_reset_module(USB_OTG_AHB_RST_N_SOFT);

	/* enable USB OTG PLL */
	SYS_USB_ATX_PLL_PD_REG = 0x0;
	/* wait for PLL to lock */
	while (!(EVRT_RSR(bank) & _BIT(bit_pos)));

	g_otg_dev.curr_dev = NULL;

	/* enable pull-up on ID pin so that we detect external pull-downs*/
	USB_DEV_OTGSC |= OTGSC_IDPU;
	/* delay */
	udelay(5);
	
	/* check ID state */
	if (!(USB_DEV_OTGSC & OTGSC_STATUS(OTGSC_ID_INT))) {
		/* register host */
		printk(KERN_INFO "Registering USB host 0x%08x 0x%08x (%d)\n", USB_DEV_OTGSC, EVRT_RSR(bank), bank);
		g_otg_dev.curr_dev = &ehci_lpc_device;
		g_otg_dev.current_state = OTG_STATE_A_IDLE;
		lpc313x_vbus_power(1);

	} else {
		/* register gadget */
		printk(KERN_INFO "Registering USB gadget 0x%08x 0x%08x (%d)\n", USB_DEV_OTGSC, EVRT_RSR(bank), bank);
		lpc313x_vbus_power(0);

		g_otg_dev.curr_dev = &lpc313x_udc_device;
		g_otg_dev.current_state = OTG_STATE_B_IDLE;
	}

	if (0 == retval) {
		retval = platform_device_register(g_otg_dev.curr_dev);
		if ( 0 != retval )
			printk(KERN_ERR "Can't register %s device\n", g_otg_dev.curr_dev->name);
	}
	printk(KERN_INFO "Returning from usbotg_init()\n");
#if 0 
	/* request IRQ to handle OTG events */
	retval = request_irq(IRQ_USB, lpc313x_otg_irq, IRQF_SHARED, "OTG", &g_otg_dev);
	/* enable OTG interrupts */
	USB_DEV_OTGSC |= OTGSC_INT_EN(OTGSC_ID_INT) | OTGSC_INT_STAT(OTGSC_ID_INT);
#endif

	return retval;
}

// __initcall(usbotg_init);


