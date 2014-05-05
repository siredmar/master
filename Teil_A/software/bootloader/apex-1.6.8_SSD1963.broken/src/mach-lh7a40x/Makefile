#
# Makefile for APEX loader
#
#   Copyright (C) 2004 Marc Singer
#
#   This program is free software; you can redistribute it and/or
#   modify it under the terms of the GNU General Public License
#   version 2 as published by the Free Software Foundation.
#   Please refer to the file debian/copyright for further details.
#

obj-y := arch.o initialize.o serial.o timer.o env.o cpuinfo.o
obj-y += cmd-spi.o
obj-$(CONFIG_CLCDC)		+= clcdc.o
obj-$(CONFIG_CODEC)		+= codec.o
obj-$(CONFIG_ADC)		+= adc.o
obj-y				+= cmd-reset.o
obj-y				+= cmd-standby.o
obj-$(CONFIG_CMD_ARM920_TEST)	+= cmd-cp15test.o
obj-$(CONFIG_RELOCATE_COMPANION_EVT1) += relocate-companion.o
obj-$(CONFIG_MACH_COMPANION)    += clcdc-companion.o
obj-$(CONFIG_MACH_KARMA)	+= clcdc-karma.o
obj-$(CONFIG_MACH_KARMA)	+= cmd-karmatouch.o cmd-karmaaccel.o
obj-$(CONFIG_RELOCATE_COMPANION_EVT2) += preinitialization-companion.o

obj-$(CONFIG_MACH_COMPANION)	+= test-companion.o
obj-x$(CONFIG_MACH_COMPANION)$(CONFIG_VARIATIONS) += variation-companion.o

obj-y += $(obj-xyy)
