cmd_src/lib/ctype.o := arm-linux-gnueabi-gcc -Wp,-MD,src/lib/.ctype.o.d  -nostdinc -isystem /opt/eldk-5.2.1/armv5te/sysroots/i686-eldk-linux/usr/lib/armv5te-linux-gnueabi/gcc/arm-linux-gnueabi/4.6.4/include -D__KERNEL__ -Iinclude  -include include/linux/autoconf.h -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -fno-builtin-printf -Os   -fno-stack-protector -D__LINUX_ARM_ARCH__=5 -march=armv5te -mtune=arm9tdmi -Wall -fomit-frame-pointer -g -Wdeclaration-after-statement -Wno-pointer-sign     -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(ctype)"  -D"KBUILD_MODNAME=KBUILD_STR(ctype)" -c -o src/lib/ctype.o src/lib/ctype.c

deps_src/lib/ctype.o := \
  src/lib/ctype.c \
  include/linux/ctype.h \
  include/linux/module.h \

src/lib/ctype.o: $(deps_src/lib/ctype.o)

$(deps_src/lib/ctype.o):
