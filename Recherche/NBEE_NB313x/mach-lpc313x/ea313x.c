/*  arch/arm/mach-lpc313x/ea313x.c
 *
 *  Author:	Durgesh Pattamatta
 *  Copyright (C) 2009 NXP semiconductors
 *
 *  ea313x board init routines.
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

#include <linux/device.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/dm9000.h>
#include <linux/i2c.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/spi/spi.h>

#include <asm/system.h>
#include <mach/hardware.h>
#include <asm/irq.h>
#include <asm/pgtable.h>
#include <asm/page.h>
#include <asm/sizes.h>

#include <asm/mach/map.h>
#include <asm/mach-types.h>

#include <asm/mach/arch.h>
#include <mach/gpio.h>
#include <mach/i2c.h>
#include <mach/board.h>

#include <linux/spi/ads7846.h>
#include <linux/i2c/at24.h>

static struct lpc313x_mci_irq_data irq_data = {
	.irq = IRQ_SDMMC_CD,
};

static int mci_get_cd(u32 slot_id)
{
	return lpc31xx_gpio_get_value(GPIO_MI2STX_BCK0);
}

static irqreturn_t ea313x_mci_detect_interrupt(int irq, void *data)
{
	struct lpc313x_mci_irq_data	*pdata = data;

	/* select the opposite level senstivity */
	int level = mci_get_cd(0)?IRQ_TYPE_LEVEL_LOW:IRQ_TYPE_LEVEL_HIGH;

	set_irq_type(pdata->irq, level);

	/* change the polarity of irq trigger */
	return pdata->irq_hdlr(irq, pdata->data);
}

static int mci_init(u32 slot_id, irq_handler_t irqhdlr, void *data)
{
	int ret;
	int level;

	/* enable power to the slot */
	lpc31xx_gpio_set_value(GPIO_MI2STX_DATA0, 0);
	/* set cd pins as GPIO pins */
	lpc31xx_gpio_direction_input(GPIO_MI2STX_BCK0);

	/* select the opposite level senstivity */
	level = mci_get_cd(0)?IRQ_TYPE_LEVEL_LOW:IRQ_TYPE_LEVEL_HIGH;
	/* set card detect irq info */
	irq_data.data = data;
	irq_data.irq_hdlr = irqhdlr;
	set_irq_type(irq_data.irq, level);
	ret = request_irq(irq_data.irq,
			ea313x_mci_detect_interrupt,
			level,
			"mmc-cd", 
			&irq_data);
	return ret;

}

static int mci_get_ro(u32 slot_id)
{
	return 0;
}

static int mci_get_ocr(u32 slot_id)
{
	return MMC_VDD_32_33 | MMC_VDD_33_34;
}

static void mci_setpower(u32 slot_id, u32 volt)
{
	/* power is always on for both slots nothing to do*/
}
static int mci_get_bus_wd(u32 slot_id)
{
	return 4;
}

static void mci_exit(u32 slot_id)
{
	free_irq(irq_data.irq, &irq_data);
}

static struct resource lpc313x_mci_resources[] = {
	[0] = {
		.start  = IO_SDMMC_PHYS,
		.end	= IO_SDMMC_PHYS + IO_SDMMC_SIZE,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= IRQ_MCI,
		.end	= IRQ_MCI,
		.flags	= IORESOURCE_IRQ,
	},
};
static struct lpc313x_mci_board ea313x_mci_platform_data = {
	.num_slots		= 1,
	.detect_delay_ms	= 250,
	.init 			= mci_init,
	.get_ro			= mci_get_ro,
	.get_cd 		= mci_get_cd,
	.get_ocr		= mci_get_ocr,
	.get_bus_wd		= mci_get_bus_wd,
	.setpower 		= mci_setpower,
	.exit			= mci_exit,
};

