cmd_firmware/rtlwifi/rtl8712u.bin.gen.o := arm-linux-gnueabi-gcc -Wp,-MD,firmware/rtlwifi/.rtl8712u.bin.gen.o.d  -nostdinc -isystem /opt/eldk-5.2.1/armv5te/sysroots/i686-eldk-linux/usr/lib/armv5te-linux-gnueabi/gcc/arm-linux-gnueabi/4.6.4/include -I/home/armin/Masterarbeit/gnublin/linux-2.6.33-lpc313x-fb/arch/arm/include -Iinclude  -include include/generated/autoconf.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-lpc313x/include -D__ASSEMBLY__ -mabi=aapcs-linux -mno-thumb-interwork -funwind-tables  -D__LINUX_ARM_ARCH__=5 -march=armv5te -mtune=arm9tdmi -include asm/unified.h -msoft-float       -c -o firmware/rtlwifi/rtl8712u.bin.gen.o firmware/rtlwifi/rtl8712u.bin.gen.S

deps_firmware/rtlwifi/rtl8712u.bin.gen.o := \
  firmware/rtlwifi/rtl8712u.bin.gen.S \
  /home/armin/Masterarbeit/gnublin/linux-2.6.33-lpc313x-fb/arch/arm/include/asm/unified.h \
    $(wildcard include/config/arm/asm/unified.h) \
    $(wildcard include/config/thumb2/kernel.h) \

firmware/rtlwifi/rtl8712u.bin.gen.o: $(deps_firmware/rtlwifi/rtl8712u.bin.gen.o)

$(deps_firmware/rtlwifi/rtl8712u.bin.gen.o):
