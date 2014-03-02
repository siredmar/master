/*  arch/arm/mach-lpc313x/generic.c
 *
 *  Author:	Durgesh Pattamatta
 *  Copyright (C) 2009 NXP semiconductors
 *
 *  Common code for machines with LPC313x and LPC315x SoCs.
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
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/serial_8250.h>

#include <asm/errno.h>
#include <mach/hardware.h>

#include <mach/gpio.h>
#include <asm/mach/map.h>

/* local functions */
extern void __init usbotg_init(void);

static struct plat_serial8250_port platform_serial_ports[] = {
	{
		.membase = (void *)io_p2v(UART_PHYS),
		.mapbase = (unsigned long)UART_PHYS,
		.irq = IRQ_UART,
		.uartclk = XTAL_CLOCK,
		.regshift = 2,
		.iotype = UPIO_MEM,
		.flags = UPF_BOOT_AUTOCONF | UPF_BUGGY_UART | UPF_SKIP_TEST,
	},
	{
		.flags		= 0
	},
};

static struct platform_device serial_device = {
	.name = "serial8250",
	.id = PLAT8250_DEV_PLATFORM,
	.dev = {
		.platform_data = &platform_serial_ports,
	},
};


static struct platform_device *devices[] __initdata = {
	&serial_device,
};

static struct map_desc lpc313x_io_desc[] __initdata = {
	{
		.virtual	= io_p2v(IO_INTC_PHYS),
		.pfn		= __phys_to_pfn(IO_INTC_PHYS),
		.length		= IO_INTC_SIZE,
		.type		= MT_DEVICE
	},
	{
		.virtual	= io_p2v(IO_APB01_PHYS),
		.pfn		= __phys_to_pfn(IO_APB01_PHYS),
		.length		= IO_APB01_SIZE,
		.type		= MT_DEVICE
	},
	{
		.virtual	= io_p2v(IO_APB2_PHYS),
		.pfn		= __phys_to_pfn(IO_APB2_PHYS),
		.length		= IO_APB2_SIZE,
		.type		= MT_DEVICE
	},
	{
		.virtual	= io_p2v(IO_APB3_PHYS),
		.pfn		= __phys_to_pfn(IO_APB3_PHYS),
		.length		= IO_APB3_SIZE,
		.type		= MT_DEVICE
	},
	{
		.virtual	= io_p2v(IO_APB4_PHYS),
		.pfn		= __phys_to_pfn(IO_APB4_PHYS),
		.length		= IO_APB4_SIZE,
		.type		= MT_DEVICE
	},
	{
		.virtual	= io_p2v(IO_DMA_REG_PHYS),
		.pfn		= __phys_to_pfn(IO_DMA_REG_PHYS),
		.length		= IO_DMA_REG_SIZE,
		.type		= MT_DEVICE
	},
	{
		.virtual	= io_p2v(IO_MPMC_CFG_PHYS),
		.pfn		= __phys_to_pfn(IO_MPMC_CFG_PHYS),
		.length		= IO_MPMC_CFG_SIZE,
		.type		= MT_DEVICE
	},
	{
		.virtual	= io_p2v(IO_NAND_BUF_PHYS),
		.pfn		= __phys_to_pfn(IO_NAND_BUF_PHYS),
		.length		= IO_NAND_BUF_SIZE,
		.type		= MT_DEVICE
	},
};

void __init lpc313x_map_io(void)
{
	iotable_init(lpc313x_io_desc, ARRAY_SIZE(lpc313x_io_desc));
}
extern int __init cgu_init(char *str);

int __init lpc313x_init(void)
{
	/* cgu init */
	cgu_init("");
	/* Switch on the UART clocks */
	cgu_clk_en_dis(CGU_SB_UART_APB_CLK_ID, 1);
	cgu_clk_en_dis(CGU_SB_UART_U_CLK_ID, 1);
	cgu_clk_en_dis(CGU_SB_IOCONF_PCLK_ID, 1);

	/* enable clocks to USB block */
	usbotg_init();

	/* Mux I2S signals based on selected channel */
#if defined (CONFIG_SND_I2S_TX0_MASTER)
	/* I2S TX0 WS, DATA */
	GPIO_DRV_IP(IOCONF_EBI_I2STX_0, 0x60);

	/* I2S TX0 BCK */
	GPIO_DRV_IP(IOCONF_EBI_MCI, 0x80);
#endif

#if defined (CONFIG_SND_I2S_TX1_MASTER)
	/* I2S TX1 BCK, WS, DATA */
	GPIO_DRV_IP(IOCONF_I2STX_1, 0x7);
#endif

#if defined (CONFIG_SND_I2S_RX0_MASTER) | defined (CONFIG_SND_I2S_RX0_SLAVE)
	/* I2S RX0 BCK, WS, DATA */
	GPIO_DRV_IP(IOCONF_I2SRX_0, 0x7);
#endif
#if defined (CONFIG_SND_I2S_RX1_MASTER) | defined (CONFIG_SND_I2S_RX1_SLAVE)
	/* I2S RX1 BCK, WS, DATA */
	GPIO_DRV_IP(IOCONF_I2SRX_1, 0x7);
#endif
	/* AUDIO CODEC CLOCK (256FS) */
	GPIO_DRV_IP(IOCONF_I2STX_1, 0x8);

	return platform_add_devices(devices, ARRAY_SIZE(devices));
}

#if 0
static void __init usbotg_init(void)
{
	u32 bank = EVT_GET_BANK(EVT_usb_atx_pll_lock);
	u32 bit_pos = EVT_usb_atx_pll_lock & 0x1F;

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
}
#endif

#if defined(CONFIG_SERIAL_8250_CONSOLE)
static int __init lpc313x_init_console(void)
{
	static __initdata char serr[] =
		KERN_ERR "Serial port #%u setup failed\n";
	struct uart_port up;
	int mul, div;

	/* Switch on the UART clocks */
	cgu_clk_en_dis(CGU_SB_UART_APB_CLK_ID, 1);
	cgu_clk_en_dis(CGU_SB_UART_U_CLK_ID, 1);

 	/*
	 * Set up serial port #0. Do not use autodetection; the result is
	 * not what we want.
 	 */
	memset(&up, 0, sizeof(up));

	up.membase	= (char *) io_p2v(UART_PHYS);
	up.mapbase = (unsigned long)UART_PHYS,
	up.irq		= IRQ_UART;
	up.uartclk	= XTAL_CLOCK;
	/* check what FDR bootloader is using */
	mul = (UART_FDR_REG >> 4) & 0xF;
	div = UART_FDR_REG & 0xF;
	if (div != 0)  {
		up.uartclk = (XTAL_CLOCK * mul) / (mul + div); 
	} 
	up.regshift	= 2;
	up.iotype	= UPIO_MEM;
	up.type		= PORT_NXP16750;
	up.flags	= UPF_BOOT_AUTOCONF | UPF_BUGGY_UART | UPF_SKIP_TEST;
	up.line		= 0;
	platform_serial_ports[0].uartclk = up.uartclk;
	if (early_serial_setup(&up))
		printk(serr, up.line);

	return 0;
}
console_initcall(lpc313x_init_console);

#endif /*CONFIG_SERIAL_8250_CONSOLE*/




