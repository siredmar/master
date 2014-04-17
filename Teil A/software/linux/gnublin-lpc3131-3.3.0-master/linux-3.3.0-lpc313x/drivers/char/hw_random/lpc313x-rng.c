/*
 * lpc313x-rng.c - RNG driver for NXP LPC313x
 *
 * Author: Ingo Albrecht <prom@berlin.ccc.de>
 *
 * Copyright 2010 (c) Ingo Albrecht
 *
 * This file is licensed under  the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/hw_random.h>

#include <mach/hardware.h>

static void __iomem *rng_base;
static struct platform_device *rng_dev;

#define RNG_REG_RANDOM 0x000
#define RNG_REG_POWER  0xFF4

static inline u32 lpc313x_rng_read_reg(int reg)
{
	return __raw_readl(rng_base + reg);
}

static inline void lpc313x_rng_write_reg(int reg, u32 val)
{
	__raw_writel(val, rng_base + reg);
}

static int lpc313x_rng_data_read(struct hwrng *rng, u32 * data)
{
	*data = lpc313x_rng_read_reg(RNG_REG_RANDOM);
	return 4;
}

static struct hwrng lpc313x_rng = {
	.name		= "lpc313x",
	.data_read	  = lpc313x_rng_data_read,
};

static void lpc313x_rng_endis_ring_clocks(int enable)
{
	if(enable) {
		SYS_RNG_OSC_CFG = 3;
	} else {
		SYS_RNG_OSC_CFG = 0;
	}
}

static void lpc313x_rng_endis_bus_clock(int enable)
{
	cgu_clk_en_dis(CGU_SB_RNG_PCLK_ID, enable);
}

static int __init lpc313x_rng_probe(struct platform_device *pdev)
{
	struct resource *res, *mem;
	int ret;

	if (rng_dev)
		return -EBUSY;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);

	if(!res)
		return -ENOENT;

	mem = request_mem_region(res->start, resource_size(res),
				 pdev->name);
	if (mem == NULL)
		return -EBUSY;

	dev_set_drvdata(&pdev->dev, mem);
	rng_base = ioremap(res->start, resource_size(res));
	if (!rng_base) {
		ret = -ENOMEM;
		goto err_ioremap;
	}

	/* initialize the rng itself */
	lpc313x_rng_endis_bus_clock(1);
	lpc313x_rng_endis_ring_clocks(1);
	lpc313x_rng_write_reg(RNG_REG_POWER, 0);

	ret = hwrng_register(&lpc313x_rng);
	if (ret)
		goto err_register;

	dev_info(&pdev->dev, "rng initialized\n");

	rng_dev = pdev;

	return 0;

err_register:
	lpc313x_rng_endis_bus_clock(0);
	lpc313x_rng_endis_ring_clocks(0);
	iounmap(rng_base);
	rng_base = NULL;
err_ioremap:
	release_resource(mem);
	return ret;
}

static int __exit lpc313x_rng_remove(struct platform_device *pdev)
{
	struct resource *mem = dev_get_drvdata(&pdev->dev);

	hwrng_unregister(&lpc313x_rng);

	iounmap(rng_base);

	lpc313x_rng_endis_bus_clock(0);
	lpc313x_rng_endis_ring_clocks(0);

	release_resource(mem);
	rng_base = NULL;

	return 0;
}

#ifdef CONFIG_PM

static int lpc313x_rng_suspend(struct platform_device *pdev, pm_message_t message)
{
	return 0;
}

static int lpc313x_rng_resume(struct platform_device *pdev)
{
	return 0;
}

#else

#define lpc313x_rng_suspend NULL
#define lpc313x_rng_resume NULL

#endif

static struct platform_driver lpc313x_rng_driver = {
	.driver = {
		   .name = "lpc313x-rng",
		   .owner = THIS_MODULE,
		   },
	.probe   = lpc313x_rng_probe,
	.remove  = __exit_p(lpc313x_rng_remove),
	.suspend = lpc313x_rng_suspend,
	.resume	 = lpc313x_rng_resume,
};

static int __init mod_init(void)
{
	return platform_driver_register(&lpc313x_rng_driver);
}

static void __exit mod_exit(void)
{
	platform_driver_unregister(&lpc313x_rng_driver);
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_AUTHOR("Ingo Albrecht");
MODULE_DESCRIPTION("H/W RNG driver for NXP LPC313x");
MODULE_LICENSE("GPL");
