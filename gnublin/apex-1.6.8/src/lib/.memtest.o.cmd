cmd_src/lib/memtest.o := arm-linux-gnueabi-gcc -Wp,-MD,src/lib/.memtest.o.d  -nostdinc -isystem /opt/eldk-5.2.1/armv5te/sysroots/i686-eldk-linux/usr/lib/armv5te-linux-gnueabi/gcc/arm-linux-gnueabi/4.6.4/include -D__KERNEL__ -Iinclude  -include include/linux/autoconf.h -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -fno-builtin-printf -Os   -fno-stack-protector -D__LINUX_ARM_ARCH__=5 -march=armv5te -mtune=arm9tdmi -Wall -fomit-frame-pointer -g -Wdeclaration-after-statement -Wno-pointer-sign     -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(memtest)"  -D"KBUILD_MODNAME=KBUILD_STR(memtest)" -c -o src/lib/memtest.o src/lib/memtest.c

deps_src/lib/memtest.o := \
  src/lib/memtest.c \
    $(wildcard include/config/bootstrap/memtest/full.h) \
  include/config.h \
    $(wildcard include/config/h.h) \
  include/attributes.h \
  include/linux/compiler.h \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  include/linux/compiler-gcc4.h \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/arch/supports/optimized/inlining.h) \
    $(wildcard include/config/optimize/inlining.h) \
  include/memtest.h \
  include/debug_ll.h \
    $(wildcard include/config/startup/uart.h) \
    $(wildcard include/config/debug/ll.h) \

src/lib/memtest.o: $(deps_src/lib/memtest.o)

$(deps_src/lib/memtest.o):
