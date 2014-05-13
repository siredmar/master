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

obj-y := initialize.o serial.o timer.o env.o lpc313x_cgu_default.o lpc313x_cgu_driver.o drv-crc32.o 
obj-y += cmd-reset.o
obj-y += drv-mmc.o
obj-$(CONFIG_DRIVER_NAND)	+= drv-lpcnand.o
obj-$(CONFIG_DRIVER_SPINOR)	+= drv-spinor.o
obj-$(CONFIG_HAS_ANALOG_DIE)	+= lpc315x_ad.o
obj-$(CONFIG_USB_BOOT)		+= usb/usbmsc.d
obj-$(CONFIG_LOGO_TUX) += boot_logo_tux.o


obj-y += $(obj-xyy)

CFLAGS_env.o	     += -DBUILDDATE=\"$(BUILDDATE)\"
CFLAGS_env.o	     += -DAPEXVERSION=\"$(APEXVERSION)\"

