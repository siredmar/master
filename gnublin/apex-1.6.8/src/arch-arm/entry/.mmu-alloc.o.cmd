cmd_src/arch-arm/entry/mmu-alloc.o := arm-linux-gnueabi-gcc -Wp,-MD,src/arch-arm/entry/.mmu-alloc.o.d  -nostdinc -isystem /opt/eldk-5.2.1/armv5te/sysroots/i686-eldk-linux/usr/lib/armv5te-linux-gnueabi/gcc/arm-linux-gnueabi/4.6.4/include -D__KERNEL__ -Iinclude  -include include/linux/autoconf.h -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -fno-builtin-printf -Os   -fno-stack-protector -D__LINUX_ARM_ARCH__=5 -march=armv5te -mtune=arm9tdmi -Wall -fomit-frame-pointer -g -Wdeclaration-after-statement -Wno-pointer-sign     -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(mmu_alloc)"  -D"KBUILD_MODNAME=KBUILD_STR(mmu_alloc)" -c -o src/arch-arm/entry/mmu-alloc.o src/arch-arm/entry/mmu-alloc.c

deps_src/arch-arm/entry/mmu-alloc.o := \
  src/arch-arm/entry/mmu-alloc.c \
    $(wildcard include/config/mmu.h) \
  include/config.h \
    $(wildcard include/config/h.h) \
  include/debug_ll.h \
    $(wildcard include/config/startup/uart.h) \
    $(wildcard include/config/debug/ll.h) \
  include/attributes.h \
  include/linux/compiler.h \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  include/linux/compiler-gcc4.h \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/arch/supports/optimized/inlining.h) \
    $(wildcard include/config/optimize/inlining.h) \
  include/linux/string.h \
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
  include/asm/string.h \
  include/service.h \
  include/apex.h \
    $(wildcard include/config/small.h) \
    $(wildcard include/config/command/history.h) \
  /opt/eldk-5.2.1/armv5te/sysroots/i686-eldk-linux/usr/lib/armv5te-linux-gnueabi/gcc/arm-linux-gnueabi/4.6.4/include/stdarg.h \
  include/asm/cp15.h \
    $(wildcard include/config/cpu/armv4.h) \
    $(wildcard include/config/cpu/xscale.h) \
    $(wildcard include/config/cpu/armv5te.h) \
    $(wildcard include/config/cpu/armv5.h) \
    $(wildcard include/config/cpu/armv6.h) \
  include/asm/cp15-armv5.h \
  include/asm/mmu.h \
  include/drv-mem.h \

src/arch-arm/entry/mmu-alloc.o: $(deps_src/arch-arm/entry/mmu-alloc.o)

$(deps_src/arch-arm/entry/mmu-alloc.o):