static u64 mci_dmamask = 0xffffffffUL;
static struct platform_device	lpc313x_mci_device = {
	.name		= "lpc313x_mmc",
	.num_resources	= ARRAY_SIZE(lpc313x_mci_resources),
	.dev		= {
		.dma_mask		= &mci_dmamask,
		.coherent_dma_mask	= 0xffffffff,
		.platform_data		= &ea313x_mci_platform_data,
	},
	.resource	= lpc313x_mci_resources,
};


#if defined (CONFIG_FB_TLS8301S) 
static struct resource tls8301s_resource[] = {
	[0] = {
		.start = EXT_SRAM0_PHYS + 0x00000 + 0x0000,
		.end   = EXT_SRAM0_PHYS + 0x00000 + 0xffff,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = EXT_SRAM0_PHYS + 0x10000 + 0x0000,
		.end   = EXT_SRAM0_PHYS + 0x10000 + 0xffff,
		.flags = IORESOURCE_MEM,
	},
};

static struct platform_device tls8301s_device = {
	.name          = "tls8301s",
	.id            = 0,
	.num_resources = ARRAY_SIZE(tls8301s_resource),
	.resource      = tls8301s_resource,
};

static void __init ea_add_device_tls8301s(void)
{
	MPMC_STCONFIG0 = 0x81;
	MPMC_STWTWEN0  = 0;
	MPMC_STWTOEN0  = 0;
	MPMC_STWTRD0   = 31;
	MPMC_STWTPG0   = 0;
	MPMC_STWTWR0   = 3;
	MPMC_STWTTURN0 = 0;

	platform_device_register(&tls8301s_device);
}
#else
static void __init ea_add_device_tls8301s(void) {}
#endif 

#if defined (CONFIG_FB_SSD1963) 
static struct resource ssd1963_resource[] = {
	[0] = {
		.start = EXT_SRAM0_PHYS + 0x00000 + 0x0000,
		.end   = EXT_SRAM0_PHYS + 0x00000 + 0xffff,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = EXT_SRAM0_PHYS + 0x10000 + 0x0000,
		.end   = EXT_SRAM0_PHYS + 0x10000 + 0xffff,
		.flags = IORESOURCE_MEM,
	},
};

static struct platform_device ssd1963_device = {
	.name          = "ssd1963",
	.id            = 0,
	.num_resources = ARRAY_SIZE(ssd1963_resource),
	.resource      = ssd1963_resource,
};

static void __init ea_add_device_ssd1963(void)
{
	MPMC_STCONFIG0 = 0x81;
	MPMC_STWTWEN0  = 0;
	MPMC_STWTOEN0  = 0;
	MPMC_STWTRD0   = 31;
	MPMC_STWTPG0   = 0;
	MPMC_STWTWR0   = 3;
	MPMC_STWTTURN0 = 0;

	platform_device_register(&ssd1963_device);
}
#else
static void __init ea_add_device_ssd1963(void) {}
#endif /* CONFIG_SSD1289 */


/*
 * DM9000 ethernet device
 */
#if defined(CONFIG_DM9000)
static struct resource dm9000_resource[] = {
	[0] = {
		.start	= EXT_SRAM1_PHYS,
		.end	= EXT_SRAM1_PHYS + 0xFF,
		.flags	= IORESOURCE_MEM
	},
	[1] = {
		.start	= EXT_SRAM1_PHYS + 0x10000,
		.end	= EXT_SRAM1_PHYS + 0x100FF,
		.flags	= IORESOURCE_MEM
	},
	[2] = {
		.start	= IRQ_DM9000_ETH_INT,
		.end	= IRQ_DM9000_ETH_INT,
		.flags	= IORESOURCE_IRQ | IORESOURCE_IRQ_HIGHLEVEL
	}
};
/* ARM MPMC contoller as part of low power design doesn't de-assert nCS and nOE for consecutive 
reads but just changes address. But DM9000 requires nCS and nOE change between address. So access
other chip select area (nCS0) to force de-assertion of nCS1 and nOE1. Or else wait for long time 
such as 80 usecs. 
LPC313x has external logic outside of MPMC IP to toggle nOE to split consecutive reads.
The latest Apex bootloader pacth makes use of this feture.
For this to work SYS_MPMC_WTD_DEL0 & SYS_MPMC_WTD_DEL1 should be programmed with MPMC_STWTRD0
& MPMC_STWTRD1 values. The logic only deactivates the nOE for one clock cycle which is
11nsec but DM9000 needs 80nsec between nOEs. So lets add some dummy instructions such as
reading a GPIO register to compensate for extra 70nsec.
*/
# define DM_IO_DELAY()	do { lpc31xx_gpio_get_value(GPIO_MNAND_RYBN3);} while(0)

