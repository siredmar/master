#
# Makefile for APEX loader
#
#   Copyright (C) 2008 Marc Singer
#
#   This program is free software; you can redistribute it and/or
#   modify it under the terms of the GNU General Public License
#   version 2 as published by the Free Software Foundation.
#   Please refer to the file debian/copyright for further details.
#

obj-y += cpu.o serial.o initialize.o timer.o
obj-y += cpuinfo.o env.o
obj-y += cmd-reset.o
obj-y += drv-ata.o
obj-y += atag-marvell.o
# cmd-sleep.o

ifneq ($(CONFIG_THUMB),)
 CFLAGS_drv-ata.o	+= -mthumb
 CFLAGS_serial.o	+= -mthumb
 CFLAGS_timer.o		+= -mthumb
endif
