cmd_src/lib/crc32.o := arm-linux-gnueabi-gcc -Wp,-MD,src/lib/.crc32.o.d  -nostdinc -isystem /opt/eldk-5.2.1/armv5te/sysroots/i686-eldk-linux/usr/lib/armv5te-linux-gnueabi/gcc/arm-linux-gnueabi/4.6.4/include -D__KERNEL__ -Iinclude  -include include/linux/autoconf.h -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -fno-builtin-printf -Os   -fno-stack-protector -D__LINUX_ARM_ARCH__=5 -march=armv5te -mtune=arm9tdmi -Wall -fomit-frame-pointer -g -Wdeclaration-after-statement -Wno-pointer-sign     -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(crc32)"  -D"KBUILD_MODNAME=KBUILD_STR(crc32)" -c -o src/lib/crc32.o src/lib/crc32.c

deps_src/lib/crc32.o := \
  src/lib/crc32.c \
    $(wildcard include/config/small.h) \

src/lib/crc32.o: $(deps_src/lib/crc32.o)

$(deps_src/lib/crc32.o):