static void dm9000_dumpblk(void __iomem *reg, int count)
{
	int i;
	int tmp;

	count = (count + 1) >> 1;
	for (i = 0; i < count; i++) {
		DM_IO_DELAY();
		tmp = readw(reg);
	}
}

static void dm9000_inblk(void __iomem *reg, void *data, int count)
{
	int i;
	u16* pdata = (u16*)data;
	count = (count + 1) >> 1;
	for (i = 0; i < count; i++) {
		DM_IO_DELAY();
		*pdata++ = readw(reg);
	}
}

static struct dm9000_plat_data dm9000_platdata = {
	.flags		= DM9000_PLATF_16BITONLY,
	.dumpblk = dm9000_dumpblk,
	.inblk = dm9000_inblk,
};

static struct platform_device dm9000_device = {
	.name		= "dm9000",
	.id		= 0,
	.num_resources	= ARRAY_SIZE(dm9000_resource),
	.resource	= dm9000_resource,
	.dev		= {
		.platform_data	= &dm9000_platdata,
	}
};
static void __init ea_add_device_dm9000(void)
{
	/*
	 * Configure Chip-Select 2 on SMC for the DM9000.
	 * Note: These timings were calculated for MASTER_CLOCK = 90000000
	 *  according to the DM9000 timings.
	 */
	MPMC_STCONFIG1 = 0x81;
	MPMC_STWTWEN1 = 1;
	MPMC_STWTOEN1 = 1;
	MPMC_STWTRD1 = 4;
	MPMC_STWTPG1 = 1;
	MPMC_STWTWR1 = 1;
	MPMC_STWTTURN1 = 2;
	/* enable oe toggle between consec reads */
	SYS_MPMC_WTD_DEL1 = _BIT(5) | 4;

	/* Configure Interrupt pin as input, no pull-up */
	lpc31xx_gpio_direction_input(GPIO_MNAND_RYBN3);

	platform_device_register(&dm9000_device);
}
#else
static void __init ea_add_device_dm9000(void) {}
#endif /* CONFIG_DM9000 */


#if defined (CONFIG_MTD_NAND_LPC313X)
static struct resource lpc313x_nand_resources[] = {
	[0] = {
		.start  = IO_NAND_PHYS,
		.end	= IO_NAND_PHYS + IO_NAND_SIZE,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start 	= IO_NAND_BUF_PHYS,
		.end 	= IO_NAND_BUF_PHYS + IO_NAND_BUF_SIZE,
		.flags	= IORESOURCE_MEM,
	},
	[2] = {
		.start 	= IRQ_NAND_FLASH,
		.end 	= IRQ_NAND_FLASH,
		.flags	= IORESOURCE_IRQ,
	}
};

#define BLK_SIZE (2048 * 64)
static struct mtd_partition ea313x_nand0_partitions[] = {
	/* The EA3131 board uses the following block scheme:
	128K: Blocks 0   - 0    - LPC31xx info and bad block table
	384K: Blocks 1   - 3    - Apex bootloader
	256K: Blocks 4   - 5    - Apex environment
	4M:   Blocks 6   - 37   - Kernel image
	16M:  Blocks 38  - 165  - Ramdisk image (if used)
	???:  Blocks 166 - end  - Root filesystem/storage */
	{
		.name	= "lpc313x-rootfs",
		.offset	= (BLK_SIZE * 166),
		.size	= MTDPART_SIZ_FULL
	},
};

