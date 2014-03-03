# Makefile
#
#   Copyright (C) 2008 Marc Singer
#
#   This program is free software; you can redistribute it and/or
#   modify it under the terms of the GNU General Public License
#   version 2 as published by the Free Software Foundation.
#   Please refer to the file debian/copyright for further details.
#

PACKAGE:=arm-kernel-shim
VERSION:=1.5

CROSS_COMPILE:=/usr/arm-linux/gcc3/arm-linux-
CC=$(CROSS_COMPILE)gcc
LD=$(CROSS_COMPILE)ld
OBJCOPY=$(CROSS_COMPILE)objcopy

CFLAGS:=-g -Os -nostartfiles -nostdlib
LDFLAGS:= -g -Ttext 0 -X -Map shim.map

SRCS:= shim.lds main.c
OBJS:=$(SRCS:.c=.o)

all: $(PACKAGE).bin $(PACKAGE).bin-swapped

$(PACKAGE).bin: $(PACKAGE)
	$(OBJCOPY) -O binary -R .bss -R .comment $< $@

$(PACKAGE).bin-swapped: $(PACKAGE).bin
	cat $< | ./wordswap > $@

$(PACKAGE): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

%.s: %.c
	$(CC) -S $(CFLAGS) $< -o $@


.PHONY: clean
clean:
	-rm *.o $(PACKAGE) $(PACKAGE).bin *~ *.map

.PHONY: tag
tag:
	svn cp file:///svn/tools/trunk/apex/$(PACKAGE)\
	       file:///svn/tools/tags/$(PACKAGE)/$(VERSION)

.PHONY: remove-tag
remove-tag:
	svn rm file:///svn/tools/tags/$(PACKAGE)/$(VERSION)

.PHONY: package
package: $(PACKAGE)-$(VERSION).tar.gz

$(PACKAGE)-$(VERSION).tar.gz:
	[ ! -d $(PACKAGE)-$(VERSION) ] || rm -rf $(PACKAGE)-$(VERSION)
	svn export -q file:///svn/tools/tags/$(PACKAGE)/$(VERSION)\
	  $(PACKAGE)-$(VERSION)
	tar zcvf $(PACKAGE)-$(VERSION).tar.gz $(PACKAGE)-$(VERSION)
	rm -rf $(PACKAGE)-$(VERSION)
