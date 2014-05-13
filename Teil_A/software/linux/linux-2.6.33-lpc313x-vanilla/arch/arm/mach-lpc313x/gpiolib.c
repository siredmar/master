/*
 * gpiolib.c - GPIOLIB platform support for NXP LPC313x
 *
 * Author: Ingo Albrecht <prom@berlin.ccc.de>
 *
 * Copyright 2010 (c) Ingo Albrecht
 *
 * This file is licensed under  the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#include <linux/seq_file.h>
#include <linux/gpio.h> 
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/io.h>

#include <mach/gpio.h> 

#include <mach/irqs.h>
#include <mach/registers.h>

#include <asm-generic/gpio.h>   /* 2011-12-11, added --hh */



/* Internal structures */

struct lpc313x_gpio_pin {
	char *name;
	int event_id;
	int pin; 
};

struct lpc313x_gpio_chip {
	struct gpio_chip  chip;
	void __iomem     *regbase;
	struct lpc313x_gpio_pin *pins;
};

#define to_lpc313x_gpio_chip(c) container_of(c, struct lpc313x_gpio_chip, chip)

#define LPC313X_GPIO_PIN(pname, pevent, pnr) \
	{              						\
		.name = pname, 					\
		.event_id = pevent,				\
    .pin = pnr, \
	}

#define LPC313X_GPIO_CHIP(name, basereg, base_gpio, nr_gpio, pinfo) \
    {                                                               \
    .chip = {                                                     	\
	    .label            = name,				     \
            .request          = lpc313x_gpiolib_request,          	\
            .free             = lpc313x_gpiolib_free,             	\
            .direction_input  = lpc313x_gpiolib_direction_input,  	\
            .direction_output = lpc313x_gpiolib_direction_output, 	\
            .get              = lpc313x_gpiolib_get,              	\
            .set              = lpc313x_gpiolib_set,              	\
            .to_irq           = lpc313x_gpiolib_to_irq,             \
            .dbg_show         = lpc313x_gpiolib_dbg_show,         	\
            .base             = (base_gpio),                      	\
            .ngpio            = (nr_gpio),                        	\
        },                                                        	\
    .regbase = ((void *)basereg),                                 	\
    .pins = ((struct lpc313x_gpio_pin *)pinfo), 					\
    }


/* Forward declarations */

static int lpc313x_gpiolib_request(struct gpio_chip *chip, unsigned offset);
static void lpc313x_gpiolib_free(struct gpio_chip *chip, unsigned offset);
static int lpc313x_gpiolib_direction_input(struct gpio_chip *chip,
					   unsigned offset);
static int lpc313x_gpiolib_direction_output(struct gpio_chip *chip,
					    unsigned offset, int val);
static int lpc313x_gpiolib_get(struct gpio_chip *chip, unsigned offset);
static void lpc313x_gpiolib_set(struct gpio_chip *chip,
				unsigned offset, int val);
static int lpc313x_gpiolib_to_irq(struct gpio_chip *chip,
				unsigned offset);
static void lpc313x_gpiolib_dbg_show(struct seq_file *s,
				     struct gpio_chip *chip);

/*
 * Table of GPIO chips.
 *
 * Each pin group on the chip is exposed as a separate chip.
 *
 * Chips without pin descriptions will work but will not have irq support.
 *
 */