static struct lpc313x_nand_timing ea313x_nanddev_timing = {
	.ns_trsd	= 36,
	.ns_tals	= 36,
	.ns_talh	= 12,
	.ns_tcls	= 36,
	.ns_tclh	= 12,
	.ns_tdrd	= 36,
	.ns_tebidel	= 12,
	.ns_tch		= 12,
	.ns_tcs		= 48,
	.ns_treh	= 24,
	.ns_trp		= 48,
	.ns_trw		= 24,
	.ns_twp		= 36
};

static struct lpc313x_nand_dev_info ea313x_ndev[] = {
	{
		.name		= "nand0",
		.nr_partitions	= ARRAY_SIZE(ea313x_nand0_partitions),
		.partitions	= ea313x_nand0_partitions
	}
};

static struct lpc313x_nand_cfg ea313x_plat_nand = {
	.nr_devices	= ARRAY_SIZE(ea313x_ndev),
	.devices	= ea313x_ndev,
	.timing		= &ea313x_nanddev_timing,
	.support_16bit	= 0,
};

static struct platform_device	lpc313x_nand_device = {
	.name		= "lpc313x_nand",
	.dev		= {
				.platform_data	= &ea313x_plat_nand,
	},
	.num_resources	= ARRAY_SIZE(lpc313x_nand_resources),
	.resource	= lpc313x_nand_resources,
};
#endif

#if defined(CONFIG_SPI_LPC313X)
static struct resource lpc313x_spi_resources[] = {
	[0] = {
		.start	= SPI_PHYS,
		.end	= SPI_PHYS + SZ_4K - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= IRQ_SPI,
		.end	= IRQ_SPI,
		.flags	= IORESOURCE_IRQ,
	},
};

static void spi_set_cs_state(int cs_num, int state)
{

	switch (cs_num) { 
		case 0: lpc313x_gpio_set_value(GPIO_SPI_CS_OUT0, state); // SPI.CS0 is not muxed and always connected to SPI NOR
			break;
		case 1: lpc313x_gpio_set_value(GPIO_MUART_CTS_N, state); // UART.RTS is muxed with CS1 = ADS7846 touchscreen
			break;
		case 2:	lpc313x_gpio_set_value(GPIO_MUART_RTS_N, state); // UART.RTS is muxed with CS2 = External SPI (exported to ports)
			break;	
		case 3:	lpc313x_gpio_set_value(GPIO_MUART_RTS_N, state); // OLED CMD = CS2 low / CS1 low		
			lpc313x_gpio_set_value(GPIO_MUART_CTS_N, state); 
			break;
	}
}

struct lpc313x_spics_cfg lpc313x_stdspics_cfg[] =
{
	[0] = { /* SPI CS0 */
	.spi_spo = 0, /* Low clock between transfers */
	.spi_sph = 0, /* Data capture on first clock edge (high edge with spi_spo=0) */
	.spi_cs_set = spi_set_cs_state,
	},
	[1] = { /* SPI CS1 = ADS7846 Touchscreen */
	.spi_spo = 0, /* Low clock between transfers */
	.spi_sph = 0, /* Data capture on first clock edge (high edge with spi_spo=0) */
	.spi_cs_set = spi_set_cs_state,
	},
	[2] = { /* SPI CS2 = External SPI CS available on user ports */
	.spi_spo = 0, /* Low clock between transfers */
	.spi_sph = 0, /* Data capture on first clock edge (high edge with spi_spo=0) */
	.spi_cs_set = spi_set_cs_state,
	},
	[3] = { /* SPI CS1 + CS2 = Oled CMD access */
	.spi_spo = 0, /* Low clock between transfers */
	.spi_sph = 0, /* Data capture on first clock edge (high edge with spi_spo=0) */
	.spi_cs_set = spi_set_cs_state,
	},


};

