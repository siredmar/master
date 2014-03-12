cmd_src/apex/env.o := arm-linux-gnueabi-gcc -Wp,-MD,src/apex/.env.o.d  -nostdinc -isystem /opt/eldk-5.2.1/armv5te/sysroots/i686-eldk-linux/usr/lib/armv5te-linux-gnueabi/gcc/arm-linux-gnueabi/4.6.4/include -D__KERNEL__ -Iinclude  -include include/linux/autoconf.h -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -fno-builtin-printf -Os   -fno-stack-protector -D__LINUX_ARM_ARCH__=5 -march=armv5te -mtune=arm9tdmi -Wall -fomit-frame-pointer -g -Wdeclaration-after-statement -Wno-pointer-sign   -DAPEXVERSION=\"1.6.8-ep\" -DBUILDDATE=\"2014.Mar.12-16:02:04\"  -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(env)"  -D"KBUILD_MODNAME=KBUILD_STR(env)" -c -o src/apex/env.o src/apex/env.c

deps_src/apex/env.o := \
  src/apex/env.c \
    $(wildcard include/config/arch/number.h) \
    $(wildcard include/config/kernel/lma.h) \
    $(wildcard include/config/ramdisk/lma.h) \
    $(wildcard include/config/ramdisk/size.h) \
    $(wildcard include/config/env/region/kernel.h) \
    $(wildcard include/config/env/region/kernel/alt.h) \
    $(wildcard include/config/variation/suffix.h) \
    $(wildcard include/config/env/region/ramdisk.h) \
    $(wildcard include/config/env/region/ramdisk/alt.h) \
    $(wildcard include/config/env/default/startup/alt.h) \
    $(wildcard include/config/env/default/startup/override.h) \
    $(wildcard include/config/env/default/startup.h) \
    $(wildcard include/config/env/startup/prefix.h) \
    $(wildcard include/config/env/startup/kernel/copy.h) \
    $(wildcard include/config/env/startup/ramdisk/copy.h) \
    $(wildcard include/config/autoboot.h) \
    $(wildcard include/config/env/region/kernel/swap.h) \
    $(wildcard include/config/env/region/ramdisk/swap.h) \
    $(wildcard include/config/autoboot/delay.h) \
    $(wildcard include/config/env/link.h) \
    $(wildcard include/config/env/region.h) \
    $(wildcard include/config/env/saveatonce.h) \
    $(wildcard include/config/env/size.h) \
  include/config.h \
    $(wildcard include/config/h.h) \
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
  include/environment.h \
  include/attributes.h \
  include/driver.h \
    $(wildcard include/config/driver/long/long/size.h) \
  include/service.h \
  include/linux/string.h \
  include/asm/string.h \

src/apex/env.o: $(deps_src/apex/env.o)

$(deps_src/apex/env.o):
