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

obj-y := serial.o env.o cpuinfo.o

obj-$(CONFIG_MACH_LNODE80)      += init-lnode80.o timer-lh79520.o
obj-$(CONFIG_MACH_LPD79520)     += init-lpd79520.o timer-lh79520.o

obj-$(CONFIG_MACH_LPD79524)     += init-lpd79524.o timer-lh79524.o
obj-$(CONFIG_MACH_KEV79524)     += init-kev79524.o timer-lh79524.o
obj-$(CONFIG_MACH_LPD79525)     += init-lpd79524.o timer-lh79524.o
obj-$(CONFIG_MACH_KEV79525)     += init-kev79524.o timer-lh79524.o
obj-$(CONFIG_MACH_MOTO_EDGE)    += init-motoedge.o timer-lh79524.o

obj-x$(CONFIG_INTERRUPTS)$(CONFIG_ARCH_LH79524)	+= exception_vectors.o
obj-$(CONFIG_DRIVER_CPLD_SPI)	+= drv-cpld-spi.o
obj-$(CONFIG_DRIVER_EMAC_LH79524) += drv-emac.o
obj-$(CONFIG_CLCDC)		+= clcdc.o
obj-$(CONFIG_CODEC)		+= codec.o
obj-$(CONFIG_CODEC_LH79520)	+= codec-lh79520.o
obj-$(CONFIG_ADS7843)		+= ads7843.o
obj-y				+= cmd-reset.o

obj-y += $(obj-xyy)
