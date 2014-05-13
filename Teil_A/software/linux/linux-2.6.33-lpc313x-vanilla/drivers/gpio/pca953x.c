/*
 *  pca953x.c - 4/8/16 bit I/O ports
 *
 *  Copyright (C) 2005 Ben Gardner <bgardner@wabtec.com>
 *  Copyright (C) 2007 Marvell International Ltd.
 *
 *  Derived from drivers/i2c/chips/pca9539.c
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/i2c.h>
#include <linux/i2c/pca953x.h>
#ifdef CONFIG_OF_GPIO
#include <linux/of_platform.h>
#include <linux/of_gpio.h>
#endif

#define PCA953X_INPUT          0
#define PCA953X_OUTPUT         1
#define PCA953X_INVERT         2
#define PCA953X_DIRECTION      3


static int cnt = 1;
module_param(cnt, int, S_IRUGO);
MODULE_PARM_DESC(cnt, "Number of pca9555 slaves on the i2c-bus. Default is 1");

static char addr1[5] = "";
module_param_string(addr1, addr1, sizeof(addr1), S_IRUGO);
MODULE_PARM_DESC(addr1, "Adress of pca9555 chip 1. E.g addr1=0x20");


static char addr2[5] = "";
module_param_string(addr2, addr2, sizeof(addr2), S_IRUGO);
MODULE_PARM_DESC(addr2, "Adress of pca9555 chip 2");


static char addr3[5] = "";
module_param_string(addr3, addr3, sizeof(addr3), S_IRUGO);
MODULE_PARM_DESC(addr3, "Adress of pca9555 chip 3");


static char addr4[5] = "";
module_param_string(addr4, addr4, sizeof(addr4), S_IRUGO);
MODULE_PARM_DESC(addr4, "Adress of pca9555 chip 4");


static char addr5[5] = "";
module_param_string(addr5, addr5, sizeof(addr5), S_IRUGO);
MODULE_PARM_DESC(addr5, "Adress of pca9555 chip 5");

 
static const struct i2c_device_id pca953x_id[] = {
	{ "pca9534", 8, },
	{ "pca9535", 16, },
	{ "pca9536", 4, },
	{ "pca9537", 4, },
	{ "pca9538", 8, },
	{ "pca9539", 16, },
	{ "pca9554", 8, },
	{ "pca9555", 16, },
	{ "pca9556", 8, },
	{ "pca9557", 8, },

	{ "max7310", 8, },
	{ "max7315", 8, },
	{ "pca6107", 8, },
	{ "tca6408", 8, },
	{ "tca6416", 16, },
	/* NYET:  { "tca6424", 24, }, */
	{ }
};
MODULE_DEVICE_TABLE(i2c, pca953x_id);

struct pca953x_chip {
	unsigned gpio_start;
	uint16_t reg_output;
	uint16_t reg_direction;

	struct i2c_client *client;
	struct pca953x_platform_data *dyn_pdata;
	struct gpio_chip gpio_chip;
	char **names;
};

static int pca953x_write_reg(struct pca953x_chip *chip, int reg, uint16_t val)
{
	int ret;

	if (chip->gpio_chip.ngpio <= 8)
		ret = i2c_smbus_write_byte_data(chip->client, reg, val);
	else
		ret = i2c_smbus_write_word_data(chip->client, reg << 1, val);

	if (ret < 0) {
		dev_err(&chip->client->dev, "failed writing register\n");
		return ret;
	}

	return 0;
}

static int pca953x_read_reg(struct pca953x_chip *chip, int reg, uint16_t *val)
{
	int ret;

	if (chip->gpio_chip.ngpio <= 8)
		ret = i2c_smbus_read_byte_data(chip->client, reg);
	else
		ret = i2c_smbus_read_word_data(chip->client, reg << 1);

	if (ret < 0) {
		dev_err(&chip->client->dev, "failed reading register\n");
		return ret;
	}

	*val = (uint16_t)ret;
	return 0;
}

