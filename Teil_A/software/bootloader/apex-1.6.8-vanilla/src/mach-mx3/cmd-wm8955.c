/* cmd-wm8955.c

   written by Marc Singer
   22 Dec 2008

   Copyright (C) 2008 Marc Singer

   -----------
   DESCRIPTION
   -----------

   APEX command for sending control messages to the WM8955 codec.

*/



#include <config.h>
#include <apex.h>
#include <driver.h>
#include <service.h>
#include <command.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <spinner.h>
#include <asm/reg.h>
#include <error.h>
#include <environment.h>
#include <lookup.h>

//#define TALK 1
#include <talk.h>

#include "hardware.h"

#if defined (CONFIG_ENV)
static __env struct env_d e_wm8955_i2c_drv = {
  .key = "wm8955-i2c-drv",
  .default_value = CONFIG_DRIVER_WM8955_I2C_DRIVER,
  .description = "I2C driver for wm8955 codec control interface",
};
#endif

static inline const char* i2c_driver (void)
{
  return lookup_alias_or_env ("wm8955-i2c-drv",
                              CONFIG_DRIVER_WM8955_I2C_DRIVER);
}


static int cmd_wm8955 (int argc, const char** argv)
{
  int reg;
  int data;
  char rgb[2];
  u16 message;
  struct descriptor_d d;
  int result;

  if (argc < 3)
    return ERROR_PARAM;

  reg  = simple_strtoul (argv[1], NULL, 0);
  data = simple_strtoul (argv[2], NULL, 0);

  if (   (result = parse_descriptor (i2c_driver (), &d))
      || (result = open_descriptor (&d))) {
    PRINTF ("%s: unable to open i2c driver '%s'\n", __FUNCTION__, sz);
    return result;
  }

  message = ((reg & 0x7f) << 9) | (data & 0x1ff);
  rgb[0] = (message >> 8) & 0xff;
  rgb[1] = (message >> 0) & 0xff;

  printf ("writing [%2d (0x%02x)]<- 0x%03x 0x%x 0x%x\n",
          reg, reg, data, rgb[0], rgb[1]);

  result = d.driver->write (&d, rgb, 2);
  close_descriptor (&d);

  return (result == 2) ? 0 : result;
}


void wm8955_init (void)
{
  /* MCLK pin to WM8955L */
  IOMUX_PIN_CONFIG_GPIO  (MX31_PIN_GPIO3_1);
  GPIO_PIN_CONFIG_OUTPUT (MX31_PIN_GPIO3_1);
  GPIO_PIN_SET           (MX31_PIN_GPIO3_1);
}

static __service_6 struct service_d wm8955_service = {
  .init = wm8955_init,
//  .release = wm8955_release,
#if !defined (CONFIG_SMALL)
//  .report = wm8955_report,
#endif
};


static __command struct command_d c_wm8955 = {
  .command = "wm8955",
  .func = (command_func_t) cmd_wm8955,
  COMMAND_DESCRIPTION ("send I2C commands to wm8955")
  COMMAND_HELP(
"wm8955 REG DATA\n"
"  Send DATA to REG of the WM8955L.\n"
  )
};
