cmd_src/arch-arm/entry/reset.o := arm-linux-gnueabi-gcc -Wp,-MD,src/arch-arm/entry/.reset.o.d  -nostdinc -isystem /opt/eldk-5.2.1/armv5te/sysroots/i686-eldk-linux/usr/lib/armv5te-linux-gnueabi/gcc/arm-linux-gnueabi/4.6.4/include -D__KERNEL__ -Iinclude  -include include/linux/autoconf.h -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -fno-builtin-printf -Os   -fno-stack-protector -D__LINUX_ARM_ARCH__=5 -march=armv5te -mtune=arm9tdmi -Wall -fomit-frame-pointer -g -Wdeclaration-after-statement -Wno-pointer-sign     -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(reset)"  -D"KBUILD_MODNAME=KBUILD_STR(reset)" -c -o src/arch-arm/entry/reset.o src/arch-arm/entry/reset.c

deps_src/arch-arm/entry/reset.o := \
  src/arch-arm/entry/reset.c \
    $(wildcard include/config/env/link.h) \
    $(wildcard include/config/arch/lpc313x.h) \
    $(wildcard include/config/bigendian.h) \
    $(wildcard include/config/littleendian.h) \
    $(wildcard include/config/disable/mmu/at/boot.h) \
    $(wildcard include/config/alignment/trap.h) \
    $(wildcard include/config/inline/platform/initialization.h) \
    $(wildcard include/config/inline/platform/initialization/sdram/pre/override.h) \
    $(wildcard include/config/sdramboot/report.h) \
    $(wildcard include/config/bootstrap/memtest.h) \
    $(wildcard include/config/bootstrap/memtest/base.h) \
    $(wildcard include/config/bootstrap/memtest/size.h) \
    $(wildcard include/config/mach/ixp42x.h) \
    $(wildcard include/config/clear/stacks.h) \
  include/config.h \
    $(wildcard include/config/h.h) \
  include/asm/bootstrap.h \
  include/attributes.h \
  include/linux/compiler.h \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  include/linux/compiler-gcc4.h \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/arch/supports/optimized/inlining.h) \
    $(wildcard include/config/optimize/inlining.h) \
  include/arch-arm.h \
  include/memtest.h \
  include/asm/cp15.h \
    $(wildcard include/config/cpu/armv4.h) \
    $(wildcard include/config/cpu/xscale.h) \
    $(wildcard include/config/cpu/armv5te.h) \
    $(wildcard include/config/cpu/armv5.h) \
    $(wildcard include/config/cpu/armv6.h) \
  include/linux/types.h \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/lbd.h) \
    $(wildcard include/config/lsf.h) \
    $(wildcard include/config/resources/64bit.h) \
  include/linux/posix_types.h \
  include/linux/stddef.h \
  include/asm/posix_types.h \
  include/asm/types.h \
  include/asm-generic/int-ll64.h \
  include/asm/cp15-armv5.h \
  include/mach/memory.h \
  include/mach/hardware.h \
  include/mach/lpc313x.h \
    $(wildcard include/config/reg.h) \
    $(wildcard include/config/osc/freq.h) \
  include/apex.h \
    $(wildcard include/config/small.h) \
    $(wildcard include/config/command/history.h) \
  /opt/eldk-5.2.1/armv5te/sysroots/i686-eldk-linux/usr/lib/armv5te-linux-gnueabi/gcc/arm-linux-gnueabi/4.6.4/include/stdarg.h \
  include/asm/reg.h \
  include/mach/lpc313x_cgu.h \
    $(wildcard include/config/regs.h) \
    $(wildcard include/config/soft/reset.h) \
    $(wildcard include/config/has/analog/die.h) \
  include/mach/event_router.h \
  include/sdramboot.h \
  include/debug_ll.h \
    $(wildcard include/config/startup/uart.h) \
    $(wildcard include/config/debug/ll.h) \

src/arch-arm/entry/reset.o: $(deps_src/arch-arm/entry/reset.o)

$(deps_src/arch-arm/entry/reset.o):