struct lpc313x_spi_cfg lpc313x_spidata =
{
	.num_cs			= ARRAY_SIZE(lpc313x_stdspics_cfg),
	.spics_cfg		= lpc313x_stdspics_cfg,
};

static u64 lpc313x_spi_dma_mask = 0xffffffffUL;
static struct platform_device lpc313x_spi_device = {
	.name		= "spi_lpc313x",
	.id		= 0,
	.dev		= {
		.dma_mask = &lpc313x_spi_dma_mask,
		.coherent_dma_mask = 0xffffffffUL,
		.platform_data	= &lpc313x_spidata,
	},
	.num_resources	= ARRAY_SIZE(lpc313x_spi_resources),
	.resource	= lpc313x_spi_resources,
};

/* If both SPIDEV and MTD data flash are enabled with the same chip select, only 1 will work */
#if defined(CONFIG_SPI_SPIDEV)
/* SPIDEV driver registration */
static int __init lpc313x_spidev_register(void)
{
	struct spi_board_info info[] =
	{
	  [0]={
		.modalias = "spidev",
		.max_speed_hz = 1000000,
		.bus_num = 0,
		.chip_select = 0
	      },
	  [1]={
		.modalias = "spidev",
		.max_speed_hz = 1000000,
		.bus_num = 0,
		.chip_select = 1 
	      },
	  [2]={
		.modalias = "spidev",
		.max_speed_hz = 1000000,
		.bus_num = 0,
		.chip_select = 2 
	      },
	  [3]={
		.modalias = "spidev",
		.max_speed_hz = 1000000,
		.bus_num = 0,
		.chip_select = 3 
	      },
	
	};

	return spi_register_board_info(info, ARRAY_SIZE(info));
}
arch_initcall(lpc313x_spidev_register);
#endif

#if defined(CONFIG_MTD_DATAFLASH)
/* MTD Data FLASH driver registration */
static int __init lpc313x_spimtd_register(void)
{
	struct spi_board_info info =
	{
		.modalias = "mtd_dataflash",
		.max_speed_hz = 30000000,
		.bus_num = 0,
		.chip_select = 0,
	};

	return spi_register_board_info(&info, 1);
}
arch_initcall(lpc313x_spimtd_register);
#endif
#endif

static struct platform_device *devices[] __initdata = {
	&lpc313x_mci_device,
#if defined (CONFIG_MTD_NAND_LPC313X)
	&lpc313x_nand_device,
#endif
#if defined(CONFIG_SPI_LPC313X)
	&lpc313x_spi_device,
#endif
};

static struct map_desc ea313x_io_desc[] __initdata = {
	{
		.virtual	= io_p2v(EXT_SRAM0_PHYS),
		.pfn		= __phys_to_pfn(EXT_SRAM0_PHYS),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	},
	{
		.virtual	= io_p2v(EXT_SRAM1_PHYS + 0x10000),
		.pfn		= __phys_to_pfn(EXT_SRAM1_PHYS + 0x10000),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	},
	{
		.virtual	= io_p2v(IO_SDMMC_PHYS),
		.pfn		= __phys_to_pfn(IO_SDMMC_PHYS),
		.length		= IO_SDMMC_SIZE,
		.type		= MT_DEVICE
	},
	{
		.virtual	= io_p2v(IO_USB_PHYS),
		.pfn		= __phys_to_pfn(IO_USB_PHYS),
		.length		= IO_USB_SIZE,
		.type		= MT_DEVICE
	},
};

static struct at24_platform_data at24c02 = {
        .byte_len        = SZ_1K * 2 / 8,
        .page_size        = 8,
};

static struct i2c_board_info ea313x_i2c_devices[] __initdata = {
	{
			I2C_BOARD_INFO("s35390a", 0x30),
			
	},
	{
			I2C_BOARD_INFO("pca9532", 0x60),

        },
	{
		I2C_BOARD_INFO("24c02", 0x50),
               	.platform_data        = &at24c02
	}

};


void __init lpc31xx_gpiolib_init(void);