static int pca953x_gpio_direction_input(struct gpio_chip *gc, unsigned off)
{
	struct pca953x_chip *chip;
	uint16_t reg_val;
	int ret;

	chip = container_of(gc, struct pca953x_chip, gpio_chip);

	reg_val = chip->reg_direction | (1u << off);
	ret = pca953x_write_reg(chip, PCA953X_DIRECTION, reg_val);
	if (ret)
		return ret;

	chip->reg_direction = reg_val;
	return 0;
}

static int pca953x_gpio_direction_output(struct gpio_chip *gc,
		unsigned off, int val)
{
	struct pca953x_chip *chip;
	uint16_t reg_val;
	int ret;

	chip = container_of(gc, struct pca953x_chip, gpio_chip);

	/* set output level */
	if (val)
		reg_val = chip->reg_output | (1u << off);
	else
		reg_val = chip->reg_output & ~(1u << off);

	ret = pca953x_write_reg(chip, PCA953X_OUTPUT, reg_val);
	if (ret)
		return ret;

	chip->reg_output = reg_val;

	/* then direction */
	reg_val = chip->reg_direction & ~(1u << off);
	ret = pca953x_write_reg(chip, PCA953X_DIRECTION, reg_val);
	if (ret)
		return ret;

	chip->reg_direction = reg_val;
	return 0;
}

static int pca953x_gpio_get_value(struct gpio_chip *gc, unsigned off)
{
	struct pca953x_chip *chip;
	uint16_t reg_val;
	int ret;

	chip = container_of(gc, struct pca953x_chip, gpio_chip);

	ret = pca953x_read_reg(chip, PCA953X_INPUT, &reg_val);
	if (ret < 0) {
		/* NOTE:  diagnostic already emitted; that's all we should
		 * do unless gpio_*_value_cansleep() calls become different
		 * from their nonsleeping siblings (and report faults).
		 */
		return 0;
	}

	return (reg_val & (1u << off)) ? 1 : 0;
}

static void pca953x_gpio_set_value(struct gpio_chip *gc, unsigned off, int val)
{
	struct pca953x_chip *chip;
	uint16_t reg_val;
	int ret;

	chip = container_of(gc, struct pca953x_chip, gpio_chip);

	if (val)
		reg_val = chip->reg_output | (1u << off);
	else
		reg_val = chip->reg_output & ~(1u << off);

	ret = pca953x_write_reg(chip, PCA953X_OUTPUT, reg_val);
	if (ret)
		return;

	chip->reg_output = reg_val;
}

static void pca953x_setup_gpio(struct pca953x_chip *chip, int gpios)
{
	struct gpio_chip *gc;

	gc = &chip->gpio_chip;

	/* Set handler and functionality called by gpiolib ! */
	gc->direction_input  = pca953x_gpio_direction_input;
	gc->direction_output = pca953x_gpio_direction_output;
	gc->get = pca953x_gpio_get_value;
	gc->set = pca953x_gpio_set_value;
	gc->can_sleep = 1;

	gc->base = chip->gpio_start;
	gc->ngpio = gpios;
	gc->label = chip->client->name;

	/* Set device */
	gc->dev = &chip->client->dev;
	gc->owner = THIS_MODULE;
	gc->names = chip->names;
}

/*
 * Handlers for alternative sources of platform_data
 */
#ifdef CONFIG_OF_GPIO
/*
 * Translate OpenFirmware node properties into platform_data
 */
static struct pca953x_platform_data *
pca953x_get_alt_pdata(struct i2c_client *client)
{
	struct pca953x_platform_data *pdata;
	struct device_node *node;
	const uint16_t *val;

	node = dev_archdata_get_node(&client->dev.archdata);
	if (node == NULL)
		return NULL;

	pdata = kzalloc(sizeof(struct pca953x_platform_data), GFP_KERNEL);
	if (pdata == NULL) {
		dev_err(&client->dev, "Unable to allocate platform_data\n");
		return NULL;
	}

	pdata->gpio_base = -1;
	val = of_get_property(node, "linux,gpio-base", NULL);
	if (val) {
		if (*val < 0)
			dev_warn(&client->dev,
				 "invalid gpio-base in device tree\n");
		else
			pdata->gpio_base = *val;
	}

	val = of_get_property(node, "polarity", NULL);
	if (val)
		pdata->invert = *val;

	return pdata;
}
#else
static struct pca953x_platform_data *
pca953x_get_alt_pdata(struct i2c_client *client)
{
	return NULL;
}
#endif