static struct lpc313x_gpio_pin gpio_pins_gpio[] = {
		LPC313X_GPIO_PIN("GPIO0", EVT_GPIO0, 1),     /* 0 */
		LPC313X_GPIO_PIN("GPIO1", EVT_GPIO1, 0),
		LPC313X_GPIO_PIN("GPIO2", EVT_GPIO2, 2),
		LPC313X_GPIO_PIN("GPIO3", EVT_GPIO3, 3),
		LPC313X_GPIO_PIN("GPIO4", EVT_GPIO4, 4),   
		LPC313X_GPIO_PIN("-",  -1, -1),                  /* 5 */
		LPC313X_GPIO_PIN("-",  -1, -1),   
		LPC313X_GPIO_PIN("-",  -1, -1),   
		LPC313X_GPIO_PIN("-",  -1, -1),   
		LPC313X_GPIO_PIN("-",  -1, -1),   
		LPC313X_GPIO_PIN("-",  -1, -1),                 /* 10 */
		LPC313X_GPIO_PIN("GPIO11", EVT_GPIO11, 5), 
		LPC313X_GPIO_PIN("GPIO12", EVT_GPIO12, 6),
		LPC313X_GPIO_PIN("GPIO13", EVT_GPIO13, 7),
		LPC313X_GPIO_PIN("GPIO14", EVT_GPIO14, 8),
		LPC313X_GPIO_PIN("GPIO15", EVT_GPIO15, 9),
		LPC313X_GPIO_PIN("GPIO16", EVT_GPIO16, 10),
		LPC313X_GPIO_PIN("GPIO17", EVT_GPIO17, 11),
		LPC313X_GPIO_PIN("GPIO18", EVT_GPIO18, 12),
		LPC313X_GPIO_PIN("GPIO19", EVT_GPIO19, 13),
		LPC313X_GPIO_PIN("GPIO20", EVT_GPIO20, 14),
};

static struct lpc313x_gpio_pin gpio_pins_ebi_mci[] = {
		LPC313X_GPIO_PIN("mGPIO9",           EVT_mGPIO9, 0),
		LPC313X_GPIO_PIN("mGPIO6",           EVT_mGPIO6, 1),
		LPC313X_GPIO_PIN("mLCD_DB_7",        EVT_mLCD_DB_7, 2),
		LPC313X_GPIO_PIN("mLCD_DB_4",        EVT_mLCD_DB_4, 3),
		LPC313X_GPIO_PIN("mLCD_DB_2",        EVT_mLCD_DB_2, 4),
		LPC313X_GPIO_PIN("mNAND_RYBN0",      EVT_mNAND_RYBN0, 5),
		LPC313X_GPIO_PIN("mI2STX_CLK0",      EVT_mI2STX_CLK0, 6),
		LPC313X_GPIO_PIN("mI2STX_BCK0",      EVT_mI2STX_BCK0, 7),
		LPC313X_GPIO_PIN("EBI_A_1_CLE",      EVT_EBI_A_1_CLE, 8),
		LPC313X_GPIO_PIN("EBI_NCAS_BLOUT_0", EVT_EBI_NCAS_BLOUT_0, 9),
		LPC313X_GPIO_PIN("mLCD_DB_0",        EVT_mLCD_DB_0, 10),
		LPC313X_GPIO_PIN("EBI_DQM_0_NOE",    EVT_EBI_DQM_0_NOE, 11),
		LPC313X_GPIO_PIN("mLCD_CSB",         EVT_mLCD_CSB, 12),
		LPC313X_GPIO_PIN("mLCD_DB_1",        EVT_mLCD_DB_1, 13),
		LPC313X_GPIO_PIN("mLCD_E_RD",        EVT_mLCD_E_RD, 14),
		LPC313X_GPIO_PIN("mLCD_RS",          EVT_mLCD_RS, 15),
		LPC313X_GPIO_PIN("mLCD_RW_WR",       EVT_mLCD_RW_WR, 16),
		LPC313X_GPIO_PIN("mLCD_DB_3",        EVT_mLCD_DB_3, 17),
		LPC313X_GPIO_PIN("mLCD_DB_5",        EVT_mLCD_DB_5, 18),
		LPC313X_GPIO_PIN("mLCD_DB_6",        EVT_mLCD_DB_6, 19),
		LPC313X_GPIO_PIN("mLCD_DB_8",        EVT_mLCD_DB_8, 20),
		LPC313X_GPIO_PIN("mLCD_DB_9",        EVT_mLCD_DB_9, 21),
		LPC313X_GPIO_PIN("mLCD_DB_10",       EVT_mLCD_DB_10, 22),
		LPC313X_GPIO_PIN("mLCD_DB_11",       EVT_mLCD_DB_11, 23),
		LPC313X_GPIO_PIN("mLCD_DB_12",       EVT_mLCD_DB_12, 24),
		LPC313X_GPIO_PIN("mLCD_DB_13",       EVT_mLCD_DB_13, 25),
		LPC313X_GPIO_PIN("mLCD_DB_14",       EVT_mLCD_DB_14, 26),
		LPC313X_GPIO_PIN("mLCD_DB_15",       EVT_mLCD_DB_15, 27),
		LPC313X_GPIO_PIN("mGPIO5",           EVT_mGPIO5, 28),
		LPC313X_GPIO_PIN("mGPIO7",           EVT_mGPIO7, 29),
		LPC313X_GPIO_PIN("mGPIO8",           EVT_mGPIO8, 30),
		LPC313X_GPIO_PIN("mGPIO10",          EVT_mGPIO10, 31),
};

