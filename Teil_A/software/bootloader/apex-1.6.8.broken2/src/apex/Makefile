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

obj-y := init.o services.o console.o console-printf.o
obj-y += command.o cmd-version.o cmd-help.o cmd-boot.o
obj-y += region-copy.o region-checksum.o

obj-$(CONFIG_COMMAND_HISTORY)   += command-history.o

obj-$(CONFIG_ENV)		+= env.o

obj-$(CONFIG_CMD_ALIAS)		+= cmd-alias.o
obj-$(CONFIG_CMD_CHECKSUM)	+= cmd-checksum.o
obj-$(CONFIG_CMD_COMPARE)	+= cmd-compare.o
obj-$(CONFIG_CMD_COPY)		+= cmd-copy.o
obj-$(CONFIG_CMD_DRVINFO)	+= cmd-drvinfo.o
obj-$(CONFIG_CMD_DUMP)		+= cmd-dump.o
obj-$(CONFIG_CMD_ENV)		+= cmd-env.o
obj-$(CONFIG_CMD_ERASE)		+= cmd-erase.o
obj-$(CONFIG_CMD_FILL)		+= cmd-fill.o
obj-$(CONFIG_CMD_GO)		+= cmd-go.o
obj-$(CONFIG_CMD_INFO)		+= cmd-info.o
obj-$(CONFIG_CMD_MEMTEST)	+= cmd-memtest.o
obj-$(CONFIG_CMD_PAUSE)		+= cmd-pause.o
obj-$(CONFIG_CMD_PARSE)		+= cmd-parse.o
obj-$(CONFIG_CMD_WAIT)		+= cmd-wait.o
obj-$(CONFIG_CMD_XRECEIVE)	+= cmd-xreceive.o
obj-$(CONFIG_CMD_IMAGE)		+= cmd-image.o
obj-$(CONFIG_CMD_IMAGE_APEX)	+= cmd-image-apex.o
obj-$(CONFIG_CMD_IMAGE_UBOOT)	+= cmd-image-uboot.o
obj-$(CONFIG_CMD_FLASHUSAGE)	+= cmd-flashusage.o

ifneq ($(CONFIG_THUMB),)
 EXTRA_CFLAGS += -mthumb
endif

CFLAGS_cmd-version.o += -DAPEXVERSION=\"$(APEXVERSION)\"
CFLAGS_env.o	     += -DAPEXVERSION=\"$(APEXVERSION)\"

ifdef BUILDDATE
CFLAGS_cmd-version.o += -DBUILDDATE=\"$(BUILDDATE)\"
CFLAGS_env.o	     += -DBUILDDATE=\"$(BUILDDATE)\"
endif