static int __devinit pca953x_probe(struct i2c_client *client,
				   const struct i2c_device_id *id)
{
	struct pca953x_platform_data *pdata;
	struct pca953x_chip *chip;
	int ret;

	chip = kzalloc(sizeof(struct pca953x_chip), GFP_KERNEL);
	if (chip == NULL)
		return -ENOMEM;

	pdata = client->dev.platform_data;
	if (pdata == NULL) {
		pdata = pca953x_get_alt_pdata(client);
		/*
		 * Unlike normal platform_data, this is allocated
		 * dynamically and must be freed in the driver
		 */
		chip->dyn_pdata = pdata;
	}

	if (pdata == NULL) {
		dev_dbg(&client->dev, "no platform data\n");
		ret = -EINVAL;
		goto out_failed;
	}

	chip->client = client;

	chip->gpio_start = pdata->gpio_base;

	chip->names = pdata->names;

	/* initialize cached registers from their original values.
	 * we can't share this chip with another i2c master.
	 */
	pca953x_setup_gpio(chip, id->driver_data);

	ret = pca953x_read_reg(chip, PCA953X_OUTPUT, &chip->reg_output);
	if (ret)
		goto out_failed;

	ret = pca953x_read_reg(chip, PCA953X_DIRECTION, &chip->reg_direction);
	if (ret)
		goto out_failed;

	/* set platform specific polarity inversion */
	ret = pca953x_write_reg(chip, PCA953X_INVERT, pdata->invert);
	if (ret)
		goto out_failed;


	ret = gpiochip_add(&chip->gpio_chip);
	if (ret)
		goto out_failed;

	if (pdata->setup) {
		ret = pdata->setup(client, chip->gpio_chip.base,
				chip->gpio_chip.ngpio, pdata->context);
		if (ret < 0)
			dev_warn(&client->dev, "setup failed, %d\n", ret);
	}

	i2c_set_clientdata(client, chip);
	return 0;

out_failed:
	kfree(chip->dyn_pdata);
	kfree(chip);
	return ret;
}

static int pca953x_remove(struct i2c_client *client)
{
	struct pca953x_platform_data *pdata = client->dev.platform_data;
	struct pca953x_chip *chip = i2c_get_clientdata(client);
	int ret = 0;

	if (pdata->teardown) {
		ret = pdata->teardown(client, chip->gpio_chip.base,
				chip->gpio_chip.ngpio, pdata->context);
		if (ret < 0) {
			dev_err(&client->dev, "%s failed, %d\n",
					"teardown", ret);
			return ret;
		}
	}

	ret = gpiochip_remove(&chip->gpio_chip);
	if (ret) {
		dev_err(&client->dev, "%s failed, %d\n",
				"gpiochip_remove()", ret);
		return ret;
	}

	kfree(chip->dyn_pdata);
	kfree(chip);
	return 0;
}

static struct i2c_driver pca953x_driver = {
	.driver = {
		.name	= "pca953x",
	},
	.probe		= pca953x_probe,
	.remove		= pca953x_remove,
	.id_table	= pca953x_id,
};



struct pca953x_platform_data pca9555_plaform_info[] = {
		{
			.gpio_base = 98,
			.invert = 0,
		},
		{
			.gpio_base = 114,
			.invert = 0,
		},
		{
			.gpio_base = 130,
			.invert = 0,
		},
		{
			.gpio_base = 146,
			.invert = 0,
		},
		{
			.gpio_base = 162,
			.invert = 0,
		},
};

/*
static struct i2c_board_info pca953x_i2c_devices[] = {
	{
		.type = "pca9555",
		.addr= 0x4d,
		.platform_data = &pca9555_plaform_info[0],
	},
	{
		.type = "pca9555",
		.addr= 0x4c,
		.platform_data = &pca9555_plaform_info[1],
	},
	{
		.type = "pca9555",
		.addr= 0x4b,
		.platform_data = &pca9555_plaform_info[2],
	},
};
*/