static struct lpc313x_gpio_pin gpio_pins_ebi_i2stx_0[] = {
		LPC313X_GPIO_PIN("mNAND_RYBN1",      EVT_mNAND_RYBN1, 0),
		LPC313X_GPIO_PIN("mNAND_RYBN2",      EVT_mNAND_RYBN2, 1),
		LPC313X_GPIO_PIN("mNAND_RYBN3",      EVT_mNAND_RYBN3, 2),
		LPC313X_GPIO_PIN("mUART_CTS_N",      EVT_mUART_CTS_N, 3),
		LPC313X_GPIO_PIN("mUART_RTS_N",      EVT_mUART_RTS_N, 4),
		LPC313X_GPIO_PIN("mI2STX_DATA0",     EVT_mI2STX_DATA0, 5),
		LPC313X_GPIO_PIN("mI2STX_WS0",       EVT_mI2STX_WS0, 6),
		LPC313X_GPIO_PIN("EBI_NRAS_BLOUT_1", EVT_EBI_NRAS_BLOUT_1, 7),
		LPC313X_GPIO_PIN("EBI_A_0_ALE",      EVT_EBI_A_0_ALE, 8),
		LPC313X_GPIO_PIN("EBI_NWE",          EVT_EBI_NWE, 9),
};

static struct lpc313x_gpio_pin gpio_pins_spi[] = {
		LPC313X_GPIO_PIN("SPI_MISO",    EVT_SPI_MISO, 0),
		LPC313X_GPIO_PIN("SPI_MOSI",    EVT_SPI_MOSI, 1),
		LPC313X_GPIO_PIN("SPI_CS_IN",   EVT_SPI_CS_IN, 2),
		LPC313X_GPIO_PIN("SPI_SCK",     EVT_SPI_SCK, 3),
		LPC313X_GPIO_PIN("SPI_CS_OUT0", EVT_SPI_CS_OUT0, 4),
};

static struct lpc313x_gpio_pin gpio_pins_uart[] = {
		LPC313X_GPIO_PIN("UART_RXD", EVT_UART_RXD, 0),
		LPC313X_GPIO_PIN("UART_TXD", EVT_UART_TXD, 1),
};

