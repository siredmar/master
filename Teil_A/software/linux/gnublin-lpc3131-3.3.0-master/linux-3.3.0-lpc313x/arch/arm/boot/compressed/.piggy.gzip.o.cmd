cmd_arch/arm/boot/compressed/piggy.gzip.o := arm-linux-gnueabi-gcc -Wp,-MD,arch/arm/boot/compressed/.piggy.gzip.o.d  -nostdinc -isystem /opt/eldk-5.2.1/armv5te/sysroots/i686-eldk-linux/usr/lib/armv5te-linux-gnueabi/gcc/arm-linux-gnueabi/4.6.4/include -I/home/armin/Masterarbeit/gnublin/gnublin-lpc3131-3.3.0-master/linux-3.3.0-lpc313x/arch/arm/include -Iarch/arm/include/generated -Iinclude  -include /home/armin/Masterarbeit/gnublin/gnublin-lpc3131-3.3.0-master/linux-3.3.0-lpc313x/include/linux/kconfig.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-lpc31xx/include -D__ASSEMBLY__ -mabi=aapcs-linux -mno-thumb-interwork -funwind-tables  -D__LINUX_ARM_ARCH__=5 -march=armv5te -mtune=arm9tdmi -include asm/unified.h -msoft-float    -Wa,-march=all     -c -o arch/arm/boot/compressed/piggy.gzip.o arch/arm/boot/compressed/piggy.gzip.S

source_arch/arm/boot/compressed/piggy.gzip.o := arch/arm/boot/compressed/piggy.gzip.S

deps_arch/arm/boot/compressed/piggy.gzip.o := \
  /home/armin/Masterarbeit/gnublin/gnublin-lpc3131-3.3.0-master/linux-3.3.0-lpc313x/arch/arm/include/asm/unified.h \
    $(wildcard include/config/arm/asm/unified.h) \
    $(wildcard include/config/thumb2/kernel.h) \

arch/arm/boot/compressed/piggy.gzip.o: $(deps_arch/arm/boot/compressed/piggy.gzip.o)

$(deps_arch/arm/boot/compressed/piggy.gzip.o):