static int get_platform_data_pca953x(void)
{
	struct i2c_board_info      board_info[10];
	struct i2c_adapter *i2c_adap;
	struct i2c_client *dev;
	int  i;
	int addr1_int,addr2_int,addr3_int,addr4_int,addr5_int;
for(i=1; i <= cnt; i++) 
{
	if(i == 1) {
			printk("ADRESS 1=%s\n",addr1);
			memset(&board_info[i-1], 0, sizeof(struct i2c_board_info));
			addr1_int = simple_strtol (addr1, NULL, 16);
			i2c_adap = i2c_get_adapter(1);
			strlcpy(board_info[i-1].type, "pca9555", I2C_NAME_SIZE);
			board_info[i-1].addr = addr1_int;
			board_info[i-1].platform_data = &pca9555_plaform_info[i-1];
			dev = i2c_new_device(i2c_adap, &board_info[i-1]);

	} else if (i == 2) {
			printk("ADRESS 2=%s\n",addr2);
			memset(&board_info[i-1], 0, sizeof(struct i2c_board_info));
			addr2_int = simple_strtol (addr2, NULL, 16);
			i2c_adap = i2c_get_adapter(1);
			strlcpy(board_info[i-1].type, "pca9555", I2C_NAME_SIZE);
			board_info[i-1].addr = addr2_int;
			board_info[i-1].platform_data = &pca9555_plaform_info[i-1];
			dev = i2c_new_device(i2c_adap, &board_info[i-1]);
	
	} else if (i == 3) {
			printk("ADRESS 3=%s\n",addr3);
			memset(&board_info[i-1], 0, sizeof(struct i2c_board_info));
			addr3_int = simple_strtol (addr3, NULL, 16);
			i2c_adap = i2c_get_adapter(1);
			strlcpy(board_info[i-1].type, "pca9555", I2C_NAME_SIZE);
			board_info[i-1].addr = addr3_int;
			board_info[i-1].platform_data = &pca9555_plaform_info[i-1];
			dev = i2c_new_device(i2c_adap, &board_info[i-1]);
	} else if (i == 4) {
			printk("ADRESS 4=%s\n",addr4);
			memset(&board_info[i-1], 0, sizeof(struct i2c_board_info));
			addr4_int = simple_strtol (addr4, NULL, 16);
			i2c_adap = i2c_get_adapter(1);
			strlcpy(board_info[i-1].type, "pca9555", I2C_NAME_SIZE);
			board_info[i-1].addr = addr4_int;
			board_info[i-1].platform_data = &pca9555_plaform_info[i-1];
			dev = i2c_new_device(i2c_adap, &board_info[i-1]);
	} else if (i == 5) {
			printk("ADRESS 5=%s\n",addr5);
			memset(&board_info[i-1], 0, sizeof(struct i2c_board_info));
			addr5_int = simple_strtol (addr5, NULL, 16);
			i2c_adap = i2c_get_adapter(1);
			strlcpy(board_info[i-1].type, "pca9555", I2C_NAME_SIZE);
			board_info[i-1].addr = addr5_int;
			board_info[i-1].platform_data = &pca9555_plaform_info[i-1];
			dev = i2c_new_device(i2c_adap, &board_info[i-1]);
	}
}

}


static int __init pca953x_init(void)
{
	
	get_platform_data_pca953x();
	return i2c_add_driver(&pca953x_driver);
}
/* register after i2c postcore initcall and before
 * subsys initcalls that may rely on these GPIOs
 */
subsys_initcall(pca953x_init);

static void __exit pca953x_exit(void)
{
	i2c_del_driver(&pca953x_driver);
}
module_exit(pca953x_exit);

MODULE_AUTHOR("eric miao <eric.miao@marvell.com>");
MODULE_DESCRIPTION("GPIO expander driver for PCA953x");
MODULE_LICENSE("GPL");
