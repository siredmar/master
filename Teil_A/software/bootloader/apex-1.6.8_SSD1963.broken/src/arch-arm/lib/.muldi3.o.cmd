cmd_src/arch-arm/lib/muldi3.o := arm-linux-gnueabi-gcc -Wp,-MD,src/arch-arm/lib/.muldi3.o.d  -nostdinc -isystem /opt/eldk-5.2.1/armv5te/sysroots/i686-eldk-linux/usr/lib/armv5te-linux-gnueabi/gcc/arm-linux-gnueabi/4.6.4/include -D__KERNEL__ -Iinclude  -include include/linux/autoconf.h -D__ASSEMBLY__ -gdwarf-2     -c -o src/arch-arm/lib/muldi3.o src/arch-arm/lib/muldi3.S

deps_src/arch-arm/lib/muldi3.o := \
  src/arch-arm/lib/muldi3.S \
  include/linux/linkage.h \
  include/asm/linkage.h \

src/arch-arm/lib/muldi3.o: $(deps_src/arch-arm/lib/muldi3.o)

$(deps_src/arch-arm/lib/muldi3.o):
