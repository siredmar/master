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

obj-y				:= reset.o mmu-alloc.o
obj-$(CONFIG_SDRAMBOOT_REPORT)	+= sdramboot.o
obj-$(CONFIG_STACKLIMIT)	+= stacklimit.o
obj-$(CONFIG_MMU)		+= mmu.o
obj-$(CONFIG_RELOCATE_SIMPLE)	+= relocate-simple.o
#lib-y += relocate.o

CPPFLAGS_apex.lds +=\
 -DAPEX_VMA=$(CONFIG_APEX_VMA)\
 -DSTACK_SIZE=$(CONFIG_STACK_SIZE)

ifeq ('$(CONFIG_MMU)','y')
CPPFLAGS_apex.lds += -DCONFIG_MMU
endif
ifeq ('$(CONFIG_INTERRUPTS)','y')
CPPFLAGS_apex.lds += -DCONFIG_INTERRUPTS
endif
ifeq ('$(CONFIG_RELOCATE_COMPANION_EVT1)','y')
CPPFLAGS_apex.lds += -DCONFIG_RELOCATE_COMPANION_EVT1
endif

extra-y=apex.lds

ifneq ($(CONFIG_THUMB),)
 CFLAGS_sdramboot.o	+= -mthumb
endif
