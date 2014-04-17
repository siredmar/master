TARGET = testmod
ARCH   ?= arm
CROSS_COMPILE ?= arm-linux-gnueabi-
export ARCH CROSS_COMPILE

obj-m += $(TARGET).o waitchange.o testmod_ssd1963.o
#KDIR=/home/armin/Masterarbeit/gnublin/gnublin-lpc3131-3.3.0-master/linux-3.3.0-lpc313x
KDIR = /home/armin/Masterarbeit/gnublin/linux-2.6.33-lpc313x-fb


KBUILD_TARGETS	:= clean help modules modules_install

.PHONY: all $(KBUILD_TARGETS)
all: modules

$(KBUILD_TARGETS):
	$(MAKE) -C $(KDIR) M=$(shell pwd) $@
