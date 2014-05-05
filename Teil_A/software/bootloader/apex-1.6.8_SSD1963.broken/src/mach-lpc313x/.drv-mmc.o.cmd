cmd_src/mach-lpc313x/drv-mmc.o := arm-linux-gnueabi-gcc -Wp,-MD,src/mach-lpc313x/.drv-mmc.o.d  -nostdinc -isystem /opt/eldk-5.2.1/armv5te/sysroots/i686-eldk-linux/usr/lib/armv5te-linux-gnueabi/gcc/arm-linux-gnueabi/4.6.4/include -D__KERNEL__ -Iinclude  -include include/linux/autoconf.h -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -fno-builtin-printf -Os   -fno-stack-protector -D__LINUX_ARM_ARCH__=5 -march=armv5te -mtune=arm9tdmi -Wall -fomit-frame-pointer -g -Wdeclaration-after-statement -Wno-pointer-sign     -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(drv_mmc)"  -D"KBUILD_MODNAME=KBUILD_STR(drv_mmc)" -c -o src/mach-lpc313x/drv-mmc.o src/mach-lpc313x/drv-mmc.c

deps_src/mach-lpc313x/drv-mmc.o := \
  src/mach-lpc313x/drv-mmc.c \
    $(wildcard include/config/mach/val313x.h) \
    $(wildcard include/config/mach/ea313x/v1.h) \
    $(wildcard include/config/mach/ea313x/v2.h) \
    $(wildcard include/config/mach/val3154.h) \
    $(wildcard include/config/mach/eplpc3131/v1.h) \
    $(wildcard include/config/soft/reset.h) \
    $(wildcard include/config/small.h) \
  include/config.h \
    $(wildcard include/config/h.h) \
  include/driver.h \
    $(wildcard include/config/driver/long/long/size.h) \
  include/linux/types.h \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/lbd.h) \
    $(wildcard include/config/lsf.h) \
    $(wildcard include/config/resources/64bit.h) \
  include/linux/posix_types.h \
  include/linux/stddef.h \
  include/linux/compiler.h \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  include/linux/compiler-gcc4.h \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/arch/supports/optimized/inlining.h) \
    $(wildcard include/config/optimize/inlining.h) \
  include/asm/posix_types.h \
  include/asm/types.h \
  include/asm-generic/int-ll64.h \
  include/attributes.h \
  include/service.h \
  include/linux/string.h \
  include/asm/string.h \
  include/apex.h \
    $(wildcard include/config/command/history.h) \
  /opt/eldk-5.2.1/armv5te/sysroots/i686-eldk-linux/usr/lib/armv5te-linux-gnueabi/gcc/arm-linux-gnueabi/4.6.4/include/stdarg.h \
  include/command.h \
    $(wildcard include/config/nohelp.h) \
    $(wildcard include/config/allhelp.h) \
    $(wildcard include/config/partial/matches.h) \
  include/config.h \
  include/strimatch.h \
  include/mach/hardware.h \
  include/mach/lpc313x.h \
    $(wildcard include/config/reg.h) \
    $(wildcard include/config/osc/freq.h) \
  include/asm/reg.h \
  include/mach/lpc313x_cgu.h \
    $(wildcard include/config/regs.h) \
    $(wildcard include/config/has/analog/die.h) \
  include/mach/event_router.h \
  include/console.h \
  include/error.h \
  include/mmc.h \
    $(wildcard include/config/driver/lh/mmc.h) \
  include/driver.h \

src/mach-lpc313x/drv-mmc.o: $(deps_src/mach-lpc313x/drv-mmc.o)

$(deps_src/mach-lpc313x/drv-mmc.o):
