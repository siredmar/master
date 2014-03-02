cmd_arch/arm/lib/io-writesw-armv4.o := arm-linux-gnueabi-gcc -Wp,-MD,arch/arm/lib/.io-writesw-armv4.o.d  -nostdinc -isystem /opt/eldk-5.2.1/armv5te/sysroots/i686-eldk-linux/usr/lib/armv5te-linux-gnueabi/gcc/arm-linux-gnueabi/4.6.4/include -I/home/armin/Masterarbeit/gnublin/linux-2.6.33-lpc313x-fb/arch/arm/include -Iinclude  -include include/generated/autoconf.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-lpc313x/include -D__ASSEMBLY__ -mabi=aapcs-linux -mno-thumb-interwork -funwind-tables  -D__LINUX_ARM_ARCH__=5 -march=armv5te -mtune=arm9tdmi -include asm/unified.h -msoft-float       -c -o arch/arm/lib/io-writesw-armv4.o arch/arm/lib/io-writesw-armv4.S

deps_arch/arm/lib/io-writesw-armv4.o := \
  arch/arm/lib/io-writesw-armv4.S \
  /home/armin/Masterarbeit/gnublin/linux-2.6.33-lpc313x-fb/arch/arm/include/asm/unified.h \
    $(wildcard include/config/arm/asm/unified.h) \
    $(wildcard include/config/thumb2/kernel.h) \
  include/linux/linkage.h \
  include/linux/compiler.h \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  /home/armin/Masterarbeit/gnublin/linux-2.6.33-lpc313x-fb/arch/arm/include/asm/linkage.h \
  /home/armin/Masterarbeit/gnublin/linux-2.6.33-lpc313x-fb/arch/arm/include/asm/assembler.h \
    $(wildcard include/config/cpu/feroceon.h) \
    $(wildcard include/config/trace/irqflags.h) \
    $(wildcard include/config/smp.h) \
  /home/armin/Masterarbeit/gnublin/linux-2.6.33-lpc313x-fb/arch/arm/include/asm/ptrace.h \
    $(wildcard include/config/cpu/endian/be8.h) \
    $(wildcard include/config/arm/thumb.h) \
  /home/armin/Masterarbeit/gnublin/linux-2.6.33-lpc313x-fb/arch/arm/include/asm/hwcap.h \

arch/arm/lib/io-writesw-armv4.o: $(deps_arch/arm/lib/io-writesw-armv4.o)

$(deps_arch/arm/lib/io-writesw-armv4.o):
