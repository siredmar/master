TARGET = ssd1963
ARCH   ?= arm
CROSS_COMPILE ?= arm-linux-gnueabi-
export ARCH CROSS_COMPILE

obj-m += $(TARGET).o

KDIR = ../linux-2.6.33-lpc313x-fb/

KBUILD_TARGETS	:= clean help modules modules_install

.PHONY: all $(KBUILD_TARGETS)
all: modules

$(KBUILD_TARGETS):
	$(MAKE) -C $(KDIR) M=$(shell pwd) $@

copy:
	scp ssd1963.ko root@gnublin:/lib/modules/2.6.33/kernel/drivers/video
