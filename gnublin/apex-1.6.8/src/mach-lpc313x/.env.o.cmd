cmd_src/mach-lpc313x/env.o := arm-linux-gnueabi-gcc -Wp,-MD,src/mach-lpc313x/.env.o.d  -nostdinc -isystem /opt/eldk-5.2.1/armv5te/sysroots/i686-eldk-linux/usr/lib/armv5te-linux-gnueabi/gcc/arm-linux-gnueabi/4.6.4/include -D__KERNEL__ -Iinclude  -include include/linux/autoconf.h -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -fno-builtin-printf -Os   -fno-stack-protector -D__LINUX_ARM_ARCH__=5 -march=armv5te -mtune=arm9tdmi -Wall -fomit-frame-pointer -g -Wdeclaration-after-statement -Wno-pointer-sign   -DBUILDDATE=\"2014.Mar.12-16:02:04\" -DAPEXVERSION=\"1.6.8-ep\"  -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(env)"  -D"KBUILD_MODNAME=KBUILD_STR(env)" -c -o src/mach-lpc313x/env.o src/mach-lpc313x/env.c

deps_src/mach-lpc313x/env.o := \
  src/mach-lpc313x/env.c \
    $(wildcard include/config/env/default/cmdline.h) \
    $(wildcard include/config/mach/val3153.h) \
    $(wildcard include/config/mach/ea313x/v2.h) \
    $(wildcard include/config/mach/ea3152.h) \
    $(wildcard include/config/env/link.h) \
    $(wildcard include/config/env/region.h) \
  include/config.h \
    $(wildcard include/config/h.h) \
  include/environment.h \
    $(wildcard include/config/env/size.h) \
  include/attributes.h \
  include/linux/compiler.h \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  include/linux/compiler-gcc4.h \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/arch/supports/optimized/inlining.h) \
    $(wildcard include/config/optimize/inlining.h) \
  include/driver.h \
    $(wildcard include/config/driver/long/long/size.h) \
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
  include/service.h \

src/mach-lpc313x/env.o: $(deps_src/mach-lpc313x/env.o)

$(deps_src/mach-lpc313x/env.o):
