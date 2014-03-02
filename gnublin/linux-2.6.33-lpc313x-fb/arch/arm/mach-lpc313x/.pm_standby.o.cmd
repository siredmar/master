cmd_arch/arm/mach-lpc313x/pm_standby.o := arm-linux-gnueabi-gcc -Wp,-MD,arch/arm/mach-lpc313x/.pm_standby.o.d  -nostdinc -isystem /opt/eldk-5.2.1/armv5te/sysroots/i686-eldk-linux/usr/lib/armv5te-linux-gnueabi/gcc/arm-linux-gnueabi/4.6.4/include -I/home/armin/Masterarbeit/gnublin/linux-2.6.33-lpc313x-fb/arch/arm/include -Iinclude  -include include/generated/autoconf.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-lpc313x/include -D__ASSEMBLY__ -mabi=aapcs-linux -mno-thumb-interwork -funwind-tables  -D__LINUX_ARM_ARCH__=5 -march=armv5te -mtune=arm9tdmi -include asm/unified.h -msoft-float       -c -o arch/arm/mach-lpc313x/pm_standby.o arch/arm/mach-lpc313x/pm_standby.S

deps_arch/arm/mach-lpc313x/pm_standby.o := \
  arch/arm/mach-lpc313x/pm_standby.S \
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
  arch/arm/mach-lpc313x/include/mach/hardware.h \
  /home/armin/Masterarbeit/gnublin/linux-2.6.33-lpc313x-fb/arch/arm/include/asm/sizes.h \
  arch/arm/mach-lpc313x/include/mach/constants.h \
  arch/arm/mach-lpc313x/include/mach/registers.h \
    $(wildcard include/config/reg.h) \
    $(wildcard include/config/pec.h) \
    $(wildcard include/config/ecgc.h) \
    $(wildcard include/config/8bit/ecc.h) \
    $(wildcard include/config/tl/528.h) \
    $(wildcard include/config/tl/516.h) \
    $(wildcard include/config/tl/512.h) \
    $(wildcard include/config/tl/mask.h) \
    $(wildcard include/config/eo.h) \
    $(wildcard include/config/dc.h) \
    $(wildcard include/config/m.h) \
    $(wildcard include/config/lc/0.h) \
    $(wildcard include/config/lc/1.h) \
    $(wildcard include/config/lc/2.h) \
    $(wildcard include/config/lc/mask.h) \
    $(wildcard include/config/es.h) \
    $(wildcard include/config/de.h) \
    $(wildcard include/config/ao.h) \
    $(wildcard include/config/wd.h) \
    $(wildcard include/config/ec.h) \
    $(wildcard include/config/cfg/io/drive.h) \
    $(wildcard include/config/cfg/data/def.h) \
    $(wildcard include/config/cfg/cle/1.h) \
    $(wildcard include/config/cfg/ale/1.h) \
    $(wildcard include/config/cfg/we/1.h) \
    $(wildcard include/config/cfg/re/1.h) \

arch/arm/mach-lpc313x/pm_standby.o: $(deps_arch/arm/mach-lpc313x/pm_standby.o)

$(deps_arch/arm/mach-lpc313x/pm_standby.o):
