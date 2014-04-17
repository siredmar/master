#
# Makefile for the linux kernel.
#

# Object file lists.

obj-y	+= irq.o time.o generic.o i2c.o gpio.o dma.o usb.o wdt.o clock.o


# Specific board support
obj-$(CONFIG_MACH_VAL3153) += board-val3153.o leds.o
obj-$(CONFIG_MACH_EA313X) += board-ea313x.o
obj-$(CONFIG_MACH_GNUBLIN) += board-gnublin.o
obj-$(CONFIG_MACH_EA3152) += board-ea313x.o
obj-$(CONFIG_MACH_VAL3154) += board-val3154.o leds.o
obj-$(CONFIG_LPC3152_AD) += psu.o

# Power Management
obj-$(CONFIG_PM)		+= pm.o pm_standby.o

ifeq ($(CONFIG_PM_DEBUG),y)
CFLAGS_pm.o += -DDEBUG
endif
