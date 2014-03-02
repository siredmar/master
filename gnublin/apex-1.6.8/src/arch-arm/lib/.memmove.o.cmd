cmd_src/arch-arm/lib/memmove.o := arm-linux-gnueabi-gcc -Wp,-MD,src/arch-arm/lib/.memmove.o.d  -nostdinc -isystem /opt/eldk-5.2.1/armv5te/sysroots/i686-eldk-linux/usr/lib/armv5te-linux-gnueabi/gcc/arm-linux-gnueabi/4.6.4/include -D__KERNEL__ -Iinclude  -include include/linux/autoconf.h -D__ASSEMBLY__ -gdwarf-2     -c -o src/arch-arm/lib/memmove.o src/arch-arm/lib/memmove.S

deps_src/arch-arm/lib/memmove.o := \
  src/arch-arm/lib/memmove.S \
  include/linux/linkage.h \
  include/asm/linkage.h \
  include/asm/assembler.h \
    $(wildcard include/config/cpu/feroceon.h) \
  include/asm/ptrace.h \
    $(wildcard include/config/arm/thumb.h) \
    $(wildcard include/config/smp.h) \
  include/asm/hwcap.h \

src/arch-arm/lib/memmove.o: $(deps_src/arch-arm/lib/memmove.o)

$(deps_src/arch-arm/lib/memmove.o):
