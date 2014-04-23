cmd_drivers/net/wireless/rtl818x/rtl8187/leds.o := arm-linux-gnueabi-gcc -Wp,-MD,drivers/net/wireless/rtl818x/rtl8187/.leds.o.d  -nostdinc -isystem /opt/eldk-5.2.1/armv5te/sysroots/i686-eldk-linux/usr/lib/armv5te-linux-gnueabi/gcc/arm-linux-gnueabi/4.6.4/include -I/home/armin/Masterarbeit/gnublin/gnublin-lpc3131-3.3.0-master/linux-3.3.0-lpc313x/arch/arm/include -Iarch/arm/include/generated -Iinclude  -include /home/armin/Masterarbeit/gnublin/gnublin-lpc3131-3.3.0-master/linux-3.3.0-lpc313x/include/linux/kconfig.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-lpc31xx/include -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -Os -marm -fno-dwarf2-cfi-asm -mabi=aapcs-linux -mno-thumb-interwork -funwind-tables -D__LINUX_ARM_ARCH__=5 -march=armv5te -mtune=arm9tdmi -msoft-float -Uarm -Wframe-larger-than=1024 -fno-stack-protector -Wno-unused-but-set-variable -fomit-frame-pointer -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack -DCC_HAVE_ASM_GOTO -Idrivers/net/wireless/rtl818x  -DMODULE  -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(leds)"  -D"KBUILD_MODNAME=KBUILD_STR(rtl8187)" -c -o drivers/net/wireless/rtl818x/rtl8187/leds.o drivers/net/wireless/rtl818x/rtl8187/leds.c

source_drivers/net/wireless/rtl818x/rtl8187/leds.o := drivers/net/wireless/rtl818x/rtl8187/leds.c

deps_drivers/net/wireless/rtl818x/rtl8187/leds.o := \
    $(wildcard include/config/rtl8187/leds.h) \

drivers/net/wireless/rtl818x/rtl8187/leds.o: $(deps_drivers/net/wireless/rtl818x/rtl8187/leds.o)

$(deps_drivers/net/wireless/rtl818x/rtl8187/leds.o):
