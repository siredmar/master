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

lib-y := vsprintf.o ctype.o strtol.o
lib-y += strlen.o strnlen.o strchr.o strlcpy.o strcat.o strcpy.o
lib-y += strcmp.o strnicmp.o strcspn.o
lib-y += memcmp.o memset.o
lib-y += memcpy.o
#lib-$(CONFIG_SMALL) += memcpy.o
lib-$(CONFIG_SMALL) += memmove.o
#lib-$(CONFIG_THUMB) += memcpy.o
lib-y += crc32.o xmodem.o
lib-$(CONFIG_CRC32_LSB) += crc32-lsb.o
lib-y += spinner.o
lib-$(CONFIG_ENV) += env.o
lib-y += dump.o
lib-y += zlib.o zlib-heap.o
#lib-y += png.o
lib-y += pngr.o
lib-y += sort.o
lib-$(CONFIG_ALIASES) += alias.o
lib-y += lookup.o
lib-y += memtest.o
lib-y += strimatch.o
lib-y += gmtime.o
lib-y += describe-size.o

ifneq ($(CONFIG_THUMB),)
  EXTRA_CFLAGS += -mthumb
endif

#lib-y += div64.o
#lib-y += udiv.o
