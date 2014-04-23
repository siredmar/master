cmd_drivers/net/wireless/rtl818x/rtl8187_leds.o := arm-linux-gnueabi-gcc -Wp,-MD,drivers/net/wireless/rtl818x/.rtl8187_leds.o.d  -nostdinc -isystem /opt/eldk-5.2.1/armv5te/sysroots/i686-eldk-linux/usr/lib/armv5te-linux-gnueabi/gcc/arm-linux-gnueabi/4.6.4/include -I/home/armin/Masterarbeit/Teil_A/software/linux/linux-2.6.33-lpc313x-fb/arch/arm/include -Iinclude  -include include/generated/autoconf.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-lpc313x/include -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -Os -marm -mabi=aapcs-linux -mno-thumb-interwork -funwind-tables -D__LINUX_ARM_ARCH__=5 -march=armv5te -mtune=arm9tdmi -msoft-float -Uarm -Wframe-larger-than=1024 -fno-stack-protector -fomit-frame-pointer -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fno-dwarf2-cfi-asm -fconserve-stack  -DMODULE -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(rtl8187_leds)"  -D"KBUILD_MODNAME=KBUILD_STR(rtl8187)"  -c -o drivers/net/wireless/rtl818x/rtl8187_leds.o drivers/net/wireless/rtl818x/rtl8187_leds.c

deps_drivers/net/wireless/rtl818x/rtl8187_leds.o := \
  drivers/net/wireless/rtl818x/rtl8187_leds.c \
    $(wildcard include/config/rtl8187/leds.h) \
    $(wildcard include/config/rtl8187/led.h) \

drivers/net/wireless/rtl818x/rtl8187_leds.o: $(deps_drivers/net/wireless/rtl818x/rtl8187_leds.o)

$(deps_drivers/net/wireless/rtl818x/rtl8187_leds.o):
