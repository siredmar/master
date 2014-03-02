cmd_src/apex/command.o := arm-linux-gnueabi-gcc -Wp,-MD,src/apex/.command.o.d  -nostdinc -isystem /opt/eldk-5.2.1/armv5te/sysroots/i686-eldk-linux/usr/lib/armv5te-linux-gnueabi/gcc/arm-linux-gnueabi/4.6.4/include -D__KERNEL__ -Iinclude  -include include/linux/autoconf.h -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -fno-builtin-printf -Os   -fno-stack-protector -D__LINUX_ARM_ARCH__=5 -march=armv5te -mtune=arm9tdmi -Wall -fomit-frame-pointer -g -Wdeclaration-after-statement -Wno-pointer-sign     -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(command)"  -D"KBUILD_MODNAME=KBUILD_STR(command)" -c -o src/apex/command.o src/apex/command.c

deps_src/apex/command.o := \
  src/apex/command.c \
    $(wildcard include/config/time/commands.h) \
    $(wildcard include/config/aliases.h) \
    $(wildcard include/config/env.h) \
    $(wildcard include/config/small.h) \
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
  include/linux/string.h \
  include/asm/string.h \
  include/apex.h \
    $(wildcard include/config/command/history.h) \
  /opt/eldk-5.2.1/armv5te/sysroots/i686-eldk-linux/usr/lib/armv5te-linux-gnueabi/gcc/arm-linux-gnueabi/4.6.4/include/stdarg.h \
  include/config.h \
    $(wildcard include/config/h.h) \
  include/command.h \
    $(wildcard include/config/nohelp.h) \
    $(wildcard include/config/allhelp.h) \
    $(wildcard include/config/partial/matches.h) \
  include/config.h \
  include/attributes.h \
  include/strimatch.h \
  include/error.h \
  include/linux/ctype.h \
  include/environment.h \
    $(wildcard include/config/env/region.h) \
    $(wildcard include/config/env/size.h) \
  include/spinner.h \
    $(wildcard include/config/spinner.h) \
  include/lookup.h \
  include/talk.h \

src/apex/command.o: $(deps_src/apex/command.o)

$(deps_src/apex/command.o):
