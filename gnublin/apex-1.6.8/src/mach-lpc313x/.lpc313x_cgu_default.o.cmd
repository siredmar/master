cmd_src/mach-lpc313x/lpc313x_cgu_default.o := arm-linux-gnueabi-gcc -Wp,-MD,src/mach-lpc313x/.lpc313x_cgu_default.o.d  -nostdinc -isystem /opt/eldk-5.2.1/armv5te/sysroots/i686-eldk-linux/usr/lib/armv5te-linux-gnueabi/gcc/arm-linux-gnueabi/4.6.4/include -D__KERNEL__ -Iinclude  -include include/linux/autoconf.h -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -fno-builtin-printf -Os   -fno-stack-protector -D__LINUX_ARM_ARCH__=5 -march=armv5te -mtune=arm9tdmi -Wall -fomit-frame-pointer -g -Wdeclaration-after-statement -Wno-pointer-sign     -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(lpc313x_cgu_default)"  -D"KBUILD_MODNAME=KBUILD_STR(lpc313x_cgu_default)" -c -o src/mach-lpc313x/lpc313x_cgu_default.o src/mach-lpc313x/lpc313x_cgu_default.c

deps_src/mach-lpc313x/lpc313x_cgu_default.o := \
  src/mach-lpc313x/lpc313x_cgu_default.c \
    $(wildcard include/config/pll/270.h) \
    $(wildcard include/config/pll/180.h) \
  include/apex.h \
    $(wildcard include/config/small.h) \
    $(wildcard include/config/command/history.h) \
  /opt/eldk-5.2.1/armv5te/sysroots/i686-eldk-linux/usr/lib/armv5te-linux-gnueabi/gcc/arm-linux-gnueabi/4.6.4/include/stdarg.h \
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
  include/config.h \
    $(wildcard include/config/h.h) \
  src/mach-lpc313x/lpc313x.h \
    $(wildcard include/config/reg.h) \
    $(wildcard include/config/osc/freq.h) \
  include/asm/reg.h \
  src/mach-lpc313x/lpc313x_cgu.h \
    $(wildcard include/config/regs.h) \
    $(wildcard include/config/soft/reset.h) \
    $(wildcard include/config/has/analog/die.h) \
  src/mach-lpc313x/event_router.h \

src/mach-lpc313x/lpc313x_cgu_default.o: $(deps_src/mach-lpc313x/lpc313x_cgu_default.o)

$(deps_src/mach-lpc313x/lpc313x_cgu_default.o):
