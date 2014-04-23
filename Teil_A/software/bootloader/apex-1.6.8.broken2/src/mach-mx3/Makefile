#
# Makefile for APEX loader
#
#   Copyright (C) 2006 Marc Singer
#
#   This program is free software; you can redistribute it and/or
#   modify it under the terms of the GNU General Public License
#   version 2 as published by the Free Software Foundation.
#   Please refer to the file debian/copyright for further details.
#

obj-y += initialize.o timer.o env.o cpuinfo.o cmd-reset.o cmd-sleep.o
obj-$(CONFIG_EXCEPTIONS)        += exception_vectors.o
obj-$(CONFIG_DRIVER_ONENAND)	+= cmd-onenand.o
obj-$(CONFIG_IPU_TEST)		+= ipu.o
obj-$(CONFIG_MX31ADS_UARTA)	+= serial-sc16c652.o
obj-$(CONFIG_MX31_UART1)	+= serial-mx31.o
obj-$(CONFIG_MACH_KARMA5)	+= karma-cmd-clcdc.o karma-cmd-accel.o
obj-$(CONFIG_MX31_I2C_DRIVER)	+= drv-i2c.o
obj-$(CONFIG_CMD_WM8955)	+= cmd-wm8955.o

ifneq ($(CONFIG_THUMB),)
  CFLAGS_timer.o		+= -mthumb
#  CFLAGS_initialize.o		+= -mthumb
  CFLAGS_timer.o		+= -mthumb
  CFLAGS_env.o			+= -mthumb
#  CFLAGS_cpuinfo.o		+= -mthumb
  CFLAGS_cmd-reset.o		+= -mthumb
#  CFLAGS_cmd-sleep.o		+= -mthumb
  CFLAGS_serial-mx31.o		+= -mthumb
  CFLAGS_drv-i2c.o		+= -mthumb
  CFLAGS_cmd-8955.o		+= -mthumb
endif
