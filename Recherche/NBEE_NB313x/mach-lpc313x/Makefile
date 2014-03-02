#
# Makefile for the linux kernel.
#

# Object file lists.

obj-y			+= irq.o time.o cgu.o generic.o i2c.o gpio.o dma.o usb.o gpiolib.o


# Specific board support
obj-$(CONFIG_MACH_FHS3143) += fhs3143.o leds-pca9532.o fhsbattery.o adc.o
obj-$(CONFIG_MACH_VAL3153) += val3153.o leds.o
obj-$(CONFIG_MACH_EA313X) += ea313x.o leds-pca9532.o
obj-$(CONFIG_MACH_NB31) += nb31xx.o adc.o
obj-$(CONFIG_MACH_VAL3154) += val3154.o leds.o
obj-$(CONFIG_LPC3152_AD) += psu.o