static struct lpc313x_gpio_chip gpio_chips[] = {
		LPC313X_GPIO_CHIP(
			"GPIO", IOCONF_GPIO,
			BASE_GPIO_GPIO, NUM_GPIO_GPIO,
			&gpio_pins_gpio),
		LPC313X_GPIO_CHIP(
			"EBI_MCI", IOCONF_EBI_MCI,
			BASE_GPIO_EBI_MCI, NUM_GPIO_EBI_MCI,
			&gpio_pins_ebi_mci),
		LPC313X_GPIO_CHIP(
			"EBI_I2STX_0", IOCONF_EBI_I2STX_0,
			BASE_GPIO_EBI_I2STX_0, NUM_GPIO_EBI_I2STX_0,
			&gpio_pins_ebi_i2stx_0),
		LPC313X_GPIO_CHIP(
			"CGU", IOCONF_CGU,
			BASE_GPIO_CGU, NUM_GPIO_CGU,
			NULL),
		LPC313X_GPIO_CHIP(
			"I2SRX_0", IOCONF_I2SRX_0,
			BASE_GPIO_I2SRX_0, NUM_GPIO_I2SRX_0,
			NULL),
		LPC313X_GPIO_CHIP(
			"I2SRX_1", IOCONF_I2SRX_1,
			BASE_GPIO_I2SRX_1, NUM_GPIO_I2SRX_1,
			NULL),
		LPC313X_GPIO_CHIP(
			"I2STX_1", IOCONF_I2STX_1,
			BASE_GPIO_I2STX_1, NUM_GPIO_I2STX_1,
			NULL),
		LPC313X_GPIO_CHIP(
			"EBI", IOCONF_EBI,
			BASE_GPIO_EBI, NUM_GPIO_EBI,
			NULL),
		LPC313X_GPIO_CHIP(
			"I2C1", IOCONF_I2C1,
			BASE_GPIO_I2C1, NUM_GPIO_I2C1,
			NULL),
		LPC313X_GPIO_CHIP(
			"SPI", IOCONF_SPI,
			BASE_GPIO_SPI, NUM_GPIO_SPI,
			&gpio_pins_spi),
		LPC313X_GPIO_CHIP(
			"NAND_CTRL", IOCONF_NAND_CTRL,
			BASE_GPIO_NAND_CTRL, NUM_GPIO_NAND_CTRL,
			NULL),
		LPC313X_GPIO_CHIP(
			"PWM", IOCONF_PWM,
			BASE_GPIO_PWM, NUM_GPIO_PWM,
			NULL),
		LPC313X_GPIO_CHIP(
			"UART", IOCONF_UART,
			BASE_GPIO_UART, NUM_GPIO_UART,
			&gpio_pins_uart),
};


static int lpc313x_gpiolib_request(struct gpio_chip *chip, unsigned offset)
{
	return lpc313x_gpiolib_direction_input(chip, offset);
}

static void lpc313x_gpiolib_free(struct gpio_chip *chip, unsigned offset)
{
	struct lpc313x_gpio_chip *pchip = to_lpc313x_gpio_chip(chip);
	unsigned long flags;
	unsigned port = ((unsigned)pchip->regbase);
	unsigned pin = (1 << pchip->pins[offset].pin);

	raw_local_irq_save(flags);

	GPIO_M0_RESET(port) = pin;
	GPIO_M1_SET(port) = pin;

	raw_local_irq_restore(flags);
}

static int lpc313x_gpiolib_direction_input(struct gpio_chip *chip,
					   unsigned offset)
{
	struct lpc313x_gpio_chip *pchip = to_lpc313x_gpio_chip(chip);
	unsigned long flags;
	unsigned port = ((unsigned)pchip->regbase);
	unsigned pin = (1 << pchip->pins[offset].pin);

	raw_local_irq_save(flags);

	GPIO_M1_RESET(port) = pin;
	GPIO_M0_RESET(port) = pin;

	raw_local_irq_restore(flags);

	return 0;
}

static int lpc313x_gpiolib_direction_output(struct gpio_chip *chip,
					    unsigned offset,
					    int value)
{
	struct lpc313x_gpio_chip *pchip = to_lpc313x_gpio_chip(chip);
	unsigned long flags;
	unsigned port = ((unsigned)pchip->regbase);
	unsigned pin = (1 << pchip->pins[offset].pin);

	if(pin == 16){  
	printk("GPI4 is input only and can not be used as an output!!! \n");  
	return 0;
	}
	
	raw_local_irq_save(flags);

	GPIO_M1_SET(port) = pin;

	if(value) {
		GPIO_M0_SET(port) = pin;
	} else {
		GPIO_M0_RESET(port) = pin;
	}

	raw_local_irq_restore(flags);

	return 0;
}

