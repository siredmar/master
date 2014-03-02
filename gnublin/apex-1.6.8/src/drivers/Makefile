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


obj-y := driver.o
obj-y += drv-mem.o

obj-$(CONFIG_DRIVER_FAT)		+= drv-fat.o
obj-$(CONFIG_DRIVER_EXT2)		+= drv-ext2.o
obj-$(CONFIG_DRIVER_JFFS2)		+= drv-jffs2.o
obj-$(CONFIG_DRIVER_NOR_CFI)		+= drv-nor-cfi.o
obj-$(CONFIG_DRIVER_COMPACTFLASH)	+= drv-cf.o
obj-$(CONFIG_DRIVER_SMC91X)		+= drv-smc91x.o
obj-$(CONFIG_DRIVER_CS8900)		+= drv-cs8900.o
obj-$(CONFIG_CMD_ETH_DM9000)		+= drv-dm9000.o
obj-$(CONFIG_DRIVER_DM9000)		+= drv-dm9000.o
obj-$(CONFIG_DRIVER_FIS)		+= drv-fis.o
obj-$(CONFIG_DRIVER_NAND)		+= drv-nand.o
obj-$(CONFIG_RELOCATE_NAND)		+= relocate-nand.o
obj-$(CONFIG_DRIVER_ONENAND)		+= drv-onenand.o
obj-$(CONFIG_RELOCATE_ONENAND)		+= relocate-onenand.o

obj-$(CONFIG_IXP4XX_NPE)		+= ixp4xx-npe/

ifneq ($(CONFIG_THUMB),)
 CFLAGS_driver.o	+= -mthumb
 CFLAGS_drv-fat.o	+= -mthumb
 CFLAGS_drv-ext2.o	+= -mthumb
 CFLAGS_drv-jffs2.o	+= -mthumb
 CFLAGS_drv-nor_cfi.o	+= -mthumb
 CFLAGS_drv-cf.o	+= -mthumb
 CFLAGS_drv-smc91x.o	+= -mthumb
 CFLAGS_drv-dm9000.o	+= -mthumb
 CFLAGS_drv-fis.o	+= -mthumb
 CFLAGS_drv-nand.o	+= -mthumb
 CFLAGS_drv-onenand.o	+= -mthumb
 CFLAGS_drv-nor-cfi.o	+= -mthumb
 CFLAGS_drv-mem.o	+= -mthumb
endif
