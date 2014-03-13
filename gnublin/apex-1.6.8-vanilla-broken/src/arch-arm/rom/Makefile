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

targets += apex.bin apex.srec

$(obj)/apex.bin: apex FORCE
	@$(call if_changed,objcopy)
	@[ -L apex.bin ] || ln -s $@ apex.bin

$(obj)/apex.bin_swap: $(obj)/apex.bin FORCE
	@echo "  SWAP    $@"
	@cat $< | perl -e 'while (sysread(STDIN,$$d,4)){print pack("N",unpack("V",$$d));}' > $@
	@[ -L apex.bin_swap ] || ln -s $@ apex.bin_swap

# *** FIXME: there may need to be an adjustment to the start address
OBJCOPYFLAGS_apex.srec := -O srec
$(obj)/apex.srec: apex FORCE
	@$(call if_changed,objcopy)
	@[ -L apex.srec ] || ln -s $@ apex.srec

$(obj)/apex.elf: apex FORCE
	@$(STRIP) apex -o $@
	@[ -L apex.elf ] || ln -s $@ apex.elf
