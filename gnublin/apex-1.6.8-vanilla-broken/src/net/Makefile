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

obj-$(CONFIG_ETHERNET)		+= ethernet.o
obj-$(CONFIG_CMD_IPCONFIG)	+= ipconfig.o
obj-$(CONFIG_CMD_PING)		+= ping.o
obj-$(CONFIG_CMD_ARP)		+= arp.o
obj-$(CONFIG_CMD_TFTP)		+= tftp.o

ifneq ($(CONFIG_THUMB),)
 EXTRA_CFLAGS += -mthumb
endif

