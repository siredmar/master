/*
 * sound/soc/lpc313x/lpc313x-uda1380.c
 *
 * Author: Kevin Wells <kevin.wells@nxp.com>
 *
 * Copyright (C) 2009 NXP Semiconductors
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
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/clk.h>
#include <linux/timer.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>

#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <sound/uda1380.h>

#include "../codecs/uda1380.h"
#include "lpc313x-pcm.h"
#include "lpc313x-i2s.h"
#include "lpc313x-i2s-clocking.h"

#define SND_MODNAME "lpc313x_uda1380"

static int ea3131_uda1380_hw_params(struct snd_pcm_substream *substream,
			      struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *cpu_dai = rtd->cpu_dai;
	struct snd_soc_dai *codec_dai = rtd->codec_dai;
	const unsigned int fmt = (SND_SOC_DAIFMT_I2S |
				  SND_SOC_DAIFMT_CBS_CFS);
	int ret;


	/* Set the CPU I2S rate clock (first) */
	ret = snd_soc_dai_set_sysclk(cpu_dai, 0, params_rate(params),
					    SND_SOC_CLOCK_OUT);
	if (ret < 0) {
		pr_warning("%s: "
			   "Failed to set I2S clock (%d)\n",
			   SND_MODNAME, ret);
		return ret;
	}

	/* Set CPU and CODEC DAI format */
	ret = snd_soc_dai_set_fmt(cpu_dai, fmt);
	if (ret < 0) {
		pr_warning("%s: "
			   "Failed to set CPU DAI format (%d)\n",
			   SND_MODNAME, ret);
		return ret;
	}
	ret = snd_soc_dai_set_fmt(codec_dai, fmt);
	if (ret < 0) {
		pr_warning("%s: "
			   "Failed to set CODEC DAI format (%d)\n",
			   SND_MODNAME, ret);
		return ret;
	}

	return 0;
}

static struct snd_soc_ops ea3131_uda1380_ops = {
	.hw_params = ea3131_uda1380_hw_params,
};

static const struct snd_soc_dapm_widget ea3131_dapm_widgets[] = {
	SND_SOC_DAPM_HP("Headphone Jack", NULL),
	SND_SOC_DAPM_LINE("Line Out", NULL),
	SND_SOC_DAPM_MIC("Mic Jack", NULL),
	SND_SOC_DAPM_LINE("Line In", NULL),
};

static const struct snd_soc_dapm_route intercon[] = {
	/* Headphone connected to VOUTRHP, VOUTLHP */
	{"Headphone Jack", NULL, "VOUTRHP"},
	{"Headphone Jack", NULL, "VOUTLHP"},

	/* Line Out connected to VOUTR, VOUTL */
	{"Line Out", NULL, "VOUTR"},
	{"Line Out", NULL, "VOUTL"},

	/* Mic connected to VINM */
	{"VINM", NULL, "Mic Jack"},

	/* Line In connected to VINR, VINL */
	{"VINL", NULL, "Line In"},
	{"VINR", NULL, "Line In"},
};

static int ea3131_uda1380_init(struct snd_soc_pcm_runtime *rtd)
{
	struct snd_soc_codec *codec = rtd->codec;
	struct snd_soc_dapm_context *dapm = &codec->dapm;

	/* Add widgets */
	snd_soc_dapm_new_controls(dapm, ea3131_dapm_widgets,
				  ARRAY_SIZE(ea3131_dapm_widgets));

	/* Set up audio path audio_map */
	snd_soc_dapm_add_routes(dapm, intercon, ARRAY_SIZE(intercon));

	/* Always connected pins */
	snd_soc_dapm_enable_pin(dapm, "Headphone Jack");
	snd_soc_dapm_enable_pin(dapm, "Line Out");
	snd_soc_dapm_enable_pin(dapm, "Mic Jack");
	snd_soc_dapm_enable_pin(dapm, "Line In");

	snd_soc_dapm_sync(dapm);

	return 0;
}

static struct snd_soc_dai_link ea3131_uda1380_dai[] = {
	{
		.name = "uda1380",
		.stream_name = "uda1380",
		.codec_name	= "uda1380-codec.1-001a",
		.cpu_dai_name = "lpc313x-i2s.0",
		.codec_dai_name = "uda1380-hifi",
		.platform_name	= "lpc313x-audio.0",
		.init = ea3131_uda1380_init,
		.ops = &ea3131_uda1380_ops,
	},
};

static struct snd_soc_card snd_soc_machine_ea3131 = {
	.name = "lpc313x-i2s-uda1380",
	.dai_link = &ea3131_uda1380_dai[0],
	.num_links = ARRAY_SIZE(ea3131_uda1380_dai),
};

static struct uda1380_platform_data uda1380_info = {
	.dac_clk    = UDA1380_DAC_CLK_SYSCLK,
	.gpio_power = -1,
	.gpio_reset = -1,
};

static struct i2c_board_info i2c_board_info[] = {
	{
		I2C_BOARD_INFO("uda1380", 0x1A),
		.platform_data = &uda1380_info,
	},
};

static struct platform_device *ea3131_snd_device;

static int __devinit ea3131_asoc_probe(struct platform_device *pd)
{
	struct platform_device *snd_dev;
	int ret = 0;
	struct i2c_adapter *adapter;
	struct i2c_client *client;

	/*
	 * Enable CODEC clock first or I2C will fail to the CODEC
	 */
	lpc313x_main_clk_rate(48000);

#if defined (CONFIG_SND_I2C1_CHANNEL_UDA1380)
	adapter = i2c_get_adapter(1);
#else
	adapter = i2c_get_adapter(0);
#endif
	if (!adapter)
		return -ENODEV;
	client = i2c_new_device(adapter, i2c_board_info);
	i2c_put_adapter(adapter);
	if (!client)
		return -ENODEV;

	snd_dev = platform_device_alloc("soc-audio", -1);
	if (!snd_dev) {
		dev_err(&pd->dev, "failed to alloc soc-audio devicec\n");
		return -ENOMEM;
	}

	platform_set_drvdata(snd_dev, &snd_soc_machine_ea3131);

	ret = platform_device_add(snd_dev);
	if (ret) {
		dev_err(&pd->dev, "failed to add soc-audio dev\n");
		return -ENODEV;
	}

	platform_set_drvdata(pd, snd_dev);
	return ret;
}

static int __devexit ea3131_asoc_remove(struct platform_device *pdev)
{
	platform_device_unregister(ea3131_snd_device);
	lpc313x_main_clk_rate(0);
	ea3131_snd_device = NULL;
	return 0;
}

static struct platform_driver ea3131_asoc_platdrv = {
	.driver	= {
		.owner	= THIS_MODULE,
		.name	= "lpc313x-uda1380",
		//.pm	= ea3131_asoc_pm,
	},
	.probe	= ea3131_asoc_probe,
	.remove	= __devexit_p(ea3131_asoc_remove),
};

static int __init ea3131_asoc_modinit(void)
{
	return platform_driver_register(&ea3131_asoc_platdrv);
}

static void __exit ea3131_asoc_modexit(void)
{
	platform_driver_unregister(&ea3131_asoc_platdrv);
}

module_init(ea3131_asoc_modinit);
module_exit(ea3131_asoc_modexit);

MODULE_AUTHOR("Kevin Wells <kevin.wells@nxp.com>");
MODULE_DESCRIPTION("ASoC machine driver for LPC313X/UDA1380");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:lpc313x-uda1380");

