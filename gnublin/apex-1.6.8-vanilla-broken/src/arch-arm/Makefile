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

LDFLAGS_apex := -p --no-undefined -X -g
OBJCOPYFLAGS := -O binary -R .note -R .comment -R .bss
GZFLAGS      := -9

# *** This lets the clean target work, but I'm not sure it's a good
# idea to include it.
-include .config
quote:="
# "
CONFIG_MACH:= $(subst $(quote),,$(CONFIG_MACH))

arch-$(CONFIG_CPU_ARMV4)   :=-D__LINUX_ARM_ARCH__=4 -march=armv4t
arch-$(CONFIG_CPU_ARMV5)   :=-D__LINUX_ARM_ARCH__=5 $(call cc-option,-march=armv5te,-march=armv4)
arch-$(CONFIG_CPU_ARMV6)   :=-D__LINUX_ARM_ARCH__=6 $(call cc-option,-march=armv6,-march=armv5t -Wa$(comma)-march=armv6)

tune-$(CONFIG_CPU_ARM720T) :=-mtune=arm7tdmi
tune-$(CONFIG_CPU_ARM920T) :=-mtune=arm9tdmi
tune-$(CONFIG_CPU_ARM922T) :=-mtune=arm9tdmi
tune-$(CONFIG_CPU_ARM925T) :=-mtune=arm9tdmi
tune-$(CONFIG_CPU_ARM926T) :=-mtune=arm9tdmi
tune-$(CONFIG_CPU_XSCALE)  :=$(call cc-option,-mtune=xscale,-mtune=strongarm110) -Wa,-mcpu=xscale
tune-$(CONFIG_CPU_ARMV6)   :=$(call cc-option,-mtune=arm1136j-s,-mtune=strongarm)

KBUILD_CFLAGS+= $(arch-y) $(tune-y) -Wall

ifneq ($(CONFIG_THUMB),)
 KBUILD_CFLAGS += -mthumb-interwork
 KBUILD_AFLAGS += -mthumb-interwork
endif

init-y += src/arch-arm/entry/
ifneq ($(CONFIG_MACH),)
core-y += src/mach-$(CONFIG_MACH)/
endif
core-y += src/drivers/ src/net/ src/arch-arm/linux/
core-$(CONFIG_DRIVERS_LH) +=  src/drivers-lh/
libs-y += src/arch-arm/lib/

ifneq ($(CONFIG_MACH),)
-include src/mach-$(CONFIG_MACH)/Makefile.libs
endif

rom := src/arch-arm/rom

apex.bin apex.bin_swap apex.elf apex.srec :\
		 include/config.h apex
	$(Q)$(MAKE) $(build)=$(rom) $(rom)/$@