static int lpc313x_gpiolib_get(struct gpio_chip *chip, unsigned offset)
{
	struct lpc313x_gpio_chip *pchip = to_lpc313x_gpio_chip(chip);
	unsigned port = ((unsigned)pchip->regbase);
	unsigned pin = (1 << pchip->pins[offset].pin);


	if(GPIO_STATE(port) & pin) {
		return 1;
	} else {
		return 0;
	}
}

static void lpc313x_gpiolib_set(struct gpio_chip *chip,
				unsigned offset,
				int value)
{
	struct lpc313x_gpio_chip *pchip = to_lpc313x_gpio_chip(chip);
	unsigned long flags;
	unsigned port = ((unsigned)pchip->regbase);
	unsigned pin = (1 << pchip->pins[offset].pin);
    
	if(pin == 16){  
	printk("GPI4 is input only and can not be used as an output!!! \n");  
	return;
	}
	
	raw_local_irq_save(flags);

	GPIO_M1_SET(port) = pin;

	if(value) {
		GPIO_M0_SET(port) = pin;
	} else {
		GPIO_M0_RESET(port) = pin;
	}

	raw_local_irq_restore(flags);
}

static int lpc313x_gpiolib_to_irq(struct gpio_chip *chip, unsigned offset)
{
	struct lpc313x_gpio_chip *pchip = to_lpc313x_gpio_chip(chip);

//printk("GPIO_TO_IRQ: GPIO_NAME=%s, GPIO_ID=%d, IRQ_ID=%d EVENT_ID=%d\n",pchip->pins[offset].name, offset, (event + (NR_IRQ_CPU )), pchip->pins[offset].event_id );
	if(pchip->pins) {
		int event = pchip->pins[offset].event_id;
		if(event >= 0) {
      return (event + (NR_IRQ_CPU) );   
             /* Now IRQ number is correct IRQ = event + NR_IRQ_CPU  --BN */
		}
	}
	return -1;
}

static void lpc313x_gpiolib_dbg_show(struct seq_file *s, struct gpio_chip *chip)
{
	struct lpc313x_gpio_chip *pchip = to_lpc313x_gpio_chip(chip);
	unsigned long flags;
	unsigned port = ((unsigned)pchip->regbase);
	unsigned pin, mode0, mode1, state;

	int i;

	for (i = 0; i < chip->ngpio; i++) {
		const char *gpio_label;

		pin = (1 << i);

		gpio_label = gpiochip_is_requested(chip, i);

		if (gpio_label) {

			raw_local_irq_save(flags);
			mode0 = GPIO_STATE_M0(port) & pin;
			mode1 = GPIO_STATE_M1(port) & pin;
			state = GPIO_STATE(port) & pin;
			raw_local_irq_restore(flags);

			seq_printf(s, "[%s] ", gpio_label);
			if(pchip->pins) {
				seq_printf(s, "%s: ", pchip->pins[i].name);
			} else {
				seq_printf(s, "%s[%d/%d]: ", chip->label, i, pchip->pins[i].pin);
			}

			if(mode1) {
				seq_printf(s, "output %s\n",
					   mode0 ? "high" : "low");
			} else {
				if(mode0) {
					seq_printf(s, "device function\n");
				} else {
					seq_printf(s, "input %s\n",
						   state ? "high" : "low");
				}
			}
		}
	}
}

void __init lpc313x_gpiolib_init()
{
	int numchips = sizeof(gpio_chips) / sizeof(struct lpc313x_gpio_chip);
	int i;

	for(i = 0; i < numchips; i++) {
		gpiochip_add(&gpio_chips[i].chip);
	}
}