static void __init ea313x_init(void)
{
	lpc313x_init();
	
	/* register GPIOLIB gpios */
	lpc31xx_gpiolib_init();
	
	/* register i2cdevices */
	lpc313x_register_i2c_devices();
	

	platform_add_devices(devices, ARRAY_SIZE(devices));

	/* add DM9000 device */
	ea_add_device_dm9000();
	
	/* add Framebuffer device SSD1963 (where available...)*/
	ea_add_device_ssd1963();
	
	/* add Framebuffer device TLS8301S (where available...)*/
	ea_add_device_tls8301s();
	
	
	i2c_register_board_info(0, ea313x_i2c_devices,
		ARRAY_SIZE(ea313x_i2c_devices));

}

static void __init ea313x_map_io(void)
{
	lpc313x_map_io();
	iotable_init(ea313x_io_desc, ARRAY_SIZE(ea313x_io_desc));
}


static int ads7846_pendown_state(void)
{
	// LPC313x Touchscreen PENIRQ is connected to GPIO4 
	// (Touch_EN is SPI.CS1 (UART_CTS_SPI_CS_OUT1))
	return !lpc313x_gpio_get_value(GPIO_GPIO4); 
}

static struct ads7846_platform_data lpc_ads7846_platform_data = 
{
	.model = 7846, // The touch screen controller used is TSC2046 from 
		       // Texas Instruments = (TSC2046 is just newer ads7846 silicon.
			// .debounce_max = 1,
			// .debounce_rep = 0,
			// .debounce_tol = (~0),
#if 1
	.pressure_max = 1024,
	.debounce_max=10,
	.debounce_tol=3,
	.x_min = 150, //***MOD:AJO: Measurments taken on small replacement (through ts_calibrate) 339
	.x_max = 3900, // 3858
	.y_min = 190, // 258
	.y_max = 3830, // 3587
	
#endif

#if 0

	.x_min = 300, //***MOD:JP: Measurments taken on small replacement (through ts_calibrate) 339
	.x_max = 3900, // 3858
	.y_min = 250, // 258
	.y_max = 3600, // 3587
#endif
#if 0
	.x_min = 150,
	.x_max = 3830,
	.y_min = 190,
	.y_max = 3830,
#endif
	.x_plate_ohms = 450,
	.y_plate_ohms = 250,
	.vref_delay_usecs = 100,
// .debounce_max = 10, // From LPC32xx
// .debounce_tol = 3,
// .pressure_max = 1024,
	.get_pendown_state = ads7846_pendown_state,
};

static int __init lpc313x_spitouch_register(void)
{
#ifdef NBEE_TOUCH_SENSOR


	struct spi_board_info info =
	{
	.modalias = "ads7846",
	.max_speed_hz = 125000 * 26, /* (max sample rate @ 3V) * (cmd + data + overhead) */ //***TEMP! //***TODO: Speed up!
	.bus_num = 0,
	.chip_select = 1, // AD7846 is on SPI.CS1
	.platform_data = &lpc_ads7846_platform_data, //***MOD
	.irq = IRQ_Touch_PenDown, // Touchscreen has its PENIRQ line connected to GPIO4
	};
	
	lpc31xx_gpio_direction_input(GPIO_GPIO4); // ADS7846 penirq is connected to LPC313x.GPIO4 (Set touchscreen penirq as a GPIO input)

	return spi_register_board_info(&info, 1);
#else
	return 0;
#endif
}

arch_initcall(lpc313x_spitouch_register);




MACHINE_START(EA313X, "NXP EA313X")
	/* Maintainer: Durgesh Pattamatta, NXP */
	.phys_io	= IO_APB01_PHYS,
	.io_pg_offst	= (io_p2v(IO_APB01_PHYS) >> 18) & 0xfffc,
	.boot_params	= 0x30000100,
	.map_io		= ea313x_map_io,
	.init_irq	= lpc313x_init_irq,
	.timer		= &lpc313x_timer,
	.init_machine	= ea313x_init,
MACHINE_END
